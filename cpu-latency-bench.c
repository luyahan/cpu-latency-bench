/*
 * x64 指令延迟测试框架 v2.0
 * 编译: gcc -O0 -o latency_test_x64 latency_test_x64.c -lm
 * 运行: ./latency_test_x64 [迭代次数] [测试次数]
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

__attribute__((optimize("O0")))

/* 底层工具 */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

/* 测试宏 - 使用这个简化新指令添加 */
#define TEST_INSN(name, category, expected, asm_code) \
    static double test_##name(int64_t iter) { \
        volatile int64_t count = iter; \
        uint64_t start = rdtsc(); \
        for (volatile int64_t i = count; i > 0; --i) { \
            __asm__ volatile (asm_code); \
        } \
        uint64_t end = rdtsc(); \
        return (end - start) / (double)iter; \
    }

/* ============================================
 * 在这里添加新指令测试
 * ============================================ */

// 基础运算
TEST_INSN(nop,     "基础", 1.0, "nop")
TEST_INSN(add,     "算术", 1.0, "add %eax, %eax")
TEST_INSN(sub,     "算术", 1.0, "sub %eax, %eax")
TEST_INSN(xor,     "逻辑", 1.0, "xor %eax, %eax")
TEST_INSN(and,     "逻辑", 1.0, "and %eax, %eax")
TEST_INSN(or,      "逻辑", 1.0, "or %eax, %eax")
TEST_INSN(not,     "逻辑", 1.0, "not %eax")

// 乘除法
TEST_INSN(imul,    "乘除", 3.0, "imul %eax, %eax")

// 位移
TEST_INSN(shl,     "位移", 1.0, "shl $1, %eax")
TEST_INSN(shr,     "位移", 1.0, "shr $1, %eax")

// 数据移动
TEST_INSN(mov,     "移动", 1.0, "mov %eax, %ebx")

// 比较
TEST_INSN(cmp,     "比较", 1.0, "cmp %eax, %eax")
TEST_INSN(test,    "比较", 1.0, "test %eax, %eax")

/* 添加新指令示例:
 * TEST_INSN(指令名, "类别", 预期值, "汇编指令")
 */

/* ============================================
 * 指令注册表
 * ============================================ */

typedef struct {
    const char* name;
    const char* category;
    double expected;
    double (*func)(int64_t);
} TestEntry;

#define MAX_TEST 50
static TestEntry g_tests[MAX_TEST];
static int g_count = 0;

static void register_test(const char* n, const char* c, double e, double (*f)(int64_t)) {
    g_tests[g_count++] = (TestEntry){n, c, e, f};
}

static void init_tests(void) {
    register_test("NOP",   "基础", 1.0, test_nop);
    register_test("ADD",   "算术", 1.0, test_add);
    register_test("SUB",   "算术", 1.0, test_sub);
    register_test("XOR",   "逻辑", 1.0, test_xor);
    register_test("AND",   "逻辑", 1.0, test_and);
    register_test("OR",    "逻辑", 1.0, test_or);
    register_test("NOT",   "逻辑", 1.0, test_not);
    register_test("IMUL",  "乘除", 3.0, test_imul);
    register_test("SHL",   "位移", 1.0, test_shl);
    register_test("SHR",   "位移", 1.0, test_shr);
    register_test("MOV",   "移动", 1.0, test_mov);
    register_test("CMP",   "比较", 1.0, test_cmp);
    register_test("TEST",  "比较", 1.0, test_test);
    /* 在这里添加更多: register_test("新指令", "类别", 预期值, test_函数); */
}

/* ============================================
 * 主程序
 * ============================================ */

#define DEFAULT_ITER 1000000
#define DEFAULT_RUNS 3
#define TOLERANCE    0.5

int main(int argc, char* argv[]) {
    int64_t iter = DEFAULT_ITER;
    int runs = DEFAULT_RUNS;
    
    if (argc > 1) iter = atoll(argv[1]);
    if (argc > 2) runs = atoi(argv[2]);
    
    init_tests();
    
    printf("============================================================\n");
    printf("           x64 指令延迟测试框架 v2.0\n");
    printf("============================================================\n");
    printf("  编译: gcc -O0 -o latency_test_x64 latency_test_x64.c -lm\n");
    printf("  运行: ./latency_test_x64 [迭代次数] [测试次数]\n");
    printf("============================================================\n\n");
    
    printf("参数: 迭代=%ld, 测试次数=%d, 容差=%.0f%%\n\n", iter, runs, TOLERANCE*100);
    
    printf("%-10s %-8s %-8s %-8s %-6s\n", "指令", "类别", "预期", "实测", "结果");
    printf("------------------------------------------------------------\n");
    
    int passed = 0;
    
    for (int i = 0; i < g_count; i++) {
        double total = 0;
        for (int r = 0; r < runs; r++) {
            total += g_tests[i].func(iter);
        }
        double measured = total / runs;
        
        double diff_pct = fabs(measured - g_tests[i].expected) / g_tests[i].expected * 100;
        int pass = (diff_pct <= TOLERANCE * 100);
        if (pass) passed++;
        
        printf("%-10s %-8s %-8.2f %-8.2f [%s]\n",
               g_tests[i].name, g_tests[i].category,
               g_tests[i].expected, measured,
               pass ? "PASS" : "FAIL");
    }
    
    printf("------------------------------------------------------------\n");
    printf("结果: %d/%d 通过 (%.1f%%)\n", passed, g_count, (double)passed/g_count*100);
    
    return 0;
}
