# CPU Latency Bench

x86/x64 CPU instruction latency measurement tool with extensible architecture.

## Quick Start

```bash
# Build
gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm

# Run
./cpu-latency-bench              # Default params
./cpu-latency-bench 500000 3     # Custom iterations and runs
```

## Project Structure

```
.
├── cpu-latency-bench.c      # Main source code
├── README.md               # Documentation
├── cpu-latency-bench.md     # Technical spec
├── benchmark-report.md      # Official data comparison
└── .gitignore
```

## Add New Instructions

```c
// 1. Add in TEST_INSN section:
TEST_INSN(rol, "Shift", 1.0, "rol $1, %eax")

// 2. Register in init_tests():
register_test("ROL", "Shift", 1.0, test_rol);
```

## Test Results (WSL2)

| Instr | Expect | Actual | Status |
|-------|--------|--------|--------|
| NOP   | 1.0    | 1.07   | PASS   |
| ADD   | 1.0    | 0.82   | PASS   |
| IMUL  | 3.0    | 0.89   | FAIL*  |

* Virtualization may affect results; real hardware recommended

## Technical Details

- Uses RDTSC to read CPU cycle counter
- Compiles with `-O0` to prevent optimization
- Multiple runs averaged to reduce error
- Macro-based extension system

## Dependencies

- GCC
- Linux (x86_64)

## References

- Intel 64 and IA-32 Architectures Optimization Reference Manual
- Agner Fog's Instruction Tables

---

## Git Rules

- **DO NOT push compiled binaries** - Only commit source code
- Update `.gitignore` to exclude build artifacts
