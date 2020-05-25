// Заменяет пробел в имени файла на _ (потоки)
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct Node // Структура списка
{
	int id;
	struct Node* next;
} Node;

void push(Node** head, int data) // Вставка элемента в начало списка
{ 
	Node* tmp = (Node*)malloc(sizeof(Node));
	if (tmp != NULL)
	{
		tmp->id = data;
		tmp->next = (*head);
		(*head) = tmp;
	}
	else
	{
		printf("Error! memory not allocated.");
		exit(0);
	}
}

Node* getLast(Node* head) // Получить последний элемент списка
{
	if (head == NULL) 
	{
		return NULL;
	}
	while (head->next)
	{
		head = head->next;
	}
	return head;
}

void pushBack(Node* head, int value) // Вставка элемента в конец списка
{
	Node* last = getLast(head);
	Node* tmp = (Node*)malloc(sizeof(Node));
	if (tmp != NULL)
	{
		tmp->id = value;
		tmp->next = NULL;
		last->next = tmp;
	}
	else
	{
		printf("Error! memory not allocated.");
		exit(0);
	}
}

void PrintLinkedList(const Node* head) // Вывод элементов списка
{
	while (head)
	{
		printf(" -> %d <- \n", head->id);
		head = head->next;
	}
}

void DeleteList(Node** head) // Удаление списка
{
	if (*head != NULL)
	{
		Node* prev = NULL;
		while ((*head)->next)
		{
			prev = (*head);
			*head = (*head)->next;
			free(prev);
		}

		free(*head);
		*head = NULL;
	}
}



int main()
{
	Node* head = NULL;		// Список потоков
	char way[4095];			// Полное имя файла
	char path[4095];		// Старое полное имя файла
	char fileName[4095];	// Имя файла
	char newName[4096];		// Новое полное имя файла
		
	strcpy(way, "/home/yavay/Folder3/"); // Папка, в которой происходит поиск
	int isRename = 0; // Условие переименования

	struct dirent** namelist; 
	struct stat sb; 

	int n;

	/*
	int scandir(const char *dir, struct dirent ***namelist, int(*select)(const struct dirent *), int(*compar)(const struct dirent **, const struct dirent **));
	Функция scandir() производит поиск элементов в каталоге dir, посылая каждому элементу вызов select(). 
	Элементы, которым select() возвращает ненулевое значение, записываются в строках и размещаются в памяти при 
	помощи malloc(); они сортируются посредством функции qsort() и функции сравнения compar(), а затем накапливаются 
	в массиве namelist , который размещается в памяти функцией malloc(). Если select равен NULL, то выбираются все записи.
	Функции alphasort() и versionsort() могут быть использованы как функции сравнения compar(). 
	alphasort() упорядочивает записи с помощью strcoll(3)
	versionsort() использует strverscmpcmp(3) для строк (*a)->d_name и (*b)->d_name. 
	*/
	n = scandir(way, &namelist, 0, alphasort);

	if (n < 0)
	{
		perror("scandir");
	}
	else
	{
		while (n--) // Проход по всем файлам
		{
			// Присваивание значений переменным path, newName, fileName
			strcpy(fileName, namelist[n]->d_name); // Имя файла
			strcpy(path, way);
			strcat(path, "/");
			strcpy(newName, path);
			strcat(path, fileName);	


			if (stat(path, &sb) == 0)
			{
				if (S_ISREG(sb.st_mode))
				{
					// Цикл ищет пробел в названии файла
					for (int i = 0; i < strlen(fileName); i++)
					{
						printf("|%c", fileName[i]);
						if (fileName[i] == ' ')
						{
							fileName[i] = '_';
							isRename = 1;
						}
					}

					// Проверка выполнения условия переименования
					if (isRename == 1)
					{
						printf("\nRENAME");
						strcat(newName, fileName); //
						printf("\nOld: %s \n", path);
						printf("\nNew: %s \n", newName);
						if (rename(path, newName)) perror("rename");
					}

				}
				else
				{
					// Найдена папка
					if ((S_ISDIR(sb.st_mode)) && (strcmp(fileName, ".")) && (strcmp(fileName, "..")))
					{
						pid_t p = fork(); // Новый процесс


						if (p > 0) // родительский
						{
							printf("Create child process: %d\n\n", p);
							if (head == NULL)
							{
								push(&head, p); // Если в списке нет процессов, добавляем процесс в начало списка
							}
							else
							{
								pushBack(head, p); // Если в списке процессы есть, то добавляем процесс в конец
							}
						}
						else if (p == 0) // дочерний
						{
							strcat(way, "/");
							strcat(way, fileName);

							/*
							Функция free освобождает место в памяти. Блок памяти, ранее выделенный с помощью вызова malloc, calloc 
							или realloc освобождается. То есть освобожденная память может дальше  использоваться программами или ОС.
							*/
							free(namelist);
							DeleteList(&head);

							n = scandir(way, &namelist, 0, alphasort);
							if (n < 0)
							{
								perror("scandir");
							}
						}
						else
						{
							printf("%s\n", strerror(errno));
						}
					}
				}
			}
		}

		int status;
		while (head != NULL)
		{
			// Проверка завершения потока и записка результата в status
			if (waitpid(head->id, &status, 0) == -1)
			{
				printf("%s\n", strerror(errno));
			}
			else
			{
				// WIFEXITED не равно нулю, если дочерний процесс успешно завершился
				if (!WIFEXITED(status))
				{
					printf("Process finished uncorrectly!");
				}
			}
			printf("Try to waitpid: %d \n", head->id); // Вывод, завершение какого потока ожидается
			head = head->next;
		}

		DeleteList(&head);
	}

	free(namelist);
}

