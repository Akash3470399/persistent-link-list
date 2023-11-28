#ifndef LIST_H
#define LIST_H

typedef struct search_result search_result;
typedef struct main_link_list_node mlln;
typedef struct data_link_list_node dlln;


struct search_result
{
	char found;
	int data;
};

struct main_link_list_node
{
	int left, data, right;
};

struct data_link_list_node
{
	int left, data, right;
};

int getnlists();
search_result read(int listno, int nodeno);
int write(int listno, int nodeno, int data);	// overwrite
int addlist(int position);
int listsize(int listno);
int insert(int listno, int nodeno, int data);
int delete(int listno, int nodeno);


// internal methods
mlln mlln_get(int blkno);
int mlln_put(mlln node, int blkno);

dlln dlln_get(int blkno);
int dlln_put(dlln node, int blkno);

#endif
