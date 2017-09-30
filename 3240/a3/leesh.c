#include "apue.h"
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int makeargv(char * s, char * delimiters, char *** argvp);
char* read_line(void);
int my_umask(unsigned long int);
void my_cd(char* path);
void free_makeargv(int count, char** buffers);

unsigned long int mask=0777;

int main(int argc, char* argv[]){
	int status=1;
	char* line;
	char** commands;
	char** singlecommand;
	int pid, commandc, Status, count;
	int pipefd[2], pipe2fd[2];
	char* cwd_buffer = malloc(200);

	do{
		getcwd(cwd_buffer,200);
		printf("%s>",cwd_buffer);
		line = read_line();
		if(0>strcmp("",line)){
			if(0==strcmp("zzz",line)){
				printf("Thank you for using leesh!\n");
				status=0;
				break;
			}
			commandc = makeargv(line, "|", &commands);
			free(line);
			if(0==strcmp("umask", commands[0])){
				makeargv(commands[0], " ", &singlecommand);
				if(-1!=my_umask(strtoul(singlecommand[1],NULL,8))){
					printf("%lo\n",mask);
				}
			}else if(0==strcmp("cd", commands[0])){
				makeargv(commands[0], " ", &singlecommand);
				my_cd(singlecommand[1]);
			}else{
				pid = fork();
				if(pid==0){
					if(commandc>1){
						pipe(pipefd);
						for(int i=0;i<commandc;i++){
							count = makeargv(commands[i], " ", &singlecommand);
							if(i==0){//leftmost process
								pid = fork();
								if(pid==0){
									for(int j=0;j<count;j++){
										if(0==strcmp("<", singlecommand[j])){
											dup2(open(singlecommand[j+1],O_RDONLY),STDIN_FILENO);
											singlecommand[j] = NULL;
											break;
										}
									}

									dup2(pipefd[1],STDOUT_FILENO);
									close(pipefd[0]);
									execvp(singlecommand[0],singlecommand);
								}
							}else if(commandc==i+1){//rightmost process
								for(int j=0;j<count;j++){
									if(0==strcmp(">", singlecommand[j])){
										dup2(open(singlecommand[j+1],O_WRONLY|O_CREAT,mask),STDOUT_FILENO);
										singlecommand[j] = NULL;
										break;
									}
								}

								dup2(pipefd[0],STDIN_FILENO);
								close(pipefd[1]);
								execvp(singlecommand[0],singlecommand);
							}else{//middle process
								pipe2fd[0]=pipefd[0];
								dup2(pipe2fd[0],STDIN_FILENO);
								pipe2fd[1] = pipefd[1];
								close(pipefd[1]);

								pipe(pipefd);
								pid = fork();
								if(pid==0){
									close(pipefd[0]);
									dup2(pipefd[1],STDOUT_FILENO);
									execvp(singlecommand[0],singlecommand);
								}
							}
						}
					}else{
						count = makeargv(commands[0], " ", &singlecommand);
						for(int j=0;j<count;j++){
							if(0==strcmp("<", singlecommand[j])){
								dup2(open(singlecommand[j+1],O_RDONLY),STDIN_FILENO);
								singlecommand[j] = NULL;
								break;
							}
							if(0==strcmp(">", singlecommand[j])){
								dup2(open(singlecommand[j+1],O_WRONLY|O_CREAT,mask),STDOUT_FILENO);
								singlecommand[j] = NULL;
								break;
							}
						}
						execvp(singlecommand[0],singlecommand);
					}
				}
			}
			count = makeargv(commands[commandc-1]," ",&singlecommand);	
			if(0!=strncmp(singlecommand[count-1],"&",1)){
				waitpid(pid,&Status,0);
			}
		}
	}while(status);

	free(cwd_buffer);
	free_makeargv(commandc, commands);
	free_makeargv(count, singlecommand);
	free(line);
}

char* read_line(void){
	char* buffer = malloc(sizeof(char)*1024);
	if(NULL==fgets(buffer, 1024, stdin)){
		printf("fgets error in read_line");
	}
	buffer[strcspn(buffer, "\n")] = 0;//removes the trailing \n from fgets(who thought that was a good idea?)
	return buffer;
}

int my_umask(unsigned long int new_mask){
	if(new_mask>=0&&new_mask<=0777){
		mask = new_mask;
		return new_mask;
	}else{
		return -1;
	}
}

void my_cd(char* path){
	if(-1==chdir(path)){
		printf("chdir error");
	}
}

void free_makeargv(int count, char** buffers){
	for(int i=0;i<count;i++){
		free(buffers[i]);
	}
}

