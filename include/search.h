#ifndef SEARCH_H_INC
#define SEARCH_H_INC

#include <stdint.h>

typedef struct _searchparams_t {
    /* ways to control how long the engine searches for */
    int starttime;
    int endtime;
    int depth;
    int max_depth;
    int max_time;
    int movestogo;
    int infinite;

    /* number of nodes visited */
    uint64_t nodes;

    /* tell the engine to quit */
    int quit;

    /* tell the engine to stop */
    int stopped;

    /* move ordering performance */
    float fhf, fh;
} searchparams_t;

moveinfo_t search_start(searchparams_t *params, board_t *pos);

#endif /* SEARCH_H_INC */
