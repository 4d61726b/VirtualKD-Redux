# Time-stamp: <03/10/17 19:29:26 ptr>
# $Id: rules-install-a.mak,v 1.2 2008-02-26 14:59:43 bazis Exp $

install-static: install-release-static install-dbg-static install-stldbg-static

install-release-static: release-static $(INSTALL_STATIC_BIN_DIR)
	$(INSTALL_A) $(PRG_A) $(INSTALL_STATIC_BIN_DIR)
	@if exist $(A_PDB_NAME_OUT) $(INSTALL_A) $(A_PDB_NAME_OUT) $(INSTALL_STATIC_BIN_DIR)

install-dbg-static: dbg-static $(INSTALL_STATIC_BIN_DIR_DBG)
	$(INSTALL_A) $(PRG_A_DBG) $(INSTALL_STATIC_BIN_DIR_DBG)
	@if exist $(A_PDB_NAME_OUT_DBG) $(INSTALL_A) $(A_PDB_NAME_OUT_DBG) $(INSTALL_STATIC_BIN_DIR_DBG)

install-stldbg-static: stldbg-static $(INSTALL_STATIC_BIN_DIR_STLDBG)
	$(INSTALL_A) $(PRG_A_STLDBG) $(INSTALL_STATIC_BIN_DIR_STLDBG)
	@if exist $(A_PDB_NAME_OUT_STLDBG) $(INSTALL_A) $(A_PDB_NAME_OUT_STLDBG) $(INSTALL_STATIC_BIN_DIR_STLDBG)
