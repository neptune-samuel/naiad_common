
### libraries

- [libspdlog](https://github.com/gabime/spdlog.git)  (1.11.0)

sudo apt install libspdlog-dev

- [libuv](https://github.com/libuv/libuv.git) (1.44.2)  

source code install 

- [libfmt](https://github.com/fmtlib/fmt.git) (8.1.1only)



source code install

### uninstall  

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