#include <stdio.h>

#include "disk.h"
#include "bitsarr.h"
#include "helper.h"
#include "list.h"

int main()
{
	dlln d1 = {1, 23222, 23}, d2 = {23233, 800, 000}, d;
	mlln m1 = {2332, 22322,25}, m2 = {0, 0, 0}, m;

	int num;
	check_or_mkdir("disk");
	disk_creator("llvd", 1769472);
	disk_init("./disk/llvd");
	disk_calculations("./disk/llvd");

	bitmap_print(data_bitmap);
	fclose(diskfp);
}
