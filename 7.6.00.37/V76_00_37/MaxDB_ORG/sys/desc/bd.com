w# @(#)   desc/bd.com   7.2.0   23/07/99
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

*       inc=gsp00,ggg00,gbd00 trace=cccprint shrglob

regex(.*\.cpp$)   inc=

regex(g.*\.h$)    inc=+hta99.h
regex(.*\.cpp$)   inc=+hta99.h

*-d     noshrglob

&if $OMS_MODUL
*       %X-DOMS_MODUL 
&endif 
 
# ------  PASCAL / C  ------ 

vbd01       inc=+vsp002,gsp03,gsp03_3,vsp0031,ggg11,gkb00
vbd02       inc=+gsp03,gsp03_3,ggg11

vbd05       inc=+gsp03,gsp03_3,ggg11
vbd05.cpp   inc=gsp00,ggg00,gbd00,ggg11,gsp03,gsp03_3,\
                hbd01.h,hbd01_1.h,hbd05.h,hbd13.h,\
                hbd20_9.h,hbd30.h,hbd60.h,hbd61.h,hbd73.h,\
                hbd998.h,hgg01_1.h,hgg01_3.h,\
                Converter_PageCollector.hpp,\
                Converter_Version.hpp,\
                Converter_ICommon.hpp,\
                Data_Types.hpp,\
                Data_PageFrame.hpp,\
                Kernel_Trace.hpp,\
                Kernel_VTrace.hpp,\
                FrameCtrl_ICommon.hpp,\
                IOMan_IDataInfo.hpp,\
                RTE_ISystem.hpp,\
                SAPDB_RangeCode.hpp,\
                Trace_Entry.hpp,\
                Trans_Context.hpp

vbd06       inc=+gsp03,gsp03_3,ggg04
vbd07       inc=+gsp03,gsp03_3,ggg11
vbd08       inc=+vsp002
vbd09
vbd10       inc=+gkb00,vsp0031
vbd101      inc=+ggg00,gkb101
hbd101.h    inc=+ggg00,gkb101,-gbd00
vbd14       inc=+gkb00
hbd14.h     inc=ggg00
vbd17       inc=+gsp03,gsp03_3
hbd170.h    inc=gsp00,ggg00
vbd170.cpp  inc=hbd170.h,FileDir_Directory.hpp,FileDir_IDirectory.hpp,\
                FileDir_Common.hpp,\
                Kernel_IAdminInfo.hpp,\
                RTEMem_Allocator.hpp,hbd30.h,gbd00,\
                FileDir_Iterator.hpp,FileDir_Generic.hpp,\
                FileDir_Table.hpp,\
                SAPDBTrace_Usage.hpp,SAPDBTrace_Topic.hpp,\
                SAPDB_MemCopyMove.hpp,\
                Kernel_VTrace.hpp,\
                RTESys_MemoryBarrier.h,\
                RTE_Crash.hpp,\
                Msg_List.hpp,\
                hbd01_1.h,hbd03.h,ggg00,gsp00,gsp00_2.h
vbd20       inc=+gsp03,gsp03_3,vsp0031,ggg01,gbd02 
vbd21       inc=+gsp03,gsp03_3
vbd30       inc=+gsp03,gsp03_3,ggg11
vbd31       inc=+gsp03,gsp03_3
vbd360.cpp  inc=-hta99.h,+gsp00,ggg00,gbd00,hbd13.h,hbd13_3.h,\
                hbd20_2.h,hbd30.h,hbd31.h,hbd73.h,hbd360.h,hbd998.h,\
                hsp30.h,hkb53.h,hkb71.h,\
                CnsRead_CachedRow.hpp,\
                Converter_PageCollector.hpp,\
                Data_BasePage.hpp,\
                Data_Types.hpp,\
                Data_Row.hpp,\
                Data_PageFrame.hpp,\
                Data_IOBuffer.hpp,\
                FileDir_FileNo.hpp,\
                FrameCtrl_ICommon.hpp,\
                Kernel_FileIO.hpp,\
                Kernel_Common.hpp,\
                Kernel_Trace.hpp,\
                Kernel_VTrace.hpp,\
                IOMan_ClusterAddress.hpp,\
                IOMan_IDataInfo.hpp,\
                RTE_Message.hpp,\
                SDBMsg_Data.h,\
                SAPDB_RangeCode.hpp,\
                Trans_Context.hpp
vbd37       inc=+ggg03,ggg04,gsp03,gsp03_3
vbd50       inc=+gsp03,gsp03_3
vbd51       inc=+gsp03,gsp03_3
vbd53       inc=+gsp03,gsp03_3
vbd54       inc=+gsp03,gsp03_3
vbd55       inc=+gsp03,gsp03_3
vbd60.cpp   inc=gsp00,ggg00,gbd00,hbd06.h,hbd13.h,hbd17.h,\
                hbd20_9.h,hbd30.h,hbd60.h,hbd61.h,hbd998.h,\
                hgg01_1.h,gsp03,gsp03_3,hsp20.h,\
                Converter_Version.hpp,\
                Converter_IVerify.hpp,\
                Data_BasePage.hpp,\
                Kernel_FileIO.hpp,\
                RTE_Message.hpp,\
                SAPDB_RangeCode.hpp,\
                SDBMsg_Data.h,\
                Trans_Context.hpp
