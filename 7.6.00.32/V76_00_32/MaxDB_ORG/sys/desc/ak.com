# @(#)   desc/ak.com   7.2.0   07/16/98
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

*       inc=gsp00,vsp001,ggg00,ggg04,gak00,vak001 trace=cccprint shrglob

regex(^vak[0123456789].*$)   trace_step_out

*-d     noshrglob


&if $MACH == SUN || $OSSPEC == SOLARIS
# is this only SPARC specific? [JnZ]
vak06   -NO
vak22   -NO
&endif

&if $MACH = _IBMR2
# PTS 1108060
vak42 -NO
vak681 -NO
&endif

&if $OSSPEC = OSF1
vak11   -OPT=-O0
vak681  -OPT=-O1
vak720  -OPT=-O0
vak542  -OPT=-O0
&endif

&if $OSSPEC = FREEBSD
vak58   -NO
&endif

&if $OSSPEC = LINUX
# PTS 1114151
&if $COMPILER != ICC
vak58   -NO
&else
vak58
&endif
&endif


&fast

&if $OSSPEC in [ WIN32 ]
vak16   -NO -Od
# vak70, weil sonst in ak70or_qual die Zuweisung
# orqual_term := orqual_term + andqual_term - orqual_term - andqual_term
# im fasten Kern nicht ganz korrekte Ergebnisse liefert, siehe EXPL.vdnts
vak70	-NO -Od
&endif

&endfast

&if $MACH in [ HP9 ]
vak22   -OPT=+O1
vak23   -OPT=+O1
vak38   -OPT=+O1
vak70   -OPT=+O1
vak71   -OPT=+O1
vak720  -OPT=+O1
vak721  -OPT=+O1
vak723  -OPT=+O1
vak724  -OPT=+O1
vak726  -OPT=+O1
&endif

hak01.h  inc=gsp00,vsp001,ggg00,vak001
hak05.h  inc=-vsp001,ggg04
hak06.h  inc=ggg04,gsp00,vsp001,ggg00,gak00,vak001
hak061.h inc=gsp00,ggg00,gak00,vak001
hak07.h  inc=gsp00,ggg00,ggg04,vak001,vsp008
hak071.h inc=gsp00,ggg00
hak10.h  inc=gsp00,ggg00,gak00,vak001,gak40
hak101.h inc=gsp00,vsp001,ggg00,vak001,gak40
hak103.h inc=gsp00,ggg00,vak001,gak40,gak00
hak12.h  inc=-vsp001,ggg04
hak24.h  inc=-vsp001,ggg04
hak28.h  inc=-vsp001,ggg04,+gak40
hak34.h  inc=gsp00,vsp001,ggg00,ggg01,vak001
hak35.h  inc=ggg00,vak001
hak36.h  inc=gsp00,ggg00,vak001
hak36_1.h inc=RTE_CompilerFeatures.h
hak37.h  inc=gsp00,ggg00,vak001,vsp0031,gak30
hak260.h inc=-vsp001,gak00,-ggg04

hak341.h inc=ggg00,ggg01,ggg92.h,vak001,hkb04.h,hak34.h,hgg08.h,heo55k.h
hak262.h inc=gsp00,ggg00,vak001,vsp001
hak263.h inc=OMS_Defines.h,vak001
hak362.h inc=gsp00,ggg00,vak001
hak40.h  inc=-vsp001,ggg04,+gak40,gak71
hak42.h  inc=gsp00,ggg00,vak001,gak40
hak43.h  inc=gsp00,vak001,gak40
hak47.h  inc=gak00,vak001,gak40
hak51.h  inc=gsp00,ggg00,vak001
hak542.h inc=gsp00,vsp001,ggg00,vak001
hak545.h inc=gsp00,ggg00,vak001
hak663.h inc=-gak00,-ggg04,-vsp001
hak664wrapper.h inc=ggg00,vak001
hak651.h inc=gsp00,ggg00,vak001
hak91.h  inc=ggg00,vak001
hak93.h  inc=gsp00,vsp001,ggg00,vak001
hak99.h  inc=gsp00,ggg00,gak00,vak001
hak102.h inc=vak001
hak687.h inc=gsp00,ggg00
hak722.h inc=gsp00,ggg00,vak001,ggg07,gak71,gak40


