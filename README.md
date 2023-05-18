
### 依赖库

- [libspdlog](https://github.com/gabime/spdlog.git)  (1.11.0)
- [libuv](https://github.com/libuv/libuv.git) (1.44.2)  
- [libfmt](https://github.com/fmtlib/fmt.git) (8.1.1only)
- [docopt.cpp](https://github.com/docopt/docopt.cpp.git) (0.6.3)

### 编译-依赖库

```sh

make libs

## 或者

cd tools

./build-libs.py 

```

所有依赖库默认安装到libs-install/<arch>目录

### 编译

```sh

mkdir build
cd build 
cmake ..
cmake --build .
cmake --install .

```

### 其它

如果安装了一些库，但想要删除掉，可以参考以下命令

```sh
#!/bin/sh

rm -rf /usr/local/lib/libfmt.a
rm -rf /usr/local/lib/cmake/fmt/
rm -rf /usr/local/include/fmt/
rm -rf /usr/local/lib/pkgconfig/fmt.pc

```

```sh
#!/bin/sh


rm -rf  /usr/local/include/spdlog
rm -rf  /usr/local/lib/libspdlog.a
rm -rf  /usr/local/include/spdlog/
rm -rf  /usr/local/lib/pkgconfig/spdlog.pc
rm -rf  /usr/local/lib/cmake/spdlog

```