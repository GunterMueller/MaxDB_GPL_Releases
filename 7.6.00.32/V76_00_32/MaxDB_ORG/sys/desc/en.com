# @(#)   desc/en.com   7.2.0   07/16/98
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

&if $OSSPEC not in [ WIN32 ]
#
# All Modules described in this file are ONLY for UNIX ports!
#
*       inc= trace=none check=no -DDEVELOPMENT \
&if $MACH == SUN || $OSSPEC in [ SOLARIS, SVR4 ]
       -DASYNC_IO
&endif
#
##########################################
# only for SUNOS_64 ######################
##########################################
&if $MACH == SUN || $OSSPEC == SOLARIS
&  if $MACH in [ SUN ]
&    if defined BIT64
       :en/mtmalloc2.o binary
&    endif
&  else
# link dynamically, so we have the choice of libmtmalloc or libumem
    -lumem
&  endif
&endif
##########################################
# only for HPUX_32 ######################
##########################################
&if $MACH in [ PA11 ]
       :en/ugetfram.o binary
&endif
##########################################
#
*-a     APPLIC=Y
#
*-e     -DANW_RTE
#
# This variant uses kernel structures, but is not used inside kernel process
#
#---------------------------------------------------------------------------
# gen*.h Headerfiles contain definitions and prototypes used by layers en/eo
#---------------------------------------------------------------------------
#
gen00.h   inc=gsp00,gsp100.h,geo60.h,vsp002,RTESys_Spinlock.h,geo00_2.h,SAPDB_string.h
gen003.h  inc=gsp00,gsp01.h,geo003.h,geo40.h,gen00.h
# independent terminal include module
gen04.h   inc=
gen13.h   inc=gsp00,vsp004,geo13.h,gen00.h
gen22.h   inc=SAPDB_Types.h
gen23.h   inc=geo00.h,gen00.h,gen003.h
gen32.h   inc=geo00.h,gen00.h,gen003.h
gen33.h   inc=geo00.h
gen34.h   inc=geo00.h,gen00.h,gen50.h
gen38.h   inc=gsp00,gen00.h
gen41.h   inc=geo00.h,gen00.h
gen42.h   inc=gsp00,gsp01.h,gen00.h
gen43.h   inc=geo43_0.h
gen45.h   inc=gen00.h,gen50.h
gen46.h   inc=gsp00,gen003.h
gen48.h   inc=gsp00
gen49.h   inc=geo00.h,gen00.h
gen50.h   inc=gen00.h,heo00.h,geo00_3.h,heo07.h,vsp002,geo52.h,RTE_Types.h,RTETask_LegacyQueueElement.h,RTETask_LegacyQueueElementFreeList.h,Kernel_SharedState.h
gen52.h   inc=geo00.h
gen53.h   inc=geo50_0.h
gen54.h   inc=gsp00,vsp002
gen55.h   inc=gsp00
gen57.h   inc=geo00.h
gen60.h   inc=RTE_Types.h
gen60_1.h inc=geo00.h,geo002.h
gen66.h   inc=gsp00
gen70.h   inc=geo002.h
gen71.h   inc=gsp00,gen50.h
gen72.h   inc=geo00.h,gen50.h
gen73.h   inc=gsp00,gen50.h
gen74.h   inc=geo00.h,geo002.h,geo70k.h,gen00.h,gen003.h
gen75.h   inc=geo00.h,geo002.h,geo70k.h,gen00.h,gen003.h
gen81.h   inc=gsp00
gen83.h   inc=geo00.h,gen50.h
gen84.h   inc=geo00.h,gen50.h
gen85.h   inc=geo00.h
gen88.h   inc=geo00.h,gen50.h

