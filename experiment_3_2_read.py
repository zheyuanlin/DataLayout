#!/usr/bin/env python3

import sys
import subprocess
import csv
import os
import shutil



def read_fixed_len_pages(page_file, page_size):
    result = subprocess.run(
        [os.path.join(os.path.dirname(os.path.realpath(__file__)), './read_fixed_len_pages'),
            page_file,
            str(page_size)], stdout=subprocess.PIPE)
    # parse the result to get the desired time
    return int(result.stdout.split()[-2])


def main():
    """
    Call ./read_fixed_len_pages to write to example_page_file with ten different block sizes,
    each in a range of 10 to 100MB.
    NOTE: piazza says use 500mb???
    https://piazza.com/class/isubcms1kpl3xq?cid=92
    I would assume we can only do this if we make tuples.csv larger.

    """
    if len(sys.argv) != 3:
        print('wrong number of arguments')
        sys.exit(1)

    else:

        page_file = sys.argv[1]
        page_size = sys.argv[2]

        page_sizes = [128, 512, 1024, 4096, 1024 * 8, 1024 * 64, 1024 * 256, 1024 ** 2, 2 * 1024 ** 2]

        if os.path.exists('./out'):
            shutil.rmtree('./out')
        os.mkdir('./out')

        writer = csv.DictWriter(sys.stdout, fieldnames=('page_size', 'milliseconds_elapsed'), dialect='unix')
        writer.writeheader()
        
        for p_size in page_sizes:
            for i in range(50):
                ms_elapsed = read_fixed_len_pages(page_file, p_size)
                csvwriter.writerow({
                    'page_size': p_size,
                    'time taken (ms)': ms_elapsed,
                })

if __name__ == '__main__':
    main()