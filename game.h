#ifndef GAME_H
#define GAME_H

#include <cstdio>
#include <cstring>
#include <iostream>

#define _WIN32_WINNT 0x0500
#include <windows.h>
using namespace std;

const char fill_1 = 178,
		   fill_2 = 176,
		   draw_horizontal = 254,
		   draw_vertical = 219;
		   
const int horizontal_gap = 5,
		  vertical_gap = 2,
		  margin_top = 0;
		  
int cache[100][100];
COORD origin = {1, 1};

class board
{
private:
	int m, n, moves, id;
	char **status;
public:
	int points[2];
	long long hash;
	bool turn, end;
	board(int x, int y)
	{
		m = x; n = y;
		status = new char*[m];
		for (int i = 0; i < m; ++i)
		{
			status[i] = new char[n];
			for (int j = 0; j < n; ++j)
				status[i][j] = 0;
		}
		points[0] = points[1] = 0;
		turn = end = moves = id = 0;
		hash = 0;
	}
	board(const board &other)
	{
		m = other.m; n = other.n;
		status = new char*[m];
		for (int i = 0; i < m; ++i)
		{
			status[i] = new char[n];
			for (int j = 0; j < n; ++j)
				status[i][j] = other.status[i][j];
		}
		points[0] = other.points[0];
		points[1] = other.points[1];
		turn = other.turn; end = other.end;
		moves = other.moves;
		id = other.id;
		hash = other.hash;
	}
	board &operator = (const board &other)
	{
		if (m != other.m || n != other.n)
		{
			this->~board();
			m = other.m; n = other.n;
			status = new char*[m];
			for (int i = 0; i < m; ++i)
			{
				status[i] = new char[n];
				for (int j = 0; j < n; ++j)
					status[i][j] = other.status[i][j];
			}
		}
		else
			for (int i = 0; i < m; ++i)
				for (int j = 0; j < n; ++j)
					status[i][j] = other.status[i][j];
		points[0] = other.points[0];
		points[1] = other.points[1];
		turn = other.turn; end = other.end;
		moves = other.moves;
		id = other.id;
		hash = other.hash;
		return *this;
	}
	~board()
	{
		for (int i = 0; i < m; ++i)
			delete status[i];
		delete status;
	}
	
	void print();
	void draw();
	void re_draw();
	void draw_box(int i, int j);
	void save_cache();
	void reset();
	
	void update(int i, int j, char v);
	void move(int x);
	bool check_valid(int x);
	
	int height();
	int width();
	
	int generate_move();
	long long get_hash();
} game(3, 3);

void board::print()
{
	for (int i = 0; i < m; ++i)
	{
		for (int j = 0; j < n; ++j)
			printf("%d ", status[i][j]);
		printf("\n");
	}
}

void board::reset()
{
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			status[i][j] = 0;
	turn = points[0] = points[1] = moves = end = 0;
}

void board::draw()
{
	printf("----------------------------------------------------------------------\n");
	printf(" %c", draw_horizontal);
	for (int j = 0; j < n; ++j)
		if (status[0][j] & 1 || status[0][j] > 15)
			printf("%c%c%c%c%c%c", draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal);
		else printf("     %c", draw_horizontal);
	printf("  Game #%d\n", id);
	for (int i = 0; i < m; ++i)
	{
		for (int k = 0; k < 2; ++k)
		{
			if (status[i][0] & 8 || status[i][0] > 15)
				printf(" %c", draw_vertical);
			else printf("  ");
			for (int j = 0; j < n; ++j)
				if (status[i][j] == 16) printf("%c%c%c%c%c%c", fill_1, fill_1, fill_1, fill_1, fill_1, draw_vertical);
				else if (status[i][j] == 17) printf("%c%c%c%c%c%c", fill_2, fill_2, fill_2, fill_2, fill_2, draw_vertical);
				else if (status[i][j] & 2) printf("     %c", draw_vertical);
				else printf("      ");
			if (i == 0)
			{
				if (k == 0) printf("  Player 1: %d", points[0]);
				else if (k == 1) printf("  Player 2: %d", points[1]);
			}
			printf("\n");
		}
		printf(" %c", draw_horizontal);
		for (int j = 0; j < n; ++j)
			if (status[i][j] & 4 || status[i][j] > 15) printf("%c%c%c%c%c%c", draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal);
			else printf("     %c", draw_horizontal);
		if (i == 0)
		{
			if (!end) printf("  Next move: Player %d", (int)turn + 1);
			else if (points[0] != points[1]) printf("  Player %d wins!", (points[0] > points[1])?1:2);
			else printf("  Draw!");
		}
		printf("\n");
	}
	printf("----------------------------------------------------------------------\n");
}

