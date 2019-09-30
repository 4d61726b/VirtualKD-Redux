# Time-stamp: <05/09/09 21:03:45 ptr>
# $Id: targetsys.mak,v 1.2 2008-02-26 14:59:41 bazis Exp $

CC ?= gcc
CXX ?= g++

# shared library:
SO  := dll
ifeq (gcc,$(COMPILER_NAME))
LIB := dll.a
else
LIB := lib
endif
EXP := exp

# executable:
EXE := .exe

# static library extention:
ifeq (dmc,$(COMPILER_NAME))
ARCH := lib
AR := dm_lib -n
AR_INS_R := -c
AR_EXTR := -x
AR_OUT = $(subst /,\,$@)
else
ifeq (bcc,$(COMPILER_NAME))
ARCH := lib
AR := tlib
AR_INS_R := +
AR_EXTR := *
AR_OUT = $(subst /,\,$@)
else
ARCH := a
AR := ar
AR_INS_R := -rs
AR_EXTR := -x
AR_OUT = $@
endif
endif