# Kernel RTE Message Include File (to be replaced by geo007_1.h):
gen500.h  inc=
gen541.h  inc=gsp00,gsp01.h
gen780.h  inc=
gen904.h  inc=gsp00,gsp09.h
gen950.h  inc=
#
#---------------------------------------------------------------------------
# hen*.h Headerfiles contain prototypes used by layers other than en/eo
#---------------------------------------------------------------------------
#
hen02.h   inc=gsp00
hen03.h   inc=gsp00
hen04.h   inc=gsp00
hen05.h   inc=gsp00
hen06.h   inc=gsp00
hen07.h   inc=gsp00
hen08.h   inc=gsp00
hen11.h   inc=gsp00
hen12.h   inc=gsp00
hen14.h   inc=gsp00
hen15.h   inc=gsp00
hen17.h   inc=gsp00
hen18.h   inc=gsp00
hen22.h   inc=gsp00
hen37.h   inc=gsp00
hen38.h   inc=
hen40.h   inc=geo00.h
hen42.h   inc=gsp00,gen00.h
hen43.h   inc=gsp00,gsp01.h
hen50.h   inc=gsp00
hen53.h   inc=gsp00,gsp01.h
hen55.h   inc=gsp00
hen56.h   inc=gsp00
hen59.h   inc=gsp00
hen60.h   inc=gsp00
hen62.h   inc=gsp00
hen66.h   inc=gsp00
hen71.h   inc=gsp00
hen78.h   inc=
hen541.h  inc=gsp00,vsp002

#
#---------------------------------------------------------------------------
# ven*.c/cpp contain implementation
#---------------------------------------------------------------------------
#
ven01.c  -Dstrings -DMAIN              inc=gen00.h,geo001.h,geo57.h,heo46.h,gen22.h,SAPDBErr_MessageOutput.h,RTE_UNIXAuthenticate.h,:SAPDB/RunTime/Security/Crypto/RTECrypto_EntropyPool.h
ven02.c                                inc=gen00.h
ven03.c -DSOCKETS                      inc=gsp01.h,gsp09.h,geo001.h,geo003.h,geo007_1.h,geo007_2.h,\
geo03.h,heo03.h,gen003.h,geo40.h,geo401.h,heo46.h,geo57.h,\
gen23.h,gen33.h,gen41.h,gen42.h,gen43.h,gen46.h,heo07.h,RTESec_SAPSSL.h,RTESec_SSL.h,hen40.h,RTESys_MicroTime.h,heo420.h

# virtual terminal
&if $COMPILER != ICC
ven04   -NO                            inc=gsp00
&else
ven04                                  inc=gsp00
&endif
&if $MACH in [ PA11, PA20W, HP_IA64 ]
# virtual terminal
ven041.c  -DKANJI -D_XOPEN_SOURCE_EXTENDED \
                                       inc=geo002.h,gen04.h,gen500.h,geo001.h,geo57.h,hen40.h
# virtual terminal subroutines
ven042.c -D_XOPEN_SOURCE_EXTENDED       inc=geo002.h,gen04.h,geo001.h,geo57.h,geo007_1.h,heo01.h,hen40.h
&else
# virtual terminal
ven041.c  -DKANJI                        inc=geo002.h,gen04.h,gen500.h,geo001.h,geo57.h,hen40.h
# virtual terminal subroutines
ven042.c                                inc=geo002.h,gen04.h,geo001.h,geo57.h,geo007_1.h,heo01.h,hen40.h
&endif
&if $MACH in [ PA11, PA20W, HP_IA64 ]
# maketi
ven043.c -DMAKETI -D_XOPEN_SOURCE_EXTENDED \
                                       inc=geo002.h,gen04.h,geo001.h,geo57.h,hen40.h,heo01.h
