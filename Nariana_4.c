#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *myFile; // Файл
	
	int isError = 0; // Проверка на ошибку
	int inputNum = 0; // Сколько нужно вывести строк
	size_t lines_count = 0; // Счётчик строк
	
	
	printf("\n\n");
	if(argc == 1)
		printf("ОШИБКА! Файл для чтения не введён! \nВведите название файла и количство строк через пробел.\n");
	else if(argc == 2)
		printf("ОШИБКА! Введён только файл! \nВведите название файла и количство строк через пробел.\n");
	else if(argc == 3)
	{
		if((myFile = fopen(argv[1],"r")) == NULL) // Введённый файл не существует
		{
			perror("fopen");
		}
		
		while(!feof(myFile) && !ferror(myFile)) // Считаем кол-во строк в файле
		{
			if(fgetc(myFile)=='\n') lines_count++; 
		}
		if(ferror(myFile)) perror("error msg"); // Вывод ошибки чтения файла
		
		
		if(strlen(argv[2])<10) // Длина числа меньше 10
		{
			for(int i = 0; i<strlen(argv[2]) && !isError; i++) // Проверка на то, что 2й аргумент число
			{
				if(!isdigit(argv[2][i]))  isError = 1;
			}
			if(isError) printf("Введено не число!\n");
			else
			{
				// Для отладки
				/*
				printf("----\n");		
				printf("%ld << lines_count\n", lines_count); // Всего строк в файле
				printf("%s << argv[2]\n",argv[2]); // Введено строк для вывода
				
				printf("%d << fileStrings\n", inputNum);
				*/
				
				inputNum = atoi(argv[2]); // Перевод строки в число
				if(inputNum>0) // Введённое число больше 0
				{				
					if(inputNum<=lines_count) // Введённое число меньше, чем кол-во строк в файле
					{
						//printf("\n----\n\n START \n");
						
						char line[255];
						char *output; 
						char c;
						
						fseek(myFile, 0, SEEK_SET); // Перемещаем указатель чтения файла в начало
						
						/*
						while(!feof(myFile))
						{
							//output = malloc(sizeof(char)*strlen(line)+1);
							output = fgets(line, sizeof(line+1), myFile); // Получаем строку из файла
							lines_count--;
							if(lines_count<inputNum)
							{								
								//printf("[%ld | %d]> %s", lines_count, inputNum, output); // Выводим строку
								printf("%s", output); // Выводим строку
							}	
							free(output);					
						}	
						*/
						
						while((c = fgetc(myFile)) != EOF)
						{
							if(c == '\n') lines_count--;
							if(lines_count <= inputNum)
								printf("%c", c);
								//putchar(c);
							
						}					
					}
					else
					{
						printf("Введённое число превышает количество строк в файле (%ld)!\n", lines_count);
					}				
				}
				else
				{
					printf("Число не может быть меньше или равно 0!\n");
				}
			}
		}
		else
		{
			printf("Введенное число превышает максимальный размер!\n");
			return 0;
		}		
		
		fclose(myFile); // Закрыть файл
	}
	else if (argc>3)
		printf("ОШИБКА! Введёно слишком много файлов! \nВведите название файла и количство строк через пробел.\n");
		
	printf("\n\n");
	
	return 0;
}
