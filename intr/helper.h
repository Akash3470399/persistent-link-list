#ifndef HELPER_H
#define HELPER_H

long disk_creator(char *dname, long dsize);
int check_or_mkdir(char *dirname);
int mceil(int a, int b);
int get_filesize(char *filenm);
unsigned int bytestonum(unsigned char *bitsarr);
void numtobytes(unsigned char *bitarr, unsigned int n);
#endif
