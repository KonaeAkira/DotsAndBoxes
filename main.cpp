#include <cstdio>
#include <cstdlib>
#include "game.h"
//#include "engine.h"
using namespace std;

int main()
{
	board game(3, 3);
	while (!game.end)
	{
		int m = game.height(), n = game.width();
		if (!game.turn)
		{
			// PC
			generate_move:
			int x = rand() % (n * (m + 1) + m * (n + 1));
			if (game.check_valid(x))
				game.move(x);
			else
				goto generate_move;
		}
		else
		{
			int x;
			printf("Enter move: ");
			get_move:
			scanf("%d", &x);
			if (game.check_valid(x))
				game.move(x);
			else
				goto generate_move;
		}
		game.print();
	}
	return 0;
}