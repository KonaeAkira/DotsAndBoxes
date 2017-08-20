#ifndef GAME_H
#define GAME_H

#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

const char vertical_line = '!',
		   horizontal_line = '_',
		   corner_marker = '.',
		   fill_1 = 223,
		   fill_2 = 177;

class board
{
private:
	int m, n, moves;
	char **status;
public:
	int points[2];
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
		turn = end = moves = 0;
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
		return *this;
	}
	~board()
	{
		for (int i = 0; i < m; ++i)
			delete status[i];
		delete status;
	}
	void print();
	void update(int i, int j, char v);
	void move(int x);
	bool check_valid(int x);
	int height();
	int width();
};

void board::print()
{
	printf("\n");
	for (int j = 0; j < n; ++j)
		if (status[0][j] & 1) printf("%c%c", corner_marker, horizontal_line);
		else printf("%c ", corner_marker);
	printf("%c\n", corner_marker);
	for (int i = 0; i < m; ++i)
	{
		if (status[i][0] & 8) printf("%c", vertical_line);
		else printf("%c", corner_marker);
		for (int j = 0; j < n; ++j)
		{
			if (status[i][j] == 16) printf("%c%c", fill_1, corner_marker);
			else if (status[i][j] == 17) printf("%c%c", fill_2, corner_marker);
			else
			{
				if (status[i][j] & 4) printf("%c", horizontal_line);
				else printf(" ");
				if (status[i][j] & 2) printf("%c", vertical_line);
				else printf("%c", corner_marker);
			}
		}
		if (i == 0) printf("  Player 1: %d", points[0]);
		else if (i == 1) printf("  Player 2: %d", points[1]);
		else if (i == 2)
		{
			if (end)
			{
				if (points[0] != points[1])
					printf("  Player %d wins!", (points[0] > points[1])?1:2);
				else
					printf("  Draw!");
			}
			else
				printf("  Next move: Player %d", (int)turn + 1);
		}
		printf("\n");
	}
	printf("\n");
}

void board::update(int i, int j, char v)
{
	status[i][j] = v;
}

void board::move(int x)
{
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