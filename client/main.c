#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFF_SIZE 512
char write_buff[BUFF_SIZE];
char read_buff[BUFF_SIZE];

int main(int argc, char* argv[]){
	int fd;
	char option;
	printf("Welcome to the character device demo!\n");
	
	fd = open("/dev/ccdev", O_RDWR);
	if(fd < 0){
		printf("Unable open device file\n");
		return 0;
	}
	
	while(1){
		printf("***Chose your option***\n");
		printf("   1 write\n");
		printf("   2 read\n");
		printf("   3 exit\n");
		scanf(" %c", &option);
		printf("your option - %c\n", option);
		switch(option){
			case '1' : //write
			{	 
				 printf("Enter string\n");
				 scanf(" %[^\t\n]", write_buff);
				 printf("your string - %s\n", write_buff);
				 write(fd, write_buff, BUFF_SIZE);
				 break;
			}
			case '2' : //read
			{
				printf("Read string\n");
				read(fd, read_buff, BUFF_SIZE);
				printf("Readed str : %s\n\n", read_buff);
				break;
			}
			case '3' : //close
			{
				close(fd);
				exit(1);
			}

		}
	}
	close(fd);
}
