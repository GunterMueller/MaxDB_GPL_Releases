#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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
DBMSrvBMedia_DeviceType.cpp inc=gcn003.h,hcn90.h,\
    :SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_DeviceType.hpp
DBMSrvBMedia_DeviceType.hpp inc=
DBMSrvBMedia_MediaGroup.cpp inc=\
    :SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_MediaGroup.hpp
DBMSrvBMedia_MediaGroup.hpp inc=\
    :SAPDB/DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp,gcn003.h
DBMSrvBMedia_Medium.cpp inc=gcn002.h,hcn20.h,hcn29.h,hcn36.h,hcn90.h,\
    :SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_Medium.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvBMedia_Medium.hpp inc=gcn003.h

