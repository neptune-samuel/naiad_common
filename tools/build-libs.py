#!/usr/bin/env python3

import os 
import make_build as make
import platform
import shutil
import sys

## 前面的是用来检验是否需要编译的，如果文件已存在，就不编译了
targets = {
    'lib/libdocopt.so':'docopt.cpp-0.6.3.tar.gz',
    'lib/libfmt.a':'fmt-8.1.1.tar.gz',
    'lib/libuv.so':'libuv-1.44.2.tar.gz',
    'lib/libspdlog.a':'spdlog-1.11.0.tar.gz'
}

## 设定目录
zips_dir = "../zips"
build_dir = "../libs-build"
install_dir = "../local-install"

if len(sys.argv) > 1:
    install_dir = sys.argv[1]
else:
    install_dir = os.path.join(install_dir, platform.machine())

print("install_dir:", install_dir)

# ## 获取绝对位置
zips_abs_dir = os.path.abspath(zips_dir)
build_abs_dir = os.path.abspath(build_dir)
install_abs_dir = os.path.abspath(install_dir)

for lib,tgz in targets.items():
    test_file = os.path.join(install_abs_dir, lib)
    if os.path.exists(test_file):
        print("==> file(%s) exist, ignore %s" % (lib, tgz))
    else:
        make.MakeBuild(os.path.join(zips_abs_dir, tgz), build_abs_dir, install_abs_dir).install_all()

## 删除安装源目录
if os.path.exists(build_abs_dir):    
    shutil.rmtree(build_abs_dir)

