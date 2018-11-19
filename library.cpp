#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <assert.h>
#include "library.h"


/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {
	unsigned int i = 0;
	while (i < record->size()) {
		int l = std::strlen(record->at(i));
		std::memcpy((char *)buf + (int)(i * l), record->at(i), l);

		i += 1;
	}
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {
	// allocate some bytes if necessary, there are 100 attributes
	record->reserve(100);

	for (int i = 0; i < 100; i++) {
		// each attribute is 10 bytes each
		char *read = (char *) malloc(11);
		memcpy(read, (char *)buf + (10 * i), 10);
		// terminate string
		read[10] = '\0';

		record->push_back(read);
	}
}

/**
 * lambda function to add length
 */
int add_length(int sum, const char *nextString) {
	return std::strlen(nextString) + sum;
}
/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
	return std::accumulate(record->begin(), record->end(), 0, add_length);
}


/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
	int i = 0;
	page->dir_size = std::ceil((page_size / slot_size) / (double) 8);
	page->page_size = page_size;
	page->slot_size = slot_size;
	page->data = malloc(page_size);

	// Initialize empty dir
	uint8_t *dir = page->page_size - page->dir_size + ((uint8_t *) page->data);
    int need_setting = (8 * page->dir_size) - fixed_len_page_capacity(page);
    *dir = 0xFF << (8 - need_setting);
    while (i < page->dir_size) {
    	if (i > 0) {
    		uint8_t *byte = ((uint8_t *) page->data) + page->page_size - i;
        	*byte = 0;
    	}
    	i += 1;
    }
}
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
    return (page->page_size / page->slot_size) - std::ceil(page->dir_size / page->slot_size);
}
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page) {
	int ctr = 0;
	uint8_t *dir = page->page_size - page->dir_size + ((uint8_t *) page->data);
	int i = 0;
	int j = 0;
	while  (i < page->dir_size) {
		while (j < 8) {
			if (((dir[i] >> j) & 1) == 0) {
				ctr += 1;
			}
			j += 1;
		}
		i += 1;
		j = 0;
	}
	return ctr;
}

/**
 * Find first free space, same as fixed_len_page_freeslots but returns early.
 */