vak01   inc=+vsp008
vak01-80 inc=+vsp008 KERNEL80=1
vak011  inc=+vsp008
vak04   inc=+ggg07,gak40,gak68,gak70
vak05   inc=+vsp006,vsp008,gak104
vak06   inc=+ggg11,gsp03,gsp03_1,vsp008,ggg07,gak104
vak07   inc=+gsp03,gsp03_1,vsp008,vsp0031,ggg01
vak09   inc=+vsp007,gbd00,ggg03
vak10   inc=+gsp03,gsp03_1,ggg07,gak40,gak68,gak104
vak10-80 inc=+gsp03,gsp03_1,ggg07,gak40,gak68 KERNEL80=1
vak101  inc=+gak40
vak102.cpp -except inc=hak102.h,gsp03_1,ggg04,hta01_3.h,hak10.h,vak001,hsp77.h,hgg01_1.h,SAPDB_MemCopyMove.hpp
vak103  inc=+gak40
vak12   inc=+vsp008,ggg01
vak16   inc=+ggg07,gak68,gsp03

&fast
&if $COMPILER != ICC
vak11   -NO
&else
vak11
&endif
&endfast
vak111  inc=+ggg07
vak13   inc=+vsp008,ggg07,ggg03
vak14   inc=+vsp008
&if $MACH in [PA20W]
vak14   -NO
&endif
vak15   inc=+ggg07,gak40
&if $COMPILER != ICC
vak17   inc=+vsp007,vsp008,ggg03  -NO
&else
vak17   inc=+vsp007,vsp008,ggg03
&endif
vak21   inc=+vsp008,ggg07,gak40,ggg01
&fast
&if $OSSPEC in [ SOLARIS, SVR4 ]
vak22   -NO
&endif
&endfast
vak24   inc=+ggg01,ggg07,gak40,gsp03,gsp03_1
vak25   inc=+vsp008
vak26   inc=+vsp008
vak260  inc=+vsp008,gsp03,gsp03_1,ggg01
vak260-t inc=+vsp008,gsp03,gsp03_1,ggg01 OMSTST=1
vak262  inc=+gsp03,gsp03_1,vsp008,ggg01
vak263.cpp inc=vak001,hak263.h,hak12.h,hak34.h,hak061.h,hak07.h,hgg01_3.h,hgg17.h,\
           IFRUtil_VDNNumber.h,OMS_Types.hpp,SAPDB_MemCopyMove.hpp noshrglob dep=OMS_LiveCacheTypes.xml -DOMS_IMPLEMENTATION \
           -DIFR_NAMESPACE=SQLDBC_Kernel -DSQLDBC=SQLDBC_Kernel -DSQLDBC_RUNTIME_IN_NAMESPACE
