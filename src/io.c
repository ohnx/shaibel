/* i/o parsing, etc. functions */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "board.h"
#include "move.h"
#include "io.h"

#define square_valid(square) ((square < BOARD_SIZE) && (pos->pieces[square] < PIECE_INVALID))

/*
 * parse in a move string, and return a moveinfo_t (stored in the pointer).
 * returns negative values on failure (e.g. illegal move) and does not touch
 * the prep pointer's value, and 0 if okay (setting the prep pointer's value
 * to the corresponding moveinfo_t)
 */
int io_parse_move(const char *inp, board_t *pos, moveinfo_t *prep) {
    uint8_t from, to;
    uint32_t i;
    movelist_t moves;
    moveinfo_t rep;

    /* input validation */
    if (inp[0] < 'a' || inp[0] > 'h') return -__LINE__;
    if (inp[1] < '1' || inp[1] > '8') return -__LINE__;
    if (inp[2] < 'a' || inp[2] > 'h') return -__LINE__;
    if (inp[3] < '1' || inp[3] > '8') return -__LINE__;

    /* get from and to squares */
    from = filerank_to_square(inp[0] - 'a' + FILE_A, inp[1] - '1' + RANK_1);
    to = filerank_to_square(inp[2] - 'a' + FILE_A, inp[3] - '1' + RANK_1);

    /* double-check that the square we found is valid (it should be!) */
    assert(square_valid(from) && square_valid(to));

    /* generate all currently possible moves */
    movelist_fill(&moves, pos);

    /* loop through all possible moves to see if we found a matching one */
    for (i = 0; i < moves.count; i++) {
        rep = moves.moves[i].rep;
        /* check if from and to squares match */
        if ((moveinfo_get_from(rep) == from) && (moveinfo_get_to(rep) == to)) {
            /* match! */
            if (moveinfo_get_ispromote(rep)) {
                /* this move includes a promoted piece! */
                switch (inp[4]) {
                case 'n':
                    if (piece_is_knight(moveinfo_get_promotepiece(rep))) {
                        /* yay, found a matching move! */
                        goto happy_done;
                    }
                    break;
                case 'b':
                    if (piece_is_bishop(moveinfo_get_promotepiece(rep))) {
                        /* yay, found a matching move! */
                        goto happy_done;
                    }
                    break;
                case 'r':
                    if (piece_is_rook(moveinfo_get_promotepiece(rep))) {
                        /* yay, found a matching move! */
                        goto happy_done;
                    }
                    break;
                case 'q':
                    if (piece_is_queen(moveinfo_get_promotepiece(rep))) {
                        /* yay, found a matching move! */
                        goto happy_done;
                    }
                    break;
                default:
                    /* invalid input from user, actually */
                    return -__LINE__;
                }
            } else {
                /* no promoted piece and the squares/etc. match, so 
                 * we have found the move that we were looking for!
                 */
                goto happy_done;
            }
        }
    }

    /* still nothing? oof. that means that the move was invalid. */
    return -__LINE__;

happy_done:
    *prep = rep;
    return 0;
}
