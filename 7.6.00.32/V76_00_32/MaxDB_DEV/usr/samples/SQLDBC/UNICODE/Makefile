CPPSOURCE=simpleUCS2

SDKINC    = -I$(SQLDBCSDK)/sdk/sqldbc/incl
CPPLIBS   = -L$(SQLDBCSDK)/lib -lSQLDBC
CPPLIBSMS = $(SQLDBCSDK)/sdk/sqldbc/lib/libSQLDBC.lib
CLIBSMS   = $(SQLDBCSDK)/sdk/sqldbc/lib/libSQLDBC_C.lib
CPPLIBS64 = -L$(SQLDBCSDK)/lib/lib64 -lSQLDBC
CLIBS     = -L$(SQLDBCSDK)/lib -lSQLDBC_C
CLIBS64   = -L$(SQLDBCSDK)/lib/lib64 -lSQLDBC_C

win32:
	$(MAKE) CC=cl  CPPFLAGS="-DWIN32 $(SDKINC)" CPPLIBS="$(CPPLIBSMS)" $(CPPSOURCE)

amd64:
	$(MAKE) CC=cl  CPPFLAGS="-DWIN32 $(SDKINC)" CPPLIBS="$(CPPLIBSMS) bufferoverflowu.lib" $(CPPSOURCE)

linux:
	$(MAKE) CC=c++ CPPFLAGS="$(SDKINC)" $(CPPSOURCE)

linuxppc:
	$(MAKE) CC=c++ CPPFLAGS="-m64 $(SDKINC)" $(CPPSOURCE)

sun:
	$(MAKE) CC=CC  CPPFLAGS="-xarch=v9 $(SDKINC)" CPPLIBS="$(CPPLIBS64)" $(CPPSOURCE)

sunx86:
	$(MAKE) CC=CC  CPPFLAGS="$(SDKINC)" CPPLIBS="$(CPPLIBS)" $(CPPSOURCE)

sunx86_64:
	$(MAKE) CC=CC  CPPFLAGS="-xarch=amd64 $(SDKINC)" CPPLIBS="$(CPPLIBS64)" $(CPPSOURCE)

hpux:
	$(MAKE) CC=aCC CPPFLAGS="+DA2.0W +DS2.0 $(SDKINC)" CPPLIBS="$(CPPLIBS64)" $(CPPSOURCE)

hpIa64:
	$(MAKE) CC=aCC CPPFLAGS="+DD64 $(SDKINC)" $(CPPSOURCE)

aix:
	$(MAKE) CC=xlC CPPFLAGS="-q64 $(SDKINC)" CPPLIBS="-brtl $(CPPLIBS64)" $(CPPSOURCE)

dec:
	$(MAKE) CC=cxx CPPFLAGS="-distinguish_nested_enums $(SDKINC)" $(CPPSOURCE)

$(CPPSOURCE):
	$(CC) $(CPPFLAGS) $@.cpp $(CPPLIBS) -o $@

clean:
	-rm -f $(CPPSOURCE) $(CPPSOURCE).o
