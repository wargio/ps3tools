TOOLS    =	norunpack readself unpkg pkg scekrit spp
TOOLS   +=	unself2 cospkg eidsplitr puppack sceverify unself 
TOOLS   +=	cosunpack makeself pupunpack undat unspp cosunpkg 
TOOLS   +=	readself2 self_rebuilder ungpkg sfo hip2his syscon
TOOLS   +=	QGL sfo_sfx_converter
BUILT   :=	$(TOOLS)
BUILDFLD =	BUILD

COMMON  =	tools.o aes.o sha1.o ec.o bn.o syscon_m.o self.o
DEPS    =	Makefile tools.h types.h self.h common.h
OSNAME  =	$(shell uname -s)

ifeq ($(findstring MINGW, $(OSNAME)), MINGW)
COMMON  += mingw_mmap.o
endif

CC      =	gcc
CFLAGS  =	-g -O2 -Wall -W
LDLIBS  =	-lz

# Darwin's MacPorts Default Path
ifeq ($(shell test -e /opt/local/include/gmp.h; echo $$?),0)
CFLAGS  += -I/opt/local/include
LDLIBS  += -L/opt/local/lib
endif


OBJS	= $(COMMON) $(addsuffix .o, $(TOOLS))
OBJS2	= $(COMMON) $(addsuffix .o, $(CPPTOOLS))

all: $(TOOLS)
	@[ -d $(BUILDFLD) ] || mkdir -p $(BUILDFLD)
	@for TOOL in $(BUILT); do mv $$TOOL $(BUILDFLD)/$$TOOL ; done 
	@cp *.sh $(BUILDFLD)

$(TOOLS): %: %.o $(COMMON) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON) $(LDLIBS) 

scekrit: LDLIBS += -lgmp

$(OBJS): %.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f $(OBJS) $(TOOLS) *.exe *.o
	-rm -rf $(BUILDFLD)

