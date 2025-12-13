#include "emcs51.h"

const char* emcs51_err_name(int err)
{
    switch(err)
    {
        case EMCS51_OK:
            return "OK";
        case EMCS51_ERR:
            return "ERR";
        case EMCS51_ERR_CORE_NULL:
            return "ERR_CORE_NULL";
        case EMCS51_ERR_CODE_OUT_OF_RANGE:
            return "ERR_CODE_OUT_OF_RANGE";
        case EMCS51_ERR_UNKNOWN_INST:
            return "ERR_UNKNOWN_INST";
    }

    return "UNKNOWN_ERR";
}
