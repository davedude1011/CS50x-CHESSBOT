#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/*
    My information:

    My name is Thomas Smallwood, my github is www.github.com/davedude1011,
    born in cyprus, living in robertsbridge england,
    the current date is sunday the 13th of april 2025

    this project is CS50 CHESS, a fairly simple chess bot that uses the Minimax
    algorithm to search through possible moves, then uses bitboards and piece
    values to numerically choose which move in the tree is most effective
    and attempts to play it based on the actuall rules of the game.
*/

// BOARD
typedef struct {
    uint64_t pawn_white;
    uint64_t knight_white;
    uint64_t bishop_white;
    uint64_t rook_white;
    uint64_t queen_white;
    uint64_t king_white;

    uint64_t pawn_black;
    uint64_t knight_black;
    uint64_t bishop_black;
    uint64_t rook_black;
    uint64_t queen_black;
    uint64_t king_black;

    int move_num;
} board_t;

const char *board_cli_pieces[] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟", "♞", "♝", "♜", "♛", "♚"};
//                               {"i", "n", "b", "r", "q", "k", "I", "N", "B", "R", "Q", "K"}

// RAY BITBOARDS
uint64_t ray_bitboards_north[64];
uint64_t ray_bitboards_east[64];
uint64_t ray_bitboards_south[64];
uint64_t ray_bitboards_west[64];
uint64_t ray_bitboards_north_east[64];
uint64_t ray_bitboards_north_west[64];
uint64_t ray_bitboards_south_east[64];
uint64_t ray_bitboards_south_west[64];

// PRECOMPUTED MOVEMENTS
uint64_t precomputed_movements_knight[64];
uint64_t precomputed_movements_king[64];

// EVALUATION PSTs (PIECE-SQUARE TABLEs)
typedef struct {
    int pawn_black[64];
    int knight_black[64];
    int bishop_black[64];
    int rook_black[64];
    int queen_black[64];
    int king_black[64];
    
    int pawn_white[64];
    int knight_white[64];
    int bishop_white[64];
    int rook_white[64];
    int queen_white[64];
    int king_white[64];
} pst_midgame_t;

typedef struct {
    int pawn_black[64];
    int knight_black[64];
    int bishop_black[64];
    int rook_black[64];
    int queen_black[64];
    int king_black[64];
    
    int pawn_white[64];
    int knight_white[64];
    int bishop_white[64];
    int rook_white[64];
    int queen_white[64];
    int king_white[64];
} pst_endgame_t;

int PST_MIDGAME_BASE_PAWN[64]   = { 0, 0, 0, 0, 0, 0, 0, 0, 98, 134, 61, 95, 68, 126, 34, -11, -6, 7, 26, 31, 65, 56, 25, -20, -14, 13, 6, 21, 23, 12, 17, -23, -27, -2, -5, 12, 17, 6, 10, -25, -26, -4, -4, -10, 3, 3, 33, -12, -35, -1, -20, -23, -15, 24, 38, -22, 0, 0, 0, 0, 0, 0, 0, 0 };
int PST_MIDGAME_BASE_KNIGHT[64] = { -167, -89, -34, -49, 61, -97, -15, -107, -73, -41, 72, 36, 23, 62, 7, -17, -47, 60, 37, 65, 84, 129, 73, 44, -9, 17, 19, 53, 37, 69, 18, 22, -13, 4, 16, 13, 28, 19, 21, -8, -23, -9, 12, 10, 19, 17, 25, -16, -29, -53, -12, -3, -1, 18, -14, -19, -105, -21, -58, -33, -17, -28, -19, -23 };
int PST_MIDGAME_BASE_BISHOP[64] = { -29, 4, -82, -37, -25, -42, 7, -8, -26, 16, -18, -13, 30, 59, 18, -47, -16, 37, 43, 40, 35, 50, 37, -2, -4, 5, 19, 50, 37, 37, 7, -2, -6, 13, 13, 26, 34, 12, 10, 4, 0, 15, 15, 15, 14, 27, 18, 10, 4, 15, 16, 0, 7, 21, 33, 1, -33, -3, -14, -21, -13, -12, -39, -21 };
int PST_MIDGAME_BASE_ROOK[64]   = { 32, 42, 32, 51, 63, 9, 31, 43, 27, 32, 58, 62, 80, 67, 26, 44, -5, 19, 26, 36, 17, 45, 61, 16, -24, -11, 7, 26, 24, 35, -8, -20, -36, -26, -12, -1, 9, -7, 6, -23, -45, -25, -16, -17, 3, 0, -5, -33, -44, -16, -20, -9, -1, 11, -6, -71, -19, -13, 1, 17, 16, 7, -37, -26 };
int PST_MIDGAME_BASE_QUEEN[64]  = { -28, 0, 29, 12, 59, 44, 43, 45, -24, -39, -5, 1, -16, 57, 28, 54, -13, -17, 7, 8, 29, 56, 47, 57, -27, -27, -16, -16, -1, 17, -2, 1, -9, -26, -9, -10, -2, -4, 3, -3, -14, 2, -11, -2, -5, 2, 14, 5, -35, -8, 11, 2, 8, 15, -3, 1, -1, -18, -9, 10, -15, -25, -31, -50 };
int PST_MIDGAME_BASE_KING[64]   = { -65, 23, 16, -15, -56, -34, 2, 13, 29, -1, -20, -7, -8, -4, -38, -29, -9, 24, 2, -16, -20, 6, 22, -22, -17, -20, -12, -27, -30, -25, -14, -36, -49, -1, -27, -39, -46, -44, -33, -51, -14, -14, -22, -46, -44, -30, -15, -27, 1, 7, -8, -64, -43, -16, 9, 8, -15, 36, 12, -54, 8, -28, 24, 14 };
int PST_ENDGAME_BASE_PAWN[64]   = { 0, 0, 0, 0, 0, 0, 0, 0, 178, 173, 158, 134, 147, 132, 165, 187, 94, 100, 85, 67, 56, 53, 82, 84, 32, 24, 13, 5, -2, 4, 17, 17, 13, 9, -3, -7, -7, -8, 3, -1, 4, 7, -6, 1, 0, -5, -1, -8, 13, 8, 8, 10, 13, 0, 2, -7, 0, 0, 0, 0, 0, 0, 0, 0 };
int PST_ENDGAME_BASE_KNIGHT[64] = { -58, -38, -13, -28, -31, -27, -63, -99, -25, -8, -25, -2, -9, -25, -24, -52, -24, -20, 10, 9, -1, -9, -19, -41, -17, 3, 22, 22, 22, 11, 8, -18, -18, -6, 16, 25, 16, 17, 4, -18, -23, -3, -1, 15, 10, -3, -20, -22, -42, -20, -10, -5, -2, -20, -23, -44, -29, -51, -23, -15, -22, -18, -50, -64 };
int PST_ENDGAME_BASE_BISHOP[64] = { -14, -21, -11, -8, -7, -9, -17, -24, -8, -4, 7, -12, -3, -13, -4, -14, 2, -8, 0, -1, -2, 6, 0, 4, -3, 9, 12, 9, 14, 10, 3, 2, -6, 3, 13, 19, 7, 10, -3, -9, -12, -3, 8, 10, 13, 3, -7, -15, -14, -18, -7, -1, 4, -9, -15, -27, -23, -9, -23, -5, -9, -16, -5, -17 };
int PST_ENDGAME_BASE_ROOK[64]   = { 13, 10, 18, 15, 12, 12, 8, 5, 11, 13, 13, 11, -3, 3, 8, 3, 7, 7, 7, 5, 4, -3, -5, -3, 4, 3, 13, 1, 2, 1, -1, 2, 3, 5, 8, 4, -5, -6, -8, -11, -4, 0, -5, -1, -7, -12, -8, -16, -6, -6, 0, 2, -9, -9, -11, -3, -9, 2, 3, -1, -5, -13, 4, -20 };
int PST_ENDGAME_BASE_QUEEN[64]  = { -9, 22, 22, 27, 27, 19, 10, 20, -17, 20, 32, 41, 58, 25, 30, 0, -20, 6, 9, 49, 47, 35, 19, 9, 3, 22, 24, 45, 57, 40, 57, 36, -18, 28, 19, 47, 31, 34, 39, 23, -16, -27, 15, 6, 9, 17, 10, 5, -22, -23, -30, -16, -16, -23, -36, -32, -33, -28, -22, -43, -5, -32, -20, -41 };
int PST_ENDGAME_BASE_KING[64]   = { -74, -35, -18, -18, -11, 15, 4, -17, -12, 17, 14, 17, 17, 38, 23, 11, 10, 17, 23, 15, 20, 45, 44, 13, -8, 22, 24, 27, 26, 33, 26, 3, -18, -4, 21, 24, 27, 23, 9, -11, -19, -3, 11, 21, 23, 16, 7, -9, -27, -11, 4, 13, 14, 4, -5, -17, -53, -34, -21, -11, -28, -14, -24, -43 };

