/* file to implement various logic bits of the chess engine */

#include <stdio.h>
#include <assert.h>

#include "board.h"
#include "logic.h"
#include "move.h"

/* directions that we need to check for knights, rooks, bishops, and kings (queens are covered by rooks and bishops) */
static int8_t attacksq_knight[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
static int8_t attacksq_rook[4] = {-1, -10, 1, 10};
static int8_t attacksq_bishop[4] = {-9, -11, 9, 11};
static int8_t attacksq_king[8] = {-1, -10, -9, -11, 1, 10, 9, 11};

/* check if a square is under attack by a given side
 * return 1 if under attack, 0 if no.
 */
int logic_under_attack(board_t *pos, uint8_t square, uint8_t side) {
    uint8_t i, piece, check_square;
    assert(square < BOARD_SIZE);
    assert(side < COLOUR_EITHER);
    assert(board_check(pos));
    assert(pos->pieces[square] != PIECE_INVALID);

    /* pawn attack checks */
    if (side == COLOUR_WHITE) {
        /* white pawns can attack diagonally upleft, or upright */
        if (pos->pieces[square - 11] == PIECE_WHITE_PAWN ||
            pos->pieces[square - 9] == PIECE_WHITE_PAWN) {
            return 1;
        }
    } else if (side == COLOUR_BLACK) {
        /* black pawns can attack diagonally downleft, or downright */
        if (pos->pieces[square + 9] == PIECE_BLACK_PAWN ||
            pos->pieces[square + 11] == PIECE_BLACK_PAWN) {
            return 1;
        }
    }

    /* knight attack checks */
    for (i = 0; i < (sizeof(attacksq_knight)/sizeof(attacksq_knight[0])); i++) {
        /* get the piece at this square */
        piece = pos->pieces[square + attacksq_knight[i]];
        /* check if it's a knight of the colour we're looking for */
        if (piece_is_knight(piece) && piece_colour(piece) == side) {
            return 1;
        }
    }

    /* rook/queen checks */
    /* note these pieces are "sliding" so we have to check multiple squares */
    for (i = 0; i < (sizeof(attacksq_rook)/sizeof(attacksq_rook[0])); i++) {
        /* get the initial valid square */
        check_square = square + attacksq_rook[i];
        piece = pos->pieces[check_square];
        /* loop until we go off the board */
        while (piece != PIECE_INVALID) {
            if (piece != PIECE_EMPTY) {
                /* found a piece here! check if it's a queen or rook of
                 * the right colour
                 */
                if ((piece_is_queen(piece) || piece_is_rook(piece)) &&
                    (piece_colour(piece) == side)) {
                    /* under attack! */
                    return 1;
                } else {
                    /* not under attack, but there's a blocking piece now, so
                     * we won't check further
                     */
                    break;
                }
            }

            /* keep checking next square */
            check_square += attacksq_rook[i];
            piece = pos->pieces[check_square];
        }
    }

    /* bishop/queen checks */
    /* note these pieces are "sliding" so we have to check multiple squares */
    for (i = 0; i < (sizeof(attacksq_bishop)/sizeof(attacksq_bishop[0])); i++) {
        /* get the initial valid square */
        check_square = square + attacksq_bishop[i];
        piece = pos->pieces[check_square];
        /* loop until we go off the board */
        while (piece != PIECE_INVALID) {
            if (piece != PIECE_EMPTY) {
                /* found a piece here! check if it's a queen or rook of
                 * the right colour
                 */
                if ((piece_is_queen(piece) || piece_is_bishop(piece)) &&
                    (piece_colour(piece) == side)) {
                    /* under attack! */
                    return 1;
                } else {
                    /* not under attack, but there's a blocking piece now, so
                     * we won't check further
                     */
                    break;
                }
            }

            /* keep checking next square */
            check_square += attacksq_bishop[i];
            piece = pos->pieces[check_square];
        } 
    }

    /* king attack checks */
    for (i = 0; i < (sizeof(attacksq_king)/sizeof(attacksq_king[0])); i++) {
        /* get the piece at this square */
        piece = pos->pieces[square + attacksq_king[i]];
        /* check if it's a king of the colour we're looking for */
        if (piece_is_king(piece) && piece_colour(piece) == side) {
            return 1;
        }
    }

    /* not under attack */
    return 0;
}

/* return non-zero value if there has been a repeated position in the last
 * "pawnmovecnt" moves, and 0 otherwise
 */
uint32_t logic_repeated_move(board_t *pos) {
    uint32_t i;

    assert(pos->hist_ply < MAX_HIST);
    assert(pos->hist_ply >= pos->pawnmovecnt);

    /* loop through all history entries since the most recent 50 move rule reset
     * to the most recent move (excluding that one, though) */
    for (i = pos->hist_ply - pos->pawnmovecnt; (i + 1) < pos->hist_ply; i++) {
        if (pos->pos_key == pos->history[i].pos_key) {
            /* found an identical position key! */
            return i;
        }
    }

    /* reached the end of the history search with no matches */
    return 0;
}

/* returns 1 if the move is legal, and 0 if the move is not. */
int logic_legal_move(board_t *pos, moveinfo_t rep) {
    movelist_t moves;
    int i;

    /* first, generate all possible moves */
    movelist_fill(&moves, pos);

    /* loop through all the possible moves found */
    for (i = 0; i < moves.count; i++) {
        /* check if the move matches */
        if (rep == moves.moves[i].rep) {
            /* try to make the move */
            if (!board_make_move(pos, rep)) {
                /* move all ok! undo move & return */
                board_undo_move(pos);
                return 1;
            } else {
                /* tried to make the same move, but it's illegal */
                return 0;
            }
        }
    }

    /* move isn't even possible right now */
    return 0;
}

