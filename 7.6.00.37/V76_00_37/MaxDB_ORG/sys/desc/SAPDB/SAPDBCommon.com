# ------------------------------------------------------------------------------
#
# SAPDB Common descriptions
#
# ------------------------------------------------------------------------------
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

regex(.*\.cpp$)    -except


# -------------------------------------------------------------------------------------
# Errors, exceptions and messages:
# -------------------------------------------------------------------------------------
#
SAPDB_MessageFile.cpp               inc=SAPDB_Types.h,\
                                        heo07.h,\
                                        heo15.h,\
                                        geo007_2.h,\
                                        heo07.h,\
										SAPDB_string.h
SAPDB_MessageFile.hpp               inc=gsp00,\
                                        geo00_0.h,\
                                        heo07.h

SAPDB_Messages.genmsg               ascii

SAPDB_String_Messages.hpp           inc=SAPDBErr_MessageList.hpp

SAPDB_UTF8String_Messages.hpp       inc=SAPDBErr_MessageList.hpp

SAPDB_MemCopyMove_Messages.hpp      inc=SAPDBErr_MessageList.hpp

SAPDB_Exception.hpp                 inc=SAPDBErr_MessageList.hpp

# -------------------------------------------------------------------------------------
# Global configuration:
# -------------------------------------------------------------------------------------
#
SAPDB_Config.hpp
# -------------------------------------------------------------------------------------
# Basis Types:
# -------------------------------------------------------------------------------------
#
SAPDB_Types.h                       inc=
SAPDB_Types.hpp                     inc=SAPDB_Types.h,SAPDB_Config.hpp
SAPDB_EnhancedTypes.hpp             inc=SAPDB_Types.hpp
SAPDB_RangeIterator.hpp             inc=SAPDB_Types.hpp,\
                                        Kernel_VTrace.hpp,\
                                        SAPDBErr_Assertions.hpp

# -------------------------------------------------------------------------------------
# Elementary operations:
# -------------------------------------------------------------------------------------
#
SAPDB_BitOperations.hpp             inc=SAPDB_Types.hpp
SAPDB_IntegerArithmetic.hpp         inc=SAPDBErr_Assertions.hpp
SAPDB_PersistentTypes.hpp           inc=SAPDB_Types.hpp,\
                                        SAPDBErr_Assertions.hpp

# -------------------------------------------------------------------------------------
# ToString classes:
# -------------------------------------------------------------------------------------
#
SAPDB_ToString.hpp                  inc=SAPDB_Types.hpp
SAPDB_ToString.cpp                  inc=SAPDB_ToString.hpp,RTE_IInterface.hpp,hsp77.h,SAPDB_string.h,SAPDBMem_IRawAllocator.hpp
SAPDB_ToStringTest.cpp              inc=SAPDB_ToString.hpp

# -------------------------------------------------------------------------------------
# String elements and string basis class:
# -------------------------------------------------------------------------------------
#
SAPDB_UTF8StringElement.hpp         inc=SAPDB_UTF8Basis.hpp,\
                                        SAPDBErr_Assertions.hpp
                                        
SAPDB_StrUtils.hpp                  inc=SAPDB_Types.hpp                                        

# -------------------------------------------------------------------------------------
# UTF-8 string classes:
# -------------------------------------------------------------------------------------
#
SAPDB_UTF8Basis.hpp                 inc=SAPDB_Types.hpp,\
                                        hsp81.h

SAPDB_UTF8Basis.cpp                 inc=SAPDB_UTF8Basis.hpp

# -------------------------------------------------------------------------------------
# Streams and stream test:
# -------------------------------------------------------------------------------------
#
SAPDB_OStream.hpp                   inc=SAPDB_Types.hpp,\
                                        SAPDB_ToString.hpp
SAPDB_OStream.cpp                   inc=SAPDB_OStream.hpp,\
                                        RTE_CompilerFeatures.h

SAPDB_StreamTest.cpp                inc=SAPDB_Types.hpp,\
                                        SAPDB_ToString.hpp,\
                                        SAPDB_OStream.hpp

# -------------------------------------------------------------------------------------
# Include file useful for C/C++ header files, that means header files that are
# used in both universes...
# -------------------------------------------------------------------------------------
#
SAPDB_C_Cplusplus.h                 inc=

