// Найти длину имени файла с самым коротким именем
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <errno.h>



int MinFileName(char *dirName)
{
	DIR *dir;
	
	struct dirent *ent;
	struct stat sb;
	
	char fileName[256];				// имя файла, найденного в текущем каталоге
	char minFileWay[256] = "";		// полное имя файла
	int len = 0;					// длина имени файла
	
	char nextDir[256] = ""; // вложенный каталог
	
	
	if ((dir = opendir (dirName)) != NULL)
	{
		errno=0;
		while ( ((ent = readdir (dir)) != NULL) && !errno) 
		{
			strcpy(fileName,ent->d_name);
			
			char way[256];
			strcpy(way,dirName);
			strcat(way, fileName);
			
			
			if(!stat(way, &sb))
			{
				if (S_ISREG(sb.st_mode)) // найден файл
				{				
					 
					if(len==0 || len>strlen(fileName))
					{
						len = strlen(fileName);
						strcpy(minFileWay, way);
					}
					
				} // найден подкаталог
				else if(S_ISDIR(sb.st_mode) && strcmp(fileName, ".") && strcmp(fileName, ".."))
				{
					// Получить путь к новому подкаталогу и входим в него
					strcpy(nextDir, way);
					strcat(nextDir, "/");
					
					int retlen = MinFileName(&nextDir); // получить минимальное имя файла из подкаталога
					
					if(len == 0 || (len > retlen && retlen>0))
					{							
						strcpy(minFileWay, nextDir); // если minFile длинее, то обновить
						len = retlen;
						
					}						
					
				}
				errno=0;
				
			}
			else
			{
				perror("stat");
			}
			
			if(errno)
			{
				perror("readdir");
			}
			
			
			
		}
		// После проверки всех файлов можно закрыть каталог
		closedir(dir);
		
	} 
	else 
	{
		perror("opendir");
		
		printf("%s --> ", dirName);
		strcpy(dirName,"Указанный каталог не найден!");
		printf("%s \n", dirName);
	}
	
	printf("From: %s \n=> %s [%d]\n", dirName, minFileWay, len);
	strcpy(dirName, minFileWay); // Вернуть новое имя
	
	return len;
}

int main()
{
	char myDir[256] = "/home/yavay/Folder2/";
	MinFileName(&myDir);
	
	printf("RESULT: %s\n", myDir);
	
	
	return 0;
}




