# @(#)   desc/fast/os.com   7.1   02/17/98
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

*                                 trace=none check=no
*-u       -DUSER                  trace=none check=no
*-s       -DSERVER                trace=none check=no
*-k       -DKERNEL   -Y           trace=none check=no
*-CAP     -D_FASTCAP              trace=none check=no
*-c       -DCOMPO                 trace=none check=no
*-w       -DWINCOMPO              trace=none check=no
*-x       -DASLIBRARY             trace=none check=no


*                            inc=gos00.h,heo46.h

gos00.h     inc=gsp00,gsp100.h,geo60.h,SAPDB_string.h,RTE_Types.h
gos003.h    inc=gsp00,gos00.h,gsp01.h,geo003.h
hos21u.h    inc=gsp00
hos13u.h    inc=gos00.h,hos21u.h
hos04u.h    inc=
hos06.h     inc=gsp01.h
hos23.h     inc=gos00.h,geo003.h
gos42.h     inc=gsp00
hos42.h     inc=gos00.h,geo003.h,gos42.h
gos44.h     inc=gsp01.h
gos38k.h    inc=-gos00.h,heo00.h,-heo46.h
gos38.h     inc=-gos00.h,heo00.h
gos39.h     inc=+ggg00,ggg92.h
gos41.h     inc=gsp00
hos38k.h    inc=gos00.h
hos38.h     inc=
hos37.h     inc=-gos00.h,heo46.h
hos33.h     inc=gos00.h
gos60.h     inc=SAPDB_Types.h
hos63k.h    inc=hos33.h
hos96.h     inc=gos00.h
gos74.h     inc=geo002.h,gos00.h,geo007_1.h,RTETask_LegacyTaskCtrl.h
gos00k.h    inc=gos00.h,vsp002c,heo61.h,geo52.h,geo00_2.h,geo00_3.h,RTE_Types.h,RTETask_LegacyQueueElement.h,RTETask_LegacyQueueElementFreeList.h
gos94.h     inc=gos00.h,gsp09.h
vos94.cpp   inc=gos00.h,RTE_NTService.hpp,RTE_MessageList.hpp,RTE_Messages.hpp,RTE_SpeedInfo.hpp,RTE_DBRegister.hpp,gsp09.h,heo44.h

gos00ds.h   inc=
gos00sec.h  inc=

vos01i.c  -DCOMPO  -uc -ansi inc=heo01.h,gos00.h,gos969.h,gsp09.h,gos94.h                        # INSTALL
vos01c.c  -DCOMPO  -uc -ansi                                                    # SystemRC

gos96.h    inc=geo00.h,gos003.h,hos23.h,hos33.h
vos96.c   -DSERVER -uc -ansi inc=gos00.h,gos96.h,geo43_0.h,geo007_1.h	-DRUN_AS_SERVICE    # SERVER

gos906.h   inc=geo906.h
gos960.h   inc=gos96.h
vos960.c  -DSERVER -uc -ansi inc=heo420.h,geo007_1.h,hos42.h,gos96.h,gos960.h,heo46.h,geo908.h,RTESec_SAPSSL.h,hsp100.h -DRUN_AS_SERVICE    # SERVER

gos962.h  inc=gos96.h
vos962.c  -DSERVER -uc -ansi inc=hsp100.h,geo007_1.h,geo40.h,heo420.h,geo43_0.h,hos42.h,gos96.h,gos962.h,heo46.h,geo908.h,RTESec_SAPSSL.h	-DRUN_AS_SERVICE    # SERVER

gos964.h  inc=gos96.h
vos964.c  -DSERVER -uc -ansi inc=hsp100.h,geo007_1.h,geo40.h,heo420.h,geo43_0.h,hos42.h,gos96.h,gos962.h,gos964.h,heo46.h,geo908.h,RTESec_SSL.h,RTESec_SAPSSL.h	-DRUN_AS_SERVICE    # SERVER

vos968.c -DSERVER -uc -ansi inc=hsp100.h,geo007_1.h,geo007_2.h,heo01.h,geo43_0.h,gos96.h,gos960.h,gos962.h,gos964.h,gos969.h,heo670.h,geo40.h,SAPDB_Names.h   -DRUN_AS_SERVICE   # SERVER

gos969.h                                inc=gos00.h,gos41.h
vos969.cpp                              inc=gos969.h,hsp100.h,geo007_1.h,heo01.h
vos969-w.cpp -DCLUSTER_SERVER_VERSION   inc=gos969.h,hsp100.h,geo007_1.h,heo01.h \
                                         dep=gos200.h,gos212.h