vbd61.cpp   inc=gsp00,ggg00,gbd00,hbd61.h,hbd13.h,hbd13_3.h,\
                hsp20.h,\
                Converter_PageCollector.hpp,\
                Data_Types.hpp,\
                Data_BasePage.hpp,\
                Data_PageNoList.hpp,\
                Data_PageFrame.hpp,\
                IOMan_ClusterAddress.hpp,\
                Kernel_FileIO.hpp,\
                Kernel_Common.hpp,\
                SAPDB_RangeCode.hpp,\
                Trans_Context.hpp
vbd72       inc=+ggg11
vbd73       inc=+vsp0031 trace=none


# ------  ------ 

hbd102.h        inc=gsp00,gbd00

# ------  C++  'g'-files ------ 

gbd05.h         inc=+hbd01_1.h,SAPDB_MemCopyMove.hpp,-hta99.h

# --- InvTree Interface ---

hbd03.h         inc=gsp00,ggg00,gbd00
vbd03.cpp       inc=+ggg01,ggg11,gbd300.h,gbd500.h,gbd600.h,hbd01.h,hbd01_1.h,hbd03.h,\
                     hbd13.h,hbd20_9.h,hbd30.h,hbd73.h,hbd300.h,hbd400.h,\
                     hbd401.h,hbd402.h,hbd403.h,hbd404.h,hbd405.h,hgg04.h,hsp30.h,\
                     hgg01_3.h,hbd998.h,\
                     Data_Types.hpp,\
                     SDBMsg_Data.h,\
                     SAPDB_RangeCode.hpp,\
                     Converter_Version.hpp,\
                     FileDir_IDirectory.hpp,\
                     FileDir_Table.hpp,\
                     FileDir_Index.hpp,\
                     Msg_List.hpp,\
                     FileDir_OmsKeyPartition.hpp,\
                     Kernel_VTrace.hpp,\
                     Kernel_IAdminRestartShutdown.hpp,\
                     Trans_Context.hpp,\
                     RTE_Message.hpp,\
                     Trace_Entry.hpp,\
                     SAPDBErr_Assertions.hpp,\
                     SAPDB_RangeCode.hpp

# --- InvTree Class ---

gbd400.h    inc=gbd00,gbd500.h,gbd600.h,gbd601.h

# --- InvCurrent Classes ---

gbd300.h        inc=+gbd05.h,gbd450.h,CnsRead_Manager.hpp
hbd300.h        inc=ggg00,gbd00
vbd300.cpp      inc=+gsp00,ggg00,gbd00,ggg11,gbd300.h,hsp20.h,hsp30.h,hbd01.h,hbd06.h,hbd06_1.h,\
                     hbd13.h,hbd20_9.h,hbd30.h,heo55k.h,hgg01_1.h,hgg01_3.h,Trace_Entry.hpp,\
                     FileDir_Index.hpp,\
                     FileDir_IDirectory.hpp,\
                     FileDir_ISharedDirectory.hpp,\
                     FileDir_Types.hpp,\
                     Trans_Context.hpp,\
                     Msg_List.hpp,\
                     Kernel_IAdminInfo.hpp,\
                     Kernel_Trace.hpp,\
                     RTE_Message.hpp

# --- InvList Classes ---

gbd450.h        inc=+ggg90.h,gsp03,gsp03_3,hbd102.h,hgg01_1.h,SAPDB_RangeCode.hpp,hsp30.h 

# --- InvListIterator Classes ---

gbd460.h        inc=+gbd600.h,gbd601.h,gbd450.h,gbd500.h,gbd510.h
vbd460.cpp      inc=+gbd460.h,heo51.h,heo56.h,-hta99.h,\
                    +RTE_Message.hpp,SAPDB_MemCopyMove.hpp,SAPDBErr_MessageList.hpp,SAPDBMem_NewDestroy.hpp

# --- Tree Class ---

gbd500.h    inc=+gbd300.h,gbd600.h,gbd601.h,gbd520.h,hgg01_3.h
gbd501.h    inc=+gsp03,gsp03_3,hgg01_1.h,gbd600.h
vbd500.cpp  inc=+gsp00,ggg00,gbd00,gbd500.h,gbd501.h,hbd13_3.h,hbd20_2.h,heo51.h,heo56.h,hgg01_1.h,hgg01_3.h,\
                gsp03,gsp03_3,hsp77.h,\
                Trace_Entry.hpp,\
                SAPDB_RangeCode.hpp,\
                Converter_PageCollector.hpp,\
                IOMan_ClusterAddress.hpp,\
                Data_IOBuffer.hpp,\
                Kernel_VTrace.hpp,\
                Data_Types.hpp

