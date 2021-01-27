#ifndef MOVE_H_INC
#define MOVE_H_INC

#include <stdint.h>
#include "board.h"

/**
 * move info is a 32-bit unsigned int
 *
 * MSB                                          LSB
 * V                                            V
 * 0000 000|0 |0000 |0|0|00 00|00 0000 0|000 0000
 *         |  |     | | |     |         ----------> Initial square
 *         |  |     | | |     --------------------> Destination square
 *         |  |     | | --------------------------> Captured piece
 *         |  |     | ----------------------------> En passant capturing move?
 *         |  |     ------------------------------> Move is initial pawn double moving?
 *         |  ------------------------------------> Piece promoted to
 *         ---------------------------------------> Was a castle?
 */
typedef uint32_t moveinfo_t;

#define MOVE_NONE 0xFE000000

/* i feel like i'm writing lisp here ... */
#define moveinfo_new(initsq, destsq, cappiece, enpcap, pdub, pprom, cstl) \
                    ((moveinfo_t)( \
                    ((((moveinfo_t)(initsq)) & 0x7F)) | \
                    ((((moveinfo_t)(destsq)) & 0x7F) << 7) | \
                    ((((moveinfo_t)(cappiece)) & 0xF) << 14) | \
                    ((((moveinfo_t)(enpcap)) & 0x1) << 18) | \
                    ((((moveinfo_t)(pdub)) & 0x1) << 19) | \
                    ((((moveinfo_t)(pprom)) & 0xF) << 20) | \
                    ((((moveinfo_t)(cstl)) & 0x1) << 24)))
#define moveinfo_get_from(x) ((x) & 0x7F)
#define moveinfo_get_to(x) (((x) >> 7) & 0x7F)
#define moveinfo_get_capturepiece(x) (((x) >> 14) & 0xF)
#define moveinfo_get_isenpassant(x) ((x) & 0x40000)
#define moveinfo_get_ispawnstart(x) ((x) & 0x80000)
#define moveinfo_get_promotepiece(x) (((x) >> 20) & 0xF)
#define moveinfo_get_iscastle(x) ((x) & 0x1000000)
/* capture can mean having a captured piece or en passant capture */
#define moveinfo_get_iscapture(x) ((x) & 0x7C000)
/* nonzero = piece was promoted */
#define moveinfo_get_ispromote(x) ((x) & 0xF00000)

/* predefined move scores */
#define MOVE_SCORE_PVT          2000000
#define MOVE_SCORE_CAPTURE_BASE 1000000
#define MOVE_SCORE_KILLER_FIRST  900000
#define MOVE_SCORE_KILLER_SECOND 800000

typedef struct _move_t {
    moveinfo_t rep;
    int score;
} move_t;

#define MOVELIST_MAX_LEN 256
typedef struct _movelist_t {
    uint32_t count;
    move_t moves[MOVELIST_MAX_LEN];
} movelist_t;

void move_mvvlva_init();
void moveinfo_print_stdout(moveinfo_t rep);
void moveinfo_print(moveinfo_t rep);
void movelist_print(movelist_t *moves);
struct _board_t;
int32_t movelist_fill(movelist_t *moves, struct _board_t *pos);
int32_t movelist_fill_capture(movelist_t *moves, struct _board_t *pos);

#endif /* MOVE_H_INC */
