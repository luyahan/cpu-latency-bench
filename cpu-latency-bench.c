/*
 * x64 Instruction Latency Benchmark
 * Compile: gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm
 * Run: ./cpu-latency-bench [iterations] [runs]
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

__attribute__((optimize("O0")))

/* ==================== Utils ==================== */

static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

/* ==================== Test Core ==================== */

#define TEST(name, asm_code) \
    static double test_##name(int64_t iter) { \
        volatile int64_t count = iter; \
        uint64_t start = rdtsc(); \
        for (volatile int64_t i = count; i > 0; --i) __asm__ volatile (asm_code); \
        uint64_t end = rdtsc(); \
        return (end - start) / (double)iter; \
    }

#define TEST_SSE(name, asm_code) TEST(name, asm_code)

/* ==================== Test Functions ==================== */

// Integer
TEST(nop,     "nop")
TEST(add,     "add %eax, %eax")
TEST(sub,     "sub %eax, %eax")
TEST(xor,     "xor %eax, %eax")
TEST(and,     "and %eax, %eax")
TEST(or,      "or %eax, %eax")
TEST(not,     "not %eax")
TEST(imul,    "imul %eax, %eax")
TEST(shl,     "shl $1, %eax")
TEST(shr,     "shr $1, %eax")
TEST(mov,     "mov %eax, %ebx")
TEST(cmp,     "cmp %eax, %eax")
TEST(test,    "test %eax, %eax")

// SSE Scalar (32-bit)
TEST(addss,  "addss %xmm0, %xmm0")
TEST(mulss,  "mulss %xmm0, %xmm0")
TEST(divss,  "divss %xmm0, %xmm0")
TEST(sqrtss, "sqrtss %xmm0, %xmm0")

// SSE Scalar (64-bit)
TEST(addsd,  "addsd %xmm0, %xmm0")
TEST(mulsd,  "mulsd %xmm0, %xmm0")
TEST(divsd,  "divsd %xmm0, %xmm0")
TEST(sqrtsd, "sqrtsd %xmm0, %xmm0")

// Conversion
TEST(cvtsi2ss, "cvtsi2ss %eax, %xmm0")
TEST(cvtss2si, "cvtss2si %xmm0, %eax")

// Move
TEST(movss, "movss %xmm0, %xmm1")
TEST(movsd, "movsd %xmm0, %xmm1")

/* ==================== Registry ==================== */

typedef struct {
    const char* name;
    const char* category;
    double expected;
    double (*func)(int64_t);
} TestEntry;

static TestEntry g_tests[] = {
    // Integer
    {"NOP",   "Basic", 1.0, test_nop},
    {"ADD",   "ALU",   1.0, test_add},
    {"SUB",   "ALU",   1.0, test_sub},
    {"XOR",   "Logic", 1.0, test_xor},
    {"AND",   "Logic", 1.0, test_and},
    {"OR",    "Logic", 1.0, test_or},
    {"NOT",   "Logic", 1.0, test_not},
    {"IMUL",  "Mul",   3.0, test_imul},
    {"SHL",   "Shift", 1.0, test_shl},
    {"SHR",   "Shift", 1.0, test_shr},
    {"MOV",   "Move",  1.0, test_mov},
    {"CMP",   "Cmp",   1.0, test_cmp},
    {"TEST",  "Cmp",   1.0, test_test},
    // SSE 32-bit
    {"ADDSS",  "SSE", 3.0, test_addss},
    {"MULSS",  "SSE", 4.0, test_mulss},
    {"DIVSS",  "SSE", 14.0, test_divss},
    {"SQRTSS", "SSE", 14.0, test_sqrtss},
    // SSE 64-bit
    {"ADDSD",  "SSE", 3.0, test_addsd},
    {"MULSD",  "SSE", 4.0, test_mulsd},
    {"DIVSD",  "SSE", 14.0, test_divsd},
    {"SQRTSD", "SSE", 14.0, test_sqrtsd},
    // Conversion
    {"CVTSI2SS", "Conv", 4.0, test_cvtsi2ss},
    {"CVTSS2SI", "Conv", 4.0, test_cvtss2si},
    // Move
    {"MOVSS",  "SSE", 1.0, test_movss},
    {"MOVSD",  "SSE", 1.0, test_movsd},
};

#define TEST_COUNT (int)(sizeof(g_tests) / sizeof(g_tests[0]))

/* ==================== Main ==================== */

#define DEFAULT_ITER 1000000
#define DEFAULT_RUNS 3
#define TOLERANCE    0.5

int main(int argc, char* argv[]) {
    int64_t iter = DEFAULT_ITER;
    int runs = DEFAULT_RUNS;
    
    if (argc > 1) iter = atoll(argv[1]);
    if (argc > 2) runs = atoi(argv[2]);
    
    printf("============================================================\n");
    printf("           CPU Latency Benchmark\n");
    printf("============================================================\n");
    printf("  Compile: gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm\n");
    printf("  Run: ./cpu-latency-bench [iterations] [runs]\n");
    printf("============================================================\n\n");
    
    printf("Params: iter=%ld, runs=%d, tolerance=%.0f%%\n\n", iter, runs, TOLERANCE*100);
    printf("%-12s %-8s %-8s %-8s %-6s\n", "Instr", "Cat", "Expect", "Actual", "Result");
    printf("------------------------------------------------------------\n");
    
    int passed = 0;
    
    for (int i = 0; i < TEST_COUNT; i++) {
        double total = 0;
        for (int r = 0; r < runs; r++) total += g_tests[i].func(iter);
        double measured = total / runs;
        
        double diff_pct = fabs(measured - g_tests[i].expected) / g_tests[i].expected * 100;
        int pass = (diff_pct <= TOLERANCE * 100);
        if (pass) passed++;
        
        printf("%-12s %-8s %-8.2f %-8.2f [%s]\n",
               g_tests[i].name, g_tests[i].category,
               g_tests[i].expected, measured,
               pass ? "PASS" : "FAIL");
    }
    
    printf("------------------------------------------------------------\n");
    printf("Result: %d/%d passed (%.1f%%)\n", passed, TEST_COUNT, (double)passed/TEST_COUNT*100);
    
    return 0;
}
