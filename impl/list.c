#include "list.h"
#include "disk.h"

// returns length of main link list
int getnlists()
{
	int itr, count = 0, mainll_base;
	
	return count;
}


// get main link list node at blkno main ll block 
mlln mlln_get(int blkno)
{
	mlln node = {0, 0, 0};
	int node_base = main_list_base - (blkno * m);

	disk_rd((unsigned char*)&(node.left), ptr_size, node_base);
	disk_rd((unsigned char*)&(node.data), ptr_size, node_base + ptr_size);
	disk_rd((unsigned char*)&(node.right), ptr_size, node_base + (2*ptr_size));

	return node;
}

// put main link list node at blkno 
int mlln_put(mlln node, int blkno)
{
	int res = 0;
	int node_base = main_list_base - (blkno * m);

	res += disk_wr((unsigned char*)&(node.left), ptr_size, node_base);
	res += disk_wr((unsigned char*)&(node.data), ptr_size, node_base + ptr_size);
	res += disk_wr((unsigned char*)&(node.right), ptr_size, node_base + (2*ptr_size));
	
	return res;
}
