/* this file contains the logic on representing moves & generating possible moves */
#include <stdio.h>
#include <assert.h>

#include "board.h"
#include "logic.h"
#include "move.h"

/* least valuable attacker (mvvlva) computation */
const int victim_scores[PIECE_INVALID] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};
static int mvvlva_scores[PIECE_INVALID][PIECE_INVALID];

void move_mvvlva_init() {
    uint8_t apiece, vpiece;

    for (apiece = PIECE_EMPTY + 1; apiece < PIECE_INVALID; apiece++) {
        for (vpiece = PIECE_EMPTY + 1; vpiece < PIECE_INVALID; vpiece++) {
            mvvlva_scores[vpiece][apiece] = victim_scores[vpiece] + 6 - (victim_scores[apiece] / 100);
        }
    }
}

/* print out the move info */
void moveinfo_print_stdout(moveinfo_t rep) {
    uint8_t from_file, from_rank, to_file, to_rank, prompiece;
    char promchar;

    /* get the basic information on the square we move to/from */
    from_file = board120_to_file[moveinfo_get_from(rep)];
    from_rank = board120_to_rank[moveinfo_get_from(rep)];
    to_file = board120_to_file[moveinfo_get_to(rep)];
    to_rank = board120_to_rank[moveinfo_get_to(rep)];

    /* output the first part of the move first */
    fprintf(stdout, "%c%c%c%c", ('a' + from_file - FILE_A),
                                ('1' + from_rank - RANK_1),
                                ('a' + to_file - FILE_A),
                                ('1' + to_rank - RANK_1));

    if (moveinfo_get_ispromote(rep)) {
        /* if it's a promotion, we also need to mention the piece it was promoted to */
        prompiece = moveinfo_get_promotepiece(rep);
        /* default to queen promotion */
        promchar = 'q';
        if (piece_is_bishop(prompiece)) {
            promchar = 'b';
        } else if (piece_is_knight(prompiece)) {
            promchar = 'n';
        } else if (piece_is_rook(prompiece)) {
            promchar = 'r';
        }
        fprintf(stdout, "%c", promchar);
    }
}
void moveinfo_print(moveinfo_t rep) {
    uint8_t from_file, from_rank, to_file, to_rank, prompiece;
    char promchar;

    /* get the basic information on the square we move to/from */
    from_file = board120_to_file[moveinfo_get_from(rep)];
    from_rank = board120_to_rank[moveinfo_get_from(rep)];
    to_file = board120_to_file[moveinfo_get_to(rep)];
    to_rank = board120_to_rank[moveinfo_get_to(rep)];

    /* output the first part of the move first */
    fprintf(stderr, "%c%c%c%c", ('a' + from_file - FILE_A),
                                ('1' + from_rank - RANK_1),
                                ('a' + to_file - FILE_A),
                                ('1' + to_rank - RANK_1));

    if (moveinfo_get_ispromote(rep)) {
        /* if it's a promotion, we also need to mention the piece it was promoted to */
        prompiece = moveinfo_get_promotepiece(rep);
        /* default to queen promotion */
        promchar = 'q';
        if (piece_is_bishop(prompiece)) {
            promchar = 'b';
        } else if (piece_is_knight(prompiece)) {
            promchar = 'n';
        } else if (piece_is_rook(prompiece)) {
            promchar = 'r';
        }
        fprintf(stderr, "%c", promchar);
    }
}

void movelist_print(movelist_t *moves) {
    uint32_t i;

    for (i = 0; i < moves->count; i++) {
        fprintf(stderr, "Move %u (score %d): ", i, moves->moves[i].score);
        moveinfo_print(moves->moves[i].rep);
        fprintf(stderr, "\n");
    }
}

/* add a quiet move */
static int movelist_add_quiet(movelist_t *moves, board_t *pos, moveinfo_t rep) {
    /* TODO: maybe this should be an assert instead of if check? */
    if (moves->count >= MOVELIST_MAX_LEN) return -1;

    assert(pos->pieces[moveinfo_get_from(rep)] < PIECE_INVALID);
    assert(pos->pieces[moveinfo_get_to(rep)] == PIECE_EMPTY);

    /* add a quiet move */
    moves->moves[moves->count].rep = rep;
    
    /* first take a look at the "killer moves" */
    if (pos->search_killers[0][pos->ply] == rep) {
        moves->moves[moves->count].score = MOVE_SCORE_KILLER_FIRST;
    } else if (pos->search_killers[1][pos->ply] == rep) {
        moves->moves[moves->count].score = MOVE_SCORE_KILLER_SECOND;
    } else {
        /* search history */
        moves->moves[moves->count].score = pos->search_history[pos->pieces[moveinfo_get_from(rep)]][moveinfo_get_to(rep)];
    }

    /* increment number of total moves */
    moves->count++;

    /* ok */
    return 0;
}

