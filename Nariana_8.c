// Получение длины имени файла с самым коротким именем
#include <stdio.h> 
#include <dirent.h> 
#include <sys/stat.h> 
#include <errno.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 



// Структура-элемент списка
struct list_int
{
    pid_t num;
    int f[2];
    struct list_int* next;
};

// Сущность папки
struct MinFile 
{
    int count; // длина имени
    char* name; // название
};

// Добавляем в список процессы, если список не пуст,
// то добавляем в конец. Иначе выделяем память
// и добавляем 1й элемент
void addList(struct list_int** p, pid_t proc, int f[2])
{
    struct list_int* pr = NULL;

    if (*p == NULL) {
        pr = malloc(sizeof(struct list_int));
        if (pr) {
            *p = pr;
            pr->num = proc;
            pr->f[0] = f[0];
            pr->next = NULL;
        }
    }
    else {
        pr = *p;
        while (pr->next != NULL) pr = pr->next;
        pr->next = malloc(sizeof(struct list_int));
        if (pr->next) {
            pr->next->num = proc;
            pr->next->f[0] = f[0];
            pr->next->next = NULL;
        }
    }
}

// Удалить элементы списка
void delList(struct list_int** p)
{
    struct list_int* points;
    while (*p != NULL)
    {
        points = *p;
        *p = (*p)->next; 
        free(points);
    }
    *p = NULL;
}

void rec(char* str)
{
	DIR* dir = NULL;
	struct list_int* proc = NULL; // список процессов
	char* buf = malloc(sizeof(char)*strlen(str) + 1); // выделить необходимое кол-во памяти
	int count = 0; // кол-во файлов в папке
	int pepe;
	
	char *fullName = NULL; // полное имя файла
	 
	struct MinFile name; // сама папка
	int pipefd[2];
	pid_t l = 1;
	if (buf)
	{
		strcpy(buf, str);
		struct dirent* ent = NULL;
		errno = 0;
		name.count = 0; // обнуляем длину имени
		if ((dir = opendir(buf)) != NULL) // открыть каталог
		{
			while ((ent = readdir(dir)) != NULL) // читаем все файлы каталога
			{
				char* file = NULL; // выделяем память для названия объекта(файл иди каталог)
				file = malloc(sizeof(char) * strlen(str) + sizeof(char) * strlen(ent->d_name) + 2);
				if (file != NULL)
				{
					strcpy(file, str); // получить имя текущей папки
					strcat(file, "/"); 
					strcat(file, ent->d_name); // добавить имя найденного объекта
					struct stat s;

					if (stat(file, &s) == -1) printf("problem files %s\n", strerror(errno));
					else
					{
						// Найден новый каталог
						if (S_ISDIR(s.st_mode) && (strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0))
						{
							if (pipe(pipefd) == -1) 
								printf("Error: %s %s\n", str, strerror(errno));
							
							// Создать новый процесс
							pid_t i = fork();
							if (i > 0) // процесс родитель
							{
								close(pipefd[1]);
								addList(&proc, i, pipefd); // добавить процесс в список
							}
							else if (i == 0) // процесс потомок
							{
								l = i;
								close(pipefd[0]);
								pepe = pipefd[1];							
								delList(&proc);
								
								if (proc == NULL)
								{
									free(str);
									str = malloc(sizeof(char)*strlen(file)+1);
									name.count = 0;
									if (str)
									{
										strcpy(str,file);
										closedir(dir);
										if ((dir = opendir(str)) == NULL) 
											printf("catalog open error: %s %s\n", str, strerror(errno));									
									}
								}
							}
							else printf("Error: %s %s\n", str, strerror(errno));							
						}
						else if (S_ISREG(s.st_mode)) // найден новый файл
						{
							// сохраняем имя и длину имени файла, если это первый найденный файл
							// или если длина его имени короче предыдущего
							if(count == 0 || count > strlen(ent->d_name))
							{
								count = strlen(ent->d_name); // обновляем длину
								fullName = malloc(sizeof(char)*strlen(file)+1); // выделяем память под новое имя
								strcpy(fullName,file); // обновляем имя
								
							}
							
						}
					}
					free(file);
				}
				errno = 0;
			}
			if (errno != 0) printf("problem file %s\n", strerror(errno));
			closedir(dir);
			free(buf);
		}
		else printf("catalog open error: %s %s\n", str, strerror(errno));		
	}

	if(fullName != NULL) // в каталоге найден хотя бы 1 файл
		name.name = malloc(sizeof(char)*strlen(fullName) + 1); // выделяем память под сохранённое имя файла
	else name.name = malloc(sizeof(char) + 1); // иначе выделяем память для пустой строки
	
	
	if (name.name)
	{
		if(fullName != NULL)
			strcpy(name.name,fullName); // сохраняем имя найденного файла с минимальной длиной имени
		else strcpy(name.name,""); // иначе сохраням пустую строку
		name.count = count;

		if (proc != NULL)
		{			
			struct list_int* points = proc;
			while (points != NULL) // просматриваем список и находим минимальное имя файла
			{
				int status; 
				if (waitpid(points->num, &status, 0) == -1) printf("%s\n", strerror(errno));
				else if (WIFEXITED(status) == 0) printf("error close process");	
				struct MinFile k;
				int size;																				
				if (read(points->f[0], &size, sizeof(int)) != -1)
				{
					k.name = malloc(sizeof(char) * (size + 1));
					if (k.name)
					{
						if (read(points->f[0], k.name, size + 1) != -1)
						{
							if (read(points->f[0], &k.count, sizeof(int)) != -1)
							{
								// если в текущем каталоге не найдено файлов
								// или длина имени найденного файла больше ранее найденного
								if (name.count==0 || k.count < name.count)
								{
									free(name.name); // обновляем минимальное имя и идём дальше
									name.name = malloc(sizeof(char) * strlen(k.name) + 1);
									if (name.name)
									{
										strcpy(name.name, k.name);
										name.count = k.count;
									}
								}
							}
							else printf(" error: %s\n", strerror(errno));
							free(k.name);
						}
						else printf(" error: %s\n", strerror(errno));
					}
				}
				else printf(" error: %s\n", strerror(errno));
				points = points->next;
				errno = 0;
			}
			delList(&proc);
		}
		if (l == 0)
		{
			int size = sizeof(char) * strlen(name.name);			
			if (write(pepe, &size , sizeof(int)) != -1)
			{
				if (write(pepe, name.name, size + 1) != -1)
				{
					if (write(pepe, &name.count, sizeof(int)) == -1) 
						printf(" error: %s\n", strerror(errno));
				}
				else printf(" error: %s\n", strerror(errno));
			}
			else printf(" error: %s\n", strerror(errno));
			
		}
		else printf("Папка: %s\n", name.name);
		free(name.name);
		free(str);
	}
}



int main()
{
	char* myMinFile = malloc(sizeof(char) * strlen("./Folder2") + 1);
	
    if (myMinFile)
	{
		strcpy(myMinFile,"./Folder2");
		rec(myMinFile);
	}
	
	return 0;
}
