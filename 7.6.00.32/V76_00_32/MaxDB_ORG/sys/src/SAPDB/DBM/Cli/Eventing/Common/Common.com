
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

DBMCliEvtCm_Branding.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Branding.hpp
DBMCliEvtCm_Branding.hpp inc=:SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMCliEvtCm_Event.cpp inc=:SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/DBM/Cli/DBMCli_DateTime.hpp,\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp
DBMCliEvtCm_Event.hpp inc=:SAPDB/DBM/Cli/DBMCli_DateTime.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Result.hpp,:SAPDB/RunTime/RTE_ISystem.hpp