/* add a capture move */
static int movelist_add_capture(movelist_t *moves, board_t *pos, moveinfo_t rep) {
    /* TODO: maybe this should be an assert instead of if check? */
    if (moves->count >= MOVELIST_MAX_LEN) return -1;

    assert(pos->pieces[moveinfo_get_from(rep)] < PIECE_INVALID);
    assert(pos->pieces[moveinfo_get_to(rep)] < PIECE_INVALID);
    assert(pos->pieces[moveinfo_get_to(rep)] > PIECE_EMPTY);

    /* add a capture move */
    moves->moves[moves->count].rep = rep;
    
    /* get the score of the move from the mvvlva lookup table */
    moves->moves[moves->count].score = MOVE_SCORE_CAPTURE_BASE + mvvlva_scores[moveinfo_get_capturepiece(rep)][pos->pieces[moveinfo_get_from(rep)]];

    /* increment number of total moves */
    moves->count++;

    /* ok */
    return 0;
}

/* add an en passant move */
static int movelist_add_enpassant(movelist_t *moves, board_t *pos, moveinfo_t rep) {
    /* TODO: maybe this should be an assert instead of if check? */
    if (moves->count >= MOVELIST_MAX_LEN) return -1;

    /* add an en passant move */
    moves->moves[moves->count].rep = rep;
    
    /* the score here is just that of a pawn taking a pawn (colour can be ignored) */
    moves->moves[moves->count].score = MOVE_SCORE_CAPTURE_BASE + mvvlva_scores[PIECE_WHITE_PAWN][PIECE_WHITE_PAWN];

    /* increment number of total moves */
    moves->count++;

    /* ok */
    return 0;
}

static int movelist_add_white_pawn_move(movelist_t *moves, board_t *pos, uint8_t from, uint8_t to, uint8_t capture) {
    int r = 0;

    if (board120_to_rank[to] == RANK_8) {
        /* will also be promoting to 4 possible pieces */
        if (capture) {
            /* capturing! */
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_WHITE_KNIGHT, 0));
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_WHITE_BISHOP, 0));
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_WHITE_ROOK, 0));
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_WHITE_QUEEN, 0));
        } else {
            /* regular move */
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_WHITE_KNIGHT, 0));
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_WHITE_BISHOP, 0));
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_WHITE_ROOK, 0));
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_WHITE_QUEEN, 0));
        }
    } else {
        /* no promotion, so only a single move needed */
        if (capture) {
            /* capturing! */
            r += movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_EMPTY, 0));
        } else {
            /* regular move */
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_EMPTY, 0));
        }
    }

    return r;
}

static int movelist_add_black_pawn_move(movelist_t *moves, board_t *pos, uint8_t from, uint8_t to, uint8_t capture) {
    int r = 0;

    if (board120_to_rank[to] == RANK_1) {
        /* will also be promoting to 4 possible pieces */
        if (capture) {
            /* capturing! */
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_BLACK_KNIGHT, 0));
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_BLACK_BISHOP, 0));
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_BLACK_ROOK, 0));
            movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_BLACK_QUEEN, 0));
        } else {
            /* regular move */
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_BLACK_KNIGHT, 0));
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_BLACK_BISHOP, 0));
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_BLACK_ROOK, 0));
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_BLACK_QUEEN, 0));
        }
    } else {
        /* no promotion, so only a single move needed */
        if (capture) {
            /* capturing! */
            r += movelist_add_capture(moves, pos, moveinfo_new(from, to, pos->pieces[to], 0, 0, PIECE_EMPTY, 0));
        } else {
            /* regular move */
            movelist_add_quiet(moves, pos, moveinfo_new(from, to, PIECE_EMPTY, 0, 0, PIECE_EMPTY, 0));
        }
    }

    return r;
}

