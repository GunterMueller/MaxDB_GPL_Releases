#  @(#)make_w32.mak		2.0.5  1996-09-03  SQL DBS
#


all: libs csys sql

libs:
	cd libpc  && $(MAKE) -f make_w32.mak -k install
	@echo -------------------------------------------------

csys:
	cd pcomp  && $(MAKE) -f make_w32.mak -k install
	@echo -------------------------------------------------
	cd ptoc   && $(MAKE) -f make_w32.mak -k install
	@echo -------------------------------------------------

sql:
	cd sqlsrc && $(MAKE) -f make_w32.mak -k install
	@echo -------------------------------------------------

clean:
	cd sqlsrc && $(MAKE) -i -f make_w32.mak clean
	cd pcomp  && $(MAKE) -i -f make_w32.mak clean
	cd ptoc   && $(MAKE) -i -f make_w32.mak clean
	cd libpc  && $(MAKE) -i -f make_w32.mak clean

