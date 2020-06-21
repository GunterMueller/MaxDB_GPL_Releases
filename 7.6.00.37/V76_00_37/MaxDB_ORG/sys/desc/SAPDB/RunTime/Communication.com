#
# RunTime Environment / Communication Descriptions
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

*         -except 
#
# No exception handling code variant
#
*-noex    -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
#
RTEComm_Swapping.h						inc=RTE_Types.h,RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTEComm_Swapping.hpp
RTEComm_Swapping.hpp				    inc=RTE_Types.h,RTE_MessageList.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp

RTEComm_Swapping.cpp					inc=RTE_Types.h,RTE_Crash.hpp,RTE_MessageList.hpp,RTE_Comm_Messages.hpp,RTEComm_Swapping.h,\
											SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp

RTEComm_PacketHeader.h					inc=RTE_Types.h,RTEComm_PacketHeader.hpp
RTEComm_PacketHeader.hpp     			inc=RTE_Types.h,RTE_MessageList.hpp,RTEComm_Swapping.h,SAPDB_MemCopyMoveNoCheck.hpp

RTEComm_PacketHeader.cpp				inc=RTE_Types.h,RTE_MessageList.hpp,RTEComm_Swapping.h,RTEComm_PacketHeader.h,SAPDBErr_Assertions.hpp,\
											SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp

RTEComm_ParseURIEscapeRFC2396.hpp		inc=SAPDB_Types.h
RTEComm_BuildURIEscapeRFC2396.cpp		inc=SAPDB_Types.hpp,SAPDBMem_Alloca.h

RTEComm_URIUtils.hpp					inc=SAPDB_Types.hpp,RTE_MessageList.hpp,RTEComm_ParseURIEscapeRFC2396.hpp,SAPDB_ToString.hpp
RTEComm_URIUtils.cpp					inc=SAPDB_Types.hpp,RTEComm_URIUtils.hpp,RTEComm_ParseURIEscapeRFC2396.hpp,\
											RTEMem_RteAllocator.hpp,RTE_MessageList.hpp,RTE_Comm_Messages.hpp

RTEComm_ParseURI.hpp					inc=SAPDB_Types.hpp,RTEComm_URIUtils.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTE_MessageList.hpp
RTEComm_ParseURI.cpp					inc=SAPDB_Types.hpp,RTEComm_ParseURI.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,\
											RTE_MessageList.hpp,RTE_Comm_Messages.hpp\
											,:sap/700/saptype.h,:sap/700/saptypeb.h,:sap/700/saptypec.h\
											,:sap/700/sapuc.h,:sap/700/nixx.h,:sap/700/err.h,:sap/700/dptrace.h,:sap/700/sapuc2.h 
											
RTEComm_URIBuilder.hpp					inc=SAPDB_Types.hpp,RTEComm_URIUtils.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTEComm_URIBuilder.cpp					inc=geo401.h,geo43_0.h,RTEComm_URIBuilder.hpp,SAPDB_string.h,SDBMsg_RTEComm.h

RTEComm_RequestRepsonsePacket.hpp		inc=SAPDB_Types.h,RTE_Message.hpp,SAPDBErr_Assertions.hpp,RTEComm_Swapping.h
RTEComm_RequestRepsonsePacket.cpp		inc=RTEComm_RequestRepsonsePacket.hpp,RTE_Comm_Messages.hpp,SAPDB_MemCopyMoveNoCheck.hpp

RTEComm_Packet.hpp						inc=SAPDB_Types.h,RTE_Message.hpp,SAPDBErr_Assertions.hpp,RTEComm_Swapping.h,RTEComm_RequestRepsonsePacket.hpp,RTEComm_PacketHeader.h
RTEComm_Packet.cpp						inc=RTEComm_RequestRepsonsePacket.hpp,RTEComm_Packet.hpp,RTE_Comm_Messages.hpp
