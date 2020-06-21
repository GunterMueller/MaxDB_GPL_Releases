# @(#)   desc/in.com   7.2   28/07/98
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

*       inc=gsp00,vsp001,vsp004,vsp008,vin00 
*-k     INLINK=1 trace=cccprint shrglob
*-r     RUNTIME=1
*-x     APPLONLY=1
*-t     TEST=1
*-d     DUMMY=1

vin20   inc=gsp00,vsp001,vsp004,vsp007,vsp008,vin00 
vin21   inc=gsp00,vsp001,vsp004,vsp007,vsp008,vin00 
vin21-k inc=gsp00,vsp001,vsp004,vsp007,vsp008,vin00 INLINK=1 trace=cccprint shrglob

#gin02.h    inc=vsp001,hin97.h
#hin201.h   inc=gin02.h,hsp81.h
#vin201.cpp inc=hsp77.h,hin201.h
#hin202.h   inc=vsp001,hin201.h
#vin202.cpp inc=hin202.h
#hin203.h   inc=vsp001,hin202.h
#vin203.cpp inc=hin203.h
#hin204.h   inc=hin203.h
#vin204.cpp inc=hin204.h,hsp40.h
#hin205.h   inc=hin203.h,hsp81.h
#vin205.cpp inc=hsp77.h,hin205.h
#hin206.h   inc=vsp004,hin98.h,hin203.h
#vin206.cpp inc=heo02.h,heo03.h,gsp01.h,hsp02.h,hin205.h,hin206.h
#vin209.cpp inc=heo02.h,heo03.h,vsp004,hsp02.h,hin98.h,hin204.h,hin205.h,hin206.h

&if $MACH in [ HP9 ]
vin59   -OPT=+O1
&endif

&if $OSSPEC in [ SOLARIS, SVR4 ]
vin07	-NO
&endif

&slow

&if $OS = WIN32
vin43c  -DDEBUG d
vin44c  -DDEBUG d
vin45c  -DDEBUG d
vin46c  -DDEBUG d
vin47c  -DDEBUG d
vin48c  -DDEBUG d
vin49c  -DDEBUG d
vin64c  -DDEBUG d
&endif

&endslow

gin01.h    inc=vsp001,vsp004,gsp01.h

hin28.h    inc=gin01.h,vsp001,vsp0031
vin28.c    inc=heo102.h,heo03.h,hsp26.h,hsp30.h,hsp77.h,hsp78_0.h,hsp100.h,hin28.h


vin43c     inc=vsp00c,vin49c,vin64c,vin44c,vin45c,vin43c
vin44c     inc=vsp00c,vin49c,vin45c,vin44c
vin44c-d   inc=vsp00c,vin49c,vin45c,vin44c
vin45c     inc=vsp00c,vin49c,vin46c,vin64c,vin47c,vin48c,vin45c
vin46c     inc=vsp00c,vin49c,vin46c
vin46c-t   inc=vsp00c,vin49c,vin46c
vin47c     inc=vsp00c,vin49c,vin64c,vin47c
vin48c     inc=vsp00c,vin49c,vin48c
vin49c     inc=vsp00c,vin49c
vin64c     inc=vsp00c,vin49c,vin64c

# scripting
&if $OS in [ WIN32 ]
vin76-a.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,hin28.h -I$ACTIVEPERL_INCLUDE -DWIN32 -DMSWIN32 -DACTIVE_PERL -TP
vin76-c.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,hin28.h -I$COREPERL_INCLUDE -DWIN32 -DMSWIN32 -DWIN32IO_IS_STDIO
&else
vin76-c.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,hin28.h -I$COREPERL_INCLUDE
&endif

vin77.c   inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,hin28.h -I$PYTHON_INCLUDE

instperl.pl ascii exec ->$INSTMISC/     inc=
sapdb.pm ascii ->$INSTMISC/SAP/DBTech/  inc=
SAP_DB.pm ascii ->$INSTMISC/DBD/        inc=
sapdbapi.py ascii ->$INSTMISC/          inc=
ireport.py ascii exec ->$INSTROOT/bin/  inc=

# end scripting

#hin97.h   inc=gsp12.h
#vin97.cpp inc=hsp77.h,hin97.h
#hin98.h   inc=hin97.h
#vin98.cpp inc=heo06.h,hin98.h

gin100.h   inc=geo200.h,geo47.h
hin100.h   inc=hin105.h
vin100.cpp inc=hin100.h,gcn00.h,DBMSrvMsg_Info.hpp,DBMSrvMsg_Error.hpp

hin105.h   inc=SAPDBErr_MessageList.hpp,gsp00
vin105.cpp inc=heo02.h,heo06.h,gin100.h,hin105.h
