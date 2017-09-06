nqueens: nqueens.c
	clang -g -Werror -Wall -pedantic -o nqueens nqueens.c

opt: nqueens.c
	clang -g -Ofast -march=native -Werror -Wall -pedantic -o nqueens nqueens.c

run: nqueens
	./nqueens

clean:
	rm nqueens
