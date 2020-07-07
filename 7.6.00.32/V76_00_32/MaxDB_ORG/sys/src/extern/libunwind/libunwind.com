#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

&if $OSSPEC in [ LINUX ] && $MACH in [ I386, X86_64, IA64 ]
# The original name was modified, since vmake does use '-' for variants...
libunwind_0_98_3.tar.gz ->$WRK/extern/libunwind-0.98.3.tar.gz binary

build_libunwind.pl ->$WRK/extern/build_libunwind.pl ascii \
	execscript=build_libunwind.pl \
        dep=:extern/libunwind/libunwind_0_98_3.tar.gz \
        extdep=$WRK/extern/lib/libunwind.a,\
               $WRK/incl/extern/include/libunwind.h,\
               $WRK/incl/extern/include/libunwind-dynamic.h,\
               $WRK/incl/extern/include/libunwind-common.h
&endif
