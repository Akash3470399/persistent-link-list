#include <stdio.h>

#include "disk.h"
#include "bitsarr.h"
#include "helper.h"


int main()
{
	int num;
	check_or_mkdir("disk");
	disk_creator("llvd", 1769472);
	disk_init("./disk/llvd");

	printf("x %ld, y %ld\n", x, y);
}