vbd510.cpp  inc=+gsp00,ggg00,gbd00,gbd500.h,gbd501.h,gbd520.h,gbd600.h,hbd102.h,hgg01_3.h
vbd520.cpp  inc=+gsp00,ggg00,gbd00,gbd500.h,gbd501.h,gbd600.h,heo60.h,hbd102.h,hgg01_1.h,hgg01_3.h,\
                SAPDB_RangeCode.hpp,\
				Data_Types.hpp
vbd530.cpp  inc=+gsp00,ggg00,gbd00,gbd500.h,gbd600.h

# --- PageChain Class ---

gbd550.h     inc=+hbd13.h,SAPDB_RangeCode.hpp,gsp03,gsp03_3,hgg01_1.h

# --- RecordIterator Class ---

gbd510.h    inc=+gbd600.h

# --- Separator Class ---

gbd520.h    inc=+hbd102.h,hsp30.h,SAPDB_RangeCode.hpp

# --- Node Class ---

gbd600.h    inc=+gsp03,gsp03_3,gbd300.h,hbd13.h,hgg01_1.h,gbd610.h,\
            SAPDB_RangeCode.hpp,\
            CnsRead_Manager.hpp,\
            Trans_Context.hpp,\
            SAPDBMem_IRawAllocator.hpp,\
            Data_PageNoList.hpp
gbd601.h    inc=gbd600.h
vbd600.cpp  inc=+gbd600.h,gbd500.h,gbd520.h,gbd450.h,hbd102.h,hbd06.h,hbd13.h,hbd20_2.h,\
                hbd30.h,hgg01_1.h,hgg01_3.h,hgg04.h,hsp30.h,\
                SAPDB_RangeCode.hpp,\
                Trans_Context.hpp,\
                Data_Types.hpp,\
                RTE_Message.hpp,\
                Kernel_VTrace.hpp,\
                Kernel_Trace.hpp,\
                Converter_PageCollector.hpp
vbd620.cpp  inc=+gsp00,ggg00,gbd00,gbd500.h,gbd600.h,gbd610.h,hbd102.h,hbd06.h,hgg01_1.h,\
                SAPDB_RangeCode.hpp,\
                Trans_Context.hpp,\
                RTE_Message.hpp,\
				Data_Types.hpp
gbd610.h    inc=ggg00,gbd00
vbd610.cpp  inc=gbd610.h,gsp00_2.h,\
                SAPDB_MemCopyMove.hpp,\
                SDBMsg_Index.h,\
                SAPDBTrace_Usage.hpp,\
                Data_Types.hpp,\
                Kernel_VTrace.hpp,\
                Kernel_Trace.hpp
                

# --- InvTree Routines (Class) ---

hbd400.h
vbd400.cpp      inc=+hgg01_1.h,hgg01_3.h,ggg01,gsp03_3,\
                     gbd400.h,gbd600.h,gbd601.h,gbd300.h,\
                     gbd450.h,gbd500.h,gbd510.h,gbd520.h,hbd102.h,\
                     hbd06.h,hbd17.h,hbd400.h,\
                     FileDir_ISharedDirectory.hpp,\
                     Converter_IVerify.hpp,\
                     Data_BasePage.hpp,\
                     Kernel_FileIO.hpp,\
                     Data_Types.hpp,\
                     RTETask_ITask.hpp,\
                     SAPDB_RangeCode.hpp,\
                     Kernel_OpMsg.hpp
hbd401.h
vbd401.cpp      inc=+gbd600.h,gbd520.h,gbd300.h,gbd450.h,gbd460.h,gbd500.h,\
                     hbd52.h,hbd72.h,hbd401.h,hgg01_1.h,hkb53.h,hkb71.h,\
                     hgg01_1.h,gsp03,gsp03_3,hgg01_1.h,gsp03,\
                     SAPDB_RangeCode.hpp,\
                     IOMan_ClusterAddress.hpp,\
                     Converter_PageCollector.hpp,\
                     Kernel_VTrace.hpp
hbd402.h
vbd402.cpp      inc=+gbd600.h,gbd300.h,gbd450.h,gbd460.h,gbd500.h,gbd510.h,gsp03_3,\
                     hbd02.h,hbd402.h,hkb71.h,hkb53.h,\
                     SAPDB_RangeCode.hpp,\
                     CnsRead_Manager.hpp,\
                     Data_Types.hpp,\
                     Data_IOBuffer.hpp

hbd403.h        inc=ggg00,gsp00
vbd403.cpp      inc=+gbd300.h,gbd460.h,gbd500.h,gbd510.h,gbd550.h,gbd600.h,ggg200.h,gsp03,gsp03_3,\
                     hbd06.h,hbd401.h,hbd403.h,hgg01_1.h,hsp30.h,\
                     SAPDBMem_IRawAllocator.hpp,\
                     SAPDB_RangeCode.hpp


