# msvc70.com
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

msvcr70.dll ->$INSTROOT binary
dbghelp.dll ->$INSTROOT binary
msvcr70-install.dll ->$WRK/fast/install/msvcr70.dll binary
msvcr70-install56.dll ->$WRK/fast/install56/msvcr70.dll binary
dbghelp-develop.dll binary ->$INSTROOT/pgm/dbghelp.dll
mfc70.dll ->$INSTROOT binary
mfc70u.dll ->$INSTROOT binary