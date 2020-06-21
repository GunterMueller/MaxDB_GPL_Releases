
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

* -I$WRK/incl/SAPDB
#
# StudioUtil
#


StudioUtil_Convert.hpp			inc=SAPDB_Types.hpp,hsp78_0.h dep=gsp100.h
StudioUtil_Convert.cpp          inc=StudioUtil_Convert.hpp 
StudioUtil_Functions.hpp		inc=SAPDB_Types.hpp
StudioUtil_Functions.cpp        inc=StudioUtil_Functions.hpp