/* list of sliding pieces (black first, then white) */
static uint8_t sliding_pieces[COLOUR_EITHER][4] = {
    {PIECE_BLACK_BISHOP, PIECE_BLACK_ROOK, PIECE_BLACK_QUEEN, PIECE_EMPTY},
    {PIECE_WHITE_BISHOP, PIECE_WHITE_ROOK, PIECE_WHITE_QUEEN, PIECE_EMPTY}
};

/* list of non-sliding pieces (black first, then white) */
static uint8_t non_sliding_pieces[COLOUR_EITHER][3] = {
    {PIECE_BLACK_KNIGHT, PIECE_BLACK_KING, PIECE_EMPTY},
    {PIECE_WHITE_KNIGHT, PIECE_WHITE_KING, PIECE_EMPTY}
};

/* list of squares that each piece can move */
static int8_t knight_dirs[] = {-8, -19, -21, -12, 8, 19, 21, 12, 0};
static int8_t bishop_dirs[] = {-9, -11, 9, 11, 0};
static int8_t rook_dirs[] = {-1, -10, 1, 10, 0};
static int8_t queen_dirs[] = {-1, -9, -10, -11, 1, 9, 10, 11, 0};

static int8_t *piece_dirs[PIECE_INVALID] = {
    NULL, NULL, knight_dirs, bishop_dirs, rook_dirs, queen_dirs, queen_dirs,
    NULL, knight_dirs, bishop_dirs, rook_dirs, queen_dirs, queen_dirs
};

