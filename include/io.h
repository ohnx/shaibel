#ifndef IO_H_INC
#define IO_H_INC

#include "board.h"
#include "move.h"

int io_parse_move(const char *inp, board_t *pos, moveinfo_t *prep);

#endif /* IO_H_INC */
