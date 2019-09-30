# -*- makefile -*- Time-stamp: <03/10/29 22:20:01 ptr>
# $Id: clean.mak,v 1.2 2008-02-26 14:59:42 bazis Exp $

clean:
	@if exist $(OUTPUT_DIR)\*.o del /F /Q $(OUTPUT_DIR)\*.o
	@if exist $(OUTPUT_DIR_DBG)\*.o del /F /Q $(OUTPUT_DIR_DBG)\*.o
	@if exist $(OUTPUT_DIR_STLDBG)\*.o del /F /Q $(OUTPUT_DIR_STLDBG)\*.o
	@if exist $(OUTPUT_DIR_A)\*.o del /F /Q $(OUTPUT_DIR_A)\*.o
	@if exist $(OUTPUT_DIR_A_DBG)\*.o del /F /Q $(OUTPUT_DIR_A_DBG)\*.o
	@if exist $(OUTPUT_DIR_A_STLDBG)\*.o del /F /Q $(OUTPUT_DIR_A_STLDBG)\*.o
	@if exist $(OUTPUT_DIR)\*.obj del /F /Q $(OUTPUT_DIR)\*.obj
	@if exist $(OUTPUT_DIR_DBG)\*.obj del /F /Q $(OUTPUT_DIR_DBG)\*.obj
	@if exist $(OUTPUT_DIR_STLDBG)\*.obj del /F /Q $(OUTPUT_DIR_STLDBG)\*.obj
	@if exist $(OUTPUT_DIR_A)\*.obj del /F /Q $(OUTPUT_DIR_A)\*.obj
	@if exist $(OUTPUT_DIR_A_DBG)\*.obj del /F /Q $(OUTPUT_DIR_A_DBG)\*.obj
	@if exist $(OUTPUT_DIR_A_STLDBG)\*.obj del /F /Q $(OUTPUT_DIR_A_STLDBG)\*.obj
