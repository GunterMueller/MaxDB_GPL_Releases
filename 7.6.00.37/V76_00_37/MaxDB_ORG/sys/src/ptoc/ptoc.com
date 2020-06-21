

#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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


convert_ptoc_types.pl ascii concom=no noversion unpack=no exec ->$OWN/tool/bin/ suppress_uncond
truncate_ptoc_header.pl ascii concom=no noversion unpack=no exec ->$OWN/tool/bin/ suppress_uncond

ptoc_h ascii concom=no unpack=no noversion <-$TOOL/incl/ptoc.h ->$WRK/incl/SAPDB/ptoc.h \
       dep=:ptoc/convert_ptoc_types.pl execscript=$OWN/tool/bin/convert_ptoc_types.pl suppress_uncond
       
#ptoc.h ascii concom=no unpack=no noversion ->$WRK/incl/SAPDB/ptoc.h \
#       dep=:ptoc/convert_ptoc_types.pl execscript=$OWN/tool/bin/convert_ptoc_types.pl suppress_uncond

ptoc.h ->$WRK/incl/SAPDB/ 

