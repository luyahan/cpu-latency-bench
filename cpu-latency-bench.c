/*
 * x64 Instruction Latency Benchmark v2.0
 * Compile: gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm
 * Run: ./cpu-latency-bench [iterations] [runs]
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

__attribute__((optimize("O0")))

/* Low-level utilities */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

/* Test macro - use this to add new instructions */
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
 * Add new instruction tests here
 * ============================================ */

// Basic arithmetic
TEST_INSN(nop,     "Basic", 1.0, "nop")
TEST_INSN(add,     "ALU",   1.0, "add %eax, %eax")
TEST_INSN(sub,     "ALU",   1.0, "sub %eax, %eax")
TEST_INSN(xor,     "Logic", 1.0, "xor %eax, %eax")
TEST_INSN(and,     "Logic", 1.0, "and %eax, %eax")
TEST_INSN(or,      "Logic", 1.0, "or %eax, %eax")
TEST_INSN(not,     "Logic", 1.0, "not %eax")

// Mul/Div
TEST_INSN(imul,    "Mul",   3.0, "imul %eax, %eax")

// Shift
TEST_INSN(shl,     "Shift", 1.0, "shl $1, %eax")
TEST_INSN(shr,     "Shift", 1.0, "shr $1, %eax")

// Data move
TEST_INSN(mov,     "Move",  1.0, "mov %eax, %ebx")

// Compare
TEST_INSN(cmp,     "Cmp",   1.0, "cmp %eax, %eax")
TEST_INSN(test,    "Cmp",   1.0, "test %eax, %eax")

/* Add new instruction example:
 * TEST_INSN(rol, "Shift", 1.0, "rol $1, %eax")
 */

/* ============================================
 * Instruction registry
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
    register_test("NOP",   "Basic", 1.0, test_nop);
    register_test("ADD",   "ALU",   1.0, test_add);
    register_test("SUB",   "ALU",   1.0, test_sub);
    register_test("XOR",   "Logic", 1.0, test_xor);
    register_test("AND",   "Logic", 1.0, test_and);
    register_test("OR",    "Logic", 1.0, test_or);
    register_test("NOT",   "Logic", 1.0, test_not);
    register_test("IMUL",  "Mul",   3.0, test_imul);
    register_test("SHL",   "Shift", 1.0, test_shl);
    register_test("SHR",   "Shift", 1.0, test_shr);
    register_test("MOV",   "Move",  1.0, test_mov);
    register_test("CMP",   "Cmp",   1.0, test_cmp);
    register_test("TEST",  "Cmp",   1.0, test_test);
    /* Add more: register_test("NEW", "Category", expected, test_func); */
}

/* ============================================
 * Main program
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
    printf("           CPU Latency Benchmark v2.0\n");
    printf("============================================================\n");
    printf("  Compile: gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm\n");
    printf("  Run: ./cpu-latency-bench [iterations] [runs]\n");
    printf("============================================================\n\n");
    
    printf("Params: iter=%ld, runs=%d, tolerance=%.0f%%\n\n", iter, runs, TOLERANCE*100);
    
    printf("%-10s %-8s %-8s %-8s %-6s\n", "Instr", "Cat", "Expect", "Actual", "Result");
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
    printf("Result: %d/%d passed (%.1f%%)\n", passed, g_count, (double)passed/g_count*100);
    
    return 0;
}
