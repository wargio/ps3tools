TOOLS	 =	norunpack readself unpkg pkg scekrit spp
TOOLS	+=	unself2 cospkg eidsplitr puppack sceverify unself 
TOOLS	+=	cosunpack makeself pupunpack undat unspp cosunpkg 
TOOLS	+=	readself2 self_rebuilder ungpkg sfo hip2his syscon
TOOLS	+=	QGL
CPPTOOLS =	sfo_sfx_converter
BUILT   :=	$(TOOLS) $(CPPTOOLS)

COMMON	=	tools.o aes.o sha1.o ec.o bn.o syscon_m.o self.o
DEPS	=	Makefile tools.h types.h self.h common.h
OSNAME  =	$(shell uname -s)

ifeq ($(findstring MINGW, $(OSNAME)), MINGW)
COMMON  += mingw_mmap.o
endif

CC	=	gcc
CFLAGS	=	-g -O2 -Wall -W
LDLIBS  =	-lz

# Darwin's MacPorts Default Path
ifeq ($(shell test -e /opt/local/include/gmp.h; echo $$?),0)
CFLAGS  += -I/opt/local/include
LDLIBS  += -L/opt/local/lib
endif


OBJS	= $(COMMON) $(addsuffix .o, $(TOOLS))
OBJS2	= $(COMMON) $(addsuffix .o, $(CPPTOOLS))

all: $(TOOLS) sfo2sfx
	@[ -d ./BUILD ] || mkdir -p ./BUILD
	@for TOOL in $(BUILT); do mv $$TOOL ./BUILD/$$TOOL ; done 

$(TOOLS): %: %.o $(COMMON) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LDLIBS) 

scekrit: LDLIBS += -lgmp

$(OBJS): %.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f $(OBJS) $(TOOLS) $(CPPTOOLS).o $(CPPTOOLS) *.exe *.o
	-rm -rf ./BUILD

stuff_cpp:
	g++ $(CFLAGS) -c -o tools_cpp.o tools.cpp
	g++ $(CFLAGS) -c -o aes_cpp.o aes.c
	g++ $(CFLAGS) -c -o sha1_cpp.o sha1.c
	g++ $(CFLAGS) -c -o ec_cpp.o ec.c
	g++ $(CFLAGS) -c -o bn_cpp.o bn.c

sfo2sfx: stuff_cpp
	g++ $(CFLAGS) -o $(CPPTOOLS) $(CPPTOOLS).cpp tools_cpp.o aes_cpp.o sha1_cpp.o ec_cpp.o bn_cpp.o $(LDLIBS) 

