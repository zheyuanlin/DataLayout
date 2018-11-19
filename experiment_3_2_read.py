#!/usr/bin/env python3

import sys
import subprocess
import csv
import os
import shutil



def read_fixed_len_pages(page_file, page_size):
    result = subprocess.run(
        [os.path.join(os.path.dirname(os.path.realpath(__file__)), './read_fixed_len_page'),
            page_file,
            str(page_size)], stdout=subprocess.PIPE)
    
    return int(result.stdout.split()[-2])


def main():
    
    if len(sys.argv) != 2:
        print('only 1 argument, the page file')
        sys.exit(1)

    else:

        page_file = sys.argv[1]

        page_sizes = [128, 512, 1024, 4096, 1024 * 8, 1024 * 64, 1024 * 256, 1024 ** 2, 2 * 1024 ** 2]

        # write csv
        writer = csv.DictWriter(sys.stdout, fieldnames=('page_size', 'time (ms)'))
        writer.writeheader()

        for p_size in page_sizes:
            for i in range(5):
                time_taken = read_fixed_len_pages(page_file, p_size)
                writer.writerow({
                    'page_size': p_size,
                    'time (ms)': time_taken,
                })

if __name__ == '__main__':
    main()