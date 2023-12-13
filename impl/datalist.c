#include <string.h>

#include "helper.h"
#include "consts.h"
#include "bitsarr.h"
#include "disk.h"
#include "mainlist.h"
#include "datalist.h"

// here d is size of datanode in bits

// to find length of list
int _dllsize(dlln itrnode);

// return list size
int dllsize(int listno)
{
    mlln basenode;                      // node that gives the ptr to head of list
    dlln head;       

    int size = -1, ptr;
    
    if(listno > 0 && listno <= getnlists())
    {
        basenode = getnth(listno);      
        
        if(basenode.data == nullblk)    // if list is not empty
            size = 0;
        else                        
            size = _dllsize(dlln_load(basenode.data)); 
    }

    return size;
}

// read data of node at nodeno from listno'th list
search_result dllread(int listno, int nodeno)
{
    search_result sr = {0, 0};
    mlln basenode;
    dlln datanode;

    if(listno > 0 && listno <= getnlists())
    {
        basenode = getnth(listno);

        if(basenode.data != nullblk)                // if list is not empty
        {
            datanode = dlln_load(basenode.data);
            
            // approch toward expected data node
            while((datanode.next != nullblk) && nodeno > 1)
            {
                datanode = dlln_load(datanode.next);
                nodeno -= 1;
            }

            if(nodeno == 1)
            {
                sr.found = 1;
                sr.data = datanode.data;
            }
        }

    }

    return sr;
}

// write data at node at nodeno from listno't list
int dllwrite(int listno, int nodeno, int data)  // overwrite
{
    int ret = 0;
    mlln basenode;
    dlln datanode;

    if(listno > 0 && listno <= getnlists())
    {
        basenode = getnth(listno);

        if(basenode.data != nullblk)
        {
            datanode = dlln_load(basenode.data);
            
            while((datanode.next != nullblk) && nodeno > 1)
            {
                datanode = dlln_load(datanode.next);
                nodeno -= 1;
            }

            if(nodeno == 1)
            {
                datanode.data = data;
                dlln_dump(datanode);
            }
        }

    }

    return ret;
}


// insert new node at nodeno from listno't list with data
// shifts previous nodes to right side
int dllinsert(int listno, int nodeno, int data)
{
    int ret = 0;
    mlln basenode;
    dlln newnode, tmpnode;

    if(listno > 0 && listno <= getnlists())
    {
        basenode = getnth(listno);
        
        if(nodeno == 1)
        {
            newnode = new_dlln(nullblk, data, nullblk);

            if(basenode.data != nullblk)                // if link is not empty
            {
                tmpnode = dlln_load(basenode.data);
                newnode.next = tmpnode.base;
                tmpnode.pre = newnode.base;

                dlln_dump(tmpnode);
            }

            basenode.data = newnode.base;
            dlln_dump(newnode);
            mlln_dump(basenode);
            ret = 1;
        }
        else if(basenode.data != nullblk)
        {
            tmpnode = dlln_load(basenode.data);
            nodeno -= 1;
            
            // here i have to do 2 task
            // 1. find if nodeno is valid 
            // 2. find previous node after which new node to be added

            // either we reach to last of link list or we found the previous node
            while((tmpnode.next != nullblk) && (nodeno > 1))
            {
                nodeno -= 1;
                tmpnode = dlln_load(tmpnode.next);
            }

            if(nodeno == 1)                     // if previous for newnode is found
            {

                newnode = new_dlln(tmpnode.base, data, tmpnode.next);
                
                tmpnode.next = newnode.base;
                dlln_dump(tmpnode);

                if(newnode.next != nullblk)     // if next node exist then update it
                {
                    tmpnode = dlln_load(newnode.next);
                    tmpnode.pre = newnode.base;
                    dlln_dump(tmpnode);
                }

                dlln_dump(newnode);

                ret = 1;
            }
        }
    }    

    return ret;
}

