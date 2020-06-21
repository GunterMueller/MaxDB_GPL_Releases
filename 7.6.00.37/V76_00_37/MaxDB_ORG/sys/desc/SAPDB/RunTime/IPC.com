#
# RunTime Synchronization Modules
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


RTEIPC_NamedSharedMemory.hpp		inc=RTE_MessageList.hpp,SAPDB_ToString.hpp
RTEIPC_NamedSharedMemory.cpp		inc=RTEIPC_NamedSharedMemory.hpp,SAPDBMem_Alloca.h,RTEMem_RteAllocator.hpp,\
										RTESys_SystemInfo.h,RTE_GlobalNameSpaceNT.h,RTESys_SecurityAttributes.hpp,RTE_Message.hpp,RTE_IPC_Messages.hpp,\
										SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp\
&if $OSSPEC not in [ WIN32 ]
									,RTE_saveUNIXcalls.h,RTE_UNIXAuthenticate.hpp	
&endif

RTEIPC_Mailslot.hpp					inc=RTEIPC_CommunicationObject.hpp,RTE_MessageList.hpp
RTEIPC_Mailslot.cpp					inc=RTEIPC_Mailslot.hpp,RTEMem_RteAllocator.hpp,RTE_Message.hpp,RTE_IPC_Messages.hpp,\
                                        SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,SAPDB_MemCopyMoveNoCheck.hpp\
&if $OSSPEC not in [ WIN32 ]
									,RTE_saveUNIXcalls.h,RTE_UNIXAuthenticate.hpp	
&endif
										
RTEIPC_CommunicationObject.hpp		inc=RTE_MessageList.hpp,SAPDB_ToString.hpp										