void board::draw_box(int i, int j)
{
	int x = origin.X + j * (horizontal_gap + 1),
		y = origin.Y + i * (vertical_gap + 1);
	if (status[i][j] & 1 || status[i][j] > 15)
		for (int k = 1; k <= horizontal_gap; ++k)
		{
			COORD cursor = {x + k, y};
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
			printf("%c", draw_horizontal);
		}
	if (status[i][j] & 2 || status[i][j] > 15)
		for (int k = 1; k <= vertical_gap; ++k)
		{
			COORD cursor = {x + horizontal_gap + 1, y + k};
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
			printf("%c", draw_vertical);
		}
	if (status[i][j] & 4 || status[i][j] > 15)
		for (int k = 1; k <= horizontal_gap; ++k)
		{
			COORD cursor = {x + k, y + vertical_gap + 1};
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
			printf("%c", draw_horizontal);
		}
	if (status[i][j] & 8 || status[i][j] > 15)
		for (int k = 1; k <= vertical_gap; ++k)
		{
			COORD cursor = {x, y + k};
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
			printf("%c", draw_vertical);
		}
	if (status[i][j] > 15)
	{
		for (int u = 1; u <= vertical_gap; ++u)
			for (int v = 1; v <= horizontal_gap; ++v)
			{
				COORD cursor = {x + v, y + u};
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
				printf("%c", (status[i][j] == 16)?fill_1:fill_2);
			}
	}
	
	//reset pointer
	COORD cursor = {0, m * (vertical_gap + 1) + 3 + margin_top};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
}

void board::re_draw()
{
	// 12
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			if (status[i][j] != cache[i][j])
				draw_box(i, j);
	COORD cursor = {n * (horizontal_gap + 1) + 14, 2 + margin_top};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
	printf("%d", points[0]);
	cursor.X = n * (horizontal_gap + 1) + 14; cursor.Y = 3 + margin_top;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
	printf("%d", points[1]);
	cursor.X = n * (horizontal_gap + 1) + 2; cursor.Y = 4 + margin_top;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
	if (!end) printf("  Next move: Player %d", (int)turn + 1);
	else if (points[0] != points[1]) printf("  Player %d wins!         ", (points[0] > points[1])?1:2);
	else printf("  Draw!                      ");
	
	//reset pointer
	cursor.X = 0; cursor.Y = m * (vertical_gap + 1) + 3 + margin_top;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
}	

void board::save_cache()
{
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			cache[i][j] = status[i][j];
}

void board::update(int i, int j, char v)
{
	status[i][j] = v;
}

void board::move(int x)
{
	//printf("Last move: %d\n", x);
	hash |= 1 << x;
	bool bonus = 0;
	if (x < (m + 1) * n) //horizontal line
	{
		int i = x / n, j = x % n;
		if (i < m)
		{
			status[i][j] |= 1;
			if (status[i][j] == 15)
			{
				status[i][j] = 16 + turn;
				bonus = 1;
				++points[turn];
			}
		}
		if (i)
		{
			status[i - 1][j] |= 4;
			if (status[i - 1][j] == 15)
			{
				status[i - 1][j] = 16 + turn;
				bonus = 1;
				++points[turn];
			}
		}
	}
	else //vertical line
	{
		x -= (m + 1) * n;
		int i = x / (n + 1), j = x % (n + 1);
		if (j < n)
		{
			status[i][j] |= 8;
			if (status[i][j] == 15)
			{
				status[i][j] = 16 + turn;
				bonus = 1;
				++points[turn];
			}
		}
		if (j)
		{
			status[i][j - 1] |= 2;
			if (status[i][j - 1] == 15)
			{
				status[i][j - 1] = 16 + turn;
				bonus = 1;
				++points[turn];
			}
		}
	}
	end = (++moves == m * (n + 1) + n * (m + 1)) || (points[0] > m * n >> 1) || (points[1] > m * n >> 1);
	turn ^= !bonus;
	hash ^= (!bonus) << 31;
}

bool board::check_valid(int x)
{
	if (x < 0 || x >= n * (m + 1) + m * (n + 1)) return 0;
	if (x < (m + 1) * n) //horizontal line
	{
		int i = x / n, j = x % n;
		if (i < m)
		{
			if (status[i][j] & 1 || status[i][j] > 15)
				return 0;
		}
		if (i)
		{
			if (status[i - 1][j] & 4 || status[i - 1][j] > 15)
				return 0;
		}
	}
	else //vertical line
	{
		x -= (m + 1) * n;
		int i = x / (n + 1), j = x % (n + 1);
		if (j < n)
		{
			if (status[i][j] & 8 || status[i][j] > 15)
				return 0;
		}
		if (j)
		{
			if (status[i][j - 1] & 2 || status[i][j - 1] > 15)
				return 0;
		}
	}
	return 1;
}

int board::height()
{
	return m;
}

int board::width()
{
	return n;
}

#endif