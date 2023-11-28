#include "list.h"
#include "disk.h"

// returns length of main link list
int getnlists()
{
	int itr, count = 0, mainll_base;
	
	return count;
}


// get main link list node at position pos
mlln mlln_get(int blkno)
{
	mlln node;
	int node_base = main_list_base - (blkno * m);
	//disk_rd((unsigned char*)&(node.left), );
}
