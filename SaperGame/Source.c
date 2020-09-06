#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

#define LEFT 0x4B
#define RIGHT 0x4D
#define DOWN 0x50
#define UP 0x48
#define ENTER 0x0D
#define ESC 0x1B

#define _CRT_SECURE_NO_WARNINGS

typedef struct field
{
	int value; //9-Bomb
	int covered; //1 covered, 0-uncovered
}field;

typedef struct player
{
	char name[30];
	char surname[30];
	char nick[30];
	double best_score;
}player;

typedef struct stos
{
	struct stos* previous;
	player data;
	struct stos* next;
}stos;



void Menu(stos* list);
void ClrBuff();
void GetInt(int* x);
void GetFloat(float* x);
void ClrScreen();
void ShowList(stos* list);
void SaveList(stos* list);
void ShowBestScore(stos* list);
void SortList(stos* list);
void ChangeWithNext(stos* active);
void Saper(stos* list);
void GenerateBoard(field** board, int size);
void PlaceBomb(field** board, int size, int pos_x, int pos_y);
void DisplayBoard(field** board, int size, int pos_x, int pos_y);
void UncoverField(field** board, int size, int pos_x, int pos_y);
void InitGameMenu();

player AddPlayer();

stos* Push(stos* list);
stos* Pop(stos* list);
stos* GoToFirst(stos* active);
stos* GetList(stos* list);
stos* ChoosePlayer(stos* list);

int CheckIfWin(field** board, int size);
int ChooseSize();
int InitGame(stos** list);

int main()
{
	stos* list = NULL;
	int number;
	srand((unsigned)time(NULL));
	list = GetList(list);
	if (InitGame(&list) == 0)
	{
		SaveList(list);
		free(list);
		return 0;
	}
	while (1)
	{
		Menu(list);
		GetInt(&number);
		switch (number)
		{
		case 1://new game
			Saper(list);
			ClrScreen();
			break;
		case 2://statistics
			SortList(list);
			ShowBestScore(list);
			ClrScreen();
			break;
		case 3://log out
			ClrScreen();
			if (InitGame(&list) == 0)
			{
				SaveList(list);
				free(list);
				return 0;
			}
			ClrScreen();
			break;
		case 4://Delete
			printf("Are you sure??\n1.Yes\n2.No\n");
			GetInt(&number);
			if (number == 1)
			{
				list = Pop(list);
				if (InitGame(&list) == 0)
				{
					SaveList(list);
					free(list);
					return 0;
				}
			}
			break;
		case 0://exit
			SaveList(list);
			free(list);
			return 0;
			break;
		default:
			printf("Type vaild value\n");
			break;
		}
	}
	return 0;
}

