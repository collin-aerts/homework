#include "apue.h"
#include <fcntl.h>

typedef struct {
	char code[4];
	char name[50];
	int pop;
	double lifeexp;
} Country;

typedef struct {
	char code[4];
	int offset;
} Direction;

int search(char* code, Direction directory[]);

Country file_search(FILE*, int);


int directory_size;

int main(int argc, char* argv[])
{
	FILE* struct_stream = fopen("./BinaryAllCountries", "rb");
	FILE* directory_stream = fopen("./DirectoryAllCountries", "rb");
	Country country;

	fread(&directory_size,sizeof(int),1,struct_stream);
	int offset;
	Direction directory[directory_size];
	fread(&directory,sizeof(Direction),directory_size,directory_stream);

	char* BUFF = malloc(20);
	while(1){
		printf("Enter input:");
		scanf("%s",BUFF);
		if(0==strcmp("zzz",BUFF)){
			free(BUFF);
			fclose(struct_stream);
			fclose(directory_stream);
			return 0;
		}
		offset = search(BUFF, directory);
		if(offset==-1){
			printf("not found\n");
		}else{
			country = file_search(struct_stream, offset);
			printf("CODE:%s\nNAME:%s\nLIFE EXP:%f\nPOP:%i\n",country.code,country.name,country.lifeexp,country.pop);
		}
	}
}

int search(char* code, Direction directory[]){
	int first = 0;
	int last = directory_size-1;
	int middle;
	while(first<=last){
		middle=(first+last)/2;
		if(0==strcmp(code,directory[middle].code)){
			return directory[middle].offset;
		}else if(0<strcmp(code,directory[middle].code)){
			first=middle+1;
		}else{
			last=middle-1;
		}
	}
		return -1;
}

Country file_search(FILE* stream, int offset){
	fseek(stream,offset-1,SEEK_SET);
	Country ret_country;
	int num;
	fread(&ret_country.code, sizeof(char),4,stream);
	fread(&ret_country.pop, sizeof(int),1,stream);
	fread(&ret_country.lifeexp, sizeof(&ret_country.lifeexp),1,stream);
	fread(&num, sizeof(int),1,stream);
	fread(&ret_country.name,sizeof(char),num+1,stream);

	return ret_country;
}