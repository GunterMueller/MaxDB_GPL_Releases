#   Makefile
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


include    ../Makedefs

CPCLNK=cpclnk

.SUFFIXES : .cpc .c .h .o .lib

.cpc.o:
	cpc -S oracle -H nocheck -I$(DBROOT)/incl $* -DUNIX

all :	GetBuildNr \
	PutBuildPrt \
	GetBuildPrt
clean :
	-rm *.o *.prot *.pct GetBuildNr PutBuildPrt GetBuildPrt \
	GetBuildNr.c PutBuildPrt.c GetBuildPrt.c > /dev/null 2>&1

AHuff.c:    AHuff.h
    
BitEA.c:    BitEA.h

AHuff.o :   AHuff.h BitEA.h AHuff.c

BitEA.o :   BitEA.h BitEA.c

PutBuildPrt.o : AHuff.o BitEA.o

GetBuildPrt.o : AHuff.o BitEA.o

GetBuildNr :    GetBuildNr.o
	$(CPCLNK) GetBuildNr

PutBuildPrt :   PutBuildPrt.o AHuff.o BitEA.o
	$(CPCLNK) PutBuildPrt AHuff BitEA

GetBuildPrt :   GetBuildPrt.o AHuff.o BitEA.o
	$(CPCLNK) GetBuildPrt AHuff BitEA

install: all
		rm -f $(TOOL)/pgm/GetBuildNr-
		-mv -f $(TOOL)/pgm/GetBuildNr $(TOOL)/pgm/GetBuildNr-
		cp GetBuildNr $(TOOL)/pgm/
		rm -f $(TOOL)/pgm/PutBuildPrt-
		-mv -f $(TOOL)/pgm/PutBuildPrt $(TOOL)/pgm/PutBuildPrt-
		cp PutBuildPrt $(TOOL)/pgm/
		rm -f $(TOOL)/pgm/GetBuildPrt-
		-mv -f $(TOOL)/pgm/GetBuildPrt $(TOOL)/pgm/GetBuildPrt-
		cp GetBuildPrt $(TOOL)/pgm/
