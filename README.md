# CPU Latency Bench

x86/x64 CPU instruction latency measurement tool.

## Build & Run

```bash
gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm
./cpu-latency-bench [iterations] [runs]
```

## Parameters

- `iterations`: Loop count per test (default 1000000)
- `runs`: Number of test runs for averaging (default 3)

## Output Example

```
============================================================
           CPU Latency Benchmark v2.0
============================================================
  Compile: gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm
  Run: ./cpu-latency-bench [iterations] [runs]
============================================================

Params: iter=1000000, runs=3, tolerance=50%

Instr     Cat    Expect  Actual   Result
------------------------------------------------------------
NOP        Basic   1.00     1.07     [PASS]
ADD        ALU     1.00     0.82     [PASS]
SUB        ALU     1.00     0.83     [PASS]
...
============================================================
Result: 10/13 passed (76.9%)
```

## Add New Instructions

Find `/* Add new instruction tests here */` section and use the macro:

```c
TEST_INSN(name, "Category", expected_cycles, "asm_instruction")
```

Then register in `init_tests()`:

```c
register_test("NAME", "Category", expected, test_func);
```

**Example: Add ROL instruction**
```c
TEST_INSN(rol, "Shift", 1.0, "rol $1, %eax")
// ...
register_test("ROL", "Shift", 1.0, test_rol);
```

## Supported Instructions

| Category | Instructions |
|----------|--------------|
| Basic    | NOP          |
| ALU      | ADD, SUB     |
| Logic    | XOR, AND, OR, NOT |
| Mul      | IMUL         |
| Shift    | SHL, SHR     |
| Move     | MOV          |
| Cmp      | CMP, TEST    |

## Notes

- Measurements may be inaccurate under WSL/virtualization
- Some instructions (e.g., PUSH/POP) may crash in user mode
- Run on real hardware for accurate data
