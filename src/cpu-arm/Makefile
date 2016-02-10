armobj:=asm-off.o

v5obj:= $(armobj)	\
	task-cpu.o	\
	cpu.o		\
	gic.o		\
	dbm-arm.o	\
	cache-vmsa.o	\
	vector5.o	\
        sem-cpu-c.o	\
        mut-cpu-c.o 

v7obj:=$(armobj)	\
	task-cpu.o	\
	cpu.o		\
	gic.o		\
	cache-vmsa.o	\
	vector7.o	\
        sem-cpu.o	\
        mut-cpu.o	

v7mobj:=$(armobj)	\
	task-cpum.o	\
	cpum.o		\
        sem-cpu.o	\
        mut-cpu.o	\
        cache-pmsa.o	\
	vector7m.o

VOBJ+=$(v$(AARCH)$(AARCHX)obj)

.PHONY:_cpu 
_cpu:include/asm-off.h
	
.PHONY:include/asm-off.h
include/asm-off.h:src/cpu-arm/asm-off.c
	$(GCC) $(CFLAGS) $(INCLUDE) -DASM_UTIL $(CONFIG) -o asm-off.s.text -S $<
	cat asm-off.s.text | grep define | sed -e 's/#//g' -e 's/[\t]*define/#define/g' > $@

