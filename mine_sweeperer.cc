#include <iostream>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <set>

#define BOARD_WIDTH 16
#define BOARD_HEIGHT 30
#define NUM_MINE 99
#define UNKNOWN 0x0FFFFFFF
#define MINE 0x0FFFFFFE
#define REV 0x0FFFFFFD 
#define VERBOSE 0

void mark_as_mine(const size_t, const size_t, const size_t, const size_t, const bool **, unsigned int **const, unsigned int **const, const unsigned int **, int **const, std::set< std::pair<size_t, size_t> > &, size_t &);

void reveal_sq(const size_t, const size_t, const size_t, const size_t, const bool **, unsigned int **const, unsigned int **const, const unsigned int **, int **const, std::set< std::pair<size_t, size_t> > &, size_t &);

void new_game(const size_t board_width, const size_t board_height, const size_t num_mine, bool **&board, unsigned int **&mine_nums, unsigned int **&board_status, unsigned int **&num_unknown, int **&num_mine_remaining) {
	size_t __num_mine = 0; 
	board = new bool*[board_height];
	mine_nums = new unsigned int*[board_height];
	board_status = new unsigned int*[board_height];
	num_unknown = new unsigned int*[board_height];
	num_mine_remaining = new int*[board_height];
	for (size_t y = 0; y < board_height; ++y) {
		board[y] = new bool[board_width];
		mine_nums[y] = new unsigned int[board_width];
		board_status[y] = new unsigned int[board_width];
		num_unknown[y] = new unsigned int[board_width];
		num_mine_remaining[y] = new int[board_width];
	}
	for (size_t y = 0; y < board_height; ++y) {
		for (size_t x = 0; x < board_width; ++x) {
			board[y][x] = false;
			mine_nums[y][x] = 0;
			board_status[y][x] = UNKNOWN;
			num_unknown[y][x] = 0;
			num_mine_remaining[y][x] = 0;
			if (x > 0) {
				++num_unknown[y][x];  // (x - 1, y)
			}
			if (x + 1 < board_width) {
				++num_unknown[y][x];   // (x + 1, y)
			}
			if (y > 0) {
				++num_unknown[y][x];  // (x, y - 1)
				if (x > 0) {
					++num_unknown[y][x];   // (x - 1, y - 1)
				}
				if (x + 1 < board_width) {
					++num_unknown[y][x];   // (x + 1, y - 1)
				}
			}
			if (y + 1 < board_height) {
				++num_unknown[y][x];   // (x, y + 1)
				if (x > 0) {
					++num_unknown[y][x];   // (x - 1, y + 1)
				}
				if (x + 1 < board_width) {
					++num_unknown[y][x];   // (x + 1, y + 1)
				}
			}
		}
	}
	srand(time(0));
	while (__num_mine < num_mine) {
		size_t y = rand() % board_height, x = rand() % board_width;
		if (!board[y][x]) {
			board[y][x] = true;
			if (x > 0) {
				++mine_nums[y][x - 1];
			}
			if (x + 1 < board_width) {
				++mine_nums[y][x + 1];
			}
			if (y > 0) {
				++mine_nums[y - 1][x];
				if (x > 0) {
					++mine_nums[y - 1][x - 1];
				}
				if (x + 1 < board_width) {
					++mine_nums[y - 1][x + 1];
				}
			}
			if (y + 1 < board_height) {
				++mine_nums[y + 1][x];
				if (x > 0) {
					++mine_nums[y + 1][x - 1];
				}
				if (x + 1 < board_width) {
					++mine_nums[y + 1][x + 1];
				}
			}
			++__num_mine;
		}
	}
}

void print_board(const size_t board_width, const size_t board_height, const bool **board) {
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			std::cout << (board[y][x] ? "M|" : "_|");
		}
		std::cout << "\n";
	}
}

void print_board_status(const size_t board_width, const size_t board_height, const unsigned int **board_status) {
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			if (board_status[y][x] == UNKNOWN) {
				std::cout << "_|";
			}else if (board_status[y][x] == MINE) {
				std::cout << "M|";
			}else {
				std::cout << board_status[y][x] << "|";
			}
		}
		std::cout << "\n";
	}
}

void print_board_status_debug(const size_t board_width, const size_t board_height, const size_t rx, const size_t ry, const unsigned int **board_status) {
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			if (y == ry && x == rx) { 
				std::cout << "R|";
			}else if (board_status[y][x] == UNKNOWN) {
				std::cout << "_|";
			}else if (board_status[y][x] == MINE) {
				std::cout << "M|";
			}else {
				std::cout << board_status[y][x] << "|";
			}
		}
		std::cout << "\n";
	}
}

void print_mine_nums(const size_t board_width, const size_t board_height, const unsigned int **mine_nums) {
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			std::cout << mine_nums[y][x] << "|";
		}
		std::cout << "\n";
	}
}

void print_num_unknown(const size_t board_width, const size_t board_height, const unsigned int **num_unknown) {
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			std::cout << num_unknown[y][x] << "|";
		}
		std::cout << "\n";
	}
}