int PST_MIDGAME_BASE_PAWN_FLIPPED[64]   = { 0, 0, 0, 0, 0, 0, 0, 0, 35, 1, 20, 23, 15, -24, -38, 22, 26, 4, 4, 10, -3, -3, -33, 12, 27, 2, 5, -12, -17, -6, -10, 25, 14, -13, -6, -21, -23, -12, -17, 23, 6, -7, -26, -31, -65, -56, -25, 20, -98, -134, -61, -95, -68, -126, -34, 11, 0, 0, 0, 0, 0, 0, 0, 0 };
int PST_MIDGAME_BASE_KNIGHT_FLIPPED[64] = { 105, 21, 58, 33, 17, 28, 19, 23, 29, 53, 12, 3, 1, -18, 14, 19, 23, 9, -12, -10, -19, -17, -25, 16, 13, -4, -16, -13, -28, -19, -21, 8, 9, -17, -19, -53, -37, -69, -18, -22, 47, -60, -37, -65, -84, -129, -73, -44, 73, 41, -72, -36, -23, -62, -7, 17, 167, 89, 34, 49, -61, 97, 15, 107 };
int PST_MIDGAME_BASE_BISHOP_FLIPPED[64] = { 33, 3, 14, 21, 13, 12, 39, 21, -4, -15, -16, 0, -7, -21, -33, -1, 0, -15, -15, -15, -14, -27, -18, -10, 6, -13, -13, -26, -34, -12, -10, -4, 4, -5, -19, -50, -37, -37, -7, 2, 16, -37, -43, -40, -35, -50, -37, 2, 26, -16, 18, 13, -30, -59, -18, 47, 29, -4, 82, 37, 25, 42, -7, 8 };
int PST_MIDGAME_BASE_ROOK_FLIPPED[64]   = { 19, 13, -1, -17, -16, -7, 37, 26, 44, 16, 20, 9, 1, -11, 6, 71, 45, 25, 16, 17, -3, 0, 5, 33, 36, 26, 12, 1, -9, 7, -6, 23, 24, 11, -7, -26, -24, -35, 8, 20, 5, -19, -26, -36, -17, -45, -61, -16, -27, -32, -58, -62, -80, -67, -26, -44, -32, -42, -32, -51, -63, -9, -31, -43 };
int PST_MIDGAME_BASE_QUEEN_FLIPPED[64]  = { 1, 18, 9, -10, 15, 25, 31, 50, 35, 8, -11, -2, -8, -15, 3, -1, 14, -2, 11, 2, 5, -2, -14, -5, 9, 26, 9, 10, 2, 4, -3, 3, 27, 27, 16, 16, 1, -17, 2, -1, 13, 17, -7, -8, -29, -56, -47, -57, 24, 39, 5, -1, 16, -57, -28, -54, 28, 0, -29, -12, -59, -44, -43, -4 };
int PST_MIDGAME_BASE_KING_FLIPPED[64]   = { 53, 34, 21, 11, 28, 14, 24, 43, 27, 11, -4, -13, -14, -4, 5, 17, 19, 3, -11, -21, -23, -16, -7, 9, 18, 4, -21, -24, -27, -23, -9, 11, 8, -22, -24, -27, -26, -33, -26, -3, -10, -17, -23, -15, -20, -45, -44, -13, 12, -17, -14, -17, -17, -38, -23, -11, 74, 35, 18, 18, 11, -15, -4, 17 };
int PST_ENDGAME_BASE_PAWN_FLIPPED[64]   = { 0, 0, 0, 0, 0, 0, 0, 0, -13, -8, -8, -10, -13, 0, -2, 7, -4, -7, 6, -1, 0, 5, 1, 8, -13, -9, 3, 7, 7, 8, -3, 1, -32, -24, -13, -5, 2, -4, -17, -17, -94, -100, -85, -67, -56, -53, -82, -84, -178, -173, -158, -134, -147, -132, -165, -187, 0, 0, 0, 0, 0, 0, 0,  };
int PST_ENDGAME_BASE_KNIGHT_FLIPPED[64] = { 29, 51, 23, 15, 22, 18, 50, 64, 42, 20, 10, 5, 2, 20, 23, 44, 23, 3, 1, -15, -10, 3, 20, 22, 18, 6, -16, -25, -16, -17, -4, 18, 17, -3, -22, -22, -22, -11, -8, 18, 24, 20, -10, -9, 1, 9, 19, 41, 25, 8, 25, 2, 9, 25, 24, 52, 58, 38, 13, 28, 31, 27, 63, 99 };
int PST_ENDGAME_BASE_BISHOP_FLIPPED[64] = { 23, 9, 23, 5, 9, 16, 5, 17, 14, 18, 7, 1, -4, 9, 15, 27, 12, 3, -8, -10, -13, -3, 7, 15, 6, -3, -13, -19, -7, -10, 3, 9, 3, -9, -12, -9, -14, -10, -3, -2, -2, 8, 0, 1, 2, -6, 0, -4, 8, 4, -7, 12, 3, 13, 4, 14, 14, 21, 11, 8, 7, 9, 17, 24 };
int PST_ENDGAME_BASE_ROOK_FLIPPED[64]   = { 9, -2, -3, 1, 5, 13, -4, 20, 6, 6, 0, -2, 9, 9, 11, 3, 4, 0, 5, 1, 7, 12, 8, 16, -3, -5, -8, -4, 5, 6, 8, 11, -4, -3, -13, -1, -2, -1, 1, -2, -7, -7, -7, -5, -4, 3, 5, 3, -11, -13, -13, -11, 3, -3, -8, -3, -13, -10, -18, -15, -12, -12, -8, -5 };
int PST_ENDGAME_BASE_QUEEN_FLIPPED[64]  = { 33, 28, 22, 43, 5, 32, 20, 41, 22, 23, 30, 16, 16, 23, 36, 32, 16, 27, -15, -6, -9, -17, -10, -5, 18, -28, -19, -47, -31, -34, -39, -23, -3, -22, -24, -45, -57, -40, -57, -36, 20, -6, -9, -49, -47, -35, -19, -9, 17, -20, -32, -41, -58, -25, -30, 0, 9, -22, -22, -27, -27, -19, -10, -20 };
int PST_ENDGAME_BASE_KING_FLIPPED[64]   = { 53, 34, 21, 11, 28, 14, 24, 43, 27, 11, -4, -13, -14, -4, 5, 17, 19, 3, -11, -21, -23, -16, -7, 9, 18, 4, -21, -24, -27, -23, -9, 11, 8, -22, -24, -27, -26, -33, -26, -3, -10, -17, -23, -15, -20, -45, -44, -13, 12, -17, -14, -17, -17, -38, -23, -11, 74, 35, 18, 18, 11, -15, -4, 17 };

const int PIECE_VALUES_WHITE[6] = { 150,  175,  180,  200,  300,  100000};
const int PIECE_VALUES_BLACK[6] = {-150, -175, -180, -200, -300, -100000};

// ALL_POSSIBLE_MOVES
typedef struct possible_move_t {
    board_t move_board;
    struct possible_move_t *next;
} possible_move_t;

// GLOBALS
board_t board;
pst_midgame_t pst_midgame;
pst_midgame_t pst_endgame;
int moves_evaluated = 0;
const int MID_END_GAME_THRESHHOLD = 25;
const bool PRINT_BOARD_NUMBERS = true;
int minimax_depth = 3;

