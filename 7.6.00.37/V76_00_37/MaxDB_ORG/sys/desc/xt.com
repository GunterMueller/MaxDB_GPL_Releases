# @(#)   desc/fast/xt.com   6.1.2   07/26/96
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

*       inc=gsp00,gut00,ggg00 trace=none shadow=yes

hxt12.h     inc=-gut00
vxt01		inc=-ggg00,+vsp004
vxt02		inc=+vsp001,vsp002,gsp05,ggg04,ggg07,ggg11,ggg12,gak00,vak001
vxt03		inc=+vsp002,vsp0031,gkb00,gkb03
vxt03-ci SLOWCI=1 inc=+vsp002,vsp0031,gkb00,gkb03
vxt04		inc=+gkb00,gbd00
vxt05		inc=+vsp001,gsp05,ggg04,gak00,vak001
vxt07		inc=+gbd00
vxt07-ci SLOWCI=1	inc=+gbd00
vxt08	    inc=+vsp001,vsp002,ggg04,gkb00,gkb05,gbd00,gbd02,gak00,vak001
 
