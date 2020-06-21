/*!***************************************************************************

  module      : vgg942.cpp
  responsible : UweH
  special area: ComponentTests
  created     : 2000-11-24
  last changed: 2000-11-30  9:36
  Copyright (c) 2000-2005 SAP AG
  description : component tests
                DIAGNOSE CHECK INSTANCE <number>

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "Logging/Log_Types.hpp" // needed for vgg999 PTS 1124727 mb 2003-10-30

#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h" /* nocheck */
#endif

#include "hgg942.h" // check of PASCAL header vgg942
#include "hgg999.h" // KernelAdministration_PascalInterface
#include "ggg92.h"  // in dependence of hgg06
#include "hgg06.h"  // gg06buildCompressedKey,gg06CalculateHash

#include "DataAccess/Data_Test.hpp"
#include "KernelCommon/Kernel_Test.hpp"
#include "Logging/Log_Test.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Queue.hpp"
#include "Logging/Log_AfterImageSpace.hpp"
#include "RunTime/RTE_Test.hpp"
#include "Table/Table_Test.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_FieldList.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_Test.hpp"
#include "SQLManager/SQLMan_Test.hpp"
#include "SQLManager/Catalog/Catalog_Test.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Test.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/RTE_MySQLProxy.hpp"
#include "ConsistentRead/CnsRead_Test.hpp"
#include "Transaction/Trans_Iterator.hpp"
#include "Transaction/Trans_Context.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Restart/Rst_RedoFile.hpp"
#include "ServerTasks/SrvTasks_Test.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DIAGNOSE.h"
/*----------------------------------------------------------------------------*/
bool ReserveRedoFile (Rst_RedoFile &file, SAPDB_UInt size, SAPDB_UInt minsize, SAPDB_UInt seq, SAPDB_UInt ioseq, Msg_List &errlist)
{
    Data_SplitSpaceWriter writer = file.ReserveSpace (size, minsize, seq, ioseq, errlist);
    return errlist.IsEmpty();
}

#define UNDO_CHANGES true

