#include <string.h>

#include "helper.h"
#include "consts.h"
#include "bitsarr.h"
#include "disk.h"
#include "datalist.h"


// get data link list node at blkno from disk
dlln dlln_load(int blkno)
{
    dlln node = {0, 0, 0, 0};
    int node_base = datalist_base + (blkno * d), blklen=mceil(d, 8);
    int datasize = 32;
    unsigned char dlln_block[blklen], buffer[BUFLEN];
    
    memset(dlln_block, 0, blklen);
    disk_rd(dlln_block, d, node_base);  // read one data link list block from disk

    // extracting information from bits
    memset(buffer, 0, BUFLEN);
    bits_copy(buffer, dlln_block, 0, ptrsize);
    node.pre = bytestonum(buffer);

    memset(buffer, 0, BUFLEN);
    bits_copy(buffer, dlln_block, ptrsize, DATASIZE);  // as data is integer
    node.data = bytestonum(buffer);

    memset(buffer, 0, BUFLEN);
    bits_copy(buffer, dlln_block, ptrsize + DATASIZE, ptrsize);
    node.next = bytestonum(buffer);

    return node;
}

// store dlln node to disk fiel at blkno wrt to data link list blocks
int dlln_dump(dlln node)
{
    int res = 0;
    int node_base =  datalist_base + (node.base * d), blklen= mceil(d, 8);
    unsigned char dlln_block[blklen], buffer[BUFLEN];
    memset(dlln_block, 0, blklen);

    // creating mlln block
    memset(buffer, 0, BUFLEN);
    numtobytes(buffer, node.pre);
    bits_put(dlln_block, 0, buffer, ptrsize);

    memset(buffer, 0, BUFLEN);
    numtobytes(buffer, node.data);
    bits_put(dlln_block, ptrsize, buffer, DATASIZE);

    memset(buffer, 0, BUFLEN);
    numtobytes(buffer, node.next);
    bits_put(dlln_block, ptrsize + DATASIZE, buffer, ptrsize);

    res += disk_wr(dlln_block, d, node_base);   // writing block to disk

    return res;
}