&else
# maketi
ven043.c -DMAKETI                       inc=geo002.h,gen04.h,geo001.h,geo57.h,hen40.h,heo01.h
&endif
ven05.c                                inc=gen00.h,geo007_1.h,heo46.h,hen50.h,hen40.h
#
###### user virt. file
ven06.c                                inc=gen00.h,heo46.h,geo001.h,geo57.h,hen40.h,geo007_1.h
ven06-t.c                              inc=gen00.h,heo00.h,heo46.h,geo001.h,geo57.h,hen40.h
#
ven07.c                                inc=gen00.h,geo007_1.h,hen40.h
ven08.c                                inc=gen00.h
ven11.c                                inc=gen00.h,heo01.h,heo11.h,heo46.h,geo001.h,geo007_1.h,geo007_2.h,geo57.h,hen40.h,RTESys_Time.h
####### sqlargs
ven12.c                                inc=gen00.h,heo46.h,gen13.h,geo001.h,geo57.h,hsp77.h,hsp78_0.h,hsp81.h,hsp83.h,heo12.h,heo13.h
&if $MACH in [ _IBMR2]
ven12c.imp  binary
&endif
ven12a                                 inc=
#
####### sqluser
ven13.c                                inc=gen00.h,gen13.h,heo46.h,geo007_1.h,geo001.h,geo57.h,hen40.h
ven14.c                                inc=gen00.h,geo007_1.h,heo46.h,hen40.h,hsp77.h
ven15.c                                inc=gen00.h,heo46.h
ven17.c                                inc=gen00.h,geo00.h,geo001.h,geo57.h,geo007_1.h,hen40.h
ven18.c                                inc=gen00.h,hen40.h,geo007_1.h,heo01.h,heo46.h
ven22.c                                inc=gen00.h,vsp003c,hen40.h,gsp01.h,heo01.h,RTESys_Time.h,hsp77.h,gen22.h,SAPDBErr_MessageOutput.h,heo06.h
ven23.c -DSOCKETS                      inc=gen00.h,vsp001c,gsp09.h,gen23.h,gen42.h,geo001.h,geo57.h,hen40.h,gsp01.h,hsp77.h,RTE_saveUNIXcalls.h
ven32.c                                inc=gen00.h,vsp001c,gen32.h,hen40.h,gen41.h,RTE_saveUNIXcalls.h,gen42.h
# CTS 1106771
ven32-s.c  -DSOCKETS                   inc=gen00.h,vsp001c,gen32.h,hen40.h,gen41.h,RTE_saveUNIXcalls.h,gen42.h
ven33.c -DSOCKETS                      inc=RTESys_MemoryBarrier.h,gen00.h,geo007_1.h,vsp001c,gen003.h,gen33.h,gen32.h,gen41.h,gen42.h,geo001.h,geo57.h,hen40.h,RTE_saveUNIXcalls.h
ven34.c -DUSE_KGS                      inc=gen00.h,geo007_1.h,geo50_0.h,gen34.h,hen40.h
ven36.cpp                              inc=vsp001,geo00d.h,heo38.h,heo78.h,heo01.h,heo02x.h,geo60.h,gen500.h,LVC_LoadLibrary.hpp
ven37.c                                inc=heo46.h,geo60.h,hen37.h
ven381.c  -DUSE_KGS                   inc=gen00.h,geo002.h,gen50.h,gen38.h,RTE_ConsoleStandardOutput.h
ven38.cpp -DUSE_KGS                    inc=gsp00,vsp002c,heo00.h,hen38.h,heo01.h,heo52.h,heo46.h,heo02x.h,geo60.h,heo60.h,geo50_0.h,gen500.h,OMS_Defines.h,geo00d.h,hak341.h,geo38.h,heo38.h,geo573.h,LVC_Dispatcher.hpp,LVC_LoadLibrary.hpp
&if $MACH in [ S390X ]
ven38NMP.cpp		                   inc=geo00d.h,OMS_Defines.h,vak001
&else
# PTS 1107243
ven38NMP.cpp                           inc=geo00d.h,OMS_Defines.h,vak001 -FIalloca
&endif
ven40.c    -DUSE_KGS                   inc=gen00.h,hen40.h
ven40-i.c  -DENUMINST_ONLY -DUSE_KGS   inc=gen00.h,hen40.h
ven40-k.c  -DUSE_KGS -DKERNEL_LZU      inc=gen00.h,hen40.h
ven41.c                                inc=heo41.h,gen41.h,geo007_1.h,hen40.h,hsp77.h,SAPDB_Names.h,RTE_IniFileHandling.h,RTE_UNIXAuthenticate.h,heo01.h
ven42.c                                inc=gen00.h,heo00.h,vsp001c,gen003.h,gen42.h,hen42.h,gen43.h,gen46.h,heo46.h,gen500.h,hen40.h,RTE_saveUNIXcalls.h,hsp77.h,gen60.h,geo007_2.h,heo420.h
ven42-s.c  -DSOCKETS                   inc=gen00.h,heo00.h,vsp001c,gen003.h,gen42.h,hen42.h,gen43.h,gen46.h,heo46.h,gen500.h,hen40.h,RTE_saveUNIXcalls.h,hsp77.h,gen60.h,geo007_2.h,heo420.h
ven43.c                                inc=gen00.h,heo00.h,vsp001c,gen003.h,gen43.h,hen43.h,geo007_1.h,hen40.h,heo46.h
ven43-s.c  -DSOCKETS                   inc=gen00.h,heo00.h,vsp001c,gen003.h,gen43.h,hen43.h,geo007_1.h,hen40.h,heo46.h
ven45.c   -DUSE_KGS                    inc=gen00.h,heo00.h,gen50.h,vsp001c,gen003.h,gen41.h,gen45.h,heo07.h,hsp77.h dep=gen500.h
ven45-u.c -DUSE_KGS -DOUTSIDE_KERNEL   inc=gen00.h,heo00.h,geo002.h,gen50.h,vsp001c,gen003.h,gen41.h,gen45.h,heo07.h,hsp77.h dep=geo002.h
ven46.c                                inc=gen00.h,vsp001c,gen46.h,gen500.h,hen40.h,gen60.h
ven48.c                                inc=gen00.h,gen48.h,SAPDB_string.h
&if $MACH in [ _IBMR2, PA11, PA20W, HP_IA64 ]
ven49.c                                inc=gen49.h,geo60.h
ven49-m.c -DMULTITHREADED              inc=gen49.h,geo60.h
&endif

