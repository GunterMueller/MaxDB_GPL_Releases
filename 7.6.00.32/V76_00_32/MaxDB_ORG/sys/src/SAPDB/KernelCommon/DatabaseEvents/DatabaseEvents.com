
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

KernelDBE_CallbackHandler.cpp inc=\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_CallbackHandler.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventDefinition.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/Converter/Converter_IEvent.hpp,:SAPDB/Logging/Log_Volume.hpp,\
    heo60.h,hgg01_2.h
KernelDBE_CallbackHandler.hpp inc=\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp,\
    vsp0031
KernelDBE_DBFull.cpp inc=vsp0031,heo60.h,hgg01_2.h,\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_DBFull.hpp
KernelDBE_DBFull.hpp inc=
KernelDBE_Event.cpp inc=heo60.h,hgg01_2.h,\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_Event.hpp,\
    :SAPDB/KernelCommon/DatabaseEvents/KernelDBE_Messages.hpp,\
    :SAPDB/RunTime/RTE_Message.hpp,\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
KernelDBE_Event.hpp inc=vsp0031,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
KernelDBE_Messages.hpp inc=\
    :SAPDB/SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp

