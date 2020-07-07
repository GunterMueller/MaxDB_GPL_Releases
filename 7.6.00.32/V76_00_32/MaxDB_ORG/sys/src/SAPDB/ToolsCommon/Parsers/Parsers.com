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

ToolsParsers_ParseResult.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Keyword.hpp,\
    :SAPDB/ToolsCommon/Parsers/ToolsParsers_ParseResult.hpp
ToolsParsers_ParseResult.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Keyword.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Variable.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp
ToolsParsers_ProgramParameterParser.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/ToolsParsers_ProgramParameterParser.hpp
ToolsParsers_ProgramParameterParser.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Syntax/ToolsParsersSyntax_Syntax.hpp,\
    :SAPDB/ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterBase.hpp

