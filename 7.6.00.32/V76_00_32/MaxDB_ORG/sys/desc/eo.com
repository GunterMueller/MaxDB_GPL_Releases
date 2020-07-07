# @(#)   desc/eo.com   7.2.0   08/25/98
#
# If you add a description, sort!!!!
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

*-d -ansi -DDUMMY_MODULE
*-l -DDBROOT_PATHES_ONLY
*-b -DDBROOT_PATHES_ONLY
*-k -DKERNEL_LZU
*-c -DCONSOLE
*-dir -DDIRECTORY_OPS_ONLY

#----------------------------
&if $OS in [ WIN32 ]
#----------------------------
#*      inc=vsp00c,vsp002,heo00.h,vos00spc,vos00c trace=none check=no
* -ansi trace=none check=no

geo00_0.h                                    inc= \
                                              dep=gos00.h
geo29.h                                      inc=gsp00
geo50_0.h                                    inc= \
                                              dep=gos00k.h
geo60.h                                      inc=geo00.h,gsp00_0.h,geo47.h

heo01x.h                                     inc=gsp00
heo36.h                                      inc=
heo38.h                                      inc=gsp00,OMS_Defines.h
geo57.h                                      inc= \
                                              dep=gos00.h
heo52.h                                      inc=gsp00,geo52.h
heo78.h                                      inc=

heo00.h                                      inc=gsp00
veo02-m.c -MD                                inc=heo02x.h,gip00.h,hsp77.h,SAPDB_string.h
veo06.c                                      inc=gsp00,gos00.h,vsp002c,geo007_2.h,heo01.h,hos06.h,geo007_1.h,gsp01.h,heo46.h,gos44.h,heo06.h,hsp78_0.h,SAPDB_string.h,RTE_EnvironmentVariables.h,geo54.h
veo07-k.c                                    inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,SAPDB_sprintf.h \
                                              dep=geo50_0.h
veo11.c                                      inc=hsp100.h,heo11.h,heo11_1.h,heo46.h,heo47.h,geo60.h,RTEConf_BuildParamFileName.h,RTEConf_ParameterAccessSimple.h,RTESys_IO.h, \
                                              dep=gos44.h
veo13.c   -DPROTOTYPE                        inc=geo13.h,geo00_0.h,heo13.h,geo001.h,geo57.h,heo46.h,geo007_1.h,geo007_2.h,hsp100.h,hsp78_0.h,hsp02.h,SAPDB_string.h,heo21.h \
                                              dep=vsp002c,hos13u.h
veo15.cpp  -DKERNEL                          inc=gsp00,heo07.h,heo15.h,geo007_1.h,geo007_2.h,geo57.h,geo001.h,hsp77.h,gos00k.h,RTESys_IO.h,RTEConf_ParameterAccessKernelInterface.h,SAPDB_MemCopyMoveNoCheck.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTEThread_KernelThread.hpp,RTE_Crash.hpp,RTE_MessageList.hpp
veo15-t.cpp -DTEST -except                   inc=gsp00,heo07.h,heo15.h,geo007_1.h,geo007_2.h,geo57.h,geo001.h,hsp77.h,gos00k.h,RTESys_IO.h,RTEConf_ParameterAccessKernelInterface.h,SAPDB_MemCopyMoveNoCheck.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTEThread_KernelThread.hpp,RTE_Crash.hpp,RTE_MessageList.hpp
veo15test.cpp -except                        inc=gsp00,heo07.h,heo15.h,geo007_1.h,geo007_2.h,geo57.h,geo001.h,hsp77.h,hen40.h,RTESys_IO.h
veo29-u.cpp                                  inc=geo29.h,heo07.h,gos00.h
veo29-k.cpp -DKERNEL                         inc=geo29.h,heo07.h,gos00.h
veo38.cpp -DUSE_KGS -Y -uc -except           inc=LVC_IliveCacheSink.hpp,gsp00,ggg00,ggg01,hgg08.h,ggg92.h,heo01.h,heo51.h,heo52.h,heo55k.h,heo56.h,heo15.h,geo00d.h,geo007_1.h,heo58.h,geo60.h,hak071.h,hak34.h,hak341.h,hak42.h,geo50_0.h,geo38.h,\
                                             heo38.h,heo02x.h,ggg251.h,hgg01.h,geo573.h,heo922.h,OMS_Types.hpp,RTESys_IO.h,OMS_CallbackInterface.hpp,LVC_Dispatcher.hpp,\
                                             hsp77.h,SAPDBMem_NewDestroy.hpp,SAPDBMem_RawAllocator.hpp,LVCMem_Allocator.hpp,\
                                             LVC_LoadLibrary.hpp,OMS_VarObj.hpp,hos38.h,SAPDB_MemCopyMoveNoCheck.hpp,\
                                             LVC_ContainerManager.hpp,SQLMan_Context.hpp,Catalog_Types.hpp \
                                             dep=gos00.h,gos00k.h,hos38.h,heo56.h,geo60.h -DOMS_IMPLEMENTATION
