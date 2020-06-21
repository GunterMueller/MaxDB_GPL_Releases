# @(#)   desc/gg.com   7.2.0   07/16/98
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

* inc=gsp00,ggg00 trace=none [s trace=cccprint shrglob s]

# ------  PASCAL / C / C++ ------

ggg00	 inc=gsp00 dep=ggg90.h,ggg91.h
ggg00_trace.h inc=+SAPDB_OStream.hpp
ggg07    inc=+ggg04
ggg12	 dep=ggg92.h

# ------  PASCAL / C  ------
vgg01	 inc=+vsp002,vsp0011,gsp03,vsp0031,vsp0058,ggg04,ggg07   shrglob
#vgg01gw  inc=gsp00,ggg00		                     shrglob
vgg02	 inc=+ggg04				                     shrglob
vgg03    inc=+vsp006,ggg04                           shrglob
hgg03.h  inc=+vsp006,ggg04
vgg04    inc=+gsp03,vsp008,ggg04,ggg07,gak104        shrglob trace_step_out
vgg041   inc=+gsp03,vsp008,ggg04,ggg07               trace=none
vgg08    inc=+gsp03                                  shrglob
vgg17       inc=+vsp001,vsp002,ggg01,ggg04,ggg07,gbd00,gbd02
vgg172   inc=+gbd02
vgg172.cpp inc=SAPDB_Types.h,SAPDB_ToString.hpp,FileDir_FileNo.hpp,\
               SAPDB_RangeCode.hpp,\
               gbd00,hgg172.h
hgg02.h     inc=+ggg04
hgg17.h		inc=gsp00,ggg00
hgg17_1.h	inc=gbd00
hgg17_2.h	inc=vsp001
hgg17_3.h	inc=ggg07
hgg17_4.h	inc=gbd02
hgg17_5.h
vgg17-k  inc=+vsp001,vsp002,ggg01,ggg04,ggg07,gbd00,gbd02  KERNEL shrglob

hgg171.h    inc=ggg00
hgg172.h    inc=gsp00,gbd00

vgg20    inc=+gsp03,vsp008                           shrglob
hgg20.h  inc=+vsp008,-ggg00
vgg51    inc=gsp00 [s trace=none s]                  shrglob
vgg53    inc=gsp00 [s trace=none noshrglob s]
vgg54    inc=gsp00 [s trace=none noshrglob s]
vgg55    inc=gsp00 [s trace=none noshrglob s]

# ------  C++  ------
hgg01.h     inc=+ggg04
hgg01_1.h	inc=gsp00,gsp03
hgg01_2.h   inc=vsp0031
hgg01_3.h   inc=gsp00,ggg00,ggg04

hgg011.h inc=ggg00
vgg011.cpp  inc=RTEConf_ParameterAccessKernelInterface.hpp,\
    KernelParam_Pager.hpp,hgg01_3.h,\
    hak01.h,hak10.h,hak101.h,hak341.h,hak545.h,hbd20_8.h,hbd03.h,gbd490.h,\
    hkb03_1.h,heo58.h,hsp20.h,\
    KernelParam_Kernel.hpp,\
    KernelParam_Join.hpp,\
    KernelParam_QueryRewrite.hpp,\
    KernelParam_SharedSQL.hpp,\
    KernelParam_DataAccess.hpp,\
    KernelParam_DataCache.hpp,\
    SAPDBDBE_KernelParameterNames.hpp,\
    SAPDB_MemCopyMove.hpp,\
    Kernel_TraceTopics.hpp,\
    Kernel_OpMsg.hpp,\
    KernelDBE_CallbackHandler.hpp,\
    LVCMem_Allocator.hpp,\
    Join_HashAccessAllocator.hpp,\
    SQLMan_UpdateStatistics.hpp,\
    SAPDBDiag_Topic.hpp,\
    SQLMan_Connect.hpp,\
    SharedSQL_SQLCache.hpp,\
    Log_Volume.hpp

hgg04_1.h   inc=+ggg07
hgg06.h		inc=gsp00,ggg00
vgg06*.cpp	        inc=hgg06.h,ggg92.h,hta99.h,hta01.h,hgg17.h,\
                        SAPDB_RangeCode.hpp,\
                        SAPDB_Types.h,\
                        SAPDBAlgo_FNVHash.hpp,\
                        Log_Types.hpp,\
                        SrvTasks_JobTypes.hpp,\
                        RTETask_ITask.hpp,\
                        SAPDB_ToString.hpp \
                        noshrglob