/* fill a move list with all possible moves in the board */
int32_t movelist_fill(movelist_t *moves, board_t *pos) {
    uint8_t i, j, square, piece, target_square, target_piece, target_colour, side;
    int8_t *dir;

    /* check that stuff is ok */
    assert(board_check(pos));

    /* clear the move list */
    moves->count = 0;

    /* get the side to move */
    side = pos->side;

    /* TODO: is it possible to generate a move where the king is captured? what
     * are the implications of this if yes?
     */

    /* check the side to move */
    if (side == COLOUR_WHITE) {
        /* explore where all white pawns are */
        for (i = 0; i < pos->num_pieces[PIECE_WHITE_PAWN]; i++) {
            square = pos->piece_list[PIECE_WHITE_PAWN][i];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == PIECE_WHITE_PAWN));
            
            /* check for forwards move */
            if (pos->pieces[square + 10] == PIECE_EMPTY) {
                /* the square above is empty, so we can move to it! */
                movelist_add_white_pawn_move(moves, pos, square, square + 10, 0);
                /* if we're in the 2nd rank and the 4th rank is also empty, we can double move */
                if ((board120_to_rank[square] == RANK_2) && (pos->pieces[square + 20] == PIECE_EMPTY)) {
                    movelist_add_quiet(moves, pos, moveinfo_new(square, square + 20, PIECE_EMPTY, 0, 1, PIECE_EMPTY, 0));
                }
            }

            /* now check for diagonal capturing moves */
            if ((pos->pieces[square + 9] != PIECE_EMPTY) && (piece_colour(pos->pieces[square + 9]) == COLOUR_BLACK)) {
                movelist_add_white_pawn_move(moves, pos, square, square + 9, 1);
            }
            if ((pos->pieces[square + 11] != PIECE_EMPTY) && (piece_colour(pos->pieces[square + 11]) == COLOUR_BLACK)) {
                movelist_add_white_pawn_move(moves, pos, square, square + 11, 1);
            }

            /* finally, check for en passant capturing moves */
            if (square + 9 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square + 9, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
            if (square + 11 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square + 11, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
        }

        /* logic for castling */
        if (pos->castle_perm & CASTLE_WHITE_KING) {
            /* king-side castling available */
            /* first ensure empty spaces are available */
            if ((pos->pieces[SQUARE_F1] == PIECE_EMPTY) && (pos->pieces[SQUARE_G1] == PIECE_EMPTY)) {
                /* then ensure squares are not under attack */
                if ((!logic_under_attack(pos, SQUARE_E1, COLOUR_BLACK)) &&
                    (!logic_under_attack(pos, SQUARE_F1, COLOUR_BLACK))) {
                    movelist_add_quiet(moves, pos, moveinfo_new(
                        SQUARE_E1, SQUARE_G1, PIECE_EMPTY, 0, 0, PIECE_EMPTY, 1
                    ));
                }
            }
        }
        if (pos->castle_perm & CASTLE_WHITE_QUEEN) {
            /* queen-side castling available */
            /* first ensure empty spaces are available */
            if ((pos->pieces[SQUARE_D1] == PIECE_EMPTY) &&
                (pos->pieces[SQUARE_C1] == PIECE_EMPTY) &&
                (pos->pieces[SQUARE_B1] == PIECE_EMPTY)) {
                /* then ensure squares are not under attack */
                if ((!logic_under_attack(pos, SQUARE_E1, COLOUR_BLACK)) &&
                    (!logic_under_attack(pos, SQUARE_D1, COLOUR_BLACK))) {
                    movelist_add_quiet(moves, pos, moveinfo_new(
                        SQUARE_E1, SQUARE_C1, PIECE_EMPTY, 0, 0, PIECE_EMPTY, 1
                    ));
                }
            }
        }
    } else {
        /* explore where all black pawns are */
        for (i = 0; i < pos->num_pieces[PIECE_BLACK_PAWN]; i++) {
            square = pos->piece_list[PIECE_BLACK_PAWN][i];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == PIECE_BLACK_PAWN));
            
            /* check for forwards move */
            if (pos->pieces[square - 10] == PIECE_EMPTY) {
                /* the square below is empty, so we can move to it! */
                movelist_add_black_pawn_move(moves, pos, square, square - 10, 0);
                /* if we're in the 7th rank and the 5th rank is also empty, we can double move */
                if ((board120_to_rank[square] == RANK_7) && (pos->pieces[square - 20] == PIECE_EMPTY)) {
                    movelist_add_quiet(moves, pos, moveinfo_new(square, square - 20, PIECE_EMPTY, 0, 1, PIECE_EMPTY, 0));
                }
            }

            /* now check for diagonal capturing moves */
            if ((pos->pieces[square - 9] != PIECE_EMPTY) && (piece_colour(pos->pieces[square - 9]) == COLOUR_WHITE)) {
                movelist_add_black_pawn_move(moves, pos, square, square - 9, 1);
            }
            if ((pos->pieces[square - 11] != PIECE_EMPTY) && (piece_colour(pos->pieces[square - 11]) == COLOUR_WHITE)) {
                movelist_add_black_pawn_move(moves, pos, square, square - 11, 1);
            }

            /* finally, check for en passant capturing moves */
            if (square - 9 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square - 9, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
            if (square - 11 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square - 11, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
        }

        /* logic for castling */
        if (pos->castle_perm & CASTLE_BLACK_KING) {
            /* king-side castling available */
            /* first ensure empty spaces are available */
            if ((pos->pieces[SQUARE_F8] == PIECE_EMPTY) &&
                (pos->pieces[SQUARE_G8] == PIECE_EMPTY)) {
                /* then ensure squares are not under attack */
                if ((!logic_under_attack(pos, SQUARE_E8, COLOUR_WHITE)) &&
                    (!logic_under_attack(pos, SQUARE_F8, COLOUR_WHITE))) {
                    movelist_add_quiet(moves, pos, moveinfo_new(
                        SQUARE_E8, SQUARE_G8, PIECE_EMPTY, 0, 0, PIECE_EMPTY, 1
                    ));
                }
            }
        }
        if (pos->castle_perm & CASTLE_BLACK_QUEEN) {
            /* queen-side castling available */
            /* first ensure empty spaces are available */
            if ((pos->pieces[SQUARE_D8] == PIECE_EMPTY) &&
                (pos->pieces[SQUARE_C8] == PIECE_EMPTY) &&
                (pos->pieces[SQUARE_B8] == PIECE_EMPTY)) {
                /* then ensure squares are not under attack */
                if ((!logic_under_attack(pos, SQUARE_E8, COLOUR_WHITE)) &&
                    (!logic_under_attack(pos, SQUARE_D8, COLOUR_WHITE))) {
                    movelist_add_quiet(moves, pos, moveinfo_new(
                        SQUARE_E8, SQUARE_C8, PIECE_EMPTY, 0, 0, PIECE_EMPTY, 1
                    ));
                }
            }
        }
    }

    /* sliding pieces */
    for (i = 0; (piece = sliding_pieces[side][i]) != PIECE_EMPTY; i++) {
        /* loop through all instances of this sliding piece on the board */
        for (j = 0; j < pos->num_pieces[piece]; j++) {
            /* get the square that this piece is on */
            square = pos->piece_list[piece][j];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == piece));
            assert(piece_dirs[piece] != NULL);

            /* loop through all the squares that this piece can move to */
            for (dir = piece_dirs[piece]; *dir; dir++) {
                /* target square */
                target_square = square + *dir;

                /* target piece */
                target_piece = pos->pieces[target_square];

                /* loop until we've jumped off board */
                while (target_piece != PIECE_INVALID) {
                    /* get piece colour */
                    target_colour = piece_colour(target_piece);

                    if (target_colour == side) {
                        /* eating own side if we jump here, so don't */
                        break;
                    } else if (target_colour == COLOUR_EITHER) {
                        /* colour is nonexistent, so square is empty */
                        assert(target_piece == PIECE_EMPTY);
                        movelist_add_quiet(moves, pos, moveinfo_new(
                            square, target_square,
                            PIECE_EMPTY, 0, 0, PIECE_EMPTY, 0
                        ));
                    } else {
                        /* different colours; capture */
                        movelist_add_capture(moves, pos, moveinfo_new(
                            square, target_square,
                            target_piece, 0, 0, PIECE_EMPTY, 0
                        ));
                        /* can not move further; move would be blocked by this piece */
                        break;
                    }

                    /* keep looping! update target square */
                    target_square += *dir;

                    /* also update target piece */
                    target_piece = pos->pieces[target_square];
                }
            }
        }
    }

    /* non-sliding pieces */
    for (i = 0; (piece = non_sliding_pieces[side][i]) != PIECE_EMPTY; i++) {
        /* loop through all instances of this non-sliding piece on the board */
        for (j = 0; j < pos->num_pieces[piece]; j++) {
            /* get the square that this piece is on */
            square = pos->piece_list[piece][j];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == piece));
            assert(piece_dirs[piece] != NULL);

            /* loop through all the squares that this piece can move to */
            for (dir = piece_dirs[piece]; *dir; dir++) {
                /* target square */
                target_square = square + *dir;

                /* target piece */
                target_piece = pos->pieces[target_square];

                /* jumped off board */
                if (target_piece == PIECE_INVALID) continue;

                /* get piece colour */
                target_colour = piece_colour(target_piece);

                if (target_colour == side) {
                    /* eating own side if we jump here, so don't */
                    continue;
                } else if (target_colour == COLOUR_EITHER) {
                    /* colour is nonexistent, so square is empty */
                    assert(target_piece == PIECE_EMPTY);
                    movelist_add_quiet(moves, pos, moveinfo_new(
                        square, target_square,
                        PIECE_EMPTY, 0, 0, PIECE_EMPTY, 0
                    ));
                } else {
                    /* different colours; capture */
                    movelist_add_capture(moves, pos, moveinfo_new(
                        square, target_square,
                        target_piece, 0, 0, PIECE_EMPTY, 0
                    ));
                }
            }
        }
    }

    return 0;
}

