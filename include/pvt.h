#ifndef PVT_H_INC
#define PVT_H_INC

#include <stdint.h>
#include "move.h"

/* default size for entire PV table */
#define PVT_SIZE 0x200000

/* entry in a PVT - need to store the pos_key and the corresponding best move */
typedef struct _pvtentry_t {
    uint64_t pos_key;
    moveinfo_t rep;
} pvtentry_t;

/* PVT */
typedef struct _pvtable_t {
    pvtentry_t *entries;
    int nalloc;
} pvtable_t;

int pvt_init(pvtable_t *pvt);
void pvt_clear(pvtable_t *pvt);
void pvt_add(pvtable_t *pvt, uint64_t pos_key, moveinfo_t rep);
moveinfo_t pvt_get(pvtable_t *pvt, uint64_t pos_key);

#endif /* PVT_H_INC */
