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

#
# include dependencies (do not edit)
#
DBMSrvVol_AutoExtensionValue.cpp inc=hcn36.h,hcn51.h,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_AutoExtensionValue.hpp
DBMSrvVol_AutoExtensionValue.hpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_Volume.hpp
DBMSrvVol_Volume.cpp inc=hsp77.h,gcn00.h,gcn002.h,hcn20.h,hcn36.h,\
    hcn46.h,hcn50.h,:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_Volume.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_VolumeName.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
DBMSrvVol_Volume.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_VolumeName.hpp
DBMSrvVol_VolumeName.cpp inc=\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_VolumeName.hpp
DBMSrvVol_VolumeName.hpp inc=\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp
DBMSrvVol_VolumeSize.cpp inc=hcn36.h,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_VolumeSize.hpp
DBMSrvVol_VolumeSize.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvVol_Volumes.cpp inc=hcn36.h,hcn51.h,hcn52.h,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_AutoExtensionValue.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_Volumes.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_ToString.hpp
DBMSrvVol_Volumes.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_Volume.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_VolumeSize.hpp

