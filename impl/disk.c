#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "consts.h"
#include "disk.h"
#include "bitsarr.h"
#include "bitmap.h"
#include "helper.h"

FILE *diskfp;   // virt disk fp
long t ;        // disk size in bits
int m ;         // main link list node size
int d ;         // data link list node size



unsigned int data_bitmap_size, main_bitmap_size, sblk;
long data_list_base, main_list_base;                    // base of lists (bit position in file)
int ptrsize;                                            // pointer size of pointer used in file
long x, y;                                              // x and y are last valid block in bitmaps
bitmap *main_bitmap = NULL, *data_bitmap = NULL;     


// open virt disk & configures disk setting
int disk_config(char *diskname)
{
    
    long filesize;
    diskfp = fopen(diskname, "rb+");
    if(diskfp == NULL)
    {
        printf("Please check you have provied right disk name.\n");
        return 0;
    }
    
    filesize = get_filesize(diskname);  
        
    ptrsize = _calc_ptrsize(filesize);

    t = filesize * 8;
    d = ptrsize + DATASIZE + ptrsize;                   // as data node have pre, next pointers & data
    m = ptrsize + ptrsize + ptrsize;                    // as main node have pre, data node & next pointers

    data_bitmap_size = t/d;                             // total data node possible
    main_bitmap_size = t/m;                             // total main node possible

    data_list_base = ptrsize + data_bitmap_size;
    main_list_base = t - (ptrsize + main_bitmap_size) - m; // as reverse   

    data_bitmap = bitmap_init(data_bitmap_size, 0);
    main_bitmap = bitmap_init(main_bitmap_size, 0);

    sblk = data_bitmap_size -1;

    disk_rd((unsigned char*)&x, ptrsize, 0);
    disk_rd((unsigned char*)&y, ptrsize, t-ptrsize);

    disk_rd(data_bitmap->arr, data_bitmap_size, ptrsize);
    disk_rd(main_bitmap->arr, main_bitmap_size, main_list_base + m);

    
    fprintf(stdout, "t %ld , ptr size %d, data ll bitmap size %d, main ll bitmap size %d\n", t, ptrsize, data_bitmap_size, main_bitmap_size);
    fprintf(stdout, "data list base : %ld, main_list_base %ld\n", data_list_base, main_list_base);
    fprintf(stdout, "data bitmap bast %d, main bitmap base %ld\n", ptrsize, main_list_base+m);
    
    return 1;
}

// initilize the disk
int disk_init(char *diskname)
{
    int preoccupied_space, preoccupied_data_nodes, preoccupied_main_nodes, ones;
    bitmap *bm;

    disk_config(diskname);
    preoccupied_space = (ptrsize * 2) + data_bitmap_size + main_bitmap_size;

    preoccupied_data_nodes = mceil(preoccupied_space, d);
    preoccupied_main_nodes = mceil(preoccupied_space, m);

    x = data_bitmap_size - preoccupied_data_nodes;   
    y = main_bitmap_size - preoccupied_main_nodes;

    disk_wr((unsigned char*)&x, ptrsize, 0);
    disk_wr((unsigned char*)&y, ptrsize, t-ptrsize);

    bm = bitmap_init(preoccupied_data_nodes, 0xFF);
    disk_wr(bm->arr, preoccupied_data_nodes, data_list_base-preoccupied_data_nodes);
    destroy_bitmap(bm);

    bm = bitmap_init(preoccupied_main_nodes, 0xFF);
    disk_wr(bm->arr, preoccupied_main_nodes, t - ptrsize - preoccupied_main_nodes);
    destroy_bitmap(bm);
}

void disk_update_config()
{

    disk_wr((unsigned char*)&x, ptrsize, 0);
    disk_wr((unsigned char*)&y, ptrsize, t-ptrsize);
    
    disk_wr(main_bitmap->arr, main_bitmap->len, main_list_base + m);
    disk_wr(data_bitmap->arr, data_bitmap->len, ptrsize);

}


    // ********* datalen config *********
    // e.g filepos = 7 & datalen = 3
    // as to store data we only 3 bits & 3 bits can be store in one byte
    // but as in startbyte byte we only have 1 bit available so we need fetch
    // one byte extra to store remaining 2 bytes
    
    // startbyte : byte number in file where filepos numbered bit is present
    // startpos : position of filepos bit in startbyte byte

// write datalen number of bits from the bitsarr into the disk file
// starting from filepos bit position in the file
// 8 represent the byte size
int disk_wr(unsigned char *bitsarr, int datalen, int filepos)
{

    int buflen = 1 + mceil(datalen-(8- filepos%8), 8);

    unsigned char buffer[buflen+1];
    int startbyte = filepos/8, startpos = filepos % 8;

    memset(buffer, 0, buflen+1);
    fseek(diskfp, startbyte, SEEK_SET);
    fread(buffer, 1, buflen, diskfp);                   // it will move fp forward
    
    bits_put(buffer, startpos, bitsarr, datalen);

    fseek(diskfp, buflen * (-1), SEEK_CUR);             // get back to buffers position
    fwrite(buffer, 1, buflen, diskfp);
    fflush(diskfp);
}


// read datalen number of bits into bitsarr from the file starting from filepos bit number
int disk_rd(unsigned char *bitsarr, int datalen, int filepos)
{
    int buflen = 1 + mceil(datalen-(8- filepos%8), 8);
    unsigned char buffer[buflen+1];
    int startbyte = filepos/8, startpos = filepos % 8, bitscnt, rdbits = 0;

    memset(buffer, 0, buflen);
    fseek(diskfp, startbyte, SEEK_SET);
    fread(buffer, 1, buflen, diskfp);

    int i = 0;
    while(rdbits < datalen)
    {
        bitscnt = ((datalen - rdbits) < 8)? (datalen-rdbits): 8;
        bitsarr[i++] = bits_get(buffer, startpos+rdbits ,bitscnt);
        rdbits += bitscnt;
    }
    return rdbits;
}




/*
 *      Here t : file size in bits
 *      k : is pointer size
 *            
 *      Equation to find count of main node possible in file with size t bits
 *               t
 *      2^k =  -----       (2^k main node blocks)    
 *               3k 
 *
 *       i.e (2^k) * (k) =   t
 *                          ---
 *                           3
 *
 *       now let k1 = 2^k, t1 = t/3
 *                      
 */
// returns 
int _calc_ptrsize(long filesize)
{
    int k;
    long p = 1, t1;
   
    t1 = (filesize * 8) / 3;    // converted file size into bits & dividing it by 3
        
    k = log(t1) / log(2);       // rough value of k 
    
    p = (long)1 << k;
    // reducing k till equation holds true
    while(p * k >= t1)
    {
        k -= 1;
        p = (long)1 << k;
    }

    return (k + 1);
}
