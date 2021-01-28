/* this file contains the logic on move search, including alpha/beta minimax search and move ordering */
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "board.h"
#include "search.h"
#include "logic.h"
#include "pvt.h"

#define SCORE_INF  INT_MAX
#define SCORE_MATE (INT_MAX - 100)

/* check if the search time limit has been exceeded */
static void check_time() {
    
}

/* clear various tracking variables in preparation for search begin */
static void search_prepare(searchparams_t *params, board_t *pos) {
    uint8_t i, j;

    /* clear search history variable */
    for (i = 0; i < PIECE_INVALID; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            pos->search_history[i][j] = 0;
        }
    }

    /* clear search killers variable */
    for (i = 0; i < 2; i++) {
        for (j = 0; j < MAX_SEARCH_DEPTH; j++) {
            pos->search_killers[i][j] = MOVE_NONE;
        }
    }

    /* clear pvt */
    pvt_clear(&(pos->pvt));

    /* clear ply */
    pos->ply = 0;

    /* clear various searchparams */
    params->stopped = 0;
    params->nodes = 0;
    params->starttime = 0;

    /* clear fail high / fail high first */
    params->fhf = 0;
    params->fh = 0;
}

/* pick the next best move in the move list and shuffle it to the front */
static void search_reorder_best_move(uint32_t moves_so_far, movelist_t *moves) {
    uint32_t i, best_idx = MOVELIST_MAX_LEN;
    int best_score = 0;
    move_t tmp;

    /* loop through all remaining moves to pick out the best one */
    for (i = moves_so_far; i < moves->count; i++) {
        if (moves->moves[i].score > best_score) {
            /* move score beats the score of other moves seen so far, so update
             * the best variables
             */
            best_score = moves->moves[i].score;
            best_idx = i;
        }
    }

    /* no move to swap in */
    if (best_idx == MOVELIST_MAX_LEN) return;

    /* swap the move in to the next slot */
    tmp = moves->moves[moves_so_far];
    moves->moves[moves_so_far] = moves->moves[best_idx];
    moves->moves[best_idx] = tmp;
}

/* search only capturing moves */
static int search_quiescence(searchparams_t *params, board_t *pos, int alpha, int beta) {
    movelist_t moves;
    int old_alpha = alpha;
    moveinfo_t best_move = MOVE_NONE;
    int score;
    uint32_t i;
    uint64_t legal_moves = 0;

    assert(board_check(pos));

    /* visiting a new node! */
    params->nodes++;

    /* check that we don't repeat */
    if (logic_repeated_move(pos) || pos->pawnmovecnt >= 100) {
        /* 3 repeated moves or pawn move count >= 100 ply (50 moves) is a draw */
        return 0;
    }

    /* check if we have reached the maximum depth set in the engine */
    if (pos->ply >= MAX_SEARCH_DEPTH) {
        /* we can't safely use the pvtarr past MAX_SEARCH_DEPTH, so we have to
         * stop searching now
         */
        return board_evaluate_position(pos);
    }

    /* get current score of the board */
    score = board_evaluate_position(pos);

    /* we know that we won't decrease the score, so we can safely return beta now */
    if (score >= beta) return beta;

    /* TODO: what does this mean? part 65, 3:02 */
    if (score > alpha) alpha = score;
    /* TODO: danger here that best_move is not set if score > alpha?? */

    /* fill move list with only captures */
    movelist_fill_capture(&moves, pos);

    /* loop through all the possible moves found */
    /* TODO: would a qsort on the movelist be better here instead? */
    for (i = 0; i < moves.count; i++) {
        /* search done */
        if (params->stopped) return 0;

        /* get the next best move in the list */
        search_reorder_best_move(i, &moves);

        /* try to make this move */
        if (board_make_move(pos, moves.moves[i].rep)) continue;

        /* legal move found */
        legal_moves++;

        /* recursive minimax search :) */
        score = -search_quiescence(params, pos, -beta, -alpha);

        /* undo this move */
        board_undo_move(pos);

        /* check the score of the move */
        if (score > alpha) {
            if (score >= beta) {
                /* beta cutoff */

                /* move ordering heuristic calculation */
                if (legal_moves == 1) {
                    /* this was the first move we searched (good thing) */
                    params->fhf++;
                } else {
                    /* this was not the first item search; move ordering could
                     * be better
                     */
                }
                params->fh++;
                return beta;
            }

            /* alpha cutoff */
            alpha = score;
            best_move = moves.moves[i].rep;
        }
    }

    if (alpha != old_alpha) {
        pvt_add(&(pos->pvt), pos->pos_key, best_move);
    }

    return alpha;
}

