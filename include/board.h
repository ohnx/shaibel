/**
 * The chess board is represented in two formats; 10x12 array with pieces in
 * each slot (for more intuitive representation), a 64-bit bitfield for some pieces
 * (for easier evaluation of certain positions), and a piece list (for faster move generation).
 */
#ifndef BOARD_INC_H
#define BOARD_INC_H

#include <stdint.h>
#include "move.h"
#include "pvt.h"

/* maximum history of game */
#define MAX_HIST 2048

/* maximum search depth */
#define MAX_SEARCH_DEPTH 64

/* can only have max of 10 pieces of a given type (2 initial, all pawn promotions) */
#define MAX_PIECES_PER_TYPE 10

/* board is 10x12 */
#define BOARD_SIZE 120

/* used for pawn tracking */
#define COLOUR_BLACK 0
#define COLOUR_WHITE 1
#define COLOUR_EITHER 2

/* used for castling checking */
#define CASTLE_WHITE_KING 0x1
#define CASTLE_WHITE_QUEEN 0x2
#define CASTLE_BLACK_KING 0x4
#define CASTLE_BLACK_QUEEN 0x8

/* TODO: define dataype for each piece */

/* each possible piece */
/* TODO: consider using int16_t for a piece, which would allow for
 * bit to indicate white/black, and bit to indicate which piece type
 */
#define PIECE_EMPTY 0
#define PIECE_WHITE_PAWN 1
#define PIECE_WHITE_KNIGHT 2
#define PIECE_WHITE_BISHOP 3
#define PIECE_WHITE_ROOK 4
#define PIECE_WHITE_QUEEN 5
#define PIECE_WHITE_KING 6
#define PIECE_BLACK_PAWN 7
#define PIECE_BLACK_KNIGHT 8
#define PIECE_BLACK_BISHOP 9
#define PIECE_BLACK_ROOK 10
#define PIECE_BLACK_QUEEN 11
#define PIECE_BLACK_KING 12
#define PIECE_INVALID 13

/* todo: could use arrays here */
#define piece_is_pawn(x) ((x) == PIECE_WHITE_PAWN || (x) == PIECE_BLACK_PAWN)
#define piece_is_knight(x) ((x) == PIECE_WHITE_KNIGHT || (x) == PIECE_BLACK_KNIGHT)
#define piece_is_bishop(x) ((x) == PIECE_WHITE_BISHOP || (x) == PIECE_BLACK_BISHOP)
#define piece_is_rook(x) ((x) == PIECE_WHITE_ROOK || (x) == PIECE_BLACK_ROOK)
#define piece_is_queen(x) ((x) == PIECE_WHITE_QUEEN || (x) == PIECE_BLACK_QUEEN)
#define piece_is_king(x) ((x) == PIECE_WHITE_KING || (x) == PIECE_BLACK_KING)
#define piece_colour(x) (((x) > 0 && (x) < PIECE_BLACK_PAWN) ? COLOUR_WHITE : \
                        (((x) > PIECE_WHITE_KING && (x) < PIECE_INVALID) ? \
                        COLOUR_BLACK : COLOUR_EITHER))
#define piece_is_sliding(x) (piece_is_bishop(x) || piece_is_rook(x) || piece_is_queen(x))

/* ranks and files */
#define RANK_1 2
#define RANK_2 3
#define RANK_3 4
#define RANK_4 5
#define RANK_5 6
#define RANK_6 7
#define RANK_7 8
#define RANK_8 9

#define FILE_A 1
#define FILE_B 2
#define FILE_C 3
#define FILE_D 4
#define FILE_E 5
#define FILE_F 6
#define FILE_G 7
#define FILE_H 8

/* all squares */
/* to generate:
var files = "ABCDEFGH";
for (var j = 1; j < 9; j++) {
  for (var i = 0; i < 8; i++) {
    console.log(`#define SQUARE_${files[i]}${j} ${j+1}${i+1}`);
  }
}
*/
#define SQUARE_A1 21
#define SQUARE_B1 22
#define SQUARE_C1 23
#define SQUARE_D1 24
#define SQUARE_E1 25
#define SQUARE_F1 26
#define SQUARE_G1 27
#define SQUARE_H1 28
#define SQUARE_A2 31
#define SQUARE_B2 32
#define SQUARE_C2 33
#define SQUARE_D2 34
#define SQUARE_E2 35
#define SQUARE_F2 36
#define SQUARE_G2 37
#define SQUARE_H2 38
#define SQUARE_A3 41
#define SQUARE_B3 42
#define SQUARE_C3 43
#define SQUARE_D3 44
#define SQUARE_E3 45
#define SQUARE_F3 46
#define SQUARE_G3 47
#define SQUARE_H3 48
#define SQUARE_A4 51
#define SQUARE_B4 52
#define SQUARE_C4 53
#define SQUARE_D4 54
#define SQUARE_E4 55
#define SQUARE_F4 56
#define SQUARE_G4 57
#define SQUARE_H4 58
#define SQUARE_A5 61
#define SQUARE_B5 62
#define SQUARE_C5 63
#define SQUARE_D5 64
#define SQUARE_E5 65
#define SQUARE_F5 66
#define SQUARE_G5 67
#define SQUARE_H5 68
#define SQUARE_A6 71
#define SQUARE_B6 72
#define SQUARE_C6 73
#define SQUARE_D6 74
#define SQUARE_E6 75
#define SQUARE_F6 76
#define SQUARE_G6 77
#define SQUARE_H6 78
#define SQUARE_A7 81
#define SQUARE_B7 82
#define SQUARE_C7 83
#define SQUARE_D7 84
#define SQUARE_E7 85
#define SQUARE_F7 86
#define SQUARE_G7 87
#define SQUARE_H7 88
#define SQUARE_A8 91
#define SQUARE_B8 92
#define SQUARE_C8 93
#define SQUARE_D8 94
#define SQUARE_E8 95
#define SQUARE_F8 96
#define SQUARE_G8 97
#define SQUARE_H8 98
#define SQUARE_NONE 0