void print_num_mine_remaining(const size_t board_width, const size_t board_height, const int **num_mine_remaining) {
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			std::cout << num_mine_remaining[y][x] << "|";
		}
		std::cout << "\n";
	}
}

void print_partial_sq_set(const size_t board_width, const size_t board_height, const std::set< std::pair<size_t, size_t> > &partial_sq_set) { 
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			std::cout << (partial_sq_set.find(std::pair<size_t, size_t>(x, y)) == partial_sq_set.end() ? "_|" : "P|");
		}
		std::cout << "\n";
	}
}

void deduce_remaining_mines(const size_t board_width, const size_t board_height, const size_t x, const size_t y, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, const bool **board, std::set< std::pair<size_t, size_t> > &partial_sq_set, size_t &ssq_count) {
	if (num_mine_remaining[y][x] > 0 && ((unsigned int)num_mine_remaining[y][x]) == num_unknown[y][x]) {
		partial_sq_set.erase(std::pair<size_t, size_t>(x, y)); 
		if (x > 0) {
			if (board_status[y][x - 1] == UNKNOWN) {
				mark_as_mine(board_width, board_height, x - 1, y, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
		}
		if (x + 1 < board_width) {
			if (board_status[y][x + 1] == UNKNOWN) {
				mark_as_mine(board_width, board_height, x + 1, y, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}

		}
		if (y > 0) {
			if (board_status[y - 1][x] == UNKNOWN) {
				mark_as_mine(board_width, board_height, x, y - 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
			if (x > 0) {
				if (board_status[y - 1][x - 1] == UNKNOWN) {
					mark_as_mine(board_width, board_height, x - 1, y - 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
				}
			}
			if (x + 1 < board_width) {
				if (board_status[y - 1][x + 1] == UNKNOWN) {
					mark_as_mine(board_width, board_height, x + 1, y - 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
				}
			}
		}
		if (y + 1 < board_height) {
			if (board_status[y + 1][x] == UNKNOWN) {
				mark_as_mine(board_width, board_height, x, y + 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
			if (x > 0) {
				if (board_status[y + 1][x - 1] == UNKNOWN) {
					mark_as_mine(board_width, board_height, x - 1, y + 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
				}
			}
			if (x + 1 < board_width) {
				if (board_status[y + 1][x + 1] == UNKNOWN) {
					mark_as_mine(board_width, board_height, x + 1, y + 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
				}
			}
		}
	}
}

void deduce_remaining_non_mines(const size_t board_width, const size_t board_height, const size_t x, const size_t y, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, const bool **board, std::set< std::pair<size_t, size_t> > &partial_sq_set, size_t &ssq_count) {
	if (board_status[y][x] != UNKNOWN && board_status[y][x] != MINE && num_mine_remaining[y][x] == 0) {
		partial_sq_set.erase(std::pair<size_t, size_t>(x, y));
		if (x > 0 && board_status[y][x - 1] == UNKNOWN) {
			reveal_sq(board_width, board_height, x - 1, y, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
		}
		if (x + 1 < board_width && board_status[y][x + 1] == UNKNOWN) {
			reveal_sq(board_width, board_height, x + 1, y, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
		}
		if (y > 0) {
			if (board_status[y - 1][x] == UNKNOWN) {
				reveal_sq(board_width, board_height, x, y - 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
			if (x > 0 && board_status[y - 1][x - 1] == UNKNOWN) {
				reveal_sq(board_width, board_height, x - 1, y - 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
			if (x + 1 < board_width && board_status[y - 1][x + 1] == UNKNOWN) {
				reveal_sq(board_width, board_height, x + 1, y - 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
		}
		if (y + 1 < board_height) {
			if (board_status[y + 1][x] == UNKNOWN) {
				reveal_sq(board_width, board_height, x, y + 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
			if (x > 0 && board_status[y + 1][x - 1] == UNKNOWN) {
				reveal_sq(board_width, board_height, x - 1, y + 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
			if (x + 1 < board_width && board_status[y + 1][x + 1] == UNKNOWN) {
				reveal_sq(board_width, board_height, x + 1, y + 1, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
			}
		}
	}
}


void decr_num_unknown(const size_t board_width, const size_t board_height, const size_t x, const size_t y, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, const bool **board, std::set< std::pair<size_t, size_t> > &partial_sq_set, size_t &ssq_count) {
	--num_unknown[y][x];
	deduce_remaining_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
	deduce_remaining_non_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
}

void mark_as_mine(const size_t board_width, const size_t board_height, const size_t x, const size_t y, const bool **board, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, std::set< std::pair<size_t, size_t> > &partial_sq_set, size_t &ssq_count) {
	if (board_status[y][x] != MINE) {
		assert(board[y][x]);
		board_status[y][x] = MINE;
		++ssq_count;
		if (x > 0) {
			--num_mine_remaining[y][x - 1];
			decr_num_unknown(board_width, board_height, x - 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		}
		if (x + 1 < board_width) {
			--num_mine_remaining[y][x + 1];
			decr_num_unknown(board_width, board_height, x + 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		}
		if (y > 0) {
			--num_mine_remaining[y - 1][x];
			decr_num_unknown(board_width, board_height, x, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
			if (x > 0) {
				--num_mine_remaining[y - 1][x - 1];
				decr_num_unknown(board_width, board_height, x - 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
			}
			if (x + 1 < board_width) {
				--num_mine_remaining[y - 1][x + 1];
				decr_num_unknown(board_width, board_height, x + 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
			}
		}
		if (y + 1 < board_height) {
			--num_mine_remaining[y + 1][x];
			decr_num_unknown(board_width, board_height, x, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
			if (x > 0) {
				--num_mine_remaining[y + 1][x - 1];
				decr_num_unknown(board_width, board_height, x - 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
			}
			if (x + 1 < board_width) {
				--num_mine_remaining[y + 1][x + 1];
				decr_num_unknown(board_width, board_height, x + 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
			}
		}
	}
}

void reveal_sq(const size_t board_width, const size_t board_height, const size_t x, const size_t y, const bool **board, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, std::set< std::pair<size_t, size_t> > &partial_sq_set, size_t &ssq_count) {
	assert(!board[y][x]);
	assert(board_status[y][x] == UNKNOWN);
	board_status[y][x] = mine_nums[y][x];
	num_mine_remaining[y][x] += mine_nums[y][x];
	++ssq_count;
	if (x > 0) {
		decr_num_unknown(board_width, board_height, x - 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
	}
	if (x + 1 < board_width) {
		decr_num_unknown(board_width, board_height, x + 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
	}
	if (y > 0) {
		decr_num_unknown(board_width, board_height, x, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		if (x > 0) {
			decr_num_unknown(board_width, board_height, x - 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		}
		if (x + 1 < board_width) {
			decr_num_unknown(board_width, board_height, x + 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		}
	}
	if (y + 1 < board_height) {
		decr_num_unknown(board_width, board_height, x, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		if (x > 0) {
			decr_num_unknown(board_width, board_height, x - 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		}
		if (x + 1 < board_width) {
			decr_num_unknown(board_width, board_height, x + 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
		}
	}
	deduce_remaining_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
	deduce_remaining_non_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, ssq_count);
	if (num_mine_remaining[y][x] > 0) {
		partial_sq_set.insert(std::pair<size_t, size_t>(x, y));
	}
}

void solve(const size_t board_width, const size_t board_height, const bool **board, const unsigned int **mine_nums, unsigned int **const board_status, int **const num_mine_remaining, unsigned int **const num_unknown, std::set< std::pair<size_t, size_t> > &partial_sq_set, size_t &ssq_count) {
	//if (ssq_count == 0) {
	while (ssq_count < board_width * board_height) {	
		size_t y = rand() % board_height, x = rand() % board_width; 
		while (true) {
			if (!board[y][x] && board_status[y][x] == UNKNOWN) {
				break;
			}
			y = rand() % board_height;
			x = rand() % board_width;
		}
		reveal_sq(board_width, board_height, x, y, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, ssq_count);
	}
}

void delete_game(const size_t board_height, bool **const board, unsigned int **const mine_nums, unsigned int **const board_status, int **const num_mine_remaining, unsigned int ** const num_unknown) {
	for (size_t h = 0; h < board_height; ++h) {
		delete [] board[h];
		delete [] mine_nums[h];
		delete [] board_status[h];
		delete [] num_mine_remaining[h];
		delete [] num_unknown[h];
	}
	delete [] board;
	delete [] mine_nums;
	delete [] board_status;
	delete [] num_mine_remaining;
	delete [] num_unknown;
}

int main(int argc, char *argv[]) {
	size_t ssq_count = 0;
	bool **board = NULL;
	unsigned int **mine_nums = NULL, **board_status = NULL, **num_unknown = NULL;
	int **num_mine_remaining = NULL;
	std::set< std::pair<size_t, size_t> > partial_sq_set;
	new_game(BOARD_WIDTH, BOARD_HEIGHT, NUM_MINE, board, mine_nums, board_status, num_unknown, num_mine_remaining);
	//print_board(BOARD_WIDTH, BOARD_HEIGHT, (const bool**)board);
	//print_mine_nums(BOARD_WIDTH, BOARD_HEIGHT, (const unsigned int**)mine_nums);
	solve(BOARD_WIDTH, BOARD_HEIGHT, (const bool**)board, (const unsigned int**)mine_nums, board_status, num_mine_remaining, num_unknown, partial_sq_set, ssq_count);
	print_board_status(BOARD_WIDTH, BOARD_HEIGHT, (const unsigned int**)board_status);
	//print_partial_sq_set(BOARD_WIDTH, BOARD_HEIGHT, partial_sq_set);
	//print_num_unknown(BOARD_WIDTH, BOARD_HEIGHT, (const unsigned int **)num_unknown);
	delete_game(BOARD_HEIGHT, board, mine_nums, board_status, num_mine_remaining, num_unknown);
	return 0;
}
