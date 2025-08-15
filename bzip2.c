/*  A program to create a BZip2 compressed file with maximum compression
    Copyright (C) 2025 Anonymous1212144

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned long add_crc(unsigned long a, const unsigned long b, unsigned long long n)
{
    // Combine the CRC of two blocks
    n &= 31;
    a = (a << n) | (a >> (32 - n));
    return a ^ b;
}

unsigned long calc_crc(unsigned long b, unsigned long long n)
{
    // Calculate the CRC of the entire data
    unsigned long c = b;
    unsigned long long i = 1;
    while (1)
    {
        if (n & 1)
            b = add_crc(b, c, i);
        n >>= 1;
        if (!n)
            break;
        c = add_crc(c, c, i);
        i <<= 1;
    }
    return b;
}

void print_msg(size_t n, long m, long c, char pre[], char suf[])
{
    // Print a message that may contain large numbers
    long long a = (n / 1000000000) * m;
    long long b = (n % 1000000000) * m + c;
    a += b / 1000000000;
    b %= 1000000000;
    if (a)
        printf("%s%lld%09lld%s", pre, a, b, suf);
    else
        printf("%s%lld%s", pre, b, suf);
}

int main()
{
    fputs("How many chunks do you want? ", stdout);
    char l[25];
    size_t n = strtoull(fgets(l, 24, stdin), NULL, 0);
    if (!n)
    {
        fputs("Nothing to do.", stdout);
        return 0;
    }
    print_msg(n, 26, 14, "Compressed output will be: ", " bytes\n");
    print_msg(n, 45899235, 0, "Extracted output will be: ", " bytes\n");

    const char header[4] = "BZh9";
    const char body[26] = "\x31\x41\x59\x26\x53\x59\x5F\xAC\x96\x03\x00\x00\x00\x00\x00\x80\x08\x20\x00\x30\x8C\x26\x92\x94\x21\x13";
    char footer[10] = "\x17\x72\x45\x38\x50\x90\0\0\0\0";
    unsigned long crc = calc_crc(1605146115, n - 1);
    for (int i = 9; i > 5; i--)
    {
        footer[i] = crc & 255;
        crc >>= 8;
    }

    puts("Setting up memory...");
    size_t maximum = 1 << 30;
    size_t block = maximum / 26;
    if (block > n)
        block = n;
    size_t block2 = block * 26;
    char *buffer = malloc(block2);
    if (!buffer)
    {
        perror("Failed to allocate memory");
        return 1;
    }
    if (block)
    {
        memcpy(buffer, body, 26);
        for (size_t i = 26;; i <<= 1)
        {
            if (i > block2 >> 1)
            {
                memcpy(buffer + i, buffer, block2 - i);
                break;
            }
            memcpy(buffer + i, buffer, i);
        }
    }

    puts("Writing to \"output_archive.bz2\"...");
    FILE *file = fopen("output_archive.bz2", "wb");
    if (!file)
    {
        perror("Failed to open file");
        return 1;
    }
    fwrite(header, 1, 4, file);
    for (size_t i = 0;; i += block)
    {
        if (i > n - block)
        {
            fwrite(buffer, 1, (n % block) * 26, file);
            break;
        }
        fwrite(buffer, 1, block2, file);
    }
    fwrite(footer, 1, 10, file);
    fclose(file);
    fputs("Done.", stdout);
}