#
#  KERNEL
#
ven50.c -DUSE_KGS                      inc=hen50.h,geo50_0.h,geo007_1.h,heo07.h,heo670.h,geo002.h,gen81.h,gen71.h
ven51.cpp -DUSE_KGS -DKERNEL_LZU -Dstrings \
                                       inc=gen00.h,geo50_0.h,geo00_2.h,hen50.h,heo51.h,heo46.h,gen71.h,gen72.h,gen73.h,gen75.h,gen81.h,gen88.h,hen56.h,hen40.h,geo007_1.h,heo15.h,RTE_KernelMain.h,hen51.h,RTE_DiagPipe.h,gen41.h,heo92.h,heo69.h,RTETask_Task.hpp,RTE_Version.h
ven52.c -DUSE_KGS -Dstrings            inc=gen00.h,heo00.h,geo50_0.h,geo007_1.h,gen52.h,RTESys_Time.h,RTESys_MicroTime.h
ven53.cpp -DUSE_KGS -DKERNEL_LZU -Dstrings \
                                       inc=RTESys_MemoryBarrier.h,gen00.h,heo00.h,hen53.h,geo50_0.h,gen500.h,vsp001c,gen003.h,geo67.h,gen41.h,gen42.h,gen71.h,RTESys_MicroTime.h,hen50.h,gen75.h,gen45.h,gen32.h,gen42.h,RTETask_Task.hpp,RTETask_TaskScheduler.hpp,gen53.h,heo420.h,heo52.h,RTE_KGS.hpp,RTECKC_KernelCommunication.hpp