vos87c.c  -DCOMPO  -uc -ansi -x=ntcpc                                           # DBTIMES
vos88c.c  -DCOMPO  -uc -ansi -x=ntcpc                                           # DBPARAMS
vos89c.c  -DCOMPO  -uc -ansi inc=+gos00k.h                                       # DEVINIT  OS/2 only
vos93c.c  -DCOMPO  -uc -ansi inc=vos00rxc                                       # SQLREXX  OS/2 only
vos94c.c  -DCOMPO  -uc -ansi inc=+geo007_1.h,geo007_2.h,heo05.h,gos94.h         # SERVICE  Win32 only

vos97c.c  -DCOMPO  -uc -ansi inc=+gsp09.h,geo007_2.h,RTE_GetDBRootOfDB.h,SAPDB_string.h,SAPDB_sprintf.h,heo01.h        # SHOW
vos98c.c  -DCOMPO  -uc -ansi inc=+heo11.h,geo007_1.h,geo007_2.h,hos21u.h        # PUTPARAM

# RTE COMPONENTS
vos60-c.c  -uc -ansi inc=+geo007_1.h,heo00.h,geo007_2.h,gos44.h,heo01.h,geo00_2.h,RTESys_Time.h,RTE_DiagPipe.h,hsp77.h,SAPDBErr_MessageOutput.h

# WINDOWS COMPONENTS
vos60-w.c  -uc -ansi inc=+geo007_1.h,heo00.h,geo007_2.h,gos44.h,heo01.h,geo00_2.h,RTESys_Time.h,RTE_DiagPipe.h,hsp77.h,SAPDBErr_MessageOutput.h

# COMMON ASM.
vos76a

#
#  TCP/IP
#
vos40l.c   -uc -ansi inc=+geo007_1.h,hsp100.h,heo01.h,heo00.h
vos40-n.c  -uc -ansi inc=+gos003.h,hsp100.h  -DNO_SOCKET
vos40-1.c  -uc -ansi inc=+gos003.h,hsp100.h
vos40-2.c  -uc -ansi inc=+gos003.h,hsp100.h  -DNO_SOCKET_DELAY             

#
# SERVER
#
vos01-s.c -uc -ansi inc=+heo01.h,heo46.h,geo007_1.h,heo00.h
vos02-s.c -uc -ansi inc=+geo57.h,geo001.h,heo00.h
vos09-s.c -uc -ansi
vos23-s.c -uc -ansi inc=+gsp09.h,heo420.h,gos003.h,hos23.h,hos42.h,geo007_1.h,geo007_2.h,geo43_0.h
vos03-s.c -uc -ansi inc=+geo007_1.h,heo00.h
vos33-s.c -uc -ansi inc=+gos003.h,hos33.h,geo007_1.h,geo007_2.h,heo420.h,gos41.h,hsp100.h,RTE_KSS.h,RTESys_MemoryBarrier.h
vos41-s.c -uc -ansi inc=+geo007_1.h,gos41.h,heo00.h
vos42-s.c -uc -ansi inc=+gos003.h,geo007_1.h,geo007_2.h,heo420.h,hos42.h,heo00.h
vos43-s.c -uc -ansi inc=+geo007_1.h,geo007_2.h,gos003.h,hsp100.h,geo43_0.h,heo00.h
vos44-s.c -uc -ansi inc=+geo007_1.h,geo007_2.h,gos44.h,heo00.h,SAPDB_string.h
vos49-s.c -uc -ansi inc=+geo007_1.h,gos00sec.h,heo00.h
vos50-s.c -uc -ansi inc=+RTE_Types.h,heo01.h
vos57-s.c -uc -ansi inc=+geo007_1.h,heo00.h
vos60-s.c -uc -ansi inc=+geo007_1.h,heo00.h,geo007_2.h,gos44.h,heo01.h,geo00_2.h,RTESys_Time.h,RTE_DiagPipe.h,hsp77.h,SAPDBErr_MessageOutput.h
vos80-s.c -uc -ansi 

#
#  USER
#
vos01-u.c -uc -ansi inc=+heo01.h,heo46.h,geo007_1.h,heo00.h
vos02-u.c -uc -ansi inc=+geo57.h,geo001.h,heo00.h
vos06-u.c -uc -ansi inc=+geo007_1.h,geo007_2.h,hos06.h,gsp01.h,heo46.h,gos44.h,heo00.h
vos03-u.c -uc -ansi inc=+geo007_1.h,heo00.h
vos09-u.c -uc -ansi 
vos11-u.c -uc -ansi inc=+geo007_1.h,geo007_2.h,heo11.h,heo11_1.h,heo00.h,RTE_EnvironmentVariables.h,RTESys_Time.h

