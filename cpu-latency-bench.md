# CPU Instruction Latency Testing Specification

## 1. Test Objectives
Measure instruction latency and throughput for x64, ARM, and RISC-V architectures.

## 2. Core Concepts

### Instruction Latency
- Number of clock cycles from instruction start to result generation
- Example: integer ADD ~1 cycle, MUL ~3-5 cycles

### Instruction Throughput
- Instructions per cycle in steady-state pipeline
- Inverse = cycles to execute N instructions

---

## 3. Technical Challenges

| Challenge | Description | Solution |
|-----------|-------------|----------|
| **CPU Cache** | First execution slow, subsequent from cache | Warmup + multiple runs |
| **Branch Prediction** | Conditional jumps affect pipeline | Use branchless code |
| **Instruction Reordering** | CPU dynamically reorders | Use `lfence` / `cpuid` serialization |
| **Dynamic Frequency** | Modern CPU turbo/downclock | Lock frequency or use averages |
| **Memory Access** | Load/store affected by memory | Use register-only instructions |
| **Cross-arch Differences** | x86/ARM/RISC-V ISAs differ | Write separate tests per arch |

---

## 4. Test Methods

### 1. Using CPU Internal Counters

**x86 (RDTSC)**
```asm
rdtsc
mov ebx, eax
; execute instruction under test
rdtsc
sub eax, ebx
```

**ARM (CNTVCT)**
```asm
mrs x0, cntvct_el0
; execute instruction under test
mrs x1, cntvct_el0
sub x0, x1, x0
```

**RISC-V (cycle CSR)**
```asm
csrr t0, cycle
; execute instruction under test
csrr t1, cycle
sub t2, t1, t0
```

---

## 5. Reference Instruction Latency

### Integer Operations
| Instruction | x86 Latency | ARM64 | RISC-V |
|------------|-------------|-------|--------|
| ADD/SUB | 1 | 1 | 1 |
| MUL | 3-4 | 2-4 | 3-5 |
| DIV | 20-80 | 4-40 | 3-60 |

### Memory Access
| Instruction | x86 Latency | Notes |
|------------|-------------|-------|
| LOAD (L1) | 4 | L1 cache hit |
| LOAD (L2) | 12 | L2 cache hit |
| LOAD (RAM) | 50-200 | Main memory |

---

## 6. Execution Plan

- [ ] Phase 1: Write C test framework for x86/ARM/RISC-V
- [ ] Phase 2: Implement basic instruction tests (ALU, logic, shift)
- [ ] Phase 3: Add memory access latency tests
- [ ] Phase 4: Implement cross-platform build scripts
- [ ] Phase 5: Generate comparison reports

---

## 7. Required Tools

- GCC/Clang compiler
- QEMU (ARM/RISC-V emulation)
- objdump / objconv (disassembler)
- Linux perf (optional)