veo42_0.c                                    inc=gsp00,gos00.h,vsp002c,vsp009c,heo01.h,heo102.h,heo05.h,SAPDB_string.h
veo42_1.c                                    inc=gsp00,gos00.h,vsp002c,vsp009c,heo01.h,heo102.h,heo05.h,SAPDB_string.h
veo420.c                                     inc=gsp00,geo003.h,geo007_1.h,geo007_2.h,geo40.h,heo46.h,heo47.h,heo420.h,geo60.h,SAPDB_string.h,RTE_Types.h \
                                              dep=gos42.h
veo420-k.c -DKERNEL_LZU                      inc=gsp00,geo003.h,geo007_1.h,geo007_2.h,geo40.h,heo46.h,heo47.h,heo420.h,geo60.h,SAPDB_string.h,RTE_Types.h \
                                              dep=gos42.h
veo43_0.c                                    inc=gsp00,vsp009c,heo05.h,gos00.h
veo44_0.c                                    inc=gsp00,vsp009c,heo05.h,gos00.h
veo44_0-d.c -MD                              inc=gsp00,vsp009c,heo05.h,gos00.h
veo49.c                                      inc=gsp00,geo49.h dep=gos00.h
veo69.cpp -DKERNEL_LZU                       inc=gsp00,heo69.h,geo00_0.h,geo50_0.h,geo002.h,geo007_1.h,RTE_CurrentStack.hpp
veo908.cpp -except              			 inc=gsp01.h,geo00_2.h,geo60.h,geo200.h,geo007_1.h,geo907.h,geo908.h,geo573.h,gos00.h
veo922.cpp  -DKERNEL_LZU                     inc=gsp100.h,heo922.h,geo00_0.h,geo50_0.h,geo002.h,geo57.h,heo47.h,\
    heo92.h,heo00x.h,RTEMem_AllocatorRegister.hpp,RTESync_NamedSpinlock.hpp,RTE_ConsoleStandardOutput.h,\
    RTETask_Task.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTEDiag_SymbolResolutionLock.h \
                                              dep=gos003.h

#**************************** ???????????  UNIX  ????????? *****************************
&else
#**************************** ???????????  UNIX  ????????? *****************************
*-s -ansi -DVSERVER
*   -ansi trace=none check=no

geo00_0.h                                    inc= \
                                              dep=gen00.h
geo29.h                                      inc=gsp00
geo50_0.h                                    inc= \
                                              dep=gen50.h
geo57.h                                      inc= \
                                              dep=gen57.h
geo60.h                                      inc=geo00.h,gsp00_0.h,geo47.h \
                                              dep=gen60.h
heo52.h                                      inc=gsp00,geo52.h

heo00.h                                      inc=gsp00
veo02-m.c                                    inc=heo02x.h,gip00.h,hsp77.h,SAPDB_string.h
veo06.c                                      inc=gsp00,geo007_2.h,heo01.h,gen00.h,heo06.h,geo54.h,heo602.h,hsp78_0.h,SAPDB_string.h,heo46.h,RTE_EnvironmentVariables.h
veo06-dir.c                                  inc=gsp00,heo00.h,geo007_2.h,heo01.h,gen00.h,heo06.h,geo54.h,heo602.h,hsp78_0.h,SAPDB_string.h,heo46.h,RTE_EnvironmentVariables.h
veo07-k.c   -DKERNEL_LZU                     inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,SAPDB_sprintf.h \
                                              dep=geo50_0.h,RTE_Coroutine.h,hen50.h
veo11.c                                      inc=hsp100.h,heo11.h,heo11_1.h,heo46.h,heo47.h,geo60.h,RTEConf_BuildParamFileName.h,RTEConf_ParameterAccessSimple.h,RTESys_IO.h \
                                              dep=RTE_saveUNIXcalls.h
veo13.c                                      inc=geo13.h,geo00_0.h,geo007_2.h,geo001.h,geo57.h,heo13.h,heo46.h,geo007_1.h,hsp100.h,hsp78_0.h,hsp02.h,SAPDB_string.h,heo21.h
veo15.cpp -DKERNEL -except                   inc=gsp00,heo07.h,heo15.h,geo007_1.h,geo007_2.h,geo57.h,geo001.h,hsp77.h,hen40.h,RTESys_IO.h,RTEConf_ParameterAccessKernelInterface.h,SAPDB_MemCopyMoveNoCheck.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTEThread_KernelThread.hpp,RTE_Crash.hpp,RTE_MessageList.hpp
veo15-t.cpp -DTEST -except                   inc=gsp00,heo07.h,heo15.h,geo007_1.h,geo007_2.h,geo57.h,geo001.h,hsp77.h,hen40.h,RTESys_IO.h,RTEConf_ParameterAccessKernelInterface.h,SAPDB_MemCopyMoveNoCheck.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,RTEThread_KernelThread.hpp,RTE_Crash.hpp,RTE_MessageList.hpp
veo15test.cpp -except                        inc=gsp00,heo07.h,heo15.h,geo007_1.h,geo007_2.h,geo57.h,geo001.h,hsp77.h,hen40.h,RTESys_IO.h
veo29-u.cpp                                  inc=geo29.h,heo07.h,gen00.h
veo29-k.cpp -DUSE_KGS                        inc=geo29.h,heo07.h,gen00.h
veo38.cpp -DUSE_KGS                          inc=LVC_IliveCacheSink.hpp,gsp00,ggg00,ggg01,hgg08.h,ggg92.h,heo01.h,heo51.h,heo52.h,heo55k.h,heo56.h,heo15.h,heo00.h,geo00d.h,geo007_1.h,geo50_0.h,heo58.h,geo60.h,hak071.h,\
                                             hak34.h,hak341.h,hak42.h,geo38.h,heo38.h,heo02x.h,ggg251.h,hgg01.h,geo573.h,heo922.h,OMS_Types.hpp,RTESys_IO.h,OMS_CallbackInterface.hpp,LVC_Dispatcher.hpp,\
                                             hsp77.h,SAPDBMem_NewDestroy.hpp,SAPDBMem_RawAllocator.hpp,LVCMem_Allocator.hpp,\
                                             LVC_LoadLibrary.hpp,OMS_VarObj.hpp,hos38.h,SAPDB_MemCopyMoveNoCheck.hpp,\
                                             LVC_ContainerManager.hpp,SQLMan_Context.hpp,Catalog_Types.hpp \
                                             dep=vsp002c,heo46.h,heo60.h,gen50.h,hen38.h,heo56.h,geo60.h -DOMS_IMPLEMENTATION