void InitGameMenu()
{
	printf("1.New player\n");
	printf("2.Log in\n");
	printf("0.exit\n");
}
void Menu(stos* list)
{
	printf("You are logged as: %s\n", list->data.nick);
	printf("1.New Game\n");
	printf("2.Statistics\n");
	printf("3.Log out\n");
	printf("4.Delete Account\n");
	printf("0.Exit\n");
}
void ClrBuff()
{
	while (getchar() != '\n')
		;
}
void GetInt(int* x)
{
	if (scanf_s("%d", x) != 1)
	{
		printf("zle podales wartosc sprobuj jeszcze raz\n");
		getchar();
		GetInt(x);
	}
	ClrBuff();
}
void GetFloat(float* x)
{
	if (scanf_s("%f", x) != 1)
	{
		printf("zle podales wartosc sprobuj jeszcze raz\n");
		getchar();
		GetFloat(x);
	}
	ClrBuff();
}
void ClrScreen()
{
	system("cls");
}
void ShowList(stos* list)
{
	stos* element = list;
	int i = 1;
	element = GoToFirst(element);
	while (element != NULL)
	{
		printf("%d. %s\n", i, element->data.nick);
		i = i + 1;
		element = element->next;
	}
}
void SaveList(stos* list)
{
	FILE* file = NULL;
	stos* element = list;
	file = fopen("List_of_players.csv", "w");
	if (file != NULL)
	{
		setlocale(LC_ALL, "Polish");
		element = GoToFirst(element);
		while (element != NULL)
		{
			fprintf(file, "%s;%s;%s;%lf\n", element->data.name, element->data.surname, element->data.nick, element->data.best_score);
			element = element->next;
		}
	}
	fclose(file);
}
void ShowBestScore(stos* list)
{
	stos* element = list;
	int i = 1;
	element = GoToFirst(element);
	if (element != NULL);
	while (element != NULL)
	{

		printf("%d. %s %lf\n", i, element->data.nick, element->data.best_score);
		i = i + 1;
		element = element->next;
	}
	system("pause");
}
void SortList(stos* list)
{
	int licznik = 0;
	while (1)
	{
		list = GoToFirst(list);
		licznik = 0;
		while (list->next != NULL)
		{
			if (list->data.best_score > list->next->data.best_score)
			{
				ChangeWithNext(list);
				licznik = licznik + 1;
			}
			else
			{
				list = list->next;
			}
		}
		if (licznik == 0)
			break;
	}
}
void ChangeWithNext(stos* active)
{
	if (active->previous != NULL)
		active->previous->next = active->next;
	active->next->previous = active->previous;
	active->previous = active->next;
	active->next = active->previous->next;
	active->previous->next = active;
	if (active->next != NULL)
		active->next->previous = active;
}
void Array2dMalloc(field*** tab, int n_wiersze, int n_kolumny)
{
	int i, j;
	*tab = (field**)malloc(n_wiersze * sizeof(field*));
	if (*tab != NULL)
	{
		for (i = 0; i < n_wiersze; i++)
		{
			*(*tab + i) = (field*)malloc(n_kolumny * sizeof(field));
			if (*(*tab + i) == NULL)
			{
				for (j = 0; j <= i; j++)
				{
					free(*(*tab + j));
				}
				free(*tab);
			}
		}
	}
	else
	{
		free(*tab);
	}
}
void Saper(stos* active)
{
	double time;
	clock_t start, stop;
	field** board;
	int size, end = 0, pos_x = 0, pos_y = 0;
	char c;
	size = ChooseSize();
	Array2dMalloc(&board, size, size);
	GenerateBoard(board, size);
	DisplayBoard(board, size, pos_x, pos_y);
	start = clock();
	while (end == 0)
	{
		if (_kbhit)
		{
			c = _getch();
			if (c == ENTER)
			{
				if (board[pos_x][pos_y].value == 9) //trafiles na mine
				{
					end = 2;
				}
				else
				{
					if (CheckIfWin(board, size) == 1)
						end = 1;
				}
				UncoverField(board, size, pos_x, pos_y);
			}
			if (c == UP)
			{
				if (pos_y > 0)
				{
					pos_y = pos_y - 1;
					DisplayBoard(board, size, pos_x, pos_y);
				}

			}
			if (c == DOWN)
			{
				if (pos_y < size - 1)
				{
					pos_y = pos_y + 1;
					DisplayBoard(board, size, pos_x, pos_y);
				}
			}
			if (c == LEFT)
			{
				if (pos_x > 0)
				{
					pos_x = pos_x - 1;
					DisplayBoard(board, size, pos_x, pos_y);
				}
			}
			if (c == RIGHT)
			{
				if (pos_x < size - 1)
				{
					pos_x = pos_x + 1;
					DisplayBoard(board, size, pos_x, pos_y);
				}

			}
			if (c == ESC)
			{
				return;
			}

		}
	}
	if (end == 1)
	{
		stop = clock();
		time = (stop - start) / CLOCKS_PER_SEC;
		printf("YOU WIN!!! \nYour time:%lf\n", time);
		active->data.best_score = time;
		system("pause");
	}
	if (end == 2)
	{
		stop = clock();
		time = (stop - start) / CLOCKS_PER_SEC;
		printf("YOU LOSE !! \nYour time:%lf\n", time);
		system("pause");
	}
}
void GenerateBoard(field** board, int size)
{
	int pos_x, pos_y, i, j;
	int amount_of_bombs = size;

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			board[i][j].covered = 1;
			board[i][j].value = 0;
		}
	}
	while (amount_of_bombs > 0)
	{
		pos_x = rand() % 10;
		pos_y = rand() % 10;

		if (board[pos_x][pos_y].value != 9)
		{
			PlaceBomb(board, size, pos_x, pos_y);
			amount_of_bombs = amount_of_bombs - 1;
		}
	}
}
void PlaceBomb(field** board, int size, int pos_x, int pos_y)
{
	int i, j;
	if (board[pos_x][pos_y].value != 9)
	{
		board[pos_x][pos_y].value = 9;
		for (i = -1; i < 2; i++)
		{
			for (j = -1; j < 2; j++)
			{
				if ((pos_x + j) < 0 || (pos_y + i) < 0)
					continue;
				if ((pos_x + j) > size - 1 || (pos_y + i) > size - 1)
					continue;
				if (board[pos_x + j][pos_y + i].value == 9)
					continue;
				board[pos_x + j][pos_y + i].value = board[pos_x + j][pos_y + i].value + 1;
			}
		}
	}
}
void DisplayBoard(field** board, int size, int pos_x, int pos_y)
{
	int i, j;
	ClrScreen();
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (j == pos_x && i == pos_y) //aktualkna pozycja kursora
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x02);
				printf("#");
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
				if (board[j][i].covered == 0) // pole odkryte
				{
					if (board[j][i].value == 0)   //wartosc = 0
						printf(" ");                //wyswietl spacje
					else
						printf("%d", board[j][i].value); //wyswietl wartosc 1-8

				}
				if (board[j][i].covered == 1) //pole nie odkryte
					printf("#"); //wyswietl #
			}
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
		printf("\n");
	}
}
void UncoverField(field** board, int size, int pos_x, int pos_y)
{
	int i, j;
	if (pos_x<0 || pos_x>size - 1)
		return;
	if (pos_y<0 || pos_y>size - 1)
		return;
	if (board[pos_x][pos_y].covered == 0)
		return;

	if (board[pos_x][pos_y].value != 9 && board[pos_x][pos_y].covered == 1)
		board[pos_x][pos_y].covered = 0;

	if (board[pos_x][pos_y].value != 0)
		return; // wartość > 0 wyjście

	for (i = -1; i < 2; i++)
	{
		for (j = -1; j < 2; j++)
		{
			UncoverField(board, size, pos_x + i, pos_y + j);
		}
	}
}

