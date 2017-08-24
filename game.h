#ifndef GAME_H
#define GAME_H

#include <cstdio>
#include <cstring>
#include <iostream>

#define _WIN32_WINNT 0x0500
#include <windows.h>
using namespace std;

// game settings
const int game_height = 3, game_width = 3,
		  max_moves = game_height * (game_width + 1) + game_width * (game_height + 1);

// character set
const char fill_char[2] = {176, 178},
		   draw_horizontal = 254,
		   draw_vertical = 219;

// draw settings
const int horizontal_gap = 5,
		  vertical_gap = 2,
		  margin_top = 0;
		  
int cache[game_height][game_width];
COORD origin = {1, 1};

class board
{
private:
	int id;
	char status[game_height][game_width];
public:
	int points[2];
	long long hash;
	bool turn, end;
	board()
	{
		for (int i = 0; i < game_height; ++i)
			for (int j = 0; j < game_width; ++j)
				status[i][j] = 0;
		points[0] = points[1] = 0;
		turn = end = id = 0;
		hash = 0;
	}
	board(const board &other)
	{
		for (int i = 0; i < game_height; ++i)
			for (int j = 0; j < game_width; ++j)
				status[i][j] = other.status[i][j];
		points[0] = other.points[0];
		points[1] = other.points[1];
		turn = other.turn; end = other.end;
		id = other.id; hash = other.hash;
	}
	board &operator = (const board &other)
	{
		for (int i = 0; i < game_height; ++i)
			for (int j = 0; j < game_width; ++j)
				status[i][j] = other.status[i][j];
		points[0] = other.points[0];
		points[1] = other.points[1];
		turn = other.turn; end = other.end;
		id = other.id; hash = other.hash;
		return *this;
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
	
	int generate_move();
	long long get_hash();
	void random_move();
} game;

void board::random_move()
{
	generate_move:
	int x = rand() % max_moves;
	if (check_valid(x))
		move(x);
	else
		goto generate_move;
}

void board::print()
{
	for (int i = 0; i < game_height; ++i)
	{
		for (int j = 0; j < game_width; ++j)
			printf("%d ", status[i][j]);
		printf("\n");
	}
}

void board::reset()
{
	for (int i = 0; i < game_height; ++i)
		for (int j = 0; j < game_width; ++j)
			status[i][j] = 0;
	turn = points[0] = points[1] = end = 0;
}

void board::draw()
{
	int m = game_height, n = game_width;
	printf("----------------------------------------------------------------------\n");
	printf(" %c", draw_horizontal);
	for (int j = 0; j < n; ++j)
		if (status[0][j] & 1 || status[0][j] >= 15)
			printf("%c%c%c%c%c%c", draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal);
		else printf("     %c", draw_horizontal);
	printf("  Game #%d\n", id);
	for (int i = 0; i < m; ++i)
	{
		for (int k = 0; k < 2; ++k)
		{
			if (status[i][0] & 8 || status[i][0] >= 15)
				printf(" %c", draw_vertical);
			else printf("  ");
			for (int j = 0; j < n; ++j)
				if (status[i][j] >= 15) printf("%c%c%c%c%c%c", fill_char[status[i][j] >> 4], fill_char[status[i][j] >> 4], fill_char[status[i][j] >> 4], fill_char[status[i][j] >> 4], fill_char[status[i][j] >> 4], draw_vertical);
				else if (status[i][j] & 2) printf("     %c", draw_vertical);
				else printf("      ");
			if (i == 0)
			{
				if (k == 0) printf("  %c Player 1: %d", fill_char[0], points[0]);
				else if (k == 1) printf("  %c Player 2: %d", fill_char[1], points[1]);
			}
			printf("\n");
		}
		printf(" %c", draw_horizontal);
		for (int j = 0; j < n; ++j)
			if (status[i][j] & 4 || status[i][j] >= 15) printf("%c%c%c%c%c%c", draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal, draw_horizontal);
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
	int m = game_height, n = game_width;
	int x = origin.X + j * (horizontal_gap + 1),
		y = origin.Y + i * (vertical_gap + 1);
	char c;
	c = (status[i][j] & 1 || status[i][j] >= 15)?draw_horizontal:' '; 
	for (int k = 1; k <= horizontal_gap; ++k)
	{
		COORD cursor = {x + k, y};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
		printf("%c", c);
	}
	c = (status[i][j] & 2 || status[i][j] >= 15)?draw_vertical:' ';
	for (int k = 1; k <= vertical_gap; ++k)
	{
		COORD cursor = {x + horizontal_gap + 1, y + k};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
		printf("%c", c);
	}
	c = (status[i][j] & 4 || status[i][j] >= 15)?draw_horizontal:' ';
	for (int k = 1; k <= horizontal_gap; ++k)
	{
		COORD cursor = {x + k, y + vertical_gap + 1};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
		printf("%c", c);
	}
	c = (status[i][j] & 8 || status[i][j] >= 15)?draw_vertical:' ';
	for (int k = 1; k <= vertical_gap; ++k)
	{
		COORD cursor = {x, y + k};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
		printf("%c", c);
	}
	c = (status[i][j] >= 15)?fill_char[status[i][j] >> 4]:' ';
	for (int u = 1; u <= vertical_gap; ++u)
		for (int v = 1; v <= horizontal_gap; ++v)
		{
			COORD cursor = {x + v, y + u};
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
			printf("%c", c);
		}
	
	//reset pointer
	COORD cursor = {0, m * (vertical_gap + 1) + 3 + margin_top};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
}

void board::re_draw()
{
	int m = game_height, n = game_width;
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			if (status[i][j] != cache[i][j])
				draw_box(i, j);
	COORD cursor = {n * (horizontal_gap + 1) + 16, 2 + margin_top};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
	printf("%d", points[0]);
	cursor.X = n * (horizontal_gap + 1) + 16; cursor.Y = 3 + margin_top;
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
	for (int i = 0; i < game_height; ++i)
		for (int j = 0; j < game_width; ++j)
			cache[i][j] = status[i][j];
}

void board::update(int i, int j, char v)
{
	status[i][j] = v;
}

void board::move(int x)
{
	int m = game_height, n = game_width;
	hash ^= 1 << x;
	bool bonus = 0;
	if (x < (m + 1) * n) //horizontal line
	{
		int i = x / n, j = x % n;
		if (i < m)
		{
			if (status[i][j] >= 15)
			{
				bonus = 1;
				--points[status[i][j] >> 4];
			}
			status[i][j] ^= 1;
			if (status[i][j] >= 15)
			{
				status[i][j] ^= status[i][j] & 16;
				status[i][j] |= 16 * turn;
				bonus = 1;
				++points[turn];
			}
		}
		if (i)
		{
			if (status[i - 1][j] >= 15)
			{
				bonus = 1;
				--points[status[i - 1][j] >> 4];
			}
			status[i - 1][j] ^= 4;
			if (status[i - 1][j] >= 15)
			{
				status[i - 1][j] ^= status[i - 1][j] & 16;
				status[i - 1][j] |= 16 * turn;
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
			if (status[i][j] >= 15)
			{
				bonus = 1;
				--points[status[i][j] >> 4];
			}
			status[i][j] ^= 8;
			if (status[i][j] >= 15)
			{
				status[i][j] ^= status[i][j] & 16;
				status[i][j] |= 16 * turn;
				bonus = 1;
				++points[turn];
			}
		}
		if (j)
		{
			if (status[i][j - 1] >= 15)
			{
				bonus = 1;
				--points[status[i][j - 1] >> 4];
			}
			status[i][j - 1] ^= 2;
			if (status[i][j - 1] >= 15)
			{
				status[i][j - 1] ^= status[i][j - 1] & 16;
				status[i][j - 1] |= 16 * turn;
				bonus = 1;
				++points[turn];
			}
		}
	}
	end = points[0] + points[1] == m * n;
	turn ^= !bonus;
}

bool board::check_valid(int x)
{
	int m = game_height, n = game_width;
	if (x < 0 || x >= max_moves) return 0;
	else if (x < (m + 1) * n) //horizontal line
	{
		int i = x / n, j = x % n;
		if (i < m)
		{
			if (status[i][j] & 1 || status[i][j] >= 15)
				return 0;
		}
		if (i)
		{
			if (status[i - 1][j] & 4 || status[i - 1][j] >= 15)
				return 0;
		}
	}
	else //vertical line
	{
		x -= (m + 1) * n;
		int i = x / (n + 1), j = x % (n + 1);
		if (j < n)
		{
			if (status[i][j] & 8 || status[i][j] >= 15)
				return 0;
		}
		if (j)
		{
			if (status[i][j - 1] & 2 || status[i][j - 1] >= 15)
				return 0;
		}
	}
	return 1;
}

#endif