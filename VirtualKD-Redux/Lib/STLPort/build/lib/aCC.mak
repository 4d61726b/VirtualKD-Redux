# -*- Makefile -*- Time-stamp: <03/10/12 20:35:49 ptr>
# $Id: aCC.mak,v 1.2 2008-02-26 14:59:43 bazis Exp $

SRCROOT := ..
COMPILER_NAME := aCC

STLPORT_INCLUDE_DIR = ../../stlport
include Makefile.inc
include ${SRCROOT}/Makefiles/top.mak


INCLUDES += -I$(STLPORT_INCLUDE_DIR)

# options for build with boost support
ifdef STLP_BUILD_BOOST_PATH
INCLUDES += -I$(STLP_BUILD_BOOST_PATH)
endif

