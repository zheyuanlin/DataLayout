#!/usr/bin/env python3

import csv
import os
import shutil
import subprocess
import sys


def write_fixed_len_pages(csv_file, page_file, page_size):
    
    result = subprocess.run(
        [os.path.join(os.path.dirname(os.path.realpath(__file__)), './write_fixed_len_pages'),
            csv_file,
            page_file,
            str(page_size),
        ], stdout=subprocess.PIPE)
    
    return int(result.stdout.split()[-2])


def main():
    
    if len(sys.argv) != 3:
        print('only 2 arguments please, the csv file and the page file')
        sys.exit(1)

    else:
        csv_file = sys.argv[1]
        page_file = sys.argv[2]

        page_sizes = [128, 512, 1024, 4096, 1024 * 8, 1024 * 64, 1024 * 256, 1024 ** 2, 2 * 1024 ** 2]

        writer = csv.DictWriter(sys.stdout, fieldnames=('page_size', 'time_taken(ms)'))
        writer.writeheader()

        for page_size in page_sizes:
            for i in range(5):
                time_taken = write_fixed_len_pages(csv_file, page_file, page_size)
                writer.writerow({'page_size': page_size, 'time_taken(ms)': time_taken})

if __name__ == '__main__':
    main()