&if $LC_STATE == "DEV"
# vgetapplipid will produce emergency shutdown for illegal task id given (but only in DEV version)
# Reason: It produces only output values and an illegal output value can be ignored...
ven53.cpp -DSDB_DEV_ONLY_CHECK
&endif
ven54.cpp -DUSE_KGS -Dstrings -DDEVELOPMENT \
                                       inc=gen00.h,heo00.h,hen50.h,heo52.h,geo50_0.h,gen500.h,geo54.h,gen54.h,heo54k.h,geo00_2.h,\
                                           gen41.h,gen541.h,geo57.h,heo58.h,gen71.h,gen72.h,gen75.h,heo07.h,gen81.h,gen88.h,hen40.h,\
                                           geo001.h,RTEIO_VolumeInfo.h,RTESys_MicroTime.h,RTEConf_ParameterAccessKernelInterface.h,\
                                           RTE_saveUNIXcalls.h,RTEHSS_PascalInterface.h,RTETask_LegacyTaskCtrl.h,hsp77.h,heo51.h,\
                                           RTE_KGS.hpp,RTETask_CommonQueueElement.hpp
ven541.c -DUSE_KGS -Dstrings           inc=gen00.h,heo00.h,gsp01.h,gen500.h,heo46.h,gen541.h,hen541.h,geo001.h,geo57.h,hen40.h
ven542.c -DUSE_KGS -Dstrings           inc=gen00.h,heo00.h,geo50_0.h,geo54.h,gen500.h,hen40.h,gen54.h,RTETask_LegacyTaskCtrl.h
ven55.c -DUSE_KGS -Dstrings            inc=gen00.h,heo00.h,geo002.h,geo50_0.h,gen500.h,gen55.h,heo55k.h,gen71.h,hen50.h,hen56.h,RTETask_LegacyTaskCtrl.h,heo46.h
ven555.cpp inc=
ven56.c -DUSE_KGS -Dstrings            inc=heo56.h,geo00_0.h,geo50_0.h,geo007_1.h,geo00_2.h,heo52.h,geo002.h,gen71.h,gen83.h,RTETask_LegacyTaskCtrl.h,RTESys_MicroTime.h,hen50.h
ven58.c -DUSE_KGS                      inc=gen00.h,heo00.h,geo002.h,gen50.h,gen500.h,heo46.h,heo58.h,geo001.h,geo57.h,geo50_0.h,hen40.h,heo54k.h,hen50.h
ven60.c -DUSE_KGS -DNEW_DIAG_FILES     inc=gen00.h,heo00.h,geo50_0.h,RTE_Types.h,geo007_1.h,geo007_2.h,gen60.h,gen60_1.h,hen60.h,gen41.h,heo46.h,gen48.h,hen40.h,geo00_2.h,RTEDiag_ServerCrashHist.h,RTESys_Time.h,hsp100.h,RTE_Speed.h,RTE_DiagPipe.h,SAPDBErr_MessageOutput.h,SAPDB_Names.h,hsp77.h,heo60_2.h,Msg_OutputKnlDiag.h
#
# Kern virtuelle File
ven66.c  -DUSE_KGS -Dstrings           inc=gen00.h,heo00.h,gen500.h,gen66.h,heo46.h,hen40.h
ven69.c -DUSE_KGS -Dstrings            inc=gen00.h,heo00.h,geo50_0.h,gen500.h,heo69.h
ven69-a.c -DUSE_KGS -Dstrings          inc=gen00.h,heo00.h,gen500.h,heo69.h
ven70.cpp -DUSE_KGS -DKERNEL_LZU -Dstrings \
                                       inc=gen71.h,gen00.h,geo00_1.h,geo001.h,geo002.h,geo50_0.h,gen55.h,geo70k.h,heo07.h,gen500.h,gen70.h,gen41.h,geo57.h,gen73.h,gen74.h,gen38.h,hen40.h,gen54.h,heo15.h,gen81.h,RTEConf_ParameterAccessKernelInterface.h,hen50.h,RTETask_Task.hpp,RTETask_TaskScheduler.hpp,\
                                       SAPDBMem_NewDestroy.hpp,RTETask_CommonQueueElement.hpp,RTETask_CommonQueue.hpp,RTECKC_KernelCommunication.hpp,gen84.h,Kernel_ExtendedState.hpp