veo42_0.c                                    inc=gsp00,vsp009c,heo01.h,heo102.h,heo05.h,SAPDB_string.h
veo42_1.c                                    inc=gsp00,vsp009c,heo01.h,heo102.h,heo05.h,RTE_SdbStarter.h,SAPDB_string.h
veo420.c                                     inc=gsp00,geo003.h,geo007_1.h,geo007_2.h,geo40.h,heo46.h,heo47.h,heo420.h,geo60.h,SAPDB_string.h,RTE_Types.h
veo420-k.c -DKERNEL_LZU                      inc=gsp00,geo003.h,geo007_1.h,geo007_2.h,geo40.h,heo46.h,heo47.h,heo420.h,geo60.h,SAPDB_string.h,RTE_Types.h
veo43_0.c                                    inc=gsp00,vsp009c,heo05.h,gen00.h
veo44_0.c                                    inc=gsp00,vsp009c,heo05.h,gen00.h
veo44_0-d.c -MD                              inc=gsp00,vsp009c,heo05.h,gen00.h
veo49.c                                      inc=gsp00,geo49.h dep=RTE_UNIXAuthenticate.h
geo57.h                                      inc= \
                                              dep=gen57.h
veo69.cpp -DKERNEL_LZU                       inc=gsp00,heo69.h,geo00_0.h,geo50_0.h,geo002.h,geo007_1.h,RTE_CurrentStack.hpp \
                                              dep=hen50.h
veo908.cpp -except                           inc=gsp01.h,geo00_2.h,geo60.h,geo200.h,geo007_1.h,geo907.h,geo908.h,geo573.h,gen904.h
veo922.cpp  -DKERNEL_LZU                     inc=gsp100.h,heo922.h,geo00_0.h,geo50_0.h,geo002.h,geo57.h,heo47.h,\
    heo92.h,heo00x.h,RTEMem_AllocatorRegister.hpp,RTESync_NamedSpinlock.hpp,RTE_ConsoleStandardOutput.h,\
    RTETask_Task.hpp,SAPDB_MemCopyMoveNoCheck.hpp,RTEDiag_SymbolResolutionLock.h \
                                              dep=gen45.h,gen003.h

#**************************** ???????????????????????????? *****************************
&endif
#**************************** ???????????????????????????? *****************************

#**************************** ---------------------------- *****************************

geo00.h                                      inc=RTE_CompilerFeatures.h,SAPDB_Types.h
geo00d.h                                     dep=OMS_IliveCacheSpec.hpp,OMS_LiveCacheTypes.xml inc=LVC_IliveCacheSink.hpp,hsp77.h,SAPDB_string.h,SAPDB_MemCopyMoveNoCheck.hpp
geo00_2.h                                    inc=
geo00_1.h                                    inc=
geo001.h                                     inc=
geo002.h                                     inc=gsp00,vsp002,geo00_0.h,RTE_Types.h,KernelParam_RunTime.h,KernelParam_IOManager.hpp
# obsolete module
geo002xp.h                                   inc=
#
geo007_1.h                                   inc=geo47.h
geo007_2.h                                   inc=
geo003.h                                     inc=gsp00,geo001.h,heo01.h
geo03.h                                      inc=geo003.h,geo40.h,geo401.h
veo03.c                                      inc=gsp01.h,geo03.h,geo001.h,geo007_1.h,geo007_2.h,heo46.h,geo57.h,geo60.h,heo420.h,heo03.h,geo43_0.h,,SAPDB_string.h\
												 ,RTESec_SAPSSL.h\
&if $OSSPEC = WIN32					
												 ,hos23.h
&else
												 ,gen003.h,gen42.h
&endif												 							 
												 
