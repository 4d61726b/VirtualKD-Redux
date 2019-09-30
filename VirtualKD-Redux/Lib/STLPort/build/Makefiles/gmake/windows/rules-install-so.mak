# -*- makefile -*- Time-stamp: <03/07/15 18:26:22 ptr>
# $Id: rules-install-so.mak,v 1.2 2008-02-26 14:59:42 bazis Exp $

INSTALL_TAGS ?= install-shared

PHONY += install $(INSTALL_TAGS)

install:	$(INSTALL_TAGS)

install-release-shared: release-shared $(INSTALL_BIN_DIR) $(INSTALL_LIB_DIR)
	$(INSTALL_SO) $(subst /,\,$(SO_NAME_OUT)) $(subst /,\,$(INSTALL_BIN_DIR)/)
	$(INSTALL_SO) $(subst /,\,$(LIB_NAME_OUT)) $(subst /,\,$(INSTALL_LIB_DIR)/)

install-dbg-shared: dbg-shared $(INSTALL_BIN_DIR) $(INSTALL_LIB_DIR_DBG)
	$(INSTALL_SO) $(subst /,\,$(SO_NAME_OUT_DBG)) $(subst /,\,$(INSTALL_BIN_DIR)/)
	$(INSTALL_SO) $(subst /,\,$(LIB_NAME_OUT_DBG)) $(subst /,\,$(INSTALL_LIB_DIR_DBG)/)

install-stldbg-shared: stldbg-shared $(INSTALL_BIN_DIR) $(INSTALL_LIB_DIR_STLDBG)
	$(INSTALL_SO) $(subst /,\,$(SO_NAME_OUT_STLDBG)) $(subst /,\,$(INSTALL_BIN_DIR)/)
	$(INSTALL_SO) $(subst /,\,$(LIB_NAME_OUT_STLDBG)) $(subst /,\,$(INSTALL_LIB_DIR_STLDBG)/)

