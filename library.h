#include <vector>

// Make functions/structs as specified in the handout
typedef const char* V;
typedef std::vector<V> Record;

const int SLOTSIZE = 5000;

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf);

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record);

typedef struct {
    int dir_size; // bytes
    int next_directory;
    int page_size;
    int slot_size;
    void *data;
} Page;

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r);
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r);

// PageId and Record ID
typedef int PageID;
 
typedef struct {
    int page_id;
    int slot;
} RecordID;



// Heapfile struct
typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;

// Heapfile functions:

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile);

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page);

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid);

class RecordIterator {
    public:
    RecordIterator(Heapfile *heapfile);
    Record next();
    bool hasNext();
};