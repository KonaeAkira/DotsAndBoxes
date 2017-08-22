#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include <map>
#include <cstdlib>
#include <vector>

const long long max_execution = 1e15;

const int max_depth = 11, // odd number for best result
		  max_cycles = 10000,
		  inf = 1e9;
		  
struct data
{
	int value, move;
};

map <long long, data> hash_map;
bool org_turn;
int interval = 3, last = -100,
	start_time, end_time;

long long board::get_hash()
{
	long long ans = 0;
	for (int i = 0; i < max_moves; ++i)
		ans |= (!check_valid(i) << i);
	ans |= (int)turn << 31;
	return ans;
}

data prune(board cur, int depth, int alpha, int beta, const bool &org_turn) // alpha-beta pruning
{
	int m = game_height, n = game_width;
	data best, tmp = {cur.points[org_turn] - cur.points[!org_turn], 0};
	map <long long, data>::iterator ptr;
	if (cur.end) tmp.value += (cur.points[org_turn] >= cur.points[!org_turn])?100000:(-100000);
	if (depth <= 0 || cur.end)
		return tmp;
	ptr = hash_map.find(cur.hash);
	if (ptr != hash_map.end())
		return ptr->second;
	else if (cur.turn == org_turn) // max
	{
		best.value = -inf;
		for (int i = 0; i < max_moves; ++i)
			if (cur.check_valid(i))
			{
				board tar(cur);
				tar.move(i);
				tmp = prune(tar, depth - (tar.turn != cur.turn), alpha, beta, org_turn);
				if (tmp.value > best.value)
				{
					best.value = tmp.value;
					best.move = i;
				}
				alpha = max(alpha, best.value);
				if (beta <= alpha && cur.turn != tar.turn) break;
			}
		best.value += cur.points[org_turn] - cur.points[!org_turn];
		hash_map[cur.hash] = best;
		return best;
	}
	else // min
	{
		best.value = inf;
		for (int i = 0; i < max_moves; ++i)
			if (cur.check_valid(i))
			{
				board tar(cur);
				tar.move(i);
				tmp = prune(tar, depth - (tar.turn != cur.turn), alpha, beta, org_turn);
				if (tmp.value < best.value)
				{
					best.value = tmp.value;
					best.move = i;
				}
				beta = min(beta, best.value);
				if (beta <= alpha && cur.turn != tar.turn) break;
			}
		hash_map[cur.hash] = best;
		return best;
	}
}

int board::generate_move()
{
	start_time = clock();
	hash_map.clear();
	data result = prune(*this, max_depth, -inf, inf, org_turn);
	end_time = clock();
	printf("\nTime: %.3fs    \n", (end_time - start_time) / double(CLOCKS_PER_SEC));
	return result.move;
}

#endif