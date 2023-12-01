#include <string.h>
#include "list.h"
#include "disk.h"
#include "helper.h"
#include "bitsarr.h"

// if pre pointer of a block is pointing to self then that is first block of list
// if next pointer of a block is pointing to self then that is last block of list

/*
   	N is new node
	we want to add this node N between node A and B
	each node have pre & next pointers

							pre		 ___________	next
		new node			<-------|   N       |------>
									|___________|


						 ___________	next ___________
		list		...	|	A		|------>|	B		| ..... 
						|___________|<------|___________|
										pre

	N.next = A.next;
	n.pre = B.pre;

	A.next = ptr(N);
	B.pre = ptr(N);

*/



// returns length of main link list
int getnlists()
{
	int itr, count = 0, mainll_base;
	if(bitmap_isset(main_bitmap, MLLSTRT) == 1)	// if there is no main link list
	{
		for(int i = 0; i < x; i++)
			if(bitmap_isset(main_bitmap, i) == 1)
				count += 1;
	}
	return count;
}


// add an empty data list at specified position in main link list 
// position starts from 1
// on success returns position else -1
int addlist(int position)
{

	mlln A, B, N, mnode;

	int llstart = 0, mnode_blk, itr = position;

	if(position == 1)
	{
		if(bitmap_isset(main_bitmap, MLLSTRT) == 0)			// when main link list is empty
		{
			bitmap_set(main_bitmap, MLLSTRT);
			N.pre = MLLSTRT, N.data = sblk, N.next = MLLSTRT;
			mlln_put(N, MLLSTRT);
		}
		else												// when main link list is not empty
		{
			
		}
	}
	else if(position > 0 && position <= getnlists()+1)		// 1 <= position <= length of main link list + 1
	{
		mnode_blk = MLLSTRT;
		mnode = mlln_get(MLLSTRT);							// head of main link list
		
		while(mnode.next != mnode_blk && itr > 1)
		{
			mnode_blk = mnode.next;
			mnode = mlln_get(mnode.next);
			itr -= 1;
		}

		if(itr == 1)										// we are at the position where new empty list is to be added
		{
			B = mnode;
			A = mlln_get(B.pre);
			mnode_blk = bitmap_getpos(main_bitmap);
				
			N.pre = B.pre, N.data = sblk, N.next = A.next;
			A.next = mnode_blk, B.pre = mnode_blk;

			mlln_put(A, N.pre);
			mlln_put(N, mnode_blk);
			mlln_put(B, N.next);
		}
		else if(itr == 2)									// empty list is added at the end
		{
			A = mnode;										// A.next is pointing to A only
			mnode_blk = bitmap_getpos(main_bitmap);
			N.pre = A.next, N.data = sblk, N.next = mnode_blk;
			A.next = mnode_blk;

			mlln_put(A, N.pre);
			mlln_put(N, mnode_blk);
		}
	}
	else
		position = -1;

	return position;

}

// returns the length of data link list at position of listno in main link list
// if list no is invalid then -1 is returned
int listsize(int listno)
{	
	int count = -1, llstart, dnode_blk;
	dlln dnode;
	if(listno > 0 && (listno <= getnlists()))
	{
		count = 0;
		mlln mnode= mlln_get(MLLSTRT);			// head of main link list
	
		for(int i = 1; i < listno; i++)			// find ptr to data link list
			mnode = mlln_get(mnode.next);	

		dnode_blk = mnode.data;
		
		if(dnode_blk == sblk)						// if data list is empty
			count = 0;
		else
		{
			count = 1;
			dnode = dlln_get(dnode_blk);		// head of data link list at position listno
			
			while(dnode.next != dnode_blk)
			{
				dnode_blk = dnode.next;
				dnode = dlln_get(dnode.next);
				count += 1;
			}
		}
	}
	return count;
}

// write the data at nodeno node in data link list at postion listno
// it overwrites the content of data node
// if listno or nodeno is invalid then 0 is returned else 1
int llwrite(int listno, int nodeno, int data)
{
	int mnode_blk = MLLSTRT, dnode_blk, llstart, ret = 0;
	mlln mnode;
	dlln dnode;

	if(listno > 0 && listno <= getnlists())
	{
		mnode = mlln_get(MLLSTRT);
		for(int i = 1; i < listno; i++)
		{
			mnode_blk = mnode.next; 
			mnode = mlln_get(mnode.next);
		}

		dnode_blk = mnode.data;
		if (dnode_blk != sblk)				 
		{
			dnode = dlln_get(dnode_blk);
			while(dnode.next != dnode_blk && nodeno > 1)	// find the nodeno position in list
			{
				dnode_blk = dnode.next;
				dnode = dlln_get(dnode.next);
				nodeno -= 1;
			}

			if(nodeno == 1)
			{
				dnode.data = data;
				dlln_put(dnode, dnode_blk);
				ret = 1;
			}	
		}
	}
	return ret;
}