geo07.h                                      inc=
geo13.h                                      inc=vsp004,gsp100.h
geo28.h                                      inc=gsp00,geo200.h
geo31.h                                      inc=gsp00
geo43.h                                      inc=gsp00,geo47.h
geo38.h                                      inc=LVC_IliveCacheSink.hpp,RTESync_Spinlock.hpp,LVC_DispatcherHashEntry.hpp,LVC_DispatcherCOMClassHash.hpp
geo40.h                                      inc=geo003.h
geo401.h				     inc=geo003.h
geo41.h                                      inc=geo200.h,gsp00
geo42.h                                      inc=geo003.h,geo200.h,geo60.h,geo41.h,geo007_1.h,gsp00,RTEComm_PacketHeader.h,SAPDB_MemCopyMoveNoCheck.hpp
geo43_0.h                                    inc=gsp00
geo44.h                                      inc=geo42.h,gsp00
geo45.h                                      inc=geo42.h,geo44.h
geo46.h                                      inc=geo42.h,geo45.h
geo47.h                                      inc=
geo49.h                                      inc=gsp00
geo52.h                                      inc=geo00.h
geo54.h                                      inc=gsp00
geo61.h                                      inc=gsp00,SAPDB_MemCopyMoveNoCheck.hpp
geo66.h                                      inc=gsp00
geo67.h                                      inc=gsp00,vsp0031
geo70k.h                                     inc=geo002.h
geo77.h                                      inc=
geo79.h                                      inc=gsp00,gsp01.h,geo002.h
geo80.h	                                     inc=geo47.h,geo60.h
geo81.h	                                     inc=geo80.h
geo102.h unpack=no                           inc=gsp00,gsp01.h,vsp004
geo110.h                                     inc=geo42.h,geo200.h
geo111.h                                     inc=geo42.h,geo110.h,geo200.h
geo200.h			                               inc=geo001.h,geo47.h,geo60.h,gsp00
geo570.h                                     inc=geo00_0.h,geo200.h
geo571.h                                     inc=
geo572.h                                     inc=
geo573.h                                     inc=
geo900.h                                     inc=geo001.h,geo00.h
geo900.hpp                                   inc=geo001.h,geo60.h
geo906.h                                     inc=geo001.h
geo907.h                                     inc=geo111.h,geo42.h,geo45.h
geo908.h                                     inc=
heo01.h                                      inc=gsp01.h
heo02.h	                                     inc=gsp00,vsp004,geo572.h
heo02x.h                                     inc=gsp00
heo03.h	                                     inc=gsp00,gsp01.h,vsp001,geo003.h
heo05.h                                      inc=gsp09.h
heo06.h                                      inc=gsp01.h,gsp05,hsp77.h
heo07.h                                      inc=gsp00,geo07.h
heo11.h                                      inc=gsp00,gsp01.h,gsp11.h,heo01.h
heo11_1.h                                    inc=
heo12.h                                      inc=geo00_0.h,gsp00
heo12_1.h                                    inc=gsp00
heo13.h                                      inc=gsp00,vsp004
heo15.h                                      inc=gsp00,geo00_0.h,heo07.h,RTESys_IO.h
heo16.h                                      inc=gsp00
heo21.h                                      inc=gsp00
heo35.h                                      inc=
heo36.h                                      inc=
heo38.h                                      inc=gsp00,OMS_Defines.h
heo41.h                                      inc=
heo46.h                                      inc=gsp00,gsp01.h
heo47.h                                      inc=geo00.h
heo50.h                                      inc=gsp00
heo51.h                                      inc=gsp00,vsp001,vsp002
heo54k.h                                     inc=gsp00,vsp002
heo55k.h                                     inc=gsp00
heo56.h                                      inc=gsp00
heo56k.h                                     inc=heo56.h
heo57.h                                      inc=gsp00
heo58.h                                      inc=gsp00,gsp200,vsp002,geo002.h,vsp0058
heo60.h                                      inc=gsp00,gsp03,vsp0031,heo60_1.h
heo60_1.h                                    inc=gsp00
heo60_2.h                                    inc=gsp00,gsp03
heo61.h                                      inc=gsp00,geo47.h
heo66.h                                      inc=gsp00,geo66.h
heo67.h                                      inc=gsp00,vsp0031
heo68.h                                      inc=gsp00
heo69.h                                      inc=gsp00,RTE_Types.h
heo77.h                                      inc=geo77.h
heo78.h                                      inc=
heo79.h                                      inc=gsp00,heo11.h,heo79_1.h
heo79_1.h                                    inc=gsp00
heo92.h                                      inc=geo002.h,geo00_2.h,geo50_0.h,RTETask_SuspendReason.h
heo922.h                                     inc=gsp92
heo95.h                                      inc=heo01.h
heo102.h unpack=no                           inc=geo102.h,geo572.h,heo01.h,heo03.h,heo06.h,heo13.h
heo130.h                                     inc=
heo420.h                                     inc=gsp00,geo003.h
heo520.h                                     inc=gsp00

