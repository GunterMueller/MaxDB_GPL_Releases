# @(#)   desc/ut.com   7.26.1.2   07/28/98
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

*       inc=gsp00,ggg00,gut00 trace=cccprint

vut02   inc=+vsp001,vsp004
vut05   inc=+vsp001,vsp002,ggg04
vut06   inc=+vsp001,gsp05
vut07   inc=-ggg00,+vsp001
vut10   inc=+vsp001,ggg04,gak00,vak001   -Kt  -DELTYPESP
vut11   inc=+vsp001,ggg04,gak00,vak001   -Kt  -DELTYPESP
vut12   inc=+vsp001,ggg04,gak00,vak001   -Kt  -DELTYPESP
vut13   inc=+vsp001,ggg04,gak00,vak001   -Kt  -DELTYPESP
vut14   inc=+vsp001,ggg04,gak00,vak001   -Kt  -DELTYPESP
vut15   inc=+vsp001,ggg04,gak00,vak001   -Kt  -DELTYPESP

# ------ xcontrol ------

vut03-c	xcontrol=1