/* search for the best move at a given depth with certain alpha and beta cutoff scores */
static int search_alphabeta(searchparams_t *params, board_t *pos, int alpha, int beta, int depth) {
    movelist_t moves;
    int old_alpha = alpha;
    moveinfo_t best_move = MOVE_NONE, pvt_move;
    int score = -SCORE_INF;
    uint32_t i;
    uint64_t legal_moves = 0;
    uint8_t king_piece, attack_side;

    assert(board_check(pos));

    /* trivial case, done alpha/beta searching if depth = 0 */
    if (depth == 0) {
        return search_quiescence(params, pos, alpha, beta);
    }

    /* visiting a new node! */
    params->nodes++;

    /* check that we don't repeat */
    if (logic_repeated_move(pos) || pos->pawnmovecnt >= 100) {
        /* 3 repeated moves or pawn move count >= 100 ply (50 moves) is a draw */
        return 0;
    }

    /* check if we have reached the maximum depth set in the engine */
    if (pos->ply >= MAX_SEARCH_DEPTH) {
        /* we can't safely use the pvtarr past MAX_SEARCH_DEPTH, so we have to
         * stop searching now
         */
        return board_evaluate_position(pos);
    }

    /* fill move list */
    movelist_fill(&moves, pos);

    /* check to see if we already have this move in the pvt */
    pvt_move = pvt_get(&(pos->pvt), pos->pos_key);
    if (pvt_move != MOVE_NONE) {
        /* search our entire move list for the pv move and check it first */
        for (i = 0; i < moves.count; i++) {
            if (moves.moves[i].rep == pvt_move) {
                /* match! set the score to super high MOVE_SCORE_PVT */
                moves.moves[i].score = MOVE_SCORE_PVT;
                break;
            }
        }
    }

    /* loop through all the possible moves found */
    /* TODO: would a qsort on the movelist be better here instead? */
    for (i = 0; i < moves.count; i++) {
        /* search done */
        if (params->stopped) return 0;

        /* get the next best move in the list */
        search_reorder_best_move(i, &moves);

        /* try to make this move */
        if (board_make_move(pos, moves.moves[i].rep)) continue;

        /* legal move found */
        legal_moves++;

        /* recursive minimax search :) */
        score = -search_alphabeta(params, pos, -beta, -alpha, depth - 1);

        /* undo this move */
        board_undo_move(pos);

        /* check the score of the move */
        if (score > alpha) {
            if (score >= beta) {
                /* beta cutoff */

                /* move ordering heuristic calculation */
                if (legal_moves == 1) {
                    /* this was the first move we searched (good thing) */
                    params->fhf++;
                } else {
                    /* this was not the first item search; move ordering could
                     * be better
                     */
                }
                params->fh++;

                /* killer moves calculation */
                if (!moveinfo_get_iscapture(moves.moves[i].rep)) {
                    /* only store non-capture moves because capture moves will
                     * be automatically ordered ahead of other moves
                     */
                    pos->search_killers[1][pos->ply] = pos->search_killers[0][pos->ply];
                    pos->search_killers[0][pos->ply] = moves.moves[i].rep;
                }

                return beta;
            }

            /* alpha cutoff */
            alpha = score;
            best_move = moves.moves[i].rep;

            /* store move in the history */
            if (!moveinfo_get_iscapture(moves.moves[i].rep)) {
                /* only store non-capture moves because capture moves will
                 * be automatically ordered ahead of other moves
                 */
                pos->search_history[pos->pieces[moveinfo_get_from(best_move)]][moveinfo_get_to(best_move)] += depth;
            }
        }
    }

    /* check if there's a mate or draw */
    if (legal_moves == 0) {
        if (pos->side == COLOUR_BLACK) {
            attack_side = COLOUR_WHITE;
            king_piece = PIECE_BLACK_KING;
        } else {
            attack_side = COLOUR_BLACK;
            king_piece = PIECE_WHITE_KING;
        }
        if (logic_under_attack(pos, pos->piece_list[king_piece][0], attack_side)) {
            /* checkmate! */
            return -SCORE_MATE + pos->ply;
        } else {
            /* draw */
            return 0;
        }
    }

    /* check if we found a new good move! */
    if (alpha != old_alpha) {
        /* store the good move */
        pvt_add(&(pos->pvt), pos->pos_key, best_move);
    }

    return alpha;
}

/* search for the best move starting from a given position using iterative deepening */
moveinfo_t search_start(searchparams_t *params, board_t *pos) {
    /* iterative deepening :)) */
    moveinfo_t best_move = MOVE_NONE;
    int best_score = -SCORE_INF;
    int depth = 0;
    int pv_moves = 0;
    int pv_num = 0;

    /* prepare for search */
    search_prepare(params, pos);

    /* TODO: call functions for whatever */
    check_time();

    /* begin iterative deepening */
    for (depth = 1; depth <= params->max_depth; depth++) {
        best_score = search_alphabeta(params, pos, -SCORE_INF, SCORE_INF, depth);

        /* search done */
        if (params->stopped) break;

        /* fetch the best move found so far */
        pv_moves = board_pvt_get_chain(pos, depth);
        best_move = pos->pvtarr[0];

        /* TODO: remove debug display thinking */
        fprintf(stderr, "Depth: %d score:%d nodes:%lu move:", depth, best_score,
                params->nodes);
        moveinfo_print(best_move);
        fprintf(stderr, "\nmoves: ");
        for (pv_num = 0; pv_num < pv_moves; pv_num++) {
            moveinfo_print(pos->pvtarr[pv_num]);
            fprintf(stderr, " ");
        }
        fprintf(stderr, "\nmove ordering: %.2f\n", (params->fhf/params->fh));
    }

    return best_move;
}
