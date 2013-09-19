#include <iostream>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <vector>
#include <set>
#include <list>

#define BOARD_WIDTH 16
#define BOARD_HEIGHT 30
#define NUM_MINE 99
#define UNKNOWN 0x0FFFFFFF
#define MINE 0x0FFFFFFE
#define VERBOSE 1
#define MIN_PARTIAL_SQ_SET_SIZE 16

void mark_as_mine(const size_t, const size_t, const size_t, const size_t, const bool **, unsigned int **const, unsigned int **const, const unsigned int **, int **const, std::set< std::pair<size_t, size_t> > &, std::set< std::pair<size_t, size_t> > &, size_t &);

void reveal_sq(const size_t, const size_t, const size_t, const size_t, const bool **, unsigned int **const, unsigned int **const, const unsigned int **, int **const, std::set< std::pair<size_t, size_t> > &, std::set< std::pair<size_t, size_t> > &, size_t &);

void new_game(const size_t board_width, const size_t board_height, const size_t num_mine, bool **&board, unsigned int **&mine_nums, unsigned int **&board_status, unsigned int **&num_unknown, int **&num_mine_remaining, std::set< std::pair<size_t, size_t> > &non_mines_sq_set) {
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
		const size_t y = rand() % board_height, x = rand() % board_width;
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
	for (size_t y = 0; y < board_height; ++y) {
		for (size_t x = 0; x < board_width; ++x) {
			if (!board[y][x]) {
				non_mines_sq_set.insert(std::pair<size_t, size_t>(x, y));
			}
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

void print_segs(const size_t board_width, const size_t board_height, const std::set< std::pair<size_t, size_t> > &partial_sq_set, const std::vector< std::set< std::pair<size_t, size_t> >* > &segs) {
	for (size_t x = 0; x < board_width; ++x) {
		std::cout << " _";
	}
	std::cout << "\n";
	for (size_t y = 0; y < board_height; ++y) {
		std::cout << "|";
		for (size_t x = 0; x < board_width; ++x) {
			if (partial_sq_set.find(std::pair<size_t, size_t>(x, y)) == partial_sq_set.end()) {
				std::cout << "_|";
			}else {
				char seg_label = 'A';
				for (std::vector< std::set< std::pair<size_t, size_t> >* >::const_iterator ss_iter = segs.begin(); ss_iter != segs.end(); ++ss_iter) {
					if ((*ss_iter) -> find(std::pair<size_t, size_t>(x, y)) != (*ss_iter) -> end()) {
						std::cout << seg_label << "|";
						break;
					}
					++seg_label;
				}
			}
		}
		std::cout << "\n";
	}

}

void deduce_remaining_mines(const size_t board_width, const size_t board_height, const size_t x, const size_t y, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, const bool **board, std::set< std::pair<size_t, size_t> > &partial_sq_set, std::set< std::pair<size_t, size_t> > &non_mines_sq_set, size_t &ssq_count) {
	if (num_mine_remaining[y][x] > 0 && ((unsigned int)num_mine_remaining[y][x]) == num_unknown[y][x]) {
		partial_sq_set.erase(std::pair<size_t, size_t>(x, y)); 
		if (x > 0) {
			if (board_status[y][x - 1] == UNKNOWN) {
				mark_as_mine(x - 1, y, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
		}
		if (x + 1 < board_width) {
			if (board_status[y][x + 1] == UNKNOWN) {
				mark_as_mine(x + 1, y, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}

		}
		if (y > 0) {
			if (board_status[y - 1][x] == UNKNOWN) {
				mark_as_mine(x, y - 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x > 0) {
				if (board_status[y - 1][x - 1] == UNKNOWN) {
					mark_as_mine(x - 1, y - 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
				}
			}
			if (x + 1 < board_width) {
				if (board_status[y - 1][x + 1] == UNKNOWN) {
					mark_as_mine(x + 1, y - 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
				}
			}
		}
		if (y + 1 < board_height) {
			if (board_status[y + 1][x] == UNKNOWN) {
				mark_as_mine(x, y + 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x > 0) {
				if (board_status[y + 1][x - 1] == UNKNOWN) {
					mark_as_mine(x - 1, y + 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
				}
			}
			if (x + 1 < board_width) {
				if (board_status[y + 1][x + 1] == UNKNOWN) {
					mark_as_mine(x + 1, y + 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
				}
			}
		}
	}
}

void deduce_remaining_non_mines(const size_t board_width, const size_t board_height, const size_t x, const size_t y, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, const bool **board, std::set< std::pair<size_t, size_t> > &partial_sq_set, std::set< std::pair<size_t, size_t> > &non_mines_sq_set, size_t &ssq_count) {
	if (board_status[y][x] != UNKNOWN && board_status[y][x] != MINE && num_mine_remaining[y][x] == 0) {
		partial_sq_set.erase(std::pair<size_t, size_t>(x, y));
		if (x > 0 && board_status[y][x - 1] == UNKNOWN) {
			reveal_sq(x - 1, y, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
		}
		if (x + 1 < board_width && board_status[y][x + 1] == UNKNOWN) {
			reveal_sq(x + 1, y, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
		}
		if (y > 0) {
			if (board_status[y - 1][x] == UNKNOWN) {
				reveal_sq(x, y - 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x > 0 && board_status[y - 1][x - 1] == UNKNOWN) {
				reveal_sq(x - 1, y - 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x + 1 < board_width && board_status[y - 1][x + 1] == UNKNOWN) {
				reveal_sq(x + 1, y - 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
		}
		if (y + 1 < board_height) {
			if (board_status[y + 1][x] == UNKNOWN) {
				reveal_sq(x, y + 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x > 0 && board_status[y + 1][x - 1] == UNKNOWN) {
				reveal_sq(x - 1, y + 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x + 1 < board_width && board_status[y + 1][x + 1] == UNKNOWN) {
				reveal_sq(x + 1, y + 1, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
		}
	}
}


void decr_num_unknown(const size_t board_width, const size_t board_height, const size_t x, const size_t y, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, const bool **board, std::set< std::pair<size_t, size_t> > &partial_sq_set, std::set< std::pair<size_t, size_t> > &non_mines_sq_set, size_t &ssq_count) {
	--num_unknown[y][x];
	deduce_remaining_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
	deduce_remaining_non_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
}

void mark_as_mine(const size_t x, const size_t y, const size_t board_width, const size_t board_height, const bool **board, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, std::set< std::pair<size_t, size_t> > &partial_sq_set, std::set< std::pair<size_t, size_t> > &non_mines_sq_set, size_t &ssq_count) {
	if (board_status[y][x] != MINE) {
		assert(board[y][x]);
		board_status[y][x] = MINE;
		++ssq_count;
		if (x > 0) {
			--num_mine_remaining[y][x - 1];
			decr_num_unknown(board_width, board_height, x - 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		}
		if (x + 1 < board_width) {
			--num_mine_remaining[y][x + 1];
			decr_num_unknown(board_width, board_height, x + 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		}
		if (y > 0) {
			--num_mine_remaining[y - 1][x];
			decr_num_unknown(board_width, board_height, x, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
			if (x > 0) {
				--num_mine_remaining[y - 1][x - 1];
				decr_num_unknown(board_width, board_height, x - 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x + 1 < board_width) {
				--num_mine_remaining[y - 1][x + 1];
				decr_num_unknown(board_width, board_height, x + 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
			}
		}
		if (y + 1 < board_height) {
			--num_mine_remaining[y + 1][x];
			decr_num_unknown(board_width, board_height, x, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
			if (x > 0) {
				--num_mine_remaining[y + 1][x - 1];
				decr_num_unknown(board_width, board_height, x - 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
			}
			if (x + 1 < board_width) {
				--num_mine_remaining[y + 1][x + 1];
				decr_num_unknown(board_width, board_height, x + 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
			}
		}
	}
}

void reveal_sq(const size_t x, const size_t y, const size_t board_width, const size_t board_height, const bool **board, unsigned int **const board_status, unsigned int **const num_unknown, const unsigned int **mine_nums, int **const num_mine_remaining, std::set< std::pair<size_t, size_t> > &partial_sq_set, std::set< std::pair<size_t, size_t> > &non_mines_sq_set, size_t &ssq_count) {
	assert(!board[y][x]);
	assert(board_status[y][x] == UNKNOWN);
	board_status[y][x] = mine_nums[y][x];
	non_mines_sq_set.erase(std::pair<size_t, size_t>(x, y));
	num_mine_remaining[y][x] += mine_nums[y][x];
	++ssq_count;
	if (VERBOSE) {
		print_board_status(board_width, board_height, (const unsigned int**)board_status);
		for (size_t i = 0; i < board_width; ++i) {
			std::cout << "__";
		}
		std::cout << "\nSTATUS: applying logical deductions...\n"; 
	}
	if (x > 0) {
		decr_num_unknown(board_width, board_height, x - 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
	}
	if (x + 1 < board_width) {
		decr_num_unknown(board_width, board_height, x + 1, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
	}
	if (y > 0) {
		decr_num_unknown(board_width, board_height, x, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		if (x > 0) {
			decr_num_unknown(board_width, board_height, x - 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		}
		if (x + 1 < board_width) {
			decr_num_unknown(board_width, board_height, x + 1, y - 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		}
	}
	if (y + 1 < board_height) {
		decr_num_unknown(board_width, board_height, x, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		if (x > 0) {
			decr_num_unknown(board_width, board_height, x - 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		}
		if (x + 1 < board_width) {
			decr_num_unknown(board_width, board_height, x + 1, y + 1, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
		}
	}
	deduce_remaining_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
	deduce_remaining_non_mines(board_width, board_height, x, y, board_status, num_unknown, mine_nums, num_mine_remaining, board, partial_sq_set, non_mines_sq_set, ssq_count);
	if (num_mine_remaining[y][x] > 0) {
		partial_sq_set.insert(std::pair<size_t, size_t>(x, y));
	}
}

int get_tentative_num_mine_remaining(const size_t cx, const size_t cy, const size_t board_width, const size_t board_height, const unsigned int **board_status, const int **num_mine_remaining, const std::set< std::pair<size_t, size_t> > &__tentative_mines, const std::set< std::pair<size_t, size_t> > &__tentative_non_mines) {
	int tentative_num_mine_remaining = num_mine_remaining[cy][cx];
	if (cx > 0 && __tentative_mines.find(std::pair<size_t, size_t>(cx - 1, cy)) != __tentative_mines.end()) {
		--tentative_num_mine_remaining;
	}
	if (cx + 1 < board_width && __tentative_mines.find(std::pair<size_t, size_t>(cx + 1, cy)) != __tentative_mines.end()) {
		--tentative_num_mine_remaining;
	}
	if (cy > 0) {
		if (__tentative_mines.find(std::pair<size_t, size_t>(cx, cy - 1)) != __tentative_mines.end()) {
			--tentative_num_mine_remaining;
		}
		if (cx > 0 && __tentative_mines.find(std::pair<size_t, size_t>(cx - 1, cy - 1)) != __tentative_mines.end()) {
			--tentative_num_mine_remaining;
		}
		if (cx + 1 < board_width && __tentative_mines.find(std::pair<size_t, size_t>(cx + 1, cy - 1)) != __tentative_mines.end()) {
			--tentative_num_mine_remaining;
		}
	}
	if (cy + 1 < board_height) {
		if (__tentative_mines.find(std::pair<size_t, size_t>(cx, cy + 1)) != __tentative_mines.end()) {
			--tentative_num_mine_remaining;
		}
		if (cx > 0 && __tentative_mines.find(std::pair<size_t, size_t>(cx - 1, cy + 1)) != __tentative_mines.end()) {
			--tentative_num_mine_remaining;
		}
		if (cx + 1 < board_width && __tentative_mines.find(std::pair<size_t, size_t>(cx + 1, cy + 1)) != __tentative_mines.end() ) {
			--tentative_num_mine_remaining;
		}
	}
	return tentative_num_mine_remaining;
}

bool validate_partial_sq(const size_t mx, const size_t my, const size_t board_width, const size_t board_height, const unsigned int **board_status, const int **num_mine_remaining, const std::set< std::pair<size_t, size_t> > &__tentative_mines, const std::set< std::pair<size_t, size_t> > &__tentative_non_mines, const bool validate_pm = true) {
	const int threshold = validate_pm ? 0 : -1;
	if (mx > 0 && board_status[my][mx - 1] != UNKNOWN && board_status[my][mx - 1] != MINE && get_tentative_num_mine_remaining(mx - 1, my, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
		return false;
	}
	if (mx + 1 < board_width && board_status[my][mx + 1] != UNKNOWN && board_status[my][mx + 1] != MINE && get_tentative_num_mine_remaining(mx + 1, my, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
		return false;
	}
	if (my > 0) {
		if (board_status[my - 1][mx] != UNKNOWN && board_status[my - 1][mx] != MINE && get_tentative_num_mine_remaining(mx, my - 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
			return false;
		}
		if (mx > 0 && board_status[my - 1][mx - 1] != UNKNOWN && board_status[my - 1][mx - 1] != MINE && get_tentative_num_mine_remaining(mx - 1, my - 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
			return false;
		}
		if (mx + 1 < board_width && board_status[my - 1][mx + 1] != UNKNOWN && board_status[my - 1][mx + 1] != MINE && get_tentative_num_mine_remaining(mx + 1, my - 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
			return false;
		}
	}
	if (my + 1 < board_height) {
		if (board_status[my + 1][mx] != UNKNOWN && board_status[my + 1][mx] != MINE && get_tentative_num_mine_remaining(mx, my + 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
			return false;
		}
		if (mx > 0 && board_status[my + 1][mx - 1] != UNKNOWN && board_status[my + 1][mx - 1] != MINE && get_tentative_num_mine_remaining(mx - 1, my + 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
			return false;
		}
		if (mx + 1 < board_width && board_status[my + 1][mx + 1] != UNKNOWN && board_status[my + 1][mx + 1] != MINE && get_tentative_num_mine_remaining(mx + 1, my + 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines) <= threshold) {
			return false;
		}
	}
	return true;
}

void solve_partial_sq(const size_t x, const size_t y, const size_t board_width, const size_t board_height, const unsigned int **board_status, const int **num_mine_remaining, const unsigned int **num_unknown, const std::set< std::pair<size_t, size_t> > &__tentative_mines, const std::set< std::pair<size_t, size_t> > &__tentative_non_mines, std::list< std::pair< std::set< std::pair<size_t, size_t> >, std::set< std::pair<size_t, size_t> > > > &tentative_sols) {
	unsigned int tentative_num_mine_remaining = get_tentative_num_mine_remaining(x, y, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines);
	std::list< std::list<size_t> > enums;
	std::vector< std::pair<size_t, size_t> > pm;
	if (x > 0 && board_status[y][x - 1] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x - 1, y)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x - 1, y)) == __tentative_non_mines.end() && validate_partial_sq(x - 1, y, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
		pm.push_back(std::pair<size_t, size_t>(x - 1, y));
	}
	if (x + 1 < board_width && board_status[y][x + 1] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x + 1, y)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x + 1, y)) == __tentative_non_mines.end() && validate_partial_sq(x + 1, y, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
		pm.push_back(std::pair<size_t, size_t>(x + 1, y));
	}
	if (y > 0) {
		if (board_status[y - 1][x] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x, y - 1)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x, y - 1)) == __tentative_non_mines.end() && validate_partial_sq(x, y - 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
			pm.push_back(std::pair<size_t, size_t>(x, y - 1));
		}
		if (x > 0 && board_status[y - 1][x - 1] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x - 1, y - 1)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x - 1, y - 1)) == __tentative_non_mines.end() && validate_partial_sq(x - 1, y - 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
			pm.push_back(std::pair<size_t, size_t>(x - 1, y - 1));
		}
		if (x + 1 < board_width && board_status[y - 1][x + 1] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x + 1, y - 1)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x + 1, y - 1)) == __tentative_non_mines.end() && validate_partial_sq(x + 1, y - 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
			pm.push_back(std::pair<size_t, size_t>(x + 1, y - 1));
		}
	}
	if (y + 1 < board_height) {
		if (board_status[y + 1][x] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x, y + 1)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x, y + 1)) == __tentative_non_mines.end() && validate_partial_sq(x, y + 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
			pm.push_back(std::pair<size_t, size_t>(x, y + 1));
		}
		if (x > 0 && board_status[y + 1][x - 1] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x - 1, y + 1)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x - 1, y + 1)) == __tentative_non_mines.end() && validate_partial_sq(x - 1, y + 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
			pm.push_back(std::pair<size_t, size_t>(x - 1, y + 1));
		}
		if (x + 1 < board_width && board_status[y + 1][x + 1] == UNKNOWN && __tentative_mines.find(std::pair<size_t, size_t>(x + 1, y + 1)) == __tentative_mines.end() && __tentative_non_mines.find(std::pair<size_t, size_t>(x + 1, y + 1)) == __tentative_non_mines.end() && validate_partial_sq(x + 1, y + 1, board_width, board_height, board_status, num_mine_remaining, __tentative_mines, __tentative_non_mines)) {
			pm.push_back(std::pair<size_t, size_t>(x + 1, y + 1));
		}
	}
	if (pm.size() < tentative_num_mine_remaining) {
		return;
	}
	if (tentative_num_mine_remaining == 0) {
		std::set< std::pair<size_t, size_t> > tentative_non_mines(__tentative_non_mines);
		for (std::vector< std::pair<size_t, size_t> >::const_iterator m_iter = pm.begin(); m_iter != pm.end(); ++m_iter) {
			tentative_non_mines.insert(*m_iter);
		}
		tentative_sols.push_back(std::pair< std::set< std::pair<size_t, size_t> >, std::set< std::pair<size_t, size_t> > >(__tentative_mines, tentative_non_mines));
		return;
	}
	if (pm.size() == tentative_num_mine_remaining) {
		std::set< std::pair<size_t, size_t> > tentative_mines(__tentative_mines);
		for (std::vector< std::pair<size_t, size_t> >::const_iterator m_iter = pm.begin(); m_iter != pm.end(); ++m_iter) {
			tentative_mines.insert(*m_iter);
		}
		tentative_sols.push_back(std::pair< std::set< std::pair<size_t, size_t> >, std::set< std::pair<size_t, size_t> > >(tentative_mines, __tentative_non_mines));
		return;
	}
	size_t __num_mine = 1;
	for (size_t i = 0; i < pm.size() - tentative_num_mine_remaining + 1; ++i) {
		std::list<size_t> first_elem;
		first_elem.push_back(i);
		enums.push_back(first_elem);
	}
	while (__num_mine < tentative_num_mine_remaining) {
		const size_t N = enums.size();
		for (size_t n = 0; n < N; ++n) {
			std::list<size_t> cl = enums.front();
			enums.pop_front();
			for (size_t i = cl.back() + 1; i < pm.size() - tentative_num_mine_remaining + __num_mine + 1; ++i) {
				std::list<size_t> nl(cl);
				nl.push_back(i);
				enums.push_back(nl);
			}
		}
		++__num_mine;
	}
	for (std::list< std::list<size_t> >::const_iterator enum_iter = enums.begin(); enum_iter != enums.end(); ++enum_iter) {
		bool is_valid = true;
		size_t base_idx = 0;
		std::set< std::pair<size_t, size_t> > tentative_mines(__tentative_mines), tentative_non_mines(__tentative_non_mines);
		for (std::list<size_t>::const_iterator l_iter = enum_iter -> begin(); l_iter != enum_iter -> end(); ++l_iter) {
			for (size_t nm_idx = base_idx; nm_idx < *l_iter; ++nm_idx) {
				tentative_non_mines.insert(pm[nm_idx]);
			}
			tentative_mines.insert(pm[*l_iter]);
			base_idx = *l_iter + 1;
		}
		for (size_t nm_idx = base_idx; nm_idx < pm.size(); ++nm_idx) {
			tentative_non_mines.insert(pm[nm_idx]);
		}
		for (std::list<size_t>::const_iterator l_iter = enum_iter -> begin(); l_iter != enum_iter -> end(); ++l_iter) {
			if (!validate_partial_sq(pm[*l_iter].first, pm[*l_iter].second, board_width, board_height, board_status, num_mine_remaining, tentative_mines, tentative_non_mines, false)) {
				is_valid = false;
				break;
			}
		}
		if (is_valid) {
			tentative_sols.push_back(std::pair< std::set< std::pair<size_t, size_t> >, std::set< std::pair<size_t, size_t> > >(tentative_mines, tentative_non_mines));
		}
	}	
}

void solve_partial_sq_set(const size_t board_width, const size_t board_height, const bool **board, const unsigned int **mine_nums, unsigned int **const board_status, int **const num_mine_remaining, unsigned int **const num_unknown, std::set< std::pair<size_t, size_t> > &partial_sq_set, std::set< std::pair<size_t, size_t> > &non_mines_sq_set, size_t &ssq_count) {
	std::set< std::pair<size_t, size_t> > ssq;
	std::vector< std::set< std::pair<size_t, size_t> >* > segs;
	for (std::set< std::pair<size_t, size_t> >::const_iterator p_iter = partial_sq_set.begin(); p_iter != partial_sq_set.end(); ++p_iter) {
		if (ssq.find(*p_iter) != ssq.end()) {
			continue;
		}
		std::list< std::pair<size_t, size_t> > q;
		q.push_back(*p_iter);
		ssq.insert(*p_iter);
		std::set< std::pair<size_t, size_t> > *c_seg = new std::set< std::pair<size_t, size_t> >();
		c_seg -> insert(*p_iter);
		while (!q.empty()) {
			std::pair<size_t, size_t> csq = q.front();
			q.pop_front();
			const size_t c_x = csq.first, c_y = csq.second, min_x = (c_x <= 2 ? 0 : c_x - 2), min_y = (c_y <= 2 ? 0 : c_y - 2), max_x = (c_x + 2 >= board_width ? board_width - 1 : c_x + 2), max_y = (c_y + 2 >= board_height ? board_height - 1 : c_y + 2);
			for (size_t x = min_x; x <= max_x; ++x) {
				for (size_t y = min_y; y <= max_y; ++y) {
					if (x == c_x && y == c_y) {
						continue;
					}
					if (board_status[y][x] != UNKNOWN && board_status[y][x] != MINE && partial_sq_set.find(std::pair<size_t, size_t>(x, y)) != partial_sq_set.end() && ssq.find(std::pair<size_t, size_t>(x, y)) == ssq.end()) {
						c_seg -> insert(std::pair<size_t, size_t>(x, y));	
						ssq.insert(std::pair<size_t, size_t>(x, y));
						q.push_back(std::pair<size_t, size_t>(x, y));
					}
				}
			}
		}
		segs.push_back(c_seg);
	}
	for (std::vector< std::set< std::pair<size_t, size_t> >* >::const_iterator ss_iter = segs.begin(); ss_iter != segs.end(); ++ss_iter) {
		std::list< std::pair< std::set< std::pair<size_t, size_t> >, std::set< std::pair<size_t, size_t> > > > tentative_sols; 
		for (std::set< std::pair<size_t, size_t> >::const_iterator sq_iter = (*ss_iter) -> begin(); sq_iter != (*ss_iter) -> end(); ++sq_iter) {
			if (tentative_sols.empty()) {
				std::set< std::pair<size_t, size_t> > tentative_mines, tentative_non_mines;
				solve_partial_sq(sq_iter -> first, sq_iter -> second, board_width, board_height, (const unsigned int**)board_status, (const int**)num_mine_remaining, (const unsigned int**)num_unknown, tentative_mines, tentative_non_mines, tentative_sols);
			}else {
				size_t N = tentative_sols.size();
				for (size_t n = 0; n < N; ++n) {
					std::set< std::pair<size_t, size_t> > tentative_mines(tentative_sols.front().first), tentative_non_mines(tentative_sols.front().second);
					tentative_sols.pop_front();
					solve_partial_sq(sq_iter -> first, sq_iter -> second, board_width, board_height, (const unsigned int**)board_status, (const int**)num_mine_remaining, (const unsigned int**)num_unknown, tentative_mines, tentative_non_mines, tentative_sols);
				}
			}
		}
		std::set< std::pair<size_t, size_t> > mines, non_mines;
		if (!tentative_sols.empty()) {
			std::set< std::pair<size_t, size_t> > mines_0 = tentative_sols.begin() -> first;
			std::set< std::pair<size_t, size_t> > non_mines_0 = tentative_sols.begin() -> second;
			for (std::set< std::pair<size_t, size_t> >::const_iterator m_iter = mines_0.begin(); m_iter != mines_0.end(); ++m_iter) {
				bool in_intersection = true;
				std::list< std::pair< std::set< std::pair<size_t, size_t> >, std::set< std::pair<size_t, size_t> > > >::const_iterator sol_iter = tentative_sols.begin();
				++sol_iter;
				while (sol_iter != tentative_sols.end()) {
					if (sol_iter -> first.find(*m_iter) == sol_iter -> first.end()) {
						in_intersection = false;
						break;
					}
					++sol_iter;
				}
				if (in_intersection) {
					mines.insert(*m_iter);
				}
			}
			for (std::set< std::pair<size_t, size_t> >::const_iterator nm_iter = non_mines_0.begin(); nm_iter != non_mines_0.end(); ++nm_iter) {
				bool deducible = true;
				std::list< std::pair< std::set< std::pair<size_t, size_t> >, std::set< std::pair<size_t, size_t> > > >::const_iterator sol_iter = tentative_sols.begin();
				++sol_iter;
				while (sol_iter != tentative_sols.end()) {
					if (sol_iter -> second.find(*nm_iter) == sol_iter -> second.end()) {
						deducible = false;
						break;
					}
					++sol_iter;
				}
				if (deducible) {
					non_mines.insert(*nm_iter);
				}
			}
		}
		for (std::set< std::pair<size_t, size_t> >::const_iterator m_iter = mines.begin(); m_iter != mines.end(); ++m_iter) { 
			mark_as_mine(m_iter -> first, m_iter -> second, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
		}
		for (std::set< std::pair<size_t, size_t> >::const_iterator nm_iter = non_mines.begin(); nm_iter != non_mines.end(); ++nm_iter) { 
			if (board_status[nm_iter -> second][nm_iter -> first] == UNKNOWN) {
				reveal_sq(nm_iter -> first, nm_iter -> second, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			}
		}
	}
	for (std::vector< std::set< std::pair<size_t, size_t> >* >::const_iterator ss_iter = segs.begin(); ss_iter != segs.end(); ++ss_iter) {
		delete *ss_iter;
	}
}

void solve(const size_t board_width, const size_t board_height, const bool **board, const unsigned int **mine_nums, unsigned int **const board_status, int **const num_mine_remaining, unsigned int **const num_unknown, std::set< std::pair<size_t, size_t> > &partial_sq_set, std::set< std::pair<size_t, size_t> > &non_mines_sq_set, size_t &ssq_count) {
	while (ssq_count < board_width * board_height) {
		if (!non_mines_sq_set.empty()) {
			size_t x, y, n = rand() % non_mines_sq_set.size(); 
			std::set< std::pair<size_t, size_t> >::const_iterator nms_iter = non_mines_sq_set.begin(); 
			for (size_t i = 0; i < n; ++i) {
				++nms_iter;
			}
			x = nms_iter -> first;
			y = nms_iter -> second;
			if (VERBOSE) {
				std::cout << "\n";
				for (size_t i = 0; i < board_width; ++i) {
					std::cout << "__";
				}
				std::cout << "\nSTATUS: \"I'm feeling lucky\"\n"; 
			}
			reveal_sq(x, y, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
			if (VERBOSE) {
				print_board_status(BOARD_WIDTH, BOARD_HEIGHT, (const unsigned int**)board_status);
			}
			if (VERBOSE) {
				std::cout << "\n";
				for (size_t i = 0; i < board_width; ++i) {
					std::cout << "__";
				}
				std::cout << "\nSTATUS: applying logical deductions...\n"; 
			}
			solve_partial_sq_set(board_width, board_height, board, mine_nums, board_status, num_mine_remaining, num_unknown, partial_sq_set, non_mines_sq_set, ssq_count);
		}else {
			for (size_t x = 0; x < board_width; ++x) {
				for (size_t y = 0; y < board_height; ++y) {
					if (board_status[y][x] == UNKNOWN) {
						mark_as_mine(x, y, board_width, board_height, board, board_status, num_unknown, mine_nums, num_mine_remaining, partial_sq_set, non_mines_sq_set, ssq_count);
					}
				}
			}
		}
	}
	if (VERBOSE) {
		std::cout << "\n";
		for (size_t i = 0; i < board_width; ++i) {
			std::cout << "__";
		}
		std::cout << "\nSTATUS: winning :)\n"; 
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
	std::set< std::pair<size_t, size_t> > non_mines_sq_set;
	new_game(BOARD_WIDTH, BOARD_HEIGHT, NUM_MINE, board, mine_nums, board_status, num_unknown, num_mine_remaining, non_mines_sq_set);
	solve(BOARD_WIDTH, BOARD_HEIGHT, (const bool**)board, (const unsigned int**)mine_nums, board_status, num_mine_remaining, num_unknown, partial_sq_set, non_mines_sq_set, ssq_count);
	delete_game(BOARD_HEIGHT, board, mine_nums, board_status, num_mine_remaining, num_unknown);
	return 0;
}
