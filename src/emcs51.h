#ifndef EMCS51_H
#define EMCS51_H

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum EMCS51_ERR
{
    EMCS51_OK = 0,
    EMCS51_ERR = -1,
    EMCS51_ERR_CORE_NULL = -2,         // core is NULL
    EMCS51_ERR_CODE_OUT_OF_RANGE = -3, // code out of range
    EMCS51_ERR_UNKNOWN_INST = -4,      // unknown instruction
    EMCS51_ERR_XDATA_OUT_OF_RANGE = -5 // xdata out of range
} emcs51_err_t;

const char *emcs51_err_name(int err);

#include "core/emcs51_inst.h"
#include "core/emcs51_core.h"
#include "instruction/emcs51_general_inst.h"

#endif // EMCS51_H
