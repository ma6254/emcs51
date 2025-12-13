#include "emcs51.h"
#include "emcs51_testing.h"

static const uint8_t mov_direct_immed_test1_code_memory[] = {
    0x75, 0x80, 0xAA, // MOV P0 #0AAH
    0x75, 0x80, 0x55  // MOV P0 #055H
};

static uint8_t port0_data = 0x00;

/*******************************************************************************
 * @brief 回调函数：读取code区存储器
 * @param none
 * @return none
 ******************************************************************************/
static int emcs51_mov_direct_immed_read_code_cb(uint16_t addr, uint8_t *data, uint16_t len)
{
    if (addr + len > sizeof(mov_direct_immed_test1_code_memory))
    {
        return EMCS51_ERR_CODE_OUT_OF_RANGE;
    }

    memcpy(data, &mov_direct_immed_test1_code_memory[addr], len);

    return EMCS51_OK;
}

static int emcs51_mov_direct_immed_write_data_cb(uint8_t addr, uint8_t data)
{

    port0_data = data;

    return EMCS51_OK;
}

/*******************************************************************************
 * @brief 测试：NOP指令
 * @param none
 * @return none
 ******************************************************************************/
void emcs51_test_inst_mov_direct_immed(emcs51_testing_t *t)
{
    emcs51_core_t emcs51_core;

    emcs51_core_config_t emcs51_core_config = {
        .read_code_cb = emcs51_mov_direct_immed_read_code_cb,
    };

    emcs51_core_init(&emcs51_core, &emcs51_core_config);
    emcs51_general_inst_init(&emcs51_core);
    emcs51_core_reg_add(&emcs51_core, 0x80, emcs51_mov_direct_immed_write_data_cb, NULL);

    emcs51_core_inc(&emcs51_core);
    if (emcs51_core.err < 0)
    {
        t->err = emcs51_core.err;
        return;
    }

    if(port0_data != 0xAA)
    {
        snprintf(t->msg, sizeof(t->msg), "P0 must be epuals 0xAA");
        t->err = EMCS51_ERR;
        return;
    }

    emcs51_core_inc(&emcs51_core);
    if (emcs51_core.err < 0)
    {
        t->err = emcs51_core.err;
        return;
    }
    
    if(port0_data != 0x55)
    {
        snprintf(t->msg, sizeof(t->msg), "P0 must be epuals 0x55");
        t->err = EMCS51_ERR;
        return;
    }
}