ven71.cpp -DUSE_KGS -DKERNEL_LZU -Dstrings -DTASK_STATISTIK \
                                       inc=gen00.h,heo00.h,geo002.h,heo52.h,geo50_0.h,geo007_1.h,gen71.h,hen71.h,hen40.h,geo00_2.h,gen73.h,RTESys_MicroTime.h,hsp77.h,,RTETask_LegacyUKTCtrl.h,RTETask_LegacyTaskCtrl.h,gen75.h,Trace_Manager.h,gen72.h,gen81.h,hen56.h,gen55.h,hen50.h,RTETask_TaskScheduler.hpp,RTETask_Task.hpp,RTESys_MemoryBarrier.h

ven72.cpp -DUSE_KGS -Dstrings          inc=gen00.h,heo00.h,geo50_0.h,gen500.h,gen72.h,hen50.h,hen40.h
ven73.c -DUSE_KGS -Dstrings            inc=gen00.h,heo00.h,geo50_0.h,gen500.h,gen73.h
ven74.c -DUSE_KGS -Dstrings            inc=gen00.h,heo00.h,geo50_0.h,gen500.h,vsp001c,gen003.h,geo70k.h,hen40.h,hsp77.h

&if $MACH in [_IBMR2, PA11, PA20W, HP_IA64]
ven75.c  -DUSE_KGS -Dstrings           inc=gen00.h,heo00.h,geo50_0.h,gen500.h,vsp001c,gen003.h,gen32.h,gen41.h,gen46.h,gen75.h,hen40.h,gen49.h,gen46.h,gen32.h,gen71.h,gen73.h,gen74.h,gen88.h,geo67.h,hsp77.h,gen53.h,RTE_saveUNIXcalls.h,RTE_ConsoleStandardOutput.h,heo92.h,SAPDB_Names.h
&else
ven75.c  -DUSE_KGS -Dstrings           inc=gen00.h,heo00.h,geo50_0.h,gen500.h,vsp001c,gen003.h,gen32.h,gen41.h,gen46.h,gen75.h,hen40.h,gen46.h,gen32.h,gen71.h,gen73.h,gen74.h,gen88.h,geo67.h,hsp77.h,gen53.h,RTE_saveUNIXcalls.h,RTE_ConsoleStandardOutput.h,heo92.h,SAPDB_Names.h
&endif

#ven78.cpp -DUSE_KGS                    inc=heo01.h,heo78.h,geo47.h,geo60.h
#ven78-u.cpp                            inc=heo01.h,heo78.h,geo47.h,geo60.h
#ven78-t.cpp -DTEST                     inc=heo01.h,heo78.h,geo47.h,geo60.h
ven80.cpp -DUSE_KGS                    inc=gen81.h,geo50_0.h,geo007_1.h,gen38.h,RTE_KernelMain.h,RTE_Crash.hpp,RTE_KGS.hpp,gen60.h,SAPDBErr_MessageOutput.h,SAPDB_Names.h
ven81.cpp -DUSE_KGS -Dstrings -DNOTRAC -DMAIN -D_THREAD_SAFE -DNEW_DIAG_FILES \
                                       inc=heo00.h,gen00.h,RTE_Types.h,geo50_0.h,geo007_1.h,hen40.h,gen41.h,\
                                       gen43.h,gen45.h,hen50.h,gen52.h,gen54.h,gen60.h,gen70.h,gen71.h,gen72.h,\
                                       gen74.h,heo07.h,gen81.h,gen83.h,gen84.h,gen85.h,gen88.h,hsp100.h,\
                                       geo57.h,heo15.h,geo67.h,Trace_InfoPage.h,RTEDiag_ServerCrashHist.h,\
                                       RTESys_Time.h,RTE_Speed.h,gen75.h,RTEMem_PageAllocator.h,gip00.h,\
                                       LVCMem_Allocator.h,RTE_DiagPipe.h,RTE_saveUNIXcalls.h,\
                                       RTEThread_Console.h,RTEConf_ParameterAccessKernelInterface.h,\
                                       RTEConf_ParameterAccessKernelInterface.hpp,RTETask_LegacyTaskCtrl.h,\
                                       heo670.h,SAPDB_Names.h,RTESys_SystemInfo.h,RTE_UNIXAuthenticate.h,\
                                       SAPDB_sprintf.h,gen60_1.h,heo69.h,heo92.h,RTE_SpawnRestartShutdown.h,\
                                       Msg_OutputKnlDiag.h,RTE_Version.h,RTESync_SpinlockRegister.hpp,\
                                       RTESync_SpinlockPoolRegister.hpp,RTEDiag_SymbolResolutionLock.h,\
                                       RTEThread_IORedirection.hpp,RTETask_WorkerPool.hpp,RTESys_MicroTime.h
