#    ========== licence begin  GPL
#    Copyright (c) 2005-2006 SAP AG
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

#
# include dependencies (do not edit)
#
ToolsParsersOldSyntEl_AlternativeList.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_AlternativeList.hpp
ToolsParsersOldSyntEl_AlternativeList.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterGroup.hpp
ToolsParsersOldSyntEl_Concatenation.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Concatenation.hpp
ToolsParsersOldSyntEl_Concatenation.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterGroup.hpp
ToolsParsersOldSyntEl_Headers.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_AlternativeList.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Concatenation.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Keyword.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_MultipleValueVariable.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Option.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterGroup.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Repetition.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Set.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Variable.hpp
ToolsParsersOldSyntEl_Keyword.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Keyword.hpp
ToolsParsersOldSyntEl_Keyword.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp
ToolsParsersOldSyntEl_MultipleValueVariable.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_MultipleValueVariable.hpp
ToolsParsersOldSyntEl_MultipleValueVariable.hpp inc=\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp
ToolsParsersOldSyntEl_Option.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Option.hpp
ToolsParsersOldSyntEl_Option.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp
ToolsParsersOldSyntEl_ParameterBase.hpp inc=
ToolsParsersOldSyntEl_ParameterGroup.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterGroup.hpp
ToolsParsersOldSyntEl_ParameterGroup.hpp inc=\
    :SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp
ToolsParsersOldSyntEl_Repetition.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Repetition.hpp
ToolsParsersOldSyntEl_Repetition.hpp inc=\
    :SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp
ToolsParsersOldSyntEl_Set.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Set.hpp
ToolsParsersOldSyntEl_Set.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterGroup.hpp
ToolsParsersOldSyntEl_Variable.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Variable.hpp
ToolsParsersOldSyntEl_Variable.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp

