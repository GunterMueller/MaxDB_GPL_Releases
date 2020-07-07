#
# DBanalyzer.com
#
#
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

regex(*.\.cfg$) ascii ->$OWN/usr/env/ unpack=no concom=yes TYPE=%-

database.cpp setbuildinfo
arguments.cpp setbuildinfo

#
# include dependencies (do not edit)
#
arguments.cpp inc=heo06.h,hsp100.h,:SAPDB/DBanalyzer/arguments.hpp,\
    :SAPDB/DBanalyzer/error.hpp
arguments.hpp inc=:SAPDB/DBanalyzer/types.hpp
configuration.cpp inc=heo01.h,:SAPDB/ToolsCommon/Tools_System.hpp,\
    :SAPDB/DBanalyzer/dbanalyzer.hpp,:SAPDB/DBanalyzer/dbanalyzer.hpp,\
    :SAPDB/DBanalyzer/configuration.hpp,:SAPDB/DBanalyzer/error.hpp
configuration.hpp inc=:SAPDB/DBanalyzer/types.hpp,\
    :SAPDB/ToolsCommon/Tools_Array.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/ToolsCommon/Tools_Expression.hpp,\
    :SAPDB/DBanalyzer/database.hpp,:SAPDB/DBanalyzer/snapshot.hpp,\
    :SAPDB/DBanalyzer/protocol.hpp
database.cpp inc=:SAPDB/SQLStudio/OAL/StudioOAL_WCommon.h,\
    :SAPDB/SQLStudio/OAL/StudioOAL_WResult.hpp,\
    :SAPDB/SQLStudio/OAL/StudioOAL_WError.hpp,\
    :SAPDB/DBanalyzer/dbanalyzer.hpp,:SAPDB/DBanalyzer/database.hpp,\
    :SAPDB/DBanalyzer/error.hpp
database.hpp inc=:SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/DBanalyzer/types.hpp,:SAPDB/DBanalyzer/snapshot.hpp,\
    :SAPDB/SQLStudio/OAL/StudioOAL_WResult.hpp
dbanalyzer.cpp inc=:SAPDB/ToolsCommon/Tools_System.hpp,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp,\
    :SAPDB/DBanalyzer/dbanalyzer.hpp,:SAPDB/DBanalyzer/arguments.hpp,\
    :SAPDB/DBanalyzer/configuration.hpp,:SAPDB/DBanalyzer/error.hpp,\
    :SAPDB/DBanalyzer/database.hpp,:SAPDB/DBanalyzer/snapshot.hpp,\
    :SAPDB/DBanalyzer/protocol.hpp
dbanalyzer.hpp inc=
error.cpp inc=:SAPDB/DBanalyzer/error.hpp,:SAPDB/DBanalyzer/protocol.hpp
error.hpp inc=:SAPDB/DBanalyzer/types.hpp
protocol.cpp inc=:SAPDB/ToolsCommon/Tools_System.hpp,\
    :SAPDB/DBanalyzer/protocol.hpp,:SAPDB/DBanalyzer/error.hpp
protocol.hpp inc=:SAPDB/ToolsCommon/Tools_Array.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,:SAPDB/DBanalyzer/types.hpp
resource.rc inc=gsp100.h
snapshot.cpp inc=:SAPDB/DBanalyzer/snapshot.hpp,\
    :SAPDB/DBanalyzer/error.hpp
snapshot.hpp inc=:SAPDB/ToolsCommon/Tools_Array.hpp,\
    :SAPDB/DBanalyzer/types.hpp
types.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp

