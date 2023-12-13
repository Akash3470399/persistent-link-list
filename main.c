#include <stdio.h>
#include <stdlib.h>

#include "disk.h"
#include "bitsarr.h"
#include "helper.h"
#include "datalist.h"
#include "mainlist.h"

int main()
{
    FILE *listfp;
    int choice, listno, nodeno, data, count;
    search_result sr;

    if(disk_config("./disk/llvd") == 0)
    {
        printf("Disk not found.\n");
        exit(EXIT_FAILURE);
    }

    do
    {
        printf("\nChoose the operation:\n\t");
        printf("1. create empty data list\n\t2. Get lists count\n\t3. Add data to list\n\t4. Read data from list\n\t");
        printf("5. Delete data\n\t6. Get data list length \n\t7. Save list\n\t8. Exit\n\t>>>");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                    printf("Enter list no : ");
                    scanf("%d", &listno);

                    if(addlist(listno) > 0)
                        printf("Empty list is created at position %d\n", listno);
                    else
                        printf("Please check you have provied right position number.\n");

                break;
                            
            case 2:
                    count = getnlists();
                    if(count == 0)
                        printf("Opps, You haven't added any new list.\n");
                    else
                        printf("There are total %d lists\n", count);
                        
                break;
            case 3:
                    printf("Enter list no: ");  
                    scanf("%d", &listno);
                    printf("Enter node no : ");
                    scanf("%d", &nodeno);
                    printf("Enter data : ");
                    scanf("%d", &data);
            
                    if(dllinsert(listno, nodeno, data) > 0)
                        printf("Data added\n");
                    else 
                        printf("Please check information you have provied\n");
                break;
            case 4:
                    printf("Enter list no: ");  
                    scanf("%d", &listno);
                    printf("Enter node no : ");
                    scanf("%d", &nodeno);
    
                    sr = dllread(listno, nodeno);
                    if(sr.found == 1)
                        printf("Data is : %d\n", sr.data);
                    else
                        printf("Please check information you have provied\n");
                break;
            case 5:
                    printf("Enter list no: ");  
                    scanf("%d", &listno);
                    printf("Enter node no : ");
                    scanf("%d", &nodeno);
                    
                    if(dlldelete(listno, nodeno) > 0)
                        printf("Deleted %d node form %d list\n", listno, nodeno);
                    else
                        printf("Please check information you have provied\n");
                break;
            case 6: 
                    printf("Enter list no : ");
                    scanf("%d", &listno);
                    
                    count = dllsize(listno);
                    if(count < 0)
                        printf("Please check you have provied right list number\n");
                    else
                        printf("list no %d have %d nodes\n", listno, count);
                break;
            case 7: 
                    printf("Enter list no : ");
                    scanf("%d", &listno);
                
                    count = dllsize(listno);
                    
                    if(count < 0)
                        printf("Please check you have provied right list number\n");
                    else
                    {       
                        listfp = fopen("saved_list", "wb+");
                        if(listfp != NULL)
                        {
                            fprintf(listfp, "List no : %d len :%d\ndata :\n\n%8d " , listno, count, 1);
                            for(int i = 1; i <= count; i++)
                            {
                                sr =  dllread(listno, i);
                                fprintf(listfp, "%d ", sr.data);

                                if(i % 8 == 0)
                                    fprintf(listfp, "\n%8d ", i);
                            }
                            fclose(listfp);
                            printf("List saved.\n");
                        }
                    }
                    break;
            case 8: break;
            default:
                printf("Incorrect choice\n");
        }
    }while(choice != 8);
    fclose(diskfp);
}
