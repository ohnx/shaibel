/* implementation of a "principal variation table" - lookup table for various
 * positions and the best moves to play in them
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "board.h"
#include "pvt.h"
#include "logic.h"

/* initialize a pvt structure. Returns non-zero value on failure. */
int pvt_init(pvtable_t *pvt) {
    /* set the number of entries */
    pvt->nalloc = (PVT_SIZE) / (sizeof(pvtentry_t));

    /* allocate memory for the entries */
    pvt->entries = malloc(pvt->nalloc * sizeof(pvtentry_t));
    if (!(pvt->entries)) return -1;

    /* clear PVT */
    pvt_clear(pvt);

    /* all done */
    return 0;
}

/* clear all entries in a pvt */
void pvt_clear(pvtable_t *pvt) {
    int i;

    /* loop through all items in the pvt set them to 0 */
    for (i = 0; i < pvt->nalloc; i++) {
        /* even if there is a conflict in pos_key, rep will be MOVE_NONE,
         * so it's ok
         */
        pvt->entries[i].pos_key = 0;
        pvt->entries[i].rep = MOVE_NONE;
    }
}

/* store an entry in the pvt - note that we might drop an existing item in the
 * table through this operation
 */
void pvt_add(pvtable_t *pvt, uint64_t pos_key, moveinfo_t rep) {
    /* determine the index to store at */
    int idx = (pos_key) % (pvt->nalloc);
    assert((idx >= 0) && (idx < pvt->nalloc));

    /* store the entry! */
    pvt->entries[idx].pos_key = pos_key;
    pvt->entries[idx].rep = rep;
}

/* get an item from the PVT. returns an invalid move (specifically, MOVE_NONE)
 * if the entry is not found in the PVT.
 */
moveinfo_t pvt_get(pvtable_t *pvt, uint64_t pos_key) {
    /* determine the index to store at */
    int idx = (pos_key) % (pvt->nalloc);
    assert((idx >= 0) && (idx < pvt->nalloc));

    /* return the entry if pos_key matches */
    if (pvt->entries[idx].pos_key == pos_key)
        /* match, yay! */
        return pvt->entries[idx].rep;
    else
        /* no match, return no move */
        return MOVE_NONE;
}

/* search the pvt for a chain of moves. returns the depth of moves found. */
int board_pvt_get_chain(board_t *pos, int depth) {
    moveinfo_t rep;
    int count, i;

    assert(depth < MAX_SEARCH_DEPTH);

    /* get the initial value */
    rep = pvt_get(&(pos->pvt), pos->pos_key);
    count = 0;

    /* loop until no more moves in the pvt, or we reach maximum depth */
    while ((rep != MOVE_NONE) && (count < depth)) {
        assert(count < MAX_SEARCH_DEPTH);

        /* check if this move is legal */
        if (!logic_legal_move(pos, rep)) {
            /* this move chain is no longer legal, so stop the search */
            break;
        }
        
        /* still legal, so keep going! */
        board_make_move(pos, rep);
        /* store the move chain in the search array */
        pos->pvtarr[count++] = rep;

        /* search for the next move */
        rep = pvt_get(&(pos->pvt), pos->pos_key);
    }

    /* we also need to undo all of the moves we just made */
    for (i = 0; i < count; i++) {
        board_undo_move(pos);
    }

    /* all done! */
    return count;
}
