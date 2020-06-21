# @(#)   desc/sp.com   7.2.0   07/16/98
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

# 1998-01-30 Burkhard Diesing (D025086) conditinal auspacken
#            fuer vsp00c un vsp001c

*       inc=gsp00 trace=none check=no
*-p     inc=gsp00 trace=none check=no -PIC
*-k     shrglob
&if $OS = WIN32
*-d     -OPT="-MD"
&endif

vsp02.c inc=hsp02.h
vsp26   inc=gsp00,vsp001
vsp26-k inc=gsp00,vsp001 KERNEL=1
vsp78   inc=gsp00,vsp006
hsp80.h inc=gsp00,vsp008
vsp80   inc=gsp00,vsp008
hsp81.h inc=gsp00
vsp81.c inc=hsp81.h,SAPDB_string.h
vsp81Test.c inc=hsp81.h
vsp82   inc=gsp00,gsp05,vsp008
hsp83.h inc=hsp81.h
vsp83.c inc=hsp83.h
vsp91   check=yes
vsp99   check=yes

#vsp31	inc=vsp00,vgg00


vsp10c  inc=gsp00
vsp31c  inc=gsp00
vsp32c  inc=gsp00
vsp36.c inc=gsp00_0.h,gsp00_2.h
hsp45.h inc=gsp00,hsp77.h
vsp45.c inc=hsp45.h,hsp78_0.h
vsp46c  inc=gsp00
hsp77.h inc=geo00.h
vsp77.c inc=hsp77.h,hsp83.h,hsp78_0.h,SAPDB_string.h
vsp77-d.c inc=hsp77.h,hsp83.h,hsp78_0.h,SAPDB_string.h
vsp77-t.c -DTEST inc=hsp77.h,hsp83.h,hsp78_0.h,SAPDB_string.h
hsp78.h inc=gsp00,vsp006
hsp78_0.h inc=gsp00,hsp77.h
# this is only a variant, because there already existed an pascal module vsp78,
# but was 'detected' too late... jrg 13.9.2000
vsp78_0.c inc=hsp78_0.h,hsp83.h,SAPDB_string.h

gsp00   inc= dep=gsp00_2.h
gsp00_0.h inc=geo00.h
gsp00_1.h inc=
gsp00_2.h inc=gsp00_0.h
gsp01.h inc=gsp00
gsp05   inc=gsp00
gsp09.h inc=gsp00,gsp01.h,RTE_Types.h
gsp11.h inc=gsp00
vsp001  inc=gsp00
vsp002  inc=gsp00
gsp03   inc=gsp00
vsp004  inc=gsp00
gsp05  inc=gsp00
vsp006  inc=gsp00
vsp007  inc=gsp00
vsp008  inc=gsp00
vsp009  inc=gsp00
vsp0011 inc=gsp00
vsp0031 inc=gsp00
vsp002c inc=vsp002
vsp009c inc=gsp09.h

# ------  C++  ------

gsp12.h     inc=

hsp02.h     inc=gsp00
hsp20.h     inc=gsp00
hsp26.h     inc=gsp00,vsp001
hsp30.h     inc=gsp00
hsp40.h     inc=gsp00
hsp41.h     inc=gsp00
hsp42.h     inc=gsp00

hsp76.h     inc=
vsp76.cpp   inc=heo06.h,hsp76.h,hsp77.h

gsp100.h	inc=
hsp100.h	inc=gsp00,gsp100.h
vsp100.cpp  inc=gsp00,gsp100.h,hsp100.h
vsp101.cpp  inc=gsp00,gsp100.h,hsp100.h
#
# PTS 1106862
# since vsp100.cpp exists in 66 descriptions, i put this as a parallel module...
# Code is identical to vsp100.cpp, but it is C only...
# If somebody has time,he can cleanup this mess...
#
vsp102.c    inc=gsp00,gsp100.h,hsp100.h

vsp001c concom=yes
gsp200  inc=gsp00
gsp92   inc=gsp00

&if $OSSPEC in [ WIN32 ]
vsp40 -Op
vsp41 -Op
&endif

&if $OSSPEC == FREEBSD
vsp40 -ffloat-store
vsp41 -ffloat-store
&endif

&if $OSSPEC == LINUX && $MACH == I386
vsp40 -ffloat-store
vsp41 -ffloat-store
&endif


gsp100.h setbuildinfo dep=:config/Buildinfo/VERSIONS
