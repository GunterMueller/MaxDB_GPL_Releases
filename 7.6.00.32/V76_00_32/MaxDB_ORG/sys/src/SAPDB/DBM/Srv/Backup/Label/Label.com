#	========== licence begin LGPL
#    Copyright (c) 1998-2005 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end

#
# include dependencies (do not edit)
#
DBMSrvBLabel_Label.cpp inc=\
    :SAPDB/DBM/Srv/Backup/Label/DBMSrvBLabel_Label.hpp,\
    :SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,gcn00.h,hcn36.h,\
    hcn90.h,heo02.h,heo06.h,gkb03
DBMSrvBLabel_Label.hpp inc=hcn36.h,\
    :SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_DeviceType.hpp

