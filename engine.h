#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include <map>
#include <cstdlib>
#include <mutex>
using namespace std;

const bool log_engine = 0;

const int max_depth = 4,
		  inf = 1e9;
		  
struct data
{
	int value, move;
};

map <long long, data> transposition;
bool org_turn;
int start_time, end_time;

mutex mtx;

data evaluate(board &cur, bool complete_search = 0, int depth = max_depth)
{
	int m = game_height, n = game_width;
	data tmp = {cur.points[cur.turn] - cur.points[!cur.turn], -1}, best = {-inf, -1};
	
	// return current board if game ended
	if (cur.end) return tmp;
	
	while (!mtx.try_lock());
	// return if current state cannot be reached
	if (cur.hash != (cur.hash | game.hash) && complete_search)
	{
		mtx.unlock();
		board tar(game);
		evaluate(tar, 1);
		return tmp;
	}
	// check for an existing result from the producer
	map <long long, data>::iterator ptr;
	ptr = transposition.find(cur.hash);
	if (ptr != transposition.end())
	{
		mtx.unlock();
		return ptr->second;
	}
	mtx.unlock();
	
	// return current state if max_depth is reached
	if (!complete_search && !depth) return tmp;
	
	// recursive search for all children
	for (int i = 0; i < max_moves; ++i)
		if (cur.check_valid(i))
		{
			board tar(cur); tar.move(i);
			tmp = evaluate(tar, complete_search, depth - (cur.turn ^ tar.turn));
			if (cur.turn ^ tar.turn) tmp.value = -tmp.value;
			if (tmp.value > best.value)
			{
				best.value = tmp.value;
				best.move = i;
			}
		}
	tmp = best;
	tmp.value -= cur.points[cur.turn] - cur.points[!cur.turn];
	
	// save result if executed by the producer
	while (!mtx.try_lock());
	if (complete_search && cur.hash == (cur.hash | game.hash))
		transposition[cur.hash] = tmp;
	mtx.unlock();
	
	return best;
}

int board::generate_move()
{
	start_time = clock();
	org_turn = this->turn;
	data result = evaluate(*this); 
	end_time = clock();
	printf("\nTime: %.3fs    \n", (end_time - start_time) / double(CLOCKS_PER_SEC));
	if (log_engine)
	{
		freopen("log.txt", "a", stdout);
		printf("Expected outcome: %d\nPlayer %d: %d\n", result.value, (int)turn + 1, result.move);
		printf("Time: %.3fs\n", (end_time - start_time) / double(CLOCKS_PER_SEC));
		freopen("CONOUT$", "w", stdout);
	}
	return result.move;
}

#endif