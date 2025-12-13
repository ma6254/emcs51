#include "emcs51.h"
#include "emcs51_testing.h"

static const uint8_t code_memory[] = {
    0x00, // NOP
    0x00, // NOP
    0x00, // NOP
    0x00, // NOP
};

/*******************************************************************************
 * @brief 回调函数：读取code区存储器
 * @param none
 * @return none
 ******************************************************************************/
static int emcs51_core_read_code_cb(uint16_t addr, uint8_t *data, uint16_t len)
{
    if (addr + len > sizeof(code_memory))
    {
        return EMCS51_ERR_CODE_OUT_OF_RANGE;
    }

    memcpy(data, &code_memory[addr], len);

    return EMCS51_OK;
}

/*******************************************************************************
 * @brief 测试：NOP指令
 * @param none
 * @return none
 ******************************************************************************/
void emcs51_test_inst_nop(emcs51_testing_t* t)
{
    emcs51_core_t emcs51_core;

    emcs51_core_config_t emcs51_core_config = {
        .read_code_cb = emcs51_core_read_code_cb,
    };

    emcs51_core_init(&emcs51_core, &emcs51_core_config);
    emcs51_general_inst_init(&emcs51_core);

    for (uint32_t i = 0; i < 4; i++)
    {
        emcs51_core_reg_t prev_reg = emcs51_core.reg;

        emcs51_core_inc(&emcs51_core);
        if (emcs51_core.err < 0)
        {
            t->err = emcs51_core.err;
            return;
        }

        if ((prev_reg.pc + 1) != emcs51_core.reg.pc)
        {
            // PC not changed, error
            t->err = EMCS51_ERR;
            return;
        }
        
    }
}