hbd404.h        inc=+gbd300.h
vbd404.cpp      inc=+gbd460.h,gbd500.h,gbd510.h,gbd600.h,hbd30.h,hbd404.h,hkb71.h,hkb720.h,\
                     SAPDB_RangeCode.hpp,

hbd405.h        inc=+gbd300.h
vbd405.cpp      inc=+gbd490.h,gbd491.h,gbd494.h,gbd495.h,gbd600.h,gbd500.h,gbd510.h,\
                     gbd450.h,gbd460.h,gsp03,gsp03_3,ggg11,hbd13.h,hbd30.h,hbd400.h,\
                     hbd405.h,hgg01_1.h,\
                     Data_Exceptions.hpp,\
                     Data_Messages.hpp,\
                     RTE_Message.hpp,\
                     SAPDB_RangeCode.hpp,\
                     Trace_Entry.hpp
                    

# --- Create/Diagnose InvTree Class ---

gbd490.h        inc=-hta99.h,+gbd494.h,gbd496.h,gbd497.h,\
                    SrvTasks_JobList.hpp,\
                    SrvTasks_JobCreateIndex.hpp,\
                    RTE_Types.hpp,\
                    Kernel_Common.hpp
                   
vbd490.cpp      inc=-hta99.h,+gbd490.h,gbd491.h,gbd493.h,ggg11,hbd01.h,\
                     hbd03.h,hbd50.h,hgg01_3.h,hbd20_1.h,hbd30.h,\
                     Data_Exceptions.hpp,\
                     Data_Messages.hpp,\
                     SAPDB_RangeCode.hpp,\
                     Trace_Entry.hpp,\
                     KernelParam_DataAccess.hpp,\
                     RTE_Message.hpp,\
                     RTEConf_ParameterAccess.hpp,\
                     RTEConf_ParameterAccessKernelInterface.hpp,\
                     SrvTasks_JobCreateIndex.hpp,\
                     SrvTasks_WaitingTask.hpp,\
                     SrvTasks_JobList.hpp,\
                     IOMan_IDataInfo.hpp

gbd491.h        inc=-hta99.h,+gbd497.h,\
                     Kernel_Common.hpp
vbd491.cpp      inc=-hta99.h,+gbd300.h,gbd495.h,gbd491.h,gbd492.h,gbd493.h,gbd490.h,ggg200.h,\
                     hbd13.h,hbd30.h,hbd31.h,hbd35.h,hbd50.h,hbd400.h,hgg04.h,hsp30.h,\
                     SAPDB_RangeCode.hpp,\
                     Log_InvDescMap.hpp,\
                     CnsRead_CachedRow.hpp,\
                     Container_Vector.hpp,\
                     Data_Exceptions.hpp,\
                     Data_Messages.hpp,\
                     Kernel_VTrace.hpp,\
                     Kernel_Trace.hpp,\
                     RTE_Message.hpp,\
                     SAPDBFields_Field.hpp,\
                     SAPDBFields_FieldCollection.hpp,\
                     SAPDBMem_IRawAllocator.hpp,\
                     Data_Row.hpp

gbd492.h        inc=gbd493.h,\
                     Container_Vector.hpp,\
                     SAPDBFields_Field.hpp,\
                     Data_PageFrame.hpp
        
vbd492.cpp      inc=gbd492.h,\
                     FrameCtrl_ICommon.hpp,\
                     RTE_ISystem.hpp,\
                     SAPDBMem_IRawAllocator.hpp,\
                     SAPDB_MemCopyMoveNoCheck.hpp
                  
gbd493.h        inc=Data_PageFrame.hpp,\
                     Kernel_Common.hpp,\
                     RTESync_RWRegion.hpp
                    
vbd493.cpp      inc=gbd493.h,\
                     FrameCtrl_ICreateIndex.hpp,\
                     FrameCtrl_ICommon.hpp,\
                     RTEMem_Allocator.hpp,\
                     RTETask_ITask.hpp

gbd494.h        inc=-hta99.h,+gbd400.h,gbd500.h,gbd600.h,\
                     gbd495.h,gbd497.h,ggg200.h,\
                     Data_Types.hpp,\
                     Kernel_Common.hpp
                     
vbd494.cpp      inc=-hta99.h,+gbd494.h,hsp30.h,\
                    Trans_Context.hpp,\
                    Data_Exceptions.hpp,\
                    Data_Messages.hpp,\
                    Kernel_VTrace.hpp,\
                    RTE_Message.hpp,\
                    MemoryManagement/SAPDBMem_IRawAllocator.hpp,\
                    SAPDBMem_NewDestroy.hpp

gbd495.h        inc=-hta99.h,+gbd300.h,gbd450.h,gbd460.h,gbd500.h,gbd510.h,\
                    gbd550.h,gbd600.h,\
                    Kernel_Common.hpp
