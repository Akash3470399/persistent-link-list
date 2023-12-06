#include <stdio.h>
#include "helper.h"
#include "disk.h"

int main()
{
    check_or_mkdir("disk");
    disk_creator("llvd", 1769472);
    disk_init("./disk/llvd");
}
