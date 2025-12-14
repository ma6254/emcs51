#include "emcs51.h"

/*******************************************************************************
 * @brief 初始化核心
 * @param core 核心结构体指针
 * @param config 配置参数
 * @return none
 ******************************************************************************/
void emcs51_core_init(emcs51_core_t *core, emcs51_core_config_t *config)
{
    if (core == NULL)
        return;

    if (config == NULL)
        return;

    memset(core, 0, sizeof(emcs51_core_t));

    core->read_code_cb = config->read_code_cb;
}

/*******************************************************************************
 * @brief 注册核心指令范围
 * @param core     核心结构体指针
 * @param start    指令操作码起始值
 * @param len      指令操作码范围
 * @param inst_def 指令定义结构体指针
 * @return none
 ******************************************************************************/
void emcs51_core_inst_add_range(emcs51_core_t *core, uint8_t start, uint8_t len, emcs51_inst_def_t *inst_def)
{
    if (core == NULL)
        return;

    if (len == 0)
        return;

    if (inst_def == NULL)
        return;

    if (((uint16_t)start + len) > 256)
        return;

    for (uint32_t i = 0; i < len; i++)
    {
        core->inst_def[start + i] = *inst_def;
    }
}

/*******************************************************************************
 * @brief 注册核心指令单个
 * @param core     核心结构体指针
 * @param start    指令操作码
 * @param inst_def 指令定义结构体指针
 * @return none
 ******************************************************************************/
void emcs51_core_inst_add(emcs51_core_t *core, uint8_t opcode, emcs51_inst_def_t *inst_def)
{

    if (core == NULL)
        return;

    if (inst_def == NULL)
        return;

    core->inst_def[opcode] = *inst_def;
}

/*******************************************************************************
 * @brief 注册DATA区寄存器
 * @param core          核心结构体指针
 * @param write_data_cb 寄存器写入回调函数
 * @param read_data_cb  寄存器读取回调函数
 * @return none
 ******************************************************************************/
void emcs51_core_reg_add(emcs51_core_t *core, uint8_t addr, emcs51_write_data_cb_t write_data_cb, emcs51_read_data_cb_t read_data_cb)
{
    if (core == NULL)
        return;

    core->write_data_cb[addr] = write_data_cb;
    core->read_data_cb[addr] = read_data_cb;
}

void emcs51_core_inst_dump(emcs51_core_t *core)
{

    uint32_t i = 0;
    uint32_t line_i = 0;
    uint8_t mnemonic[5];

    if (core == NULL)
        return;

    printf("[EMCS51][inst_dump] ==================================================\r\n");
    printf("[EMCS51][inst_dump]     0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F\r\n");

    for (line_i = 0; line_i < 256; line_i += 0x10)
    {
        printf("[EMCS51][inst_dump] %02X", line_i);

        for (i = 0; i < 0x10; i++)
        {
            emcs51_inst_def_t inst_def = core->inst_def[line_i + i];
            uint8_t is_defined = 0;

            memset(mnemonic, 0, sizeof(mnemonic));

            if (inst_def.mnemonic != NULL)
            {
                if (strlen(inst_def.mnemonic) > 0)
                {
                    for (uint8_t j = 0; j < 4; j++)
                    {
                        if ((inst_def.mnemonic[j] == ' ') || (inst_def.mnemonic[j] == '\0'))
                        {
                            mnemonic[j] = '\0';
                            break;
                        }

                        mnemonic[j] = inst_def.mnemonic[j];
                    }

                    is_defined = 1;
                }

            }

            if (is_defined)
            {
                // printf(" %02X  ", i + line_i);
                printf(" %-4s", mnemonic);
            }
            else
            {
                printf(" ----");
            }
        }
        printf("\r\n");
    }

    printf("[EMCS51][inst_dump] ==================================================\r\n");
}

/*******************************************************************************
 * @brief 设置XDATA区内存
 * @param core 核心结构体指针
 * @param xdata_ram XDATA区内存指针
 * @param xdata_ram_size XDATA区内存大小
 * @return none
 ******************************************************************************/
void emcs51_core_set_xdata_ram(emcs51_core_t *core, uint8_t *xdata_ram, uint32_t xdata_ram_size)
{
    if (core == NULL)
        return;

    core->xdata_ram = xdata_ram;
    core->xdata_ram_size = xdata_ram_size;
}

