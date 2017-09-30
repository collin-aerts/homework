/*
Collin Aerts
CS3240
2/14/17
a2
*/

#include "apue.h"
#include <fcntl.h>	
#include <limits.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

//Directory* tmp_dir;
char* buf;
//DIR* dp;
//struct dirent* dirp;

buf = realpath(argv[1], NULL);
printf("the real path is %s", buf);	
return(0);
}