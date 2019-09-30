# -*- Makefile -*- Time-stamp: <03/10/27 18:12:46 ptr>
# $Id: rules-res.mak,v 1.2 2008-02-26 14:59:41 bazis Exp $

# Rules for release output:

$(OUTPUT_DIR)/%.res:	$(WORD1)%.rc
	$(COMPILE.rc) $(RC_OUTPUT_OPTION) $<

# Rules for debug output:

$(OUTPUT_DIR_DBG)/%.res:	$(WORD1)%.rc
	$(COMPILE.rc) $(RC_OUTPUT_OPTION) $<

# Rules for STLport debug output:

$(OUTPUT_DIR_STLDBG)/%.res:	$(WORD1)%.rc
	$(COMPILE.rc) $(RC_OUTPUT_OPTION) $<