&if $OSSPEC = WIN32					
veo01.c                                      inc=hsp100.h,heo01.h,heo46.h,gip00.h,hsp77.h,SAPDB_Names.h,RTE_IniFileHandling.h,SAPDB_string.h
veo01-l.c                                    inc=hsp100.h,heo01.h,heo46.h,gip00.h,hsp77.h,SAPDB_Names.h,RTE_IniFileHandling.h,SAPDB_string.h -MD
veo01-b.c                                    inc=hsp100.h,heo01.h,heo46.h,gip00.h,hsp77.h,SAPDB_Names.h,RTE_IniFileHandling.h,SAPDB_string.h
&else
veo01.c                                      inc=hsp100.h,gsp09.h,heo01.h,heo46.h,gip00.h,hsp77.h,SAPDB_Names.h,RTE_IniFileHandling.h,RTE_saveUNIXcalls.h,SAPDB_string.h
veo01-l.c                                    inc=hsp100.h,gsp09.h,heo01.h,heo46.h,gip00.h,hsp77.h,SAPDB_Names.h,RTE_IniFileHandling.h,RTE_saveUNIXcalls.h,SAPDB_string.h -MD
veo01-b.c                                    inc=hsp100.h,gsp09.h,heo01.h,heo46.h,gip00.h,hsp77.h,SAPDB_Names.h,RTE_IniFileHandling.h,RTE_saveUNIXcalls.h,SAPDB_string.h
&endif
# replacement for veo01x (PTS 1106862)
veo011.c                                     inc=gsp00_0.h
veo02.c                                      inc=heo02x.h,gip00.h,hsp77.h,SAPDB_string.h
&if $OSSPEC in [ AIX ]
veo02-k.c -DKERNEL_LZU                       inc=heo02x.h,gip00.h,hsp77.h,SAPDB_string.h \
                                              dep=geo60.h,geo007_1.h,RTESys_IO.h,RTESys_CommandPipe.h
&else
veo02-k.c -DKERNEL_LZU                       inc=heo02x.h,gip00.h,hsp77.h,SAPDB_string.h \
                                              dep=geo60.h,geo007_1.h
&endif

# &gar: need dynamic loader lib under linux and sun
&if $OSSPEC in [ LINUX, SOLARIS ] || $MACH == SUN
veo02*.c parentlinkoption=-append=-ldl
&endif

veo03t.c                                     inc=gsp00,gsp01.h,vsp001,vsp0031
veo03t-u.c -DMULTICONNECT                    inc=gsp00,gsp01.h,vsp001,vsp0031
#----------------------------
&if $OS in [ WIN32 ]
#----------------------------
veo07-u.c -DUSER                             inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,SAPDB_sprintf.h
veo07.c   -DUSER                             inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,SAPDB_sprintf.h
veo07-m.c -DUSER -DUSE_MALLOC                inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,SAPDB_sprintf.h
&else
veo07-u.c -DUSER                             inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,hen40.h,SAPDB_sprintf.h
veo07.c   -DUSER                             inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,hen40.h,SAPDB_sprintf.h
veo07-m.c -DUSER -DUSE_MALLOC                inc=heo07.h,geo00_0.h,geo001.h,geo57.h,geo007_1.h,geo007_2.h,SAPDB_string.h,hen40.h,SAPDB_sprintf.h
&endif
# special module using KB interfaces
veo12.c                                      inc=heo12.h
veo12_1.c -DPROTOTYPE                        inc=gsp00,vsp004,geo00_0.h,heo46.h,heo12_1.h,heo102.h
veo130.c  -DPROTOTYPE                        inc=gsp00,vsp004,geo00_0.h,heo46.h,heo130.h,heo102.h
veo16.c                                      inc=hsp100.h,heo16.h,geo00_0.h,heo46.h
veo21.c                                      inc=heo21.h,SAPDB_string.h
veo28.cpp -except                            inc=gsp01.h,heo01.h,geo001.h,geo007_1.h,geo28.h,geo47.h,geo570.h,geo60.h,geo200.h,geo49.h,SAPDB_MemCopyMoveNoCheck.hpp
# C/C++ Wrapper
# main(argc,argv)
veo30.cpp                                    inc=
# main(argc,argv,envp)
veo302.cpp                                   inc=
#
# Pascal/C++ Wrapper
#
# tcc
veo301-ct01.cpp -DPASCAL_MAIN=ct01main       inc=
# pretty
veo301-ct11.cpp -DPASCAL_MAIN=ct11main       inc=
# usecheck
veo301-ct12.cpp -DPASCAL_MAIN=ct12main       inc=
# xref
veo301-ct64.cpp -DPASCAL_MAIN=ct64main       inc=
#
# vttest
veo301-in59.cpp -DPASCAL_MAIN=in59main       inc=
# userx
veo301-in85.cpp -DPASCAL_MAIN=in85main       inc=
# load
veo301-lo01.cpp -DPASCAL_MAIN=lo01main       inc=
# query
veo301-qu01.cpp -DPASCAL_MAIN=qu01main       inc=
# slowci
veo301-ta39.cpp -DPASCAL_MAIN=ta39main       inc=
# diagnose
veo301-xt01.cpp -DPASCAL_MAIN=xt01main       inc=
#
veo31.c                                      inc=gsp00
#
veo40.c                                      inc=geo007_2.h,heo420.h,geo40.h,heo46.h,heo47.h,geo60.h,SAPDB_string.h,\
											     :sap/700/saptype.h,:sap/700/saptypeb.h,:sap/700/saptypec.h,:sap/700/sapuc.h,:sap/700/nixx.h,:sap/700/err.h,:sap/700/dptrace.h,:sap/700/sapuc2.h
