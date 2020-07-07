CPPSOURCE=HelloWorld
CSOURCE=HelloWorld_C

SDKINCL  = -I$(SQLDBCSDK)/sdk/sqldbc/incl
CPPLIBS   = -L$(SQLDBCSDK)/lib -lSQLDBC
CPPLIBSMS = $(SQLDBCSDK)/sdk/sqldbc/lib/libSQLDBC.lib
CLIBSMS   = $(SQLDBCSDK)/sdk/sqldbc/lib/libSQLDBC_C.lib
CPPLIBS64 = -L$(SQLDBCSDK)/lib/lib64 -lSQLDBC
CLIBS     = -L$(SQLDBCSDK)/lib -lSQLDBC_C
CLIBS64   = -L$(SQLDBCSDK)/lib/lib64 -lSQLDBC_C

win32:
	$(MAKE) CC=cl  CPPFLAGS="-DWIN32 $(SDKINCL)" CPPLIBS="$(CPPLIBSMS)" $(CPPSOURCE)
	$(MAKE) CC=cl  CFLAGS="-DWIN32 $(SDKINCL)" CLIBS="$(CLIBSMS)" $(CSOURCE)

amd64:
	$(MAKE) CC=cl  CPPFLAGS="-DWIN32 $(SDKINCL)" CPPLIBS="$(CPPLIBSMS) bufferoverflowu.lib" $(CPPSOURCE)
	$(MAKE) CC=cl  CFLAGS="-DWIN32 $(SDKINCL)"   CLIBS="$(CLIBSMS) bufferoverflowu.lib" $(CSOURCE)

linux:
	$(MAKE) CC=c++ CPPFLAGS="$(SDKINCL)" $(CPPSOURCE)
	$(MAKE) CC=cc  CFLAGS="$(SDKINCL)" $(CSOURCE)

linuxppc:
	$(MAKE) CC=c++ CPPFLAGS="-m64 $(SDKINCL)" $(CPPSOURCE)
	$(MAKE) CC=cc  CFLAGS="-m64 -Wl,-melf64ppc $(SDKINCL)" $(CSOURCE)

sun:
	$(MAKE) CC=CC  CPPFLAGS="-xarch=v9 $(SDKINCL)" CPPLIBS="$(CPPLIBS64)" $(CPPSOURCE)
	$(MAKE) CC=cc  CFLAGS="-xarch=v9 $(SDKINCL)" CLIBS="$(CLIBS64) -lCrun -lm" $(CSOURCE)

sunx86:
	$(MAKE) CC=CC  CPPFLAGS="$(SDKINCL)" CPPLIBS="$(CPPLIBS)" $(CPPSOURCE)
	$(MAKE) CC=cc  CFLAGS="$(SDKINCL)" CLIBS="$(CLIBS) -lCrun -lm" $(CSOURCE)

sunx86_64:
	$(MAKE) CC=CC  CPPFLAGS="-xarch=amd64 $(SDKINCL)" CPPLIBS="$(CPPLIBS64)" $(CPPSOURCE)
	$(MAKE) CC=cc  CFLAGS="-xarch=amd64 $(SDKINCL)" CLIBS="$(CLIBS64) -lCrun -lm" $(CSOURCE)

hpux:
	$(MAKE) CC=aCC CPPFLAGS="+DA2.0W +DS2.0 $(SDKINCL)" CPPLIBS="$(CPPLIBS64)" $(CPPSOURCE)
	$(MAKE) CC=cc  CFLAGS="+DA2.0W +DS2.0 $(SDKINCL)" CLIBS="$(CLIBS64)  -lCsup -lm -lcl" $(CSOURCE)

hpIa64:
	$(MAKE) CC=aCC CPPFLAGS="+DD64 $(SDKINCL)" $(CPPSOURCE)
	$(MAKE) CC=cc  CFLAGS="+DD64 $(SDKINCL)" CLIBS="$(CLIBS) -lCsup -lm -lcl -lpthread" $(CSOURCE)

aix:
	$(MAKE) CC=xlC CPPFLAGS="-q64 $(SDKINCL)" CPPLIBS="-brtl $(CPPLIBS64)" $(CPPSOURCE)
	$(MAKE) CC=xlc CFLAGS="-q64 -qcpluscmt $(SDKINCL)" CLIBS="-brtl $(CLIBS64)" $(CSOURCE)

dec:
	$(MAKE) CC=cxx CPPFLAGS="-distinguish_nested_enums $(SDKINCL)" $(CPPSOURCE)
	$(MAKE) CC=cc CFLAGS="$(SDKINCL)" $(CSOURCE)

$(CPPSOURCE):
	$(CC) $(CPPFLAGS) $@.cpp $(CPPLIBS) -o $@

$(CSOURCE):
	$(CC) $(CFLAGS) $@.c $(CLIBS) -o $@

clean:
	-rm -f $(CPPSOURCE) $(CPPSOURCE).o
	-rm -f $(CSOURCE) $(CSOURCE).o
