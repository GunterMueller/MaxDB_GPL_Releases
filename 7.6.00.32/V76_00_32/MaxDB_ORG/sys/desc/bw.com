#
# @(#)bw.com 7.2.0.0 2000-04-13
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

*.cpp   -DOMS_IMPLEMENTATION

:bw/sphsapdbIspc.h      inc=OMS_Defines.h
:bw/sphsapdbObj.h       inc=OMS_DbpGlobals.hpp
:bw/sphsapdbIspc.cpp    -except inc=:bw/sphsapdbIspc.h
:bw/sphsapdbObj.cpp     -except inc=:bw/sphsapdb.h,:bw/sphsapdbObj.h,OMS_Defines.h dep=OMS_LiveCacheTypes.xml
:bw/sphsapdb.cpp        -except inc=OMS_Defines.h,:bw/sphsapdb.h,:bw/sphsapdbObj.h dep=:bw/sphsapdb_i.c,OMS_DbpError.hpp,OMS_ClientInterface.hpp,OMS_DbpGlobals.hpp

sphsapdb_i.c ascii ->$WRK/incl
sphsapdb.lst ascii ->$INSTROOT/sap/
