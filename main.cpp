#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <conio.h>

#include <thread>

#include "game.h"
#include "engine.h"
using namespace std;

char dump[1000];

int main()
{
	srand(time(NULL));
	
	board clone(game);
	thread producer(evaluate, ref(game), 1, 0);
	
	system("cls");
	game.draw();
	
	while (!game.end)
	{
		int x;
		game.save_cache();
		if (game.turn)
		{
			// PC
			x = game.generate_move();
			game.move(x);
		}
		else
		{
			printf("Enter move: ");
			if (scanf("%d", &x) == 1)
			{
				if (x == -1) return 0;
				else if (game.check_valid(x))
				{
					printf("                \n");
					game.move(x);
				}
				else
				{
					printf("Invalid input!\n");
					game.random_move();
				}
			}
			else
			{
				scanf("%s", dump);
				printf("Invalid input!\n");
			}
		}
		game.re_draw();
		//getch();
	}
	producer.join();
	return 0;
}