vos23-u.c -uc -ansi inc=+heo420.h,gos003.h,gsp09.h,hos23.h,hos42.h,geo007_1.h,geo007_2.h,geo43_0.h
vos41-u.c -uc -ansi inc=+geo007_1.h,gos41.h,heo00.h
vos42-u.c -uc -ansi inc=+geo007_1.h,geo007_2.h,gos003.h,heo420.h,hos42.h,heo00.h
vos43-u.c -uc -ansi inc=+geo007_1.h,geo007_2.h,gos003.h,hsp100.h,geo43_0.h,heo00.h
vos44-u.c -uc -ansi inc=+geo007_1.h,geo007_2.h,gos44.h,heo00.h,SAPDB_string.h
vos49-u.c -uc -ansi inc=+geo007_1.h,gos00sec.h,heo00.h
vos50-u.c -uc -ansi inc=+RTE_Types.h,heo01.h
vos57-u.c -uc -ansi inc=+geo007_1.h,heo00.h
vos60-u.c -uc -ansi inc=+geo007_1.h,geo007_2.h,gos44.h,heo01.h,geo00_2.h,RTESys_Time.h,hsp77.h,SAPDBErr_MessageOutput.h,heo00.h
vos33-u.c -uc -ansi inc=heo46.h,gos003.h,hos33.h,geo007_1.h,geo007_2.h,heo420.h,gos41.h,hsp100.h,gos00.h,RTE_KSS.h,RTESys_MemoryBarrier.h
vos80-u.c -uc -ansi 


vos97-u.c -uc -ansi inc=RTEConf_ParameterAccessSimple.h,geo007_1.h,gsp09.h,gos41.h,gos00.h,heo46.h,heo01.h

vos01u.c   -DUSER -uc -ansi inc=+geo007_1.h,geo007_2.h,geo572.h,heo01.h,geo00.h,gos60.h,SAPDBErr_MessageOutput.h,:SAPDB/RunTime/Security/Crypto/RTECrypto_EntropyPool.h
vos02u.c   -DUSER -uc -ansi 
vos03u.cpp -DUSER -uc -ansi inc=+geo007_1.h,geo007_2.h,gos003.h,geo003.h,heo03.h,hos23.h,hos33.h,geo40.h,geo401.h,geo03.h,geo43_0.h,RTESys_SystemInfo.h\
				,RTESec_SSL.h,RTESec_SAPSSL.h,RTESys_MicroTime.h,RTETask_ITaskClientConnection.hpp,heo420.h
vos04u                     inc=gsp00
vos04.c    -DUSER -uc -ansi inc=+geo007_1.h,gos00ds.h,hos04u.h  -DVIO_KBD
vos05u.c   -DUSER -uc -ansi inc=+geo007_1.h,geo007_2.h
vos06u.c   -DUSER -uc -ansi inc=+geo007_1.h,geo007_2.h,hos06.h,gos44.h
vos07u.c   -DUSER -uc -ansi inc=+geo572.h
vos08u.c   -DUSER -uc -ansi inc=+geo007_1.h,geo007_2.h,gos44.h
vos09u.c   -DUSER -uc -ansi inc=+gos00ds.h  # Windows NT only
vos10u.c   -DUSER -uc -ansi
vos12u.c   -DUSER -uc -ansi inc=gos00.h,heo46.h,hsp78_0.h,hsp83.h,geo13.h,heo13.h,hos13u.h,geo007_1.h,geo007_2.h,heo12.h
vos13u.c   -DUSER -uc -ansi inc=+heo13.h,geo13.h,hos13u.h,geo007_1.h,geo007_2.h,geo43_0.h  -DUSE_REGISTRY_DATABASE
vos14u.c   -DUSER -uc -ansi inc=+geo43_0.h
vos17u.c   -DUSER -uc -ansi inc=+gos003.h
vos18u.c   -DUSER -uc -ansi inc=+vsp004,geo13.h,+hos13u.h
vos21u.c   -DUSER -uc -ansi inc=+hos21u.h
vos24u.c   -DUSER -uc -ansi inc=+geo007_1.h,geo007_2.h
vos40u.c   -DUSER -uc -ansi inc=+geo007_1.h,geo007_2.h
vos57u.c   -DUSER -uc -ansi
vos69u.c   -DUSER -uc -ansi


