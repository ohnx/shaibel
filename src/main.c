/* main UCI interface */
#ifndef __TEST
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "board.h"
#include "move.h"
#include "logic.h"
#include "io.h"
#include "pvt.h"
#include "search.h"

#define BOARD_INITIAL_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

#define FEN5 "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define FEN6 "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define FEN7 "5k2/1n6/4n3/6N1/8/3N4/8/5K2 b - - 0 1"
#define FEN8 "6k1/8/5r2/8/1nR5/5N2/8/6K1 w - - 0 1"
#define FEN9 "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 w - - 0 1"
#define FENA "6k1/1b6/4n3/8/1n4B1/1B3N2/1N62b3K1 b - - 0 1"
#define FENB "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1"
#define FENC "8/8/8/8/8/8/6k1/4K2R w K - 0 1 ;D1 12 ;D2 38 ;D3 564 ;D4 2219 ;D5 37735 ;D6 185867"
#define FEND "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define FENE "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

/* TODO: make it no longer necessary */
int board_hash_init();

void init() {
    board_hash_init();
    move_mvvlva_init();
}

#if NO_UCI
int main(int argc, char **argv) {
    board_t board;
    int ret, i;
    uint8_t from, to, cap, prom, file, rank, square;
    moveinfo_t rep;
    movelist_t moves;
    searchparams_t params;
    char input[6];

    init();
    (void)board;
    (void)ret;
    (void)i;
    (void)rep;
    (void)moves;
    (void)from;
    (void)to;
    (void)cap;
    (void)prom;
    (void)file;
    (void)rank;
    (void)square;
    (void)input;
    (void)params;
/*
    fprintf(stderr, "board 1:\n");
    ret = board_init_from_fen(&board, FEN1);
    if (ret) fprintf(stderr, "ret = %d\n", ret);
    board_print(&board);

    fprintf(stderr, "board 2:\n");
    ret = board_init_from_fen(&board, FEN2);
    if (ret) fprintf(stderr, "ret = %d\n", ret);
    board_print(&board);

    fprintf(stderr, "board 3:\n");
    ret = board_init_from_fen(&board, FEN3);
    if (ret) fprintf(stderr, "ret = %d\n", ret);
    board_print(&board);
*//*
    fprintf(stderr, "board 4:\n");
    ret = board_init_from_fen(&board, FEN4);
    if (ret) fprintf(stderr, "ret = %d\n", ret);
    board_print(&board);

    fprintf(stderr, "white pawns:\n");
    board64_print(board.pawns[COLOUR_WHITE]);
    fprintf(stderr, "black pawns:\n");
    board64_print(board.pawns[COLOUR_BLACK]);
    fprintf(stderr, "either pawns:\n");
    board64_print(board.pawns[COLOUR_EITHER]);

    board_check(&board);*//*

    from = SQUARE_A2;
    to = SQUARE_H7;
    cap = PIECE_WHITE_ROOK;
    prom = PIECE_BLACK_QUEEN;

    rep = moveinfo_new(from, to, cap, 0, 0, prom, 1);

    assert(moveinfo_get_from(rep) == from);
    assert(moveinfo_get_to(rep) == to);
    assert(moveinfo_get_capturepiece(rep) == cap);
    assert(moveinfo_get_promotepiece(rep) == prom);
    assert(!moveinfo_get_isenpassant(rep));
    assert(!moveinfo_get_ispawnstart(rep));
    assert(moveinfo_get_iscastle(rep));

    fprintf(stderr, "The move: ");
    moveinfo_print(rep);
    fprintf(stderr, "\n");*/

#if 0
    board_init_from_fen(&board, FENC);
    board_print(&board);
    movelist_fill(&moves, &board);
    prom = 0;

    for (ret = 0; ret < moves.count; ret++) {
        rep = moves.moves[ret].rep;

        if (board_make_move(&board, rep)) continue;
        fprintf(stderr, "made move: ");
        moveinfo_print(rep);
        fprintf(stderr, "\n");/*
        board_print(&board);*/
        prom++;

        board_undo_move(&board);/*
        fprintf(stderr, "undid move: ");
        moveinfo_print(rep);
        fprintf(stderr, "\n");
        board_print(&board);

        getchar();*/
    }
    fprintf(stderr, "found %d LEGAL moves\n", prom);

#endif

    board_init_from_fen(&board, FENE);
    board_print(&board);

    while (fgets(input, 6, stdin)) {
        if (*input == 'u') {
            fprintf(stderr, "Undoing move\n");
            board_undo_move(&board);
            board_print(&board);
            continue;
        } else if (*input == 's') {
            params.max_depth = 6;
            search_start(&params, &board);
            continue;
        }
        if ((ret = io_parse_move(input, &board, &rep))) {
            fprintf(stderr, "Invalid move: %d!\n", ret);
            continue;
        }
        pvt_add(&(board.pvt), board.pos_key, rep);
        board_make_move(&board, rep);
        board_print(&board);
    }

    return 0;
}
#else

#define INPUT_MAX 1024
#define SEARCH_DEPTH 8

int main(int argc, char **argv) {
    char input[INPUT_MAX], *pptr;
    board_t board;
    searchparams_t params;
    moveinfo_t rep = MOVE_NONE;

    init();
    pvt_init(&(board.pvt));
    params.max_depth = SEARCH_DEPTH;

    while (fgets(input, INPUT_MAX, stdin)) {
        fprintf(stderr, ">>> %s", input);
        if (!strncmp(input, "quit", 4)) {
            break;
        } else if (!strncmp(input, "isready", 7)) {
            printf("readyok\n");
        } else if (!strncmp(input, "ucinewgame", 10)) {
        } else if (!strncmp(input, "uci", 3)) {
            printf("id name shaibel\n");
            printf("id author ohnx\n");
            printf("uciok\n");
        } else if (!strncmp(input, "position startpos moves", 23)) {
            board_init_from_fen(&board, BOARD_INITIAL_FEN);
            pptr = input + 23;
            /* parse in all the moves */
            while (*pptr) {
                if (*pptr == ' ') {
                    io_parse_move(pptr + 1, &board, &rep);
                    board_make_move(&board, rep);
                }
                pptr++;
            }
        } else if (!strncmp(input, "position startpos", 17)) {
            board_init_from_fen(&board, BOARD_INITIAL_FEN);
        } else if (!strncmp(input, "position fen", 12)) {
            board_init_from_fen(&board, input + 12);
        } else if (!strncmp(input, "go", 2)) {
            fprintf(stderr, "starting to search for moves in board\n");
            board_print(&board);
            rep = search_start(&params, &board);
            printf("bestmove ");
            moveinfo_print_stdout(rep);
            printf("\n");
        }
        fflush(stdout);
    }

    /* TODO: free memory */
    return 0;
}
#endif



#else /* __TEST */
static void b();
static void a() {b();}
static void b() {a();}
#endif /* __TEST */
