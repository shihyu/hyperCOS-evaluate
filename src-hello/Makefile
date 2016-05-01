NAME   :=hello
TARGET :=arm-none-eabi
CROSS  :=$(TARGET)-
PREFIX ?=$(shell pwd)/../../prefix/$(CROSS:%-=%)#TEMPLATE
SOC    ?=soc-sim
CPU    :=arm
INCLUDE:=-Iinclude -I$(PREFIX)/include/$(SOC) -I$(PREFIX)/include
COPTS  ?=-march=armv7-a -mthumb
AARCH  :=$(shell echo $(COPTS) | sed -e 's/.*armv\([0-9]\).*/\1/g')
MOPTS  :=$(COPTS) \
	-DCFG_AARCH=$(AARCH) \
	-fno-builtin -fno-common \
	-ffunction-sections -fdata-sections -fshort-enums
CONFIG ?=
ASFLAGS:=$(MOPTS) $(CONFIG) -O2 -g -Wall -Werror -D __ASSEMBLY__
CFLAGS :=$(MOPTS) $(CONFIG) -O2 -g -Wall -Werror
LSCRIPT:=rom.ld
LDFLAGS:=$(MOPTS) -g -nostartfiles -nodefaultlibs -L $(PREFIX)/lib -T$(LSCRIPT) dbg.o
MSCRIPT:=$(PREFIX)/share/mod.ld
LIB    :=lib$(NAME).a

ALL    :=startup.o main.elf
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

QF?=-serial stdio -serial null
sim:
	qemu-system-arm -display none -M gbb $(QF) -kernel $(QM) -gdb tcp::16888

sim-dbg:
	qemu-system-arm -display none -M gbb $(QF) -kernel $(QM) -S -gdb tcp::16888

ddd:
	ddd --debugger $(CROSS)gdb -x qemu.gdb $(QM)

gdb:
	echo "target remote 127.0.0.1:8888" > dbm.gdb
	$(CROSS)gdb -x dbm.gdb $(QM)
