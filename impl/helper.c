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
        printf("disk with %ld bytes is created.\n", wrbytes);
    }
    else
    {
        printf("Disk creation failed\n");
        wrbytes = -1;
    }

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

// takes 4 bytes of memery array and return int forming from it.
unsigned int bytestonum(unsigned char *arr)
{
    // setting all bits to 0 so that bits can copy using or operation.
    unsigned int num = 0, n;    
    for(int i = 3; i > -1; i--)
    {
        num = num << 8;     
        num |= arr[i];      // copoing 1byte to lower 8 bits.
    }
    //n = (unsigned int)le32toh((uint32_t)num);             // converting back from little endian notation
    return num;
}

// store 4 bytes of integer(num) into buffer array.
void numtobytes(unsigned char *buffer, unsigned int num)
{
    // unsigned int num = (unsigned int)htole32((uint32_t)n);       // converting to little endian notation
    for(int i = 0; i < 4; i++)
    {
        // extract first 8 bits & shift it to lower 8 bits 
        // to copy it in char array 
        buffer[i] = (num & 0xFF);   
        num = num>>8;
    }
}
