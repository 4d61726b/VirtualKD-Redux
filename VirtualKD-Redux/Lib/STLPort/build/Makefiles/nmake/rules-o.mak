# -*- Makefile -*- Time-stamp: <03/10/29 21:05:34 ptr>
# $Id: rules-o.mak,v 1.2 2008-02-26 14:59:42 bazis Exp $

# Rules for release output:

.cpp{$(OUTPUT_DIR)}.o:
	$(COMPILE_cc_REL) $(OUTPUT_OPTION) $<

{..}.cpp{$(OUTPUT_DIR)}.o:
	$(COMPILE_cc_REL) $(OUTPUT_OPTION) $<

.cc{$(OUTPUT_DIR)}.o:
	$(COMPILE_cc_REL) $(OUTPUT_OPTION) $<

{..}.cc{$(OUTPUT_DIR)}.o:
	$(COMPILE_cc_REL) $(OUTPUT_OPTION) $<

.c{$(OUTPUT_DIR)}.o:
	$(COMPILE_c_REL) $(OUTPUT_OPTION) $<

{..}.c{$(OUTPUT_DIR)}.o:
	$(COMPILE_c_REL) $(OUTPUT_OPTION) $<

.rc{$(OUTPUT_DIR)}.res:
	$(COMPILE_rc_REL) $(RC_OUTPUT_OPTION) $<

{..}.rc{$(OUTPUT_DIR)}.res:
	$(COMPILE_rc_REL) $(RC_OUTPUT_OPTION) $<

!if "$(OUTPUT_DIR_A)" != "$(OUTPUT_DIR)"
# Rules for release output (static):

.cpp{$(OUTPUT_DIR_A)}.o:
	$(COMPILE_cc_STATIC_REL) $(OUTPUT_OPTION) $<

{..}.cpp{$(OUTPUT_DIR_A)}.o:
	$(COMPILE_cc_STATIC_REL) $(OUTPUT_OPTION) $<

.cc{$(OUTPUT_DIR_A)}.o:
	$(COMPILE_cc_STATIC_REL) $(OUTPUT_OPTION) $<

{..}.cc{$(OUTPUT_DIR_A)}.o:
	$(COMPILE_cc_STATIC_REL) $(OUTPUT_OPTION) $<

.c{$(OUTPUT_DIR_A)}.o:
	$(COMPILE_c_STATIC_REL) $(OUTPUT_OPTION) $<

{..}.c{$(OUTPUT_DIR_A)}.o:
	$(COMPILE_c_STATIC_REL) $(OUTPUT_OPTION) $<

.rc{$(OUTPUT_DIR_A)}.res:
	$(COMPILE_rc_STATIC_REL) $(RC_OUTPUT_OPTION) $<

{..}.rc{$(OUTPUT_DIR_A)}.res:
	$(COMPILE_rc_STATIC_REL) $(RC_OUTPUT_OPTION) $<

!endif

# Rules for debug output:

.cpp{$(OUTPUT_DIR_DBG)}.o:
	$(COMPILE_cc_DBG) $(OUTPUT_OPTION_DBG) $<

{..}.cpp{$(OUTPUT_DIR_DBG)}.o:
	$(COMPILE_cc_DBG) $(OUTPUT_OPTION_DBG) $<

.cc{$(OUTPUT_DIR_DBG)}.o:
	$(COMPILE_cc_DBG) $(OUTPUT_OPTION_DBG) $<

{..}.cc{$(OUTPUT_DIR_DBG)}.o:
	$(COMPILE_cc_DBG) $(OUTPUT_OPTION_DBG) $<

.c{$(OUTPUT_DIR_DBG)}.o:
	$(COMPILE_c_DBG) $(OUTPUT_OPTION_DBG) $<

{..}.c{$(OUTPUT_DIR_DBG)}.o:
	$(COMPILE_c_DBG) $(OUTPUT_OPTION_DBG) $<

.rc{$(OUTPUT_DIR_DBG)}.res:
	$(COMPILE_rc_DBG) $(RC_OUTPUT_OPTION_DBG) $<

{..}.rc{$(OUTPUT_DIR_DBG)}.res:
	$(COMPILE_rc_DBG) $(RC_OUTPUT_OPTION_DBG) $<

!if "$(OUTPUT_DIR_A_DBG)" != "$(OUTPUT_DIR_DBG)"
# Rules for debug output (static):

.cpp{$(OUTPUT_DIR_A_DBG)}.o:
	$(COMPILE_cc_STATIC_DBG) $(OUTPUT_OPTION_STATIC_DBG) $<

{..}.cpp{$(OUTPUT_DIR_A_DBG)}.o:
	$(COMPILE_cc_STATIC_DBG) $(OUTPUT_OPTION_STATIC_DBG) $<