#
#  KERNEL
#
&if $MACHINE in [ IA64 ]
vos38a      -DKERNEL -Xunwind
&endif
vos01-t.c  -DOMSTST -uc -ansi inc=+heo01.h,heo46.h,geo007_1.h,heo00.h


vos01-k.c  -uc -ansi inc=+heo01.h,heo46.h,geo007_1.h,heo00.h
vos01-k.c  -uc -ansi inc=+heo01.h,heo46.h,geo007_1.h
vos02-k.c  -uc -ansi inc=+geo57.h,geo001.h,gos00k.h,heo00.h
vos03-k.c  -uc -ansi inc=+geo007_1.h,heo00.h
vos06-k.c  -uc -ansi inc=+gos00k.h,geo007_1.h,geo007_2.h,hos06.h,gsp01.h,heo46.h,gos44.h,heo00.h
vos11-k.c  -uc -ansi inc=+gos00k.h,geo007_1.h,geo007_2.h,heo11.h,heo11_1.h,heo00.h,RTE_EnvironmentVariables.h,RTESys_Time.h
vos41-k.c  -uc -ansi inc=+gos00k.h,geo007_1.h,gos41.h,heo00.h
vos42-k.c  -uc -ansi inc=+gos003.h,geo007_1.h,geo007_2.h,gos00k.h,heo420.h,hos42.h,heo00.h
vos43-k.c  -uc -ansi inc=+gos00k.h,geo007_1.h,geo007_2.h,gos003.h,hsp100.h,geo43_0.h,heo00.h
vos44-k.c  -uc -ansi inc=+gos00k.h,geo007_1.h,geo007_2.h,gos44.h,heo00.h,SAPDB_string.h
vos49-k.c  -uc -ansi inc=+gos00k.h,geo007_1.h,gos00sec.h,heo00.h
vos50-k.c  -uc -ansi inc=+gos00k.h,RTE_Types.h,heo01.h
vos60-k.c  -DKERNEL -DNEW_DIAG_FILES -uc -ansi inc=+gos00k.h,heo00.h,geo007_1.h,geo007_2.h,gos44.h,heo01.h,geo00_2.h,RTESys_Time.h,RTE_DiagPipe.h,SAPDBErr_MessageOutput.h,hsp77.h \
						dep=hsp100.h,RTE_Speed.h,geo67.h,Msg_OutputKnlDiag.h

