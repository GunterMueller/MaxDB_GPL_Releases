#
# RunTime Environment / Console Descriptions
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

*
#
# No exception handling code variant
#
*-noex -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
#
#
# XCons
RTECons_Main.h                          inc=
RTECons_Main.cpp    -DUSE_KGS           inc=heo92.h,geo007_1.h,hsp100.h,RTE_CompilerFeatures.h,RTE_MessageList.hpp,RTE_Message.hpp\
                                            ,RTE_XCons_Messages.hpp,RTE_ConsoleCommon.hpp\
                                            ,RTECons_StandardSession.hpp,RTEMem_RteAllocator.hpp\
                                            ,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_ConsoleTraceTopic.hpp,RTE_ConsoleStandardOutput.h\
&if ($OSSPEC == LINUX && $__HAS_NPTL != 1) || $MACH == SUN || $OSSPEC == SOLARIS
                                            ,RTE_ConsoleSemaphoreTimeoutList.hpp
&endif  

#
# XCons Session
RTECons_StandardSession.hpp             inc=RTE_ConsoleStandardDataRequests.h,RTECons_Main.h,RTECons_Communication.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
RTECons_StandardSession.cpp  -DUSE_KGS -DKERNEL_LZU  inc=heo92.h,heo00x.h,geo00_3.h,heo47.h,SAPDBErr_MessageEventConverter.hpp,RTE_CompilerFeatures.h,RTE_MessageList.hpp\
                                            ,RTE_Message.hpp,RTE_XCons_Messages.hpp\
                                            ,RTECons_StandardSession.hpp,RTEMem_RteAllocator.hpp,SAPDB_sprintf.h\
                                            ,RTE_ConsoleStandardOutput.h,SAPDB_MemCopyMoveNoCheck.hpp,RTETask_TaskScheduler.hpp



#
# Console communication
RTECons_Communication.hpp               inc=RTE_ConsoleDataCommunication.hpp,RTE_ConsoleRequestCommunication.hpp
RTECons_Communication.cpp  -DUSE_KGS    inc=hsp77.h,hsp100.h,heo15.h,RTESys_Spinlock.h,RTE_MessageList.hpp,RTE_Message.hpp,RTE_XCons_Messages.hpp\
                                            ,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,RTE_ConsoleCommon.hpp,RTECons_Communication.hpp\
                                            ,RTE_ConsoleRequestCommunication.hpp,RTE_MessageList.hpp,RTE_Message.hpp,RTE_XCons_Messages.hpp,RTE_KSS.h\
                                            ,RTECons_Main.h\
&if $OSSPEC = WIN32
                                            ,gos41.h
&else
                                            ,geo50_0.h,gen41.h
&endif



