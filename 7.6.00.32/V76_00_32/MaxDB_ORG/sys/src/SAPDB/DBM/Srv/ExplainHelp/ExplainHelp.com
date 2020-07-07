
#
# include dependencies (do not edit)
#
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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

DBMSrvExpHlp_CommandPart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp
DBMSrvExpHlp_DefaultValuePart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp
DBMSrvExpHlp_FuzzyStringDifference.cpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_FuzzyStringDifference.hpp
DBMSrvExpHlp_FuzzyStringDifference.hpp inc=
DBMSrvExpHlp_ParameterPart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp
DBMSrvExpHlp_Part.cpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp,hcn36.h
DBMSrvExpHlp_Part.hpp inc=
DBMSrvExpHlp_PreconditionsPart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp
DBMSrvExpHlp_ReplyPart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp
DBMSrvExpHlp_ReplyValuePart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp
DBMSrvExpHlp_SyntaxPart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp
DBMSrvExpHlp_ValuePart.hpp inc=\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp

