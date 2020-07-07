
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

autoptr.h               inc=
Options.h               inc=ZString.h,XArray.h
Options.cpp             inc=Options.h,RTE_IniFileHandling.h
XArray.h                inc=
ZString.h               inc=geo00.h
ZString.cpp             inc=ZString.h
TRACE.h                 inc=
Storage.h               inc=
CSMutex.h               inc=heo07.h
ContentStorageDefines.h inc=
ContentStorage.h        inc=Storage.h,ZString.h,Options.h,\
                        ContentStorageDefines.h,SAPCSConHdl.h,TRACE.h
ContentStorage.cpp      inc=ZString.h,Options.h,ContentStorage.h,autoptr.h
SAPCSConHdl.h           inc=ContentStorageDefines.h,hpa111ODCompr2.h,CSMutex.h
SAPCSConHdl.cpp         inc=ZString.h,Options.h,TRACE.h,SAPCSConHdl.h

comp.h                  inc=SAPStoreTest.h
comp.cpp                inc=comp.h
SAPStoreTest.h          inc=ZString.h,ContentStorage.h
SAPStoreTest.cpp        inc=SAPStoreTest.h
test1.cpp               inc=SAPStoreTest.h,comp.h
test2.cpp               inc=SAPStoreTest.h
test3.cpp               inc=SAPStoreTest.h
test4.cpp               inc=SAPStoreTest.h,comp.h
test5.cpp               inc=SAPStoreTest.h,comp.h
test6.cpp               inc=SAPStoreTest.h,comp.h
test7.cpp               inc=SAPStoreTest.h,comp.h
test8.cpp               inc=SAPStoreTest.h,comp.h
test9.cpp               inc=SAPStoreTest.h,comp.h
test10.cpp              inc=SAPStoreTest.h
test11.cpp              inc=hpa101saptype.h,hpa104CsObject.h,\
                            SAPStoreTest.h,comp.h
test12.cpp              inc=hpa101saptype.h,hpa104CsObject.h,\
                            SAPStoreTest.h,comp.h
test13.cpp              inc=SAPStoreTest.h,comp.h
test14.cpp              inc=SAPStoreTest.h,comp.h
test15.cpp              inc=hpa101saptype.h,hpa104CsObject.h,\
                            SAPStoreTest.h,comp.h
test16.cpp              inc=SAPStoreTest.h,comp.h
test17.cpp              inc=SAPStoreTest.h
