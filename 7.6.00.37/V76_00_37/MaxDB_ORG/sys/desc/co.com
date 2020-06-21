# @(#)   desc/fast/co.com   7.0  08/15/96
#
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

&ifdebug
*.c -DOMS_DEBUG -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
*.cpp -DOMS_DEBUG -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
&else
*.c -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
*.cpp -DOMS_IMPLEMENTATION -DSQLCLS_IMPL
&endif


#vco005x    unpack=no concom=yes dep=OMS_IliveCacheSpec.idl >$INSTROOT/incl/ilivecachesql.h extdep=$INSTROOT/incl/ilivecachesink.h
	
vco20.xml lang=r dep=OMS_IliveCacheSpec.hpp,OMS_DbpGlobals.hpp obj=vco20Ispc.o \
          exdep=$WRK/incl/vco20.h,$WRK/incl/vco20_i.c \
          unpack=no noversion \
          -xml2ispc -notdc
&if ($OSSPEC!=WIN32 or defined BIT64) and not ($MACH == SUN || $OSSPEC == SOLARIS)
vco20.xml -novalidate
&endif

vco70.idl   dep=gco70.h unpack=no noobjcopy obj=vco70_i \
           -header=$WRK/incl/vco70t.h extdep=$WRK/incl/vco70t.h

hco20.h     dep=OMS_DbpGlobals.hpp
vco20.rgs   binary
vco20rc.rc  dep=vco20.xml demand=vco20.rgs
# inc ggg00 inserted to LVC_IliveCacheSink.hpp PTS 1109257
gco72.h     inc=gco71.h
hco70.h     inc=gco70.h

vco70m.bmp  binary
vco70l.bmp  binary
vco70r.rc   demand=vco70m.bmp,vco70l.bmp

vco20.cpp 	-except dep=vco20.xml,OMS_LiveCacheTypes.xml,OMS_ClientInterface.hpp,OMS_DbpBase.hpp,OMS_DbpGlobals.hpp,SQL_ClientInterface.hpp inc=heo02x.h,geo573.h,hco20.h -MD -c
#vco21.cpp 	-except dep=OMS_Defines.h,OMS_DbpGlobals.hpp inc=hco21.h -MD -c
vco22.cpp 	-except dep=vco20.xml,OMS_IliveCacheSpec.hpp,OMS_Defines.h,OMS_DbpGlobals.hpp,OMS_ClientInterface.hpp,OMS_DbpBase.hpp,SQL_ClientInterface.hpp -MD -c
&if $MACH in [ PA11 , PA20W ]
vco20.cpp		-CFLAGS=+Onoinline
&endif

vco69.cpp inc=OMS_Defines.h,heo02x.h,heo01.h,heo46.h
vco70.cpp inc=gco74.h,hco70.h,gco72.h,gco71.h -MD -GX -DWIN32 [s -D_DEBUG s] [f -DNDEBUG f] [q -DNDEBUG q] -D_WINDOWS -D_WINDLL -D_AFXDLL  -c  -Y -uc -ansi
vco71.cpp inc=gco74.h,hco70.h,gco73.h,gco71.h -MD -GX -DWIN32 [s -D_DEBUG s] [f -DNDEBUG f] [q -DNDEBUG q] -D_WINDOWS -D_WINDLL -D_AFXDLL  -c  -Y -uc -ansi
vco72.cpp inc=gco74.h,hco70.h,gco72.h,gco71.h -MD -GX -DWIN32 [s -D_DEBUG s] [f -DNDEBUG f] [q -DNDEBUG q] -D_WINDOWS -D_WINDLL -D_AFXDLL  -c  -Y -uc -ansi
vco73.cpp inc=gco74.h,hco70.h,gco73.h,gco71.h -MD -GX -DWIN32 [s -D_DEBUG s] [f -DNDEBUG f] [q -DNDEBUG q] -D_WINDOWS -D_WINDLL -D_AFXDLL  -c  -Y -uc -ansi
#



