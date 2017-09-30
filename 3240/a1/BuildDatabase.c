/*
Collin Aerts
CS3240
1/20/2016
a0 - pointer hell
*/

#include "apue.h"
#include <fcntl.h>

typedef struct {
	char code[4];
	int pop;
	double lifeexp;
} Country;

typedef struct {
	char code[4];
	int offset;
} Direction;

void place_direction(Direction*, Direction**);

int directory_size = 0;
char* name;
int name_length;
int offset = sizeof(int)+1;//set the offset for the first directory entry (to account for the int)

int main(int argc, char* argv[])
{
	FILE* in_stream = fopen("./AllCountries.dat", "r");
	FILE* struct_stream = fopen("./BinaryAllCountries", "wb");
	FILE* directory_stream = fopen("./DirectoryAllCountries", "wb");

	Direction** directory = malloc(sizeof(Direction)*300);
	Direction* direction = malloc(sizeof(Direction));
	Country* tmp_country=malloc(sizeof(Country));
	char* BUFF = malloc(512);
	char* start;
	int commas_encountered=0;
	int i=0;

	fseek(struct_stream,sizeof(int),SEEK_SET);//create room for one int.

	while(!feof(in_stream)){
		if(!fgets(BUFF, 512, in_stream)){//prevents program from trying to run on an empty line
			break;
		}
		while(BUFF[i]!='\n'){
			if(BUFF[i]==','){
				commas_encountered++; 
				BUFF[i] = '\0';
				switch(commas_encountered){
					case 2:
					strncpy(tmp_country->code, start, 4);
					break;

					case 3:
					name = start;
					name_length=strlen(name);
					break;

					case 8:
					tmp_country->pop=atoi(start);
					break;

					case 9:
					tmp_country->lifeexp=atof(start);
					break;

					default:
					break;
				}
				start = &BUFF[i+1];
			}
			i++;
		}
		i=0;
		commas_encountered=0;
		fwrite(tmp_country, sizeof(Country),1,struct_stream);
		fwrite(&name_length, sizeof(int),1,struct_stream);
		fwrite(name,name_length+1,1,struct_stream);//BinaryAllCountries writing.

		strncpy(direction->code, tmp_country->code, 4);//DirectoryAllCountries writing
		direction->offset=offset;
		place_direction(direction,directory);
		
		directory_size++;
		direction = malloc(sizeof(Direction));
		free(tmp_country);
		tmp_country=malloc(sizeof(Country));
		offset +=sizeof(Country)+sizeof(int)+name_length+1;//plus for the additional nullbyte
	}
	fseek(struct_stream,0,SEEK_SET);
	fwrite(&directory_size,sizeof(int),1,struct_stream);
	for(int i=0;i<directory_size;i++){
		fwrite(directory[i],sizeof(Direction),1,directory_stream);
		free(directory[i]);
	}

	free(directory);
	free(direction);
	free(tmp_country);
	free(BUFF);
	fclose(struct_stream);
	fclose(directory_stream);
	fclose(in_stream);

	return 0;
}

void place_direction(Direction* entry, Direction** directory){
	int index=0;
	while(index!=directory_size){
		if(0>strncmp(entry->code,directory[index]->code,4)){
			for(int i=directory_size;i>index;i--){
				directory[i]=directory[i-1];
 			}
 			break;	
		}
		index++;
	}
	directory[index]=entry;
}
