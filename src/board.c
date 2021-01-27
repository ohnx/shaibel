/* implementation of various different board formats used to store data */

#include <stdio.h>
#include <assert.h>

#include "board.h"

/* to generate:
var cnt = 0;
var out = "uint8_t board120_to_board64[BOARD_SIZE] = {\n";
for (var j = 0; j < 12; j++) {
  out += "    ";
  for (var i = 0; i < 10; i++) {
    if (j >= 2 && j <= 9 && i >= 1 && i <= 8) {
      out += `${cnt++}, `;
    } else {
      out += "65, ";
    }
  }
  out = out.slice(0, -1) + '\n';
}
out = out.slice(0,-2) + "\n};";
console.log(out);
*/
uint8_t board120_to_board64[BOARD_SIZE] = {
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 0, 1, 2, 3, 4, 5, 6, 7, 65,
    65, 8, 9, 10, 11, 12, 13, 14, 15, 65,
    65, 16, 17, 18, 19, 20, 21, 22, 23, 65,
    65, 24, 25, 26, 27, 28, 29, 30, 31, 65,
    65, 32, 33, 34, 35, 36, 37, 38, 39, 65,
    65, 40, 41, 42, 43, 44, 45, 46, 47, 65,
    65, 48, 49, 50, 51, 52, 53, 54, 55, 65,
    65, 56, 57, 58, 59, 60, 61, 62, 63, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65, 65, 65
};

