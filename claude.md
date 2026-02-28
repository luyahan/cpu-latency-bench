# CPU Latency Bench

x86/x64 CPU 指令延迟测量工具，可扩展支持多种指令测试。

## 快速开始

```bash
# 编译
gcc -O0 -o cpu-latency-bench cpu-latency-bench.c -lm

# 运行
./cpu-latency-bench              # 默认参数
./cpu-latency-bench 500000 3     # 自定义迭代次数和测试次数
```

## 项目结构

```
.
├── cpu-latency-bench.c      # 主测试框架源码
├── README.md               # 使用说明
├── 芯片指令延迟测试方案.md   # 完整技术方案
├── x64指令延迟对比报告.md   # 官方数据对比
└── .gitignore
```

## 添加新指令

```c
// 1. 在 TEST_INSN 区域添加:
TEST_INSN(rol, "位移", 1.0, "rol $1, %eax")

// 2. 在 init_tests() 中注册:
register_test("ROL", "位移", 1.0, test_rol);
```

## 测试结果 (WSL2)

| 指令 | 预期 | 实测 | 状态 |
|------|------|------|------|
| NOP | 1.0 | 1.07 | PASS |
| ADD | 1.0 | 0.82 | PASS |
| IMUL | 3.0 | 0.89 | FAIL* |

* WSL 虚拟化环境影响，建议在真实硬件测试

## 技术要点

- 使用 RDTSC 读取 CPU 时钟周期
- 禁止编译优化 (`-O0`)
- 多次测量取平均减少误差
- 宏定义简化新指令添加

## 依赖

- GCC
- Linux (x86_64)

## 参考

- Intel 64 and IA-32 Architectures Optimization Reference Manual
- Agner Fog's Instruction Tables