vak28   inc=+vsp0031,ggg07,gak40,gak68
vak34   inc=+gsp03,gsp03_1,vsp008,ggg01   NOSIM=1
vak341.cpp inc=vak001,ggg01,hak01.h,hak05.h,hak07.h,hak34.h,hak37.h,hak341.h,hak40.h,hak51.h,hgg01.h,hgg01_1.h,hgg01_3.h,hkb04.h,hbd04.h,OMS_Defines.h,hgg04.h,\
hbd01.h,hbd02.h,heo01.h,heo02x.h,heo58.h,heo78.h,heo38.h,heo66.h,LVC_IliveCacheSink.hpp,\
heo670.h,heo15.h,gsp03_1,hsp26.h,hsp40.h,hsp77.h,gak40,heo00.h,hgg11.h,OMS_DumpInterface.hpp,OMS_TraceInterface.hpp,\
LVCMem_Allocator.hpp,LVC_KernelInterfaceInstance.hpp,LVC_LibOmsInterface.hpp,LVC_LoadLibrary.hpp,\
LVCMem_EmergencyAllocator.hpp,Kernel_OpMsg.hpp,Trace_Entry.hpp,SQLMan_Context.hpp,SysView_PseudoLongHandler.hpp,RTEConf_ParameterAccessKernelInterface.hpp,\
SAPDB_UTF8Basis.hpp,SAPDB_RangeCode.hpp,SAPDB_MemCopyMove.hpp,PIn_Packet.h,Kernel_TraceTopics.hpp \
noshrglob -DOMS_IMPLEMENTATION dep=OMS_IliveCacheSpec.hpp
vak341-t.cpp -DOMSTST inc=vak001,hak01.h,hak05.h,hak07.h,hak34.h,hak37.h,hak341.h,hak51.h,hak40.h,hgg01.h,hgg01_1.h,hgg01_3.h,hkb04.h,hbd04.h,OMS_Defines.h,hgg04.h,\
hbd01.h,hbd02.h,heo01.h,heo02x.h,heo58.h,heo78.h,heo38.h,heo66.h,LVC_IliveCacheSink.hpp,ggg01,\
heo670.h,heo15.h,gsp03_1,hsp26.h,hsp40.h,hsp77.h,gak40,heo00.h,hgg11.h,OMS_DumpInterface.hpp,OMS_TraceInterface.hpp,RTEConf_ParameterAccessKernelInterface.hpp,\
LVCMem_Allocator.hpp,LVC_KernelInterfaceInstance.hpp,LVC_LibOmsInterface.hpp,LVC_LoadLibrary.hpp,SAPDB_MemCopyMove.hpp,Kernel_TraceTopics.hpp,\
LVCMem_EmergencyAllocator.hpp,Kernel_OpMsg.hpp,Trace_Entry.hpp,SQLMan_Context.hpp,SysView_PseudoLongHandler.hpp,SAPDB_UTF8Basis.hpp,PIn_Packet.h,SAPDB_RangeCode.hpp \
noshrglob -DOMS_IMPLEMENTATION dep=OMS_IliveCacheSpec.hpp
vak35   inc=+gsp03,gsp03_1,vsp008,vsp0031,gak30
vak351  inc=+vsp008,vsp0031,gak30
vak36   inc=+gsp03,gsp03_1,vsp006,vsp008,vsp0011,vsp0031
vak36-80 inc=+gsp03,gsp03_1,vsp006,vsp008,vsp0011,vsp0031 KERNEL80=1
vak361  inc=+gsp03,gsp03_1,vsp0031
vak362  inc=+gsp03,gsp03_1,vsp0031,vsp008
&if $OSSPEC in [ SOLARIS, SVR4 ]
vak37   inc=+vsp008,vsp0011,vsp0031,gsp03,gsp03_1,gsp03_2,ggg07,gak30,gak40 [f -NO f]
&else
vak37   inc=+vsp008,vsp0011,vsp0031,gsp03,gsp03_1,gsp03_2,ggg07,gak30,gak40
&endif
vak38   inc=+vsp008,ggg07,gak40
gak40   inc=+ggg07
vak40   inc=+vsp002,gsp03,gsp03_1,vsp008,ggg07,gak40,gak71
vak41   inc=+ggg07,gak40
hak41.h inc=gsp00,ggg00,vak001,gak40
&if $OSSPEC in [ SOLARIS, SVR4 ]
vak42   inc=+vsp002,gsp03,gsp03_1,ggg01,ggg07,gkb00,gkb05,gak40 [f -NO f]
&else
vak42   inc=+vsp002,gsp03,gsp03_1,ggg01,ggg07,gkb00,gkb05,gak40
&endif
vak421  inc=+gsp92,ggg07,gak40
# ------------------------------------------------------------------------------
# Pascal wrapper for system views used by vak40-42
# ------------------------------------------------------------------------------
vak400.cpp     inc=gsp00,ggg00,gak40,hak07.h,\
                    SAPDBMem_IRawAllocator.hpp,SAPDBMem_NewDestroy.hpp,\
                    SysView_Defines.hpp,SysView_ISysView.hpp,SysView_SysViewBase.hpp,\
                    SysView_ActiveConfiguration.hpp,SysView_BackTrace.hpp,SysView_BackupInformation.hpp,\
                    SysView_BackupThreads.hpp,SysView_CacheStatistics.hpp,\
                    SysView_CacheStatisticsReset.hpp,SysView_ClassContainers.hpp,\
                    SysView_CatalogCacheStatistics.hpp,\
                    SysView_ClassContainerStatistics.hpp,SysView_ClassContainerStatisticsReset.hpp,\
                    SysView_CommandCacheStatistics.hpp,SysView_CommandCacheStatisticsReset.hpp,\
                    SysView_CommandStatistics.hpp,SysView_CommandStatisticsReset.hpp,\
                    SysView_DataCache.hpp,SysView_DataStatistics.hpp,\
                    SysView_DataStatisticsReset.hpp,SysView_DataVolumes.hpp,\
                    SysView_FileDirectories.hpp,SysView_Files.hpp,\
                    SysView_HistoryCleanupStatistics.hpp,SysView_HistoryCleanupStatisticsReset.hpp,\
                    SysView_HotStandbyComponent.hpp,SysView_HotStandbyGroup.hpp,\
                    SysView_IndexStorageDetails.hpp,SysView_Instance.hpp,SysView_IOThreadStatistics.hpp,\
                    SysView_IOThreadStatisticsReset.hpp,SysView_IOBufferCaches.hpp,\
                    SysView_KernelMessages.hpp,SysView_Locks.hpp,\
                    SysView_LockStatistics.hpp,SysView_LockStatisticsReset.hpp,\
                    SysView_LogInformation.hpp,\
                    SysView_LogQueueStatistics.hpp,SysView_LogQueueStatisticsReset.hpp,\
                    SysView_LogStatistics.hpp,SysView_LogStatisticsReset.hpp,\
                    SysView_LogVolumes.hpp,SysView_MachineConfiguration.hpp,SysView_MachineUtilization.hpp,\
                    SysView_MemoryAllocatorStatistics.hpp,\
                    SysView_MemoryAllocatorStatisticsReset.hpp,SysView_MultiVersionReadStatistics.hpp,\
                    SysView_MemoryHolders.hpp,SysView_Miscellaneous.hpp,SysView_OMSLocks.hpp,\
                    SysView_ObjectLocks.hpp,\
                    SysView_OMSProcedureStatistics.hpp,SysView_OMSProcedureStatisticsReset.hpp,\
                    SysView_OMSVersions.hpp,SysView_OptimizerInformation.hpp,\
                    SysView_PageLocks.hpp,SysView_PseudoLongHandler.hpp,\
                    SysView_ReaderWriterLockInformation.hpp,SysView_ReaderWriterLockStatistics.hpp,\
                    SysView_ReaderWriterLockStatisticsReset.hpp,SysView_ReaderWriterLockWaitingTasks.hpp,\
                    SysView_RegionInformation.hpp,SysView_RegionStatistics.hpp,\
                    SysView_RegionStatisticsReset.hpp,SysView_RestartInformation.hpp,\
                    SysView_Schemas.hpp,SysView_Sessions.hpp,\
                    SysView_Snapshots.hpp,SysView_SpinLockPoolStatistics.hpp,SysView_SpinLockPoolStatisticsReset.hpp,SysView_SpinLockStatistics.hpp,\
                    SysView_SpinLockStatisticsReset.hpp,SysView_TableStorageDetails.hpp,\
                    SysView_TaskGroupStatistics.hpp,\
                    SysView_TaskLoadBalancingInformation.hpp,SysView_TaskLoadBalancingTaskGroupStates.hpp,\
                    SysView_TaskLoadBalancingTaskMoves.hpp,\
                    SysView_TaskGroupStatisticsReset.hpp,\
                    SysView_TaskStatistics.hpp,SysView_TaskStatisticsReset.hpp,\
                    SysView_Transactions.hpp,\
                    SysView_UnloadedStatements.hpp,\
                    SysView_Version.hpp,SysView_OMSDiagnose.hpp,\
                    SQLMan_Context.hpp,SysView_TransactionHistory.hpp,\
                    SysView_Servertasks.hpp,\
                    SysView_PseudoLongHandler.hpp,SAPDB_MemCopyMove.hpp