// insert a node at nodeno position in listno th main list node
// if listno or nodeno is invalid 0 is returned else 1;
// valid nodeno is 1 <= nodeno <= length of lisno th list + 1
int insert(int listno, int nodeno, int data)
{
	int mnode_blk = MLLSTRT, dnode_blk, dllstrt, ret = 0, itr;
	mlln mnode;
	dlln dnode, A, N, B;

	if(listno > 0 && listno <= getnlists())
	{
		mnode = mlln_get(MLLSTRT);
		for(int i = 1; i < listno; i++)
		{
			mnode_blk = mnode.next; 
			mnode = mlln_get(mnode.next);
		}
	
		dllstrt = mnode.data;
		if(nodeno == 1)	
		{
			dnode_blk = bitmap_getpos(data_bitmap);
			mnode.data = dnode_blk;
			mlln_put(mnode, mnode_blk);										// updated data list head in main link list
			
			if(dllstrt == sblk)	 											// data link list is empty
			{
				N.pre = dnode_blk, N.data = data, N.next = dnode_blk;		// creating self pointing node
				dlln_put(N, dnode_blk);
			}
			else
			{
				B = dlln_get(dllstrt);
				N.pre = dnode_blk, N.data = data, N.next = dllstrt;
				B.pre = dnode_blk;

				dlln_put(B, N.next);
				dlln_put(N, dnode_blk);
			}
			ret = 1;
		}
		else
		{

			// Here i have 2 task 
			// 	1. check if nodeno is valid
			// 	2. if nodeno is valid then add new data node at nodeno position
			// I'm trying to complete both the task in one iteration
			ret = 1;

			dnode_blk = mnode.data;
			dnode = dlln_get(dnode_blk);
			itr = nodeno;	
			while(dnode.next != dnode_blk && itr > 1)
			{
				dnode_blk = dnode.next;
				dnode = dlln_get(dnode.next);
				itr -= 1;
			}

			if(itr == 1)										// we are at the position where insert to done
			{
				B = dnode;
				A = dlln_get(B.pre);
				dnode_blk = bitmap_getpos(data_bitmap);
					
				N.pre = B.pre, N.data = data, N.next = A.next;
				A.next = dnode_blk, B.pre = dnode_blk;

				dlln_put(A, N.pre);
				dlln_put(N, dnode_blk);
				dlln_put(B, N.next);
			}
			else if(itr == 2)									// added at the end
			{
				A = dnode;										// A.next is pointing to A only
				dnode_blk = bitmap_getpos(data_bitmap);
				N.pre = A.next, N.data = data, N.next = dnode_blk;
				A.next = dnode_blk;

				dlln_put(A, N.pre);
				dlln_put(N, dnode_blk);
			}
			else
				ret = 0; 										// nodeno is invalid
		}
	
	}
	return ret;
}

// delete node at nodeno in list listno
int delete(int listno, int nodeno)
{
	
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
	node.pre = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, mlln_block, ptr_size, ptr_size);
	node.data = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, mlln_block, ptr_size*2, ptr_size);
	node.next = bytestonum(buffer);

	return node;
}

// put main link list node at blkno to the disk
// main link list node have pre, data, next pointers
int mlln_put(mlln node, int blkno)
{
	int res = 0;
	int node_base = main_list_base - (blkno * m), blklen= mceil(m, 8);
	unsigned char mlln_block[blklen], buffer[4];

	memset(mlln_block, 0, blklen);

	// creating mlln block
	memset(buffer, 0, 4);
	numtobytes(buffer, node.pre);
	bits_put(mlln_block, 0, buffer, ptr_size);

 	memset(buffer, 0, 4);
	numtobytes(buffer, node.data);
	bits_put(mlln_block, ptr_size, buffer, ptr_size);

	memset(buffer, 0, 4);
	numtobytes(buffer, node.next);
	bits_put(mlln_block, ptr_size*2, buffer, ptr_size);

	res += disk_wr(mlln_block, m, node_base);	// writing block to disk
	fflush(diskfp);
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
	node.pre = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, dlln_block, ptr_size, datasize);	// as data is integer
	node.data = bytestonum(buffer);

	memset(buffer, 0, 4);
	bits_copy(buffer, dlln_block, ptr_size + datasize, ptr_size);
	node.next = bytestonum(buffer);

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
	numtobytes(buffer, node.pre);
	bits_put(dlln_block, 0, buffer, ptr_size);

 	memset(buffer, 0, 4);
	numtobytes(buffer, node.data);
	bits_put(dlln_block, ptr_size, buffer, datasize);

	memset(buffer, 0, 4);
	numtobytes(buffer, node.next);
	bits_put(dlln_block, ptr_size + datasize, buffer, ptr_size);

	res += disk_wr(dlln_block, d, node_base);	// writing block to disk

	return res;
}
