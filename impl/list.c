#include <string.h>
#include "list.h"
#include "disk.h"
#include "helper.h"
#include "bitsarr.h"
// returns length of main link list
int getnlists()
{
	int itr, count = 0, mainll_base;
	
	return count;
}


// get main link list node at blkno from the disk
mlln mlln_get(int blkno)
{
	mlln node = {0, 0, 0};
	int node_base = main_list_base - (blkno * m), blklen = mceil(m, 8);
	unsigned char mlln_block[blklen], buffer[4];


	memset(mlln_block, 0, blklen);
	disk_rd(mlln_block, m, node_base);	// read one main link list block

	// extracting information from bits
	memset(buffer, 0, 4);
	bits_copy(buffer, mlln_block, 0, ptr_size);
	node.left = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, mlln_block, ptr_size, ptr_size);
	node.data = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, mlln_block, ptr_size*2, ptr_size);
	node.right = bytestonum(buffer);

	return node;
}

// put main link list node at blkno to the disk
// main link list node have left, data, right pointers
int mlln_put(mlln node, int blkno)
{
	int res = 0;
	int node_base = main_list_base - (blkno * m), blklen= mceil(m, 8);
	unsigned char mlln_block[blklen], buffer[4];

	memset(mlln_block, 0, blklen);

	// creating mlln block
	memset(buffer, 0, 4);
	numtobytes(buffer, node.left);
	bits_put(mlln_block, 0, buffer, ptr_size);

 	memset(buffer, 0, 4);
	numtobytes(buffer, node.data);
	bits_put(mlln_block, ptr_size, buffer, ptr_size);

	memset(buffer, 0, 4);
	numtobytes(buffer, node.right);
	bits_put(mlln_block, ptr_size*2, buffer, ptr_size);

	res += disk_wr(mlln_block, m, node_base);	// writing block to disk

	return res;
}

// get data link list node at blkno from disk
dlln dlln_get(int blkno)
{
	dlln node = {0, 0, 0};
	int node_base = data_list_base + (blkno * d), blklen=mceil(d, 8);
	int datasize = 32;
	unsigned char dlln_block[blklen], buffer[4];


	memset(dlln_block, 0, blklen);
	disk_rd(dlln_block, d, node_base);	// read one data link list block from disk

	// extracting information from bits
	memset(buffer, 0, 4);
	bits_copy(buffer, dlln_block, 0, ptr_size);
	node.left = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, dlln_block, ptr_size, datasize);	// as data is integer
	node.data = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, dlln_block, ptr_size + datasize, ptr_size);
	node.right = bytestonum(buffer);

	return node;
}

// store dlln node to disk fiel at blkno wrt to data link list blocks
int dlln_put(dlln node, int blkno)
{
	int res = 0, datasize = 32;
	int node_base =  data_list_base + (blkno * d), blklen= mceil(d, 8);
	unsigned char dlln_block[blklen], buffer[4];

	memset(dlln_block, 0, blklen);

	// creating mlln block
	memset(buffer, 0, 4);
	numtobytes(buffer, node.left);
	bits_put(dlln_block, 0, buffer, ptr_size);

 	memset(buffer, 0, 4);
	numtobytes(buffer, node.data);
	bits_put(dlln_block, ptr_size, buffer, datasize);

	memset(buffer, 0, 4);
	numtobytes(buffer, node.right);
	bits_put(dlln_block, ptr_size + datasize, buffer, ptr_size);

	res += disk_wr(dlln_block, d, node_base);	// writing block to disk

	return res;
}
