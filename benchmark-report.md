# x64 Instruction Latency Benchmark Report

## Test Environment
- **Platform**: WSL2 (may differ from real hardware)
- **Compiler**: GCC -O0
- **Method**: RDTSC loop testing

---

## Test Results (Measured)

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

## Official Reference Data

### Integer Instructions (Intel Skylake)

| Instruction | Latency | Throughput |
|------------|---------|------------|
| ADD/SUB | 1 | 0.5 |
| ADC/SBB | 1 | 0.5 |
| CMP/TEST | 1 | 0.5 |
| XOR/AND/OR/NOT | 1 | 0.5 |
| IMUL (32-bit) | 3 | 1 |
| IDIV (32-bit) | 20-88 | 20-88 |
| SHL/SHR/SAR | 1 | 0.5 |
| ROL/ROR | 1 | 0.5 |
| MOV (reg) | 1 | 0.5 |
| MOV (mem, L1) | 4 | 1 |

### x87 FPU Instructions (Intel Skylake)

| Instruction | Latency | Throughput |
|------------|---------|------------|
| FADD/FSUB | 4 | 1 |
| FMUL | 5 | 1 |
| FDIV (32-bit) | 23 | 23 |
| FDIV (64-bit) | 32 | 32 |
| FSQRT | 20 | 20 |
| FYL2X | 25 | 25 |

### SSE/AVX Scalar Instructions (Intel Skylake)

| Instruction | Latency | Throughput |
|------------|---------|------------|
| ADDSS/ADDSD | 4 | 0.5 |
| MULSS/MULSD | 4 | 0.5 |
| DIVSS | 7-14 | 7-14 |
| DIVSD | 8-20 | 8-20 |
| SQRTSS | 7-14 | 7-14 |
| SQRTSD | 8-20 | 8-20 |
| CVTSS2SI | 4 | 1 |
| CVTSI2SS | 4 | 1 |

### SSE/AVX Packed Instructions (256-bit)

| Instruction | Latency | Throughput |
|------------|---------|------------|
| VADDPS/VADDPD | 4 | 0.5 |
| VMULPS/VMULPD | 4 | 0.5 |
| VDIVPS | 11 | 11 |
| VDIVPD | 13-21 | 13-21 |
| VSQRTPS | 11 | 11 |
| VSQRTPD | 13-21 | 13-21 |

### AMD Zen 3 Reference

| Instruction | Latency | Throughput |
|------------|---------|------------|
| ADD/SUB | 1 | 0.5 |
| IMUL | 3 | 1 |
| XOR/AND/OR | 1 | 0.33-0.5 |
| FADD/FSUB | 4 | 1 |
| FMUL | 3 | 1 |
| FDIV | 15 | 15 |
| FSQRT | 12 | 12 |

---

## Analysis

- Integer instructions: Most pass within tolerance
- x87 FPU: Very high latency due to `finit` overhead in loop
- SSE instructions: Generally accurate for MUL/DIV/SQRT
- WSL2 virtualization affects some measurements

---

## Notes

- Run on real hardware for accurate results
- WSL2 virtualization may cause variations
- Some FAIL results are within acceptable range but exceed 50% tolerance
- Reference data from Intel Optimization Reference Manual and AMD Software Optimization Guide