#
veo40-l.c -DSTUB_MODULE                      inc=geo007_2.h,heo420.h,geo40.h,heo46.h,heo47.h,geo60.h,RTESec_SAPSSL.h,RTESys_SharedLib.h,SAPDB_string.h,gip00.h,\
                                                 :sap/700/saptype.h,:sap/700/saptypeb.h,:sap/700/saptypec.h,:sap/700/sapuc.h,:sap/700/nixx.h,:sap/700/err.h,:sap/700/dptrace.h,:sap/700/sapuc2.h
#
veo401.c									 inc=geo401.h,SAPDB_string.h,\
                                                 :sap/700/saptype.h,:sap/700/saptypeb.h,:sap/700/saptypec.h,:sap/700/nixx.h
#
veo41.cpp                                    inc=geo200.h,geo007_1.h,geo41.h,gsp00
veo41-w.cpp -ansi -DUNICODE                  inc=geo200.h,geo007_1.h,geo41.h,gsp00
veo42.cpp   -ansi                            inc=geo200.h,geo007_1.h,geo60.h,geo41.h,geo42.h,gsp00,gsp01.h,SAPDB_MemCopyMoveNoCheck.hpp
veo43.cpp                                    inc=geo43.h,heo01.h,SAPDB_MemCopyMoveNoCheck.hpp
veo44.cpp		-ansi                        inc=geo42.h,geo44.h,geo47.h,gsp00
veo45.cpp		-ansi					     inc=geo42.h,geo44.h,geo45.h
veo46.cpp		-ansi					     inc=geo42.h,geo44.h,geo45.h,geo46.h
veo46.c                                      inc=heo46.h,heo47.h,geo00_0.h
veo47.c                                      inc=heo47.h,SAPDB_string.h
veo50.cpp                                    inc=heo50.h
veo51.cpp                                    inc=gsp00_0.h,SAPDB_MemCopyMoveNoCheck.hpp unpack=no
veo52.c   -DKERNEL_LZU                       inc=heo52.h,geo50_0.h,RTESys_MicroTime.h,RTESys_Time.h
veo54.c                                      inc=geo54.h
veo57.cpp -DKERNEL_LZU                       inc=heo57.h,geo00_0.h,geo50_0.h,heo00.h,geo00_1.h
veo58.cpp -DKERNEL_LZU                       inc=heo58.h,geo00_0.h,geo50_0.h,heo46.h,heo52.h,vsp0058,geo007_1.h,RTEConf_ParameterAccessKernelInterface.hpp,SAPDB_string.h
veo59.c   -DKERNEL_LZU                       inc=geo50_0.h
veo60k.c  -DKERNEL_LZU -DNEW_DIAG_FILES      inc=geo00_2.h,geo002.h,vsp0031,geo00_0.h,geo50_0.h,heo46.h,heo60.h,heo60_2.h,hsp77.h,geo007_1.h,geo60.h \
                                              dep=heo52.h,RTESys_Time.h,geo67.h,Msg_OutputKnlDiag.h,Msg_OutputUtilProt.h
