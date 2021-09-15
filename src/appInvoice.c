#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "../inc/appInvoice.h"

#define MAX_FILE_NAME_SIZE 100

static struct dirent **nameList;
int nameListSize;

// when return 1, scandir will put this dirent to the list
static int parse_ext(const struct dirent *dir)
{
    if(!dir)
        return 0;

    if(dir->d_type == DT_REG)
    {
        const char *ext = strrchr(dir->d_name,'.');
        if((!ext) || (ext == dir->d_name))
        {
            return 0;
        }
        else
        {
            if(strcmp(ext, ".inv") == 0)
                return 1;
        }
    }

    return 0;
}

void scanDir()
{
    int i = 0;

    int n = scandir("inv/", &nameList, parse_ext, alphasort);
    nameListSize = n;

    if( n < 0 )
    {
        printf("Scan problem.");
    }
    else
    {
        while( n-- )
        {
            printf("%d - %s\n", i+1, nameList[i]->d_name);

            i++;
            // free(namelist[n]);
        }
        printf("%d - Press ESC to return main menu\n", i);

        // free(namelist);
    }
}

int printInvoice(int i)
{
    // Check if file doesn't exist
    if( i-1 >= nameListSize )
    {
        printf("Record not found.\n");
        return 0;
    }

    printf("%d - %s\n", i, nameList[i-1]->d_name);

    FILE *fptr;

    char fileName[MAX_FILE_NAME_SIZE];

    snprintf(fileName, MAX_FILE_NAME_SIZE, "inv/%s", nameList[i-1]->d_name);

    fptr = fopen(fileName, "r");

    if( fptr == NULL )
    {
        printf("Cannot open file\n");
    }

    char c = fgetc(fptr);

    while( c != EOF )
    {
        printf("%c", c);
        c = fgetc(fptr);
    }
    printf("\n");
    fclose(fptr);

    return 1;
}