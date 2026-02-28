# Agner Fog Instruction Tables - Floating Point Data

## Intel Skylake (Pages 294-320)

### x87 FPU Instructions (Page 300)
| Instruction | Latency | Reciprocal Throughput |
|------------|---------|---------------------|
| FADD/FSUB r | 1 | 0.5 |
| FMUL r | 1 | 0.5 |
| FDIV r | 14-16 | 4-5 |
| FSQRT | 14-21 | 4-7 |
| FADD/FSUB m | 2 | - |
| FMUL m | 1 | - |
| FDIV m | 4-5 | - |

### SSE/AVX Scalar Instructions (Page 308)
| Instruction | Latency | Reciprocal Throughput |
|------------|---------|---------------------|
| ADDSS/ADDSD | 1 | 0.5 |
| MULSS/MULSD | 1 | 0.5 |
| DIVSS | 1 | 13 |
| DIVSD | 1 | 3-14 |
| SQRTSS | 1 | 13 |
| SQRTSD | 1 | 3-14 |

### SSE/AVX Packed (128-bit) Instructions (Page 308)
| Instruction | Latency | Reciprocal Throughput |
|------------|---------|---------------------|
| ADDPS/ADDPD | 1 | 0.5 |
| MULPS/MULPD | 1 | 0.5 |
| DIVPS | 1 | 13 |
| DIVPD | 1 | 3-14 |
| SQRTPS | 1 | 13 |
| SQRTPD | 1 | 3-14 |

### AVX (256-bit) Instructions (Page 326)
| Instruction | Latency | Reciprocal Throughput |
|------------|---------|---------------------|
| VADDPS/VADDPD | 1 | 0.5 |
| VMULPS/VMULPD | 1 | 0.5 |
| VDIVPS | 1 | 15 |
| VDIVPD | 1 | 3-14 |

### Conversion Instructions
| Instruction | Latency | Notes |
|------------|---------|-------|
| CVTSI2SS | 2 | r32 to scalar |
| CVTSS2SI | 2 | scalar to r32 |
| CVTSI2SD | 2 | r32 to scalar |
| CVTSD2SI | 2 | scalar to r32 |

---

## AMD Zen 3 (From Manual)

### Floating Point
| Instruction | Latency | Throughput |
|------------|---------|------------|
| ADD/SUB | 1 | 0.5 |
| FMUL | 3 | 1 |
| FDIV | 15 | 15 |
| FSQRT | 12 | 12 |
| ADDPS/ADDPD | 1 | 0.5 |
| MULPS/MULPD | 1 | 0.5 |
| DIVPS | 13 | 13 |
| SQRTPS | 13 | 13 |

---

## Notes
- Data extracted from Agner Fog's "Instruction Tables" manual
- URL: https://www.agner.org/optimize/instruction_tables.pdf
- Latency = delay in dependency chain (clock cycles)
- Reciprocal throughput = average clock cycles per instruction for independent instructions
