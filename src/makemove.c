/* this file contains the logic on moves & making them */
#include <stdio.h>
#include <assert.h>

#include "board.h"
#include "logic.h"
#include "move.h"

/* commonly used statements */
#define hash_piece(piece, square) (pos->pos_key ^= hashtb_pieces[piece][square])
#define hash_castle() (pos->pos_key ^= hashtb_castle[pos->castle_perm])
#define hash_side() (pos->pos_key ^= hashtb_side)
#define hash_enpassant() (pos->pos_key ^= hashtb_pieces[PIECE_EMPTY][pos->enpassant])
#define square_valid(square) ((square < BOARD_SIZE) && (pos->pieces[square] < PIECE_INVALID))
#define piece_valid(piece) (piece < PIECE_INVALID)

/* castle permissions - this board lists the castle permissions that we keep
 * whenever we move a piece in this square
 */
static uint8_t castle_perm[BOARD_SIZE] = {
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~CASTLE_WHITE_QUEEN, ~0, ~0, ~0, ~(CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN), ~0, ~0, ~CASTLE_WHITE_KING, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~CASTLE_BLACK_QUEEN, ~0, ~0, ~0, ~(CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN), ~0, ~0, ~CASTLE_BLACK_KING, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0
};

/* helper functions */
/* clear a piece from the board */
static void mm_clear_piece(board_t *pos, uint8_t square) {
    uint8_t colour, piece, i, fidx;

    assert(square_valid(square));
    assert(pos->pieces[square] != PIECE_EMPTY);

    /* set variables */
    piece = pos->pieces[square];
    colour = piece_colour(piece);

    /* hash out the piece */
    hash_piece(piece, square);

    /* update the board120 rep */
    pos->pieces[square] = PIECE_EMPTY;

    /* remove piece from material score */
    pos->material_scores[colour] -= piece_vals_lst[piece];

    /* check what type of piece this is */
    if (big_pieces_lst[piece]) {
        /* it is a "big piece", so decrease the count of them */
        pos->big_pieces[colour]--;
        if (maj_pieces_lst[piece]) {
            /* it is a "major piece", so decrease the count of them */
            pos->major_pieces[colour]--;
        } else {
            /* it is a "minor piece", so decrease the count of them */
            pos->minor_pieces[colour]--;
        }
    } else {
        /* it is a pawn */
        assert(piece_is_pawn(piece));

        /* clear the pawn from the board64 pawns */
        board64_clear(pos->pawns[colour], board120_to_board64[square]);
        board64_clear(pos->pawns[COLOUR_EITHER], board120_to_board64[square]);
    }

    /* remove the piece from the piece list */
    fidx = MAX_PIECES_PER_TYPE;
    /* first, scan the piece list for the desired square */
    for (i = 0; i < pos->num_pieces[piece]; i++) {
        if (pos->piece_list[piece][i] == square) {
            /* found the right square! */
            fidx = i;
            break;
        }
    }
    /* make sure the list found was right */
    assert(fidx < MAX_PIECES_PER_TYPE);

    /* decrease the number of pieces */
    pos->num_pieces[piece]--;
    /* sub in the last item to the position at which we found the item to remove
     * neat trick to avoid having to copy everything after this position
     * forwards 1 unit
     */
    pos->piece_list[piece][fidx] = pos->piece_list[piece][pos->num_pieces[piece]];

    /* all done! */
    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);
}

/* add a piece to the board */
static void mm_add_piece(board_t *pos, uint8_t square, uint8_t piece) {
    uint8_t colour;

    assert(square_valid(square));
    assert(piece_valid(piece));
    assert(pos->pieces[square] == PIECE_EMPTY);

    /* set variables */
    colour = piece_colour(piece);

    /* hash in the piece */
    hash_piece(piece, square);

    /* update the board120 rep */
    pos->pieces[square] = piece;

    /* add piece to material score */
    pos->material_scores[colour] += piece_vals_lst[piece];

    /* check what type of piece this is */
    if (big_pieces_lst[piece]) {
        /* it is a "big piece", so increase the count of them */
        pos->big_pieces[colour]++;
        if (maj_pieces_lst[piece]) {
            /* it is a "major piece", so increase the count of them */
            pos->major_pieces[colour]++;
        } else {
            /* it is a "minor piece", so increase the count of them */
            pos->minor_pieces[colour]++;
        }
    } else {
        /* it is a pawn */
        assert(piece_is_pawn(piece));

        /* add the pawn to the board64 pawns */
        board64_set(pos->pawns[colour], board120_to_board64[square]);
        board64_set(pos->pawns[COLOUR_EITHER], board120_to_board64[square]);
    }

    /* add the piece to the piece list and increase the length of the list */
    pos->piece_list[piece][pos->num_pieces[piece]++] = square;

    /* all done! */
    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);
}

