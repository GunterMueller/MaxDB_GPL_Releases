/* @lastChanged: "2000-12-13"

  * @filename:   vgg06.cpp
  * @purpose:    "GG_cpp_auxiliary_functions"
  * @release:    7.2.0.0
  * @see:        ""
  *
  * @Copyright (c) 1999-2005 SAP AG"


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


*/

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */

#include <string.h>
#include "ggg92.h"
#include "hgg06.h"
#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp"
#include "Logging/Log_Types.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp"
#include "ServerTasks/SrvTasks_JobTypes.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "hgg17.h"
#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#include "hta01.h"	
#endif
// #include "KernelCommon/Kernel_VTrace.hpp"

/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */

/*----------------------------------------------------------------------------*/

externPascal void gg06BuildRedoSession (
    tgg91_SessionNo       VAR_VALUE_REF  Session,
    tgg91_TransNo         VAR_VALUE_REF  transno)
{
    Session.becomes(transno.gg90GetInt4());
}

/*----------------------------------------------------------------------------*/

externPascal void gg06BuildTaskSession (
    tgg91_SessionNo       VAR_VALUE_REF  Session)
{
#if defined(DBMCLI) || defined(SLOWCI)
    Session.gg90SetNil();
#else
    Session.gg91BuildTaskSession (RTETask_ITask::Self()->Type());
#endif
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06BuildRef (/*[OUT]*/ tgg91_PageRef &PageRef,
                           /*[IN] */ tsp00_Int4     Pno,
                           /*[IN] */ tsp00_Int4     Pos)
{
    PageRef.gg91BuildRef (Pno, Pos);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06Cint6Decr (/*[IN OUT]*/ tgg90_Cint6 &Cint6)
{
    --Cint6;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06Cint6Incr (/*[IN OUT]*/ tgg90_Cint6 &Cint6)
{
    ++Cint6;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06Cint6ToLine (/*[IN]    */ tgg90_Cint6 &Cint6,
                              /*[IN OUT]*/ int         &LineLen,
                              /*[IN OUT]*/ tsp00_Line  &Line)
{
    Cint6.gg90AppendToLine (LineLen, Line);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06FileVersIncr (/*[IN OUT]*/ tgg91_FileVersion &FileVers)
{
    ++FileVers;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06FileVersToLine (/*[IN]    */ tgg91_FileVersion &FileVers,
                                 /*[IN OUT]*/ int               &LineLen,
                                 /*[IN OUT]*/ tsp00_Line        &Line)
{
    FileVers.gg91AppendToLine (LineLen, Line);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06GetObjHeaderParam (/*[IN] */ void               *ObjHeaderBuf,
                                    /*[OUT]*/ tgg00_ObjTransInfo &ObjTransInfo,
                                    /*[OUT]*/ tgg00_ObjPagePos   &NextFreeObj,
                                    /*[OUT]*/ tgg00_ObjState     &ObjState,
                                    /*[OUT]*/ tgg00_ObjFrameVers &ObjFrameVers)
{
    tgg92_ObjHeader* pObjHeader = (tgg92_ObjHeader*) ObjHeaderBuf;

    ObjTransInfo = pObjHeader->ohdTransInfo_gg92;
    NextFreeObj  = pObjHeader->ohdNextFreeObj_gg92;
    ObjState     = pObjHeader->ohdState_gg92;
    ObjFrameVers = pObjHeader->ohdFrameVersion_gg92;
}

/*----------------------------------------------------------------------------*/

externC tsp00_Int4 gg06GetOidFrameVers (/*[IN]*/ tgg92_KernelOid &oid)
{
    return ( oid.gg92GetFrameVers() );
}

/*----------------------------------------------------------------------------*/

externC tsp00_Int4 gg06GetOidPos (/*[IN]*/ tgg92_KernelOid &oid)
{
    return ( oid.gg92GetPos() );
}

/*----------------------------------------------------------------------------*/

externC void gg06InitObjTransInfo (/*[OUT]*/ tgg00_ObjTransInfo &ObjTransInfo)
{
    ObjTransInfo.otrBeforeRef_gg00.gg91SetNilRef();
    ObjTransInfo.otrLockTransIndex_gg00 = cgg_nil_transindex;
    ObjTransInfo.otrLockTransId_gg00.gg90SetNil();
    ObjTransInfo.otrUpdTransId_gg00.gg90SetNil();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06Int4Get (const tsp00_BytePtr Buffer,
                                tsp00_Int4          Pos)
{
    return ( REINTERPRET_CAST(tgg90_Cint4*,(Buffer+Pos-1))->gg90GetInt4() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface to handle Uint2
externC tsp00_Int4 gg06Int4FromUint2 (/*[IN]*/ tsp00_Uint2 Uint2)
{ 
    return Uint2; 
}

/*----------------------------------------------------------------------------*/

// PASCAL interface to handle Uint2
externC void
gg06Int4ToUint2 (/*[IN] */ tsp00_Int4   Int4,
                 /*[OUT]*/ tsp00_Uint2 &Uint2)
{
    Uint2 = Int4; 
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsDummyFileVers (/*[IN]*/ tgg91_FileVersion &FileVers)
{
    return ( FileVers.gg91IsDummyVers() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsDummyRef (/*[IN]*/ tgg91_PageRef &PageRef)
{
    return ( PageRef.gg91IsDummyRef() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsDummyTrans (/*[IN]*/ tgg91_TransNo &TransNo)
{
    return ( TransNo.gg90IsDummy() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsEqOidPnoPos (/*[IN]*/ tgg92_KernelOid &oid1,
                                /*[IN]*/ tgg92_KernelOid &oid2)
{
    return ( oid1.gg92IsEqPnoPos (oid2) );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsGreaterOrEqualUint4 (/* [IN] */ tsp00_Uint4 Left,
                                        /* [IN] */ tsp00_Uint4 Right)
{
    return (Left >= Right);
};

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsMaxSession (/*[IN]*/ tgg91_SessionNo &SessionNo)
{
    return ( SessionNo.gg91IsMaxSession() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsMaxTrans (/*[IN]*/ tgg91_TransNo &TransNo)
{
    return ( TransNo.gg91IsMaxTrans() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsNilObjFileNo (/*[IN]*/ tgg00_ObjFileNo ObjFileNo)
{
    return (NIL_OBJ_FILE_NO_GG92 == ObjFileNo);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface PTS 1105492 JA 2000-03-15
externC tsp00_Bool gg06IsNilOid (/*[IN]*/ tgg92_KernelOid &oid)
{
    return ( oid.gg92IsNil() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsNilRef (/*[IN]*/ tgg91_PageRef &PageRef)
{
    return ( PageRef.gg91IsNilRef() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsNilSession (/*[IN]*/ tgg91_SessionNo &SessionNo)
{
    return ( SessionNo.gg90IsNil() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsNilTrans (/*[IN]*/ tgg91_TransNo &TransNo)
{
    return ( TransNo.gg90IsNil() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsSpecialFileVers (/*[IN]*/ tgg91_FileVersion &FileVers)
{
    return ( FileVers.gg91IsSpecialVers() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Bool gg06IsZeroTrans (/*[IN]*/ tgg91_TransNo &TransNo)
{
    return ( TransNo.gg90IsZero() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06OidToLine (/*[IN]    */ tgg92_KernelOid &oid,
                            /*[IN OUT]*/ int             &LineLen,
                            /*[IN OUT]*/ tsp00_Line      &Line)
{
    oid.gg92AppendToLine (LineLen, Line);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06PnoGet (/*[IN]*/ tgg90_Cint4 &Cint4)
{
    return ( Cint4.gg90GetInt4() );
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06PnoPut (/*[OUT]*/ tgg90_Cint4 &Cint4,
                         /*[IN] */ tsp00_Int4   int4)
{
    Cint4.becomes (int4);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06RefPno (/*[IN]*/ tgg91_PageRef &PageRef)
{
    return (PageRef.gg91RefPno() );
};

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC	tsp00_Int4 gg06RefPos (/*[IN]*/ tgg91_PageRef &PageRef)
{
    return (PageRef.gg91RefPos() );
};

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SessionBecomes (/*[OUT]*/ tgg91_SessionNo &SessionNo,
                                 /*[IN] */ tsp00_Int4       IntValue)
{
    SessionNo.becomes(IntValue);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SessionDecr (/*[IN OUT]*/ tgg91_SessionNo &SessionNo)
{
    --SessionNo;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06SessionGetInt4 (/*[IN]*/ tgg91_SessionNo &SessionNo)
{ 
    return SessionNo.gg90GetInt4(); 
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SessionIncr (/*[IN OUT]*/ tgg91_SessionNo &SessionNo)
{
    ++SessionNo;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SessionToLine (/*[IN]    */ tgg91_SessionNo &SessionNo,
                                /*[IN OUT]*/ int             &LineLen,
                                /*[IN OUT]*/ tsp00_Line      &Line)
{
    SessionNo.gg91AppendToLine (LineLen, Line);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetDummyFileVers (/*[OUT]*/ tgg91_FileVersion &FileVers)
{
    FileVers.gg91SetDummyVers();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetDummyRef (/*[OUT]*/ tgg91_PageRef &PageRef)
{ 
    PageRef.gg91SetDummyRef ();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetDummyTrans (/*[OUT]*/ tgg91_TransNo &TransNo)
{
    TransNo.gg90SetDummy();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetNilOid (/*[OUT]*/ tgg92_KernelOid &oid)
{
    oid.gg92SetNil ();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetNilRef (/*[OUT]*/ tgg91_PageRef &PageRef)
{ 
    PageRef.gg91SetNilRef ();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetNilSession (/*[OUT]*/ tgg91_SessionNo &SessionNo)
{
    SessionNo.gg90SetNil();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetNilTrans (/*[OUT]*/ tgg91_TransNo &TransNo)
{
    TransNo.gg90SetNil();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface

externC void gg06SetOid (/*[OUT]*/ tgg92_KernelOid &oid,
                         /*[IN] */ tsp00_Int4       pno,
                         /*[IN] */ tsp00_Int4       pos,
                         /*[IN] */ tsp00_Int4       FrameVers)
{
    oid.gg92Set (pno, pos, FrameVers);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetOidFrameVers (/*[OUT]*/ tgg92_KernelOid &oid,
                                  /*[IN] */ tsp00_Int4       FrameVers)
{
    oid.gg92SetFrameVers (FrameVers);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetOidPos (/*[OUT]*/ tgg92_KernelOid &oid,
                            /*[IN] */ tsp00_Int4       pos)
{
    oid.gg92SetPos (pos);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetSpecialFileVers (/*[OUT]*/ tgg91_FileVersion &FileVers)
{
    FileVers.gg91SetSpecialVers();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetZeroCint6 (/*[OUT]*/ tgg90_Cint6 &Cint6)
{
    Cint6.gg90SetZero();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetZeroSession (/*[OUT]*/ tgg91_SessionNo &SessionNo)
{
    SessionNo.gg90SetZero();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SetZeroTrans (/*[OUT]*/ tgg91_TransNo &TransNo)
{
    TransNo.gg90SetZero();
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06SizeofObjHeader ()
{ 
    return ( sizeof (tgg92_ObjHeader) ); 
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06SubtransDecr (/*[IN OUT]*/ tgg00_SubtransNo &SubtransNo)
{
    --SubtransNo;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06TransBecomes (/*[OUT]*/ tgg91_TransNo &TransNo,
                               /*[IN] */ tsp00_Int4     IntValue)
{
    TransNo.becomes(IntValue);
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06TransDecr (/*[IN OUT]*/ tgg91_TransNo &TransNo)
{
    --TransNo;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06TransGetInt4 (/*[IN]*/ tgg91_TransNo &TransNo)
{ 
    return TransNo.gg90GetInt4(); 
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06TransIncr (/*[IN OUT]*/ tgg91_TransNo &TransNo)
{
    ++TransNo;
}

/*----------------------------------------------------------------------------*/

// PASCAL interface
externC void gg06TransToLine (/*[IN]    */ tgg91_TransNo &TransNo,
                              /*[IN OUT]*/ int           &LineLen,
                              /*[IN OUT]*/ tsp00_Line    &Line)
{
    TransNo.gg90AppendToLine (LineLen, Line);
}
/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06Min (
                       /*[IN]*/ tsp00_Int4 value1,
                       /*[IN]*/ tsp00_Int4 value2 )
{
    return((( value1 > value2 ) ? value2 : value1 ));
}
/*----------------------------------------------------------------------------*/

// PASCAL interface
externC tsp00_Int4 gg06Max (
                       /*[IN]*/ tsp00_Int4 value1,
                       /*[IN]*/ tsp00_Int4 value2 )
{
    return((( value1 > value2 ) ? value1 : value2 ));
}
/*----------------------------------------------------------------------------*/
static inline tsp00_Uint4 gg06_NextPrimNumber (tsp00_Uint4 number)
{
    if ( number <= 2 ) return 2;

    tsp00_Uint4 nextNumber = number;
    bool        isPrim     = false;
    tsp00_Uint4 i;

    while ( ! isPrim )
    {
        nextNumber++;
        i = 3;
        isPrim = (nextNumber % 2) == 1;
        while (i*i <= nextNumber && isPrim)
        {
            isPrim = (nextNumber % i) != 0;
            i += 2;
        }
    }
    return nextNumber;
}

/*----------------------------------------------------------------------------*/

// PTS 1120350 UH 2003-01-31 new
#define MAX_ROWID_HASH_PRIM_NUMBERS 1024
#define START_PRIM_NUMBER           255
#define PRIMNUMBER_DISTANCE         3

// PTS 1120350 UH 2003-01-31 new
static tsp00_Uint4 gg06RowIdHashPrimNumbers[MAX_ROWID_HASH_PRIM_NUMBERS];

/*----------------------------------------------------------------------------*/
inline SAPDB_UInt4 SimpleHash ( const SAPDB_Byte  *s,
                                const SAPDB_UInt4  l )
{
    SAPDB_UInt4 i,h;
    for ( i=0,h = 0; i<l; i++ )
        h = 131*h + *(s+i);
    return h;
/*
int hashfunction(s)
    char *s;
    
    { int i;
      for( i=0; *s; s++ ) i = 131*i + *s;
      return( i % m );
    }
*/
}

/*----------------------------------------------------------------------------*/
inline SAPDB_UInt4 OldCalculateHash(const SAPDB_Byte*  key,
                                    const SAPDB_UInt4  keylength )
{
    SAPDB_UInt4 hashvalue = 0;
        
    const SAPDB_UInt4 hashKeyLength = keylength < MAX_ROWID_HASH_PRIM_NUMBERS ? keylength : MAX_ROWID_HASH_PRIM_NUMBERS;
          SAPDB_UInt4 curroffset;
          SAPDB_UInt4 newSummmand;
          SAPDB_Byte  byte;
          // SAPDB_UInt4 curroffsetaux;  // variant3
        
    for ( curroffset = 0; curroffset < hashKeyLength; curroffset++ )
    {
        /* variant1 (produce often 8byte hash
        newSummmand = ( (*(key+curroffset) + curroffset)
                        *
                        gg06RowIdHashPrimNumbers[curroffset] );
        */
        byte          = *(key+curroffset);
        /* variant2 (modulo) */
        newSummmand   = byte * gg06RowIdHashPrimNumbers[(curroffset+byte)%MAX_ROWID_HASH_PRIM_NUMBERS];
        /* variant3 (avoid modulo)
        curroffsetaux = curroffset+byte;
        if ( curroffsetaux > MAX_ROWID_HASH_PRIM_NUMBERS )
            curroffsetaux = MAX_ROWID_HASH_PRIM_NUMBERS;
        newSummmand   = byte * gg06RowIdHashPrimNumbers[curroffsetaux];
        */
        if ( hashvalue < SAPDB_MAX_UINT4 - newSummmand )
            hashvalue += newSummmand;
        else
            break;
    }
    return hashvalue;
}

/*----------------------------------------------------------------------------*/
inline void CalculateHash(const SAPDB_Byte*  key,
                          const SAPDB_UInt4  keylength,
                          const tsp00_Uint4  hashtype,
                                SAPDB_UInt4 &hashvalue )
{
    switch ( hashtype )
    {
        case 0:
            hashvalue = OldCalculateHash(key,keylength);
            break;
        case 1:
            FNV32Hash(key, keylength, hashvalue);
            break;
        case 2:
            hashvalue = SimpleHash (key,keylength);
            break;
        default:
            hashvalue = 0;
    }
}

/*----------------------------------------------------------------------------*/
externPascal void gg06CalculateHash (
    const tsp00_Byte*                    key,
    tsp00_Uint4                          keysize,
    tsp00_Uint4                          hashtype,
    tsp00_Uint4           VAR_VALUE_REF  hashvalue)
{
	CalculateHash(key,keysize,hashtype,hashvalue);
}

/*----------------------------------------------------------------------------*/
externPascal pasbool gg06buildCompressedKey (
    const tsp00_Byte*                    key,
    tsp00_Int4                           keySize,
    tsp00_Int4                           keyStartPosition,
    tsp00_Int4                           keyLength,
    tsp00_Int4                           compressedKeySize,
    tsp00_Byte*                          compressedKey)
{
    // PTS 1121179 UH 2003-04-07 new

    ROUTINE_DBG_MEO00 ("gg06buildCompressedKey");

  	tgg00_BasisError moveError = e_ok;

	if ( keyLength <= compressedKeySize )
	{
        // nothing to do key is small enough to fit into compressed key
		SAPDB_RangeMove ( __FILE__, 1,
				 keySize,       compressedKeySize,
				 key,           keyStartPosition,
				 compressedKey, 1,
				 keyLength,     moveError);

		if ( moveError != e_ok )
			return false;
        return true;
	}

    // really compress the key
    
    struct
    {
        SAPDB_UInt4 value1;
        SAPDB_UInt4 value2;
    } hashvalues;

	if ( keyStartPosition > keySize )
		return false;

    // FNV32Hash(key+keyStartPosition-1, keyLength, hashvalues.value1);
	hashvalues.value1 = OldCalculateHash(key+keyStartPosition-1,keyLength);
    hashvalues.value2 = SimpleHash (key+keyStartPosition-1,keyLength);

    // move hashvalues to the end of the compressed key

    tsp00_Int4 moveKeyLength = compressedKeySize - sizeof(hashvalues);

    SAPDB_RangeMove ( __FILE__,  2,
             sizeof(hashvalues), compressedKeySize,
             &hashvalues,        1,
             compressedKey,      1 + moveKeyLength,
             sizeof(hashvalues), moveError);
    if ( moveError != e_ok )
        return false;
        
    // move parts of original key to compressed key

    const tsp00_Int4 moveKeyLengthFirstPart  = moveKeyLength/2;
    const tsp00_Int4 moveKeyLengthSecondPart = moveKeyLength-moveKeyLengthFirstPart; // PTS 1129353 UH 2004-04-27

    SAPDB_RangeMove ( __FILE__, 3,
             keySize,       compressedKeySize,
             key,           keyStartPosition,
             compressedKey, 1,
             moveKeyLengthFirstPart, moveError);

    if ( moveError != e_ok )
        return false;

    SAPDB_RangeMove ( __FILE__, 4,
             keySize,       compressedKeySize,
             key,           keyLength - moveKeyLengthSecondPart + 1, // PTS 1129353 UH 2004-04-27 was not set to end
             compressedKey, 1 + moveKeyLengthFirstPart,
             moveKeyLengthSecondPart, moveError); // PTS 1129353 UH 2004-04-27

    if ( moveError != e_ok )
        return false;

    return true;
}

/*----------------------------------------------------------------------------*/
externPascal void gg06CalculateRowIdHashPrimNumbers ()
{
    // PTS 1120350 UH 2003-01-31 new
    // start: 255, distance: 3, count: 1024, number: 28697, sum: 3582848940
    ROUTINE_DBG_MEO00 ("gg06CalculateRowIdHashPrimNumbers");
    
    tsp00_Uint4 primnumber = START_PRIM_NUMBER;
    
    for ( tsp00_Uint4 primnumberCount = 0;
          primnumberCount < MAX_ROWID_HASH_PRIM_NUMBERS;
          ++primnumberCount )
    {
        primnumber = gg06_NextPrimNumber(primnumber);
        if ( primnumberCount % 2 == 0 )
            gg06RowIdHashPrimNumbers[primnumberCount] = primnumber;
        else
            gg06RowIdHashPrimNumbers[MAX_ROWID_HASH_PRIM_NUMBERS-primnumberCount] = primnumber;
        
        for (tsp00_Uint4 skip = 0; skip<PRIMNUMBER_DISTANCE; skip++)
            primnumber = gg06_NextPrimNumber(primnumber);
    }
}

/*----------------------------------------------------------------------------*/

externPascal pasbool gg06IsSurrogateGreater(
    const tgg00_Surrogate VAR_ARRAY_REF surrogateA,
    const tgg00_Surrogate VAR_ARRAY_REF surrogateB)
{
    return memcmp (&surrogateA, &surrogateB, sizeof (tgg00_Surrogate)) > 0;
}

/*-----------------------------------------------------------------------------------*/
externPascal void gg06DeviceStateToLine (
    integer                              devicestate,
    integer               VAR_VALUE_REF  length,
    tsp00_Line            VAR_ARRAY_REF  line)
{
    const SAPDB_Char *str;
    
    if ( devicestate >= Log_DeviceState_DO_NOT_USE || devicestate < 0 )
        str = "*** invalid flushmode ***";
    else
        str = Log_DeviceStateStrings[devicestate];

    tgg00_BasisError moveError = e_ok;
    SAPDB_RangeMove (__FILE__, 5, strlen(str), sizeof(line),
                     str,  1, line, length, strlen(str), moveError);
    length += (integer)strlen(str);
}

/*-----------------------------------------------------------------------------------*/
externPascal void gg06FlushModeToLine (
    integer                              flushmode,
    integer               VAR_VALUE_REF  length,
    tsp00_Line            VAR_ARRAY_REF  line)
{
    const char *str;

    if ( flushmode >= Log_FlushMode_DO_NOT_USE || flushmode < 0 )
        str = "*** invalid flushmode ***";
    else
        str = Log_FlushModeStrings[flushmode];

    tgg00_BasisError moveError = e_ok;
    SAPDB_RangeMove (__FILE__, 6, strlen(str), sizeof(line),
                     str,  1, line, length, strlen(str), moveError);
    length += (integer)strlen(str);
}

/*-----------------------------------------------------------------------------------*/
externPascal void gg06LogActionToLine (
    integer                              ActionType,
    integer               VAR_VALUE_REF  LineLen,
    tsp00_Line            VAR_ARRAY_REF  Line)
{
          SAPDB_Byte     *pDest   = reinterpret_cast<SAPDB_Byte*>(&Line);
    const Log_ActionType  auxType = Log_ActionType(ActionType);
    const char           *str     = Log_GetActionTypeString(auxType);
    tgg00_BasisError moveError = e_ok;
    SAPDB_RangeMove (__FILE__, 7, strlen(str), sizeof(Line),
                     str,  1, Line, LineLen, strlen(str), moveError);
    LineLen += (integer)strlen(str);
}

/*-----------------------------------------------------------------------------------*/
externPascal void gg06LogSavepointReasonToLine (
    integer                              Reason,
    integer               VAR_VALUE_REF  LineLen,
    tsp00_Line            VAR_ARRAY_REF  Line)
{
    const char *str;

    if ( Reason >= Log_SavepointReason_DO_NOT_USE || Reason < 0 )
        str = "*** invalid reason ***";
    else
        str = Log_SavepointReasonStrings[Reason];

    tgg00_BasisError moveError = e_ok;
    SAPDB_RangeMove (__FILE__, 7, strlen(str), sizeof(Line),
                     str,  1, Line, LineLen, strlen(str), moveError);
    LineLen += (integer)strlen(str);
}
/*-----------------------------------------------------------------------------------*/
externPascal void gg06UInt4toLine (              // new for PTS 1124727 mb 2003-11-46
    SAPDB_UInt4                          number,
    integer                              maxNumWidth,
    tsp00_Line            VAR_ARRAY_REF  outArray)
{
    SAPDB_ToStringClass  convertedNumber (number, maxNumWidth);
    tgg00_BasisError moveError = e_ok;
    SAPDB_RangeMove (__FILE__, 8, maxNumWidth, sizeof(outArray),
                     (SAPDB_Char*)convertedNumber,  1, outArray, 1,
                     convertedNumber.Length(), moveError);
}

/*-----------------------------------------------------------------------------------*/
externPascal SAPDB_Int4 gg06Align (
   const SAPDB_Int4 length,
   const SAPDB_Int4 alignment )
{
    if( length )
        return ((length-1) & ~(alignment-1)) + alignment;
    else
        return 0;
}

/* --------------------------------------------------------------------------*/

externPascal void g06TaskTypeToLine (
    char                                 TaskType,
    integer               VAR_VALUE_REF  LineLen,
    tsp00_Line            VAR_ARRAY_REF  Line)
{ 
    tgg00_BasisError            moveError = e_ok;
    RTETask_ITask::TaskType     type = static_cast<RTETask_ITask::TaskType>(TaskType);
    const SAPDB_ToStringClass   taskTypeName = SAPDB_ToString (type);
    

    if (0 == strcmp ((SAPDB_Char const *)taskTypeName, RTE_TSK_TYPE_NAME_UNKNOWN))
    {
        SAPDB_Char                  unknownType[]="UnknownTaskType=";
        SAPDB_UInt4                 unknownTypeLength = strlen (unknownType);

        SAPDB_RangeMove (__FILE__, 9, unknownTypeLength, sizeof(Line),
            unknownType,  1, Line, LineLen,
            unknownTypeLength, moveError);
        if ( moveError == e_ok )
        {
            LineLen += unknownTypeLength;
            SAPDB_ToStringClass  convertedNumber (static_cast<SAPDB_Int1>(TaskType),
                                                  SAPDB_ToStringClass::hex,
                                                  SAPDB_ToStringClass::uppercase,
                                                  SAPDB_ToStringClass::showbase);
            SAPDB_RangeMove (__FILE__, 10, convertedNumber.Length(), sizeof(Line),
                static_cast<SAPDB_Char*>(convertedNumber),  1,
                Line, LineLen,
                convertedNumber.Length(), moveError);
            if ( moveError == e_ok )
                LineLen += convertedNumber.Length();
        }
    }
    else
    {
        const int nameLen = strlen ((SAPDB_Char const *)taskTypeName);
         
        SAPDB_RangeMove (__FILE__, 11, nameLen, sizeof(Line),
                         (SAPDB_Char const *)taskTypeName,  1, Line, LineLen,
                         nameLen, moveError);
        if (moveError == e_ok)
        {
            LineLen += nameLen;
        }
    }
}
