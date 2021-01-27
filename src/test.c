/* performance and correctness testing :)) */
#ifdef __TEST

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "board.h"
#include "move.h"

#define MAX_DEPTH 5
#define UPDATE_INTERVAL 100000

uint64_t expect_ndepths[MAX_DEPTH+1];
uint8_t quiet = 0;

/* function to depth-first recursively search all */
void explore(board_t *pos, int depth, uint64_t *ndepths) {
    movelist_t moves;
    uint32_t i;
    uint64_t hash, hash2;
    (void)hash;
    (void)hash2;

    /* skip if we've reached max depth */
    if (depth >= MAX_DEPTH) return;

    /* fill move list */
    movelist_fill(&moves, pos);

    /* loop through all the possible moves found */
    for (i = 0; i < moves.count; i++) {
#ifndef PERFTEST
        hash2 = board_hash(pos);
#endif
        /* try to make this move */
        if (board_make_move(pos, moves.moves[i].rep)) continue;

        /* increment the number of moves found at this depth */
        ndepths[depth]++;
        if (!quiet) {
            if (ndepths[MAX_DEPTH] % UPDATE_INTERVAL == 0) {
                fprintf(stderr, "\rsearched %lu positions...", ndepths[MAX_DEPTH]);
                fprintf(stderr, " (~%02lu%%)",
                        (ndepths[MAX_DEPTH]*100)/(expect_ndepths[MAX_DEPTH-1]));
                fflush(stderr);
            }
        }
        /* also increment total */
        ndepths[MAX_DEPTH]++;

#ifndef PERFTEST
        hash = board_hash(pos);
#endif
        /* now see what child moves can be made */
        explore(pos, depth + 1, ndepths);
#ifndef PERFTEST
        assert(hash == board_hash(pos));
#endif

        /* undo the move and move on to the next one */
        board_undo_move(pos);

#ifndef PERFTEST
        assert(hash2 == board_hash(pos));
#endif
    }
}

/* TODO: make it no longer necessary */
int board_hash_init();

#define BUF_LEN 256
int main(int argc, char **argv) {
    FILE *fp;
    char buf[BUF_LEN], *nbrs;
    uint64_t ndepths[MAX_DEPTH+1];
    int r = 0, i, ll;
    board_t board;

    if (argc != 2 && argc != 3) {
        fprintf(stderr, "usage: %s <test suite> <quiet? (optional)>\n", argv[0]);
        return -1;
    } else if (sizeof(long) != sizeof(uint64_t)) {
        fprintf(stderr, "long int is too short on this machine!\n");
        return -2;
    }
    if (argc == 3) quiet = 1;

    /* TODO: make it no longer necessary */
    board_hash_init();

    /* load the file */
    fp = fopen(argv[1], "r");

    /* read line by line */
    ll = 1;
    while(fgets(buf, BUF_LEN, fp)) {
        if (!quiet) fprintf(stderr, "parsing test case on line %d...\n", ll);
        /* parse the fen */
        board_init_from_fen(&board, buf);
#ifndef PERFTEST
        board_print(&board);
#endif

        /* find the start of the depths */
        nbrs = buf;
        expect_ndepths[MAX_DEPTH] = 0;
        ndepths[MAX_DEPTH] = 0;
        for (i = 0; i < MAX_DEPTH; i++) {
            nbrs = strchr(nbrs, ';');
            if (!nbrs || !(nbrs[0]) || !(nbrs[1]) || !(nbrs[2]) || !(nbrs[3])) {
                if (!quiet) fprintf(stderr, "OOPS! expected %d depth strings, but only found %d!\n", MAX_DEPTH, i-1);
                goto cleanup;
            }
            nbrs += 3;
            ndepths[i] = 0;
            /* parse in expected number of depths */
            expect_ndepths[i] = strtol(nbrs, &nbrs, 10);
            expect_ndepths[MAX_DEPTH] += expect_ndepths[i];
        }

#ifndef PERFTEST
        /* print out expected depths */
        fprintf(stderr, "expected depths: ");
        for (i = 0; i < MAX_DEPTH; i++) {
            fprintf(stderr, "%lu ", expect_ndepths[i]);
        }
        fprintf(stderr, "\n");
#endif

        /* explore */
        if (!quiet) fprintf(stderr, "exploring...");
        if (!quiet) fflush(stderr);

        /* explore! */
        explore(&board, 0, ndepths);

        if (!quiet) fprintf(stderr, "\rfound %lu positions at depth %d!               \n", ndepths[MAX_DEPTH], MAX_DEPTH);

        /* verify that the depths are correct */
        for (i = 0; i < MAX_DEPTH; i++) {
            if (ndepths[i] != expect_ndepths[i]) {
                if (!quiet) fprintf(stderr, "D%d: Expected %lu, got %lu!\n", i+1, expect_ndepths[i], ndepths[i]);
                r--;
            }
        }
        if (r) goto cleanup;

        /* increment line number */
        ll++;
    }

cleanup:
    /* close file */
    fclose(fp);

    /* all done! */
    return r;
}

#else /* __TEST */
static void b();
static void a() {b();}
static void b() {a();}
#endif /* __TEST */