vos01k.c    -DKERNEL -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,gos00k.h,gos41.h
vos36.cpp   -DKERNEL -Y -uc -ansi inc=vsp001,geo00d.h,hsp100.h,heo36.h,heo38.h,gos00.h,heo01.h,LVC_LoadLibrary.hpp -D_WIN32_DCOM -DOMS_IMPLEMENTATION
vos36-d.cpp          -Y -uc -ansi inc=vsp001,geo00d.h,hsp100.h,heo36.h,heo38.h,gos00.h,heo01.h,LVC_LoadLibrary.hpp -D_WIN32_DCOM -DOMS_IMPLEMENTATION  
vos37.cpp            -Y -uc -ansi inc=ggg00,gsp00,vsp001,hos37.h,SAPDB_string.h -D_WIN32_DCOM  
&if $_MSC_VER <= 13
vos38.cpp    -DUSE_KGS -Y -uc -ansi inc=gsp00,hsp100.h,geo00d.h,geo007_1.h,heo02x.h,heo36.h,gos00k.h,gos00.h,hos38.h,geo38.h,hak341.h,heo38.h,heo670.h,RTESync_Spinlock.hpp,geo573.h,LVC_Dispatcher.hpp,LVC_LoadLibrary.hpp -D_WIN32_DCOM -GX -DOMS_IMPLEMENTATION
vos38-d.cpp          -Y -uc -ansi inc=-heo00.h,heo46.h,+gsp00,hsp100.h,heo02x.h,heo36.h,geo007_1.h,gos00k.h,geo00d.h,hos38.h,geo38.h,hak341.h,heo38.h,heo670.h,RTESync_Spinlock.hpp,geo573.h,LVC_Dispatcher.hpp,LVC_LoadLibrary.hpp -D_WIN32_DCOM -GX -DOMS_IMPLEMENTATION
&else
vos38.cpp    -DUSE_KGS -Y -uc -ansi inc=gsp00,hsp100.h,geo00d.h,geo007_1.h,heo02x.h,heo36.h,gos00k.h,gos00.h,hos38.h,geo38.h,hak341.h,heo38.h,heo670.h,RTESync_Spinlock.hpp,geo573.h,LVC_Dispatcher.hpp,LVC_LoadLibrary.hpp -D_WIN32_DCOM -EHsc -DOMS_IMPLEMENTATION
vos38-d.cpp          -Y -uc -ansi inc=-heo00.h,heo46.h,+gsp00,hsp100.h,heo02x.h,heo36.h,geo007_1.h,gos00k.h,geo00d.h,hos38.h,geo38.h,hak341.h,heo38.h,heo670.h,RTESync_Spinlock.hpp,geo573.h,LVC_Dispatcher.hpp,LVC_LoadLibrary.hpp -D_WIN32_DCOM -EHsc -DOMS_IMPLEMENTATION
&endif
vos50k.c     -DKERNEL -DNEW_DIAG_FILES -Y -uc -ansi inc=heo00.h,heo46.h,geo002.h,geo007_1.h,gos00k.h,gos003.h,heo54k.h,hsp100.h,heo15.h,heo670.h,gos41.h,RTEMem_AWEAllocator.h,RTE_KSS.h,Msg_OutputKnlDiag.h,gos60.h,RTEConf_ParameterAccessSimple.h,KernelParam_RunTime.h,SAPDB_Names.h
vos50k-CAP.c -DKERNEL -Y -uc -ansi                  inc=heo00.h,heo46.h,geo002.h,geo007_1.h,gos00k.h,gos003.h,heo54k.h,hsp100.h,heo15.h,heo670.h,gos41.h,RTEMem_AWEAllocator.h,RTE_KSS.h,Msg_OutputKnlDiag.h,gos60.h,RTEConf_ParameterAccessSimple.h,KernelParam_RunTime.h,SAPDB_Names.h
vos51k.cpp   -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=gos00.h,heo00.h,heo46.h,geo007_1.h,gos00k.h,geo002.h,heo79.h,gos003.h,heo56.h,heo15.h,heo69.h,heo92.h,heo15.h,gos41.h,RTE_KSS.h,RTE_KernelMain.h,RTE_SpawnRestartShutdown.h,RTE_DiagPipe.h,heo51.h,RTETask_Task.hpp,RTE_Version.h,RTE_Message.hpp
vos52k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,heo52.h,RTESys_Time.h,heo00.h
vos53k.cpp   -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,geo007_1.h,gos00k.h,geo002.h,gos003.h,heo52.h,geo67.h,gos74.h,heo56.h,RTESys_MicroTime.h,gos41.h,RTETask_TaskScheduler.hpp,RTETask_Task.hpp,RTESys_MemoryBarrier.h,hsp77.h,SAPDB_Names.h,heo420.h,RTEConf_ParameterAccess.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTE_KGS.hpp,RTECKC_KernelCommunication.hpp
&if $LC_STATE == "DEV"
# vgetapplipid will produce emergency shutdown for illegal task id given (but only in DEV version)
# Reason: It produces only output values and an illegal output value can be ignored...
vos53k.cpp -DSDB_DEV_ONLY_CHECK
&endif

