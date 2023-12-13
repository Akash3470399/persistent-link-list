#include <string.h>

#include "helper.h"
#include "consts.h"
#include "bitsarr.h"
#include "disk.h"
#include "mainlist.h"
#include "datalist.h"


// return length of main link list
int getnlists()
{
    return mainlist_length;
}

// add an empty data list at a position in list
// valid position is 1 <= position <= list length + 1
int addlist(int position)
{
    mlln newnode, tmpnode;
    int ret = 0;
    
    if(position > 0 && position <= (getnlists()+1))
    {
        newnode = new_mlln(nullblk, nullblk, nullblk);
        if(position == 1)
        {
            if(mainlist_head != nullblk)            // main list is not empty
            {
                tmpnode = mlln_load(mainlist_head);
                tmpnode.pre = newnode.base;
                newnode.next = tmpnode.base;
                mlln_dump(tmpnode);
            }
          
            update_mainlist_head(newnode.base);
        }
        else
        {
            // get (position -1)th node so after we can add new node
            tmpnode = mlln_load(mainlist_head);
            position -= 1;                      
            for(int i = 1; i < position; i++)
               tmpnode = mlln_load(newnode.next);
           
            // making connection of new node
            newnode.pre = tmpnode.base;
            newnode.next = tmpnode.next;
    
            // updating previous node
            tmpnode.next = newnode.base;
            mlln_dump(tmpnode);
            
            if(newnode.next != nullblk)   // if adding in between to nodes
            {
                // updating next node
                tmpnode = mlln_load(newnode.next);
                tmpnode.pre = newnode.base;
                mlln_dump(tmpnode);
            }
        }    

        mlln_dump(newnode);
        mainlist_length += 1;
        ret = 1;
    }
    
    return ret;
}

// return nth main link list node
mlln getnth(int n)
{
    mlln tmp;
    
    if(n > 0 && n <= getnlists())
    {
        tmp = mlln_load(mainlist_head);
        for(int i = 1; i < n; i++)
            tmp = mlln_load(tmp.next);
    }

    return tmp;
}

mlln new_mlln(int pre, int data, int next)
{
    mlln node ;
    if(mainfreelist_head == nullblk)
    {
        node.base = main_bitmap.nextfreeblk++;
        node.pre = pre, node.data = data, node.next = next;
        set(main_bitmap, node.base);
    }
    else
    {
        
    }

    return node;
}

// when new data block is added that space is not available to main link list node
void decr_mlln_availability()
{
    main_bitmap.lastptr -= sizefactor;
}

// when data block deleted that space is available to main link list to
void incr_mlln_availability()
{
    main_bitmap.lastptr += sizefactor;
}

// get main link list node at blkno from the disk
mlln mlln_load(int blkno)
{
    mlln node = {0, 0, 0, 0};
    int node_base = mainlist_base - (blkno * m), blklen = mceil(m, 8);
    unsigned char mlln_block[blklen], buffer[BUFLEN];

    node.base = blkno;

    memset(mlln_block, 0, blklen);
    disk_rd(mlln_block, m, node_base);  // read one main link list block

    // extracting information from bits
    memset(buffer, 0, BUFLEN);
    bits_copy(buffer, mlln_block, 0, ptrsize);
    node.pre = bytestonum(buffer);

    memset(buffer, 0, BUFLEN);
    bits_copy(buffer, mlln_block, ptrsize, ptrsize);
    node.data = bytestonum(buffer);

    memset(buffer, 0, BUFLEN);
    bits_copy(buffer, mlln_block, ptrsize*2, ptrsize);
    node.next = bytestonum(buffer);

    return node;
}

// put main link list node at blkno to the disk
// main link list node have pre, data, next pointers
int mlln_dump(mlln node)
{
    int res = 0;
    int node_base = mainlist_base - (node.base * m), blklen= mceil(m, 8);
    unsigned char mlln_block[blklen], buffer[BUFLEN];

    memset(mlln_block, 0, blklen);

    // creating mlln block
    memset(buffer, 0, BUFLEN);
    numtobytes(buffer, node.pre);
    bits_put(mlln_block, 0, buffer, ptrsize);

    memset(buffer, 0, BUFLEN);
    numtobytes(buffer, node.data);
    bits_put(mlln_block, ptrsize, buffer, ptrsize);

    memset(buffer, 0, BUFLEN);
    numtobytes(buffer, node.next);
    bits_put(mlln_block, ptrsize*2, buffer, ptrsize);

    res += disk_wr(mlln_block, m, node_base);   // writing block to disk
    fflush(diskfp);
    return res;
}

