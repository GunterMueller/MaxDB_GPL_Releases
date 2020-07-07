# @(#)   desc/ta.com   7.2.0   07/16/98
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

*			inc=gsp00,ggg00,gut00 trace=none

# ------  PASCAL  ------ 

gta00		inc=+vsp001,vsp004,vsp008

&quick
vta05		inc=+vsp004
vta12		inc=+gsp05
vta13		inc=+gsp05  

&endquick

&fast
vta01		inc=-gut00,+vsp001,gsp03,ggg04,gak00,vak001,ggg07 check=no  shrglob  
vta02		inc=+vsp001			check=no
vta05		inc=+vsp004

vta12		inc=+gsp05  
vta13		inc=+gsp05  
&if $MACH = T35
vta14 -g		    # Optim Error on T35
&else
vta14  d
&endif
&endfast

vta14       inc=+gbd00
vta31		inc=+vsp001,vsp004,vsp008,gta00	[s SLOW=y s]
vta320		inc=+vsp001,vsp004,vsp008,gta00
vta340		inc=+vsp001,vsp004,vsp008,gta00,ggg04
vta370		inc=+vsp001,vsp004,vsp008,gta00
vta39		inc=gsp00
vta33       inc=+vsp001,vsp004,vsp008,gta00

# ------  C++  ------ 

hta01.h		inc=gsp00,ggg00
hta01_1.h	inc=ggg00,vsp001
hta01_2.h	inc=ggg00,ggg92.h
hta01_3.h	inc=gsp00,ggg00,gak00,vak001,ggg04,ggg07
hta99.h		inc=ggg00,gsp00_0.h,hta01.h
hta320.h    inc=gsp00,gut00,vsp001,gta00
vta321.cpp  inc=gta00,hgg17.h,hta320.h,geo200.h,\
                :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp,\
                :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp

vta99.cpp	inc=hsp77.h,hta99.h,gsp00_0.h,hta01.h

&if $OSSPEC in [ LINUX,FREEBSD ]
vta01   -CFLAGS=-ffloat-store
&endif