/*----------------------------------------------------------------------------*/
bool testRedoFile(Trans_Context &trans)
{
    Msg_List     &errlist = trans.ErrorList();

    if ( trans.OldTrans().trWriteTransId_gg00.gg90IsNil() )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Must run within transaction."));
        return false;
    }

    Rst_RedoFile  file (trans.OldTrans(), Data_PageNo(), Data_PageNo());

    if ( ! file.Create(errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: file.Create()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        return false;
    }

    // reserve successfull in one page 

    if ( ! ReserveRedoFile (file, 8000, 10, 1, 10, errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull in one page; Reserve()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }
        
    if ( ! file.ReleaseSpace( ! UNDO_CHANGES, errlist ) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull in one page; Release()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }

    if ( ! file.Truncate (0, errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull in one page; Truncate()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }

    // reserve UNsuccessfull in one page 

    if ( ! ReserveRedoFile (file, 8000, 10, 1, 10, errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve UNsuccessfull in one page; Reserve()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }
        
    if ( ! file.ReleaseSpace( UNDO_CHANGES, errlist ) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve UNsuccessfull in one page; Release()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }

    // reserve successfull in one page and then reserve as much that it needs to write into a new page

    if ( ! ReserveRedoFile (file, 8000, 10, 2, 20, errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull in one page + one new page; Reserve1()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }
        
    if ( ! file.ReleaseSpace( ! UNDO_CHANGES, errlist ) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull in one page + one new page; Release1()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }

    if ( ! ReserveRedoFile (file, 8000, 2000, 3, 30, errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull in one page + one new page; Reserve2()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }
        
    if ( ! file.ReleaseSpace( UNDO_CHANGES, errlist ) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull in one page + one new page; Release2()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }

    // reserve successfull more then one page

    if ( ! ReserveRedoFile (file, 8000*4, 40, 4, 40, errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull more then one page; Reserve()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }
        
    if ( ! file.ReleaseSpace( ! UNDO_CHANGES, errlist ) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve successfull more then one page; Release()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }

    // reserve UNsuccessfull more then one page

    if ( ! ReserveRedoFile (file, 8000*4, 40, 5, 50, errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve UNsuccessfull more then one page; Reserve()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }
        
    if ( ! file.ReleaseSpace( UNDO_CHANGES, errlist ) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Test: reserve UNsuccessfull more then one page; Release()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        Msg_List auxerrlist;
        if ( ! file.Drop(auxerrlist) )
        {
            auxerrlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
            errlist.AppendNewMessage(auxerrlist);
            return false;
        }
        return false;
    }

    // drop the complete file

    if ( ! file.Drop(errlist) )
    {
        errlist.AppendNewMessage(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"file.Drop()"));
        trans.OldTrans().trWriteTransId_gg00.gg90SetNil();
        return false;
    }
    return true;
}

/*----------------------------------------------------------------------------*/
void randomKeyAssign ( SAPDB_Byte                     *s,
                       SAPDB_UInt                      l,
                       RTE_MySQLPseudoRandomGenerator &rand,
                       bool                            onlyAscii )
{
    const SAPDB_UInt variableLength = l/10;
    const SAPDB_UInt firstOffset    = l/2 - variableLength/2;
    const SAPDB_UInt lastOffset     = firstOffset+variableLength;
    SAPDB_MemFillNoCheck (s,0x20,l-1);
    for ( SAPDB_UInt i = firstOffset; i < lastOffset; i++ )
    {
        if ( onlyAscii )
        {
            SAPDB_Byte b = static_cast<SAPDB_Byte>(rand.RandomNumber() * 127);
            *(s+i) = b < 32 ? 32 : b;
        }
        else
            *(s+i) = static_cast<SAPDB_Byte>(rand.RandomNumber() * SAPDB_MAX_UINT1);
    }
    // Kernel_TraceBuffer (s,l,"randomKey", 80);
}

#include "FileDirectory/FileDir_Test.hpp"

/*----------------------------------------------------------------------------*/
void testCompression(tgg00_TransContext& trans)
{
    const SAPDB_Byte key1[] = { 0xFF,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
                                0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
                                0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xFF,0xDD,0xDD,
                                0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,
                                0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,
                                0xDD,0xDD,0xFF,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,
                                0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,
                                0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xFF
                              };
    const SAPDB_UInt   key1length = 80;
    const SAPDB_Byte key2[] = { 0xFF,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
                                0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
                                0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xFF,0xFF,0xBB,
                                0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,
                                0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,
                                0xBB,0xBB,0xBB,0xBB,0xBB,0xFF
                              };
    const SAPDB_UInt   key2length = 56;

	const tsp00_Uint4   COMPRESSEDKEYSIZE = 64;
          tsp00_Byte    compressedkey [COMPRESSEDKEYSIZE];
    
    (void)gg06buildCompressedKey (key1,key1length,1,key1length,COMPRESSEDKEYSIZE,compressedkey);

    if ( ! memcmp (key2,compressedkey,key2length) )
        Kernel_VTrace() << "Keycompression passed.";
	else
        Kernel_TraceBuffer (compressedkey,COMPRESSEDKEYSIZE,"BAD compressedkey", COMPRESSEDKEYSIZE);
}

/*----------------------------------------------------------------------------*/
void testKnownKeys(tgg00_TransContext& trans)
{
    const SAPDB_Byte key1[] = { 0x20,0x30,0x30,0x30,0x20,0x46,0x4f,0x52,0x4d,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x48,0x52,0x5f,0x49,0x44,0x5f,0x54,0x41,0x58,0x31,0x37,0x32,0x31,0x41,0x42,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x54,0x58,0x54,0x20,0x20,0x45 };
    const SAPDB_UInt   key1length = 93;
    const SAPDB_Byte key2[] = { 0x20,0x30,0x30,0x30,0x20,0x46,0x4f,0x52,0x4d,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x4f,0x44,0x5f,0x56,0x42,0x4c,0x5f,0x41,0x4e,0x4d,0x45,0x4c,0x20,0x20,0x20,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                                0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x54,0x58,0x54,0x20,0x20,0x45 };
    const SAPDB_UInt   key2length = 93;

    const tsp00_Uint4   MAXHASHTYPE       = 3;
	const tsp00_Uint4   COMPRESSEDKEYSIZE = 64;
          tsp00_Byte    compressedkey1 [COMPRESSEDKEYSIZE];
          tsp00_Byte    compressedkey2 [COMPRESSEDKEYSIZE];
          tsp00_Uint4   key1hashvalue;
          tsp00_Uint4   key2hashvalue;
    
    for ( tsp00_Byte hashtype = 0; hashtype < MAXHASHTYPE; hashtype++ )
    {
        (void)gg06buildCompressedKey (key1,key1length,1,key1length,COMPRESSEDKEYSIZE,compressedkey1);
        (void)gg06buildCompressedKey (key2,key2length,1,key2length,COMPRESSEDKEYSIZE,compressedkey2);

        Kernel_TraceBuffer (compressedkey1,COMPRESSEDKEYSIZE,"compressedkey1", COMPRESSEDKEYSIZE);
        Kernel_TraceBuffer (compressedkey2,COMPRESSEDKEYSIZE,"compressedkey2", COMPRESSEDKEYSIZE);

        if ( memcmp (compressedkey1,compressedkey2,COMPRESSEDKEYSIZE) )
            Kernel_VTrace() << "Hash Type: " << hashtype << " (0=old,1=fnv,2=simple) PASSED.";
        else
        {
            Kernel_VTrace() << "LockKey collision for type (0=old,1=fnv,2=simple): " << hashtype;
            gg06CalculateHash(key1,key1length,hashtype,key1hashvalue);
            gg06CalculateHash(key2,key2length,hashtype,key2hashvalue);
            if ( key1hashvalue == key2hashvalue )
                Kernel_VTrace() << "Hash collision for type (0=old,1=fnv,2=simple): " << hashtype;
            else
                Kernel_VTrace() << "NO Hash collision ?";
        }
    }
}

/*----------------------------------------------------------------------------*/
void testHashSpreading(tgg00_TransContext& trans,
                       bool                onlyAscii)
{
    const tsp00_Uint4   MAXLOOPS = 100000;
    const tsp00_Uint4   MAXHASHTYPE = 3;

    typedef tsp00_Uint2 CollisionList [MAXLOOPS];

    CollisionList       spreading     [MAXHASHTYPE];
    tsp00_Uint4         collisions    [MAXHASHTYPE];
    
    const tsp00_Uint4   KEYLENGTH         = 1024;
    tsp00_Byte          key           [KEYLENGTH];

    tsp00_Uint4         hashvalue;
    tsp00_Uint4         loop;
    tsp00_Byte          hashtype;

    Kernel_VTrace() << "TEST SPREADING: loops=" << MAXLOOPS << " keylength=" << KEYLENGTH
                    << (onlyAscii?" only ascii keys":" keys 0-255");

    for ( hashtype = 0; hashtype < MAXHASHTYPE; hashtype++ )
    {
        collisions[hashtype] = 0;
        for ( loop = 0; loop < MAXLOOPS; loop++ )
            spreading [hashtype][loop] = 0;
    }
    
    RTE_MySQLPseudoRandomGenerator rand(11);

    for ( loop = 0; loop < MAXLOOPS; loop++ )
    {
        randomKeyAssign (key, KEYLENGTH, rand, onlyAscii);
        for ( hashtype = 0; hashtype < MAXHASHTYPE; hashtype++ )
        {
            gg06CalculateHash(key,KEYLENGTH,hashtype,hashvalue);
            if ( spreading [hashtype][hashvalue%MAXLOOPS] > 0 )
                collisions[hashtype]++;
            (spreading[hashtype][hashvalue%MAXLOOPS])++;
        }
    }
    for ( hashtype = 0; hashtype < MAXHASHTYPE; hashtype++ )
        Kernel_VTrace() << "HASHTYPE (0=old,1=fnv,2=simple) " << hashtype
                        << ": collisions=" << collisions[hashtype];
}
/*----------------------------------------------------------------------------*/
void testHashSpeed (tgg00_TransContext& trans)
{
    tgg00_BasisError   &berr              = trans.trError_gg00;
    const tsp00_Uint4   KEYLENGTH         = 1024;
	const tsp00_Uint4   COMPRESSEDKEYSIZE = 64;
    const tsp00_Uint4   MAXLOOPS          = 100000;
    tsp00_Byte          key           [KEYLENGTH];
    tsp00_Byte          compressedkey [COMPRESSEDKEYSIZE];
    tsp00_Uint4         loop;
    SAPDB_UInt8         timeBegin;
    SAPDB_UInt8         timeEnd;

    SAPDB_MemFillNoCheck (key, 0xFF, KEYLENGTH);
    SAPDB_MemFillNoCheck (compressedkey, 0x00, COMPRESSEDKEYSIZE);

    Kernel_VTrace() << "TEST HASHES:";
    Kernel_VTrace() << "KEYLENGTH = " << KEYLENGTH << ", COMPRESSEDKEYSIZE = " << COMPRESSEDKEYSIZE;
    Kernel_VTrace() << "MAXLOOPS  = " << MAXLOOPS;

    timeBegin = RTESys_MicroSecTimer();
    for ( loop = 0; loop < MAXLOOPS; loop++ )
    {
        (void)gg06buildCompressedKey (key,KEYLENGTH,1,KEYLENGTH,COMPRESSEDKEYSIZE,compressedkey);
    }
    timeEnd = RTESys_MicroSecTimer();
    Kernel_VTrace() << "time = " << timeEnd-timeBegin;
}

/*----------------------------------------------------------------------------*/
inline bool checkQueue (Trans_Context &trans,
                        SAPDB_UInt     maxLoops,
                        SAPDB_UInt     minReserveSize,
                        SAPDB_UInt     reserveSize,
                        Msg_List      &errlist)
{
    // PTS 1134810 UH 2005-04-12 new test
    Kernel_TaskWaitList::WaitContext waitcontext;
    
    Log_Queue              *queue     = NULL;
    Log_AfterImageSpace     space (trans.Allocator());
    Log_Page::OffsetType    entrypos;
    Log_IOSequenceNo        wioseqno;
    Log_RawDeviceOffset     offset;
    
    SAPDB_UInt8             timeBegin;
    SAPDB_UInt8             timeEnd;

	Msg_List infoOutput;
            
    Log_Volume::Instance().GetQueueForWriting(trans.Task().ID(), queue); // PTS 1135304 mb 2005-05-24

    infoOutput.AppendNewMessage(
        Msg_List(Msg_List::Info, SDBMSG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE,
                 Msg_Arg(SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE__MAX_LOOPS,SAPDB_ToString(maxLoops)),
                 Msg_Arg(SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE__MIN_RESERVE_SIZE,SAPDB_ToString(minReserveSize))));

    timeBegin = RTESys_MicroSecTimer();
    for ( SAPDB_UInt i = 0; i < maxLoops; i++ )
    {
        if ( ! queue->Reserve ( trans.Task().ID(), waitcontext, reserveSize, minReserveSize, false, space, errlist) )
        {
            errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                                   "DIAGNOSE CHECK INSTANCE 5000 (LOG_QUEUE)" );
            return false;
        }
        queue->UserTaskReady (trans.Task().ID(), space);
    }
    timeEnd = RTESys_MicroSecTimer();
    infoOutput.AddDetail(
        SDBMSG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT,
        Msg_Arg(SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT__RESERVE_SIZE,SAPDB_ToString(reserveSize)),
        Msg_Arg(SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT__TIME_VALUE,SAPDB_ToString(timeEnd-timeBegin)));
    if ( ! queue->ReserveForEOT(trans.Task().ID(), waitcontext, 10, space, entrypos, errlist) )
    {
        errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                               "DIAGNOSE CHECK INSTANCE 5000 (LOG_QUEUE)" );
        return false;
    }
    queue->UserTaskEOTReady(trans.Task().ID(), waitcontext, space, wioseqno, offset);
    Log_Volume::Instance().ReleaseQueue(trans.Task().ID(), queue); // PTS 1135304 mb 2005-05-24
    errlist.AppendNewMessage(infoOutput);
    return true;
}

/*----------------------------------------------------------------------------*/

externPascal void gg942ComponentTestsWithStringarg ( tgg00_TransContext& trans,
                                                     tsp00_Int4          TestNo,
                                                     tsp00_MoveObjPtr    StringPtr,
                                                     tsp00_Bool          isUnicode,
                                                     tsp00_Int4          StringLen )
{
    Trans_Context          &context            = Trans_Context::GetContext(trans);
    RTETask_ITask          &task               = context.Task();
    Msg_List               &errlist            = context.ErrorList();
    SAPDBMem_IRawAllocator &allocator          = context.Allocator();
    
    Kernel_Version::ComponentType    componentType;
    Kernel_Version::BuildKind        buildInfo;
    Kernel_Version::DevelopmentPhase developPhase;
    Kernel_Version::InstanceType     builtForInstance;
    
    Kernel_Version::GetExtraInfoFromSoftware (componentType, buildInfo, developPhase, builtForInstance);
    if ( developPhase != Kernel_Version::DevelopPhase ) 
    {
        trans.trError_gg00 = e_invalid_command;
        return;
    }
    else
    {
        SAPDB_UTF8             *utf8Argument       = 0;
        SAPDB_Int4              utf8ArgumentLength = 0;
        
        if ( StringLen > 0 )
        {
            if ( isUnicode )
            {
                utf8Argument = (SAPDB_UTF8 *)allocator.Allocate(3*StringLen + 1);
                if ( utf8Argument )
                {
                    tsp81_UCS2Char const * srcAt;
                    SAPDB_UTF8 * destAt;
                    if ( SAPDB_UTF8Basis::Success 
                        != SAPDB_UTF8Basis::KernelConvertFromUTF16(
                        (tsp81_UCS2Char *)StringPtr,
                        (tsp81_UCS2Char *)(((SAPDB_Byte *)StringPtr)+StringLen),
                        srcAt,
                        0, // command part always High byte / Low byte -> not swapped
                        utf8Argument,
                        utf8Argument+3*StringLen,
                        destAt ) )
                    {
                        allocator.Deallocate(utf8Argument);
                        errlist =  SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                            "DIAGNOSE CHECK INSTANCE string conversion" );
                        trans.trError_gg00 = e_internal_error;
                        return;
                    }
                    utf8ArgumentLength = destAt - utf8Argument;
                    *destAt = 0;
                }
            }
            else
            {
                utf8Argument = (SAPDB_UTF8 *)allocator.Allocate(StringLen + 1);
                if ( utf8Argument )
                {
                    SAPDB_MemCopyNoCheck(utf8Argument, StringPtr, StringLen); // source not zero terminated
                    utf8Argument[StringLen] = 0;
                    utf8ArgumentLength = StringLen;
                }
            }
            
            if ( !utf8Argument )
            {
                errlist =  SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                    "DIAGNOSE CHECK INSTANCE string allocation" );
                trans.trError_gg00 = e_internal_error;
                return;
            }
        }
        
        switch (TestNo)
        {
        case 0:
            // list all known tests ++++ to do
            break;
        case 1:
            if ( ! Data_Test (Trans_Context::GetContext(trans), utf8Argument, utf8ArgumentLength) )
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "DIAGNOSE CHECK INSTANCE 1 (Data)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 2:
            if ( ! Kernel_Test (task, errlist) )
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "DIAGNOSE CHECK INSTANCE 2 (Kernel: Version + Migration)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 3:
            {
                Log_Test testRunner;
                testRunner.PerformTest(trans, Log_Test::Log_ResetStatistics);
                testRunner.PerformTest(trans, Log_Test::Log_TestIOSequences);
                testRunner.PerformTest(trans, Log_Test::Log_TestActions);       
                break;
            }
        case 4:
            RTE_Test (trans, utf8Argument, utf8ArgumentLength);
            break;
        case 5:
            if ( ! Table_Test (context) )
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "DIAGNOSE CHECK INSTANCE 5 (Table_Test)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 6:
            if ( ! SQLMan_Test(context) )
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "DIAGNOSE CHECK INSTANCE 6 (SQLMan_Test)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 7:
            if ( ! SAPDBAlgo_Test(context) )
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "DIAGNOSE CHECK INSTANCE 7 (SAPDBAlgo_Test)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 8:
            testHashSpeed(trans);
            // testHashSpreading(trans,false);
            // testHashSpreading(trans,true);
            testKnownKeys(trans);
            testCompression(trans);
            break;
        case 9:
            if ( ! SAPDBFields_FieldList::Test(errlist) )
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "DIAGNOSE CHECK INSTANCE 9 (FieldList)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 10:
            if (!testRedoFile(Trans_Context::GetContext(trans)))
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                                       "DIAGNOSE CHECK INSTANCE 10 (RedoFile)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 11:
            if (!Trans_Iterator::Test(Trans_Context::GetContext(trans)))
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                                       "DIAGNOSE CHECK INSTANCE 11 (Trans_Iterator)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;
        case 12:
            if (!SrvTasks_Test(Trans_Context::GetContext(trans)))
            {
                errlist = errlist + SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                                       "DIAGNOSE CHECK INSTANCE 12 (ServerTasks)" );
                trans.trError_gg00 = e_internal_error;
            }
            break;

        // The following tests are not for SINGLE USER TEST: CHECK_INSTANCE*
        
        case 1000: case 1001: case 1002: case 1003: case 1004: case 1005: case 1006: case 1007: case 1008: case 1009:
            FileDir_Test::Test ( trans, TestNo - 999 );
            break;
            
        case 2000: case 2001: case 2002: case 2003: case 2004:
            Catalog_Test::Execute(TestNo - 1999);
            break;

        case 3000: case 3001: case 3002: case 3003: case 3004:
            CnsRead_Test::Execute(TestNo - 2999);
            break;
        case 4000:
            SAPDBMem_Test::Execute(TestNo);
            break;
        case 5000: case 5001: case 5002:
        {
            switch (TestNo-5000) 
            {
                case 0:
                    if ( ! checkQueue (Trans_Context::GetContext(trans), 100, 100, 200, errlist) )
                        trans.trError_gg00 = e_internal_error;
                    break;
                case 1:
                    if ( ! checkQueue (Trans_Context::GetContext(trans), 100, 100, 8192, errlist) )
                        trans.trError_gg00 = e_internal_error;
                    break;
                case 2:
                    if ( ! checkQueue (Trans_Context::GetContext(trans), 100, 100, 8192*4, errlist) )
                        trans.trError_gg00 = e_internal_error;
                    break;
                default:
                    break;
            }
        }
        default:
            break;
        }
        
        allocator.Deallocate(utf8Argument);
    }
}

externPascal void gg942ComponentTests (tgg00_TransContext& trans,
                                       tsp00_Int4          TestNo)
{
    ROUTINE_DBG_MSP00 ("gg942ComponentTests");
    gg942ComponentTestsWithStringarg( trans, TestNo, (tsp00_MoveObjPtr)0, 0, 0);
}
