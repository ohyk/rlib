# RICLib

## 编码约定

函数命名约定：

- 全小写

- 使用下划线分隔

- 模块名作为前缀

全局对象命名约定：大驼峰命名

局部对象命名约定：

- 全小写

- 使用下划线分隔

用户自定义类型命名约定：

- 大驼峰命名

- 采用后缀
  
  - struct SomeStruct_S
  
  - enum SomeEnum_E
  
  - union SomeUnion_U
  
  - 当需要使用typedef重命名tag时是一个例外，标签以及标识符均采用_T后缀；如ADT的不透明指针
  
  ```c
  // normal
  struct SomeStruct_S { ... };
  // expcept
  typedef struct SomeStruct_T *SomeStruct_T;
  ```

宏命名约定：

- 全大写

- 使用下划线分隔

错误处理约定：

- 库被调函数参数尽可能使用断言（rassert）做前置条件检测

- 库已检查运行时错误若导出异常则引发该异常，否则使用断言（rassert）

## 模块

---

### 基础设施

#### 断言（RAssert）

与标准库assert相似，区别仅在于引入了RExcept模块。当引发断言时，使用RExcept模块中的RAISE宏引发RAssertFailed异常。

#### 异常（RExcept）

#### 常规算术计算（RArith）

提供了一组标准库缺失，标准将其定义为未定义或由具体实现来定义但有用的功能，如对C除法和模运算提供确定良定义的结果。

#### 内存池（RArena）

#### 常规分配器与内存检查分配器（RMem）

#### 原子序列（RAtom）

#### 32位随机数生成器（RRandom）

#### 日志（RLog）

#### SHA256（RSha256）

#### 通用工具（RTools）

#### 测试工具（RTest）

---

### 抽象数据类型（ADT）

根据可复用与接口/实现的指导原则，ADT模块中除去向量（RRvec）模块外其余全部在接口中采用不透明指针来作为ADT的句柄来达到封装的目的。

```c
#define T SomeType_T T
typedef struct T *T;
//...
```

#### 链表（RList）

#### 动态数组（RArray）

#### 向量（RRvec/RRvector）

#### 哈希表（RTable）

#### 位向量（RBit）

#### 集合（RSet）

#### 序列（RSeq）

#### 环（RRing）
