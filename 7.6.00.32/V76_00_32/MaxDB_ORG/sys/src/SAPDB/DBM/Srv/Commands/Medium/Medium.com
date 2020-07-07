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
DBMSrvCmdMedium_Headers.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumLabelOffline.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp
DBMSrvCmdMedium_MediumDelete.cpp inc=hcn30.h,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp
DBMSrvCmdMedium_MediumDelete.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdMedium_MediumDeleteNoDistribute.cpp inc=hcn30.h,hcn36.h,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp
DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdMedium_MediumLabelOffline.cpp inc=hcn30.h,hcn31.h,hcn36.h,\
    hcn90.h,:SAPDB/DBM/Srv/Backup/Label/DBMSrvBLabel_Label.hpp,\
    :SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_DeviceType.hpp,\
    :SAPDB/DBM/Srv/Backup/Media/DBMSrvBMedia_Medium.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumLabelOffline.hpp,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp
DBMSrvCmdMedium_MediumLabelOffline.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdMedium_MediumPut.cpp inc=hcn30.h,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp
DBMSrvCmdMedium_MediumPut.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdMedium_MediumPutNoDistribute.cpp inc=hcn30.h,hcn36.h,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp
DBMSrvCmdMedium_MediumPutNoDistribute.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp

