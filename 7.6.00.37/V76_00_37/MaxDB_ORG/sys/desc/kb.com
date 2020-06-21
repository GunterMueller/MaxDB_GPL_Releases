# @(#)   desc/kb.com   7.4.0   03/21/01
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

*   inc=gsp00,ggg00,ggg04,gkb00 trace=cccprint shrglob

regex(^vkb[7].*$)   trace_step_out

#---------------------------------------------------------------------------------------
hkb03.h     inc=
hkb03_1.h   inc=gkb00

vkb04.cpp inc=hkb04.h,hbd04.h,hkb512.h,hbd90.h,ggg01,\
                hgg01_3.h,hkb05.h,hkb50.h,hkb51.h,\
                hkb67.h,hkb560.h,\
                Log_Transaction.hpp,\
                Log_ActionObject.hpp,\
                Log_ActionObjectFile.hpp,\
                SQLMan_Context.hpp,\
                Trace_Entry.hpp,\
                Kernel_OpMsg.hpp,\
                LVC_Types.hpp,\
                Kernel_VTrace.hpp,\
                FileDir_IDirectory.hpp,\
                FileDir_Generic.hpp,\
                FileDir_OmsKey.hpp,\
                Trans_Context.hpp,\
                Msg_List.hpp \
                noshrglob
hkb04.h     inc=ggg92.h,OMS_ContainerHandle.hpp,FileDir_Types.hpp

vkb05       inc=-gkb00,+ggg11,gsp03,gsp03_2
hkb05.h     inc=ggg00,gsp00

vkb101      inc=gkb101
gkb101      inc=+ggg07

hkb33.h     inc=ggg00

gkb03       inc=+vsp0031
vkb38       inc=+vsp002,vsp0031,gsp03,gsp03_2,gkb03
hkb38.h     inc=gsp00,ggg00
hkb38_1.h   inc=gsp00,ggg00
hkb38_2.h   inc=gsp00,ggg00,gsp03,gkb03
vkb39       inc=+vsp002,vsp0031,gsp03,gsp03_2,ggg11,gkb03
hkb39.h     inc=gsp00,ggg00,gkb00,gkb03
vkb391      inc=gsp00,ggg00,Log_Page.hpp,Kernel_IPage.hpp,gkb03
hkb391.h    inc=gsp00,ggg00,gkb00,gkb03
vkb391.cpp  inc=hkb391.h,\
                hkb03_1.h,\
                hkb57.h,\
                heo47.h,\
                hkb57_1.h,\
                SDBMsg_Backup.h,\
                SDBMsg_Log.h,\
                Log_SaveIterator.hpp,\
                Log_Page.hpp,\
                Log_Volume.hpp,\
                FrameCtrl_ICommon.hpp,\
                Kernel_Task.hpp,\
                Kernel_Trace.hpp,\
                Kernel_TaskWaitList.hpp,\
                Kernel_Version.hpp,\
                SAPDBMem_NewDestroy.hpp,\
                RTEConf_ParameterAccess.hpp,\
                RTEConf_ParameterAccessKernelInterface.hpp

vkb43       inc=+gkb05
hkb431.h    inc=gsp00,ggg00
vkb431.cpp  inc=SAPDB_Types.hpp,\
                SAPDBTrace_Usage.hpp,\
                SAPDB_MemCopyMoveNoCheck.hpp,\
                Kernel_Trace.hpp,\
                Log_ActionBlobCopy.hpp,\
                Log_ActionBlobCopySame.hpp,\
                Log_ActionBlobExpand.hpp,\
                Log_ActionBlobTruncate.hpp,\
                Log_ActionBlobUpdate.hpp,\
                Log_ActionCreateDropFile.hpp,\
                gsp00,ggg00,\
                hbd01.h,hbd01_1.h,hbd05.h,ggg92.h,hkb50.h,\
                hkb431.h,hta01.h

gkb05       dep=ggg92.h
vkb50       inc=+gsp03,gsp03_2,ggg01,gkb05
hkb50.h     inc=gsp00,ggg00,ggg92.h,gkb05
hkb50_1.h   inc=gsp00,ggg00,gkb05
hkb50_2.h   inc=-gkb00,ggg00,ggg04
vkb51       inc=+gsp03,gsp03_2,ggg11,gkb05,ggg92.h,vsp002
hkb51.h     inc=gsp00,ggg00,ggg92.h
hkb51_1.h   inc=gsp00,ggg00,gkb00
hkb51_2.h   inc=gsp00,ggg00,gkb05
hkb511.h    inc=ggg00,gsp00,gkb05
hkb512.h    inc=gsp00,ggg00
vkb511.cpp  inc=RTESync_RWRegion.hpp,RTEMem_Allocator.hpp,\
                SAPDBTrace_Streaming.hpp,\
                Kernel_Trace.hpp,\
                Kernel_VTrace.hpp,\
 				hkb511.h,hgg08.h,heo55k.h,gkb05
