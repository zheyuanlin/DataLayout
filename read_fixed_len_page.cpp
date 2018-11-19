#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sys/timeb.h>
#include "library.h"

int main (int argc, const char *argv[]) {
	if (argc < 3) {
		std::cout << "Wrong parameters: $ read_fixed_len_page <page_file> <page_size>";
		return 1;
	}

	std::string filename = argv[1];
	int page_size = std::stoi(argv[2]);

	struct timeb t;
	ftime(&t);
	long now_in_ms = t.time * 1000 + t.millitm;

	// Make page file
	std::ifstream page_file;
	page_file.open(page_filename, std::ios::in | std::ios::binary);

	// Record output as in handout
	int numRecords = 0;
	int numPages = 0;

	while (page_file.eof() == false) {
		Page curr_page;
		// Init curr_page using a slot size of 1000
		init_fixed_len_page(&curr_page, page_size, 1000);
		// Read page_file data into data field of curr_page
		page_file.read((char *)curr_page.data, page_size);
		
		for (int i = 0; i < fixed_len_page_capacity(&curr_page); i++) {
			Record record;
			read_fixed_len_page(&curr_page, i, &record);
			
			// // Output to test file
			// for (unsigned int j = 0; j < record.size(); j++) {
			// 	std::cout << record.at(j);
			// 	fputs(record.at(j), test_file);
			// 	if (j != record.size() - 1) {
			// 		std::cout << ",";
			// 		fputs(",", test_file);
			// 	}
			// }
			// fputs("\n", test_file);
			numRecords += 1;
		}
	}

	// Close file and stop time
	page_file.close();
	ftime(&t);
	long end_time_in_ms = t.time * 1000 + t.millitm;

	// Outputs for experiment data
	std::cout << "NUMBER OF RECORDS: " << numRecords << "\n";
    std::cout << "NUMBER OF PAGES: " << numPages << "\n";
    std::cout << "TIME: " << end_time_in_ms - now_in_ms << " milliseconds\n";

    return 0;
}