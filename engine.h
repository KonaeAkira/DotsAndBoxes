#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include <map>
#include <cstdlib>
#include <vector>

const int mode = 0;

const int max_depth = 12,
		  max_cycles = 10000,
		  inf = 1e9;
		  
struct data
{
	int value, move;
};

map <long long, data> hash_map;
bool org_turn;
int interval = 3, last = -100;

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
	int m = game_height, n = game_width,
		max_moves = m * (n + 1) + n * (m + 1);
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

data estimate(board &cur, int cycles = max_cycles)
{
	int m = game_height, n = game_width,
		org_stash[1000], stash[1000], org_cnt = 0, cnt,
		wins = 0, total = cycles;
	for (int i = 0; i < n * (m + 1) + m * (n + 1); ++i)
		if (cur.check_valid(i)) org_stash[org_cnt++] = i;
	while (cycles--)
	{
		board tar(cur); cnt = org_cnt;
		for (int i = 0; i < org_cnt; ++i)
			stash[i] = org_stash[i];
		while (!tar.end)
		{
			if (tar.turn == org_turn) //random move
			{
				new_move:
				if (cnt <= 0) break;
				int x = rand() % cnt--;
				if (x != cnt) swap(stash[x], stash[cnt]);
				if (tar.check_valid(stash[cnt]))
					tar.move(stash[cnt]);
				else 
					goto new_move;
			}
			else
			{
				hash_map.clear();
				data tmp = prune(tar, 1, -inf, inf, tar.turn);
				tar.move(tmp.move);
			}
		}
		if (tar.points[org_turn] >= tar.points[!org_turn])
			++wins;
	}
	data tmp = {wins, total};
	printf("%d %d                   \n", wins, total);
	return tmp;
}

int board::generate_move()
{
	org_turn = this->turn;
	if (mode == 0) // alpha-beta pruning
	{
		hash_map.clear();
		data result = prune(*this, max_depth, -inf, inf, org_turn);
		return result.move;
	}
	else if (mode == 1) // random estimation
	{
		float best = 0; int move;
		for (int i = 0; i < max_moves; ++i)
			if (this->check_valid(i))
			{
				board cur(*this);
				cur.move(i);
				data tmp = estimate(cur);
				float result = (float)tmp.value / tmp.move;
				if (result >= best)
				{
					best = result;
					move = i;
				}
			}
		return move;
	}
}

#endif