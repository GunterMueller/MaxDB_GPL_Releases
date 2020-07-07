#
#	Hot Standby Storage API and Tests
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

* -ansi

#
# include dependencies (do not edit)
#
RTEHSS_TestClient.c inc=SAPDB_string.h
RTEHSS_TestServer.c inc=RTEHSS_PascalInterface.h,SAPDB_string.h
RTEHSS_SimulServer.cpp inc=RTEHSS_PascalInterface.h
RTEHSS_StandbyNodeList.hpp inc=SAPDB_Types.h,RTESync_NamedSpinlock.hpp,SAPDBMem_IRawAllocator.hpp,SAPDB_MemCopyMoveNoCheck.hpp
RTEHSS_StandbyNodeList.cpp inc=RTEHSS_StandbyNodeList.hpp,SAPDBMem_NewDestroy.hpp,RTEMem_RteAllocator.hpp,SAPDBErr_Assertions.hpp,RTESys_MicroTime.h
RTEHSS_PascalInterface.h inc=RTE_CompilerFeatures.h,SAPDB_Types.h,gsp00
RTEHSS_PascalInterface.cpp inc=RTEHSS_PascalInterface.h,RTEHSS_KernelInterface.hpp,RTEHSS_StandbyNodeList.hpp,RTEHSS_ApiDll.hpp,RTE_KGS.hpp,RTEMem_RteAllocator.hpp,RTE_Message.hpp,SAPDB_MemCopyMoveNoCheck.hpp
RTEHSS_KernelInterface.hpp inc=SAPDB_Singleton.hpp,SAPDB_Types.h,SAPDBErr_MessageList.hpp,RTE_Types.h,RTEHSS_Api.h
&if $OS in [ WIN32 ]
RTEHSS_KernelInterface.cpp inc=SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,RTEHSS_KernelInterface.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTEMem_RteAllocator.hpp,SAPDB_sprintf.h,geo002.h,RTE_HSS_Messages.hpp,RTE_Messages.hpp,RTE_KGS.hpp,RTEHSS_ApiDll.hpp,RTESys_CommandPipe.h,RTESys_MicroTime.h,RTEHSS_StandbyNodeList.hpp,heo51.h,RTETask_IWorkerJob.hpp,heo01.h,Tools_PipeCall.hpp,SAPDB_MemCopyMoveNoCheck.hpp
&else
RTEHSS_KernelInterface.cpp inc=SAPDBMem_NewDestroy.hpp,SAPDBErr_Assertions.hpp,RTEHSS_KernelInterface.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTEMem_RteAllocator.hpp,SAPDB_sprintf.h,geo002.h,RTE_HSS_Messages.hpp,RTE_Messages.hpp,RTE_KGS.hpp,RTEHSS_ApiDll.hpp,RTESys_CommandPipe.h,RTESys_MicroTime.h,RTEHSS_StandbyNodeList.hpp,heo51.h,RTETask_IWorkerJob.hpp,heo01.h,Tools_PipeCall.hpp,RTE_saveUNIXcalls.h,SAPDB_MemCopyMoveNoCheck.hpp
&endif
#
RTEHSS_Api.h       inc=SAPDB_Types.h
RTEHSS_ApiEmc.c    inc=RTEHSS_Api.h,RTEHSS_symapi.h
RTEHSS_ApiDummy.c  inc=RTEHSS_Api.h
&if $OS in [ WIN32 ]
RTEHSS_ApiCopy.c   inc=RTEHSS_Api.h,RTE_Types.h,RTESys_SystemInfo.h,RTESys_IO.h,RTESys_CommandPipe.h,SAPDB_sprintf.h,geo60.h,geo007_1.h,SAPDB_string.h
&else
RTEHSS_ApiCopy.c   inc=RTEHSS_Api.h,RTE_Types.h,RTESys_SystemInfo.h,RTESys_IO.h,RTESys_CommandPipe.h,SAPDB_sprintf.h,RTE_IniFileHandling.h,RTE_saveUNIXcalls.h,geo60.h,geo007_1.h,SAPDB_string.h
&endif
RTEHSS_ApiCopyTest.cpp inc=RTEHSS_Api.h
RTEHSS_ApiScript.c inc=RTEHSS_Api.h,SAPDB_string.h
RTEHSS_ApiDll.hpp  inc=RTESys_SharedLib.h,RTEHSS_Api.h
RTEHSS_ApiDll.cpp  inc=RTEHSS_ApiDll.hpp,RTE_MessageList.hpp,SAPDBMem_NewDestroy.hpp,RTE_Message.hpp,RTEConf_ParameterAccessKernelInterface.hpp,geo002.h
#
# This is the <symapi.h> which is base for EMC interface
#
RTEHSS_symapi.h inc=
#
RTEHSS_TestGetMapping.c inc=RTEHSS_Api.h
RTEHSS_TestMirrorSplit.c inc=RTEHSS_Api.h
RTEHSS_TestMirrorData.c inc=RTEHSS_Api.h
RTEHSS_TestCheckIfSynchronized.c inc=RTEHSS_Api.h
RTEHSS_TestSetLogReadOnlyStatus.c inc=RTEHSS_Api.h

