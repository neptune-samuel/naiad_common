
### target_compile_definitions

在 CMake 中，可以使用 `target_compile_definitions` 命令为一个目标（如可执行文件、静态库或共享库）添加编译定义。编译定义是在编译时定义的预处理器符号，可以在源代码中使用 `#ifdef` 或 `#ifndef` 进行条件编译。

`target_compile_definitions` 的语法如下：

```
target_compile_definitions(target_name
    PUBLIC [items...]
    PRIVATE [items...]
    INTERFACE [items...]
)
```

其中，`target_name` 是目标名称，`items` 是一个或多个编译定义，可以使用 `add_definitions` 命令添加。

`PUBLIC`、`PRIVATE` 和 `INTERFACE` 是可选的关键字，用于指定编译定义的可见性。它们的含义如下：

- `PUBLIC`：编译定义将被添加到目标的编译选项和依赖项的编译选项中。
- `PRIVATE`：编译定义将仅被添加到目标的编译选项中。
- `INTERFACE`：编译定义将仅被添加到目标的依赖项的编译选项中。

下面是一个示例，演示如何为一个可执行文件添加编译定义：

```cmake
add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE DEBUG)
```

这将在编译时定义一个名为 `DEBUG` 的符号，可以在源代码中使用 `#ifdef DEBUG` 进行条件编译。注意，这个编译定义仅会被添加到 `myapp` 目标的编译选项中，不会影响其他目标的编译选项。

### cmake 中如何将目录下的所有文件（如所有.c文件）添加到指定变量？

可以使用 `file(GLOB ...)` 命令来实现将目录下的所有文件添加到指定变量。具体的命令格式如下：

```
file(GLOB <variable> [LIST_DIRECTORIES true|false] [RELATIVE <path>] [<globbing-expressions>...])
```

其中，`<variable>` 是要存储文件列表的变量名，`<globbing-expressions>` 是用于匹配文件的通配符表达式，可以使用 `*.c` 匹配目录下的所有 `.c` 文件。

例如，如果要将 `src` 目录下的所有 `.c` 文件添加到 `SRCS` 变量中，可以这样写：

```
file(GLOB SRCS src/*.c)
```

需要注意的是，`file(GLOB ...)` 命令会在 CMake 生成项目时自动扫描目录下的文件，如果目录中的文件发生变化，需要重新运行 CMake 以更新文件列表。另外，由于 `file(GLOB ...)` 命令会在 CMake 生成项目时自动扫描目录下的所有文件，如果文件数量很大，可能会导致 CMake 生成项目的速度变慢。因此，如果目录下的文件数量很大，建议使用 `add_executable()` 和 `add_library()` 命令手动指定文件列表。
