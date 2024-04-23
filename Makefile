
all: sppc.dll

sppc.dll: sppc.c sppcplus64.res.o libsppcs64.a
	..\\bin\\gcc -Wl,-eDllMain -municode -Os -Wall $< sppc.def sppcplus64.res.o -shared -o $@ -fno-ident -L. -nostartfiles -nostdlib -lkernel32 -lshlwapi -ladvapi32 -Wl,--exclude-all-symbols,--enable-stdcall-fixup,--dynamicbase,--nxcompat,--subsystem,windows:6.0 -lsppcs64
	strip -s $@

sppcplus64.res.o: sppc.rc
	..\\bin\\windres --codepage=65001 $< $@

libsppcs64.a:
	..\\bin\\dlltool -k -dsppcs64.def -l libsppcs64.a
