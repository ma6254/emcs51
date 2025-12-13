#include "emcs51.h"
#include "emcs51_testing.h"

static const uint8_t emcs51_sjmp_test1_code_memory[] = {
    0x00,      // NOP
    0x80, 0xFD // SJMP -3
};

/*******************************************************************************
 * @brief 回调函数：读取code区存储器
 * @param none
 * @return none
 ******************************************************************************/
static int emcs51_sjmp_test1_read_code_cb(uint16_t addr, uint8_t *data, uint16_t len)
{
    if (addr + len > sizeof(emcs51_sjmp_test1_code_memory))
    {
        return EMCS51_ERR_CODE_OUT_OF_RANGE;
    }

    memcpy(data, &emcs51_sjmp_test1_code_memory[addr], len);

    return EMCS51_OK;
}

/*******************************************************************************
 * @brief 测试：SJMP指令
 * @param none
 * @return none
 ******************************************************************************/
void emcs51_testing_inst_sjmp_test1(emcs51_testing_t *t)
{
    emcs51_core_t emcs51_core;

    emcs51_core_config_t emcs51_core_config = {
        .read_code_cb = emcs51_sjmp_test1_read_code_cb,
    };

    emcs51_core_init(&emcs51_core, &emcs51_core_config);
    emcs51_general_inst_init(&emcs51_core);

    for (uint32_t step_i = 0; step_i < 2; step_i++)
    {
        emcs51_core_reg_t prev_reg = emcs51_core.reg;

        emcs51_core_inc(&emcs51_core);
        if (emcs51_core.err < 0)
        {
            t->err = emcs51_core.err;
            return;
        }
    }

    if (emcs51_core.reg.pc != 0)
    {
        // PC not jumped correctly, error
        t->err = EMCS51_ERR;
        return;
    }
}

static const uint8_t emcs51_sjmp_test2_code_memory[] = {
    0x00,      // NOP
    0x00,      // NOP
    0x80, 0xFC // SJMP -4
};

/*******************************************************************************
 * @brief 回调函数：读取code区存储器
 * @param none
 * @return none
 ******************************************************************************/
static int emcs51_sjmp_test2_read_code_cb(uint16_t addr, uint8_t *data, uint16_t len)
{
    if (addr + len > sizeof(emcs51_sjmp_test2_code_memory))
    {
        return EMCS51_ERR_CODE_OUT_OF_RANGE;
    }

    memcpy(data, &emcs51_sjmp_test2_code_memory[addr], len);

    return EMCS51_OK;
}

/*******************************************************************************
 * @brief 测试：SJMP指令
 * @param none
 * @return none
 ******************************************************************************/
void emcs51_testing_inst_sjmp_test2(emcs51_testing_t *t)
{
    emcs51_core_t emcs51_core;

    emcs51_core_config_t emcs51_core_config = {
        .read_code_cb = emcs51_sjmp_test2_read_code_cb,
    };

    emcs51_core_init(&emcs51_core, &emcs51_core_config);
    emcs51_general_inst_init(&emcs51_core);

    for (uint32_t step_i = 0; step_i < 3; step_i++)
    {
        emcs51_core_reg_t prev_reg = emcs51_core.reg;

        emcs51_core_inc(&emcs51_core);
        if (emcs51_core.err < 0)
        {
            t->err = emcs51_core.err;
            return;
        }
    }

    if (emcs51_core.reg.pc != 0)
    {
        // PC not jumped correctly, error
        t->err = EMCS51_ERR;
        return;
    }
}