vbd495.cpp      inc=-hta99.h,+gbd495.h,hbd01.h,hbd400.h,\
                    SAPDB_RangeCode.hpp,\
                    Kernel_VTrace.hpp,\
                    SAPDBMem_NewDestroy.hpp

gbd496.h        inc=-hta99.h,gsp00,gbd00,+gbd495.h,gbd494.h,\
                    Container_Vector.hpp,\
                    Data_Types.hpp,\
                    Kernel_Common.hpp
vbd496.cpp      inc=-hta99.h,+gbd496.h,hbd01_1.h,\
                    Kernel_Trace.hpp,\
                    Kernel_VTrace.hpp,\
                    SAPDBMem_IRawAllocator.hpp,\
                    Trans_Context.hpp,\
                    Data_Exceptions.hpp,\
                    Data_Messages.hpp,\
                    RTE_Message.hpp

                   

gbd497.h        inc=-hta99.h

# --- OMS Container Classes ---
gbd90.h     inc=gsp00,FileDir_Types.hpp

gbd900.h    inc=ggg92.h,gbd00,gbd05.h,hbd01_1.h,hbd92.h,hgg01_1.h,\
                hgg01_3.h,gsp03,gsp03_3,heo57.h,\
                RTEMem_Allocator.hpp,\
                SAPDB_Types.hpp,\
                SAPDBTrace_Usage.hpp,\
                SAPDBTrace_Topic.hpp,\
                Kernel_OpMsg.hpp,\
                Container_Vector.hpp,\
                FileDir_OmsKey.hpp,\
                FileDir_OmsVar.hpp,\
                Kernel_VTrace.hpp,\
                FileDir_FileNo.hpp,\
                SAPDB_MemCopyMove.hpp,\
                hta99.h,hta01.h,hgg01_1.h

vbd900.cpp  inc=gbd900.h,gbd910.h,hgg01.h,hgg01_1.h,hgg06.h,\
                Kernel_OpMsg.hpp,\
                SAPDB_RangeCode.hpp,\
                SAPDB_MemCopyMove.hpp,\
                hta99.h

hbd902.h     inc=SAPDB_C_Cplusplus.h
vbd902.cpp   inc=hbd902.h,hgg01_3.h,\
				LVC_ContainerManager.hpp,\
				Catalog_Types.hpp,\
				Catalog_IAuthorization.hpp,\
				SQLMan_Context.hpp,\
				FileDir_FileNo.hpp,\
                FileDir_Oms.hpp,\
                Kernel_VTrace.hpp,\
				Container_Hash.hpp,\
				Kernel_IAdminInfo.hpp

gbd910.h    inc=ggg92.h,ggg01,gbd00,gbd900.h,hbd13.h,hbd13_1.h,hbd20_2.h,hbd92.h,hgg01_1.h,\
                gsp03,gsp03_3,hta99.h,hta01.h,\
                Kernel_VTrace.hpp,Kernel_OpMsg.hpp,\
                Msg_List.hpp,\
                FileDir_Oms.hpp,\
                FileDir_IDirectory.hpp,\
                Converter_ICommon.hpp
vbd910.cpp  inc=gbd900.h,gbd910.h,gbd930.h,hgg06.h,hkb67.h,\
				Kernel_IAdminInfo.hpp

hbd911.h	inc=gsp00,ggg00,FileDir_FileNo.hpp,RTESys_MemoryBarrier.h 
vbd911.cpp  inc=gbd05.h,gbd910.h,hbd01.h,hbd20_9.h,hbd911.h,hbd03.h,hbd22.h,hbd90.h,\
				hkb04.h,hkb50.h,hkb51.h,hkb560.h,hkb57.h,hkb67.h,heo56.h,hgg06.h,\
				FileDir_OmsKeyPartition.hpp,\
				Trans_Context.hpp,\
				Log_History.hpp,\
				Log_Transaction.hpp,\
				LVC_AbstractObjKeyIterator.hpp,\
				Kernel_IAdminInfo.hpp,\
				Kernel_VTrace.hpp,\
				RTESys_MemoryBarrier.h,\
				RTESync_RWRegion.hpp,\
				RTEMem_RteAllocator.hpp

gbd920.h    inc=gsp00,ggg00,ggg01,hgg08.h,\
                RTEMem_Allocator.hpp,\
                SAPDBMem_NewDestroy.hpp

gbd930.h    inc=gsp03,gsp03_3,ggg12,heo55k.h,heo56.h,\
                hgg01.h,hgg01_1.h,hgg01_3.h,hgg08.h,\
                gbd900.h,\
                Log_History.hpp,\
                FileDir_Oms.hpp,\
                RTEMem_Allocator.hpp,\
                SAPDBMem_NewDestroy.hpp,\
                LVC_WaitForDelObjFileQueue.hpp,\
                Trace_Entry.hpp,\
                GC_IGarbageCollector.hpp,\
                GC_IGCController.hpp,\
                Msg_List.hpp,\
                RTE_Crash.hpp,\
                SrvTasks_ICoordinator.hpp