/*
    00000000 00000000 00000000 00000000 00000000 00000100 00000000 00000000

    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 1 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0
    
    row = 5
    col = 5
*/

// HELPERS
void print_bitboard(uint64_t bitboard) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int bit = (bitboard >> (63 - (i * 8 + j))) & 1;
            printf("%i ", bit);
        }
        printf("\n");
    }
    printf("\n");
}
void print_board(board_t board) {
    uint64_t piece_bitboards[12] = {
        board.pawn_black, board.knight_black, board.bishop_black, board.rook_black, board.queen_black, board.king_black,
        board.pawn_white, board.knight_white, board.bishop_white, board.rook_white, board.queen_white, board.king_white,
    };

    uint64_t square_pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 8; i++) {
        if (PRINT_BOARD_NUMBERS) printf("%i ", 7-i);
        for (int j = 0; j < 8; j++) {
            bool is_blank = true;
            for (int l = 0; l < 12; l++) {
                if (__builtin_popcountll(piece_bitboards[l] & square_pos) == 1) {
                    is_blank = false;
                    printf("%s ", board_cli_pieces[l]);
                    break;
                }
            }
            if (is_blank) {
                printf(". ");
            }
            square_pos >>= 1;
        }
        printf("\n");
    }

    if (PRINT_BOARD_NUMBERS) {
        printf("  ");
        for (int i = 0; i < 8; i++) {
            printf("%i ", i);
        }
    }
    printf("\n");
}
void flip_pst_arr(int pst_arr[64]) {
    int tmp_arr[32];

    for (int i = 0; i < 32; i++) {
        tmp_arr[i] = pst_arr[i];

        int row = i / 8;
        int col = i % 8;
        pst_arr[i] = pst_arr[(7 - row) * 8 + col];
    }

    for (int i = 32; i < 64; i++) {
        int row = (i - 32) / 8;
        int col = i % 8;
        pst_arr[i] = tmp_arr[(3 - row) * 8 + col];
    }
}
void copy_pst_arr(int from_arr[64], int to_arr[64]) {
    for (int i = 0; i < 64; i++) {
        to_arr[i] = from_arr[i];
    }
}
int get_bit_pos(uint64_t bitboard) {
    return 63 - __builtin_ctzll(bitboard);
    /*
        returns the position of the piece (0 -> 63)

        example:
        bitboard = 0b0000000000000000000000000000000000000000000000000000000000010000
        pos = 59
    */
}
board_t duplicate_board(board_t board) {
    board_t dupe_board;
    
    dupe_board.pawn_white = board.pawn_white;
    dupe_board.knight_white = board.knight_white;
    dupe_board.bishop_white = board.bishop_white;
    dupe_board.rook_white = board.rook_white;
    dupe_board.queen_white = board.queen_white;
    dupe_board.king_white = board.king_white;
    
    dupe_board.pawn_black = board.pawn_black;
    dupe_board.knight_black = board.knight_black;
    dupe_board.bishop_black = board.bishop_black;
    dupe_board.rook_black = board.rook_black;
    dupe_board.queen_black = board.queen_black;
    dupe_board.king_black = board.king_black;

    dupe_board.move_num = board.move_num;

    return dupe_board;
}
void clear_lines(int n) {
    for (int i = 0; i < n; i++) {
        printf("\033[F");      // Move cursor up one line
        printf("\033[2K");     // Clear entire line
    }
}
board_t user_move_board(board_t board, char curr_team) {
    int from[2] = {-1, -1};
    int to[2] = {-1, -1};

    while (from[0] > 7 || from[0] < 0) {
        printf("x1: ");
        scanf("%i", &from[0]);
        clear_lines(1);
    }
    while (from[1] > 7 || from[1] < 0) {
        printf("y1: ");
        scanf("%i", &from[1]);
        clear_lines(1);
    }

    while (to[0] > 7 || to[0] < 0) {
        printf("x2: ");
        scanf("%i", &to[0]);
        clear_lines(1);
    }
    while (to[1] > 7 || to[1] < 0) {
        printf("y2: ");
        scanf("%i", &to[1]);
        clear_lines(1);
    }
    
    uint64_t from_mask = 1ULL << (from[1] * 8 + (7 - from[0]));
    uint64_t to_mask = 1ULL << (to[1] * 8 + (7 - to[0]));
    switch (curr_team) {
        case ('w'):
            // move piece
            if ((board.pawn_white & from_mask) > 0) {
                board.pawn_white &= ~from_mask;
                board.pawn_white |= to_mask;
            }
            else if ((board.knight_white & from_mask) > 0) {
                board.knight_white &= ~from_mask;
                board.knight_white |= to_mask;
            }
            else if ((board.bishop_white & from_mask) > 0) {
                board.bishop_white &= ~from_mask;
                board.bishop_white |= to_mask;
            }
            else if ((board.rook_white & from_mask) > 0) {
                board.rook_white &= ~from_mask;
                board.rook_white |= to_mask;
            }
            else if ((board.queen_white & from_mask) > 0) {
                board.queen_white &= ~from_mask;
                board.queen_white |= to_mask;
            }
            else if ((board.king_white & from_mask) > 0) {
                board.king_white &= ~from_mask;
                board.king_white |= to_mask;
            }

            // capture piece
            if ((board.pawn_black & to_mask) > 0) {
                board.pawn_black &= ~to_mask;
            }
            else if ((board.knight_black & to_mask) > 0) {
                board.knight_black &= ~to_mask;
            }
            else if ((board.bishop_black & to_mask) > 0) {
                board.bishop_black &= ~to_mask;
            }
            else if ((board.rook_black & to_mask) > 0) {
                board.rook_black &= ~to_mask;
            }
            else if ((board.queen_black & to_mask) > 0) {
                board.queen_black &= ~to_mask;
            }
            else if ((board.king_black & to_mask) > 0) {
                board.king_black &= ~to_mask;
            }
            break;
        case ('b'):
            // move piece
            if ((board.pawn_black & from_mask) > 0) {
                board.pawn_black &= ~from_mask;
                board.pawn_black |= to_mask;
            }
            else if ((board.knight_black & from_mask) > 0) {
                board.knight_black &= ~from_mask;
                board.knight_black |= to_mask;
            }
            else if ((board.bishop_black & from_mask) > 0) {
                board.bishop_black &= ~from_mask;
                board.bishop_black |= to_mask;
            }
            else if ((board.rook_black & from_mask) > 0) {
                board.rook_black &= ~from_mask;
                board.rook_black |= to_mask;
            }
            else if ((board.queen_black & from_mask) > 0) {
                board.queen_black &= ~from_mask;
                board.queen_black |= to_mask;
            }
            else if ((board.king_black & from_mask) > 0) {
                board.king_black &= ~from_mask;
                board.king_black |= to_mask;
            }

            // capture piece
            if ((board.pawn_white & to_mask) > 0) {
                board.pawn_white &= ~to_mask;
            }
            else if ((board.knight_white & to_mask) > 0) {
                board.knight_white &= ~to_mask;
            }
            else if ((board.bishop_white & to_mask) > 0) {
                board.bishop_white &= ~to_mask;
            }
            else if ((board.rook_white & to_mask) > 0) {
                board.rook_white &= ~to_mask;
            }
            else if ((board.queen_white & to_mask) > 0) {
                board.queen_white &= ~to_mask;
            }
            else if ((board.king_white & to_mask) > 0) {
                board.king_white &= ~to_mask;
            }
            break;
    }

    return board;
}

