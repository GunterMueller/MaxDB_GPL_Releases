#
# Message handling descriptions
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

* -except
#
# No exception handling code variant
#
*-noex -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
#
# Variant without UNIX password verification (used inside kernel)
#
*-nocrypt -DNO_CRYPT
#
Msg_IArg.hpp inc=SAPDB_Types.hpp
Msg_IOptArg.hpp inc=SAPDB_Types.hpp
Msg_IArg.cpp inc=Msg_IArg.hpp,Msg_IOptArg.hpp
Msg_SimpleArg.hpp inc=Msg_IArg.hpp,Msg_IOptArg.hpp
Msg_Arg.hpp  inc=Msg_IOptArg.hpp,Msg_IArg.hpp,SAPDB_ToString.hpp,SAPDB_MemCopyMoveNoCheck.hpp,SAPDB_string.h
Msg_Ucs2Arg.hpp inc=Msg_IArg.hpp,SAPDB_Types.hpp
# 
Msg_List.hpp inc=RTE_ISystem.hpp,SAPDB_Types.hpp,Msg_IArg.hpp
Msg_IRegistry.hpp inc=SAPDB_Types.hpp,Msg_List.hpp
Msg_Registry.hpp inc=SAPDB_Types.hpp,Msg_IRegistry.hpp
# Version without SAPDBErr_MessageList ...
# Msg_List.cpp inc=Msg_List.hpp,Msg_IArg.hpp,SAPDB_sprintf.h,SAPDB_string.h
Msg_List.cpp            inc=SDBMsg_Messages.h,SAPDB_sprintf.h,SAPDB_string.h,SAPDBMem_Alloca.h,SAPDBMem_NewDestroy.hpp,RTE_IInterface.hpp,RTEMem_Allocator.hpp,RTEMem_AllocatorRegister.hpp,Msg_Registry.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,Msg_SimpleArg.hpp
Msg_List-l.cpp -DLIBOMS inc=SDBMsg_Messages.h,SAPDB_sprintf.h,SAPDB_string.h,SAPDBMem_Alloca.h,SAPDBMem_NewDestroy.hpp,RTE_IInterface.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp,Msg_SimpleArg.hpp
Msg_IOutput.hpp inc=SAPDB_Types.hpp,RTE_ISystem.hpp,Msg_List.hpp
Msg_OutputFile.hpp inc=Msg_IOutput.hpp,Msg_List.hpp,RTESync_NamedSpinlock.hpp,RTETask_Yield.h,RTEMem_RteAllocator.hpp,heo07.h
Msg_OutputFile.cpp inc=Msg_OutputFile.hpp,Msg_List.hpp,Msg_FileProcessor.hpp,SAPDB_sprintf.h,RTESys_IO.h,SAPDB_Names.h,RTEMem_RteAllocator.hpp,SDBMsg_Messages.h
Msg_OutputKnlDiag.cpp   inc=Msg_OutputFile.hpp,heo07.h,RTEConf_ParameterAccessKernelInterface.hpp,KernelParam_RunTime.h,geo007_2.h,RTESys_Time.h,SAPDB_sprintf.h,hsp100.h,RTE_Speed.h,geo007_1.h,SAPDB_MemCopyMoveNoCheck.hpp,Msg_Arg.hpp,RTETask_Task.hpp,Msg_FileProcessor.hpp,geo60.h,SAPDB_Names.h,RTESys_AtomicOperation.h
Msg_OutputUtilProt.cpp  inc=Msg_OutputFile.hpp,heo07.h,RTEConf_ParameterAccessKernelInterface.hpp,KernelParam_RunTime.h,geo007_2.h,RTESys_Time.h,SAPDB_string.h,hsp100.h,RTE_Speed.h,geo007_1.h,SAPDB_Names.h
Msg_OutputApplDiag.cpp inc=Msg_OutputFile.hpp,SAPDB_string.h,gsp03,SAPDB_sprintf.h
Msg_OutputKnlDiag.h inc=SAPDB_C_Cplusplus.h
Msg_OutputUtilProt.h inc=SAPDB_C_Cplusplus.h

Msg_FileProcessor.hpp inc=RTESys_IO.h,SAPDB_sprintf.h,SAPDB_string.h,Msg_IOutput.hpp,Msg_OutputFile.hpp
Msg_FileProcessor.cpp inc=RTESys_IO.h,SAPDB_sprintf.h,SAPDB_string.h,SAPDB_Types.h,Msg_IOutput.hpp,Msg_OutputFile.hpp,Msg_FileProcessor.hpp,RTEMem_Allocator.hpp,SAPDB_MemCopyMoveNoCheck.hpp,SAPDB_Names.h,heo01.h
Msg_ProtConv.cpp inc=Msg_FileProcessor.hpp,RTEConf_ParameterAccessSimple.h,heo01.h,SAPDB_StandardArgumentParser.hpp,Msg_ProtFileInfo.hpp,KernelParam_RunTime.h,SAPDB_Names.h,hcn42.h,Kernel_Version.hpp,RTEConf_ParameterAccess.hpp
Msg_ProtFileInfo.cpp inc=Msg_ProtFileInfo.hpp,hcn42.h,KernelParam_RunTime.h,RTEConf_ParameterAccessSimple.h,SAPDB_Names.h
Msg_ProtFileInfo.hpp inc=Msg_IOutput.hpp

Msg_TestIOptArg.hpp inc=SAPDB_Types.hpp
Msg_TestIArg.hpp    inc=SAPDB_Types.hpp
Msg_TestArg.hpp     inc=Msg_TestIOptArg.hpp,Msg_TestIArg.hpp,SAPDB_ToString.hpp,SAPDB_MemCopyMoveNoCheck.hpp,SAPDB_string.h
Msg_TestList.hpp    inc=RTE_ISystem.hpp,SAPDB_Types.hpp,Msg_TestIArg.hpp
Msg_TestList.cpp    inc=Msg_TestList.hpp,SDBMsg_Messages.h,SAPDB_sprintf.h,SAPDB_string.h,SAPDBMem_Alloca.h,SAPDBMem_NewDestroy.hpp,RTE_IInterface.hpp,RTEMem_Allocator.hpp,RTEMem_AllocatorRegister.hpp,Msg_Registry.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
Msg_TestList-l.cpp -DLIBOMS inc=Msg_TestList.hpp,SDBMsg_Messages.h,SAPDB_sprintf.h,SAPDB_string.h,SAPDBMem_Alloca.h,SAPDBMem_NewDestroy.hpp,RTE_IInterface.hpp,SAPDBTrace_Topic.hpp,SAPDBTrace_Usage.hpp
