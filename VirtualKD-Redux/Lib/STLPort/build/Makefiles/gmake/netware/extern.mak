# Time-stamp: <05/06/03 21:34:35 ptr>
# $Id: extern.mak,v 1.2 2008-02-26 14:59:42 bazis Exp $

MWCW_BASE ?= c:/Program Files/Metrowerks/CodeWarrior
NWSDK_DIR ?= c:/Novell/ndk/nwsdk

STLPORT_LIB_DIR ?= $(STLPORT_DIR)/${TARGET_NAME}lib
STLPORT_INCLUDE_DIR ?= $(STLPORT_DIR)/stlport

MWCW_NOVELL = $(MWCW_BASE)/Novell Support/Metrowerks Support
MWCW_NOVELL_SDK = $(NWSDK_DIR)

# boost (http://www.boost.org, http://boost.sourceforge.net)
BOOST_INCLUDE_DIR ?= ${BOOST_DIR}

