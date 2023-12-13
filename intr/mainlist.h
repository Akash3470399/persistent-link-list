#ifndef MAINLIST_H
#define MAINLIST_H

typedef struct main_link_list_node mlln;
struct main_link_list_node
{
    int pre, data, next;
    int base;
};


// return count of data list available
int getnlists();

// add an empty data list at a position in list
int addlist(int position);

// return nth main link list node
mlln getnth(int n);

// when new data block is added that space is not available to main link list node
void decr_mlln_availability();

// when data block deleted that space is available to main link list to
void incr_mlln_availability();

mlln new_mlln(int pre, int data, int next);

mlln mlln_load(int blkno);
int mlln_dump(mlln node);
#endif