veo60k-u.c                                   inc=geo00_2.h,geo002.h,geo007_1.h,vsp0031,geo00_0.h,geo50_0.h,heo46.h,heo60.h,heo60_2.h,hsp77.h,geo60.h
veo61.cpp -DKERNEL_LZU                       inc=geo61.h,heo61.h,geo00_0.h,heo00.h,geo50_0.h
veo66.c   -DKERNEL_LZU                       inc=gsp00,geo00_0.h,heo00.h,geo002.h,geo50_0.h,heo66.h,heo46.h,heo01.h
veo67.cpp -DKERNEL_LZU -DDONT_USE_PRIORITY   inc=gsp00,vsp0031,geo00_0.h,geo00_2.h,geo50_0.h,geo007_1.h,heo52.h,heo56.h,heo67.h,geo67.h,RTE_PendingEventHandler.h,SAPDB_MemCopyMoveNoCheck.hpp
#
# special test compilation defines for veo70.c:
#
# use -DFORCE_ALL_IN_ONE to compile as 'ALL_IN_ONE' (cserv.pcf is ignored...)
# use -DFORCE_ONE_TO_ONE to compile as 'ONE_TO_ONE' (cserv.pcf is ignored...)
#
veo70.c   -DPROTOTYPE -DKERNEL_LZU           inc=geo70k.h,geo00_0.h,geo50_0.h,geo007_1.h,heo11.h,geo001.h,geo57.h,heo69.h,RTEConf_ParameterAccessKernelInterface.h,SAPDB_sprintf.h,SAPDB_string.h
veo77.c                                      inc=heo77.h,geo60.h
veo79.c    -DPROTOTYPE -DKERNEL_LZU          inc=heo79.h,geo002.h,heo00.h,heo11_1.h,geo50_0.h,geo007_1.h,heo46.h,RTEConf_ParameterAccessKernelInterface.h,RTEConf_ParameterAccessSimple.h,SAPDB_string.h
veo80.cpp                                    inc=geo80.h,SAPDB_MemCopyMoveNoCheck.hpp
veo81.cpp                                    inc=geo80.h,geo81.h
heo00x.h                                      inc=heo00.h,geo00_2.h          
veo92-k.c   -DKERNEL_LZU                     inc=geo00_2.h,heo00x.h,geo002.h,geo003.h,heo11.h,heo92.h,heo47.h,LVC_MethodNames.hpp,hsp77.h,RTE_ConsoleStandardOutput.h,RTETask_Task.h,KernelParam_IOManager.hpp
veo92-c.c   -DKERNEL_LZU                     inc=geo00_2.h,heo00x.h,geo002.h,geo003.h,heo11.h,heo92.h,heo47.h,LVC_MethodNames.hpp,hsp77.h,RTE_ConsoleStandardOutput.h,RTETask_Task.h,KernelParam_IOManager.hpp
veo95.cpp                                    inc=
veo98.c                                      inc=
veo100.c                                     inc=SAPDB_string.h
veo110.cpp  -except                          inc=geo60.h,geo110.h,gsp00,geo007_1.h
veo111.cpp  -except                          inc=geo81.h,geo110.h,geo111.h,geo200.h,gsp00,geo007_1.h
veo112c.cpp  -DPROTOTYPE                     inc=gsp00,ggg00,geo60.h,gkb03,heo06.h
veo131.c                                     inc=gsp00,heo12.h,geo13.h,heo13.h,hsp02.h,hsp77.h,hsp78_0.h,hsp83.h,hsp100.h,SAPDB_string.h,heo131x.h
heo131x.h                                    inc=vsp004,SAPDB_string.h
veo131x.cpp                                  inc=heo131x.h,RTEComm_URIBuilder.hpp
veo200.cpp -except                           inc=geo200.h,geo41.h,geo007_1.h,SAPDB_string.h
veo200-k.cpp -except                         inc=geo200.h,geo41.h,geo007_1.h,RTEMem_RteAllocator.hpp,SAPDB_string.h
veo200-w.cpp -except -DUNICODE               inc=geo200.h,geo41.h,geo007_1.h,SAPDB_string.h
veo290.cpp                                   inc=geo80.h,geo29.h,heo07.h
veo520.c                                     inc=heo520.h
veo570.cpp -except                           inc=geo007_1.h,geo570.h,gsp00,geo60.h
veo572.cpp -except                           inc=geo007_1.h,gsp00,geo570.h,geo572.h
veo573.cpp -except                           inc=geo570.h,geo573.h
heo600.h                                     inc=gsp00,gsp03
veo600.c -DKERNEL_LZU -DNEW_DIAG_FILES       inc=geo00_2.h,geo00_0.h,geo50_0.h,heo60_2.h,heo600.h,heo602.h,Msg_OutputKnlDiag.h
heo601.h                                     inc=
veo601.c                                     inc=
heo602.h                                     inc=gsp00,hsp78_0.h
veo602.c                                     inc=heo602.h,hsp77.h,SAPDB_string.h
#
# begin of veo670 the 'stack back trace module....
#
heo670.h                                     inc=geo00.h \
                                             dep=RTESys_MSWindows.h
veo670.c                                     inc=gsp00,hsp77.h,heo670.h,geo60.h,RTESys_Spinlock.h,SAPDB_string.h
veo670-d.c -MD                               inc=gsp00,hsp77.h,heo670.h,geo60.h,RTESys_Spinlock.h,SAPDB_string.h
veo670-k.c -DKERNEL_LZU                      inc=gsp00,hsp77.h,heo670.h,geo60.h,RTESys_Spinlock.h,SAPDB_string.h \
                                             dep=heo01.h,geo50_0.h,geo002.h,heo51.h,RTEDiag_SymbolResolutionLock.h
veo670-s.c -DSERVER_LZU					     inc=gsp00,hsp77.h,heo670.h,geo60.h,RTESys_Spinlock.h,SAPDB_string.h \
                                             dep=heo01.h,geo50_0.h,geo002.h,heo51.h
#
# if this would work libunwind will always be using a package... but unfortunately the compiler
# used with SLES8 is no longer supprted by the newer libunwind packages...
#&if $OSSPEC in [ LINUX ] && $MACH in [ I386, X86_64, IA64 ]
#
# glibc has no unwind information for most system calls on IA32 , making it currently impossible to use...
#&if $OSSPEC in [ LINUX ] && $MACH in [ I386, X86_64, IA64 ] && $__HAS_NPTL == 1
#
# glibc now has a working backtrace() for x86_64 while libunwind sometimes produces a core dump for x86_64 ...
#&if $OSSPEC in [ LINUX ] && $MACH in [ X86_64, IA64 ] && $__HAS_NPTL == 1
#
# currently active: Only LINUX_IA64 with GCC < 4.1 !
&if $OSSPEC in [ LINUX ] && ($__GNUC__ < 4) && $MACH in [ IA64 ] && $__HAS_NPTL == 1
#
# 'Linux native' libunwind packet used
#
veo670*.c -DHAS_EXTERN_LIBUNWIND \
	dep=+:extern/libunwind/build_libunwind.pl \
	parentlinkoption=-append=$WRK/extern/lib/libunwind.a

