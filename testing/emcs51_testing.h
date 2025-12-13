#ifndef EMCS51_TESTING_H
#define EMCS51_TESTING_H

struct EMCS51_TESTING;

typedef void (*emcs51_testing_func_cb_t)(struct EMCS51_TESTING *t);

typedef struct EMCS51_TESTING_CONFIG
{
    char name[256];                     // 测试名称
    emcs51_testing_func_cb_t test_func; // 测试函数
} emcs51_testing_config_t;

typedef struct EMCS51_TESTING
{
    int err;       // 错误代码
    char msg[256]; // 错误信息
    emcs51_testing_config_t cfg;
} emcs51_testing_t;

void emcs51_testing(void);

#endif // EMCS51_TESTING_H