&if $MACH == AMD64
vos54.c      -DKERNEL -DDEVELOP_CHECKS -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,geo007_2.h,geo50_0.h,geo002.h,heo01.h,heo58.h,heo54k.h,gos74.h,heo52.h,gos44.h,geo54.h,geo00_1.h,RTESys_MicroTime.h,gos41.h,RTEHSS_PascalInterface.h,RTETask_LegacyTaskCtrl.h
vos54-CAP.c  -DKERNEL -DDEVELOP_CHECKS -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,geo007_2.h,geo50_0.h,geo002.h,heo01.h,heo58.h,heo54k.h,gos74.h,heo52.h,gos44.h,geo54.h,geo00_1.h,RTESys_MicroTime.h,gos41.h,RTEHSS_PascalInterface.h,RTETask_LegacyTaskCtrl.h
&else
vos54.c      -DKERNEL -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,geo007_2.h,geo50_0.h,geo002.h,heo01.h,heo58.h,heo54k.h,gos74.h,heo52.h,gos44.h,geo54.h,geo00_1.h,RTESys_MicroTime.h,gos41.h,RTEHSS_PascalInterface.h,RTETask_LegacyTaskCtrl.h
vos54-CAP.c  -DKERNEL -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,geo007_2.h,geo50_0.h,geo002.h,heo01.h,heo58.h,heo54k.h,gos74.h,heo52.h,gos44.h,geo54.h,geo00_1.h,RTESys_MicroTime.h,gos41.h,RTEHSS_PascalInterface.h,RTETask_LegacyTaskCtrl.h
&endif
vos55k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,geo002.h,heo55k.h,gos74.h,geo50_0.h,RTETask_LegacyTaskCtrl.h,heo00.h
vos56k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,geo50_0.h,geo002.h,heo56.h,heo52.h,geo00_2.h,gos74.h,RTETask_LegacyTaskCtrl.h,heo00.h
vos57k.c     -DKERNEL -Y -uc -ansi inc=+geo50_0.h,geo002.h,geo007_1.h,heo57.h,-heo46.h
vos58k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,geo002.h,heo54k.h,gos44.h,heo00.h
vos59k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,heo00.h
vos60k.c     -DKERNEL -DNEW_DIAG_FILES -Y -uc -ansi inc=+geo007_1.h,gos00k.h,gos44.h,heo00.h,Msg_OutputKnlDiag.h
vos61k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,heo00.h
vos64k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,geo002.h,gos003.h,gos74.h,RTETask_LegacyTaskCtrl.h,RTETask_LegacyUKTCtrl.h,heo00.h
vos65k.cpp     -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,geo007_2.h,geo50_0.h,geo002.h,heo52.h,gos44.h,geo54.h,geo00_1.h,gos74.h,gos41.h,RTETask_LegacyTaskCtrl.h,RTETask_CommonQueueElement.hpp
vos65k-CAP.cpp -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,geo007_2.h,geo50_0.h,geo002.h,heo52.h,gos44.h,geo54.h,geo00_1.h,gos74.h,gos41.h,RTETask_LegacyTaskCtrl.h,RTETask_CommonQueueElement.hpp

vos66k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,heo00.h,geo007_2.h,gos00k.h,hos06.h,gos44.h,geo002.h
vos68k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,gos003.h,heo00.h
vos69k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,heo00.h,gos00k.h,heo52.h
vos70k.cpp   -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=+geo007_1.h,heo00.h,gos00k.h,geo002.h,gos003.h,hos63k.h,\
    geo70k.h,geo52.h,heo15.h,RTEMem_PageAllocator.h,geo00_1.h,RTEMem_AWEAllocator.hpp,RTETask_Task.hpp,\
    RTETask_TaskScheduler.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTETask_LegacyTaskCtrl.h,\
    RTETask_CommonQueueElement.hpp,RTETask_CommonQueue.hpp,RTE_KGS.hpp,RTECKC_KernelCommunication.hpp,\
    RTESys_SystemInfo.h,RTEDiag_SymbolResolutionLock.h
vos71ka      -DKERNEL
vos71k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,geo002.h,geo70k.h,heo00.h
vos72k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,heo00.h
vos73k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,geo002.h,heo00.h,RTESys_MemoryBarrier.h
#vos78.cpp                         inc=gos00.h,heo01.h,heo78.h,hsp100.h
vos74k.cpp   -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,geo50_0.h,geo002.h,gos003.h,heo52.h,heo69.h,heo92.h,heo670.h,gos74.h,RTESys_MicroTime.h,gos41.h,geo00_1.h,geo00_3.h,Trace_Manager.h,RTETask_TaskScheduler.hpp,RTETask_Task.hpp,RTETask_LegacyUKTCtrl.h,RTETask_LegacyTaskCtrl.h,RTE_KGS.hpp,RTESys_MemoryBarrier.h
vos79k.c     -DKERNEL -Y -uc -ansi inc=+geo007_1.h,heo00.h,gos00k.h,geo002.h,heo11.h,geo002.h,gos44.h,RTEConf_ParameterAccessKernelInterface.h
vos80k.c     -DKERNEL -Y -uc -ansi inc=heo00.h,heo46.h,hsp100.h,geo007_1.h,gos00k.h,gos41.h,gos60.h,SAPDBErr_MessageOutput.h,RTE_DiagPipe.h,hsp100.h,RTETask_Task.h
vos80kn.c    -DKERNEL -Y -uc -ansi inc=+gos00k.h,geo002.h,gos003.h,geo007_1.h,geo007_2.h,heo00.h,RTE_DiagPipe.h
vos81.cpp    -DKERNEL  -Y -uc -ansi inc=RTE_Types.h,heo00.h,heo46.h,gos00k.h,geo002.h,gos003.h,geo003.h,hsp100.h,geo007_1.h,geo67.h,gos44.h,heo15.h,geo43_0.h,hsp77.h,Trace_InfoPage.h,RTEDiag_ServerCrashHist.h,RTESys_NTLoadDLLFunc.h,heo54k.h,RTESys_NTHighResTimer.h,gos41.h,RTE_KernelMain.h\
                                      ,RTEThread_Console.h,RTE_KSS.h,RTEConf_ParameterAccessKernelInterface.h,RTEConf_ParameterAccessKernelInterface.hpp,RTEConf_ParameterAccessSimple.h,SAPDB_Names.h,RTEThread_Requestor.hpp,RTETask_WorkerPool.hpp,RTE_MessageList.hpp,RTE_Crash.hpp,RTETst_Default.hpp,Kernel_ExtendedState.hpp,RTE_Version.h,RTESys_SystemInfo.h,KernelParam_RunTime.h
