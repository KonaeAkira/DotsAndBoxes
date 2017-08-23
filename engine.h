#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include <map>
#include <cstdlib>

const bool log_engine = 1;

const int max_depth = 30, // odd number for best result
		  inf = 1e9;
		  
struct data
{
	int value, move;
};

struct hash_info
{
	long long hash;
	int points_1, points_2;
};

struct compare_hash
{
	bool operator()(const hash_info &a, const hash_info &b) const
	{
		if (a.hash != b.hash) return a.hash < b.hash;
		else if (a.points_1 != b.points_1) return a.points_1 < b.points_1;
		else return a.points_2 < b.points_2;
	}
};

map <hash_info, data, compare_hash> hash_map;
bool org_turn;
int start_time, end_time;

long long board::get_hash()
{
	long long ans = 0;
	for (int i = 0; i < max_moves; ++i)
		ans |= (!check_valid(i) << i);
	ans |= (int)turn << 31;
	return ans;
}

data prune(board &cur, int depth, int alpha, int beta) // alpha-beta pruning
{
	int m = game_height, n = game_width;
	data best = {cur.points[org_turn] - cur.points[!org_turn], 0}, tmp;
	map <hash_info, data>::iterator ptr;
	hash_info info = {cur.hash, cur.points[0], cur.points[1]};
	
	if (cur.end) best.value += cur.points[org_turn] >= cur.points[!org_turn]?1000:-1000;
	if (depth <= 0 || cur.end) return best;
	
	ptr = hash_map.find(info);
	if (ptr != hash_map.end()) return ptr->second;
	
	else if (cur.turn == org_turn) // max
	{
		best.value = -inf;
		for (int i = 0; i < max_moves; ++i)
			if (cur.check_valid(i))
			{
				board tar(cur); tar.move(i);
				tmp = prune(tar, depth - (tar.turn != cur.turn), alpha, beta);
				if (tmp.value > best.value)
				{
					best.value = tmp.value;
					best.move = i;
				}
				alpha = max(alpha, best.value);
				//if (beta <= alpha) break;
			}
		hash_map[info] = best;
		return best;
	}
	else // min
	{
		best.value = inf;
		for (int i = 0; i < max_moves; ++i)
			if (cur.check_valid(i))
			{
				board tar(cur); tar.move(i);
				tmp = prune(tar, depth - (tar.turn != cur.turn), alpha, beta);
				if (tmp.value < best.value)
				{
					best.value = tmp.value;
					best.move = i;
				}
				beta = min(beta, best.value);
				//if (beta <= alpha) break;
			}
		hash_map[info] = best;
		return best;
	}
}

int board::generate_move()
{
	start_time = clock();
	//hash_map.clear();
	org_turn = this->turn;
	data result = prune(*this, max_depth, -inf, inf); 
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