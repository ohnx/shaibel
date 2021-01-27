/* implementation of super simple hashing algorithm for uniquely identifying board positions */

#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "board.h"

/* TODO: automatic generation */
/* 13 unique pieces, each with 120 possible positions */
uint64_t hashtb_pieces[13][120];
/* the side that's to play */
uint64_t hashtb_side;
/* castle information - 4 toggleable bits, so 2^4 possible values = 16 */
uint64_t hashtb_castle[16];

#define RAND_64() ((uint64_t)rand() | (((uint64_t)rand()) << 15) | \
                  (((uint64_t)rand()) << 30) | (((uint64_t)rand()) << 45) | \
                  (((uint64_t)rand() & 0xf) << 60))

int board_hash_init() {
    int i, j;

    srand(time(NULL));

    for (i = 0; i < 13; i++) {
        for (j = 0; j < 120; j++) {
            hashtb_pieces[i][j] = RAND_64();
        }
    }

    hashtb_side = RAND_64();

    for (i = 0; i < 16; i++) {
        hashtb_castle[i] = RAND_64();
    }

    return 0;
}

/* hash a board */
uint64_t board_hash(board_t *pos) {
    uint64_t h = 0;
    uint8_t square, piece;

    /* pieces */
    for (square = 0; square < BOARD_SIZE; square++) {
        piece = pos->pieces[square];
        if (piece != PIECE_INVALID && piece != PIECE_EMPTY) {
            assert(piece < PIECE_INVALID);
            h ^= hashtb_pieces[piece][square];
        }
    }

    /* side to move */
    if (pos->side) h ^= hashtb_side;

    /* en passant */
    if (pos->enpassant != SQUARE_NONE) {
        assert(pos->enpassant < BOARD_SIZE);
        h ^= hashtb_pieces[PIECE_EMPTY][pos->enpassant];
    }

    /* castling */
    assert(pos->castle_perm <= 0xF && pos->castle_perm >= 0);
    h ^= hashtb_castle[pos->castle_perm];

    return h;
}