// RAY BITBOARDS
uint64_t ray_bitboard_north(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1; i < (row + 1); i++) {
        uint64_t tmp_bitboard = bitboard << (i * 8);

        mask |= tmp_bitboard;
    }

    return mask;
}
uint64_t ray_bitboard_east(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1; i < (8 - col); i++) {
        uint64_t tmp_bitboard = bitboard >> i;

        mask |= tmp_bitboard;
    }

    return mask;
}
uint64_t ray_bitboard_south(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1; i < (8 - row); i++) {
        uint64_t tmp_bitboard = bitboard >> (i * 8);

        mask |= tmp_bitboard;
    }

    return mask;
}
uint64_t ray_bitboard_west(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1; i < (col + 1); i++) {
        uint64_t tmp_bitboard = bitboard << i;

        mask |= tmp_bitboard;
    }

    return mask;
}
uint64_t ray_bitboard_north_east(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1, j = 1; i < (row + 1) && j < (8 - col); i++, j++) {
        uint64_t tmp_bitboard = bitboard;
        tmp_bitboard <<= (i * 8);
        tmp_bitboard >>= j;
        
        mask |= tmp_bitboard;
    }

    return mask;
}
uint64_t ray_bitboard_north_west(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1, j = 1; i < (row + 1) && j < (col + 1); i++, j++) {
        uint64_t tmp_bitboard = bitboard;
        tmp_bitboard <<= (i * 8);
        tmp_bitboard <<= j;

        mask |= tmp_bitboard;
    }

    return mask;
}
uint64_t ray_bitboard_south_east(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1, j = 1; i < (8 - row) && j < (8 - col); i++, j++) {
        uint64_t tmp_bitboard = bitboard;
        tmp_bitboard >>= (i * 8);
        tmp_bitboard >>= j;
        
        mask |= tmp_bitboard;
    }

    return mask;
}
uint64_t ray_bitboard_south_west(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    for (int i = 1, j = 1; i < (8 - row) && j < (col + 1); i++, j++) {
        uint64_t tmp_bitboard = bitboard;
        tmp_bitboard >>= (i * 8);
        tmp_bitboard <<= j;

        mask |= tmp_bitboard;
    }

    return mask;
}

