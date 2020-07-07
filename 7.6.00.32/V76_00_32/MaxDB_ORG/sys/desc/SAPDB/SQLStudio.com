
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

Studio_DBFSObject.hpp		inc=hwd101dbfs.h,Studio_Connection.hpp
Studio_DBFSObject.cpp		inc=Studio_DBFSObject.hpp,Studio_Errors.h
Studio_Objects.hpp			inc=Tools_DynamicUTF8String.hpp,vpa00sqlod32.h,gpr00.h
Studio_Objects.cpp			inc=Studio_Objects.hpp,Tools_UTF8Basis.hpp,SAPDB_Types.h
Studio_StoredFileContent.hpp			inc=Tools_DynamicUTF8String.hpp,vpa00sqlod32.h,gpr00.h
Studio_StoredFileContent.cpp			inc=Studio_StoredFileContent.hpp,Tools_UTF8Basis.hpp,SAPDB_Types.h
Studio_Statements.hpp			inc=Tools_DynamicUTF8String.hpp
Studio_Statements.cpp			inc=Studio_Statements.hpp,Studio_Functions.hpp
Studio_Functions.hpp			inc=Tools_DynamicUTF8String.hpp
Studio_Functions.cpp			inc=Studio_Functions.hpp
Studio_Connection.hpp			inc=Tools_DynamicUTF8String.hpp,hwd08dbc.h,vpa00sqlod32.h,gpr00.h
Studio_Connection.cpp			inc=Studio_Connection.hpp
Studio_List.hpp				inc=SAPDB_Types.hpp
Studio_List.cpp				inc=Studio_List.hpp	


