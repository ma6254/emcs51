#ifndef EMCS51_INST_H
#define EMCS51_INST_H

#include <stdint.h>
#include <stddef.h>
// #include "emcs51.h"

struct EMCS51_INST_DEF;

typedef struct EMCS51_INST_EXEC_EVENT
{
    uint8_t opcode;
    struct EMCS51_INST_DEF *inst_def;
    struct EMCS51_CORE *core;
} emcs51_inst_exec_event_t;

typedef void (*emcs51_inst_exec_cb_t)(emcs51_inst_exec_event_t *event);

typedef struct EMCS51_INST_DEF
{
    const char *mnemonic;          // instruction mnemonic
    uint8_t length;                // operands length
    uint8_t cycles;                // execution cycles
    emcs51_inst_exec_cb_t exec_cb; // execution callback
} emcs51_inst_def_t;

#endif // EMCS51_INST_H