vos83k.cpp   -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,gos00k.h,geo002.h,gos003.h,gos41.h,RTEThread_ConsoleConnections.h,RTE_KSS.h,RTE_DiagPipe.h,RTE_KernelMain.h,RTE_SpawnRestartShutdown.h,RTETask_Task.hpp
vos83k-CAP.cpp -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,gos00k.h,geo002.h,gos003.h,gos41.h,RTEThread_ConsoleConnections.h,RTE_KSS.h,RTETask_Task.hpp
vos84k.cpp     -DKERNEL -Y -uc -ansi inc=heo00.h,heo46.h,gsp01.h,geo007_1.h,gos00k.h,geo002.h,gos003.h,hos63k.h,gos41.h,geo00_1.h,geo00_3.h,RTECKC_KernelCommunication.hpp,RTE_KGS.hpp,RTETask_Task.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTEConf_ParameterAccessKernelInterface.h
vos84k-CAP.cpp -DKERNEL -Y -uc -ansi inc=heo00.h,heo46.h,gsp01.h,geo007_1.h,gos00k.h,geo002.h,gos003.h,hos63k.h,gos41.h,geo00_1.h,geo00_3.h,RTECKC_KernelCommunication.hpp,RTE_KGS.hpp,RTETask_Task.hpp,RTEConf_ParameterAccessKernelInterface.hpp,RTEConf_ParameterAccessKernelInterface.h
vos85k.c       -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,heo00.h
vos85k-CAP.c   -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h
vos88k.cpp     -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,gos00k.h,geo002.h,heo38.h,heo69.h,RTESys_MicroTime.h,gos41.h,gos74.h,RTETask_LegacyUKTCtrl.h,RTESys_SystemInfo.h,RTETask_Task.hpp
vos88k-CAP.cpp -DKERNEL -DKERNEL_LZU -Y -uc -ansi inc=heo00.h,heo46.h,geo007_1.h,gos00k.h,geo002.h,heo38.h,heo69.h,RTESys_MicroTime.h,gos41.h,gos74.h,RTETask_LegacyUKTCtrl.h,RTESys_SystemInfo.h,RTETask_Task.hpp
vos89k.c       -DKERNEL -Y -uc -ansi inc=+geo007_1.h,gos00k.h,gos003.h

# control components
vos905.cpp -DUSER -ansi inc=+vsp004,gsp09.h,gos003.h,geo007_1.h,geo007_2.h,gos906.h,hsp77.h,SAPDBMem_NewDestroy.hpp
vos906.c        -ansi inc=+vsp001c,gsp09.h,geo007_1.h,gos003.h,gos96.h,heo420.h,heo44.h,gos906.h

vos97c-x.c -DCTRLCOMP -uc -ansi inc=+vsp004,gsp09.h,heo05.h,geo007_2.h,RTE_GetDBRootOfDB.h,SAPDB_string.h,SAPDB_sprintf.h,heo01.h
vos97c-s.c -DCTRLCOMP -uc -ansi inc=+vsp004,gsp09.h,heo05.h,geo007_2.h,RTE_GetDBRootOfDB.h,SAPDB_string.h,SAPDB_sprintf.h,heo01.h -DRUN_AS_SERVICE
vos50-d.c  -uc -ansi -MD inc+=RTE_Types.h,heo01.h
vos97c-i.c -DCTRLCOMP -DENUMINST_ONLY -uc -ansi inc=+vsp004,gsp09.h,heo05.h,geo007_2.h,RTE_GetDBRootOfDB.h,SAPDB_string.h,SAPDB_sprintf.h,heo01.h
vos97c-d.c -DCTRLCOMP -DENUMINST_ONLY -MD -uc -ansi inc=+vsp004,gsp09.h,heo05.h,geo007_2.h,RTE_GetDBRootOfDB.h,SAPDB_string.h,SAPDB_sprintf.h,heo01.h
vos99d.c   -DCTRLCOMP -uc -ansi inc=

