/* this file contains the logic on evaluating a position's score */
#include <stdio.h>
#include <assert.h>

#include "board.h"

/* TODO "machine learning" this and tweak :)) */

/* piece-square tables */
/* note that these tables are for a single side, and assume that pawns of
 * that colour go from top to bottom */
static const int pawn_table[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    10, 10, 0, -10, -10, 0, 10, 10,
    5, 0, 0, 5, 5, 0, 0, 5,
    0, 0, 10, 20, 20, 10, 0, 0,
    5, 5, 5, 10, 10, 5, 5, 5,
    10, 10, 10, 20, 20, 10, 10, 10,
    20, 20, 20, 30, 30, 20, 20, 20,
    0, 0, 0, 0, 0, 0, 0, 0	
};

static const int knight_table[64] = {
    0, -10, 0, 0, 0, 0, -10, 0,
    0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 10, 10, 10, 10, 0, 0,
    0, 0, 10, 20, 20, 10, 5, 0,
    5, 10, 15, 20, 20, 15, 10, 5,
    5, 10, 10, 20, 20, 10, 10, 5,
    0, 0, 5, 10, 10, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0		
};

static const int bishop_table[64] = {
    0, 0, -10, 0, 0, -10, 0, 0,
    0, 0, 0, 10, 10, 0, 0, 0,
    0, 0, 10, 15, 15, 10, 0, 0,
    0, 10, 15, 20, 20, 15, 10, 0,
    0, 10, 15, 20, 20, 15, 10, 0,
    0, 0, 10, 15, 15, 10, 0, 0,
    0, 0, 0, 10, 10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0	
};

static const int rook_table[64] = {
    0, 0, 5, 10, 10, 5, 0, 0,
    0, 0, 5, 10, 10, 5, 0, 0,
    0, 0, 5, 10, 10, 5, 0, 0,
    0, 0, 5, 10, 10, 5, 0, 0,
    0, 0, 5, 10, 10, 5, 0, 0,
    0, 0, 5, 10, 10, 5, 0, 0,
    25, 25, 25, 25, 25, 25, 25, 25,
    0, 0, 5, 10, 10, 5, 0, 0		
};

static const int black_mirror[64] = {
    56, 57, 58, 59, 60, 61, 62, 63,
    48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47,
    32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31,
    16, 17, 18, 19, 20, 21, 22, 23,
    8, 9, 10, 11, 12, 13, 14, 15,
    0, 1, 2, 3, 4, 5, 6, 7
};

static const int *piece_pos_scores[PIECE_INVALID] = {
    NULL,
    pawn_table, knight_table, bishop_table, rook_table, NULL, NULL,
    pawn_table, knight_table, bishop_table, rook_table, NULL, NULL
};

/* evaluate the score of a position */
int board_evaluate_position(board_t *pos) {
    int score;
    const int *pst_lookup;
    uint8_t piece, i, square, square64;

    /* base score is calculated from base material values */
    score = pos->material_scores[COLOUR_WHITE] - pos->material_scores[COLOUR_BLACK];

    /* sum up all the additional points from pieces in certain positions */
    /* loop through all piece types */
    for (piece = PIECE_EMPTY+1; piece < PIECE_INVALID; piece++) {
        /* kings and queens aren't scored through this */
        if (piece_is_king(piece) || piece_is_queen(piece)) continue;

        /* loop through each piece of that type */
        for (i = 0; i < pos->num_pieces[piece]; i++) {
            /* get that square in board64 format */
            square = pos->piece_list[piece][i];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == piece));
            square64 = board120_to_board64[square];

            /* get the array to fetch the score from */
            pst_lookup = piece_pos_scores[piece];
            assert(pst_lookup != NULL);

            if (piece_colour(piece) == COLOUR_BLACK) {
                /* need to mirror the square if we're black */
                square64 = black_mirror[square64];
                /* also, need to subtract */
                score -= pst_lookup[square64];
            } else {
                /* white adds to the score here */
                score += pst_lookup[square64];
            }
        }
    }

    /* TODO: add a lot more stuff here ... */

    /* invert the score if black is to move */
    if (pos->side == COLOUR_BLACK) {
        score = -score;
    }

    return score;
}

