#ifndef LOGIC_H_INC
#define LOGIC_H_INC

#include <stdint.h>
#include "board.h"
#include "move.h"

int logic_under_attack(board_t *pos, uint8_t square, uint8_t side);
uint32_t logic_repeated_move(board_t *pos);
int logic_legal_move(board_t *pos, moveinfo_t rep);

#endif /* LOGIC_H_INC */
