#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// create a vhd with filename as dname & with size of dsize bytes
// initilize each byte to 0
// return disk size of created disk if disk creation operation is successfull else 0
long disk_creator(char *dname, long dsize)
{
	long wrbytes = 0;
	int buflen = 1024;
	char buffer[buflen];
	FILE *diskfp;

	sprintf(buffer, "./disk/%s", dname);
	diskfp = fopen(buffer, "wb+");

	if(diskfp != NULL)
	{
		memset(buffer, 0, buflen);

		for(int i = 0; i < dsize/buflen; i++)
			wrbytes += fwrite(buffer, 1, buflen, diskfp);

		if(dsize % buflen != 0)
			wrbytes += fwrite(buffer, 1, dsize % buflen, diskfp);

		fflush(diskfp);
		fclose(diskfp);
	}
	else
		printf("Disk creation failed\n");

	return wrbytes;
}

int check_or_mkdir(char *dirname)
{
	int res = -1;

	struct stat st = {0};
	if(stat(dirname, &st) == 0)
		res = S_ISDIR(st.st_mode);
	else
		res = mkdir(dirname, 0777);

	return res;
}


// if a <= 0 return 0 else return ceil of the a/b
int mceil(int a, int b)
{
	int res = a/b;
	if(a <= 0)		
		res = 0;
	else 
		res += ((a%b) > 0);
	return res;
}

// return the size of file.
// on error return -1
int get_filesize(char *filenm)
{
	int size = -1;

	FILE *fp = fopen(filenm, "r");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fclose(fp);
	}
	else
		printf("err while caculating file size : %s\n", filenm);
	return size;
}
