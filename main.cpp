#include <cstdio>
#include <cstdlib>
#include "game.h"
using namespace std;

board game(3, 3);

void random_move()
{
	int m = game.height(), n = game.width();
	generate_move:
	int x = rand() % (n * (m + 1) + m * (n + 1));
	if (game.check_valid(x))
		game.move(x);
	else
		goto generate_move;
}

int main()
{
	system("cls");
	game.draw();
	while (!game.end)
	{
		game.save_cache();
		if (!game.turn)
		{
			// PC
			random_move();
		}
		else
		{
			int x;
			printf("Enter move: ");
			scanf("%d", &x);
			if (x == -1) return 0;
			else if (game.check_valid(x))
				game.move(x);
			else random_move();
		}
		game.re_draw();
	}
	return 0;
}