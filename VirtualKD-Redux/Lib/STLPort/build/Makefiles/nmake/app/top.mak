# -*- makefile -*- Time-stamp: <03/09/28 18:46:10 ptr>
# $Id: top.mak,v 1.2 2008-02-26 14:59:43 bazis Exp $

!ifndef LDFLAGS
LDFLAGS = 
!endif

!include $(RULESBASE)/$(USE_MAKE)/app/macro.mak
!include $(RULESBASE)/$(USE_MAKE)/app/$(COMPILER_NAME).mak
!include $(RULESBASE)/$(USE_MAKE)/app/rules-exe-$(OSNAME).mak

!include $(RULESBASE)/$(USE_MAKE)/app/rules-install-so.mak
!include $(RULESBASE)/$(USE_MAKE)/app/rules-install-a.mak

!include $(RULESBASE)/$(USE_MAKE)/app/clean.mak
