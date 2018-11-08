CC = g++
CFLAGS = -std=c++17 -Ofast -g -Wfatal-errors -c -Iinclude

same_game_sim: same_game_sim.o
	$(CC) -o $@ $<

same_game_sim.o: src/same_game_sim.cc include/*.hpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