vbd930.cpp  inc=ggg00,gbd05.h,gbd930.h,gbd900.h,gbd910.h,ggg91.h,hbd06.h,\
                hbd17.h,hbd21.h,hbd22.h,heo55k.h,heo56.h,SAPDB_RangeCode.hpp,hkb51.h,hkb53.h,\
                hkb57.h,hbd91.h,heo51.h,\
                Kernel_OpMsg.hpp,\
                Kernel_IAdminInfo.hpp,\
                Converter_ICommon.hpp,\
                Converter_Types.hpp,\
                Log_History.hpp,\
                RTESys_Time.h,\
                SAPDB_MemCopyMove.hpp,\
                Trans_Context.hpp,\
                RTESys_AtomicOperation.hpp,\
                RTETask_ITask.hpp,\
                RTETask_SuspendReason.h
                
# --- Node Classes ---

vbd13.cpp   inc=gsp00,\
                gsp03_3,\
                ggg01,\
                Converter_Version.hpp,\
                Converter_IPageNoManager.hpp,\
                Converter_ICommon.hpp,\
                Data_Types.hpp,\
                Data_BasePage.hpp,\
                Data_Exceptions.hpp,\
                Data_Messages.hpp,\
                Data_PageNoList.hpp,\
                Data_PageFrame.hpp,\
                FileDir_FileNo.hpp,\
                IOMan_Types.hpp,\
                IOMan_BlockAddress.hpp,\
                IOMan_ClusterAddress.hpp,\
                IOMan_ComplexClusterAddress.hpp,\
                IOMan_IDataIO.hpp,\
                IOMan_DataPages.hpp,\
                Kernel_FileIO.hpp,\
                Kernel_Common.hpp,\
                Kernel_VTrace.hpp,\
                SAPDB_ToString.hpp,\
                SAPDBTrace_Hex.hpp,\
                RTE_Message.hpp,\
                Trace_PageNoEntry.hpp,\
                Catalog_Interface.hpp,\
                RTE_Crash.hpp,\
                RTETask_ITask.hpp,\
                SAPDB_MemCopyMove.hpp,\
                Trans_Context.hpp
                 

vbd13.cpp   inc=+ggg00,ggg91.h,gbd00,gbd02,gbd490.h,hbd01.h,hbd01_1.h,hbd06.h,\
                hbd13.h,hbd13_2.h,hbd17.h,hbd20.h,hbd30.h,hbd73.h,hbd102.h,\
                hgg01_1.h,hgg01_3.h,hgg04.h,hgg17.h,ggg92.h,hgg06.h,\
                hsp30.h,heo55k.h,heo56.h,heo670.h,hta01.h,hta99.h

gbd201.h    inc=RTEMem_Allocator.hpp,\
                 Container_Vector.hpp,\
                 IOMan_DataPages.hpp,\
                 IOMan_Types.hpp,\
                 gbd02

vbd201.cpp   inc=+gbd201.h,\
                  hgg01.h,\
                  hgg04.h,\
                  hbd20_5.h,\
                  heo55k.h,\
                  hgg08.h,\
                 -hta99.h
                 

# ------  Headerfiles ------ 

hbd01.h         inc=gsp00,ggg00,gbd00
hbd01_1.h       inc=ggg00,gbd00
hbd01_2.h       inc=gsp00,ggg00,vsp002

hbd02.h         inc=-gbd00

hbd04.h         inc=gsp00,ggg00,ggg91.h,ggg92.h,\
                    FileDir_FileNo.hpp,\
                    FileDir_Types.hpp,\
                    Log_Types.hpp,\
                    LVC_Types.hpp,\
                    Data_Types.hpp
vbd04.cpp    inc=+gsp03,gsp03_3,ggg01,ggg11,ggg12,gbd00,gbd05.h,gbd900.h,hbd01.h,hbd04.h,hbd06.h,\
                    hbd22.h,hbd30.h,hbd90.h,hbd911.h,hbd92.h,hbd95_1.h,heo56.h,hgg01_1.h,hgg01_3.h,hkb50.h,\
                    hbd300.h,hkb53.h,heo58.h,hak341.h,\
                    Converter_ICommon.hpp,\
                    LVC_AbstractObjKeyIterator.hpp,\
                    LVC_SingleFileObjKeyIterator.hpp,\
                    LVC_MultiFileObjKeyIterator.hpp,\
                    LVC_InProcDispatcher.hpp,\
                    Trace_Entry.hpp,\
                    Kernel_VTrace.hpp,\
                    SAPDBMem_IRawAllocator.hpp,\
                    SAPDBMem_RawAllocator.hpp,\
                    SAPDBMem_NewDestroy.hpp,\
                    SAPDB_sprintf.h,\
                    hta99.h,hta01_2.h,\
                    FileDir_IDirectory.hpp,\
                    Msg_List.hpp,\
                    RTEMem_Allocator.hpp,\
                    RTEMem_BlockAllocator.hpp

