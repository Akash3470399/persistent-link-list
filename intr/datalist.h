#ifndef DATALIST_H
#define DATALIST_H

typedef struct data_link_list_node dlln;
typedef struct search_result search_result;


struct data_link_list_node
{
    int pre, data, next;
    int base;
};

// if found == 1 then then data is valid
struct search_result
{
    char found;
    int data;
};

// return list size
int dllsize(int listno);

// read data of node at nodeno from listno'th list
search_result dllread(int listno, int nodeno);

// write data at node at nodeno from listno't list
int dllwrite(int listno, int nodeno, int data);  // overwrite

// insert new node at nodeno from listno't list with data
int dllinsert(int listno, int nodeno, int data);

// delete nodeno'th node from listno'th list
int dlldelete(int listno, int nodeno);

// insert node to data blocks free list
void addto_datafreelist(dlln node);

// returns availbe free data link list node
dlln get_datafreelist_node();

// when some main nodes are deleted then that space is available to data nodes too
void incr_dlln_availability();

// when some new main nodes are added then that space is not available to data nodes 
void decr_dlln_availability();


// return new data link list node with given properties
dlln new_dlln(int pre, int data, int next);

// read data link list block at blkno from disk and return
dlln dlln_load(int blkno);

// write data link list block to disk
int dlln_dump(dlln node);
#endif 
