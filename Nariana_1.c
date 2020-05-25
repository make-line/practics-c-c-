#define _DEFAULT_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>

int main(){
	struct dirent **namelist;
	int n;
	n = scandir(".", &namelist, NULL, alphasort);
	if (n == -1) {
		perror("scandir");
	}
	else{
		struct stat buff;
		char newFileName[]ls  = "zero<       ";
		int count=1;
		while(n--){
			int l;
			l = stat(namelist[n]->d_name,&buff);
			if (l == -1) {
				perror("stat");
			}
			else{
				if(!buff.st_size){
					int i = 5;
					int z = count;					
					while (z != 0) {
						newFileName[10-i] = z%10+48;
						z = z / 10;
						i--;
					}
					newFileName[10-i] = 62;
					l = rename(namelist[n]->d_name,newFileName); 
					if (l == -1) {
						perror("rename");
					}
					else {
						count++;
						free(namelist[n]);
					}
				}
			}
		}
	}
        free(namelist);
	return 0;
}
