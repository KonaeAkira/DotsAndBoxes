#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include <map>

const int max_depth = 10,
		  inf = 1e9;
		  
struct prune_data
{
	int value, move;
};

map <long long, prune_data> hash_map;
bool org_turn;
int interval = 3, last = -100;

long long board::get_hash()
{
	long long ans = 0;
	for (int i = 0; i < m * (n + 1) + n * (m + 1); ++i)
		ans |= (!check_valid(i) << i);
	ans |= (int)turn << 31;
	return ans;
}

prune_data prune(board cur, int depth, int alpha, int beta)
{
	int m = cur.height(), n = cur.width(),
		max_moves = m * (n + 1) + n * (m + 1);
	prune_data best, tmp = {cur.points[org_turn] - cur.points[!org_turn], 0};
	map <long long, prune_data>::iterator ptr;
	if ((depth >= max_depth && cur.turn == org_turn) || cur.end)
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
				tmp = prune(tar, depth + (tar.turn != cur.turn), alpha, beta);
				if (tmp.value > best.value)
				{
					best.value = tmp.value;
					best.move = i;
				}
				alpha = max(alpha, best.value);
				if (beta <= alpha && cur.turn != tar.turn) break;
			}
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
				tmp = prune(tar, depth + (tar.turn != cur.turn), alpha, beta);
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
	hash_map.clear();
	org_turn = this->turn;
	prune_data result = prune(*this, 0, -inf, inf);
	return result.move;
}

#endif