ven83.cpp  -DUSE_KGS -Dstrings -DNEW_DIAG_FILES -DKERNEL_LZU    inc=gen00.h,heo00.h,geo002.h,gen50.h,gen500.h,gen81.h,gen83.h,gen45.h,geo50_0.h,gen72.h,gen73.h,gen84.h,hen40.h,RTEThread_ConsoleConnections.h,RTE_KernelMain.h,gen71.h,gen72.h,gen73.h,gen84.h,hen50.h,Msg_OutputKnlDiag.h,RTETask_Task.hpp
&if $OSSPEC in [ HPUX ]
ven84.cpp  -DUSE_KGS -Dstrings -OPT=+O1 inc=gen00.h,heo00.h,geo002.h,gsp01.h,geo50_0.h,gen500.h,vsp001c,gen003.h,gen81.h,gen84.h,hen40.h,gen32.h,gen38.h,gen41.h,gen42.h,gen71.h,gen72.h,gen73.h,hsp77.h,hen50.h,heo56.h,RTECKC_KernelCommunication.hpp,RTE_KGS.hpp,RTETask_Task.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTEConf_ParameterAccessKernelInterface.h
&else
ven84.cpp  -DUSE_KGS -Dstrings         inc=gen00.h,heo00.h,geo002.h,gsp01.h,geo50_0.h,gen500.h,vsp001c,gen003.h,gen81.h,gen84.h,hen40.h,gen32.h,gen38.h,gen41.h,gen42.h,gen71.h,gen72.h,gen73.h,hsp77.h,hen50.h,heo56.h,RTECKC_KernelCommunication.hpp,RTE_KGS.hpp,RTETask_Task.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTEConf_ParameterAccessKernelInterface.h
&endif
ven88.cpp -DUSE_KGS                    inc=gen88.h,geo007_1.h,geo50_0.h,heo07.h,heo69.h,geo002.h,heo92.h,hen40.h,hen50.h,gen71.h,gen73.h,gen74.h,gen81.h,RTE_Crash.hpp,gen38.h,RTE_ITaskStackControl.hpp,SAPDBErr_Assertions.hpp,RTEMem_RteAllocator.hpp,RTE_Coroutine.h,hen51.h,RTETask_LegacyUKTCtrl.h,RTETask_Task.h,RTESys_MicroTime.h

#
# RTE main programs
#
# console
gen921.h   -DUSE_KGS -DKERNEL_LZU      inc=geo50_0.h
ven921.cpp -DUSE_KGS -DKERNEL_LZU      inc=gen921.h,RTETask_CommunicationQueue.hpp,RTETask_Task.hpp,RTETask_TaskScheduler.hpp
ven92.c    -DUSE_KGS -DMAIN            inc=hsp100.h,heo00x.h,geo002.h,geo007_1.h,gen003.h,heo46.h,heo92.h,geo50_0.h,geo001.h,geo57.h,hen40.h,gen41.h,gen38.h,heo52.h,RTECons_Main.h,RTE_ConsoleStandardOutput.h,gen921.h,hsp77.h,gen22.h

