CC = g++
CFLAGS = -std=c++17 -Ofast -g -Wfatal-errors -c -Iinclude
LDFLAGS = -lpthread

default: same_game_sim same_game_random_search

same_game_sim: same_game_sim.o
	$(CC) -o $@ $< $(LDFLAGS)

same_game_sim.o: src/same_game_sim.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

same_game_random_search: same_game_random_search.o
	$(CC) -o $@ $< $(LDFLAGS)

same_game_random_search.o: src/same_game_random_search.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
