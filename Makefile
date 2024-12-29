all:
	@g++ -Wall -Wextra -Werror -g src/ai.cpp src/game.cpp src/node.cpp src/main.cpp -o AITermProject lib/libraylib.a

run:
	@./AITermProject

val:
	@valgrind --leak-check=full ./AITermProject

re: clean all

clean:
	@rm AITermProject