#ifndef DATALIST_H
#define DATALIST_H

typedef struct data_link_list_node dlln;

struct data_link_list_node
{
    int pre, data, next;
    int base;
};


dlln dlln_load(int blkno);
int dlln_dump(dlln node);
#endif 
