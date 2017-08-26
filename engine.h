#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include <stdio.h>
#include <conio.h>

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

map <long long, data> transposition, tmp_map;
bool org_turn;
int start_time, end_time;
long long last_hash = 0;

mutex mtx;

void import_data()
{
	mtx.lock();
	long long hash;
	data tmp;
	int start_time = clock(), end_time;
	printf("Importing data ...\n");
	freopen("transposition.dat", "r", stdin);
	while (scanf("%lld %d %d\n", &hash, &tmp.value, &tmp.move) == 3)
		transposition[hash] = tmp;
	freopen("CONIN$", "r", stdin);
	if (transposition.empty()) printf("Error: Save file does not exist or is empty.\n");
	else printf("Done importing.\n");
	end_time = clock();
	printf("Time: %.3fs\n", (end_time - start_time) / double(CLOCKS_PER_SEC));
	printf("Press any key to continue ...");
	getch();
	mtx.unlock();
}

void export_data()
{
	mtx.lock();
	printf("Exporting data ...\n");
	freopen("transposition.dat", "w", stdout);
	for (auto i = transposition.begin(); i != transposition.end(); ++i)
		printf("%lld %d %d\n", i->first, i->second.value, i->second.move);
	freopen("CONOUT$", "w", stdout);
	printf("Done exporting.\n");
	mtx.unlock();
}

data evaluate(board &cur, bool complete_search = 0, int depth = max_depth)
{
	int m = game_height, n = game_width;
	long long game_hash;
	data tmp = {0, -1}, best = {-inf, -1};
	
	// return current board if game ended
	if (cur.end) return tmp;
	
	// check if producer already has a new target
	if (cur.hash != (cur.hash ^ last_hash) && complete_search)
		return tmp;
	
	while (!mtx.try_lock());
	game_hash = game.hash;
	// return if current state cannot be reached
	if (cur.hash != (cur.hash | game.hash) && complete_search)
	{
		board tar(game);
		mtx.unlock();
		if (last_hash != game_hash)
		{
			last_hash = game_hash;
			evaluate(tar, 1);
		}
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
	
	// check tmp_map if not complete searching
	if (!complete_search)
	{
		ptr = tmp_map.find(cur.hash);
		if (ptr != tmp_map.end())
			return ptr->second;
	}
	
	// return current state if max_depth is reached
	if (!complete_search && !depth) return tmp;
	
	// recursive search for all children
	for (int i = 0; i < max_moves; ++i)
		if (cur.check_valid(i))
		{
			board tar(cur); tar.move(i);
			tmp = evaluate(tar, complete_search, depth - (cur.turn ^ tar.turn));
			if (cur.turn ^ tar.turn) tmp.value = -tmp.value;
			else ++tmp.value;
			if (tmp.value > best.value)
			{
				best.value = tmp.value;
				best.move = i;
			}
		}
	
	// save result if executed by the producer
	if (complete_search && cur.hash == (cur.hash | game.hash))
	{
		while (!mtx.try_lock());
		transposition.insert(pair <long long, data> (cur.hash, best));
		mtx.unlock();
	}
	
	// save to tmp_map if not complete searching
	if (!complete_search)
		tmp_map.insert(pair <long long, data> (cur.hash, best));
	
	return best;
}

int board::generate_move()
{
	tmp_map.clear();
	start_time = clock();
	data result = evaluate(*this); 
	end_time = clock();
	printf("\nTime: %.3fs    \nOutcome: %d   ", (end_time - start_time) / double(CLOCKS_PER_SEC), result.value);
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