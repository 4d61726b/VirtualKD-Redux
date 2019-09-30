# -*- makefile -*- Time-stamp: <03/11/30 11:41:22 ptr>
# $Id: bcc.mak,v 1.2 2008-02-26 14:59:41 bazis Exp $


release-shared : OPT += -tWD
dbg-shared : OPT += -tWD
stldbg-shared : OPT += -tWD

release-shared : LDFLAGS += -Tpd -V4.00 -w -w-dup
dbg-shared : LDFLAGS += -Tpd -V4.00 -v -w -w-dup
stldbg-shared : LDFLAGS += -Tpd -V4.00 -v -w -w-dup

START_OBJ := c0d32.obj