/* move a piece on the board */
static void mm_move_piece(board_t *pos, uint8_t from, uint8_t to) {
    uint8_t colour, piece, i, fidx;

    assert(square_valid(from) && square_valid(to));
    /* expects to move to empty square */
    assert(pos->pieces[to] == PIECE_EMPTY);

    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);

    /* set variables */
    piece = pos->pieces[from];
    colour = piece_colour(piece);

    /* hash in/out the piece */
    hash_piece(piece, from);
    hash_piece(piece, to);

    /* update the board120 rep */
    pos->pieces[from] = PIECE_EMPTY;
    pos->pieces[to] = piece;

    /* check what type of piece this is */
    if (piece_is_pawn(piece)) {
        /* it is a pawn, so we need to update the bitboards */
        /* clear the pawn from the board64 pawns */
        board64_clear(pos->pawns[colour], board120_to_board64[from]);
        board64_clear(pos->pawns[COLOUR_EITHER], board120_to_board64[from]);
        /* add the pawn to the board64 pawns */
        board64_set(pos->pawns[colour], board120_to_board64[to]);
        board64_set(pos->pawns[COLOUR_EITHER], board120_to_board64[to]);
    }

    /* find the piece in the piece list */
    fidx = MAX_PIECES_PER_TYPE;
    /* first, scan the piece list for the desired square */
    for (i = 0; i < pos->num_pieces[piece]; i++) {
        if (pos->piece_list[piece][i] == from) {
            /* found the right square! */
            fidx = i;
            break;
        }
    }
    /* make sure the list found was right */
    assert(fidx < MAX_PIECES_PER_TYPE);

    /* update the square of "from" to "to" */
    pos->piece_list[piece][fidx] = to;

    /* all done! */
    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);
}

/* make a move - will return 1 if the move makes the king of the side making
 * the move in check (and also refuse to make the move), and 0 if the move is
 * okay.
 *
 * NB: implicitly assumes the move is OK besides potentially making the king in
 * check (i.e., it is possible to make this function make any piece move to any
 * arbitrary place on the board just by supplying the rep with the right from
 * and to squares)
 */