vgg06-slowci.cpp -DSLOWCI
vgg06-dbmcli.cpp -DDBMCLI
hgg08.h		inc=gsp00
hgg11.h		inc=gsp00
vgg11.cpp   inc=gsp00,gsp100.h,hsp100.h,hgg11.h
vgg11-t.cpp -DOMSTST inc=gsp00,gsp100.h,hsp100.h,hgg11.h
ggg90.h		inc=gsp00,gsp00_1.h,geo47.h
vgg90.cpp	inc=ggg90.h noshrglob
ggg91.h		inc=ggg90.h
vgg91.cpp	inc=ggg91.h,ggg00,hgg06.h,SrvTasks_JobTypes.hpp noshrglob
ggg92.h		inc=geo00.h,ggg00
vgg92.cpp	inc=ggg92.h,hgg17.h noshrglob
ggg200.h	inc=gsp00
ggg250.h    inc=ggg00,OMS_IterDirection.hpp
ggg251.h    inc=
vgg200.cpp  inc=ggg200.h
hgg941.h    inc=ggg00,gsp00
vgg941.cpp  inc=SAPDB_Types.hpp,\
                SAPDBMem_RawAllocator.hpp,\
                RTEMem_BlockAllocator.hpp,\
                RTE_Crash.hpp,\
                RTEConf_ParameterAccessKernelInterface.hpp,\
                SAPDBTrace_Usage.hpp,\
                RTETask_ITask.hpp,\
                SAPDBTrace_Topic.hpp,SAPDBTrace_Streaming.hpp,\
                hgg941.h,heo51.h,hgg01_1.h,gsp03_1,hsp77.h -except
hgg942.h    inc=gsp00,ggg00
vgg942.cpp	inc=gsp00,ggg00,hgg942.h,hgg999.h,ggg92.h,hgg06.h,\
                Data_Test.hpp,\
                Kernel_Test.hpp,\
                Kernel_IAdminRestartShutdown.hpp,\
                Log_Types.hpp,\
                Log_Test.hpp,\
                Log_Transaction.hpp,\
                Log_Queue.hpp,\
                Log_Volume.hpp,\
                Log_AfterImageSpace.hpp,\
                Table_Test.hpp,\
                SAPDBFields_FieldList.hpp,\
                SAPDBAlgo_Test.hpp,\
                SDBMsg_DIAGNOSE.h,\
                SQLMan_Test.hpp,\
                SAPDBErr_Assertions.hpp,\
                SAPDBMem_NewDestroy.hpp,\
                FileDir_Test.hpp,\
				SAPDB_UTF8Basis.hpp,\
                SAPDB_RangeCode.hpp,\
                Kernel_VTrace.hpp,\
                Kernel_Trace.hpp,\
                Kernel_Version.hpp,\
                RTE_Test.hpp,\
                RTE_MySQLProxy.hpp,\
                RTESys_MicroTime.h,\
                Catalog_Test.hpp,\
                CnsRead_Test.hpp,\
                RTETask_ITask.hpp,\
                Trans_Context.hpp,\
                Rst_RedoFile.hpp,\
                SrvTasks_Test.hpp,\
                Trans_Iterator.hpp,\
                SAPDBMem_Test.hpp \
                [s dep=hta99.h s]
hgg999.h    inc=gsp00,ggg00
vgg999.cpp  inc=hgg999.h,\
                hgg02.h,\
                hsp41.h,hbd91.h,\
                Kernel_Administration.hpp,\
                Kernel_IAdminRestartShutdown.hpp,\
                Kernel_IAdminHotStandby.hpp,\
                Kernel_IAdminBackupRecovery.hpp,\
                Kernel_IAdminInfo.hpp,\
                Kernel_IAdminConfig.hpp,\
                Kernel_Version.hpp,\
                Kernel_Exception.hpp,\
                Kernel_Messages.hpp,\
                Kernel_Trace.hpp,\
                Kernel_Exception.hpp,\
                Kernel_Task.hpp,\
                Trans_Context.hpp,\
                IOMan_BlockAddress.hpp,\
                RTEMem_Allocator.hpp,\
                RTETask_ITask.hpp,\
                Log_Types.hpp,\
                Trace_Writer.hpp,\
                SQLMan_Context.hpp,\
                SAPDB_RangeCode.hpp,\
                SDBMsg_Admin.h,\
                SAPDB_sprintf.h,\
                SAPDB_string.h


vgg12.cpp  inc=+SAPDB_Types.hpp,gsp00_2.h,hgg941.h,hgg12.h,SAPDBTrace_Usage.hpp,\
            SAPDBTrace_Topic.hpp,SAPDBTrace_Streaming.hpp\

&if $OSSPEC == FREEBSD
vgg17           -CFLAGS=-ffloat-store
vgg17-k         -CFLAGS=-ffloat-store
&endif

&if $OSSPEC == LINUX && $MACH == I386
vgg17           -CFLAGS=-ffloat-store
vgg17-k         -CFLAGS=-ffloat-store
&endif