hbd06.h         inc=-gbd00,+ggg04
hbd06_1.h       inc=ggg00         

hbd09.h         inc=-gbd00

hbd10.h         inc=+gkb00,-gbd00
hbd11.h         inc=gsp00,ggg00

hbd13.h         
hbd13_2.h        inc=-ggg00
hbd13_3.h        inc=-gbd00,+Data_PageNoList.hpp,Data_PageFrame.hpp

hbd17.h         

hbd20.h         inc=+gbd02
hbd20_1.h       inc=gsp00
hbd20_2.h       inc=
hbd20_3.h       inc=-ggg00
hbd20_4.h       inc=gsp00,ggg00
hbd20_5.h       inc=+gbd02
hbd20_6.h       inc=gsp00
hbd20_7.h       inc=-gbd00
hbd20_8.h       inc=
hbd20_9.h       inc=gsp00,ggg00
hbd20_10.h      inc=gsp00
hbd20_11.h      inc=gsp00_2.h


hbd21.h         inc=-gbd00

hbd22.h         inc=gbd00,ggg92.h,gbd900.h,\
                    FileDir_OmsKey.hpp,LVC_Types.hpp
                     
vbd22.cpp  inc=+ggg01,ggg12,gbd900.h,gsp03_3,hbd01.h,hbd03.h,hbd22.h,hbd90.h,hbd92.h,\
                     hbd911.h,hgg01_1.h,hgg01_3.h,Trace_Entry.hpp,Kernel_VTrace.hpp,\
                     Kernel_OpMsg.hpp,\
                     FileDir_IDirectory.hpp,\
                     FileDir_OmsKeyPartition.hpp,\
                     FileDir_IDirectory.hpp,\
                     Msg_List.hpp,\
					 RTESys_MemoryBarrier.h,\
                     SAPDB_MemCopyMove.hpp

hbd30.h
hbd360.h
hbd37.h         inc=ggg00,ggg03,gsp00

hbd52.h         

hbd60.h         
hbd61.h         inc=gsp00,gbd00,DataAccess/Data_PageFrame.hpp

hbd72.h         

hbd73.h         inc=-gbd00

hbd90.h         inc=ggg92.h,gbd00,gbd910.h,gbd05.h,gbd910.h,Log_Types.hpp,LVC_Types.hpp,\
                    LVC_AbstractObjKeyIterator.hpp 

vbd90.cpp  inc=+hbd90.h,gsp03,gsp03_3,ggg01,gbd05.h,hbd20_9.h,gbd900.h,gbd910.h,gbd930.h,\
                    gbd920.h,hbd06.h,hbd20_1.h,hbd22.h,hbd902.h,hbd91.h,hbd911.h,hbd92.h,hbd93.h,\
                    hbd95_1.h,hbd300.h,hgg01.h,hgg01_1.h,hgg06.h,hgg17.h,hkb50.h,hkb67.h,\
                    hkb53.h,\
                    Kernel_OpMsg.hpp,\
                    Kernel_VTrace.hpp,\
                    Kernel_IAdminInfo.hpp,\
                    Log_History.hpp,\
                    Log_Transaction.hpp,\
                    LVC_ErrorTraceObject.hpp,\
                    FileDir_OmsVar.hpp,\
                    Msg_List.hpp,\
                    Converter_ICommon.hpp,\
                    SAPDB_RangeCode.hpp,\
                    SDBMsg_ObjectContainer.h,\
                    Trans_Context.hpp,\
                    hta99.h,hta01.h

hbd91.h         inc=gbd900.h,gbd910.h,\
                    LVC_ErrorTraceObject.hpp,\
                    Log_UndoFile.hpp,\
                    GC_IGarbageCollector.hpp,\
                    FileDir_Iterator.hpp,\
                    FileDir_Types.hpp,\
                    Msg_List.hpp,\
                    LVC_Types.hpp
                    
vbd91.cpp  inc=hbd90.h,hbd91.h,gsp03,gsp03_3,gbd05.h,hbd20_9.h,gbd900.h,gbd910.h,gbd930.h,\
                    hbd01.h,hbd06.h,hbd22.h,hbd911.h,hbd92.h,hbd93.h,heo55k.h,\
                    ggg01,hgg01_1.h,hgg01_3.h,hgg11.h,gsp100.h,hkb53.h,hbd902.h,\
                    RTEMem_Allocator.hpp,\
                    Trace_Entry.hpp,\
                    Kernel_OpMsg.hpp,\
                    Kernel_VTrace.hpp,\
                    Kernel_IAdminInfo.hpp,\
                    SAPDBMem_NewDestroy.hpp,\
                    RTEConf_ParameterAccessKernelInterface.hpp,\
                    RTETask_ITask.hpp,\
                    FileDir_Iterator.hpp,\
                    FileDir_IDirectory.hpp,\
                    FileDir_Oms.hpp,\
                    FileDir_OmsVar.hpp,\
                    FileDir_OmsFixed.hpp,\
                    FileDir_OmsKey.hpp,\
                    FileDir_OmsKeyPartition.hpp,\
                    Msg_List.hpp,\
                    hta99.h,hta01.h,\
                    RTETask_ITask.hpp,\
                    RTETask_SuspendReason.h

