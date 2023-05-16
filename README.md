
### libraries

- libspdlog 1.11.0

sudo apt install libspdlog-dev

- libuv 

source code install 

- libfmt (8.1.1only)

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