/* fill a move list with only capture moves in the board 
 * copy paste of above function, basically
 */
int32_t movelist_fill_capture(movelist_t *moves, board_t *pos) {
    uint8_t i, j, square, piece, target_square, target_piece, target_colour, side;
    int8_t *dir;

    /* check that stuff is ok */
    assert(board_check(pos));

    /* clear the move list */
    moves->count = 0;

    /* get the side to move */
    side = pos->side;

    /* TODO: is it possible to generate a move where the king is captured? what
     * are the implications of this if yes?
     */

    /* check the side to move */
    if (side == COLOUR_WHITE) {
        /* explore where all white pawns are */
        for (i = 0; i < pos->num_pieces[PIECE_WHITE_PAWN]; i++) {
            square = pos->piece_list[PIECE_WHITE_PAWN][i];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == PIECE_WHITE_PAWN));

            /* now check for diagonal capturing moves */
            if ((pos->pieces[square + 9] != PIECE_EMPTY) && (piece_colour(pos->pieces[square + 9]) == COLOUR_BLACK)) {
                movelist_add_white_pawn_move(moves, pos, square, square + 9, 1);
            }
            if ((pos->pieces[square + 11] != PIECE_EMPTY) && (piece_colour(pos->pieces[square + 11]) == COLOUR_BLACK)) {
                movelist_add_white_pawn_move(moves, pos, square, square + 11, 1);
            }

            /* finally, check for en passant capturing moves */
            if (square + 9 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square + 9, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
            if (square + 11 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square + 11, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
        }
    } else {
        /* explore where all black pawns are */
        for (i = 0; i < pos->num_pieces[PIECE_BLACK_PAWN]; i++) {
            square = pos->piece_list[PIECE_BLACK_PAWN][i];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == PIECE_BLACK_PAWN));

            /* now check for diagonal capturing moves */
            if ((pos->pieces[square - 9] != PIECE_EMPTY) && (piece_colour(pos->pieces[square - 9]) == COLOUR_WHITE)) {
                movelist_add_black_pawn_move(moves, pos, square, square - 9, 1);
            }
            if ((pos->pieces[square - 11] != PIECE_EMPTY) && (piece_colour(pos->pieces[square - 11]) == COLOUR_WHITE)) {
                movelist_add_black_pawn_move(moves, pos, square, square - 11, 1);
            }

            /* finally, check for en passant capturing moves */
            if (square - 9 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square - 9, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
            if (square - 11 == pos->enpassant) {
                movelist_add_enpassant(moves, pos, moveinfo_new(square, square - 11, PIECE_EMPTY, 1, 0, PIECE_EMPTY, 0));
            }
        }
    }

    /* sliding pieces */
    for (i = 0; (piece = sliding_pieces[side][i]) != PIECE_EMPTY; i++) {
        /* loop through all instances of this sliding piece on the board */
        for (j = 0; j < pos->num_pieces[piece]; j++) {
            /* get the square that this piece is on */
            square = pos->piece_list[piece][j];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == piece));
            assert(piece_dirs[piece] != NULL);

            /* loop through all the squares that this piece can move to */
            for (dir = piece_dirs[piece]; *dir; dir++) {
                /* target square */
                target_square = square + *dir;

                /* target piece */
                target_piece = pos->pieces[target_square];

                /* loop until we've jumped off board */
                while (target_piece != PIECE_INVALID) {
                    /* get piece colour */
                    target_colour = piece_colour(target_piece);

                    if (target_colour == side) {
                        /* eating own side if we jump here, so don't */
                        break;
                    } else if (target_colour == COLOUR_EITHER) {
                        /* colour is nonexistent, so square is empty */
                        assert(target_piece == PIECE_EMPTY);
                    } else {
                        /* different colours; capture */
                        movelist_add_capture(moves, pos, moveinfo_new(
                            square, target_square,
                            target_piece, 0, 0, PIECE_EMPTY, 0
                        ));
                        /* can not move further; move would be blocked by this piece */
                        break;
                    }

                    /* keep looping! update target square */
                    target_square += *dir;

                    /* also update target piece */
                    target_piece = pos->pieces[target_square];
                }
            }
        }
    }

    /* non-sliding pieces */
    for (i = 0; (piece = non_sliding_pieces[side][i]) != PIECE_EMPTY; i++) {
        /* loop through all instances of this non-sliding piece on the board */
        for (j = 0; j < pos->num_pieces[piece]; j++) {
            /* get the square that this piece is on */
            square = pos->piece_list[piece][j];
            assert((square < BOARD_SIZE) && (pos->pieces[square] == piece));
            assert(piece_dirs[piece] != NULL);

            /* loop through all the squares that this piece can move to */
            for (dir = piece_dirs[piece]; *dir; dir++) {
                /* target square */
                target_square = square + *dir;

                /* target piece */
                target_piece = pos->pieces[target_square];

                /* jumped off board */
                if (target_piece == PIECE_INVALID) continue;

                /* get piece colour */
                target_colour = piece_colour(target_piece);

                if (target_colour == side) {
                    /* eating own side if we jump here, so don't */
                    continue;
                } else if (target_colour == COLOUR_EITHER) {
                    /* colour is nonexistent, so square is empty */
                    assert(target_piece == PIECE_EMPTY);
                } else {
                    /* different colours; capture */
                    movelist_add_capture(moves, pos, moveinfo_new(
                        square, target_square,
                        target_piece, 0, 0, PIECE_EMPTY, 0
                    ));
                }
            }
        }
    }

    return 0;
}