&if $MACH in [ X86_64 ]
veo670*.c   parentlinkoption=-append=$WRK/extern/lib/libunwind-x86_64.a
&elif $MACH in [ IA64 ]
veo670*.c   parentlinkoption=-append=$WRK/extern/lib/libunwind-ia64.a
&elif $MACH in [ I386 ]
veo670*.c   parentlinkoption=-append=$WRK/extern/lib/libunwind-x86.a
&endif
veo670*.c   parentlinkoption=-append=-lgcc_eh
#
&elif $OSSPEC == LINUX && ($__GNUC__ < 4) && $MACH == IA64
#
# SLES8 for IA64 needs own stripped version of libunwind...
#
# 13.12.2004 jrg
# Now only needed for SLES8 compilation...
#
# 10.5.2004 jrg
# Reenabled! Stripped libunwind-0.96 of all modules that confused libstd++...
#
# 26.2.2004 jrg
# The ia64 stack backtrace uses external header files and an external library found in the tools only
# These are
#   $TOOL/extern/libunwind_0.96/include/libunwind-dynamic.h
#   $TOOL/extern/libunwind_0.96/include/libunwind-common.h
#   $TOOL/extern/libunwind_0.96/include/libunwind.h
#   $TOOL/extern/libunwind_0.96/lib/libunwind-ia64.a
#
# To formulate the external dependency 'virtual files' are needed! These do only exists
# as 'alias' to make vmake happy....
#
v_libunwinddynamic_h				     ascii \
concom=no unpack=no noversion \
<-$TOOL/extern/libunwind_0.96/include/libunwind-dynamic.h ->$WRK/incl/libunwind-dynamic.h
v_libunwindcommon_h				     ascii \
concom=no unpack=no noversion dep=:eo/v_libunwinddynamic_h \
<-$TOOL/extern/libunwind_0.96/include/libunwind-common.h  ->$WRK/incl/libunwind-common.h
v_libunwind_h                         ascii \
concom=no unpack=no noversion dep=:eo/v_libunwindcommon_h  \
<-$TOOL/extern/libunwind_0.96/include/libunwind.h         ->$WRK/incl/libunwind.h
v_libunwindia64.a                     binary \
<-$TOOL/extern/libunwind_0.96/lib/libunwind-ia64.a        ->$INSTLIB/libunwind-ia64.a
veo670*.c dep=+:eo/v_libunwind_h parentlinkoption=-append=$INSTLIB/libunwind-ia64.a dep=:eo/v_libunwindia64.a
&endif
#
# end of veo670 the 'stack back trace module....
#
veo900.c                                     inc=geo900.h
veo900.cpp                                   inc=geo900.hpp
veo906.cpp -except                           inc=gsp01.h,geo007_1.h,geo00_2.h,geo60.h,geo200.h,geo906.h,geo907.h
&if $OS in [ WIN32 ]
veo906.cpp									 inc=+gos00.h,gos906.h	# use -DDUMP_PACKET_DATA to dump DBMSERVER send/receive data
&endif
veo907.cpp -ansi                						 inc=geo42.h,geo44.h,geo45.h,geo907.h,geo570.h
veo950.cpp -D$OS -DDEF_PROGLIST              inc=gsp00,heo46.h,heo95.h,hsp100.h,SAPDB_string.h
&if $OS in [ WIN32 ]
veo950.cpp                                   inc=+gos00.h
&endif
veo951.c  -D$OS                              inc=gsp01.h,geo002.h,heo95.h,heo06.h,RTEConf_ParameterAccessSimple.h
&if $OSSPEC = WIN32
veo952.cpp -D$OS                             inc=geo002.h,heo95.h,heo11.h,gsp09.h,gip00.h,RTE_GetDBRootOfDB.h,SAPDB_Names.h,SAPDB_string.h,heo46.h,vsp003c,RTEConf_ParameterAccess.hpp
&else
veo952.cpp -D$OS                             inc=geo002.h,heo95.h,heo11.h,gsp09.h,gip00.h,RTE_GetDBRootOfDB.h,SAPDB_Names.h,SAPDB_string.h,heo46.h,vsp003c,RTEConf_ParameterAccess.hpp dep=gen41.h,RTE_SdbStarter.h
&endif
veo953.cpp -D$OS                             inc=geo002.h,heo01.h,heo95.h,heo12_1.h,gsp09.h,RTE_GetDBRootOfDB.h,RTEConf_ParameterAccess.hpp
veo954.cpp -D$OS                             inc=geo002.h,heo95.h,RTEConf_ParameterAccessSimple.h,heo46.h,gsp09.h,RTE_GetDBRootOfDB.h,RTEConf_ParameterAccess.hpp
veo964.cpp                                   inc=gsp00,gsp01.h,geo03.h,heo03.h,hsp100.h,RTESec_SAPSSL.h,RTESec_SSL.h,SAPDB_StandardArgumentParser.hpp,RTESys_MicroTime.h
veo991.c  -DPROTOTYPE                        inc=heo11.h,heo01.h
veo999test.cpp -except                       inc=geo200.h,geo57.h
veo999fsize.c                                inc=heo06.h,gsp05.h
veo999sysrc.c                                inc=heo46.h
veo999tsig.c                                 inc=