int board_make_move(board_t *pos, moveinfo_t rep) {
    uint8_t from, to, side, cappiece, prompiece, kingsq;
    undo_move_t *hist_ent;

    assert(board_check(pos));
    assert(square_valid(moveinfo_get_from(rep)));
    assert(square_valid(moveinfo_get_to(rep)));
    assert(piece_valid(moveinfo_get_capturepiece(rep)));

    /* set variables */
    from = moveinfo_get_from(rep);
    to = moveinfo_get_to(rep);
    cappiece = moveinfo_get_capturepiece(rep);
    prompiece = moveinfo_get_promotepiece(rep);
    side = pos->side;

    assert(pos->pieces[from] != PIECE_EMPTY);
    assert((pos->pieces[to] == PIECE_EMPTY) || (pos->pieces[to] == cappiece));

    /* store the history */
    hist_ent = &(pos->history[pos->hist_ply]);
    hist_ent->rep = rep;
    hist_ent->castle_perm = pos->castle_perm;
    hist_ent->enpassant = pos->enpassant;
    hist_ent->pawnmovecnt = pos->pawnmovecnt;
    hist_ent->pos_key = pos->pos_key;

    /* check for special moves */
    /* en passant */
    if (moveinfo_get_isenpassant(rep)) {
        if (side == COLOUR_WHITE) {
            /* white's en passant clears the square above the one where the pawn moves to */
            assert(board120_to_rank[(to - 10)] == RANK_5);
            mm_clear_piece(pos, to - 10);
        } else {
            /* black's en passant clears the square below the one where the pawn moves to */
            assert(board120_to_rank[(to + 10)] == RANK_4);
            mm_clear_piece(pos, to + 10);
        }
    } else if (moveinfo_get_iscastle(rep)) {
        /* castling! */
        switch (to) {
        case SQUARE_G1: mm_move_piece(pos, SQUARE_H1, SQUARE_F1); break; /* white kingside */
        case SQUARE_C1: mm_move_piece(pos, SQUARE_A1, SQUARE_D1); break; /* white queenside */
        case SQUARE_G8: mm_move_piece(pos, SQUARE_H8, SQUARE_F8); break; /* black kingside */
        case SQUARE_C8: mm_move_piece(pos, SQUARE_A8, SQUARE_D8); break; /* black queenside */
        default:
            /* something bad has happened! */
            assert(to == SQUARE_NONE);
            break;
        }
    }

    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);

    /* hash out the en passant square, if there is one */
    if (pos->enpassant != SQUARE_NONE) hash_enpassant();
    /* update en passant square */
    pos->enpassant = SQUARE_NONE;

    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);

    /* update castle permissions */
    hash_castle();
    pos->castle_perm &= (castle_perm[from] & castle_perm[to]);
    hash_castle();

    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);

    /* update various move counters */
    /* "fifty move rule" - needs to have a capture or pawn move every 50 moves */
    pos->pawnmovecnt++;
    /* ply (used when searching moves) */
    pos->ply++;
    /* length of history array */
    pos->hist_ply++;

    /* check if we captured a piece */
    if (cappiece != PIECE_EMPTY) {
        /* TODO: is cappiece set for en passant capture? what to do if no? */
        /* had a capture, so reset 50 move counter */
        pos->pawnmovecnt = 0;

        /* clear the captured piece from the board */
        mm_clear_piece(pos, to);
    }

    /* check if it was a pawn move */
    if (piece_is_pawn(pos->pieces[from])) {
        /* pawn move, so reset 50 move counter */
        pos->pawnmovecnt = 0;

        if (moveinfo_get_ispawnstart(rep)) {
            /* pawn starting move, so we need to set the en passant square */
            if (side == COLOUR_WHITE) {
                /* TODO is this right? shouldn't it be - 10? */
                /* white just moved; the en passant square is 1 above where we moved from */
                pos->enpassant = from + 10;
                assert(board120_to_rank[pos->enpassant] == RANK_3);
            } else {
                /* TODO is this right? shouldn't it be + 10? */
                /* black just moved; the en passant square is 1 below where we moved from */
                pos->enpassant = from - 10;
                assert(board120_to_rank[pos->enpassant] == RANK_6);
            }

            /* hash in the new en passant square */
            hash_enpassant();
            
            /* TODO: debug */
            assert(board_hash(pos) == pos->pos_key);
        }
    }

    /* now it should be safe to move the piece! */
    mm_move_piece(pos, from, to);

    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);

    /* check if there was a promotion made */
    if (prompiece != PIECE_EMPTY) {
        assert(piece_valid(prompiece) && (!piece_is_pawn(prompiece)));

        /* clear the old pawn from the board */
        mm_clear_piece(pos, to);

        /* add the newly promoted piece */
        mm_add_piece(pos, to, prompiece);
    }

    /* TODO: debug */
    assert(board_hash(pos) == pos->pos_key);

    /* swap the side to move */
    pos->side = (side == COLOUR_BLACK) ? COLOUR_WHITE : COLOUR_BLACK;
    hash_side();

    /* ensures board is valid */
    assert(board_check(pos));

    /* 
     * check if the side that made this move resulted in the king being under attack
     * (i.e. the move is invalid), and undo the move if yes.
     */
    kingsq = (side == COLOUR_BLACK) ? (pos->piece_list[PIECE_BLACK_KING][0]) : (pos->piece_list[PIECE_WHITE_KING][0]);
    if (logic_under_attack(pos, kingsq, pos->side)) {
        /* ahh, our king is under attack by the other side now! this is bad, so undo the move. */
        board_undo_move(pos);
        return 1;
    }

    /* should be an ok move! */
    return 0;
}

