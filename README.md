# OptimalBZip2
A program to create a BZip2 compressed file with maximum compression

# Usage
This program takes a user input and creates `output_archive.bz2` with the number of blocks specified. Each block adds 26 bytes to the compressed file, which corresponds to 45899235 bytes to the extracted file.

# Background
BZip2 compression algorithm is special in that it is the only mainstream compression I can find that uses [RLE](https://en.wikipedia.org/wiki/Run-length_encoding) for long stream of data, and RLE is special in that it can reduce the data to Θ(log n) size in the best-case scenraio, which is extremely good. Unfortunately, BZip2 have a measly 900 kb limit internally somewhere, but even then it can achieve a best-case compression of around 1765355:1. BZip2 handles arbitrarily large files by having separate blocks, each with the 900 kb limit, so the absolute optimal file would be an infinite stack of optimal blocks (because the header and footer). This program instead creates the optimal file that can be achieved with the specified (finite) number of blocks.