player AddPlayer()
{
	player new;
	new.best_score = 999999;
	printf("Enter name: ");
	scanf("%29s", new.name);
	printf("Enter surname: ");
	scanf("%29s", new.surname);
	printf("Enter nick: ");
	scanf("%29s", new.nick);
	return new;
}

stos* Push(stos* list)
{
	stos* new;
	new = malloc(sizeof(stos));
	if (new != NULL)
	{
		if (list != NULL)
		{
			list = GoToFirst(list);
			list->previous = new;
			new->previous = NULL;
			new->next = list;
		}
		else
		{
			new->previous = NULL;
			new->next = NULL;
		}
		return new;
	}
	return list;
}
stos* Pop(stos* list)
{
	if (list != NULL)
	{
		stos* element = NULL;
		if (list->previous != NULL)
		{
			list->previous->next = list->next;
			element = list->previous;
		}
		if (list->next != NULL)
		{
			list->next->previous = list->previous;
			element = list->next;
		}
		free(list);
		return element;
	}
	return list;
}
stos* GoToFirst(stos* active)
{
	if (active != NULL)
	{
		while (active->previous != NULL)
		{
			active = active->previous;
		}
	}
	return active;
}
stos* GetList(stos* list)
{
	FILE* file = NULL;
	char buffer[100];
	file = fopen("List_of_players.csv", "r");
	if (file != NULL)
	{
		setlocale(LC_ALL, "Polish");
		while (feof(file) == 0)
		{
			list = Push(list);
			fscanf(file, "%s\n", buffer);
			sscanf(buffer, "%[^;] ; %[^;] ; %[^;] ; %lf", list->data.name, list->data.surname, list->data.nick, &list->data.best_score);
		}
		//fclose(file);
	}
	return list;
}
stos* ChoosePlayer(stos* list)
{
	stos* element = list;
	int which, i = 1;
	printf("Choose your account\n");
	ShowList(element);
	GetInt(&which);
	element = GoToFirst(element);
	while (element != NULL)
	{
		if (i < which)
		{
			element = element->next;
			i = i + 1;
		}
		else
		{
			break;
		}
	}
	if (i < which)
	{
		ClrScreen();
		printf("Type valid number\n");
		list = ChoosePlayer(list);
	}
	return element;
}

int ChooseSize()
{
	int number, size = 51;
	printf("Choose size of bord:\n");
	printf("1.10x10\n");
	printf("2.20x20\n");
	printf("3.30x30\n");
	GetInt(&number);
	switch (number)
	{
	case 1:
		return 10;
		break;
	case 2:
		return 20;
		break;
	case 3:
		return 30;
		break;
	default:
		return 10;
		break;
	}
}
int CheckIfWin(field** board, int size)
{
	int i, j, win = 0, bomb = 0;
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (board[i][j].covered == 1)
			{
				bomb = bomb + 1;
			}
		}
	}
	if (bomb == 0.1 * size)
	{
		win = 1;
	}
	return win;
}
int InitGame(stos** list)
{
	int number;
	while (1)
	{
		InitGameMenu();
		GetInt(&number);
		switch (number)
		{
		case 1://new player
			*list = Push(*list);
			(*list)->data = AddPlayer();
			SortList(*list);
			ClrScreen();
			return 1;
			break;
		case 2:
			if (list == NULL)
			{
				printf("There are no acconts\n");
			}
			else
			{
				*list = ChoosePlayer(*list);
				ClrScreen();
				return 1;
			}
			ClrScreen();
			break;
		case 0:
			return 0;
			break;
		default:
			printf("Type vaild value\n");
			break;
		}
	}
}