vkb512.cpp  inc=gsp00,ggg00,hkb512.h,\
                Trans_ExtendedMonitoring.hpp,\
                SAPDB_Types.hpp,\
                SAPDB_C_Cplusplus.h
gkb501.h    inc=RTESync_RWRegion.hpp,RTEMem_Allocator.hpp,gkb05,hsp77.h,\
                Container_List.hpp,\
                Container_Vector.hpp,\
                FileDir_FileNo.hpp
hkb501.h    inc=gsp00,ggg00,gkb05
hkb501_1.h  inc=ggg00,gsp00,FileDir_FileNo.hpp
vkb501.cpp  inc=gkb501.h,hkb501.h,ggg01,hgg01.h,hgg08.h,heo55k.h,heo56.h,\
				ggg92.h,hgg01_1.h,hgg01_3.h,gsp03_2,hkb50.h,\
				hkb50_1.h,hkb51.h,hkb51_2.h,hkb52.h,hkb501_1.h,\
                SAPDBTrace_Streaming.hpp,\
                Kernel_IAdminInfo.hpp,\
                FileDir_FileNo.hpp,\
                Kernel_OpMsg.hpp,\
                Kernel_Trace.hpp,\
                Kernel_VTrace.hpp
hkb509.h    inc=                
vkb52       inc=+gsp03,gsp03_2,gkb05
hkb52.h     inc=+gkb05,-gkb00,ggg04
hkb52_1.h   inc=-gkb00,ggg04
vkb521.hpp  inc=ggg00,ggg91.h,gkb05,vak001,\
                SAPDB_Types.hpp,\
                Container_List.hpp,\
                SAPDBMem_IRawAllocator.hpp,\
                SAPDBMem_NewDestroy.hpp
vkb521.cpp  inc=ggg00,ggg91.h,gkb05,vak001,hkb52.h,\
                hkb51_2.h,hgg06.h,\
                SAPDB_RangeCode.hpp,\
                Container_List.hpp,\
                SAPDBMem_IRawAllocator.hpp,\
                SAPDBMem_NewDestroy.hpp,\
                vkb521.hpp
vkb522.hpp  inc=ggg00,ggg91.h,gkb05,vak001,\
                SAPDB_Types.hpp,\
                Container_List.hpp,\
                SAPDBMem_IRawAllocator.hpp,\
                SAPDBMem_NewDestroy.hpp
vkb522.cpp  inc=ggg00,ggg91.h,gkb05,vak001,hkb52.h,\
                hkb51_2.h,hgg06.h,\
                Container_List.hpp,\
                SAPDBMem_IRawAllocator.hpp,\
                SAPDBMem_NewDestroy.hpp,\
                vkb522.hpp
vkb53       inc=+ggg11,gkb05
hkb53.h     inc=gsp00,ggg00
hkb53_1.h   inc=gsp00,ggg00,gkb00
hkb54.h     inc=ggg00
hkb560.h    inc=gsp00,ggg00,gkb00
vkb560.cpp  inc=hkb560.h,hgg01_3.h,\
                heo56.h,heo58.h,hbd91.h,\
                hkb57_1.h,hkb57.h,vsp0031,\
                SAPDB_ToString.hpp,\
                SAPDB_RangeCode.hpp,\
                SAPDBTrace_Usage.hpp,\
                SAPDBErr_Assertions.hpp,\
                SAPDBMem_IRawAllocator.hpp,\
                SAPDBMem_NewDestroy.hpp,\
                RTEMem_Allocator.hpp,\
                RTE_Message.hpp,\
                RTE_Crash.hpp,\
                Kernel_DateTime.hpp,\
                Kernel_Trace.hpp,\
                Log_Volume.hpp,\
                Log_Transaction.hpp,\
                Log_Savepoint.hpp,\
                Log_History.hpp,\
                Log_Exceptions.hpp,\
                IOMan_ILogInfo.hpp,\
                SQLMan_Context.hpp,\
                Catalog_Interface.hpp,\
                Trans_Context.hpp
vkb57       inc=+gsp03,gsp03_2
hkb57.h     inc=-ggg04
hkb57_1.h   inc=gkb00

hkb571.h    inc=gsp00,ggg00
vkb571.cpp  inc=Log_ActionRefreshSurrogSeq.hpp,\
                Kernel_TaskWaitList.hpp,\
                Kernel_Migration.hpp,\
                gsp00,ggg00,\
                hkb571.h

vkb61       inc=+ggg07

hkb611.h    inc=gsp00,ggg00

vkb611.cpp  inc=SAPDB_Types.hpp,\
                SAPDBMem_IRawAllocator.hpp,\
                SAPDBMem_NewDestroy.hpp,\
                SAPDB_MemCopyMove.hpp,\
                Log_CommandInsDelUpd.hpp,\
                Log_InvDescMap.hpp,\
                Log_ActionCreateDropFile.hpp,\
                Log_ActionCreateIndexFile.hpp,\
                Log_ActionUnqualifiedDelete.hpp,\
                Log_Types.hpp,\
                FileDir_Table.hpp,\
                FileDir_FileNo.hpp,\
                FileDir_IDirectory.hpp,\
                SQLMan_Context.hpp,\
                Container_Vector.hpp,\
                Catalog_Cache.hpp,\
                Trans_Context.hpp,\
                SAPDB_AutoPtr.hpp,\
                gsp00,ggg00,hkb611.h,\
                hak01.h,hkb64.h,hkb62.h,hkb720.h,hbd73.h,hbd01.h,hbd01_1.h,\
                heo52.h,heo69.h,hgg01_3.h,hgg04.h,hsp41.h,hkb641.h

