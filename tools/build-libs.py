#!/usr/bin/env python3

import os 
import make_build as make
import platform
import shutil
import sys
import json

## 获取当前目录
local_dir =  os.path.dirname(os.path.abspath(__file__))
upper_dir = os.path.abspath(os.path.join(local_dir, ".."))
config_file = os.path.join(local_dir, "build-libs.json")

targets = {}

with open(config_file, "r") as fp:
    targets = json.load(fp)

## 设定目录
zips_dir =  os.path.join(upper_dir, "zips")
build_dir = os.path.join(upper_dir, "libs-build")
install_dir = os.path.join(upper_dir, "local-install")

if len(sys.argv) > 1:
    install_dir = sys.argv[1]
else:
    install_dir = os.path.join(install_dir, platform.machine())

print("install_dir:", install_dir)

for tgz, depend in targets.items():
    test_file = os.path.join(install_dir, depend)
    if os.path.exists(test_file):
        print("==> file(%s) exist, ignore %s" % (depend, tgz))
    else:
        make.MakeBuild(os.path.join(zips_dir, tgz), build_dir, install_dir).install_all()

## 删除安装源目录
if os.path.exists(build_dir):    
    shutil.rmtree(build_dir)

