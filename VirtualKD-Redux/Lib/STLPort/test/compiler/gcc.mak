# -*- Makefile -*- Time-stamp: <04/03/14 23:50:57 ptr>
# $Id: gcc.mak,v 1.2 2008-02-26 14:59:44 bazis Exp $

SRCROOT := ../../src/explore
COMPILER_NAME := gcc

ALL_TAGS := compile-only
STLPORT_DIR := ../..
include Makefile.inc
include ${SRCROOT}/Makefiles/top.mak
compile-only:	$(OBJ)