# vserver main
ven96.c -DSOCKETS                      inc=gen00.h,vsp001c,geo007_1.h,gen003.h,hsp100.h,gen32.h,gen33.h,hen40.h,gen41.h,gen42.h,gen43.h,heo46.h,gen46.h,gen49.h,gen904.h,hsp77.h,RTEDiag_Appldiag.h,RTE_IniFileHandling.h,SAPDB_Names.h,geo908.h,RTE_UNIXAuthenticate.h,RTE_saveUNIXcalls.h,RTE_SdbStarter.h,heo12.h,RTESec_SAPSSL.h,RTESec_SSL.h,heo420.h
ven962.cpp -DSOCKETS                   inc=gen00.h,gsp01.h,hsp100.h,heo01.h,geo007_1.h,geo40.h,heo420.h,heo46.h,heo47.h,heo60.h,gen003.h,gen32.h,gen33.h,gen42.h,gen43.h,hen40.h,geo908.h,RTESec_SAPSSL.h,RTESec_SSL.h
ven964.cpp -DSOCKETS                   inc=gen00.h,gsp01.h,hsp100.h,heo01.h,geo007_1.h,geo40.h,heo420.h,heo46.h,heo47.h,heo60.h,gen003.h,gen32.h,gen33.h,gen42.h,gen43.h,hen40.h,geo908.h,RTESec_SSL.h,RTESec_SAPSSL.h,RTETask_ITaskClientConnection.hpp
# die TYPE-Check-Module VEN10[1-4] sollten wenigstens uebersetzbar sein
ven101                                 inc=gsp00,vsp001,geo002.h,gsp03,vsp0031,ggg11        trace=none
ven102                                 inc=gsp00,vsp001,geo002.h,vsp004,vsp005,vsp008,vin00 trace=none
ven103                                 inc=gsp00                                            trace=none
ven104                                 inc=gsp00                                            trace=none
# testcomreg main
ven780-t.cpp -DTEST                    inc=heo01.h,hen78.h,gen780.h
# dumpcomreg main
ven780.cpp                             inc=heo01.h,hen78.h,gen780.h
#
# control RTE
#
ven904.c  -DUNIX                       inc=gen00.h,gsp01.h,gsp09.h,geo001.h,geo57.h,gen904.h,RTE_GetDBRootOfDB.h,RTE_IniFileHandling.h,SAPDB_Names.h,heo05.h
ven905.c  -DSOCKETS                    inc=gen00.h,gsp09.h,hsp77.h,gen003.h,geo001.h,geo57.h,gen42.h,hen40.h,RTE_IniFileHandling.h,SAPDB_Names.h,gen904.h,heo46.h
ven906.c                               inc=gsp00,gen00.h,gen003.h,gsp09.h,geo001.h,geo57.h,RTE_UNIXAuthenticate.h,heo05.h
ven906-d.c -DDUMP_XLOGON_INFO          inc=gsp00,gen00.h,gen003.h,gsp09.h,geo001.h,geo57.h,RTE_UNIXAuthenticate.h,heo05.h
# clr_kernel
ven930.c  -DUSE_KGS -Dstrings          inc=gen00.h,gen50.h,geo007_1.h,heo07.h,hsp100.h,hen40.h
# clr_ps_ipc
ven931.c  -Dstrings                    inc=gen00.h,geo007_1.h,hen40.h,gen41.h

#
# This creates a prototype for SAPDB_Debug_Env file. You can copy this file to your
# running directory and enable dynamic debugging...
#
ven999Debug_Env.h concom=yes inc= ->$INSTROOT/env/SAPDB_Debug_Env
&endif  /* WIN32 */


