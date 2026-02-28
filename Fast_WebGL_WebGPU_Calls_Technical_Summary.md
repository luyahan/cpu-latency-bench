# Fast WebGL/WebGPU Calls - 技术要点总结

> 来源: Fast WebGL_WebGPU calls.pdf  
> 作者: mslekova@  
> 更新时间: 2021-01-28

## 1. 背景与目标

**核心目标**: 减少 V8 生成的代码调用 C++ handlers 的开销，提升 WebGL/WebGPU 等性能关键 Web API 的性能。

**主要动机**:
- 减少从 JS 到 C++ 的调用开销
- 特别适用于 C++ handler 只是底层硬件调用（如 GPU）的薄包装的情况
- 限制：C++ handlers 不能在 JS 堆上分配对象，也不能触发 JS 执行

## 2. 核心方案

### 2.1 解决思路

扩展 V8 API，支持创建由 embedder 提供的 handler（callback）支持的 JS 方法。

### 2.2 接口规范

- 接口定义在头文件中
- 支持多种 C++ 类型到 API 表示的映射
- embedder 提供 C callback，签名示例：
  ```cpp
  void(receiver: JSObject, … args: <C types or JSObject>)
  ```

## 3. Embedder Callback 的要求

### 3.1 必须满足的条件

1. **不分配 JS 堆内存** - 由 embedder 保证
2. **不回调 JS** - 由 embedder 保证  
3. **错误处理幂等性** - 如果发出错误信号，低层 C 函数必须是幂等的，因为会被慢速绑定重新执行

### 3.2 原因

由于 host 函数不分配托管堆内存，调用点使用 `DisallowHeapAllocation` 和 `DisallowJavaScriptExecution` 保护。回调 JS 或分配新堆对象可能导致 V8 执行意外操作（如 GC）。

## 4. 实现细节

### 4.1 编译时

对接收者映射进行推断和检查：
1. 实例类型是 `kJSObjectType` 或其 API 相关子类型
2. 使用 `isTemplateFor` 检查 JSObject 与函数签名
3. 无需访问检查
4. embedder 对象索引在有效范围内
5. 任何检查失败则不生成快速调用

### 4.2 运行时 - 生成代码

1. 假设接收者有推断的映射之一（由 MapCheck 去优化保护）
2. 调用 embedder 提供的 C 函数
3. 检查 embedder callback 是否返回 `has_error`
4. 如有错误，回退到慢速 callback

### 4.3 运行时 - Embedder Callback

1. 从 wrapper JSObject 解包 C 指针
2. 对每个非原始参数：
   - 检查是 HeapObject 并获取其 map
   - 检查实例类型
   - 解包 C 指针
3. 调用底层 C 函数
4. (Blink 特有) 对返回值执行安全检查

## 5. 类型检查

### 5.1 V8 保证

- 接收者匹配函数签名或无需签名检查
- 接收者有有效的 embedder 槽数

### 5.2 Embedder 保证

- fast callback 不分配内存、不回调 JS
- 如果 fast callback 信号错误，slow callback 必须抛出异常
- `kV8Value` 类型的参数匹配预期的 embedder 类型
- 解包 `kV8Value` 类型的参数

### 5.3 Embedder 假设

- 如果有错误抛出，库函数是幂等的
- 低层 C 函数可重入执行而无副作用

## 6. Fallback 机制

如果任何 embedder 检查失败或底层调用信号错误，embedder 可以设置 `FastApiCallbackOptions` 的 `fallback` 字段为 `true` 并提前返回。

### Blink 错误报告流程

```
V8 → FastCall → 快速回调(fallback=true, 存储ExceptionState) 
   → 返回 → 检查 fallback → 如为 true 调用 SlowCall 
   → 检查 ExceptionState → 抛出 JSException
```

## 7. 未来优化方向

### 7.1 Blink 端 guards 优化

当前确保无 GC 分配和无 JS 执行的 guards 引入约 20% 性能开销。

### 7.2 Float/Double 支持

- 当前仅 x64 架构支持 float
- arm64: 修复 simulator bug
- arm: 添加 varargs 支持
- s390, ppc, mips: 外部贡献者支持
- x86: 不计划支持

### 7.3 Blink 注解处理

- `[Clamp]` 和 `[EnforceRange]` 注解处理
- `[RaisesException]` 函数处理

### 7.4 64位类型转换策略

解决不同 embedder 的需求差异：
- Blink: 转换为 IDL `unsigned long long`
- Node.js: 转换为 BigInt

## 8. 里程碑

### v0 - MVP

支持：
- **参数类型**: 原始类型、非包装 v8 对象
- **接收者类型**: (leaf 和 non-leaf) 接口类型
- **返回类型**: void

### v1 - 扩展类型

扩展覆盖更多接口：
- **参数类型**: non-leaf 接口类型、字符串、`[Clamp]` 注解、`unrestricted` 关键字
- **返回类型**: 原始类型、(leaf 和 non-leaf) 接口类型

## 9. 替代方案

### 9.1 FP 参数接口变更

1. **硬件支持** (当前实现): 使用硬件支持不同类型
2. **抽象**: 定义类似 `FunctionCallbackInfo` 的结构
3. **特定**: 整数、布尔指针参数在寄存器，float/double 在栈上
4. **抽象 no.2**: 每个 embedder callback 接受 TypeX 结构

### 9.2 接收者检查

- **生成代码检查**: 被拒绝（无法支持非叶子接口类型）
- **编译时检查**: 被拒绝（跳过必要检查）
- **映射推断**: 被拒绝（增加代码复杂度）

## 10. 性能测试

### 微基准测试

- WebGL Workload Simulator
- WebGL Animometer
- WebGPU Performance tests
- Node.js 微基准

### 实际应用

- WebGL Aquarium
- Unity Tiny Racing game

### 工具

- CPU profiler 支持
- chrome://tracing

## 11. 实际应用场景

Canvas2D API 是另一个受益者，当前受限于 bindings 层，是 CPU bound。

---

*注: 本文档根据 PDF 内容整理，详细实现请参考原始文档和 V8/Chromium 代码变更。*
