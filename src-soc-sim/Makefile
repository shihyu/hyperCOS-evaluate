NAME   :=soc-sim
TARGET :=arm-none-eabi
CROSS  :=$(TARGET)-
PREFIX ?=$(shell pwd)/../../prefix/$(CROSS:%-=%)#TEMPLATE
SOC    ?=soc-sim
CPU    :=arm
INCLUDE:=-Iinclude -I$(SOC) -I$(PREFIX)/include
COPTS  ?=-march=armv7-a -mthumb
AARCH  :=$(shell echo $(COPTS) | sed -e 's/.*armv\([0-9]\).*/\1/g')
AARCHX :=
ifeq ($(shell echo $(COPTS) | sed -e 's/.*armv[0-9]-\([amr]\).*/\1/g'),m)
AARCHX :=m
endif
MOPTS  :=$(COPTS) \
	-DCFG_AARCH=$(AARCH) \
	-fno-builtin -fno-common \
	-ffunction-sections -fdata-sections -fshort-enums
CONFIG ?=
ASFLAGS:=$(MOPTS) $(CONFIG) -O2 -g -Wall -Werror -D __ASSEMBLY__
CFLAGS :=$(MOPTS) $(CONFIG) -O2 -g -Wall -Werror
LSCRIPT:=rom.ld
LDFLAGS:=$(MOPTS) -g -nostartfiles -nodefaultlibs -L $(PREFIX)/lib -T$(LSCRIPT)
MSCRIPT:=$(PREFIX)/share/mod.ld
LIB    :=lib$(NAME).a

ALL    :=lib
CLEAN  :=
CPU    :=arm

VPATH  :=.
VOBJ   :=$(patsubst %.S,%.o, \
		$(patsubst %.c,%.o, \
		$(patsubst %.cpp, %.o, \
			$(notdir $(foreach DIR,$(VPATH),\
				$(wildcard $(DIR)/*.S)	\
				$(wildcard $(DIR)/*.c) 	\
				$(wildcard $(DIR)/*.cpp))))))
default:all

include $(PREFIX)/share/Makefile.rule
