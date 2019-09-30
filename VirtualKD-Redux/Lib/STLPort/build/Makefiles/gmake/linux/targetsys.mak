# Time-stamp: <05/09/09 20:59:42 ptr>
# $Id: targetsys.mak,v 1.2 2008-02-26 14:59:42 bazis Exp $

SO := so

ARCH := a
ifdef TARGET_OS
AR := ${TARGET_OS}-ar
else
AR := ar
endif
AR_INS_R := -rs
AR_EXTR := -x
AR_OUT = $@
