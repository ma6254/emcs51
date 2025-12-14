#include "emcs51.h"

/*******************************************************************************
 * @brief 回调函数：0x00 NOP指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_nop_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
}

static emcs51_inst_def_t nop_inst_def = {
    .mnemonic = "NOP",
    .length = 0,
    .cycles = 1,
    .exec_cb = emcs51_nop_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0x02 LJMP指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 * @details https://developer.arm.com/documentation/101655/0961/8051-Instruction-Set-Manual/Instructions/LJMP
 ******************************************************************************/
static void emcs51_ljmp_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    emcs51_core_t *core = event->core;

    uint16_t addr16 = (uint16_t)((core->operands[0] << 8) | core->operands[1]);

    core->reg.pc = addr16;
    core->is_jumped = true;
}

static emcs51_inst_def_t ljmp_inst_def = {
    .mnemonic = "LJMP addr16",
    .length = 2,
    .cycles = 2,
    .exec_cb = emcs51_ljmp_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0x75 MOV direct, #immediate 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_mov_direct_immed_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    emcs51_core_t *core = event->core;
    uint8_t iram_addr = core->operands[0];
    uint8_t data = core->operands[1];

    emcs51_write_data_cb_t write_data_cb = core->write_data_cb[iram_addr];

    if (write_data_cb)
        write_data_cb(iram_addr, data);

    core->data_ram[iram_addr] = data;
}

static emcs51_inst_def_t mov_direct_immed_inst_def = {
    .mnemonic = "MOV direct, #immediate", // MOV iram addr,#data
    .length = 2,
    .cycles = 2,
    .exec_cb = emcs51_mov_direct_immed_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0x78~0x7F MOV Rn, #immediate 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_mov_rn_immed_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    emcs51_core_t *core = event->core;
    uint8_t reg_num = event->opcode & 0x07; // R0-R7
    uint8_t data = core->operands[0];

    // Calculate the address of the register in the current register bank
    uint8_t rs = (core->reg.psw >> 3) & 0x03; // Register Bank Select bits (RS1, RS0)
    uint8_t iram_addr = (rs * 8) + reg_num;

    // printf("[EMCS51] MOV R%u, #0x%02X\r\n", reg_num, data);

    emcs51_write_data_cb_t write_data_cb = event->core->write_data_cb[iram_addr];

    if (write_data_cb)
        write_data_cb(iram_addr, data);

    core->data_ram[iram_addr] = data;
}

static emcs51_inst_def_t mov_rn_immed_inst_def = {
    .mnemonic = "MOV Rn, #immediate",
    .length = 1,
    .cycles = 1,
    .exec_cb = emcs51_mov_rn_immed_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0x80 SJMP指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_sjmp_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    emcs51_core_t *core = event->core;
    int8_t offset = (int8_t)core->operands[0];

    core->reg.pc = (uint32_t)core->reg.pc + 2 + offset;

    // printf("[EMCS51_sjmp_inst] SJMP to 0x%04X\r\n", core->reg.pc);

    core->is_jumped = true;
}

static emcs51_inst_def_t sjmp_inst_def = {
    .mnemonic = "SJMP offset",
    .length = 1,
    .cycles = 2,
    .exec_cb = emcs51_sjmp_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0x90 MOV DPTR #immediate 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_mov_dptr_immed_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    int err;
    emcs51_core_t *core = event->core;

    uint16_t dptr_value = (uint16_t)((core->operands[0] << 8) | core->operands[1]);

    emcs51_core_write_DPTR(core, dptr_value);
}

static emcs51_inst_def_t mov_dptr_immed_inst_def = {
    .mnemonic = "MOV DPTR, #immediate",
    .length = 2,
    .cycles = 2,
    .exec_cb = emcs51_mov_dptr_immed_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0xA3 INC DPTR 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_inc_dptr_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    int err;
    emcs51_core_t *core = event->core;

    uint16_t dptr_value;

    err = emcs51_core_read_DPTR(core, &dptr_value);
    if (err < 0)
    {
        core->err = err;
        return;
    }

    dptr_value++;

    // printf("[EMCS51] INC DPTR to 0x%04X\r\n", dptr_value);

    err = emcs51_core_write_DPTR(core, dptr_value);
    if (err < 0)
    {
        core->err = err;
        return;
    }
}

static emcs51_inst_def_t inc_dptr_inst_def = {
    .mnemonic = "INC DPTR",
    .length = 0,
    .cycles = 1,
    .exec_cb = emcs51_inc_dptr_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0xC2 CLR bit 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_clr_bit_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    int err;
    emcs51_core_t *core = event->core;
    uint8_t iram_addr = core->operands[0];

    // printf("[EMCS51] CLR bit 0x%02X\r\n", iram_addr);

    emcs51_write_data_cb_t write_data_cb = event->core->write_data_cb[iram_addr];

    if (write_data_cb)
        write_data_cb(iram_addr, 0x00);

    core->data_ram[iram_addr] = 0x00;
}

static emcs51_inst_def_t clr_bit_inst_def = {
    .mnemonic = "CLR bit",
    .length = 1,
    .cycles = 1,
    .exec_cb = emcs51_clr_bit_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0xD2 SET bit 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_set_bit_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    int err;
    emcs51_core_t *core = event->core;
    uint8_t iram_addr = core->operands[0];

    // printf("[EMCS51] SET bit 0x%02X\r\n", iram_addr);

    emcs51_write_data_cb_t write_data_cb = event->core->write_data_cb[iram_addr];

    if (write_data_cb)
        write_data_cb(iram_addr, 0x01);

    core->data_ram[iram_addr] = 0x01;
}

