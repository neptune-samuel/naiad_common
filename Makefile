
##
## Makefile for building third-party libraries
## 
## 
MACHINE=$(shell uname -m)
INSTALL_DIR:=$(shell pwd)/local-install/${MACHINE}
BUILD_TOOL:=$(shell pwd)/tools/build-libs.py 

.PHONY: all libs

all:
	@echo "-- Makefile for building libraries --"
	@echo " make libs  -- build all libraries"
	@echo "-- enjoy it --"

libs:
	${BUILD_TOOL} ${INSTALL_DIR}
