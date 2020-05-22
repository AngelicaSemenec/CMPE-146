#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source;C:/ti/simplelink_msp432p4_sdk_3_40_00_05/kernel/tirtos/packages
override XDCROOT = C:/ti/ccs930/xdctools_3_60_02_34_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source;C:/ti/simplelink_msp432p4_sdk_3_40_00_05/kernel/tirtos/packages;C:/ti/ccs930/xdctools_3_60_02_34_core/packages;..
HOSTOS = Windows
endif