vak43   inc=+ggg07,gak40
vak44   inc=+ggg07,gak40
vak45   inc=+ggg07,gak40
vak46   inc=+vsp008,ggg07,gak40
vak47   inc=+ggg07,gak40
vak48   inc=+ggg07,gak40
vak49   inc=+ggg07,gak40
vak501  inc=+vsp008,gsp03,gsp03_1,ggg07,gak68
vak502  inc=+ggg07,gak68,gak71
hak502.h inc=-vsp001,gsp00,gak00,ggg04
vak503  inc=+ggg07,gak71
vak505  inc=+ggg07,gak68,gak71,gak104
vak506  inc=+vsp008,gak104
hak506.h inc=gsp00,vak001
hak54.h  inc=gsp00,ggg00,vak001
hak642.h inc=gak00,vak001
hak71.h  inc=gsp00,ggg00,gak00,vak001,ggg07,gak70,
hak720.h inc=gsp00,gak00,vak001,ggg07,gak70,gak71
hak728.h inc=gsp00,vak001,ggg07,gak68
vak507  inc=+vsp008,ggg01,ggg07,gak68,gak71
vak508  inc=+vsp008
&if $OSSPEC in [ SOLARIS, SVR4 ]
vak51   inc=+gsp03,gsp03_1,vsp006,vsp008 [f -NO f]
&else
vak51   inc=+gsp03,gsp03_1,vsp006,vsp008
&endif
hak52.h inc=gsp00,ggg00,vak001
vak52   inc=+vsp008,vsp002
vak54   inc=+vsp008,ggg07,gak71,gak68
vak542  inc=+vsp008
vak543c noshrglob
vak544  inc=+gak30
vak545  inc=+vsp008
vak56   inc=+vsp008
vak562  inc=+ggg07,gak71,gak104
vak57   inc=+ggg07,gak71,gak104
vak58   inc=+ggg07,gak71
vak59   inc=+gsp03,ggg07,gak71,gak68
hak509.h inc=gsp00
vak60   inc=+vsp006,vsp008
vak61   inc=+vsp008
hak61.h inc=gsp00,ggg00,gak00,vak001
vak62   inc=+ggg07,gak71
vak640  inc=+ggg07,gak68
vak641  inc=+vsp008,gak104
vak65   inc=+ggg07,gak68,gak104
vak651  inc=+gak104
vak660  inc=+vsp008,ggg07,gak71,gsp03
vak661  inc=+ggg07,gak71
vak663  inc=+gsp03,gsp03_1
vak664wrapper inc=vak001
vak67   inc=+ggg07,gak71,gak68
vak680	inc=+gsp03,gsp03_1,ggg07,gak68,gak71
&if $OSSPEC == LINUX && $MACH in [ SDBonPPC64 ]
# with optimization in vak54 the MUT produces an error -8035 during a mass update on a table without a foreign key 
vak54  -OPT=-O0
vak680 -OPT=-O0
vak10  -OPT=-O0  # PTS 1138339 ; remove of -9206 during 'alter table'
vak726 -OPT=-00  # h.b. optimizer.xml 760 runs into diffs since pts 1141513
&endif
hak680.h inc=+gak68,gak71,-vsp001,ggg04
gak68   inc=+ggg07
vak681  inc=+ggg07,gak68
vak682  inc=+gsp03,gsp03_1,ggg07,ggg11,gak68,gak71
vak683  inc=+vsp008,ggg07,gak68  trace=none
vak684  inc=+ggg07,gak68,gak71
vak685  inc=+ggg07,gak68,gsp03
vak686  inc=+gak68
vak687.cpp inc=ggg00,SAPDB_Types.hpp,RTE_Crash.hpp,SAPDBErr_Assertions.hpp,hgg941.h,hak687.h
vak690  inc=+ggg07,gak68,gak71
gak70   inc=+ggg07
gak71   inc=+ggg07
vak70   inc=+ggg07,gak70,gak71,gak68
vak71   inc=+ggg07,gak70,gsp03
vak720  inc=+ggg07,gak70,gak71
vak721  inc=+ggg07,gak70,gak71
vak722  inc=+ggg07,gak70,gak71,gak40
vak723  inc=+ggg07,gak70
vak724  inc=+ggg07,gak70
vak725  inc=+ggg07,gak70,gak71 trace=none
vak726  inc=+ggg07,gak70
vak727  inc=+ggg07,gak70,gak71 trace=none
vak728  inc=+ggg07,gak70,gak71,gak68
vak74   inc=+ggg07,gak68
vak81   trace=none
&if $OSSPEC in [ SOLARIS, SVR4 ]
vak91   inc=+vsp002,gsp03,gsp03_1,vsp008 trace=none [s SLOW=y s] INLINK=1 [f -NO f]
&else
vak91	inc=+vsp002,gsp03,gsp03_1,vsp008 trace=none [s SLOW=y s] INLINK=1
&endif
vak92   inc=+gsp03,gsp03_1,vsp008,ggg11,gak104
&if defined SAPDBMem_USE_SYSTEM_ALLOC
&if $MACH in [ S390X ]
&else
vak92  PURIFY=1
&endif
&endif
hak92.h inc=gsp00,vsp001,ggg00,vak001
vak93   inc=+gsp03,gsp03_1,vsp008,ggg11
vak94   inc=+gsp03,gsp03_1,vsp008
vak96   inc=+vsp0031,ggg11
hak96.h inc=ggg00

hak682.h    inc=+gak68,gak71,-gak00,vsp001,ggg04
hak683.h    inc=+gak68,ggg07,-gak00,vsp001,ggg04
hak686.h    inc=-gak00,vsp001,ggg04,gsp00,+gak68
hak725.h    inc=+ggg07,gak70,gak71,-vsp001,gak00,ggg04
hak70.h     inc=+ggg07,gak70,gak71,-vsp001,ggg04

gak104      inc=
vak104      inc=gsp00,gak104
hak104.h    inc=gsp00,gak104
vak104.cpp inc=hak104.h,SAPDBFields_IncrementalMemorySequence.hpp,\
    SAPDBFields_Persistence.hpp,\
    SAPDBFields_AttributedField.hpp,\
    SAPDBMem_IRawAllocator.hpp,SAPDBMem_NewDestroy.hpp

