
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

DBMCliEvtDispKeyMatch_Expression.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp
DBMCliEvtDispKeyMatch_Expression.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_ValueProvider.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Variable.hpp
DBMCliEvtDispKeyMatch_ValueProvider.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Variable.hpp,\
    :SAPDB/ToolsCommon/Tools_Expression.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp
DBMCliEvtDispKeyMatch_Variable.hpp inc=\
    :SAPDB/ToolsCommon/Tools_Expression.hpp
