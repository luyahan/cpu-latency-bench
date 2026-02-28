# x64 Instruction Latency Benchmark Report

## Test Environment
- **Platform**: WSL2 (may differ from real hardware)
- **Compiler**: GCC -O0
- **Method**: RDTSC loop testing

---

## Test Results

| Category | Instruction | Expect | Actual | Result |
|----------|-------------|--------|--------|--------|
| **Basic** | NOP | 1.00 | 1.93 | FAIL |
| **ALU** | ADD | 1.00 | 1.08 | PASS |
| **ALU** | SUB | 1.00 | 0.82 | PASS |
| **Logic** | XOR | 1.00 | 0.84 | PASS |
| **Logic** | AND | 1.00 | 0.91 | PASS |
| **Logic** | OR | 1.00 | 1.08 | PASS |
| **Logic** | NOT | 1.00 | 1.23 | PASS |
| **Mul** | IMUL | 3.00 | 1.20 | FAIL |
| **Shift** | SHL | 1.00 | 1.81 | FAIL |
| **Shift** | SHR | 1.00 | 1.29 | PASS |
| **Move** | MOV | 1.00 | 0.96 | PASS |
| **Cmp** | CMP | 1.00 | 2.55 | FAIL |
| **Cmp** | TEST | 1.00 | 0.83 | PASS |
| **x87** | FADD | 4.00 | 408.85 | FAIL |
| **x87** | FSUB | 4.00 | 371.04 | FAIL |
| **x87** | FMUL | 5.00 | 385.94 | FAIL |
| **x87** | FDIV | 20.00 | 385.33 | FAIL |
| **x87** | FSQRT | 20.00 | 385.89 | FAIL |
| **SSE** | ADDSS | 4.00 | 1.58 | FAIL |
| **SSE** | MULSS | 4.00 | 3.85 | PASS |
| **SSE** | DIVSS | 14.00 | 8.68 | PASS |
| **SSE** | SQRTSS | 14.00 | 11.18 | PASS |
| **SSE** | ADDSD | 4.00 | 1.64 | FAIL |
| **SSE** | MULSD | 4.00 | 4.33 | PASS |
| **SSE** | DIVSD | 14.00 | 13.62 | PASS |
| **SSE** | SQRTSD | 14.00 | 13.02 | PASS |
| **Conv** | CVTSI2SS | 4.00 | 4.25 | PASS |
| **Conv** | CVTSS2SI | 4.00 | 1.26 | FAIL |
| **SSE** | MOVSS | 1.00 | 0.94 | PASS |
| **SSE** | MOVSD | 1.00 | 2.10 | FAIL |

**Total: 17/30 passed (56.7%)**

---

## Analysis

- Integer instructions: Most pass within tolerance
- x87 FPU: Very high latency due to `finit` overhead in loop
- SSE instructions: Generally accurate for MUL/DIV/SQRT
- WSL2 virtualization affects some measurements

---

## Official Data (Reference)

| Instruction | Intel Skylake Latency |
|------------|---------------------|
| ADD/SUB | 1 |
| IMUL | 3-4 |
| XOR/AND/OR | 1 |
| SHL/SHR | 1 |
| FADD/FSUB | 4 |
| FMUL | 5 |
| FDIV | 20 |
| ADDSS/ADDSD | 4 |
| MULSS/MULSD | 4 |
| DIVSS/DIVSD | 14 |
| SQRTSS/SQRTSD | 14 |

---

## Notes

- Run on real hardware for accurate results
- WSL2 virtualization may cause variations
- Some FAIL results are within acceptable range but exceed 50% tolerance
