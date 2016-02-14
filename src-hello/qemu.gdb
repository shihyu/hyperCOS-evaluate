define bs
   set logging file brestore.text
   set logging on
   info break
   set logging off
   # reformat on-the-fly to a valid gdb command file
   shell perl -n -e 'print "break $1\n" if /^\d+.+?(\S+)$/g' brestore.text|uniq > bs.gdb
   shell rm -f brestore.text
end 
define T
	p ($cpsr>>5)&1
end
define I
	p ($cpsr>>7)&1
end
define M
	p ($cpsr&0x1f)
end

define adump
	set $pc=$r0
	p $r1
end

target remote 127.0.0.1:16888
source bs.gdb
