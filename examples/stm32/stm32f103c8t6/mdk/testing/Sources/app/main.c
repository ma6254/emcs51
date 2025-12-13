#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <SEGGER_RTT.h>
#include <stm32f10x.h>
#include <emcs51.h>
#include <../testing/emcs51_testing.h>

uint8_t segger_up_buffer[1024] = {0};

int main(void)
{
    SEGGER_RTT_Init();
    SEGGER_RTT_ConfigUpBuffer(
        0, "JScope_u4u2",
        segger_up_buffer,
        sizeof(segger_up_buffer),
        SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    // SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    // SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

    emcs51_testing();

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
