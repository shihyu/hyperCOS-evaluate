NAME   :=hcos
TARGET :=arm-none-eabi
CROSS  :=$(TARGET)-
PREFIX :=$(shell pwd)/../prefix/$(CROSS:%-=%)
CPU    :=arm
INCLUDE:=-Iinclude 

# -march=armv5t -msoft-float
# -march=armv5t -mthumb -msoft-float
# -march=armv7-a -mthumb -msoft-float
# -march=armv7-m -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
COPTS  ?=-march=armv5t
AARCH  :=$(shell echo $(COPTS) | sed -e 's/.*armv\([0-9]\).*/\1/g')
AARCHX :=
ifeq ($(shell echo $(COPTS) | sed -e 's/.*armv[0-9]-\([amr]\).*/\1/g'),m)
AARCHX :=m
endif
MOPTS  :=$(COPTS) \
	-DCFG_AARCH=$(AARCH) \
	-fno-builtin -fno-common \
	-ffunction-sections -fdata-sections 
CONFIG ?=
ASFLAGS:=$(MOPTS) $(CONFIG) -O2 -g -Wall -Werror -D __ASSEMBLY__
CFLAGS :=$(MOPTS) $(CONFIG) -O2 -g -Wall -Werror
LDFLAGS:=$(MOPTS)
MSCRIPT:=share/mod.ld
LIB    :=lib$(NAME).a

ALL    :=include/cpu \
	include/mcfg.h \
	_cpu \
	_lib \
	_header \
	install
CLEAN  :=_clean

VPATH  :=src src-test src/cpu-$(CPU) 
VOBJ   :=$(patsubst %.S,%.o, \
		$(patsubst %.c,%.o, \
			$(patsubst %.cpp, %.o, \
				$(notdir $(foreach DIR,src src-test,\
					$(wildcard $(DIR)/*.S)	\
					$(wildcard $(DIR)/*.c) 	\
					$(wildcard $(DIR)/*.cpp))))))
default:all

include src/cpu-$(CPU)/Makefile
include share/Makefile.rule

.PHONY:include/cpu
include/cpu:
	rm -f $@
	ln -sf cpu-$(CPU)$(AARCHX) $@

.PHONY:include/mcfg.h
include/mcfg.h:
	bin/m-cfg.sh $(CONFIG) > $@

_lib:
	$(MAKE) lib

_header:
	install -d $(PREFIX)/include/hcos/cpu
	install -Dp include/cpu-$(CPU)$(AARCHX)/*.h $(PREFIX)/include/$(NAME)/cpu
	install -Dp include/cfg.h $(PREFIX)/include/hcos/cpu

_clean:
	rm -f src-test/*loadable*-src.c

src-test/t-mod-loadable-src.c:t-mod-loadable.o.mod.img
	echo "char _mod_src[] = {" >$@
	xxd -g 1 $^ | sed -e 's/  .*//g' -e 's/.*://g' -e 's/^ /0x/g' -e 's/ /,0x/g' -e 's/$$/,/g' >>$@
	echo "}; " >>$@
	echo "unsigned _mod_src_sz = sizeof(_mod_src); " >>$@
	
src-test/t-mod-loadable2-src.c:t-mod-loadable2.o.mod.img
	echo "char _mod_src2[] = {" >$@
	xxd -g 1 $^ | sed -e 's/  .*//g' -e 's/.*://g' -e 's/^ /0x/g' -e 's/ /,0x/g' -e 's/$$/,/g' >>$@
	echo "}; " >>$@
	echo "unsigned _mod_src2_sz = sizeof(_mod_src2); " >>$@
	
%.sub:
	cd src-$(@:%.sub=%) && $(MAKE) && $(MAKE) install

%.subc:
	cd src-$(@:%.subc=%) && $(MAKE) clean

copyright:
	$(PREFIX)/bin/m-copyright.sh .

fmt:
	bin/m-fmt.sh .
	
utest: all
	cd src-hello && \
	$(MAKE) clean && \
	$(MAKE) CONFIG=-DTEST=1 && \
	$(MAKE) run QM="main.elf.pack" QF="-serial stdio -serial null"

hello: all
	cd src-hello && \
	$(MAKE) clean && \
	$(MAKE) CONFIG=-DTEST=0 && \
	$(MAKE) run QM="main.elf.pack" QF="-serial stdio -serial null"

gdbs: all
	cd src-hello && \
	$(MAKE) clean && \
	$(MAKE) tstgdb.elf && \
	$(MAKE) run QM="tstgdb.elf" QF="-serial stdio -serial tcp::8888,server"

gdbc:
	cd src-hello && \
	$(MAKE) gdb QM="tstgdb.elf"

