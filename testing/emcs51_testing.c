#include "emcs51.h"
#include "emcs51_testing.h"

void emcs51_test_inst_nop(emcs51_testing_t *t);
void emcs51_testing_inst_sjmp_test1(emcs51_testing_t *t);
void emcs51_testing_inst_sjmp_test2(emcs51_testing_t *t);
void emcs51_test_inst_mov_direct_immed(emcs51_testing_t *t);

static emcs51_testing_config_t tests[] = {
    {"NOP Instruction Test", emcs51_test_inst_nop},
    {"SJMP Instruction Test1", emcs51_testing_inst_sjmp_test1},
    {"SJMP Instruction Test2", emcs51_testing_inst_sjmp_test2},
    {"MOV direct, #immediate Instruction Test", emcs51_test_inst_mov_direct_immed},
    {NULL, NULL},
};

void emcs51_testing(void)
{
    emcs51_testing_config_t *test_cfg = &tests[0];
    emcs51_testing_t t;

    printf("[EMCS51_testing] all tests start\r\n");

    while(1)
    {
        if ((strlen(test_cfg->name) == 0) || (test_cfg->test_func == NULL))
        {
            break;
        }

        memset(&t, 0, sizeof(emcs51_testing_t));
        t.cfg = *test_cfg;
        t.cfg.test_func(&t);

        if(t.err < 0)
        {
            printf("[EMCS51_testing][%s] finished with err:%d msg:%s\r\n", test_cfg->name, t.err, t.msg);
            break;
        }
        else
        {
            
            printf("[EMCS51_testing][%s] finished successfully\r\n", test_cfg->name);
        }

        test_cfg++;
    }

    printf("[EMCS51_testing] all tests finished\r\n");
}
