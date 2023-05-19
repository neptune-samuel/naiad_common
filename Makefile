
##
## Makefile for building third-party libraries
## 
## 
MACHINE=$(shell uname -m)
INSTALL_DIR:=$(shell pwd)/local-install/${MACHINE}
BUILD_TOOL:=$(shell pwd)/tools/build-libs.py 

.PHONY: all libs clean build install

all:
	@echo "-- Makefile for building libraries --"
	@echo " make libs  -- build all libraries"
	@echo "-- enjoy it --"

build:
	@mkdir -p build 
	@cd build && cmake .. && cmake --build .

install:
	@cd build && cmake --install .

libs:
	${BUILD_TOOL} ${INSTALL_DIR}

clean:
	@rm -rf build 

