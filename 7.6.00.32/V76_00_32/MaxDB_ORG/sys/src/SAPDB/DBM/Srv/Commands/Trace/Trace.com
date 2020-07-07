
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

DBMSrvCmdTrc_Header.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceClear.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceFlush.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOff.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOn.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceProt.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceProtOpt.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceShow.hpp
DBMSrvCmdTrc_TraceClear.cpp inc=hcn50.h,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceClear.hpp
DBMSrvCmdTrc_TraceClear.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdTrc_TraceFlush.cpp inc=hcn50.h,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceFlush.hpp
DBMSrvCmdTrc_TraceFlush.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdTrc_TraceOff.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOn.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOff.hpp
DBMSrvCmdTrc_TraceOff.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdTrc_TraceOn.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceOn.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp
DBMSrvCmdTrc_TraceOn.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdTrc_TraceProt.cpp inc=hcn40.h,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceProt.hpp
DBMSrvCmdTrc_TraceProt.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdTrc_TraceProtOpt.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceProtOpt.hpp
DBMSrvCmdTrc_TraceProtOpt.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdTrc_TraceShow.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceShow.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp
DBMSrvCmdTrc_TraceShow.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp

