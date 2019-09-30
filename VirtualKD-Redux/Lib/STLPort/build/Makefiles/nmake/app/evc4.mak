# -*- makefile -*- Time-stamp: <04/03/31 08:08:12 ptr>
# $Id: evc4.mak,v 1.2 2008-02-26 14:59:43 bazis Exp $

LDFLAGS_COMMON = $(LDFLAGS_COMMON) /base:"0x00010000"

!include evc-common.mak
