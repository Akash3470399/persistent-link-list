#include <stdio.h>

#include "disk.h"
#include "bitsarr.h"
#include "helper.h"
#include "list.h"

int main()
{
	int num;
	//check_or_mkdir("disk");
	//disk_creator("llvd", 1769472);
	//disk_init("./disk/llvd");
	disk_calculations("./disk/llvd");

	mlln node ;//= {11193, 82623, 2223};

	//mlln_put(node, 0);
	node = mlln_get(0);
	printf("%d %d %d\n", node.left, node.data, node.right);

	fclose(diskfp);
}
