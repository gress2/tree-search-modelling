CC = g++
CFLAGS = -std=c++17 -Ofast -g -Wfatal-errors -c -Iinclude
LDFLAGS = -lpthread
OBJS = game.o

default: same_game_sim same_game_random_search same_game_uct generic_game_random_search generic_game_uct

same_game_sim: same_game_sim.o $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(OBJS) 

same_game_sim.o: src/same_game_sim.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

same_game_random_search: same_game_random_search.o $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(OBJS)

same_game_random_search.o: src/same_game_random_search.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

generic_game_random_search: generic_game_random_search.o $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(OBJS)

same_game_uct: same_game_uct.o $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(OBJS)

same_game_uct.o: src/same_game_uct.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

generic_game_random_search.o: src/generic_game_random_search.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

generic_game_uct: generic_game_uct.o $(OBJS) 
	$(CC) -o $@ $< $(LDFLAGS) $(OBJS)

generic_game_uct.o: src/generic_game_uct.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

game.o: src/game.cc
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
