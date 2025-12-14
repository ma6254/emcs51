#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <SEGGER_RTT.h>
#include <stm32f10x.h>
#include <emcs51.h>
#include <instruction/emcs51_general_inst.h>
#include <../testing/emcs51_testing.h>

uint8_t segger_up_buffer[1024] = {0};

static const uint8_t code_memory[] = {
    // IDATA_SIZE = 256
    // XDATA_SIZE = 0
    // 0x02, 0x00, 0x03, 0x78, 0x7F, 0xE4, 0xF6, 0xD8, 0xFD, 0x75, 0x81, 0x07,
    // 0x02, 0x00, 0x0F, 0xC2, 0x80, 0xD2, 0x80, 0x80, 0xFA};

    // IDATA_SIZE = 256
    // XDATA_SIZE = 1024
    0x02, 0x00, 0x03, 0x78, 0x7F, 0xE4, 0xF6, 0xD8, 0xFD, 0x90, 0x00, 0x00,
    0x7F, 0x00, 0x7E, 0x04, 0xE4, 0xF0, 0xA3, 0xDF, 0xFC, 0xDE, 0xFA, 0x75,
    0x81, 0x07, 0x02, 0x00, 0x1D, 0xC2, 0x80, 0xD2, 0x80, 0x80, 0xFA};

// IDATA_SIZE = 256
// XDATA_SIZE = 8192
// 0x02, 0x00, 0x03, 0x78, 0x7F, 0xE4, 0xF6, 0xD8, 0xFD, 0x90, 0x00, 0x00,
// 0x7F, 0x00, 0x7E, 0x20, 0xE4, 0xF0, 0xA3, 0xDF, 0xFC, 0xDE, 0xFA, 0x75,
// 0x81, 0x07, 0x02, 0x00, 0x1D, 0xC2, 0x80, 0xD2, 0x80, 0x80, 0xFA};

uint8_t xdata_memory[1024] = {0};

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

static int emcs51_P0_write_data_cb(uint8_t addr, uint8_t data)
{
    printf("[main] P0 write data: 0x%02X\r\n", data);

    return EMCS51_OK;
}

int main(void)
{
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(
        0, "JScope_u4u2",
        segger_up_buffer,
        sizeof(segger_up_buffer),
        // SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        // SEGGER_RTT_MODE_NO_BLOCK_TRIM);
        SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

    emcs51_core_t emcs51_core;
    emcs51_core_config_t emcs51_core_config = {
        .read_code_cb = emcs51_core_read_code_cb,
    };

    memset(&xdata_memory, 0xFF, sizeof(xdata_memory));

    emcs51_core_init(&emcs51_core, &emcs51_core_config);
    emcs51_general_inst_init(&emcs51_core);
    emcs51_core_reg_add(&emcs51_core, 0x80, emcs51_P0_write_data_cb, NULL);
    emcs51_core_set_xdata_ram(&emcs51_core, xdata_memory, sizeof(xdata_memory));

    emcs51_core_inst_dump(&emcs51_core);

    printf("[EMCS51] start\r\n");

    while (1)
    {
        emcs51_core_inc(&emcs51_core);
        if (emcs51_core.err < 0)
        {
            break;
        }

        if (emcs51_core.reg.pc == 0x11) 
        {
            uint8_t r6_data;
            uint8_t r7_data;

            emcs51_core_read_GPR(&emcs51_core, 6, &r6_data);
            emcs51_core_read_GPR(&emcs51_core, 7, &r7_data);

            printf("[EMCS51] xdata clear byte 0x%02X%02X\r\n", r6_data, r7_data);
        }

        // if (emcs51_core.reg.pc >= 0x1D)
        // {
        //     break;
        // }
    }

    printf("[main] core exit err:%d %s pc:0x%04X\r\n", emcs51_core.err, emcs51_err_name(emcs51_core.err), emcs51_core.reg.pc);

    while (1)
    {
    }
}

/*******************************************************************************
 * @brief printf()底层输出函数
 * @param ch 要输出的字符
 * @param f
 * @return none
 ******************************************************************************/
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, ch);
    return ch;
}

/*******************************************************************************
 * @brief 硬件错误处理函数
 * @param None
 * @return None
 ******************************************************************************/
void HardFault_Handler(void)
{
    printf("HardFault\r\n");

    while (1)
        ;
}
