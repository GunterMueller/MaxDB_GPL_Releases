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

ToolsParsersSyntEl_AlternativeList.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_AlternativeList.hpp
ToolsParsersSyntEl_AlternativeList.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp
ToolsParsersSyntEl_Concatenation.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Concatenation.hpp
ToolsParsersSyntEl_Concatenation.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp
ToolsParsersSyntEl_Headers.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_AlternativeList.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Concatenation.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Keyword.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_MultipleValueVariable.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Option.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Repetition.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Set.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Variable.hpp
ToolsParsersSyntEl_Integer.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Integer.hpp
ToolsParsersSyntEl_Integer.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp
ToolsParsersSyntEl_Invalid.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Invalid.hpp
ToolsParsersSyntEl_Invalid.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp
ToolsParsersSyntEl_Keyword.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Keyword.hpp
ToolsParsersSyntEl_Keyword.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp
ToolsParsersSyntEl_MultipleValueVariable.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_MultipleValueVariable.hpp
ToolsParsersSyntEl_MultipleValueVariable.hpp inc=\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp
ToolsParsersSyntEl_NonTerminal.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp
ToolsParsersSyntEl_NonTerminal.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterBase.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp
ToolsParsersSyntEl_Option.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Option.hpp
ToolsParsersSyntEl_Option.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp
ToolsParsersSyntEl_ParameterBase.cpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterBase.hpp
ToolsParsersSyntEl_ParameterBase.hpp inc=
ToolsParsersSyntEl_ParameterGroup.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp
ToolsParsersSyntEl_ParameterGroup.hpp inc=\
    :SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp
ToolsParsersSyntEl_Repetition.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Repetition.hpp
ToolsParsersSyntEl_Repetition.hpp inc=:SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp
ToolsParsersSyntEl_Set.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Set.hpp
ToolsParsersSyntEl_Set.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp
ToolsParsersSyntEl_Terminal.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp
ToolsParsersSyntEl_Terminal.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterBase.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp
ToolsParsersSyntEl_Variable.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Variable.hpp
ToolsParsersSyntEl_Variable.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp

