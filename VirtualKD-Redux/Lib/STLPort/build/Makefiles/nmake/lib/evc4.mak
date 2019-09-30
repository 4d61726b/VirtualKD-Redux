# -*- makefile -*- Time-stamp: <04/03/31 08:10:19 ptr>
# $Id: evc4.mak,v 1.2 2008-02-26 14:59:43 bazis Exp $

LDFLAGS_COMMON = $(LDFLAGS_COMMON) /base:"0x00100000"

!include evc-common.mak
