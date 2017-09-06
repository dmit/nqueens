#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	BLACK,
	WHITE,
	ATTACK,
	QUEEN
} cell;

char cell_char(cell c) {
	switch (c) {
		case BLACK : return '.';
		case WHITE : return ' ';
		case ATTACK: return 'x';
		case QUEEN : return 'Q';
	}
}

typedef struct {
	int n;
	cell cells[];
} board;

board *board_new(int n) {
	board *b = malloc(sizeof(board) + sizeof(cell)*n*n);

	b->n = n;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cell c;
			if ((i+j) % 2 == 0) {
				c = WHITE;
			} else {
				c = BLACK;
			}
			b->cells[i*n + j] = c;
		}
	}

	return b;
}

typedef enum {
	SET_OK,
	SET_OCCUPIED,
	SET_UNSAFE_SPOT,
	SET_ATTACKING_POSITION,
	SET_INVALID_CELL
} set_result;

set_result board_set(board *from, board *to, cell c, int row, int col) {
	int n = from->n;
	assert(row < n);
	assert(col < n);

	switch (from->cells[row*n + col]) {
		case BLACK:
		case WHITE:
			break;
		case ATTACK:
			return SET_UNSAFE_SPOT;
		default:
			return SET_OCCUPIED;
	}

	*to = *from;
	memcpy(to->cells, from->cells, sizeof(cell)*n*n);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int idx = i*n + j;

			if (i == row && j == col) {
				to->cells[idx] = c;
			} else {
				switch (c) {
					case BLACK:
					case WHITE:
					case ATTACK:
						return SET_INVALID_CELL;

					case QUEEN:
						if (
								(i == row || j == col) ||
								(row - i == col -j) ||
								(row - i == j - col)
						   ) {
							if (to->cells[idx] > ATTACK) {
								return SET_ATTACKING_POSITION;
							}
							to->cells[idx] = ATTACK;
						}
						break;
				}
			}
		}
	}

	return SET_OK;
}

void board_print(board *b) {
	int n = b->n;
	for (int i = 0; i < n+2; i++) {
		for (int j = 0; j < n+2; j++) {
			if (i == 0 || i == n+1) {
				if (j > 0 && j < n+1) {
					putchar('A' - 1 + j);
				} else {
					putchar(' ');
				}
			} else {
				if (j == 0 || j == n+1) {
					putchar('1' - 1 + i);
				} else {
					cell c = b->cells[(i-1)*n + j-1];
					putchar(cell_char(c));
				}
			}
		}
		putchar('\n');
	}
}

bool test_board(board *b, int row) {
	int n = b->n;
	if (row >= n) return true;

	board *tmp = board_new(n);
	for (int col = 0; col < n; col++) {
		if (b->cells[row*n + col] > WHITE) continue;

		board_set(b, tmp, QUEEN, row, col);
		if (test_board(tmp, row+1)) {
			*b = *tmp;
			memcpy(b->cells, tmp->cells, sizeof(cell)*n*n);
			free(tmp);
			return true;
		}
	}

	free(tmp);
	return false;
}

void debug_set(board *b, int x, int y) {
	set_result res = board_set(b, b, QUEEN, x, y);
	if (res != SET_OK) {
		fprintf(stderr, "failed to set queen to %c%d: %d\n", 'A'+y, x+1, res);
		exit(1);
	}
}

int main(int argc, char** argv) {
	int n;
	if (argc > 1) {
		n = atoi(argv[1]);
	} else {
		n = 8;
	}

	board *b = board_new(n);
	if (test_board(b, 0)) {
		fprintf(stderr, "success!\n");
		board_print(b);
	} else {
		fprintf(stderr, "failure!\n");
	}
}