hbd92.h         inc=ggg92.h,gsp00,gbd00,gbd05.h,heo670.h,FileDir_FileNo.hpp

vbd92.cpp inc=hbd92.h,gsp03,gsp03_3,ggg12,gbd05.h,gbd900.h,gbd910.h,gbd930.h,\
                    hbd06.h,hbd20_9.h,hbd22.h,hbd90.h,hgg01.h,hgg01_1.h,hgg04.h,hkb50.h,heo51.h,\
                    Converter_IVerify.hpp,\
                    Kernel_OpMsg.hpp,\
                    Kernel_IAdminInfo.hpp,\
                    Trace_Entry.hpp,\
                    Log_History.hpp,\
                    Msg_List.hpp,\
                    SAPDB_MemCopyMove.hpp,\
                    RTETask_ITask.hpp,\
                    hta99.h,hta01.h

hbd93.h         inc=gbd900.h,gbd910.h,Log_Types.hpp

vbd93.cpp  inc=gbd900.h,gbd910.h,gbd930.h,hbd93.h,gsp03,gsp03_3,gbd05.h,hbd91.h,\
                    hbd06.h,ggg01,heo66.h,hgg01_1.h,hgg01_3.h,hkb53.h,hkb67.h,hbd04.h,\
                    Kernel_OpMsg.hpp,\
                    LVC_Types.hpp,\
                    FileDir_Oms.hpp,\
                    FileDir_OmsVar.hpp,\
                    Msg_List.hpp,\
                    SAPDB_RangeCode.hpp,\
                    SAPDB_MemCopyMove.hpp,\
                    hta99.h

hbd95.h         inc=ggg92.h,gbd00,Log_Types.hpp,LVC_Types.hpp,Data_FileTypes.hpp,\
                    FileDir_Types.hpp
hbd95_1.h       inc=ggg00,gbd910.h,LVC_ErrorTraceObject.hpp
vbd95.cpp inc=+hbd95.h,hbd95_1.h,hbd04.h,gbd05.h,hbd20_9.h,gbd900.h,gbd910.h,\
                    ggg01,hbd06.h,hbd22.h,hbd90.h,hbd91.h,hbd92.h,hbd93.h,\
                    hkb67.h,hgg06.h,\
                    gbd930.h,\
                    LVC_ErrorTraceObject.hpp,\
                    Log_UndoFile.hpp,\
                    Data_FileTypes.hpp,\
                    Log_Transaction.hpp,\
                    Kernel_OpMsg.hpp,\
                    Kernel_IAdminInfo.hpp,\
                    RTE_Crash.hpp,\
                    Msg_List.hpp,\
                    SAPDB_RangeCode.hpp,\
                    Trans_Context.hpp,\
                    hta99.h,hta01.h
                     
# ------  Factory Problems ------ 

&if $OSSPEC in [ OSF1 ]
vbd02   -OPT=-O1
vbd20   -OPT=-O1
vbd30   -OPT=-O1
&endif

&if $OSSPEC == FREEBSD
vbd495.cpp   -OPT=-O2 
vbd93.cpp    -OPT=-O3 -march=pentium
&endif

&if $OSSPEC == LINUX && $MACH == I386 
# PTS 1106091 TS 2000-03-23
vbd495.cpp   -OPT=-O2 
# PTS 1111494 AK 2001-08-24
vbd93.cpp    -OPT=-O3 -march=pentium
&endif

hbd999_1.h inc=gsp00,ggg00,gbd00
hbd999.h inc=gsp00,ggg00,gbd00,gkb00
hbd998.h inc=ggg00,gsp00,gbd00

&if $MACH == HP_IA64
vbd92.cpp -OPT=+O0
&endif

&if $OSSPEC == LINUX && $MACH in [ SDBonPPC64 ]
vbd30      -OPT=-O0   # PTS 1131371 TS 2004-09-13
vbd405.cpp -OPT=-O1   # PTS 1131371 TS 2004-09-13
vbd50      -OPT=-O0   # PTS 1133972 TS 2005-02-16 
vbd52      -OPT=-O0   # PTS 1133972 TS 2005-02-16 
vbd13.cpp  -OPT=-O1   # PTS 1133972 TS 2005-02-16 
vbd20      -OPT=-O1   # PTS 1133972 TS 2005-02-16 
vbd402.cpp -OPT=-O0   # PTS 1139594 TS 2006-01-06
&endif