.cc{$(OUTPUT_DIR_A_DBG)}.o:
	$(COMPILE_cc_STATIC_DBG) $(OUTPUT_OPTION_STATIC_DBG) $<

{..}.cc{$(OUTPUT_DIR_A_DBG)}.o:
	$(COMPILE_cc_STATIC_DBG) $(OUTPUT_OPTION_STATIC_DBG) $<

.c{$(OUTPUT_DIR_A_DBG)}.o:
	$(COMPILE_c_STATIC_DBG) $(OUTPUT_OPTION_STATIC_DBG) $<

{..}.c{$(OUTPUT_DIR_A_DBG)}.o:
	$(COMPILE_c_STATIC_DBG) $(OUTPUT_OPTION_STATIC_DBG) $<

.rc{$(OUTPUT_DIR_A_DBG)}.res:
	$(COMPILE_rc_STATIC_DBG) $(RC_OUTPUT_OPTION_STATIC_DBG) $<

{..}.rc{$(OUTPUT_DIR_A_DBG)}.res:
	$(COMPILE_rc_STATIC_DBG) $(RC_OUTPUT_OPTION_STATIC_DBG) $<

!endif

# Rules for STLport debug output:

.cpp{$(OUTPUT_DIR_STLDBG)}.o:
	$(COMPILE_cc_STLDBG) $(OUTPUT_OPTION_STLDBG) $<

{..}.cpp{$(OUTPUT_DIR_STLDBG)}.o:
	$(COMPILE_cc_STLDBG) $(OUTPUT_OPTION_STLDBG) $<

.cc{$(OUTPUT_DIR_STLDBG)}.o:
	$(COMPILE_cc_STLDBG) $(OUTPUT_OPTION_STLDBG) $<

{..}.cc{$(OUTPUT_DIR_STLDBG)}.o:
	$(COMPILE_cc_STLDBG) $(OUTPUT_OPTION_STLDBG) $<

.c{$(OUTPUT_DIR_STLDBG)}.o:
	$(COMPILE_c_STLDBG) $(OUTPUT_OPTION_STLDBG) $<

{..}.c{$(OUTPUT_DIR_STLDBG)}.o:
	$(COMPILE_c_STLDBG) $(OUTPUT_OPTION_STLDBG) $<

.rc{$(OUTPUT_DIR_STLDBG)}.res:
	$(COMPILE_rc_STLDBG) $(RC_OUTPUT_OPTION_STLDBG) $<

{..}.rc{$(OUTPUT_DIR_STLDBG)}.res:
	$(COMPILE_rc_STLDBG) $(RC_OUTPUT_OPTION_STLDBG) $<

!if "$(OUTPUT_DIR_A_STLDBG)" != "$(OUTPUT_DIR_STLDBG)"
# Rules for STLport debug output (static):

.cpp{$(OUTPUT_DIR_A_STLDBG)}.o:
	$(COMPILE_cc_STATIC_STLDBG) $(OUTPUT_OPTION_STATIC_STLDBG) $<

{..}.cpp{$(OUTPUT_DIR_A_STLDBG)}.o:
	$(COMPILE_cc_STATIC_STLDBG) $(OUTPUT_OPTION_STATIC_STLDBG) $<

.cc{$(OUTPUT_DIR_A_STLDBG)}.o:
	$(COMPILE_cc_STATIC_STLDBG) $(OUTPUT_OPTION_STATIC_STLDBG) $<

{..}.cc{$(OUTPUT_DIR_A_STLDBG)}.o:
	$(COMPILE_cc_STATIC_STLDBG) $(OUTPUT_OPTION_STATIC_STLDBG) $<

.c{$(OUTPUT_DIR_A_STLDBG)}.o:
	$(COMPILE_c_STATIC_STLDBG) $(OUTPUT_OPTION_STATIC_STLDBG) $<

{..}.c{$(OUTPUT_DIR_A_STLDBG)}.o:
	$(COMPILE_c_STATIC_STLDBG) $(OUTPUT_OPTION_STATIC_STLDBG) $<

.rc{$(OUTPUT_DIR_A_STLDBG)}.res:
	$(COMPILE_rc_STATIC_STLDBG) $(RC_OUTPUT_OPTION_STATIC_STLDBG) $<

{..}.rc{$(OUTPUT_DIR_A_STLDBG)}.res:
	$(COMPILE_rc_STATIC_STLDBG) $(RC_OUTPUT_OPTION_STATIC_STLDBG) $<

!endif

!if EXIST( .\rules-o.mak )
# Rules like
#    {$(BOOST_TST_SRC)}.cpp{$(OUTPUT_DIR)}.o:
#            $(COMPILE_cc_REL) $(OUTPUT_OPTION) $<
# (this is due to bad path/string processing in bogus nmake)
!include .\rules-o.mak
!endif