/* undo the most recent move */
void board_undo_move(board_t *pos) {
    uint8_t from, to, side, cappiece, prompiece;
    moveinfo_t rep;
    undo_move_t *hist_ent;

    assert(board_check(pos));
    assert(pos->hist_ply > 0);

    /* get the most recent history entry */
    hist_ent = &(pos->history[--pos->hist_ply]);

    /* decrease overall ply */
    pos->ply--;

    /* retrieve some historical values */
    rep = hist_ent->rep;
    pos->pawnmovecnt = hist_ent->pawnmovecnt;

    /* set variables */
    from = moveinfo_get_from(rep);
    to = moveinfo_get_to(rep);
    assert(square_valid(from));
    assert(square_valid(to));
    cappiece = moveinfo_get_capturepiece(rep);
    assert(piece_valid(cappiece));
    prompiece = moveinfo_get_promotepiece(rep);
    side = pos->side;

    /* hash out the en passant square, if there is one */
    if (pos->enpassant != SQUARE_NONE) hash_enpassant();
    /* retrieve the historical en passant square */
    pos->enpassant = hist_ent->enpassant;
    /* hash in the "new" (technically the old) en passant square */
    if (pos->enpassant != SQUARE_NONE) hash_enpassant();

    /* update castle permissions */
    hash_castle();
    /* retrieve the old historical en passant value */
    pos->castle_perm = hist_ent->castle_perm;
    /* update castle permissions */
    hash_castle();

    /* swap the side to move */
    pos->side = (side == COLOUR_BLACK) ? COLOUR_WHITE : COLOUR_BLACK;
    hash_side();

    /* check for special moves */
    /* en passant */
    if (moveinfo_get_isenpassant(rep)) {
        if (pos->side == COLOUR_WHITE) {
            /* white's en passant cleared the square above the one where the pawn moved to,
             * so add it back
             */
            assert(board120_to_rank[(to - 10)] == RANK_5);
            mm_add_piece(pos, to - 10, PIECE_BLACK_PAWN);
        } else {
            /* black's en passant cleared the square below the one where the pawn moved to,
             * so add it back
             */
            assert(board120_to_rank[(to + 10)] == RANK_4);
            mm_add_piece(pos, to + 10, PIECE_WHITE_PAWN);
        }
    } else if (moveinfo_get_iscastle(rep)) {
        /* castling! */
        switch (to) {
        case SQUARE_G1: mm_move_piece(pos, SQUARE_F1, SQUARE_H1); break; /* white kingside */
        case SQUARE_C1: mm_move_piece(pos, SQUARE_D1, SQUARE_A1); break; /* white queenside */
        case SQUARE_G8: mm_move_piece(pos, SQUARE_F8, SQUARE_H8); break; /* black kingside */
        case SQUARE_C8: mm_move_piece(pos, SQUARE_D8, SQUARE_A8); break; /* black queenside */
        default:
            /* something bad has happened! */
            assert(to == SQUARE_NONE);
            break;
        }
    }

    /* now it should be safe to move the piece back! */
    mm_move_piece(pos, to, from);

    /* check if we had captured a piece */
    if (cappiece != PIECE_EMPTY) {
        /* add the captured piece back to the board */
        mm_add_piece(pos, to, cappiece);
    }

    /* check if there was a promotion made */
    if (prompiece != PIECE_EMPTY) {
        assert(piece_valid(prompiece) && (!piece_is_pawn(pos->pieces[from])));

        /* clear the promoted piece from the board */
        mm_clear_piece(pos, from);

        /* add the pawn instead */
        mm_add_piece(pos, from, (pos->side == COLOUR_BLACK) ? (PIECE_BLACK_PAWN) : (PIECE_WHITE_PAWN));
    }

    /* ensure all our changes kept a good poskey */
    assert(hist_ent->pos_key == pos->pos_key);

    /* ensure we kept a valid board rep */
    assert(board_check(pos));

    /* all done! */
}

