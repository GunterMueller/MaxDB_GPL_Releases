# @(#)   desc/fast/tt.com   6.1.2   29/07/96
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

*       trace=none shadow=yes
*.pl ascii exec ->$TOOL/bin/
*.py ascii exec ->$TOOL/bin/

# kw
vtt01c
# check
vtt20c

# iget, iput, idel
vtt10x			-NO
vtt12x		-GX	-NO inc=vtt05x,vtt07x

#iserver
vtt06x		-GX	inc=vtt05x,vtt07x,vtt09x
vtt08x		-GX	inc=vtt05x,vtt07x,vtt09x

rename.dta ascii  ->$TOOL/lib/Python/
machineInfo.c

tooldel ascii  ->$TOOL/adminbin/

instdummy.pl output=$TOOL/bin/instlserver.pl,$TOOL/bin/instdbmsrv.pl,$TOOL/bin/instdbmverifypw.pl