/*******************************************************************************
 * @brief 复位核心
 * @param core 核心结构体指针
 * @return none
 ******************************************************************************/
void emcs51_core_reset(emcs51_core_t *core)
{
    if (core == NULL)
        return;

    memset(&core->reg, 0, sizeof(emcs51_core_reg_t));
}

/*******************************************************************************
 * @brief 执行单条指令
 * @param core 核心结构体指针
 * @return none
 ******************************************************************************/
void emcs51_core_inc(emcs51_core_t *core)
{
    int err;
    uint8_t opcode = 0;

    if (core == NULL)
    {
        return;
    }

    if (core->err < 0)
    {
        return;
    }

    // printf("[EMCS51_core] pc: 0x%04X\r\n", core->reg.pc);

    err = core->read_code_cb(core->reg.pc, &opcode, 1);
    if (err < 0)
    {
        core->err = err;
        return;
    }

    // printf("[EMCS51_core] opcode: 0x%02X\r\n", opcode);

    // read instruction operands
    emcs51_inst_def_t *inst_def = &core->inst_def[opcode];

    if ((inst_def->mnemonic == NULL) || (inst_def->cycles == 0))
    {
        printf("[EMCS51_core] unknown opcode: 0x%02X\r\n", opcode);
        core->err = EMCS51_ERR_UNKNOWN_INST;
        return;
    }

    // printf("[EMCS51_core] mnemonic: %s\r\n", inst_def->mnemonic);

    err = core->read_code_cb(core->reg.pc + 1, (uint8_t *)core->operands, inst_def->length);
    if (err < 0)
    {
        core->err = err;
        return;
    }

    // printf("[EMCS51_core] pc: 0x%04X opcode: 0x%02X mnemonic: %s\r\n", core->reg.pc, opcode, inst_def->mnemonic);

    // execute instruction
    if (inst_def->exec_cb != NULL)
    {
        emcs51_inst_exec_event_t event = {
            .opcode = opcode,
            .core = core,
            .inst_def = inst_def,
        };
        inst_def->exec_cb(&event);
    }

    if (((uint16_t)core->reg.pc + 1 + inst_def->length) > 0xFFFF)
    {
        core->err = EMCS51_ERR_CODE_OUT_OF_RANGE;
        return;
    }

    // instruction execution completed

    if (core->is_jumped == false)
        core->reg.pc += (1 + inst_def->length);

    if (core->is_jumped)
    {
        core->is_jumped = false;

        // printf("[EMCS51_core] jump to 0x%04X\r\n", core->reg.pc);
    }
}

/*******************************************************************************
 * @brief 读取通用寄存器R0~R7
 * @param core 核心结构体指针
 * @param n    寄存器编号
 * @param data 读取的数据
 * @return emcs51_err_t
 ******************************************************************************/
int emcs51_core_read_GPR(emcs51_core_t *core, uint8_t n, uint8_t *data)
{
    uint8_t reg_bank = core->reg.rs;

    if (n > 7)
        return EMCS51_ERR;

    uint8_t iram_addr = (reg_bank * 8) + n;

    *data = core->data_ram[iram_addr];

    return EMCS51_OK;
}

/*******************************************************************************
 * @brief 写入通用寄存器R0~R7
 * @param core 核心结构体指针
 * @param n    寄存器编号
 * @param data 写入的数据
 * @return emcs51_err_t
 ******************************************************************************/
int emcs51_core_write_GPR(emcs51_core_t *core, uint8_t n, uint8_t data)
{
    uint8_t reg_bank = core->reg.rs;

    if (n > 7)
        return EMCS51_ERR;

    uint8_t iram_addr = (reg_bank * 8) + n;

    core->data_ram[iram_addr] = data;

    return EMCS51_OK;
}

int emcs51_core_read_DPTR(emcs51_core_t *core, uint16_t *data)
{

    uint8_t DPH_data = core->data_ram[0x82];
    uint8_t DPL_data = core->data_ram[0x83];

    *data = ((uint16_t)DPH_data << 8) | DPL_data;

    return EMCS51_OK;
}

int emcs51_core_write_DPTR(emcs51_core_t *core, uint16_t data)
{
    uint8_t DPH_data = (data >> 8) & 0xFF;
    uint8_t DPL_data = data & 0xFF;

    core->data_ram[0x82] = DPH_data;
    core->data_ram[0x83] = DPL_data;

    return EMCS51_OK;
}
