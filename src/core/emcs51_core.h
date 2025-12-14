#ifndef EMCS51_CORE_H
#define EMCS51_CORE_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "core/emcs51_inst.h"

typedef int (*emcs51_read_code_cb_t)(uint16_t addr, uint8_t *data, uint16_t len);

typedef int (*emcs51_write_data_cb_t)(uint8_t addr, uint8_t data);
typedef int (*emcs51_read_data_cb_t)(uint8_t addr, uint8_t *data);

typedef enum EMCS51_CODE_TYPES
{
    EMCS51_CODE_NONE = 0, // 未定义code区
    EMCS51_CODE_CALLBACK, // 通过回调函数访问code区
    EMCS51_CODE_BUFFER,   // 通过指针访问code区
} emcs51_code_types_t;

typedef struct EMCS51_CORE_CONFIG
{
    emcs51_code_types_t code_type;
    emcs51_read_code_cb_t read_code_cb;
    uint8_t *code_buffer;

} emcs51_core_config_t;

typedef struct EMCS51_CORE_REG
{
    uint8_t a;     // ACC
    uint8_t b;     // 
    uint8_t sp;    // Stack Pointer

    uint16_t pc; // Program Counter

    union
    {
        uint8_t psw; // Program Status Word

        struct
        {
            uint8_t p : 1;    // Parity Flag
            uint8_t user : 1; // User-defined Flag
            uint8_t ov : 1;   // Overflow Flag
            uint8_t rs : 2;   // Register Bank Select
            uint8_t f0 : 1;   // User-defined Flag 0
            uint8_t ac : 1;   // Auxiliary Carry Flag
            uint8_t cy : 1;   // Carry Flag
        };
    };

} emcs51_core_reg_t;

typedef struct EMCS51_CORE
{
    int err;

    uint8_t data_ram[256];

    uint8_t *xdata_ram;
    uint32_t xdata_ram_size;

    emcs51_core_reg_t reg;
    emcs51_read_code_cb_t read_code_cb;

    emcs51_inst_def_t inst_def[256];
    emcs51_write_data_cb_t write_data_cb[256];
    emcs51_read_data_cb_t read_data_cb[256];
    uint8_t operands[4];

    uint32_t code_len;

    uint8_t is_jumped;
} emcs51_core_t;

void emcs51_core_init(emcs51_core_t *core, emcs51_core_config_t *config);
void emcs51_core_inst_add(emcs51_core_t *core, uint8_t opcode, emcs51_inst_def_t *inst_def);
void emcs51_core_inst_add_range(emcs51_core_t *core, uint8_t start, uint8_t len, emcs51_inst_def_t *inst_def);
void emcs51_core_reg_add(emcs51_core_t *core, uint8_t addr, emcs51_write_data_cb_t write_data_cb, emcs51_read_data_cb_t read_data_cb);
void emcs51_core_inst_dump(emcs51_core_t *core);

void emcs51_core_set_xdata_ram(emcs51_core_t *core, uint8_t *xdata_ram, uint32_t xdata_ram_size);

void emcs51_core_reset(emcs51_core_t *core);
void emcs51_core_inc(emcs51_core_t *core);

int emcs51_core_read_GPR(emcs51_core_t *core, uint8_t n, uint8_t *data);
int emcs51_core_write_GPR(emcs51_core_t *core, uint8_t n, uint8_t data);

int emcs51_core_read_DPTR(emcs51_core_t *core, uint16_t *data);
int emcs51_core_write_DPTR(emcs51_core_t *core, uint16_t data);

#endif // EMCS51_CORE_H
