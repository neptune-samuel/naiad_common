#!/usr/bin/env python3

import tarfile 
import zipfile
import subprocess
import os
import shutil
import sys


class MakeBuild:
    def __init__(self, path, build_dir, install_dir):
        self._build_dir = build_dir
        self._install_dir = install_dir
        self._path = path 
        self._file = os.path.basename(path)
        self._file_base_name, self._file_ext_name = os.path.splitext(path)

        # 修正扩展名        
        if self._file.endswith(".tar.gz"):
            self._file_base_name = '.'.join(self._file.split('.')[:-2])
            self._file_ext_name = ".tar.gz"

        self._build_source_dir = os.path.join(self._build_dir, self._file_base_name)
        # .zip .tgz .tar .tar.gz .tar.bz2
        print("==> File: %s  %s" % (self._file_base_name, self._file_ext_name))
        

    def setup(self)->bool:   

        print("==> Create folder:", self._build_dir)
        print("==> Create folder:", self._install_dir)

        os.makedirs(self._build_dir, exist_ok=True)
        os.makedirs(self._install_dir, exist_ok=True)

        print("==> Untar source to ", self._build_dir)
        ## 解压源码        
        if self._file_ext_name == ".zip":
            with zipfile.ZipFile(self._path, "r") as zip_ref:
                zip_ref.extractall(self._build_dir)
        elif self._file_ext_name == ".tgz" or self._file_ext_name == ".tar.gz":
            with tarfile.open(self._path, "r:gz") as tar_ref:
                tar_ref.extractall(self._build_dir)
        else :
            with tarfile.open(self._path, "r:") as tar_ref:
                tar_ref.extractall(self._build_dir)

        if not os.path.isdir(self._build_source_dir):
            print("==> Source folder not exist, please check!")
            return False
        else:
            print("==> Source folder ready")
            return True

    def build(self)->bool:
        print("==> building source:", os.path.basename(self._build_source_dir))   
        cmake_build_dir = os.path.join(self._build_source_dir, "build")
        os.makedirs(cmake_build_dir, exist_ok=True)
        
        #subprocess.call(['cd', cmake_build_dir], check=True) 报错，不需要，cd是shell内置命令
        subprocess.run(['cmake', '..'], cwd=cmake_build_dir, check=True)
        result = subprocess.run(['cmake', '--build', '.'], cwd=cmake_build_dir, check=True)
        
        if result.returncode == 0:
            print("==> Build success")
            return True
        else:
            print("==> Build failed, ret=%d" % result.returncode)
            return False

    def install(self)->bool:
        print("==> install target:", os.path.basename(self._build_source_dir))
        cmake_build_dir = os.path.join(self._build_source_dir, "build")        
        os.makedirs(cmake_build_dir, exist_ok=True)
        result = subprocess.run(['cmake', '--install', '.', '--prefix', self._install_dir], cwd=cmake_build_dir, check=True)
        if result.returncode == 0:
            print("==> Install success")
            return True
        else:
            print("==> Install failed, ret=%d" % result.returncode)
            return False        

    def clean(self)->bool:
        print("==> clean source:", os.path.basename(self._build_source_dir))
        cmake_build_dir = os.path.join(self._build_source_dir, "build")   
        if os.path.exists(cmake_build_dir):
            shutil.rmtree(cmake_build_dir)
        return True
        
    def install_all(self):
        if not self.setup():
            return False 
        self.clean()
        if not self.build():
            return False 
        if not self.install():
            return False         
        return True

def parse_args(args:list):
    options = {}
    options['--build'] = 'build'
    options['--prefix'] = 'install'
    options['file'] = ""

    usage = """ Usage:
    {name} [-h,--help] [--build <dir>] [--prefix <dir>]  <file>

 Options:
    -h, --help      print this usage    
    --build         specify the building folder
    --prefix        specify the install prefix folder
"""
    n  = len(args)
    i = 1
    while i < n:
        if args[i] == '--help' or args[i] == '-h':
            print(usage.format(name=args[0]))
            sys.exit(0)
        elif args[i] == '--build' and i < n:
            a = args[i + 1]
            if a[0] == '-':
                print("***parse option '--build' failed, it needs an argument")
                return False, {}
            options['--build'] = args[i + 1]
            i += 1
        elif args[i] == '--prefix' and i < n:
            a = args[i + 1]
            if a[0] == '-': 
                print("***parse option '--prefix' failed, it needs an argument")
                return False, {}                
            options['--prefix'] = args[i + 1]
            i += 1
        else:
            if args[i][0] == '-':
                print("***unknown option:", args[i])
                return False, {}
            options['file'] = args[i]
        i += 1
    
    ## check file
    if len(options['file']) == 0:
        print(usage.format(name=args[0]))
        return False, {}

    return True, options
    

if __name__ == "__main__":
    ok, options = parse_args(sys.argv)
    if not ok:
        sys.exit(1)

    if not os.path.exists(options['file']):
        print("***Source file(%s) not found" % options['file'])
        sys.exit(1)
    
    mb = MakeBuild(options['file'], options['--build'], options['--prefix'])
    mb.setup()
    mb.clean()
    mb.build()
    mb.install()