hkb62.h     inc=ggg00
vkb62       inc=+vsp008
vkb64       inc=+gsp03,gsp03_2
hkb64.h     inc=gsp00,ggg00


hkb641.h    inc=gsp00,ggg00

vkb641.cpp      inc=SAPDB_Types.hpp,\
                Log_ActionFile.hpp,\
                Log_ActionCreateDropFile.hpp,\
                Log_ActionCreateTable.hpp,\
                Log_ActionCreateIndexFile.hpp,\
                Log_ActionCreateShortColumnFile.hpp,\
                Log_ActionReorganizeFile.hpp,\
                Log_InvDescMap.hpp,\
                Log_Transaction.hpp,\
                SQLMan_Context.hpp,\
                gsp00,ggg91.h,ggg00,\
                hkb641.h,hbd01.h,hkb560.h

vkb66       inc=+vsp008

hkb661.h    inc=gsp00,ggg00

vkb661.cpp  inc=SAPDB_Types.hpp,\
                Log_ActionModifyTable.hpp,\
                Log_ActionReassignSecondaryFile.hpp,\
                Log_ActionSetFileAttributes.hpp,\
                Log_InvDescMap.hpp,\
                Trans_Context.hpp,\
                gsp00,ggg00,\
                hkb661.h

hkb67.h     inc=FileDir_FileNo.hpp,\
                Log_Types.hpp
vkb67.cpp   inc=hkb67.h,gsp03,gsp03_2,ggg01,heo55k.h,\
                hgg01_1.h,hgg08.h,\
                hkb50.h,hkb51.h,hkb52.h,hkb560.h,hsp77.h,\
                Kernel_IAdminInfo.hpp,\
                Kernel_OpMsg.hpp,\
                Kernel_VTrace.hpp,\
                RTE_Message.hpp,\
                Log_Transaction.hpp,\
                Log_BeforeImage.hpp,\
                Log_History.hpp,\
                Log_ActionObject.hpp,\
                Log_ActionObjectFile.hpp,\
                SQLMan_Context.hpp,\
                hta99.h,hta01.h,hta01_2.h noshrglob

<  

gkb07       inc=-gkb00,+ggg07
hkb71.h     inc=gsp00,ggg00,gkb07
vkb71       inc=-gkb00,+ggg07,gkb07,ggg01,gak104
vkb720      inc=-gkb00,+ggg07,gkb07
hkb720.h    inc=-gkb00,ggg04,+ggg07
vkb721      inc=-gkb00,+ggg07,gkb07,gsp03,gak104 
hkb721.h    inc=-gkb00,ggg04,+ggg07
vkb722      inc=-gkb00,+ggg07,gkb07
vkb73       inc=-gkb00,+ggg07,gkb07
vkb74       inc=-gkb00,+ggg07,gkb07,gbd00,gsp03
hkb74.h     inc=-gkb00,ggg04,+gkb07,gkb07,ggg07
vkb741      inc=-gkb00,+ggg07,gbd00,gsp03
hkb742.h    inc=-gsp00,gkb00,ggg04,+ggg07
vkb742      inc=-gsp00,gkb00,ggg04,+ggg07
vkb75       inc=-gkb00,+ggg07,gkb07
vkb76       inc=-gkb00,+ggg07,gkb07
vkb78       inc=-gkb00,+vsp006,vsp008,ggg07,gkb07
vkb79       inc=-gkb00,+vsp006,ggg07,gkb07

vkb900      inc=gsp00,gkb00,ggg00,ggg04
hkb900.h    inc=gsp00,ggg00
hkb90.h     inc=gsp00,ggg00
# ---------------------------------------- 

&if $MACH in [ HP9 ]
vkb57   -OPT=+O1
vkb90   -OPT=+O1
vkb91   -OPT=+O1
vkb92   -OPT=+O1
vkb93   -OPT=+O1
vkb94   -OPT=+O1
vkb95   -OPT=+O1
vkb96   -OPT=+O1
&endif

&if $OSSPEC = OSF1
vkb71   -OPT=-O1
&endif

&if $MACH in [ SUN ] || $OSSPEC == SOLARIS
vkb74   -OPT=-xO1
&endif

&if $OSSPEC == LINUX && $MACH in [ S390X, IA64 ] && $COMPILER != ICC
vkb57 -OPT=-O0
regex(\w+\.cpp) -OPT=-O0
&endif

&if $OSSPEC == LINUX && $MACH in [ SDBonPPC64 ]
vkb39 -OPT=-O0
&endif