static emcs51_inst_def_t set_bit_inst_def = {
    .mnemonic = "SET bit",
    .length = 1,
    .cycles = 1,
    .exec_cb = emcs51_set_bit_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0xD8~0xDF DJNZ Rn, offset 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_djnz_rn_offset_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    int err;
    emcs51_core_t *core = event->core;
    uint8_t reg_num = event->opcode - 0xD8; // R0-R7

    uint8_t Rn_data;

    err = emcs51_core_read_GPR(core, reg_num, &Rn_data);
    if (err < 0)
    {
        core->err = err;
        return;
    }

    Rn_data--;

    // printf("[EMCS51] DJNZ R%d=0x%02X\r\n", reg_num, Rn_data);

    err = emcs51_core_write_GPR(core, reg_num, Rn_data);
    ;
    if (err < 0)
    {
        core->err = err;
        return;
    }

    if (Rn_data != 0)
    {
        int8_t offset = (int8_t)core->operands[0];

        core->reg.pc = (uint32_t)core->reg.pc + 2 + offset;

        core->is_jumped = true;
    }
}

static emcs51_inst_def_t djnz_rn_offset_inst_def = {
    .mnemonic = "DJNZ Rn, offset",
    .length = 1,
    .cycles = 1,
    .exec_cb = emcs51_djnz_rn_offset_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0xE4 CLR A 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_clr_a_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    emcs51_core_t *core = event->core;

    // printf("[EMCS51] CLR A\r\n");

    core->reg.a = 0x00;
}

static emcs51_inst_def_t clr_a_inst_def = {
    .mnemonic = "CLR A",
    .length = 0,
    .cycles = 1,
    .exec_cb = emcs51_clr_a_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0xF0 MOVX @DPTR, A 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_movx_at_dptr_a_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    int err;
    emcs51_core_t *core = event->core;
    uint16_t dptr_value;

    err = emcs51_core_read_DPTR(core, &dptr_value);
    if (err < 0)
    {
        core->err = err;
        return;
    }

    // printf("[EMCS51] MOVX @DPTR(0x%04X), A(0x%02X)\r\n", dptr_value, core->reg.a);

    if (dptr_value >= core->xdata_ram_size)
    {
        // core->err = EMCS51_ERR_XDATA_OUT_OF_RANGE;
        return;
    }

    core->xdata_ram[dptr_value] = core->reg.a;
}

static emcs51_inst_def_t movx_at_dptr_a_inst_def = {
    .mnemonic = "MOVX @DPTR, A",
    .length = 0,
    .cycles = 1,
    .exec_cb = emcs51_movx_at_dptr_a_inst_exec_cb,
};

/*******************************************************************************
 * @brief 回调函数：0xF6~0xF7 MOV @Ri, A 指令执行
 * @param event 指令执行事件结构体指针
 * @return none
 ******************************************************************************/
static void emcs51_mov_ari_a_inst_exec_cb(emcs51_inst_exec_event_t *event)
{
    int err;
    emcs51_core_t *core = event->core;
    uint8_t reg_num = event->opcode - 0xF6; // R0-R7

    uint8_t Rn_data = 0;

    err = emcs51_core_read_GPR(core, reg_num, &Rn_data);
    if (err < 0)
    {
        core->err = err;
        return;
    }

    emcs51_write_data_cb_t write_data_cb = event->core->write_data_cb[Rn_data];

    if (write_data_cb)
        write_data_cb(Rn_data, core->reg.a);

    core->data_ram[Rn_data] = core->reg.a;
}

static emcs51_inst_def_t mov_ari_a_inst_def = {
    .mnemonic = "MOV @Ri, A",
    .length = 0,
    .cycles = 1,
    .exec_cb = emcs51_mov_ari_a_inst_exec_cb,
};

/*******************************************************************************
 * @brief 注册通用指令
 * @param core 核心结构体指针
 * @return none
 ******************************************************************************/
void emcs51_general_inst_init(emcs51_core_t *core)
{
    emcs51_core_inst_add(core, 0x00, &nop_inst_def);

    emcs51_core_inst_add(core, 0x02, &ljmp_inst_def);

    emcs51_core_inst_add(core, 0x75, &mov_direct_immed_inst_def);

    emcs51_core_inst_add_range(core, 0x78, 8, &mov_rn_immed_inst_def);

    emcs51_core_inst_add(core, 0x80, &sjmp_inst_def);

    emcs51_core_inst_add(core, 0x90, &mov_dptr_immed_inst_def);

    emcs51_core_inst_add(core, 0xA3, &inc_dptr_inst_def);

    emcs51_core_inst_add(core, 0xC2, &clr_bit_inst_def);

    emcs51_core_inst_add(core, 0xD2, &set_bit_inst_def);

    emcs51_core_inst_add_range(core, 0xD8, 8, &djnz_rn_offset_inst_def);

    emcs51_core_inst_add(core, 0xE4, &clr_a_inst_def);

    emcs51_core_inst_add(core, 0xF0, &movx_at_dptr_a_inst_def);

    emcs51_core_inst_add_range(core, 0xF6, 2, &mov_ari_a_inst_def);
}