# used within PLIBOMS.DLD
vos50-p.c    -DLIBOMS -uc -ansi inc=RTE_Types.h,heo01.h


# microsoft cluster server
vos90-w.cpp     -uc -ansi -DCLUSTER_SERVER_VERSION                  inc=geo007_1.h,geo007_2.h,hsp100.h,gos212.h,gos41.h,heo00.h,heo46.h,SAPDBErr_MessageList.hpp,RTE_ActiveDatabase.hpp,RTE_DBRegister.hpp 

gos200.h		inc=
gos202.h		inc=gos203.h,geo60.h,geo200.h,geo007_1.h
gos203.h		inc=geo60.h,geo200.h
gos204.h		inc=gos203.h,gos205.h,geo007_1.h,gos00.h,geo60.h,geo200.h
gos205.h		inc=geo200.h,geo60.h

gos210.h		inc=
gos212.h		inc=

gos220.h		inc=
gos221.h		inc=gos229.h
gos222.h		inc=gos224.h,gos225.h
gos223.h		inc=
gos224.h		inc=
gos225.h		dep=vos228.idl inc=
gos226.h		inc=
gos227.h		inc=gos222.h,gos224.h 
gos229.h		inc=

vos228.idl   unpack=no noversion -DMIDL_PASS obj=vos228_i.o \
             inc= -header=$WRK/incl/vos228_i.h extdep=$WRK/incl/vos228_i.h

vos202-l.cpp  -uc -ansi -except                                   inc=geo200.h,gos200.h,geo60.h,gos202.h,gos204.h,gos205.h,geo007_1.h -DCREATE_DLL_LOADER
vos202-w.cpp  -uc -ansi -except -DUNICODE                         inc=geo200.h,gos200.h,geo60.h,gos202.h,gos204.h,gos205.h,geo007_1.h -DCREATE_DLL

vos203.cpp    -uc -ansi -except                                   inc=geo001.h,geo007_1.h,geo60.h,geo200.h,gos203.h
vos203-w.cpp  -uc -ansi -except -DUNICODE                         inc=geo001.h,geo007_1.h,geo60.h,geo200.h,gos203.h
vos204-w.cpp  -uc -ansi -except -DUNICODE                         inc=geo007_1.h,gos204.h,gos00.h,gos200.h,geo200.h,geo60.h
vos205-w.cpp  -uc -ansi -except -DUNICODE                         inc=geo007_1.h,geo200.h,geo60.h,gos205.h,geo60.h

vos210-w.c    -uc -ansi         -DUNICODE                         inc=geo007_2.h,gos00.h,gos200.h,gos210.h,geo60.h
vos211.cpp    -uc -ansi -except                                   inc=geo81.h,gos202.h,geo60.h,gos00.h,geo007_2.h
vos212.cpp    -uc -ansi                                           inc=geo60.h,gos202.h,gos212.h


vos221.cpp    -uc -ansi -except -MD -D_WINDLL -D_AFXDLL -D_USRDLL dep=vos228.idl inc=gos200.h,gos220.h,gos221.h,gos222.h,gos225.h,gos226.h 
vos222.cpp    -uc -ansi -except -MD -D_WINDLL -D_AFXDLL -D_USRDLL inc=gos220.h,gos200.h,gos221.h,gos222.h,gos225.h
vos223.cpp    -uc -ansi -except -MD -D_WINDLL -D_AFXDLL -D_USRDLL inc=gos220.h,gos223.h,gos229.h
vos224.cpp    -uc -ansi -except -MD -D_WINDLL -D_AFXDLL -D_USRDLL inc=gos220.h,gos224.h
vos225.cpp    -uc -ansi -except -MD -D_WINDLL -D_AFXDLL -D_USRDLL inc=gos220.h,gos221.h,gos225.h,gos227.h,gos200.h,gos00.h
vos226.cpp    -uc -ansi -except -MD -D_WINDLL -D_AFXDLL -D_USRDLL inc=gos220.h
vos227.cpp    -uc -ansi -except -MD -D_WINDLL -D_AFXDLL -D_USRDLL inc=gos220.h,gos221.h,gos227.h,gos225.h,gos223.h,gos200.h,gos00.h
