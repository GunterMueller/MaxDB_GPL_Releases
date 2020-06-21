#
# TableDefinition
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

StudioTD_Util.cpp                  inc=StudioTD_Util.h
StudioTD_ColumnDef.cpp             inc=StudioTD_ColumnDef.h
StudioTD_ConditionDef.cpp          inc=StudioTD_ConditionDef.h
StudioTD_ConstraintDef.cpp         inc=StudioTD_ConstraintDef.h
StudioTD_ForeignKeyDef.cpp         inc=StudioTD_ForeignKeyDef.h
StudioTD_TableDef.cpp              inc=StudioTD_TableDef.h
StudioTD_WTableDefODBC.cpp         inc=StudioTD_WTableDefODBC.h
StudioTD_ATableDefODBC.cpp         inc=StudioTD_ATableDefODBC.h
StudioTD_TableDefOI.cpp            inc=StudioTD_TableDefOI.h

StudioTD_Global.h                  inc=Tools_DynamicUTF8String.hpp
StudioTD_Util.h                    inc=StudioTD_Global.h,SAPDB_Types.hpp
StudioTD_ColumnDef.h               inc=StudioTD_Global.h,StudioTD_Util.h,StudioTD_Base.h
StudioTD_ConditionDef.h            inc=StudioTD_Global.h,StudioTD_Util.h,StudioTD_Base.h
StudioTD_ConstraintDef.h           inc=Studio_List.hpp,StudioTD_Global.h,StudioTD_Util.h,StudioTD_ConditionDef.h,StudioTD_Base.h
StudioTD_ForeignKeyDef.h           inc=StudioTD_Global.h,StudioTD_Util.h,StudioTD_Base.h
StudioTD_TableDef.h                inc=Studio_List.hpp,StudioTD_Global.h,StudioTD_Util.h,StudioTD_ColumnDef.h,StudioTD_ConstraintDef.h,StudioTD_ForeignKeyDef.h,StudioTD_Base.h
StudioTD_WTableDefODBC.h           inc=StudioTD_TableDef.h,StudioOAL_WResult.hpp
StudioTD_ATableDefODBC.h           inc=StudioTD_TableDef.h,StudioOAL_AResult.hpp
StudioTD_TableDefOI.h              inc=StudioTD_TableDef.h

StudioTD_Wrapper.cpp               inc=StudioTD_Wrapper.h
StudioTD_Wrapper.h                 inc=StudioTD_WTableDefODBC.h

&if $OSSPEC == AIX
regex(\w+\.cpp) -NO
&endif
