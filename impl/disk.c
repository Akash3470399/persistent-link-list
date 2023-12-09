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



unsigned int nullblk;

// base of lists (bit position in file)
long datalist_base, mainlist_base;  

// head of free nodes lists
long datafreelist_head, mainfreelist_head, mainlist_head;

// pointer size of pointer used in file
int ptrsize;                                            

// x and y are last valid block in bitmaps
long x, y;                                              

// store the information related to the bitmap
bitmap_summary main_bitmap, data_bitmap;

/*
     _______________________________________________________________________________________________
    |   |          |        |                                       |        |      |           |   |
    |   | data     | data   |data nodes ...          ... main nodes | main   | main | main      |   |
    | x | freelist | list   |                                       | list   | list | free list | y |
    |   | base     | bitmap |                                       | bitmap | head | base      |   |
    |___|__________|________|_______________________________________|________|______|___________|___|

                                        Disk structure
*/


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

    data_bitmap.size = t/d;                             // total data node possible
    main_bitmap.size = t/m;                             // total main node possible

    data_bitmap.base = (2*ptrsize);
    main_bitmap.base = t- ((3 * ptrsize) + main_bitmap.size);
    
    nullblk = main_bitmap.size -1;
    
    disk_rd((unsigned char*)&datafreelist_head, ptrsize, ptrsize);
    disk_rd((unsigned char*)&mainfreelist_head, ptrsize, (t-(ptrsize * 2)));
    disk_rd((unsigned char*)&mainlist_head, ptrsize, (t-(ptrsize * 3)));

    disk_rd((unsigned char*)&x, ptrsize, 0);
    disk_rd((unsigned char*)&y, ptrsize, t-ptrsize);
    
    datalist_base = (2*ptrsize) + data_bitmap.size;     
    mainlist_base = t - ((3*ptrsize) + main_bitmap.size) - m; // as reverse   

    fprintf(stdout, "t %ld , ptr size %d, nullblk %d, data ll bitmap size %d, main ll bitmap size %d\n", t, ptrsize, nullblk, data_bitmap.size, main_bitmap.size);
    fprintf(stdout, "data list base : %ld, mainlist_base %ld\n", datalist_base, mainlist_base);
    fprintf(stdout, "data bitmap bast %d, main bitmap base %ld\n", ptrsize, mainlist_base+m);
    
    return 1;
}

// initilize the disk
int disk_init(char *diskname)
{
    int preoccupied_space, preoccupied_data_nodes, preoccupied_main_nodes, ones;

    disk_config(diskname);
    
    datafreelist_head = nullblk, mainfreelist_head = nullblk, mainlist_head = nullblk;
    disk_wr((unsigned char*)&datafreelist_head, ptrsize, ptrsize);
    disk_wr((unsigned char*)&mainfreelist_head, ptrsize, (t-(ptrsize * 2)));
    disk_wr((unsigned char*)&mainlist_head, ptrsize, t-(ptrsize * 3));

    preoccupied_space = (ptrsize * 5) + data_bitmap.size + main_bitmap.size;
    preoccupied_data_nodes = mceil(preoccupied_space, d);
    preoccupied_main_nodes = mceil(preoccupied_space, m);

    x = data_bitmap.size - preoccupied_data_nodes;   
    y = main_bitmap.size - preoccupied_main_nodes;

    disk_wr((unsigned char*)&x, ptrsize, 0);
    disk_wr((unsigned char*)&y, ptrsize, t-ptrsize);
    
    init_bitmaps();
}

void disk_update_config()
{

    disk_wr((unsigned char*)&x, ptrsize, 0);
    disk_wr((unsigned char*)&y, ptrsize, t-ptrsize);
    
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
