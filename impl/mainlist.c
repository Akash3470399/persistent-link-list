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
    mlln main_node, helper_node;
    int ret = 0;
    
    if(position > 0 && position <= (getnlists()+1))
    {
        main_node = new_mlln(nullblk, nullblk, nullblk);
        if(position == 1)
        {
            if(mainlist_head != nullblk)         
            {
                helper_node = mlln_load(mainlist_base);
                helper_node.pre = main_node.base;
                main_node.next = helper_node.base;
                mlln_dump(helper_node);
            }
            
            mainlist_length += 1;
            update_mainlist_head(main_node.base);
        }
        else
        {
            // get (position -1)th node so after we can add new node
            helper_node = mlln_load(mainlist_head);
            position -= 1;                      
            for(int i = 1; i < position; i++)
               helper_node = mlln_load(main_node.next);
           
            // making connection of new node
            main_node.pre = helper_node.base;
            main_node.next = helper_node.next;
    
            // updating previous node
            helper_node.next = main_node.base;
            mlln_dump(helper_node);
            
            if(main_node.next != nullblk)   // if adding in between to nodes
            {
                // updating next node
                helper_node = mlln_load(main_node.next);
                helper_node.pre = main_node.base;
                mlln_dump(helper_node);
            }
        }    

        mlln_dump(main_node);
        
    }
}


mlln new_mlln(int pre, int data, int next)
{
    mlln node ;
    if(mainfreelist == nullblk)
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