# -------------------------------------------------------------------------------------
# AutoPtr classes:
# -------------------------------------------------------------------------------------
#
SAPDB_AutoPtr.hpp                   inc=SAPDBErr_Assertions.hpp,\
                                        SAPDBMem_IRawAllocator.hpp

SAPDB_AutoPtrTest.cpp               inc=SAPDB_AutoPtr.hpp,\
                                        SAPDBMem_NewDestroy.hpp,\
                                        SAPDBMem_DefaultRawAllocator.hpp,\
                                        Test_Program.hpp

# -------------------------------------------------------------------------------------
# reference countin smart pointer classes:
# -------------------------------------------------------------------------------------
#
SAPDB_RefCountingSmartPtr.hpp       inc=SAPDBErr_Assertions.hpp,\
                                        SAPDBMem_IRawAllocator.hpp

SAPDB_RefCountingSmartPtrTst.cpp    inc=SAPDB_RefCountingSmartPtr.hpp,\
                                        SAPDBMem_DefaultRawAllocator.hpp

# -------------------------------------------------------------------------------------
# Single base class:
# -------------------------------------------------------------------------------------
#
SAPDB_Singleton.hpp                 inc=RTESync_Spinlock.hpp,SAPDBMem_NewDestroy.hpp

# -------------------------------------------------------------------------------------
# SAPDB Reuselibrary:
# -------------------------------------------------------------------------------------
#
SAPDB_ReuseLib.h inc=heo01.h,heo02.h,heo03.h,heo06.h,heo07.h,heo41.h,\
        :SAPDB/PacketInterface/PIn_FileTraceStream.h,\
        :SAPDB/PacketInterface/PIn_Packet.h,\
        :SAPDB/PacketInterface/PIn_ReplyParser.h,\
        :SAPDB/PacketInterface/PIn_RequestWriter.h,\
        :SAPDB/PacketInterface/PIn_XUserConnect.h,\
        hsp77.h,hsp78_0.h,geo900.h

#
# As long as vsp77.c exists, this headerfile allows only to use new names, later it should be headerfile for the implementation...
#
SAPDB_sprintf.h inc=hsp77.h

#
# Replacement for <string.h> to prevent (SAPDB_Int4) cast before strlen and conditional code for strcasecmp()
#
SAPDB_string.h

#
# Common SAPDB name definitions
#
SAPDB_Names.h inc=
SAPDB_MemCopyMoveNoCheck.hpp inc=SAPDB_Types.hpp,SAPDBErr_Assertions.hpp
SAPDB_MemCopyMove.hpp inc=SAPDB_Types.hpp,SAPDB_MemCopyMoveNoCheck.hpp,SAPDBErr_Assertions.hpp
SAPDB_MemCopyMove.cpp inc=SAPDB_MemCopyMove.hpp,SAPDB_MemCopyMove_Messages.hpp

#
# SAPDB_DIRECT_KNLDIAG_OUTPUT   Force output into knldiag in case of move error...
#
SAPDB_PascalMoveClient.c inc=SAPDB_Types.h,geo007_1.h,geo60.h
SAPDB_PascalMoveKernel.cpp -DKERNEL_LZU -DSAPDB_DIRECT_KNLDIAG_OUTPUT \
			  inc=SAPDB_MemCopyMove.hpp,SAPDB_MemCopyMove_Messages.hpp,RTE_Message.hpp,RTE_Crash.hpp,gsp00,ggg00,SQLMan_Context.hpp
SAPDB_RangeCode.hpp inc=SAPDB_MemCopyMove.hpp,SAPDB_MemCopyMove_Messages.hpp,gsp00,ggg00
SAPDB_RangeCodeClient.cpp inc=SAPDB_RangeCode.hpp,RTE_Crash.hpp

SAPDB_ArgumentParser.hpp                inc=RTE_Types.h
SAPDB_ArgumentParser.cpp                inc=SAPDB_ArgumentParser.hpp

SAPDB_StandardArgumentParser.hpp        inc=SAPDB_ArgumentParser.hpp
SAPDB_StandardArgumentParser.cpp        inc=SAPDB_StandardArgumentParser.hpp

SAPDB_FastStrlen.hpp inc=SAPDB_Types.hpp dep=RTE_Configure.pl
SAPDB_C_strlen.cpp inc=SAPDB_FastStrlen.hpp
