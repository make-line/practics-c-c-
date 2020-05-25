//заменяет пробел в названии файла на _
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <errno.h>

void Nar3(const char * dirName)
{
	DIR *dir;
	
	struct dirent *ent;
	struct stat sb;
	
	char fileName[4095];	// Название файла или папки
	char way[4095];			// Полное имя файла
	char path[4095];		// Старое имя файла (дирректория + имя_файла)
	char newName[4096];		// Новое имя файла (дирректория + новое_имя_файла)

	int isRename;
	
	
	if ((dir = opendir (dirName)) != NULL) 
	{
		errno=0;
		while ( ((ent = readdir (dir)) != NULL) && !errno) 
		{
			printf("\n- - - - - - - - - - - - \n");
			
			isRename = 0; // Условие, нужно ли менять имя файла
			
			strcpy(fileName,ent->d_name);
			
			char way[4095]; // Получить имя найденного файла 
			strcpy(way,dirName);
			strcat(way, fileName);
			
			
			printf("_DIRNAME|---> %s <---|\n", dirName);			
			printf("FILENAME|~+~> %s <~+~|\n", fileName);
			printf("_____WAY|~~~> %s <~~~|\n", way);
			
			
			if(!stat(way, &sb))
			{
				
				if (S_ISREG(sb.st_mode))
				{
					printf ("%s\n", ent->d_name);
					
					strcpy(path, way);
					//strcat(path, "/");
					strcpy(newName, dirName);
					//strcat(path, fileName);

					// цикл находит пробел в названии файла
					for (int i = 0; i < strlen(fileName); i++)
					{
						printf("|%c", fileName[i]); // посимвольный вывод названия файла (для отладки)
						if (fileName[i] == ' ')
						{
							fileName[i] = '_';
							isRename = 1;
						}
					}

					// Переименование
					if (isRename == 1)
					{
						printf("RENAME ! ! !\n");						
						printf("%s\n", ent->d_name);

						strcat(newName, fileName);
						printf("\nOld: %s \n", path);
						printf("\nNew: %s \n", newName);
						if (rename(path, newName)) perror("rename");

					}
				}
				else if(S_ISDIR(sb.st_mode) && strcmp(fileName, ".") && strcmp(fileName, "..")) // Найдена папка
				{
					printf("RECURSE ! ! !\n");				
					
					// Получить полный путь к папке
					char nextDir[4095];					
					strcpy(nextDir, way);
					strcat(nextDir, "/");
										
					
					printf ("GOTO===> %s\n", nextDir);
					
					// Зайти в папку, метод начнётся заново
					Nar3(nextDir);
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
		// Закрыть текщую папку
		closedir(dir);
	} 
	else 
	{
		perror("opendir");
	}
	
	
}

int main()
{

	Nar3("/home/yavay/Folder3/");
	
	return 0;
}