int find_first_freeslot(Page *page) {
	uint8_t *dir = page->page_size - page->dir_size + ((uint8_t *) page->data);
	int i = 0;
	int j = 0;
	while  (i < page->dir_size) {
		while (j < 8) {
			if (((dir[i] >> j) & 1) == 0) {
				return (8 * i) + j;
			}
			j += 1;
		}
		i += 1;
		j = 0;
	}
	return -1;
}
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r) {
	if (fixed_len_page_freeslots(page) != 0) {
		int free_space = find_first_freeslot(page);
		uint8_t *dirent = ((uint8_t *) page->data) + page->page_size - (free_space / 8) - 1;
    	*dirent = *dirent | (1 << (free_space % 8));
    	write_fixed_len_page(page, free_space, r);
    	return free_space;
	}
	else {
		// No free pages available
		return -1;
	}
}
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r) {
	uint8_t *curr_slot = page->slot_size * slot + ((uint8_t *) page->data);
	fixed_len_write(r, (void *) curr_slot);
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r) {
	char *curr_slot = (page->slot_size * slot) + ((char *) page->data);
	fixed_len_read(curr_slot, page->slot_size, r);
}

 void add_empty_dir(Heapfile *heapfile) {
 	Page dir_page;
 	init_fixed_len_page(&dir_page, heapfile->page_size, SLOTSIZE);
 	int write_check = fwrite((const char *) dir_page.data, heapfile->page_size, 1, heapfile->file_ptr);
 	if (write_check <= 0) {
 		perror("fwrite error add_empty_dir\n");
 		// return 1 in main
 		exit(1);
 	}
 }

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file) {
	heapfile->page_size = page_size;
	heapfile->file_ptr = file;
	add_empty_dir(heapfile);
	int fseek_check = fseek(heapfile->file_ptr, 0, SEEK_SET);
	if (fseek_check) {
		perror("fseek error init_heapfile");
		// return 1 in main
		exit(1);
	}
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile) {
	PageID page_id = 0;
	int directory_with_free_space = -1;

	Page dir_page;
	while (!feof(heapfile->file_ptr) && directory_with_free_space == -1) {
		init_fixed_len_page(&dir_page, heapfile->page_size, SLOTSIZE);
		
		int fread_check = fread(dir_page.data, heapfile->page_size, 1, heapfile->file_ptr); 
		if (fread_check <= 0 && ferror(heapfile->file_ptr) != 0) {
            perror("fread in alloc_page\n");
            exit(1);
        }

        directory_with_free_space = find_first_freeslot(&dir_page);
        // if no space, go to the next dir
        if (directory_with_free_space == -1) {
            page_id += fixed_len_page_capacity(&dir_page);
            int fseek_check = fseek(heapfile->file_ptr, heapfile->page_size * fixed_len_page_capacity(&directory_page), SEEK_CUR);
            if (fseek_check) {
                perror("alloc page fseek");
                exit(1);
            }
        }
    }

    // no dir available
    if (directory_with_free_space == -1) {
        add_empty_dir(heapfile);
    }

    Page data_page;
    init_fixed_len_page(&data_page, heapfile->page_size, SLOTSIZE);

    // record for dir_page
    Record record;
    std::string free_space = std::to_string(fixed_len_page_capacity(&dir_page));
    std::string offset = std::to_string(heapfile->page_size * directory_with_free_space);
    record.push_back(free_space.c_str());
    record.push_back(offset.c_str());

    // Add record to directory_page for the new data_page we are allocating
    int wrote_slot = add_fixed_len_page(&dir_page, &record);
    if (wrote_slot != directory_with_free_space) {
    	perror("alloc_page: slot we wrote to isnt the same as dir with free space\n");
    	exit(1);
    }

    int fseek_check = fseek(heapfile->file_ptr, (heapfile->page_size * directory_with_free_space), SEEK_CUR);
    int fwrite_check = fwrite((const char *) data_page.data, heapfile->page_size, 1, heapfile->file_ptr);
    if (fseek_check) {
        perror("alloc_page: fseek after wrote = data\n");
        exit(1);
    }
    if (fwrite_check < 1) {
        perror("alloc_page: fwrite after wrote = data");
        exit(1);
    }

   	// save page id
    page_id += wrote_slot;
    return page_id;
}

int get_heap_position(Heapfile *heapfile, PageID pid, int data_per_dir){
    return ((((pid / data_per_dir) + 1) + pid) * heapfile->page_size);
}

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page) {
	Page dir_page;
	init_fixed_len_page(&dir_page, heapfile->page_size, SLOTSIZE);
    int fread_check = fread(dir_page.data, heapfile->page_size, 1, heapfile->file_ptr);
    if (fread_check <= 0) {
    	perror("read_page: fread error\n");
    	exit(1);
    }

    int data_per_dir = fixed_len_page_capacity(&dir_page);
    int heap_position = get_heap_position(heapfile, pid, data_per_dir);

    init_fixed_len_page(page, heapfile->page_size, SLOTSIZE);
    int fseek_check = fseek(heapfile->file_ptr, heap_position, SEEK_SET);
    if (fseek_check) {
        perror("read_page: fseek error");
        exit(1);
    }

    fread_check = fread(dir_page.data, heapfile->page_size, 1, heapfile->file_ptr);
    if (fread_check <= 0) {
    	perror("read_page: final fread error\n");
    	exit(1);
    }
    
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid) {
	Page dir_page;
    init_fixed_len_page(&dir_page, heapfile->page_size, SLOTSIZE);
    int fread_check = fread(dir_page.data, heapfile->page_size, 1, heapfile->file_ptr);
    if (fread_check <= 0) {
    	perror("write_page fread\n");
    	exit(1);
    }

    int data_per_dir = fixed_len_page_capacity(&dir_page);
    int heap_position = get_heap_position(heapfile, pid, data_per_dir);

    int fseek_check = fseek(heapfile->file_ptr, heap_position, SEEK_SET);
    if (fseek_check) {
        perror("write_page fseek\n");
        exit(1);
    }
    
    int fwrite_check = fwrite(page->data, heapfile->page_size, 1, heapfile->file_ptr);
    if (fwrite_check <= 0) {
    	perror("write_page fwrite\n");
    	exit(1);
    }
}
