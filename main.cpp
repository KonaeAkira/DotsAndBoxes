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
	
	system("cls");
	//import_data();
	
	board clone(game);
	thread producer(evaluate, ref(clone), 1, 0);
	/*
	producer.join();
	export_data();
	return 0;
	*/
	system("cls");
	game.draw();
	
	while (!game.end)
	{
		int x;
		game.save_cache();
		if (game.turn)
		{
			x = game.generate_move();
			mtx.lock();
			game.move(x);
			mtx.unlock();
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
					mtx.lock();
					game.move(x);
					mtx.unlock();
				}
				else printf("Invalid input!\n");
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
	//export_data();
	return 0;
}