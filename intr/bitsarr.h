#ifndef BITSARR_H
#define BITSARR_H

unsigned char bits_get(unsigned char *arr, int start, int len);
int bits_put(unsigned char *bitsarr, int start, unsigned char *dataarr, int bitscnt);
void bits_copy(unsigned char *to, unsigned char *from, int start, int len);
#endif