// delete nodeno'th node from listno'th list
int dlldelete(int listno, int nodeno)
{
    mlln basenode;
    dlln targetnode, tmpnode;
    int ret = 0;

    if(listno > 0 && listno <= getnlists())
    {
        basenode = getnth(listno);
        if(basenode.data != nullblk)
        {
            if(nodeno == 1)
            {
                targetnode = dlln_load(basenode.data);
                
                if(targetnode.next == nullblk)
                {
                    tmpnode = dlln_load(targetnode.next);
                    tmpnode.pre = nullblk;
                    dlln_dump(tmpnode);
                }
                
                basenode.data = targetnode.next;
                mlln_dump(basenode);
                
                addto_datafreelist(targetnode);
                
                ret = 1;
            }
            else
            {
                tmpnode = dlln_load(basenode.data);

                while((tmpnode.next != nullblk) && nodeno > 1)
                {
                    tmpnode = dlln_load(tmpnode.next);
                    nodeno -= 1;
                }

                if(nodeno == 1)
                {
                    targetnode = tmpnode;
                    tmpnode = dlln_load(targetnode.pre);    // previous node

                    tmpnode.next = targetnode.next;
                    dlln_dump(tmpnode);

                    if(targetnode.next != nullblk)         // change next node too
                    {
                        tmpnode = dlln_load(targetnode.next);
                        tmpnode.pre = targetnode.pre;
                        dlln_dump(tmpnode);
                    }
                    addto_datafreelist(targetnode); 
                    ret = 1;
                }
            }
        }
    }

    return ret;
}

// insert node to data blocks free list
// insert node before head in data freelist
void addto_datafreelist(dlln node)
{
    dlln previous_head;
    node.next = datafreelist_head, node.pre = nullblk;
    
    if(node.base == (data_bitmap.nextfreeblk - 1))
    {
        data_bitmap.nextfreeblk -= 1;
        datafreelist_len -= 1;

        incr_mlln_availability();
    }
    else if(datafreelist_head != nullblk)
    {
        previous_head = dlln_load(datafreelist_head);
        previous_head.pre = node.base;
        dlln_dump(previous_head);
    }
    
    dlln_dump(node);
    
    datafreelist_len += 1;
}


// returns availbe free data link list node
// assumption before calling this function is that node availbel in free list.
dlln get_datafreelist_node()
{
    dlln head, tmpnode;

    if(datafreelist_head != nullblk)
    {
        head = dlln_load(datafreelist_head);

        if(head.next != nullblk)        // update pre of next node       
        {
            tmpnode = dlln_load(head.next);
            tmpnode.pre = nullblk;
            dlln_dump(tmpnode);
        }
        
        datafreelist_head = head.next;  // update free list head
        datafreelist_len -= 1;
    }
    
    return head;
}


// when some main nodes are deleted then that space is available to data nodes too
void incr_dlln_availability()
{
    if(maindel == sizefactor)
    {
        data_bitmap.lastptr += 1;
        maindel = 0;
    }
}

// when some new main nodes are added then that space is not available to data nodes 
void decr_dlln_availability()
{
    if(mainused == 1)       // as soon as main block uses space it is not available
    {
        data_bitmap.lastptr -= 1;
    }
    
    if(mainused == sizefactor)
        mainused = 0;
}


// return new data link list node with given properties
dlln new_dlln(int pre, int data, int next)
{
    dlln node;

    if(datafreelist_head == nullblk)
    {
         node.base = data_bitmap.nextfreeblk++;
         node.pre = pre, node.data = data, node.next = next;
         set(data_bitmap, node.base);

         decr_mlln_availability();
    }
    else
        node = get_datafreelist_node(); 

   return node;
}


// get data link list node at blkno from disk
dlln dlln_load(int blkno)
{
    dlln node = {0, 0, 0, 0};
    int node_base = datalist_base + (blkno * d), blklen=mceil(d, 8);
    int datasize = 32;
    unsigned char dlln_block[blklen], buffer[BUFLEN];
   
    node.base = blkno;

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

int _dllsize(dlln itrnode)
{
    int size = 1;       //  as there at least itrnode present

    while(itrnode.next != nullblk)
    {
        itrnode = dlln_load(itrnode.next);
        size += 1;
    }
    return size;
}
