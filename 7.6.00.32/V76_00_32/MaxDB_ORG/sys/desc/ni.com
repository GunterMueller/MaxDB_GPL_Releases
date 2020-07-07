# @(#)   desc/fast/ni.com   6.1.2   08/13/96
#
#  1995-07-25  nst  Erweiterung fuer Windows NT
#  1995-07-27  cs   Erweiterung fuer OS/2
#  1995-08-01  jh   Erweiterung fuer x_wizard,x_wizbit
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

*       inc=vni00c trace=none check=no

vni90c  -x=compcpc dep=vpi00c -I$SAPDBSDK/incl
vni90c-n -DREADONLY -x=compcpc dep=vpi00c -I$SAPDBSDK/incl
vni70c  -x=compcpc dep=vpi00c -I$SAPDBSDK/incl

vni02c inc=+hsp78_0.h
&if $OS in [ OS2, WIN32 ]
vni01c  inc=gsp00,vsp004,vni00c        trace=none check=no
&else
vni01c  inc=gsp00,vsp004,gen00.h,vni00c trace=none check=no
&endif

vni10c  -x=compcpc inc=vni00wc,heo01.h,heo11.h dep=vpi00c -I$SAPDBSDK/incl

vni11c	inc=vni00wc 
vni12c	inc= 

vni20c  -x=compcpc inc= dep=vpi00c -I$SAPDBSDK/incl

vni25c  inc=vni00wc dep=vpi00c

# xbackup, xrestore and backrest.exe for UNIX and NT
hni00b.h		inc= concom=yes
vni30.cpp		inc=hni00b.h,hni32.h,hni33.h,hsp100.h #,hni36.h
hni31.h			inc=hni00b.h

hni32.h			inc=hni00b.h,hni31.h
vni32.cpp		inc=hni32.h

hni33.h			inc=hni00b.h,hni31.h,hni34.h,RTESys_ProcessID.hpp
vni33.cpp		inc=hni33.h,hni35.h

hni34.h         inc=hni00b.h,heo06.h
vni34.cpp		inc=hni34.h,hni35.h

hni35.h			inc=hni00b.h
vni35.cpp		inc=hni35.h

vni50c  -x=compcpc inc= unpack=no dep=vpi00c -I$SAPDBSDK/incl
vni55c  -x=compcpc inc= dep=vpi00c -I$SAPDBSDK/incl
vni56c  -x=compcpc inc= dep=vpi00c -I$SAPDBSDK/incl
vni57c  -x=compcpc inc= dep=vpi00c -I$SAPDBSDK/incl
