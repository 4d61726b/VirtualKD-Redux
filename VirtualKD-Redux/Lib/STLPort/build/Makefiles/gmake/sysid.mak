# Time-stamp: <05/12/15 01:46:03 ptr>

ifndef BUILD_DATE

ifeq (mwccnlm,$(COMPILER_NAME))
# this is really cross
TARGET_OS := netware
endif

ifndef TARGET_OS

OSNAME := $(shell uname -s | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')

# RedHat use nonstandard options for uname at least in cygwin,
# macro should be overwritten:
ifeq (cygwin,$(findstring cygwin,$(OSNAME)))
OSNAME    := cygming
OSREALNAME := $(shell uname -o | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
endif

ifeq (mingw,$(findstring mingw,$(OSNAME)))
OSNAME    := cygming
OSREALNAME := mingw
endif

OSREL  := $(shell uname -r | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
M_ARCH := $(shell uname -m | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
ifeq ($(OSNAME),hp-ux)
P_ARCH := unknown
else
P_ARCH := $(shell uname -p | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
endif

else

# TARGET_OS defined, cross compilation assumed
ifndef (mwccnlm,$(COMPILER_NAME))
OSNAME := $(shell echo ${TARGET_OS} | sed 's/^[a-z0-9_]\+-[a-z0-9]\+-\([a-z]\+\).*/\1/' | sed 's/^[a-z0-9_]\+-\([a-z]\+\).*/\1/' )
OSREL  := $(shell echo ${TARGET_OS} | sed 's/^[[:alnum:]_]\+-[a-z0-9]\+-[a-z]\+\([a-zA-Z.0-9]*\).*/\1/' | sed 's/^[a-z0-9_]\+-[a-z]\+\([a-zA-Z.0-9]*\).*/\1/' )
M_ARCH := $(shell echo ${TARGET_OS} | sed 's/^\([a-z0-9_]\+\)-.*/\1/' )
P_ARCH := unknown
else
OSNAME := netware
OSREL  := 5
M_ARCH := i386
P_ARCH := unknown
endif

# TARGET_OS
endif

NODENAME := $(shell uname -n | tr '[A-Z]' '[a-z]' )
SYSVER := $(shell uname -v )
USER := $(shell echo $$USER )

ifeq ($(OSNAME),freebsd)
OSREL_MAJOR := $(shell echo ${OSREL} | tr '.-' ' ' | awk '{print $$1;}')
OSREL_MINOR := $(shell echo ${OSREL} | tr '.-' ' ' | awk '{print $$2;}')
endif

# OS_VER := $(shell uname -s | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',_')

BUILD_SYSTEM := $(shell echo `uname -n` `uname -s` `uname -r` `uname -v` `uname -m` $$USER)
BUILD_DATE := $(shell date +'%Y/%m/%d %T %Z')

BUILD_OSNAME := $(shell uname -s | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')

# RedHat use nonstandard options for uname at least in cygwin,
# macro should be overwritten:
ifeq (cygwin,$(findstring cygwin,$(BUILD_OSNAME)))
BUILD_OSNAME    := cygming
BUILD_OSREALNAME := $(shell uname -o | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
endif

ifeq (mingw,$(findstring mingw,$(BUILD_OSNAME)))
BUILD_OSNAME    := cygming
BUILD_OSREALNAME := mingw
endif

BUILD_OSREL  := $(shell uname -r | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
BUILD_M_ARCH := $(shell uname -m | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
ifeq ($(OSNAME),hp-ux)
BUILD_P_ARCH := unknown
else
BUILD_P_ARCH := $(shell uname -p | tr '[A-Z]' '[a-z]' | tr ', /\\()"' ',//////' | tr ',/' ',-')
endif

# end of BUILD_DATE not defined
endif