/* convert between file/rank to square */
#define filerank_to_square(f, r) ((r)*10 + (f))
/* get nth bit from board */
#define board64_get(b, n) ((b >> n) & 0x1)

/* TODO: come up with name for struct */
/* information necessary to store to be able to undo a move */
typedef struct _undo_move_t {
    moveinfo_t rep; /* representation of move */
    uint8_t castle_perm; /* bitfield of castle permissions (CASTLE_<>) */
    uint8_t enpassant; /* SQUARE_<> for en passant */
    int pawnmovecnt; /* using half moves */
    uint64_t pos_key;
} undo_move_t;

/* main structure to represent the board state at a given time */
typedef struct _board_t {
    /* 10x12 board of pieces */
    uint8_t pieces[BOARD_SIZE];

    /* bitfield of pawns on the board */
    uint64_t pawns[3]; /* indexed by COLOUR_<> */

    /* position (SQUARE_<>) of kings indexed by COLOUR_<> */
    /* NOTE: redundant because can use piece_list[KING_BLAH] */
    /*uint8_t kings[2];*/

    uint8_t side; /* side to move (black or white) */
    uint8_t castle_perm; /* bitfield of castle permissions (CASTLE_<>) */
    uint8_t enpassant; /* SQUARE_<> for en passant */

    uint32_t pawnmovecnt; /* using half moves */
    uint32_t ply; /* number of half moves played in a search */
    uint32_t hist_ply; /* number of half moves played in total */

    uint64_t pos_key; /* position key (unique "hash" of board position) */

    uint8_t big_pieces[3]; /* number of "big" (non-pawn) pieces present, indexed by COLOUR_<> */
    uint8_t major_pieces[3]; /* number of "major" pieces present, indexed by COLOUR_<> */
    uint8_t minor_pieces[3]; /* number of "minor" pieces present, indexed by COLOUR_<> */
    int material_scores[3]; /* scores of materials, indexed by COLOUR_<> */

    /* undo history */
    undo_move_t history[MAX_HIST];

    /* piece list */
    uint8_t num_pieces[13]; /* number of each piece, indexed by PIECE_<> */
    uint8_t piece_list[PIECE_INVALID][MAX_PIECES_PER_TYPE]; /* indexed by PIECE_<>, with values of SQUARE_<> */

    /* principal variation table */
    pvtable_t pvt;
    /* temporary array used to return results of board_pvt_get_chain() */
    /* TODO: probably shouldn't really have this in here; return an array
     * instead maybe? */
    moveinfo_t pvtarr[MAX_SEARCH_DEPTH];

    /* variables to store for optimizations for searching */
    uint32_t search_history[PIECE_INVALID][BOARD_SIZE];
    moveinfo_t search_killers[2][MAX_SEARCH_DEPTH]; /* non-capture moves that cause beta cutoffs */
} board_t;

/* translations for 10x12 board to 64-bit array and vice versa */

/* 10x12 -> 64-bit */
extern uint8_t board120_to_board64[BOARD_SIZE];

/* 10x12 -> rank */
extern uint8_t board120_to_rank[BOARD_SIZE];

/* 10x12 -> file */
extern uint8_t board120_to_file[BOARD_SIZE];

/* 64-bit to 10x12 */
extern uint8_t board64_to_board120[64];

/* TODO: find a better home for this stuff */
/* hashing/poskey info stuff */
extern uint64_t hashtb_pieces[13][120];
extern uint64_t hashtb_side;
extern uint64_t hashtb_castle[16];
/* piece values, etc. */
/* TODO: name this nicely/right */
extern uint8_t big_pieces_lst[PIECE_INVALID];
extern uint8_t maj_pieces_lst[PIECE_INVALID];
extern uint8_t min_pieces_lst[PIECE_INVALID];
extern int piece_vals_lst[PIECE_INVALID];
/* TODO: dup of piece_colour() function */
extern uint8_t piece_colours[PIECE_INVALID];

/* function definitions */
void board64_print(uint64_t board);
#ifdef __has_builtin
#if __has_builtin(__builtin_popcount)
#define board64_count_set(b) (__builtin_popcount((uint64_t)(b)))
#else /* __has_builtin(__builtin_popcount) */
uint8_t _board64_count_set(uint64_t board);
#define board64_count_set(b) _board64_count_set(b)
#endif /* __has_builtin(__builtin_popcount) */
#else /* __has_builtin */
uint8_t _board64_count_set(uint64_t board);
#define board64_count_set(b) _board64_count_set(b)
#endif /* __has_builtin */
uint8_t board64_pop(uint64_t *bb);
/* TODO: can use a static array here. is that better/faster? */
#define board64_set(b, n) ((b) |= (((uint64_t)1) << (n)))
#define board64_clear(b, n) ((b) &= ~(((uint64_t)1) << (n)))
uint64_t board_hash(board_t *pos);
void board_clear(board_t *pos);
int board_init_from_fen(board_t *pos, const char *fen);
void board_print(board_t *pos);
void board_update_material_lists(board_t *pos);
int board_check(board_t *pos);

/* making and undoing moves - see makemove.c for impl */
int board_make_move(board_t *pos, moveinfo_t rep);
void board_undo_move(board_t *pos);

/* pvt function - see pvt.c for impl */
int board_pvt_get_chain(board_t *pos, int depth);

/* evaluate the score of a position - see evaluate.c for impl */
int board_evaluate_position(board_t *pos);

#endif /* BOARD_INC_H */
