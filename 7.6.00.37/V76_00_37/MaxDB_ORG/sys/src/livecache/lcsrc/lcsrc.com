# livecache/lcrsc.com
# com descriptions of all livecache executables and headers for lcapps
#
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

incl_com ascii concom=no unpack=no ->$WRK/lcsrc/incl/incl.com 
pgm_com ascii concom=no unpack=no ->$WRK/lcsrc/pgm/pgm.com 
lib_com ascii concom=no unpack=no ->$WRK/lcsrc/lib/lib.com 
lib64_com ascii concom=no unpack=no ->$WRK/lcsrc/lib/lib64/lib64.com 