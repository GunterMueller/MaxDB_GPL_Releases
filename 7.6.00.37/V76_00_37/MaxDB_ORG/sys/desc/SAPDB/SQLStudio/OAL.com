
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

7.4
StudioOAL_ACommon.h	    inc=gpr00.h,geo47.h,vpa00sqlod32.h,Tools_DynamicUTF8String.hpp,Tools_UTF8Basis.hpp,Studio_List.hpp
StudioOAL_AResult.hpp       inc=StudioOAL_ACommon.h,StudioOAL_AColumn.hpp,StudioOAL_AParameters.hpp,StudioOAL_AError.hpp
StudioOAL_AResult.cpp       inc=StudioOAL_AResult.hpp
StudioOAL_AColumn.hpp       inc=StudioOAL_ACommon.h
StudioOAL_AColumn.cpp       inc=StudioOAL_AColumn.hpp
StudioOAL_AParameters.hpp   inc=StudioOAL_ACommon.h,SAPDB_Types.hpp
StudioOAL_AParameters.cpp   inc=StudioOAL_AParameters.hpp
StudioOAL_AError.hpp	    inc=StudioOAL_ACommon.h,SAPDB_Types.hpp
StudioOAL_AError.cpp        inc=StudioOAL_AError.hpp

StudioOAL_WCommon.h	    inc=gpr00.h,geo47.h,vpa00sqlod32.h,Tools_DynamicUTF8String.hpp,Tools_UTF8Basis.hpp,Studio_List.hpp
StudioOAL_WResult.hpp       inc=StudioOAL_WCommon.h,StudioOAL_WColumn.hpp,StudioOAL_WParameters.hpp,StudioOAL_WError.hpp
StudioOAL_WResult.cpp       inc=StudioOAL_WResult.hpp,SAPDBCommon/SAPDB_string.h
StudioOAL_WColumn.hpp       inc=StudioOAL_WCommon.h
StudioOAL_WColumn.cpp       inc=StudioOAL_WColumn.hpp
StudioOAL_WParameters.hpp   inc=StudioOAL_WCommon.h,SAPDB_Types.hpp
StudioOAL_WParameters.cpp   inc=StudioOAL_WParameters.hpp
StudioOAL_WError.hpp	    inc=StudioOAL_WCommon.h,SAPDB_Types.hpp
StudioOAL_WError.cpp        inc=StudioOAL_WError.hpp