/* to generate:
var cnt = 0;
var out = "uint8_t board120_to_rank[BOARD_SIZE] = {\n";
for (var j = 0; j < 12; j++) {
  out += "    ";
  for (var i = 0; i < 10; i++) {
    if (j >= 2 && j <= 9 && i >= 1 && i <= 8) {
      out += `RANK_${j-1}, `;
    } else {
      out += "0, ";
    }
  }
  out = out.slice(0, -1) + '\n';
}
out = out.slice(0,-2) + "\n};";
console.log(out);
*/
uint8_t board120_to_rank[BOARD_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, RANK_1, RANK_1, RANK_1, RANK_1, RANK_1, RANK_1, RANK_1, RANK_1, 0,
    0, RANK_2, RANK_2, RANK_2, RANK_2, RANK_2, RANK_2, RANK_2, RANK_2, 0,
    0, RANK_3, RANK_3, RANK_3, RANK_3, RANK_3, RANK_3, RANK_3, RANK_3, 0,
    0, RANK_4, RANK_4, RANK_4, RANK_4, RANK_4, RANK_4, RANK_4, RANK_4, 0,
    0, RANK_5, RANK_5, RANK_5, RANK_5, RANK_5, RANK_5, RANK_5, RANK_5, 0,
    0, RANK_6, RANK_6, RANK_6, RANK_6, RANK_6, RANK_6, RANK_6, RANK_6, 0,
    0, RANK_7, RANK_7, RANK_7, RANK_7, RANK_7, RANK_7, RANK_7, RANK_7, 0,
    0, RANK_8, RANK_8, RANK_8, RANK_8, RANK_8, RANK_8, RANK_8, RANK_8, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* to generate:
var cnt = 0;
var files = "ABCDEFGH";
var out = "uint8_t board120_to_file[BOARD_SIZE] = {\n";
for (var j = 0; j < 12; j++) {
  out += "    ";
  for (var i = 0; i < 10; i++) {
    if (j >= 2 && j <= 9 && i >= 1 && i <= 8) {
      out += `FILE_${files[i-1]}, `;
    } else {
      out += "0, ";
    }
  }
  out = out.slice(0, -1) + '\n';
}
out = out.slice(0,-2) + "\n};";
console.log(out);
*/
uint8_t board120_to_file[BOARD_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* to generate: 
var out = "uint8_t board64_to_board120[64] = {\n";
var files = "ABCDEFGH";
for (var j = 1; j < 9; j++) {
  out += "    ";
  for (var i = 0; i < 8; i++) {
    out += `${j+1}${i+1}, `;
  }
  out = out.slice(0, -1) + '\n';
}
out = out.slice(0,-2) + "\n};";
console.log(out);
*/
uint8_t board64_to_board120[64] = {
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98
};

/* print out the board to stdout */
void board64_print(uint64_t board) {
    uint8_t rank, file;

    for (rank = RANK_8; rank >= RANK_1; rank--) {
        for (file = FILE_A; file <= FILE_H; file++) {
            if (board64_get(board,
                            board120_to_board64[filerank_to_square(file, rank)])
            ) {
                fprintf(stderr, "X");
            } else {
                fprintf(stderr, "-");
            }
        }
        fprintf(stderr, "\n");
    }
}

/* count number of bits set in a board */
uint8_t _board64_count_set(uint64_t board) {
    uint8_t r;

    for (r = 0; board; r++) {
        board &= board - 1;
    }

    return r;
}

/* ??? TODO: rewatch part 9 ~ 1:50 and figure out what this code does */
/* code from https://www.chessprogramming.org/Looking_for_Magics */
static const uint8_t BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

uint8_t board64_pop(uint64_t *bb) {
    uint64_t b = *bb ^ (*bb - 1);
    uint32_t fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

/* clear a board, mostly */
void board_clear(board_t *pos) {
    uint8_t i, j;

    /* sanity check */
    assert(pos);

    /* pieces array */
    for (i = 0; i < BOARD_SIZE; i++) {
        /* set everything invalid first */
        pos->pieces[i] = PIECE_INVALID;
    }

    for (i = 0; i < 64; i++) {
        /* then set some empty */
        pos->pieces[board64_to_board120[i]] = PIECE_EMPTY;
    }

    /* set various arrays to 0 */
    for (i = 0; i < 3; i++) {
        pos->pawns[i] = 0;
        pos->big_pieces[i] = 0;
        pos->major_pieces[i] = 0;
        pos->minor_pieces[i] = 0;
        pos->material_scores[i] = 0;
    }
    for (i = 0; i < 13; i++) {
        pos->num_pieces[i] = 0;
    }
    for (i = 0; i < PIECE_INVALID; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            pos->search_history[i][j] = 0;
        }
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < MAX_SEARCH_DEPTH; j++) {
            pos->search_killers[i][j] = MOVE_NONE;
        }
    }

    /* misc other variables */
    pos->side = COLOUR_EITHER;
    pos->castle_perm = 0;
    pos->enpassant = SQUARE_NONE;
    pos->pawnmovecnt = 0;
    pos->ply = 0;
    pos->hist_ply = 0;
    pos->pos_key = 0;

    /* pvt */
    pvt_clear(&(pos->pvt));
}

int board_init_from_fen(board_t *pos, const char *fen) {
    uint8_t rank;
    uint8_t file;
    uint8_t piece;
    uint8_t nbr;
    uint8_t i;

    /* sanity check */
    assert(pos);
    assert(fen);

    /* clear the board state first */
    board_clear(pos);

    /* start at A8 */
    rank = RANK_8;
    file = FILE_A;

    /* loop through and consume each rank */
    for (; (rank >= RANK_1) && *fen; fen++) {
        /* how many of this piece we have */
        nbr = 1;

        switch (*fen) {
        /* pieces */
        case 'p': piece = PIECE_BLACK_PAWN; break;
        case 'n': piece = PIECE_BLACK_KNIGHT; break;
        case 'b': piece = PIECE_BLACK_BISHOP; break;
        case 'r': piece = PIECE_BLACK_ROOK; break;
        case 'q': piece = PIECE_BLACK_QUEEN; break;
        case 'k': piece = PIECE_BLACK_KING; break;
        case 'P': piece = PIECE_WHITE_PAWN; break;
        case 'N': piece = PIECE_WHITE_KNIGHT; break;
        case 'B': piece = PIECE_WHITE_BISHOP; break;
        case 'R': piece = PIECE_WHITE_ROOK; break;
        case 'Q': piece = PIECE_WHITE_QUEEN; break;
        case 'K': piece = PIECE_WHITE_KING; break;

        /* empty squares */
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            piece = PIECE_EMPTY;
            nbr = *fen - '0';
            break;

        /* next rank */
        case '/':
        case ' ':
            rank--;
            file = FILE_A;
            continue;

        default:
            /* TODO: error logger */
            fprintf(stderr, "Malformed FEN: Unexpected character %c\n", *fen);
            return -__LINE__;
        }

        for (i = 0; i < nbr; i++) {
            /* write the pieces out */
            assert(file <= FILE_H);
            assert(pos->pieces[filerank_to_square(file, rank)] == PIECE_EMPTY);
            pos->pieces[filerank_to_square(file, rank)] = piece;
            file++;
        }
    }
    
    /* ensure we didn't reach end of string */
    if (!(*fen)) {
        fprintf(stderr, "Malformed FEN: Unexpected end of string\n");
        return -__LINE__;
    }

    /* side to move */
    switch (*fen) {
    case 'w': pos->side = COLOUR_WHITE; break;
    case 'b': pos->side = COLOUR_BLACK; break;
    default:
        fprintf(stderr, "Malformed FEN: Unexpected character %c\n", *fen);
        return -__LINE__;
    }
    /* skip past space and the colour specifier */
    fen += 2;

    /* ensure we didn't reach end of string */
    if (!(*fen)) {
        fprintf(stderr, "Malformed FEN: Unexpected end of string\n");
        return -__LINE__;
    }

    /* parse castling permissions */
    for (i = 0; i < 4; i++, fen++) {
        switch (*fen) {
        case 0:
        case ' ':
            /* exit loop */
            goto castle_loop_done;
        /* specific castle permission */
        case 'K': pos->castle_perm |= CASTLE_WHITE_KING; break;
        case 'Q': pos->castle_perm |= CASTLE_WHITE_QUEEN; break;
        case 'k': pos->castle_perm |= CASTLE_BLACK_KING; break;
        case 'q': pos->castle_perm |= CASTLE_BLACK_QUEEN; break;
        /* no castle permissions */
        case '-': continue;
        /* bad fen */
        default:
            fprintf(stderr, "Malformed FEN: Unexpected character %c\n", *fen);
            return -__LINE__;
        }
    }

castle_loop_done:
    /* ensure we didn't reach end of string */
    if (!(*fen)) {
        fprintf(stderr, "Malformed FEN: Unexpected end of string\n");
        return -__LINE__;
    } else if (*fen != ' ') {
        fprintf(stderr, "Malformed FEN: Unexpected character %c\n", *fen);
        return -__LINE__;
    }

    /* skip past space */
    fen++;

    /* ensure parsed correctly */
    assert(pos->castle_perm >= 0 && pos->castle_perm <= 0xf);

    /* parse en passsant square */
    if (*fen != '-') {
        file = *(fen++) - 'a' + FILE_A;
        rank = *(fen++) - '1' + RANK_1;
        if (file > FILE_H || file < FILE_A || rank > RANK_8 || rank < RANK_1) {
            fprintf(stderr, "Malformed FEN: Bad square %c%c\n", *(fen-2), *(fen-1));
            return -__LINE__;
        }
        pos->enpassant = filerank_to_square(file, rank);
        /* TODO: ensure rank of en passant square is valid (3 or 6; depending 
         * on side to move; see board_check())
         */
    } else {
        fen += 2;
    }

    /* ensure we didn't reach end of string */
    if (!(*fen)) {
        fprintf(stderr, "Malformed FEN: Unexpected end of string\n");
        return -__LINE__;
    }

    /* TODO: parse # of moves, etc. */

    /* compute position hash */
    pos->pos_key = board_hash(pos);

    /* update move lists */
    board_update_material_lists(pos);

    return 0;
}

/* print out a board */
static const char *pieces_chars = ".PNBRQKpnbrqk!";
static const char *side_char = "bw!";
static const char *rank_char = "!!12345678";
static const char *file_char = "!ABCDEFGH";

void board_print(board_t *pos) {
    uint8_t square, file, rank, piece;

    /* top file headings */
    fprintf(stderr, "    ");
    for (file = FILE_A; file <= FILE_H; file++) {
        fprintf(stderr, "%c ", file_char[file]);
    }
    fprintf(stderr, "\n");

    /* board itself */
    for (rank = RANK_8; rank >= RANK_1; rank--) {
        fprintf(stderr, " %c  ", rank_char[rank]);
        for (file = FILE_A; file <= FILE_H; file++) {
            square = filerank_to_square(file, rank);
            piece = pos->pieces[square];
            assert(piece <= PIECE_INVALID);
            fprintf(stderr, "%c ", pieces_chars[piece]);
        }
        fprintf(stderr, "\n");
    }

    /* additional info */
    fprintf(stderr, "side: %c, enpassant: %c%c, castle: %c%c%c%c\n",
            side_char[pos->side],
            pos->enpassant == SQUARE_NONE ? '-' : board120_to_file[pos->enpassant] + 'a' - FILE_A,
            pos->enpassant == SQUARE_NONE ? '-' : board120_to_rank[pos->enpassant] + '1' - RANK_1,
            (pos->castle_perm & CASTLE_WHITE_KING)? 'K' : '-',
            (pos->castle_perm & CASTLE_WHITE_QUEEN)? 'Q' : '-',
            (pos->castle_perm & CASTLE_BLACK_KING)? 'k' : '-',
            (pos->castle_perm & CASTLE_BLACK_QUEEN)? 'q' : '-');
    fprintf(stderr, "poskey: %lX\n", pos->pos_key);
}

/* values of which pieces are considered big, major, minor, and values */
uint8_t big_pieces_lst[PIECE_INVALID] = {0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1};
uint8_t maj_pieces_lst[PIECE_INVALID] = {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1};
uint8_t min_pieces_lst[PIECE_INVALID] = {0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0};
int piece_vals_lst[PIECE_INVALID] = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
uint8_t piece_colours[PIECE_INVALID] = {COLOUR_EITHER, COLOUR_WHITE,
                                        COLOUR_WHITE, COLOUR_WHITE,
                                        COLOUR_WHITE, COLOUR_WHITE,
                                        COLOUR_WHITE, COLOUR_BLACK,
                                        COLOUR_BLACK, COLOUR_BLACK,
                                        COLOUR_BLACK, COLOUR_BLACK,
                                        COLOUR_BLACK};

/* update the piece lists in a board_t structure */
void board_update_material_lists(board_t *pos) {
    uint8_t i, piece, colour;

    for (i = 0; i < BOARD_SIZE; i++) {
        piece = pos->pieces[i];
        if (piece != PIECE_EMPTY && piece != PIECE_INVALID) {
            assert(piece < PIECE_INVALID);
            /* interesting piece here! */
            colour = piece_colours[piece];
            assert(colour < COLOUR_EITHER);

            /* check if the piece falls into any of the categories */
            if (big_pieces_lst[piece]) pos->big_pieces[colour]++;
            if (maj_pieces_lst[piece]) pos->major_pieces[colour]++;
            if (min_pieces_lst[piece]) pos->minor_pieces[colour]++;

            /* update material scores */
            pos->material_scores[colour] += piece_vals_lst[piece];

            /* update piece list */
            pos->piece_list[piece][pos->num_pieces[piece]++] = i;

            /* update pawn bitboard position */
            if (piece == PIECE_WHITE_PAWN) {
                board64_set(pos->pawns[COLOUR_WHITE], board120_to_board64[i]);
                board64_set(pos->pawns[COLOUR_EITHER], board120_to_board64[i]);
            } else if (piece == PIECE_BLACK_PAWN) {
                board64_set(pos->pawns[COLOUR_BLACK], board120_to_board64[i]);
                board64_set(pos->pawns[COLOUR_EITHER], board120_to_board64[i]);
            }
        }
    }
}

/* verify if a board is valid or not
 * always returns 1 (board OK), as it will run assert()'s on board validity,
 * which will fail if the board is invalid.
 */
int board_check(board_t *pos) {
    uint8_t piece, i, square, square64, piece_colour;
    uint8_t piece_counts[PIECE_INVALID], big_pieces[3], maj_pieces[3], min_pieces[3];
    int mat_scores[3];

    /* clear arrays */
    for (piece = PIECE_EMPTY; piece < PIECE_INVALID; piece++) {
        piece_counts[piece] = 0;
    }

    /* clear arrays */
    for (i = 0; i < 3; i++) {
        big_pieces[i] = maj_pieces[i] = min_pieces[i] = mat_scores[i] = 0;
    }

    /* iterate through entire board and do some checks */
    for (square = 0; square < BOARD_SIZE; square++) {
        piece = pos->pieces[square];
        square64 = board120_to_board64[square];
        if (square64 == 65) {
            /* invalid square; ensure it has an invalid value */
            assert(piece == PIECE_INVALID);
        } else { /* piece in the board, check consistency with other fields */
            assert(piece < PIECE_INVALID);

            /* colour of piece */
            piece_colour = piece_colours[piece];

            /* big/major/minor piece counts */
            if (big_pieces_lst[piece]) big_pieces[piece_colour]++;
            if (maj_pieces_lst[piece]) maj_pieces[piece_colour]++;
            if (min_pieces_lst[piece]) min_pieces[piece_colour]++;

            /* total material values */
            mat_scores[piece_colour] += piece_vals_lst[piece];

            /* pawn bitboard correctness */
            if (piece == PIECE_BLACK_PAWN) {
                /* should have bitboard set here only where appropriate */
                assert(board64_get(pos->pawns[COLOUR_BLACK], square64));
                assert(board64_get(pos->pawns[COLOUR_EITHER], square64));
            } else if (piece == PIECE_WHITE_PAWN) {
                /* should have bitboard set here only where appropriate */
                assert(board64_get(pos->pawns[COLOUR_WHITE], square64));
                assert(board64_get(pos->pawns[COLOUR_EITHER], square64));
            } else {
                /* should have empty/non-set bitboard here */
                for (i = 0; i < 3; i++) {
                    assert(!board64_get(pos->pawns[i], square64));
                }
            }

            /* piece counts */
            if (piece != PIECE_EMPTY) piece_counts[piece]++;
        }
    }

    /* check that count arrays are consistent */
    for (i = 0; i < 3; i++) {
        assert(big_pieces[i] == pos->big_pieces[i]);
        assert(maj_pieces[i] == pos->major_pieces[i]);
        assert(min_pieces[i] == pos->minor_pieces[i]);
        assert(mat_scores[i] == pos->material_scores[i]);
    }

    /* check piece lists and consistency with board itself */
    for (piece = PIECE_EMPTY; piece < PIECE_INVALID; piece++) {
        /* check that the num_pieces is correct */
        assert(pos->num_pieces[piece] == piece_counts[piece]);
        for (i = 0; i < pos->num_pieces[piece]; i++) {
            square = pos->piece_list[piece][i];
            /* check that the piece in the piece list is also in the board */
            assert(piece == pos->pieces[square]);
        }
    }

    /* check position key is accurate */
    assert(board_hash(pos) == pos->pos_key);

    /* check there is a side to move */
    assert(pos->side == COLOUR_WHITE || pos->side == COLOUR_BLACK);

    /* check that en passant is valid */
    if (pos->enpassant != SQUARE_NONE) {
        if (pos->side == COLOUR_WHITE) {
            /* white to move; en passant square's rank should be 6 */
            assert(board120_to_rank[pos->enpassant] == RANK_6);
        } else {
            /* black to move; en passant square's rank should be 3 */
            assert(board120_to_rank[pos->enpassant] == RANK_3);
        }
    }

    /* all checks passed */
    return 1;
}



