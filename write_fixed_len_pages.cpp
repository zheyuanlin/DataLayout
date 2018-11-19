#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/timeb.h>
#include "library.h"

int main(int argc, const char *argv[]) {
	if (argc != 4) {
		std::cout << "Please use $ write_fixed_len_pages <csv_file> <page_file> <page_size>";
		return 1;
	}

	// outputs
	int number_of_records = 0;
	int number_of_pages = 0;

	std::string csv_filename = argv[1];
	std::string page_filename = argv[2];
	int page_size = std::stoi(argv[3]);
	std::ofstream page_file;
	page_file.open(page_filename, std::ios::out | std::ios::binary);

	std::ifstream csv_file(csv_filename);
	bool make_new_page = true;
	Page page;
	std::string line;

	// time
	struct timeb t;
	ftime(&t);
	long now_in_ms = t.time * 1000 + t.millitm;

	while (std::getline(csv_file, line)) {
		std::string cell;
		std::stringstream linestringstream(line);

		Record record;
		while (std::getline(linestringstream, cell, ',')) {
			char *temp = (char *) malloc(11);
			std::strncpy(temp, cell.c_str(), 11);
			record.push_back(temp);
		}

		if (make_new_page == true) {
			init_fixed_len_page(&page, page_size, fixed_len_sizeof(&record));
			number_of_pages += 1;
		}

		// add number of records
		number_of_records += 1;


		// check if space is full
		if (add_fixed_len_page(&page, &record) == -1) {
			make_new_page = true;
		}
		else {
			make_new_page = false;
		}

		//std::cout<<add_fixed_len_page(&page, &record);

		if (make_new_page == true) {
			page_file.write((const char *) page.data, page.page_size);
			init_fixed_len_page(&page, page_size, fixed_len_sizeof(&record));
			add_fixed_len_page(&page, &record);

			make_new_page = false;
			number_of_pages += 1;
		}
	}

	if (make_new_page == false) {
		page_file.write((const char*) page.data, page.page_size);
	}

	// Close file and stop time
	page_file.close();
	ftime(&t);
	long end_time_in_ms = t.time * 1000 + t.millitm;

	std::cout << "NUMBER OF RECORDS: " << number_of_records << "\n";
    std::cout << "NUMBER OF PAGES: " << number_of_pages << "\n";
    std::cout << "TIME: " << end_time_in_ms - now_in_ms << " milliseconds\n";

    return 0;





}