// BLOCKED RAY BITBOARDS
uint64_t block_ray_bitboard_north(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_north to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_north[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = 63 - __builtin_ctzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = closest_blocker_bitboard | (closest_blocker_bitboard - 1);
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}
uint64_t block_ray_bitboard_east(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_east to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_east[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = __builtin_clzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = ~(closest_blocker_bitboard | (closest_blocker_bitboard - 1)) | closest_blocker_bitboard;
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}
uint64_t block_ray_bitboard_south(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_south to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_south[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = __builtin_clzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = ~(closest_blocker_bitboard | (closest_blocker_bitboard - 1)) | closest_blocker_bitboard;
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}
uint64_t block_ray_bitboard_west(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_west to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_west[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = 63 - __builtin_ctzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = closest_blocker_bitboard | (closest_blocker_bitboard - 1);
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}
uint64_t block_ray_bitboard_north_east(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_north_east to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_north_east[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = 63 - __builtin_ctzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = closest_blocker_bitboard | (closest_blocker_bitboard - 1);
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}
uint64_t block_ray_bitboard_north_west(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_north_west to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_north_west[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = __builtin_ctzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = closest_blocker_bitboard | (closest_blocker_bitboard - 1);
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}
uint64_t block_ray_bitboard_south_east(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_south_east to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_south_east[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = __builtin_clzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = ~(closest_blocker_bitboard | (closest_blocker_bitboard - 1)) | closest_blocker_bitboard;
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}
uint64_t block_ray_bitboard_south_west(uint64_t all_blockers, uint64_t bitboard) {
    // requires ray_bitboards_south_west to be filled before running!

    // gets the position of the piece, and its ray bitboard
    int pos = get_bit_pos(bitboard);
    uint64_t ray_bitboard = ray_bitboards_south_west[pos];

    if ((all_blockers & ray_bitboard) == 0) return ray_bitboard;

    // gets the blockers inside of the ray, then gets the closest to the piece
    int closest_blocker_pos = __builtin_clzll(all_blockers & ray_bitboard);

    /*
        generates a new bitboard for the closest blocker,
        then it flips the bits in the direction of the ray -> piece,
        then it ANDs with the ray, and adds the original closest bit back
        to get the blocked ray (inclusive of the blocker)
    */
    uint64_t closest_blocker_bitboard = 0b1000000000000000000000000000000000000000000000000000000000000000 >> closest_blocker_pos;
    closest_blocker_bitboard = ~(closest_blocker_bitboard | (closest_blocker_bitboard - 1)) | closest_blocker_bitboard;
    ray_bitboard &= closest_blocker_bitboard;

    return ray_bitboard;
}

// COMPUTE SIMPLER PIECE MOVEMENTS
uint64_t compute_movements_knight(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    if (row > 1 && col > 0) mask |= bitboard << (2 * 8 + 1);
    if (row > 1 && col < 7) mask |= bitboard << (2 * 8 - 1);

    if (row < 6 && col > 0) mask |= bitboard >> (2 * 8 - 1);
    if (row < 6 && col < 7) mask |= bitboard >> (2 * 8 + 1);

    return mask;
}
uint64_t compute_movements_king(uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t mask = 0b0000000000000000000000000000000000000000000000000000000000000000;

    if (col > 0 && row > 0) mask |= bitboard << (1 * 8 + 1); // top-left
    if (row > 0) mask |= bitboard << (1 * 8);                // top-middle
    if (col < 7 && row > 0) mask |= bitboard << (1 * 8 - 1); // top-right
    
    if (col > 0) mask |= bitboard << 1;                      // middle-left
    if (col < 7) mask |= bitboard >> 1;                      // middle-right

    if (col > 0 && row < 7) mask |= bitboard >> (1 * 8 - 1); // bottom-left
    if (row < 7) mask |= bitboard >> (1 * 8);                // bottom-middle
    if (col < 7 && row < 7) mask |= bitboard >> (1 * 8 + 1); // bottom-right
}

/*
    PIECE MOVEMENTS (everywhere a piece can move up to but inclusive of blockers of both teams)
    dynamic logic needed after this to see whether a given square is a friendly or enemy
    gets most movements from pre-computed arrays
*/
uint64_t movements_pawn_white(uint64_t all_blockers, uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t forward_mask = 0b00000000000000000000000000000000000000000000000000000000;
    uint64_t diagonal_mask = 0b00000000000000000000000000000000000000000000000000000000;

    if (((bitboard << 8) & all_blockers) == 0) {
        if (row < 7) forward_mask |= bitboard << 8;
        if (row == 6 && ((bitboard << 16) & all_blockers) == 0) forward_mask |= bitboard << 16;
    }

    if (col > 0) diagonal_mask |= bitboard << 9;
    if (col < 7) diagonal_mask |= bitboard << 7;

    return forward_mask | (diagonal_mask &= all_blockers);
}
uint64_t movements_pawn_black(uint64_t all_blockers, uint64_t bitboard) {
    int pos = get_bit_pos(bitboard);

    int row = pos / 8; // the row num top-down (0 -> 7)
    int col = pos % 8; // the col num left-right (0 -> 7)

    uint64_t forward_mask = 0b00000000000000000000000000000000000000000000000000000000;
    uint64_t diagonal_mask = 0b00000000000000000000000000000000000000000000000000000000;

    if (((bitboard >> 8) & all_blockers) == 0) {
        if (row > 0) forward_mask |= bitboard >> 8;
        if (row == 1 && ((bitboard >> 16) & all_blockers) == 0) forward_mask |= bitboard >> 16;
    }

    if (col < 7) diagonal_mask |= bitboard >> 9;
    if (col > 0) diagonal_mask |= bitboard >> 7;

    return forward_mask | (diagonal_mask &= all_blockers);
}
uint64_t movements_knight(uint64_t bitboard) {
    // requires precomputed_movements_knight to be filled before running!

    int pos = get_bit_pos(bitboard);
    return precomputed_movements_knight[pos];
}
uint64_t movements_bishop(uint64_t all_blockers, uint64_t bitboard) {
    return (
        block_ray_bitboard_north_east(all_blockers, bitboard) |
        block_ray_bitboard_north_west(all_blockers, bitboard) |
        block_ray_bitboard_south_east(all_blockers, bitboard) |
        block_ray_bitboard_south_west(all_blockers, bitboard)
    );
}
uint64_t movements_rook(uint64_t all_blockers, uint64_t bitboard) {
    return (
        block_ray_bitboard_north(all_blockers, bitboard) |
        block_ray_bitboard_east(all_blockers, bitboard)  |
        block_ray_bitboard_south(all_blockers, bitboard) |
        block_ray_bitboard_west(all_blockers, bitboard)
    );
}
uint64_t movements_queen(uint64_t all_blockers, uint64_t bitboard) {
    return (
        block_ray_bitboard_north(all_blockers, bitboard) |
        block_ray_bitboard_east(all_blockers, bitboard)  |
        block_ray_bitboard_south(all_blockers, bitboard) |
        block_ray_bitboard_west(all_blockers, bitboard)  |
        block_ray_bitboard_north_east(all_blockers, bitboard) |
        block_ray_bitboard_north_west(all_blockers, bitboard) |
        block_ray_bitboard_south_east(all_blockers, bitboard) |
        block_ray_bitboard_south_west(all_blockers, bitboard)
    );
}
uint64_t movements_king(uint64_t bitboard) {
    // requires precomputed_movements_king to be filled before running!

    int pos = get_bit_pos(bitboard);
    return precomputed_movements_king[pos];
}
possible_move_t *allocate_all_possible_moves(board_t board, char curr_team) {
    possible_move_t *current_node = malloc(sizeof(possible_move_t));
    current_node->next = NULL;
    
    possible_move_t *head = current_node;

    uint64_t all_blockers = 0;
    uint64_t friendly_blockers = 0;
    uint64_t enemy_blockers = 0;
    
    // the compiler shouted at me so i have to have the pawn_neg_mask outside the switch
    /* 
        the neg_mask's keep track of previously selected piece positions,
        it gets NOTed then ANDed to the board.pawn_white to get
        the pawns that havnt been selected yet in the correct
        order for __builtin_ctzll to do its magic
    */
    uint64_t pawn_neg_mask = 0;
    uint64_t knight_neg_mask = 0;
    uint64_t bishop_neg_mask = 0;
    uint64_t rook_neg_mask = 0;
    uint64_t queen_neg_mask = 0;
    uint64_t king_neg_mask = 0;

    switch (curr_team) {
        case ('w'):
            friendly_blockers |= board.pawn_white;
            friendly_blockers |= board.knight_white;
            friendly_blockers |= board.bishop_white;
            friendly_blockers |= board.rook_white;
            friendly_blockers |= board.queen_white;
            friendly_blockers |= board.king_white;

            enemy_blockers |= board.pawn_black;
            enemy_blockers |= board.knight_black;
            enemy_blockers |= board.bishop_black;
            enemy_blockers |= board.rook_black;
            enemy_blockers |= board.queen_black;
            enemy_blockers |= board.king_black;

            all_blockers |= friendly_blockers;
            all_blockers |= enemy_blockers;

            // PAWNS
            for (int i = 0, pawn_count = __builtin_popcountll(board.pawn_white); i < pawn_count; i++) {
                uint64_t sel_pawn_pos = 1ULL << __builtin_ctzll(board.pawn_white & ~pawn_neg_mask);
                pawn_neg_mask |= sel_pawn_pos;

                uint64_t movement_options = movements_pawn_white(all_blockers, sel_pawn_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.pawn_white &= ~sel_pawn_pos;
                    new_board.pawn_white |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_black &= ~sel_movement_option;
                            new_board.knight_black &= ~sel_movement_option;
                            new_board.bishop_black &= ~sel_movement_option;
                            new_board.rook_black &= ~sel_movement_option;
                            new_board.queen_black &= ~sel_movement_option;
                            new_board.king_black &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // KNIGHTS
            for (int i = 0, knight_count = __builtin_popcountll(board.knight_white); i < knight_count; i++) {
                uint64_t sel_knight_pos = 1ULL << __builtin_ctzll(board.knight_white & ~knight_neg_mask);
                knight_neg_mask |= sel_knight_pos;

                uint64_t movement_options = movements_knight(sel_knight_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.knight_white &= ~sel_knight_pos;
                    new_board.knight_white |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_black &= ~sel_movement_option;
                            new_board.knight_black &= ~sel_movement_option;
                            new_board.bishop_black &= ~sel_movement_option;
                            new_board.rook_black &= ~sel_movement_option;
                            new_board.queen_black &= ~sel_movement_option;
                            new_board.king_black &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // BISHOPS
            for (int i = 0, bishop_count = __builtin_popcountll(board.bishop_white); i < bishop_count; i++) {
                uint64_t sel_bishop_pos = 1ULL << __builtin_ctzll(board.bishop_white & ~bishop_neg_mask);
                bishop_neg_mask |= sel_bishop_pos;

                uint64_t movement_options = movements_bishop(all_blockers, sel_bishop_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.bishop_white &= ~sel_bishop_pos;
                    new_board.bishop_white |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_black &= ~sel_movement_option;
                            new_board.knight_black &= ~sel_movement_option;
                            new_board.bishop_black &= ~sel_movement_option;
                            new_board.rook_black &= ~sel_movement_option;
                            new_board.queen_black &= ~sel_movement_option;
                            new_board.king_black &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // ROOKS
            for (int i = 0, rook_count = __builtin_popcountll(board.rook_white); i < rook_count; i++) {
                uint64_t sel_rook_pos = 1ULL << __builtin_ctzll(board.rook_white & ~rook_neg_mask);
                rook_neg_mask |= sel_rook_pos;

                uint64_t movement_options = movements_rook(all_blockers, sel_rook_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.rook_white &= ~sel_rook_pos;
                    new_board.rook_white |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_black &= ~sel_movement_option;
                            new_board.knight_black &= ~sel_movement_option;
                            new_board.bishop_black &= ~sel_movement_option;
                            new_board.rook_black &= ~sel_movement_option;
                            new_board.queen_black &= ~sel_movement_option;
                            new_board.king_black &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // QUEENS
            for (int i = 0, queen_count = __builtin_popcountll(board.queen_white); i < queen_count; i++) {
                uint64_t sel_queen_pos = 1ULL << __builtin_ctzll(board.queen_white & ~queen_neg_mask);
                queen_neg_mask |= sel_queen_pos;

                uint64_t movement_options = movements_queen(all_blockers, sel_queen_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.queen_white &= ~sel_queen_pos;
                    new_board.queen_white |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_black &= ~sel_movement_option;
                            new_board.knight_black &= ~sel_movement_option;
                            new_board.bishop_black &= ~sel_movement_option;
                            new_board.rook_black &= ~sel_movement_option;
                            new_board.queen_black &= ~sel_movement_option;
                            new_board.king_black &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // KINGS
            for (int i = 0, king_count = __builtin_popcountll(board.king_white); i < king_count; i++) {
                uint64_t sel_king_pos = 1ULL << __builtin_ctzll(board.king_white & ~king_neg_mask);
                king_neg_mask |= sel_king_pos;

                uint64_t movement_options = movements_king(sel_king_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.king_white &= ~sel_king_pos;
                    new_board.king_white |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_black &= ~sel_movement_option;
                            new_board.knight_black &= ~sel_movement_option;
                            new_board.bishop_black &= ~sel_movement_option;
                            new_board.rook_black &= ~sel_movement_option;
                            new_board.queen_black &= ~sel_movement_option;
                            new_board.king_black &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            break;
        case ('b'):
            friendly_blockers |= board.pawn_black;
            friendly_blockers |= board.knight_black;
            friendly_blockers |= board.bishop_black;
            friendly_blockers |= board.rook_black;
            friendly_blockers |= board.queen_black;
            friendly_blockers |= board.king_black;
            
            enemy_blockers |= board.pawn_white;
            enemy_blockers |= board.knight_white;
            enemy_blockers |= board.bishop_white;
            enemy_blockers |= board.rook_white;
            enemy_blockers |= board.queen_white;
            enemy_blockers |= board.king_white;

            all_blockers |= friendly_blockers;
            all_blockers |= enemy_blockers;

            // PAWNS
            for (int i = 0, pawn_count = __builtin_popcountll(board.pawn_black); i < pawn_count; i++) {
                uint64_t sel_pawn_pos = 1ULL << __builtin_ctzll(board.pawn_black & ~pawn_neg_mask);
                pawn_neg_mask |= sel_pawn_pos;

                uint64_t movement_options = movements_pawn_black(all_blockers, sel_pawn_pos);

                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.pawn_black &= ~sel_pawn_pos;
                    new_board.pawn_black |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_white &= ~sel_movement_option;
                            new_board.knight_white &= ~sel_movement_option;
                            new_board.bishop_white &= ~sel_movement_option;
                            new_board.rook_white &= ~sel_movement_option;
                            new_board.queen_white &= ~sel_movement_option;
                            new_board.king_white &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // KNIGHTS
            for (int i = 0, knight_count = __builtin_popcountll(board.knight_black); i < knight_count; i++) {
                uint64_t sel_knight_pos = 1ULL << __builtin_ctzll(board.knight_black & ~knight_neg_mask);
                knight_neg_mask |= sel_knight_pos;

                uint64_t movement_options = movements_knight(sel_knight_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.knight_black &= ~sel_knight_pos;
                    new_board.knight_black |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_white &= ~sel_movement_option;
                            new_board.knight_white &= ~sel_movement_option;
                            new_board.bishop_white &= ~sel_movement_option;
                            new_board.rook_white &= ~sel_movement_option;
                            new_board.queen_white &= ~sel_movement_option;
                            new_board.king_white &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // BISHOPS
            for (int i = 0, bishop_count = __builtin_popcountll(board.bishop_black); i < bishop_count; i++) {
                uint64_t sel_bishop_pos = 1ULL << __builtin_ctzll(board.bishop_black & ~bishop_neg_mask);
                bishop_neg_mask |= sel_bishop_pos;

                uint64_t movement_options = movements_bishop(all_blockers, sel_bishop_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.bishop_black &= ~sel_bishop_pos;
                    new_board.bishop_black |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_white &= ~sel_movement_option;
                            new_board.knight_white &= ~sel_movement_option;
                            new_board.bishop_white &= ~sel_movement_option;
                            new_board.rook_white &= ~sel_movement_option;
                            new_board.queen_white &= ~sel_movement_option;
                            new_board.king_white &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // ROOKS
            for (int i = 0, rook_count = __builtin_popcountll(board.rook_black); i < rook_count; i++) {
                uint64_t sel_rook_pos = 1ULL << __builtin_ctzll(board.rook_black & ~rook_neg_mask);
                rook_neg_mask |= sel_rook_pos;

                uint64_t movement_options = movements_rook(all_blockers, sel_rook_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.rook_black &= ~sel_rook_pos;
                    new_board.rook_black |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_white &= ~sel_movement_option;
                            new_board.knight_white &= ~sel_movement_option;
                            new_board.bishop_white &= ~sel_movement_option;
                            new_board.rook_white &= ~sel_movement_option;
                            new_board.queen_white &= ~sel_movement_option;
                            new_board.king_white &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // QUEENS
            for (int i = 0, queen_count = __builtin_popcountll(board.queen_black); i < queen_count; i++) {
                uint64_t sel_queen_pos = 1ULL << __builtin_ctzll(board.queen_black & ~queen_neg_mask);
                queen_neg_mask |= sel_queen_pos;

                uint64_t movement_options = movements_queen(all_blockers, sel_queen_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.queen_black &= ~sel_queen_pos;
                    new_board.queen_black |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_white &= ~sel_movement_option;
                            new_board.knight_white &= ~sel_movement_option;
                            new_board.bishop_white &= ~sel_movement_option;
                            new_board.rook_white &= ~sel_movement_option;
                            new_board.queen_white &= ~sel_movement_option;
                            new_board.king_white &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            // KINGS
            for (int i = 0, king_count = __builtin_popcountll(board.king_black); i < king_count; i++) {
                uint64_t sel_king_pos = 1ULL << __builtin_ctzll(board.king_black & ~king_neg_mask);
                king_neg_mask |= sel_king_pos;

                uint64_t movement_options = movements_king(sel_king_pos);


                uint64_t movement_neg_mask = 0;
                for (int j = 0, move_count = __builtin_popcountll(movement_options); j < move_count; j++) {
                    moves_evaluated++;
                    
                    uint64_t sel_movement_option = 1ULL << __builtin_ctzll(movement_options & ~movement_neg_mask);
                    movement_neg_mask |= sel_movement_option;

                    board_t new_board = duplicate_board(board);
                    new_board.king_black &= ~sel_king_pos;
                    new_board.king_black |= sel_movement_option;

                    /*
                        check to see if the sel_movement_option is occupied by
                        a piece of the same team to stop it
                    */

                    if ((friendly_blockers & sel_movement_option) == 0) {
                        /*
                            both movements and captures are returned by the options
                            so for each new board i have to AND the inverse of sel_movement_option
                            with every other bitboard in the board to remove anything that would have
                            been taken as a capture to avoid pieces being on the same square
                            and making a race condition and just being wrong :L
                        */
    
                        if ((enemy_blockers & sel_movement_option) > 0) {
                            new_board.pawn_white &= ~sel_movement_option;
                            new_board.knight_white &= ~sel_movement_option;
                            new_board.bishop_white &= ~sel_movement_option;
                            new_board.rook_white &= ~sel_movement_option;
                            new_board.queen_white &= ~sel_movement_option;
                            new_board.king_white &= ~sel_movement_option;
                        }
    
                        possible_move_t *new_node = malloc(sizeof(possible_move_t));
                        new_node->next = NULL;
                        
                        current_node->move_board = new_board;
                        current_node->next = new_node;
    
                        current_node = new_node;
                    }
                }
            }
            break;
        default:
            printf("INVALID VALUE FOR 'curr_team' IN MINIMAX FUNCTION!\n");
            break;
    }
    return head;
}
void deallocate_all_possible_moves(possible_move_t *head) {
    possible_move_t *current = head;
    possible_move_t *next_node;

    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
}

// PIECE EVALUATIONS
int evaluate_midgame_pawn_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.pawn_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_knight_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.knight_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_bishop_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.bishop_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_rook_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.rook_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_queen_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.queen_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_king_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.king_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_pawn_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.pawn_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_knight_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.knight_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_bishop_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.bishop_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_rook_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.rook_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_queen_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.queen_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_king_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_midgame.king_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_pawn_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.pawn_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_knight_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.knight_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_bishop_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.bishop_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_rook_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.rook_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_queen_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.queen_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_king_white(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.king_white[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_pawn_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.pawn_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_knight_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.knight_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_bishop_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.bishop_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_rook_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.rook_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_queen_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.queen_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_endgame_king_black(uint64_t bitboard) {
    int piece_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if ((bitboard & pos) > 0) {
            piece_eval += pst_endgame.king_black[i];
        }
        pos >>= 1;
    }

    return piece_eval;
}
int evaluate_midgame_board_white(board_t board) {
    // gets the evaluation sum of all the white pieces on the board
    int board_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if      ((board.pawn_white   & pos) > 0) board_eval += pst_midgame.pawn_white[i];
        else if ((board.knight_white & pos) > 0) board_eval += pst_midgame.knight_white[i];
        else if ((board.bishop_white & pos) > 0) board_eval += pst_midgame.bishop_white[i];
        else if ((board.rook_white   & pos) > 0) board_eval += pst_midgame.rook_white[i];
        else if ((board.queen_white  & pos) > 0) board_eval += pst_midgame.queen_white[i];
        else if ((board.king_white   & pos) > 0) board_eval += pst_midgame.king_white[i];
        pos >>= 1;
    }

    board_eval += __builtin_popcountll(board.pawn_white) * PIECE_VALUES_WHITE[0];
    board_eval += __builtin_popcountll(board.knight_white) * PIECE_VALUES_WHITE[1];
    board_eval += __builtin_popcountll(board.bishop_white) * PIECE_VALUES_WHITE[2];
    board_eval += __builtin_popcountll(board.rook_white) * PIECE_VALUES_WHITE[3];
    board_eval += __builtin_popcountll(board.queen_white) * PIECE_VALUES_WHITE[4];
    board_eval += __builtin_popcountll(board.king_white) * PIECE_VALUES_WHITE[5];

    return board_eval;
}
int evaluate_midgame_board_black(board_t board) {
    // gets the evaluation sum of all the black pieces on the board
    int board_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if      ((board.pawn_black   & pos) > 0) board_eval += pst_midgame.pawn_black[i];
        else if ((board.knight_black & pos) > 0) board_eval += pst_midgame.knight_black[i];
        else if ((board.bishop_black & pos) > 0) board_eval += pst_midgame.bishop_black[i];
        else if ((board.rook_black   & pos) > 0) board_eval += pst_midgame.rook_black[i];
        else if ((board.queen_black  & pos) > 0) board_eval += pst_midgame.queen_black[i];
        else if ((board.king_black   & pos) > 0) board_eval += pst_midgame.king_black[i];
        pos >>= 1;
    }

    board_eval += __builtin_popcountll(board.pawn_black) * PIECE_VALUES_BLACK[0];
    board_eval += __builtin_popcountll(board.knight_black) * PIECE_VALUES_BLACK[1];
    board_eval += __builtin_popcountll(board.bishop_black) * PIECE_VALUES_BLACK[2];
    board_eval += __builtin_popcountll(board.rook_black) * PIECE_VALUES_BLACK[3];
    board_eval += __builtin_popcountll(board.queen_black) * PIECE_VALUES_BLACK[4];
    board_eval += __builtin_popcountll(board.king_black) * PIECE_VALUES_BLACK[5];

    return board_eval;
}
int evaluate_endgame_board_white(board_t board) {
    // gets the evaluation sum of all the white pieces on the board
    int board_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if      ((board.pawn_white   & pos) > 0) board_eval += pst_endgame.pawn_white[i];
        else if ((board.knight_white & pos) > 0) board_eval += pst_endgame.knight_white[i];
        else if ((board.bishop_white & pos) > 0) board_eval += pst_endgame.bishop_white[i];
        else if ((board.rook_white   & pos) > 0) board_eval += pst_endgame.rook_white[i];
        else if ((board.queen_white  & pos) > 0) board_eval += pst_endgame.queen_white[i];
        else if ((board.king_white   & pos) > 0) board_eval += pst_endgame.king_white[i];
        pos >>= 1;
    }

    board_eval += __builtin_popcountll(board.pawn_white) * PIECE_VALUES_WHITE[0];
    board_eval += __builtin_popcountll(board.knight_white) * PIECE_VALUES_WHITE[1];
    board_eval += __builtin_popcountll(board.bishop_white) * PIECE_VALUES_WHITE[2];
    board_eval += __builtin_popcountll(board.rook_white) * PIECE_VALUES_WHITE[3];
    board_eval += __builtin_popcountll(board.queen_white) * PIECE_VALUES_WHITE[4];
    board_eval += __builtin_popcountll(board.king_white) * PIECE_VALUES_WHITE[5];

    return board_eval;
}
int evaluate_endgame_board_black(board_t board) {
    // gets the evaluation sum of all the black pieces on the board
    int board_eval = 0;

    uint64_t pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i ++) {
        if      ((board.pawn_black   & pos) > 0) board_eval += pst_endgame.pawn_black[i];
        else if ((board.knight_black & pos) > 0) board_eval += pst_endgame.knight_black[i];
        else if ((board.bishop_black & pos) > 0) board_eval += pst_endgame.bishop_black[i];
        else if ((board.rook_black   & pos) > 0) board_eval += pst_endgame.rook_black[i];
        else if ((board.queen_black  & pos) > 0) board_eval += pst_endgame.queen_black[i];
        else if ((board.king_black   & pos) > 0) board_eval += pst_endgame.king_black[i];
        pos >>= 1;
    }

    board_eval += __builtin_popcountll(board.pawn_black) * PIECE_VALUES_BLACK[0];
    board_eval += __builtin_popcountll(board.knight_black) * PIECE_VALUES_BLACK[1];
    board_eval += __builtin_popcountll(board.bishop_black) * PIECE_VALUES_BLACK[2];
    board_eval += __builtin_popcountll(board.rook_black) * PIECE_VALUES_BLACK[3];
    board_eval += __builtin_popcountll(board.queen_black) * PIECE_VALUES_BLACK[4];
    board_eval += __builtin_popcountll(board.king_black) * PIECE_VALUES_BLACK[5];

    return board_eval;
}
int evaluate_midgame_board(board_t board) {
    return evaluate_midgame_board_white(board) + evaluate_midgame_board_black(board);
}
int evaluate_endgame_board(board_t board) {
    return evaluate_endgame_board_white(board) + evaluate_endgame_board_black(board);
}

// MINIMAX
int minimax(board_t board, int depth, char curr_team) {
    int init_eval = board.move_num > MID_END_GAME_THRESHHOLD ? evaluate_endgame_board(board) : evaluate_midgame_board(board);

    if (depth == 0) return init_eval;

    possible_move_t *possible_moves_head = allocate_all_possible_moves(board, curr_team);
    switch (curr_team) {
        case ('w'):
            int max_eval = -2147483648; // lowest integer value
            for (possible_move_t *node = possible_moves_head; node->next != NULL; node = node->next) {
                int eval = minimax(node->move_board, depth - 1, 'b');
                if (eval > max_eval) max_eval = eval;
            }

            deallocate_all_possible_moves(possible_moves_head);
            return max_eval;
        case ('b'):
            int min_eval = 2147483647; // highest integer value
            int counter = 0;
            for (possible_move_t *node = possible_moves_head; node->next != NULL; node = node->next) {
                int eval = minimax(node->move_board, depth - 1, 'w');
                if (eval < min_eval) min_eval = eval;
            }

            deallocate_all_possible_moves(possible_moves_head);
            return min_eval;
        default:
            printf("INVALID VALUE FOR 'curr_team' IN MINIMAX FUNCTION!\n");
            return 1;
    }
}
board_t get_best_next_move(board_t board, int max_depth, char curr_team) {
    possible_move_t *possible_moves_head = allocate_all_possible_moves(board, curr_team);
    switch (curr_team) {
        case ('w'):
            int max_eval = -2147483648; // lowest integer value
            board_t max_eval_board;
            for (possible_move_t *node = possible_moves_head; node->next != NULL; node = node->next) {
                int board_eval = minimax(node->move_board, max_depth - 1, 'b');
                if (board_eval > max_eval) {
                    max_eval = board_eval;
                    max_eval_board = node->move_board;
                }
            }
            return max_eval_board;
        case ('b'):
            int min_eval = 2147483647; // highest integer value
            board_t min_eval_board;
            for (possible_move_t *node = possible_moves_head; node->next != NULL; node = node->next) {
                int board_eval = minimax(node->move_board, max_depth - 1, 'w');
                if (board_eval < min_eval) {
                    min_eval = board_eval;
                    min_eval_board = node->move_board;
                }
            }
            return min_eval_board;
        default:
            printf("INVALID VALUE FOR 'curr_team' IN GET_BEST_NEXT_MOVE FUNCTION!\n");
            break;
    }
}

// INITS
void init_board() {
    board.pawn_white   = 0b0000000000000000000000000000000000000000000000001111111100000000;
    board.rook_white   = 0b0000000000000000000000000000000000000000000000000000000010000001;
    board.knight_white = 0b0000000000000000000000000000000000000000000000000000000001000010;
    board.bishop_white = 0b0000000000000000000000000000000000000000000000000000000000100100;
    board.king_white   = 0b0000000000000000000000000000000000000000000000000000000000001000;
    board.queen_white  = 0b0000000000000000000000000000000000000000000000000000000000010000;

    board.pawn_black   = 0b0000000011111111000000000000000000000000000000000000000000000000;
    board.rook_black   = 0b1000000100000000000000000000000000000000000000000000000000000000;
    board.knight_black = 0b0100001000000000000000000000000000000000000000000000000000000000;
    board.bishop_black = 0b0010010000000000000000000000000000000000000000000000000000000000;
    board.king_black   = 0b0000100000000000000000000000000000000000000000000000000000000000;
    board.queen_black  = 0b0001000000000000000000000000000000000000000000000000000000000000;

    board.move_num = 0;
}
void init_pst_midgame() {
    copy_pst_arr(PST_MIDGAME_BASE_PAWN, pst_midgame.pawn_white);
    copy_pst_arr(PST_MIDGAME_BASE_KNIGHT, pst_midgame.knight_white);
    copy_pst_arr(PST_MIDGAME_BASE_BISHOP, pst_midgame.bishop_white);
    copy_pst_arr(PST_MIDGAME_BASE_ROOK, pst_midgame.rook_white);
    copy_pst_arr(PST_MIDGAME_BASE_QUEEN, pst_midgame.queen_white);
    copy_pst_arr(PST_MIDGAME_BASE_KING, pst_midgame.king_white);

    copy_pst_arr(PST_MIDGAME_BASE_PAWN_FLIPPED, pst_midgame.pawn_black);
    copy_pst_arr(PST_MIDGAME_BASE_KNIGHT_FLIPPED, pst_midgame.knight_black);
    copy_pst_arr(PST_MIDGAME_BASE_BISHOP_FLIPPED, pst_midgame.bishop_black);
    copy_pst_arr(PST_MIDGAME_BASE_ROOK_FLIPPED, pst_midgame.rook_black);
    copy_pst_arr(PST_MIDGAME_BASE_QUEEN_FLIPPED, pst_midgame.queen_black);
    copy_pst_arr(PST_MIDGAME_BASE_KING_FLIPPED, pst_midgame.king_black);
}
void init_pst_endgame() {
    copy_pst_arr(PST_ENDGAME_BASE_PAWN, pst_endgame.pawn_white);
    copy_pst_arr(PST_ENDGAME_BASE_KNIGHT, pst_endgame.knight_white);
    copy_pst_arr(PST_ENDGAME_BASE_BISHOP, pst_endgame.bishop_white);
    copy_pst_arr(PST_ENDGAME_BASE_ROOK, pst_endgame.rook_white);
    copy_pst_arr(PST_ENDGAME_BASE_QUEEN, pst_endgame.queen_white);
    copy_pst_arr(PST_ENDGAME_BASE_KING, pst_endgame.king_white);

    copy_pst_arr(PST_ENDGAME_BASE_PAWN_FLIPPED, pst_endgame.pawn_black);
    copy_pst_arr(PST_ENDGAME_BASE_KNIGHT_FLIPPED, pst_endgame.knight_black);
    copy_pst_arr(PST_ENDGAME_BASE_BISHOP_FLIPPED, pst_endgame.bishop_black);
    copy_pst_arr(PST_ENDGAME_BASE_ROOK_FLIPPED, pst_endgame.rook_black);
    copy_pst_arr(PST_ENDGAME_BASE_QUEEN_FLIPPED, pst_endgame.queen_black);
    copy_pst_arr(PST_ENDGAME_BASE_KING_FLIPPED, pst_endgame.king_black);
}
void init_precomputed_movements() {
    //the index of the arr is the position (0 -> 63, top_left -> bottom_right)
    uint64_t precomp_pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i++) {
        precomputed_movements_knight[i] = compute_movements_knight(precomp_pos);
        precomputed_movements_king[i]   = compute_movements_king(precomp_pos);
        precomp_pos >>= 1;
    }
}
void init_ray_bitboards() {
    /*
        pre-compute sliding rays for each direction
        the index of the arr is the position (0 -> 63, top_left -> bottom_right)
    */
    uint64_t precomp_pos = 0b1000000000000000000000000000000000000000000000000000000000000000;
    for (int i = 0; i < 64; i++) {
        ray_bitboards_north[i] = ray_bitboard_north(precomp_pos);
        ray_bitboards_east[i]  =  ray_bitboard_east(precomp_pos);
        ray_bitboards_south[i] = ray_bitboard_south(precomp_pos);
        ray_bitboards_west[i]  =  ray_bitboard_west(precomp_pos);
        ray_bitboards_north_east[i] = ray_bitboard_north_east(precomp_pos);
        ray_bitboards_north_west[i] = ray_bitboard_north_west(precomp_pos);
        ray_bitboards_south_east[i] = ray_bitboard_south_east(precomp_pos);
        ray_bitboards_south_west[i] = ray_bitboard_south_west(precomp_pos);
        precomp_pos >>= 1;
    }
}

// GAMEMODES
void EvE() {
    print_board(board);
    for (int i = 0; i < 100; i++) {
        board = get_best_next_move(board, minimax_depth, i % 2 == 0 ? 'w' : 'b');
        clear_lines(9);
        print_board(board);
    }
    clear_lines(9);
}
void PvE() {
    print_board(board);
    while (1) {
        board = user_move_board(board, 'w');
        board.move_num ++;
        clear_lines(9);
        print_board(board);

        board = get_best_next_move(board, minimax_depth, 'b');
        board.move_num ++;
        clear_lines(9);
        print_board(board);
    }
    clear_lines(9);
}
void PvP() {
    print_board(board);
    while (1) {
        board = user_move_board(board, 'w');
        board.move_num ++;
        clear_lines(9);
        print_board(board);

        board = user_move_board(board, 'b');
        board.move_num ++;
        clear_lines(9);
        print_board(board);
    }
    clear_lines(9);
}

int main() {
    init_pst_midgame();
    init_pst_endgame();
    init_precomputed_movements();
    init_ray_bitboards();

    bool keep_menu = true;
    while (keep_menu) {
        init_board();

        printf("                                ,----,.                                                                                         ,---,  \n                              ,'   ,' |                                            ,--,                                      ,`--.' |  \n  ,----..     .--.--.       ,'   .'   |     ,----..              ,----..         ,--.'|     ,---,.   .--.--.      .--.--.    |   :  :  \n /   /   \\   /  /    '.   ,----.'    .'    /   /   \\            /   /   \\     ,--,  | :   ,'  .' |  /  /    '.   /  /    '.  '   '  ;  \n|   :     : |  :  /`. /   |    |   .'     /   .     :          |   :     : ,---.'|  : ' ,---.'   | |  :  /`. /  |  :  /`. /  |   |  |  \n.   |  ;. / ;  |  |--`    :    :  |--,   .   /   ;.  \\         .   |  ;. / |   | : _' | |   |   .' ;  |  |--`   ;  |  |--`   '   :  ;  \n.   ; /--`  |  :  ;_      :    |  ;.' \\ .   ;   /  ` ;         .   ; /--`  :   : |.'  | :   :  |-, |  :  ;_     |  :  ;_     |   |  '  \n;   | ;      \\  \\    `.   |    |      | ;   |  ; \\ ; |         ;   | ;     |   ' '  ; : :   |  ;/|  \\  \\    `.   \\  \\    `.  '   :  |  \n|   : |       `----.   \\  `----'.'\\   ; |   :  | ; | '         |   : |     '   |  .'. | |   :   .'   `----.   \\   `----.   \\ ;   |  ;  \n.   | '___    __ \\  \\  |    __  \\  .  | .   |  ' ' ' :         .   | '___  |   | :  | ' |   |  |-,   __ \\  \\  |   __ \\  \\  | `---'. |  \n'   ; : .'|  /  /`--'  /  /   /\\/  /  : '   ;  \\; /  |         '   ; : .'| '   : |  : ; '   :  ;/|  /  /`--'  /  /  /`--'  /  `--..`;  \n'   | '/  : '--'.     /  / ,,/  ',-   .  \\   \\  ',  /          '   | '/  : |   | '  ,/  |   |    \\ '--'.     /  '--'.     /  .--,_     \n|   :    /    `--'---'   \\ ''\\       ;    ;   :    /           |   :    /  ;   : ;--'   |   :   .'   `--'---'     `--'---'   |    |`.  \n \\   \\ .'                 \\   \\    .'      \\   \\ .'             \\   \\ .'   |   ,/       |   | ,'                             `-- -`, ; \n  `---`                    `--`-,-'         `---`                `---`     '---'        `----'                                 '---`'  \n\n");
        printf("1. Engine Vs Engine\n");
        printf("2. Player Vs Engine\n");
        printf("3. Player Vs Player\n");
        printf("4. Change Engine search depth (current: %i)\n", minimax_depth);
        printf("5. Quit\n");
    
        int menu_choice;
        printf("-> ");
        scanf("%i", &menu_choice);
    
        switch (menu_choice) {
            case (1):
                clear_lines(22);
                EvE();
                break;
            case (2):
                clear_lines(22);
                PvE();
                break;
            case (3):
                clear_lines(22);
                PvP();
                break;
            case (4):
                clear_lines(22);

                printf("New engine depth: \n");

                int new_depth;
                printf("-> ");
                scanf("%i", &new_depth);

                minimax_depth = new_depth;
                clear_lines(3);

                break;
            case (5):
                keep_menu = false;
                break;
            default:
                clear_lines(22);
        }
    }
}
