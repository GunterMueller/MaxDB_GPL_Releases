/*!
  @file           Log_RecordColumnMap.cpp
  @author         JuergenA
  @author         UweH
  @ingroup        Logging
  @brief          defines a class containing the index descriptions

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

*/
#include "Logging/Log_RecordColumnMap.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Exceptions.hpp"

#include "hgg04.h"
#include "gsp00.h"
#include "ggg00.h"
#include "hsp30.h"

/// standard value for filling columns if nothing else is given
#define LOG_UNDEF_FILL_BYTE 0

/*---------------------------------------------------------------------------*/
/// Ascii description of Log_ColumnType for readable debugging output
const char * Log_ColumnTypeStrings [] =
{
    "FixKeyColumn",
    "VarKeyColumn",
	"FixColumn",
    "VarColumn",
    "VarLongColumn",
    "ColumnLastEnum",
    "NilColumn"
};

extern const char* Log_ColumnTypeStrings[Log_NilColumn+1];

/// returns offset of the first column
inline static SAPDB_Int4 FirstColOffset ()
{
    return (RSN_RECHEAD_MXGG00);
}
/// returns offset of the first fixed column
inline static SAPDB_Int4 FirstFixColOffset (const tgg00_Rec *pRec)
{
    return (sizeof (tgg00_HeaderRec) + pRec->recKeyLen_gg00());
}
/// returns offset of the first key column
inline static SAPDB_Int4 FirstKeyColOffset ()
{
    return (sizeof (tgg00_HeaderRec));
}
/// returns offset of the first variable column
inline static SAPDB_Int4 FirstVarColOffset (const tgg00_Rec *pRec)
{
    return (FirstFixColOffset(pRec) + pRec->recVarcolOffset_gg00() );
}
/// returns the type of the column specified by a stack entry
inline static Log_ColumnType GetColumnType (const tgg00_StackEntry &StackEntry)
{
    switch ( StackEntry.etype() )
    {
    case st_fixkey:
        return (Log_FixKeyColumn);
    case st_varkey:
        return (Log_VarKeyColumn);
    case st_fixcol:
        return (Log_FixColumn);
    case st_varcol:
        return (Log_VarColumn);  
    case st_varlongchar:
        return (Log_VarLongColumn);
        /* +++ UH commented because of rollback error
    case st_column:
        return (Log_Column);
        */
    default:
        return (Log_NilColumn);
    }
}
/// returns moves an int2 length from the source specified by its pointer
inline static SAPDB_Int4 GetInt2Length (const void *pSource)
{
    SAPDB_Int2 Length;
    
    SAPDB_MemCopyNoCheck (&Length, pSource, sizeof(SAPDB_Int2));
    
    return Length;
}
/// moves an int2 length to the destination specified by its pointer
static void PutInt2Length (void *pDestin, SAPDB_Int2 Length)
{
    SAPDB_MemCopyNoCheck (pDestin, &Length, sizeof(Length));
}
/*!
   @brief          evaluates the truncated length of the value
   @param          pValue [in] the first byte of the value has to be the defined byte
   @param          ValueLength [in out] 
   @param          CreateUndefValue [out] indicates that an undef value has to be created
 */
static void TruncateValue (const SAPDB_Byte *pValue,
                           SAPDB_Int4       &ValueLength,
                           bool             &CreateUndefValue)
{    
    const SAPDB_Int4 SizeofDefinedByte = 1;

    CreateUndefValue = false;
    
    if (ValueLength < SizeofDefinedByte)
    {
        CreateUndefValue = true;
        return;
    }
    
    /*  if this part is on, ' ' will result in NULL
        this is not what we expect
    ValueLength = SizeofDefinedByte +
        s30lnr_defbyte (pValue, pValue[0], SizeofDefinedByte + 1, ValueLength - SizeofDefinedByte);
    
    if ( (SizeofDefinedByte == ValueLength) && 
         (csp_undef_byte   != pValue[0])      &&
         (csp_defined_byte != pValue[0]))   //boolean-value FALSE !!
    {
        // create a value containing an undef byte instead of any defined byte
        CreateUndefValue = true;
    }
    */
}

/// increment the given Log_ColumnType. The different parts of a record are written in a defined
/// order. Use this operator to cycle through them.
void operator ++ (Log_ColumnType &ColumnType)
{
    ColumnType = Log_ColumnType (ColumnType + 1);
}

/*!
  @class          Log_ColEntryIter
  @brief          Defines an iterator on a Log_ColumnList 
 */
class Log_ColEntryIter
{

private:
    Log_ColumnType   m_ColumnType;          ///< type of this group of entries
    Log_ColumnEntry *m_pNext;               ///< pointer to the next entry

protected:

    Log_ColumnEntry *m_pCurr;               ///< pointer to the current entry
    Log_ColumnEntry *m_pPrev;               ///< pointer to the next entry
    bool             m_CurrIsRemoved;       ///< true, if current has been removed and is therefore not usable

public:
    /// CTOR
    Log_ColEntryIter (const Log_ColumnList &ColList):
    m_ColumnType    (ColList.GetColumnType()),
    m_pNext         (NULL),
    m_pCurr         (NULL),
    m_pPrev         (NULL),
    m_CurrIsRemoved (false)
    {
        m_pNext = ColList.GetFirst();
    }
	/// returns the length of the column
    SAPDB_Int4 GetLength () const
    { 
        if (NULL == m_pCurr || m_CurrIsRemoved) return (0);
        return (m_pCurr->GetLength());
    }
	/// returns the number of the variable column
    SAPDB_Int4 GetNumber () const
    { 
        if (NULL == m_pCurr || m_CurrIsRemoved) return (0);
        return (m_pCurr->GetNumber());
    } 
	/// returns the offset of the fixed column
    SAPDB_Int4 GetOffset () const
    { 
        if (NULL == m_pCurr || m_CurrIsRemoved) return (0);
        return (m_pCurr->GetOffset());
    }
	/// returns pointer to the value of the column
    const SAPDB_Byte *GetValue () const
    { 
        if (NULL == m_pCurr || m_CurrIsRemoved) return (NULL);
        return (m_pCurr->GetValue());
    }
    /// returns true, if there are more columns available through this iterator
    bool HasMore ()
    {
        return (NULL != m_pNext);
    }
    /// true, if the the value of the dereferenced iterator is equal to the given column
    bool IsEqualValue(const SAPDB_Byte *pOtherValue,
                      SAPDB_Int4        OtherValueLength) const
    {  
        if (GetLength() != OtherValueLength) return (false);
        if (0 == OtherValueLength) return (true); 
        return ( 0 == memcmp (GetValue(), pOtherValue, OtherValueLength) );       
    }
    /// copies the column this iterator points to into a record
    void MoveColToRec (tgg00_Rec        *pRec,
                       SAPDB_Int4        RecSize,
                       tgg00_BasisError &Error) const
    {
        SAPDB_Int2 Length = GetLength();
        SAPDB_Int4 Offset;
        
        switch (m_ColumnType)
        {
        case Log_FixColumn:
            Offset = FirstFixColOffset (pRec) + GetOffset();
            break;
        case Log_FixKeyColumn:
        case Log_VarKeyColumn: 
            Offset = FirstKeyColOffset() + GetOffset();
            break;
        case Log_VarColumn:
            pRec->recBuf_gg00() [pRec->recLen_gg00()] = abs(Length);
            Offset = pRec->recLen_gg00() + 1;
            pRec->recLen_gg00() = Offset + Length;
            if (MAX_RECLEN_GG00 < pRec->recLen_gg00())
            {
                Error = e_too_long_record;
                return;
            }
            break;
        case Log_VarLongColumn:
            PutInt2Length (pRec->recBuf_gg00() + pRec->recLen_gg00(), Length);
            Offset = pRec->recLen_gg00() + 2;
            pRec->recLen_gg00() = Offset + Length;
            if (MAX_RECLEN_GG00 < pRec->recLen_gg00())
            {
                Error = e_too_long_record;
                return;
            }
            break;
        /* ++++ UH commented
        case Log_Column: 
            break;
            */
        }
            
        if (Length > 0)
            SAPDB_RangeMove (__FILE__, 1, Length, RecSize,
                GetValue(), 1, pRec, Offset + 1, Length, Error);
    }
    /// set the iterator to the next column
    void Next ()
    {
        m_pPrev         = m_pCurr;
        m_pCurr         = m_pNext;
        m_CurrIsRemoved = false;
        if (NULL != m_pNext) m_pNext = m_pNext->GetNext();
    }
};
/*!
   @class          Log_EntryIterWithRemove
   @brief          iterator to access a column entry
 */
class Log_EntryIterWithRemove: public Log_ColEntryIter
{

private:

    Log_ColumnList &m_ColList;            /// reference to the list of columns on which this iterator iterates

public:
    /// CTOR
    Log_EntryIterWithRemove (Log_ColumnList &ColList)
    : Log_ColEntryIter (ColList),
      m_ColList        (ColList)
    {}
    /// remove a column from the columnlist the iterator operates on
    void Remove ()
    {
        if (NULL == m_pCurr || m_CurrIsRemoved) return;
        
        m_ColList.Remove (m_pCurr, m_pPrev);
        m_pCurr         = m_pPrev;
        m_CurrIsRemoved = true;
    }
};
/*!
   @class          Log_VarPartHandler
   @brief          iterator to access a column entry
 */
class Log_VarPartHandler
{
private:
    
    const tgg00_Rec *m_pOldRec;
    tgg00_Rec       *m_pNewRec;
    SAPDB_Int4       m_NewRecSize;
    SAPDB_Int4       m_SizeofLength;
    SAPDB_Int4       m_CurrSizeofLength;
    SAPDB_Int4       m_CurrOldOffset;
    SAPDB_Int4       m_CurrOldColLen;
    SAPDB_Int4       m_CurrOldColNumber;
    Log_ColumnType   m_ColumnType;
    void ReadCurrOldColLen ();
public:
    /// CTOR
    inline Log_VarPartHandler (
        Log_ColumnType   ColumnType,
        SAPDB_Int4       OldPartOffset,
        const tgg00_Rec *pOldRec,
        tgg00_Rec       *pNewRec,
        SAPDB_Int4       NewRecSize);
	/// copies columns from the old record into the new record until a column is updated
    void CopyUnchangedColumns (
        SAPDB_Int4        NumberNextChangedCol,
        tgg00_BasisError &Error,
        bool              CopyRest = false);
	/// creates undef columns if there are not any in the old record.
    void CreateMissingUndefColumns (
        SAPDB_Int4        NumberNextChangedCol,
        tgg00_BasisError &Error);
	/// copies outstanding columns from the old record into the record.
    void FinishVarPart (
        SAPDB_Int4       &OffsetFollowingPart,
        tgg00_BasisError &Error);
	/// returns pointer to the value of current column contained in the old record
    inline const SAPDB_Byte *GetOldColumn() const;
	/// returns length of the current column contained in the old record
    inline SAPDB_Int4 GetOldColumnLength() const;
	/// return result  length of length-info of the var (long) column
    inline SAPDB_Int4 GetSizeLength() const;
	/// current column becomes the following column.
    void NextOldColumn();
    /// writes significant values of this instance to the kerneltrace
    void WriteToTrace(const char * title = 0) const
    {
        Kernel_VTrace trace;
        if ( title != NULL )
            trace << title << FlushLine;
        trace << "Columntype: " << Log_ColumnTypeStrings[m_ColumnType];
        Kernel_TraceBuffer ( m_pOldRec, m_pOldRec->recLen_gg00(), "Old record" );
        trace << "New record: m_NewRecSize: " << m_NewRecSize << FlushLine;
        Kernel_TraceBuffer ( m_pNewRec, m_pNewRec->recLen_gg00(), "New record" );
        trace << "SizeofLength: " << m_SizeofLength << ", CurrSizeofLength: " << m_CurrSizeofLength;
        trace << "CurrOldColNumber: " << m_CurrOldColNumber
              << ", CurrOldOffset: " << m_CurrOldOffset
              << ", CurrOldColLen: " << m_CurrOldColLen << FlushLine;
    }
};

inline Log_VarPartHandler::Log_VarPartHandler (Log_ColumnType   ColumnType,
                                               SAPDB_Int4       OldPartOffset,
                                               const tgg00_Rec *pOldRec,
                                               tgg00_Rec       *pNewRec,
                                               SAPDB_Int4       NewRecSize):
m_pOldRec          (pOldRec),
m_pNewRec          (pNewRec),
m_NewRecSize       (NewRecSize),
m_SizeofLength     (1),
m_CurrSizeofLength (0),
m_CurrOldOffset    (OldPartOffset),
m_CurrOldColLen    (0),
m_CurrOldColNumber (1),
m_ColumnType       (ColumnType)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VarPartHandler::Log_VarPartHandler", LogAction_Trace, 5);
    if (Log_VarColumn == ColumnType)
    {
        m_pNewRec->recLen_gg00()       = FirstVarColOffset (pNewRec);
        m_pNewRec->recVarcolCnt_gg00() = 0;
    }
    else
    {
        //Log_VarLongColumn
        m_SizeofLength = 2;
    }
    
    ReadCurrOldColLen ();
}

//----------------------------------------------------------------------------

void Log_VarPartHandler::CopyUnchangedColumns (SAPDB_Int4        NumberNextChangedCol,
                                               tgg00_BasisError &Error,
                                               bool              CopyRest)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VarPartHandler::CopyUnchangedColumns", LogAction_Trace, 5);

    SAPDB_Int4 StartOffset = m_CurrOldOffset;
    
    while ( (m_CurrOldColLen > 0)
        && (CopyRest || (m_CurrOldColNumber < NumberNextChangedCol)) )        
    {
        NextOldColumn();
    }
    
    if (m_CurrOldOffset > StartOffset) 
    {
        if ( m_NewRecSize
             <
             m_pNewRec->recLen_gg00() + m_CurrOldOffset - StartOffset )
        {
            // PTS 1115833 UH 2002-08-28 NULL columns were added
            m_NewRecSize = m_pNewRec->recLen_gg00() + m_CurrOldOffset - StartOffset;
        }
        
        SAPDB_RangeMove (__FILE__, 2, m_pOldRec->recLen_gg00(), m_NewRecSize,
            m_pOldRec, StartOffset+1,
            m_pNewRec, m_pNewRec->recLen_gg00()+1,
            m_CurrOldOffset - StartOffset, Error);
        
        if (e_ok != Error)
        {
            #ifdef SAPDB_SLOW
            Kernel_VTrace() << "StartOffset: " << StartOffset
                            << ", m_NewRecSize: " << m_NewRecSize
                            << ", m_CurrOldOffset: " << m_CurrOldOffset
                            << ", m_pOldRec->recLen_gg00(): " << m_pOldRec->recLen_gg00();
            WriteToTrace("CopyUnchangedColumns");
            #endif
            return;
        }
        
        m_pNewRec->recLen_gg00() += m_CurrOldOffset - StartOffset;
    }
}

//----------------------------------------------------------------------------

void Log_VarPartHandler::CreateMissingUndefColumns (SAPDB_Int4        NumberNextChangedCol,
                                                    tgg00_BasisError &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VarPartHandler::CreateMissingUndefColumns", LogAction_Trace, 5);

    const SAPDB_UInt1 SizeofUndefByte = 1;
    
    while (m_CurrOldColNumber < NumberNextChangedCol)
    {        
        if (m_pNewRec->recLen_gg00() + m_SizeofLength + SizeofUndefByte > m_NewRecSize)
        {
            Error = e_column_trunc; // new record too small
            return;
        }
        
        if (Log_VarLongColumn == m_ColumnType)
        {
            PutInt2Length 
                (m_pNewRec->recBuf_gg00() + m_pNewRec->recLen_gg00(), SizeofUndefByte);                
        }
        else
        {
            m_pNewRec->recBuf_gg00() [m_pNewRec->recLen_gg00()] = SizeofUndefByte;
        }
        
        m_pNewRec->recLen_gg00() += m_SizeofLength;
        
        m_pNewRec->recBuf_gg00() [m_pNewRec->recLen_gg00()]  = csp_undef_byte;
        
        m_pNewRec->recLen_gg00() += SizeofUndefByte;
        
        NextOldColumn();
    }
}

//----------------------------------------------------------------------------

void Log_VarPartHandler::FinishVarPart (SAPDB_Int4       &OffsetFollowingPart,
                                        tgg00_BasisError &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VarPartHandler::FinishVarPart", LogAction_Trace, 5);

    const bool CopyRest = true;
    
    CopyUnchangedColumns (0, Error, CopyRest);
    
    if (e_ok != Error) return;
    
    if (Log_VarColumn == m_ColumnType)
    {
        m_pNewRec->recVarcolCnt_gg00() = m_CurrOldColNumber - 1;
    }
    
    OffsetFollowingPart = m_CurrOldOffset;
}

//----------------------------------------------------------------------------

inline const SAPDB_Byte *Log_VarPartHandler::GetOldColumn() const
{
    if (m_CurrOldColLen > 0)
    {
        return ( m_pOldRec->recBuf_gg00() + m_CurrOldOffset + m_CurrSizeofLength );
    }
    
    return (NULL);
}

//----------------------------------------------------------------------------

inline SAPDB_Int4 Log_VarPartHandler::GetOldColumnLength() const
{
    return ( m_CurrOldColLen );
}

//----------------------------------------------------------------------------

inline SAPDB_Int4 Log_VarPartHandler::GetSizeLength() const
{
    return ( m_SizeofLength );
}

//----------------------------------------------------------------------------

void Log_VarPartHandler::ReadCurrOldColLen ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VarPartHandler::ReadCurrOldColLen", LogAction_Trace, 5);

    m_CurrOldColLen    = 0;
    m_CurrSizeofLength = 0;
    
    if (Log_VarLongColumn == m_ColumnType)
    {
        if ( m_CurrOldOffset + m_SizeofLength > m_pOldRec->recLen_gg00() ) return;
        
        m_CurrOldColLen = GetInt2Length (m_pOldRec->recBuf_gg00() + m_CurrOldOffset);
    }
    else
    {
        if ( m_CurrOldColNumber > m_pOldRec->recVarcolCnt_gg00() ) return;
        
        m_CurrOldColLen = m_pOldRec->recBuf_gg00() [m_CurrOldOffset];
    }
    
    m_CurrSizeofLength = m_SizeofLength;
}

//----------------------------------------------------------------------------

void Log_VarPartHandler::NextOldColumn()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_VarPartHandler::NextOldColumn", LogAction_Trace, 5);

    ++m_CurrOldColNumber;
    
    m_CurrOldOffset += m_CurrSizeofLength + m_CurrOldColLen;
    
    ReadCurrOldColLen();
}


void Log_ColumnEntry::AllocateAndInitFixCol (SAPDBMem_IRawAllocator &RawAllocator,
                                             SAPDB_Int4              ColumnOffset,
                                             SAPDB_Int4              FixedColumnLen,
                                             SAPDB_Int4              ValueLength,
                                             const SAPDB_Byte       *pValue,
                                             tgg00_BasisError       &Error)
{
    if (e_ok != Error) return;
    
    m_pAllocatedValue = REINTERPRET_CAST (SAPDB_Byte*, RawAllocator.Allocate(FixedColumnLen));
    if (NULL == m_pAllocatedValue)
    {
        Error = e_no_more_memory;
        return;
    }
    
    m_pAllocator = &RawAllocator;

    m_ValueLength    = FixedColumnLen;
    m_OffsetOrNumber = ColumnOffset;
    m_pValue         = m_pAllocatedValue;
    
    SAPDB_MemCopyNoCheck (m_pAllocatedValue, pValue, ValueLength);
    
    if ( csp_unicode_def_byte == pValue[0] )
    {
        // fill trailing unicode defined bytes
        
        SAPDB_RangeUnicodeFill (__FILE__, 1, m_ValueLength,
            m_pAllocatedValue, ValueLength + 1, FixedColumnLen - ValueLength,
            csp_unicode_blank, Error);
        
        if (e_ok != Error) return;
    }
    else
    {
        if ( csp_undef_byte == pValue[0] )
        {
            // fill trailing bytes of undef value
            SAPDB_MemFillNoCheck (m_pAllocatedValue + ValueLength,
                LOG_UNDEF_FILL_BYTE,
                FixedColumnLen - ValueLength);
        }
        else
        {
            // fill trailing defined bytes
            SAPDB_MemFillNoCheck (                                       
                m_pAllocatedValue + ValueLength,
                pValue[0],
                FixedColumnLen - ValueLength);
        }
    }
}

//----------------------------------------------------------------------------

void Log_ColumnEntry::AllocateAndInitUndefCol (SAPDBMem_IRawAllocator &RawAllocator,
                                               SAPDB_Int4              OffsetOrNumber,
                                               SAPDB_Int4              FixedColumnLen,
                                               tgg00_BasisError       &Error)
{
    if (e_ok != Error) return;
    
    const SAPDB_Int4 SizeofDefinedByte = 1;
    
    m_ValueLength = (FixedColumnLen > 0) ? FixedColumnLen : SizeofDefinedByte;
    
    m_pAllocatedValue = REINTERPRET_CAST (SAPDB_Byte*, RawAllocator.Allocate(m_ValueLength));
    
    if (NULL == m_pAllocatedValue)
    {
        m_ValueLength = 0;
        Error = e_no_more_memory;
        return;
    }
    
    m_pAllocator = &RawAllocator;

    m_OffsetOrNumber = OffsetOrNumber;
    m_pValue         = m_pAllocatedValue;
    
    m_pAllocatedValue[0] = csp_undef_byte;
    
    if (m_ValueLength > SizeofDefinedByte)
    {
        SAPDB_MemFillNoCheck (m_pAllocatedValue+1, LOG_UNDEF_FILL_BYTE, this->m_ValueLength - SizeofDefinedByte);
    }
}

//----------------------------------------------------------------------------

void Log_ColumnEntry::Insert (Log_ColumnEntry  *pPrevEntry,
                              Log_ColumnEntry *&pFirstListEntry)
{
    if (NULL == pPrevEntry)
    {
        m_pNext   = pFirstListEntry;
        pFirstListEntry = this;
        return;
    }
    
    m_pNext       = pPrevEntry->m_pNext;
    pPrevEntry->m_pNext = this;
}

//----------------------------------------------------------------------------

void Log_ColumnEntry::Remove (Log_ColumnEntry  *pPrevEntry,
                              Log_ColumnEntry *&pFirstListEntry)
{
    if (NULL == pPrevEntry)
    {
        pFirstListEntry = m_pNext;
        return;
    }
    
    pPrevEntry->m_pNext = m_pNext;
    m_pNext       = NULL;
}

//----------------------------------------------------------------------------

void Log_ColumnEntry::WriteToTrace(const char *title) const
{
    Kernel_VTrace trace;
    if ( title != NULL )
        trace << title << FlushLine;
    trace << "Offset/No: " << m_OffsetOrNumber
          << ", length: "  << m_ValueLength << FlushLine;
    Kernel_TraceBuffer ( m_pValue, m_ValueLength, "New record" );
}

void Log_ColumnList::InsertSorted (Log_ColumnEntry *pEntry)
{
    Log_ColumnEntry *pCurr = m_pFirst;
    Log_ColumnEntry *pPrev = NULL;
    
    while (NULL != pCurr)
    {
        if (*pCurr > *pEntry) break;
        
        pPrev = pCurr;
        pCurr = pCurr->GetNext();
    }
    
    pEntry->Insert (pPrev, m_pFirst);
    
    m_SumValueLength += pEntry->GetLength();
    
    ++m_EntryCount;
}


/*===========================================================================*
*  METHODS of Log_RecordColumnMap                                            *
*============================================================================*/

Log_RecordColumnMap::Log_RecordColumnMap (Log_ActionType ActionType):

Log_ReadWriteActionImage (ActionType),
m_EntryHeader            (),
m_pRawAllocator          (NULL),
m_pAllocatedMap          (NULL),
m_ColList                (),
m_AllocatedEntries       (0),
m_UsedEntries            (0),
m_pAllValuesForRead      (NULL),
m_pAllValuesAllocator    (NULL),
m_PseudoNullValLen       (0)
{ 
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::Log_RecordColumnMap", LogAction_Trace, 5);
    for (Log_ColumnType Curr = Log_FixKeyColumn; Curr < Log_ColumnLastEnum; ++Curr)
        m_ColList[Curr].Init (Curr);
}

//----------------------------------------------------------------------------

Log_RecordColumnMap::~Log_RecordColumnMap()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::~Log_RecordColumnMap", LogAction_Trace, 5);

    if (NULL != m_pAllValuesAllocator)
    {
        m_pAllValuesAllocator->Deallocate (m_pAllValuesForRead);
        m_pAllValuesForRead = NULL;
    }
    
    if ((NULL != m_pAllocatedMap) && (m_AllocatedEntries > 0))
    {
        destroyarray (m_pAllocatedMap, m_AllocatedEntries, *(m_pRawAllocator));
    }
}

//----------------------------------------------------------------------------

bool Log_RecordColumnMap::AllocateColumnMap (SAPDBMem_IRawAllocator &RawAllocator,
                                             SAPDB_Int4              MaxEntries)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::AllocateColumnMap", LogAction_Trace, 5);
    
    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "MaxEntries: " << MaxEntries);

    if (m_pAllocatedMap != 0)
        return false;
    
    m_pRawAllocator = &RawAllocator;
    
    newarray (m_pAllocatedMap, MaxEntries, RawAllocator);
    
    if ( 0 == m_pAllocatedMap )
        return false;
    
    m_AllocatedEntries = MaxEntries;
    return true;
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CalculatePersistentImage()
{    
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CalculatePersistentImage", LogAction_Trace, 5);
    m_EntryHeader.ImageLen = 
        Log_AlignedImageLen   (sizeof (m_EntryHeader))
        + Log_AlignedImageLen (m_UsedEntries * sizeof (PersistentColEntry));
    
    m_EntryHeader.LengthAllValues = 0; 
    
    for (Log_ColumnType Curr = Log_FixKeyColumn; Curr < Log_ColumnLastEnum; ++Curr)
    {        
        m_EntryHeader.LengthAllValues += m_ColList[Curr].GetSumValueLength();
    }
    m_EntryHeader.ImageEntries     = m_UsedEntries;
    m_EntryHeader.FixColCnt        = m_ColList [Log_FixColumn    ].GetEntryCount();
    m_EntryHeader.VarColCnt        = m_ColList [Log_VarColumn    ].GetEntryCount();
    m_EntryHeader.VarLongColCnt    = m_ColList [Log_VarLongColumn].GetEntryCount();

    m_EntryHeader.ImageLen += Log_AlignedImageLen (m_EntryHeader.LengthAllValues);
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateAnyVarColPart (const Log_ColumnList &ColList,
                                               SAPDB_Int4           &OffsetFollowingOldPart,
                                               const tgg00_Rec      *pOldRec,
                                               tgg00_Rec            *pNewRec,
                                               SAPDB_Int4            NewRecSize,
                                               tgg00_BasisError     &Error)     const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateAnyVarColPart", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    
    if (e_ok != Error) return;
    
    Log_ColEntryIter Iter (ColList);
    
    Log_VarPartHandler VarPartHandler (ColList.GetColumnType(),
        OffsetFollowingOldPart, pOldRec, pNewRec, NewRecSize);
    
    while ( Iter.HasMore() )
    {
        Iter.Next();
        VarPartHandler.CopyUnchangedColumns (Iter.GetNumber(), Error);
        
        if (e_ok != Error) return;
        
        VarPartHandler.CreateMissingUndefColumns (Iter.GetNumber(), Error);
        
        if (e_ok != Error) return;
        
        Iter.MoveColToRec (pNewRec, NewRecSize, Error);
        
        if (e_ok != Error) return;
        
        VarPartHandler.NextOldColumn();
    } 
    
    VarPartHandler.FinishVarPart (OffsetFollowingOldPart, Error);
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateAnyVarColPartAndBeforeImage (Log_ColumnList      &ColList,
                                                             SAPDB_Int4          &OffsetFollowingOldPart,
                                                             const tgg00_Rec     *pOldRec,
                                                             tgg00_Rec           *pNewRec,
                                                             SAPDB_Int4           NewRecSize,
                                                             Log_RecordColumnMap &BeforeImage,
                                                             tgg00_BasisError    &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateAnyVarColPartAndBeforeImage", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);

    const SAPDB_Int4 SizeofDefinedByte = 1;

    if (e_ok != Error) return;
    
    Log_EntryIterWithRemove Iter (ColList);
    
    Log_VarPartHandler VarPartHandler (ColList.GetColumnType(),
        OffsetFollowingOldPart, pOldRec, pNewRec, NewRecSize);
    
    SAPDBTRACE_IF (LogAction_Trace,6,VarPartHandler.WriteToTrace("Log_RecordColumnMap::CreateAnyVarColPartAndBeforeImage::VarPartHandler begin"));

    SAPDB_UInt2 OldRecLen;

    while ( Iter.HasMore() )
    {
        Iter.Next();
        
        VarPartHandler.CopyUnchangedColumns (Iter.GetNumber(), Error);
        
        if (e_ok != Error) return;
        
        OldRecLen = pNewRec->recLen_gg00();
        VarPartHandler.CreateMissingUndefColumns (Iter.GetNumber(), Error);
        BeforeImage.AddNullValLen (pNewRec->recLen_gg00() - OldRecLen);
        
        if (e_ok != Error) return;
        
        if ( Iter.IsEqualValue (VarPartHandler.GetOldColumn(), VarPartHandler.GetOldColumnLength()) )
        {
            -- m_UsedEntries;
            Iter.Remove ();
        }
        else
        {           
            if (VarPartHandler.GetOldColumnLength() > 0)
            {
                BeforeImage.InsertValue (ColList.GetColumnType(), Iter.GetNumber(), 
                    VarPartHandler.GetOldColumnLength(), VarPartHandler.GetOldColumn(), Error);
            }
            else
            {
                BeforeImage.InsertVarUndefValue (ColList.GetColumnType(), Iter.GetNumber(), Error);
                BeforeImage.AddNullValLen (VarPartHandler.GetSizeLength() + SizeofDefinedByte);
            }
            
            if (e_ok != Error) return;
            
            Iter.MoveColToRec (pNewRec, NewRecSize, Error);
            
            if (e_ok != Error) return;
            
            VarPartHandler.NextOldColumn();
        }
    } 
    
    VarPartHandler.FinishVarPart (OffsetFollowingOldPart, Error);

    SAPDBTRACE_IF (LogAction_Trace,6,VarPartHandler.WriteToTrace("Log_RecordColumnMap::CreateAnyVarColPartAndBeforeImage::VarPartHandler end"));
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateColDescForExistingNewRec (SAPDBMem_IRawAllocator &RawAllocator,
                                                          const tgg00_Rec        *pOldRecord,
                                                          const tgg00_Rec        *pNewRecord,
                                                          Log_RecordColumnMap    &BeforeImage,
                                                          tgg00_BasisError       &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateColDescForExistingNewRec", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    SAPDBTRACE_IF         (LogAction_Trace,5,Kernel_TraceBuffer(pOldRecord,pOldRecord->recLen_gg00(),"OldRecord"));
    SAPDBTRACE_IF         (LogAction_Trace,5,Kernel_TraceBuffer(pNewRecord,pNewRecord->recLen_gg00(),"NewRecord"));
    
    if (e_ok != Error) return;
    
    if ( pOldRecord->recKeyLen_gg00() != pNewRecord->recKeyLen_gg00() )
    {
        Error = e_key_update;
        return;
    }
    
    if (0 != memcmp
        (pOldRecord->recBody_gg00()+0, pNewRecord->recBody_gg00()+0, pOldRecord->recKeyLen_gg00()))
    {
        // key of OldRecord != key of NewRecord
        Error = e_key_update;
        return;
    }
    
    {
        const SAPDB_Byte *pOldRest        = pOldRecord->recBuf_gg00() + FirstFixColOffset (pOldRecord);
        const SAPDB_Byte *pNewRest        = pNewRecord->recBuf_gg00() + FirstFixColOffset (pNewRecord);
        const SAPDB_Int   OldLen          = pOldRecord->recLen_gg00() - FirstFixColOffset (pOldRecord);
        const SAPDB_Int   NewLen          = pNewRecord->recLen_gg00() - FirstFixColOffset (pNewRecord);
        const SAPDB_Int   MinLen          = (OldLen > NewLen) ? NewLen : OldLen;
        const SAPDB_Int   ComparePartSize = 32;
        const SAPDB_Int   CompareEntries  = ((MinLen+ComparePartSize-1) / ComparePartSize);
        
        bool      IsOk       = true;
        SAPDB_Int CurrOffset = 0;
        SAPDB_Int CompareLen;
        
        if (OldLen == NewLen)
        {
            if (0 == OldLen)
            {
                Error = e_skip_upd;
                return;
            }
            
            if ( 0 == memcmp (pOldRest, pNewRest, OldLen) )
            {
                Error = e_skip_upd;
                return;
            }
        }
        
        IsOk = AllocateColumnMap (RawAllocator, CompareEntries+1);
        
        if (IsOk)
        {
            IsOk = BeforeImage.AllocateColumnMap (RawAllocator, CompareEntries+1);
        }
        
        if ( ! IsOk)
        {
            Error = e_no_more_memory;
            return;
        }
        
        while (CurrOffset < MinLen)
        {
            CompareLen = (CurrOffset + ComparePartSize <= MinLen) ? ComparePartSize : MinLen - CurrOffset;
            
            if ( 0 != memcmp (pOldRest+CurrOffset, pNewRest+CurrOffset, CompareLen) )
            {
                BeforeImage.InsertValue (Log_FixColumn, CurrOffset, CompareLen, pOldRest+CurrOffset, Error);
                
                if (e_ok != Error) return;
                
                InsertValue (Log_FixColumn, CurrOffset, CompareLen, pNewRest+CurrOffset, Error);
                
                if (e_ok != Error) return;
            }
            
            CurrOffset += CompareLen;
        }
        
        if (OldLen > MinLen)
        {
            BeforeImage.InsertValue (Log_FixColumn, CurrOffset, OldLen-MinLen, pOldRest+MinLen, Error);
            
            if (e_ok != Error) return;
        }
        
        if (NewLen > MinLen)
        {
            InsertValue (Log_FixColumn, CurrOffset, NewLen-MinLen, pNewRest+MinLen, Error);
            
            if (e_ok != Error) return;
        }
    }
    
    BeforeImage.CalculatePersistentImage();
    CalculatePersistentImage();
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateFixColPartAndBeforeImage (const tgg00_Rec     *pOldRec,
                                                          tgg00_Rec           *pNewRec,
                                                          SAPDB_Int4           NewRecSize,
                                                          Log_RecordColumnMap &BeforeImage,
                                                          tgg00_BasisError    &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateFixColPartAndBeforeImage", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    
    if (e_ok != Error) return;
    
    SAPDB_Int4 MoveLen = FirstVarColOffset(pOldRec) - FirstFixColOffset(pOldRec);
    
    if (MoveLen > 0)
    {
        // copy all fixed columns of the old record to the new record
        SAPDB_RangeMove (__FILE__, 3, pOldRec->recLen_gg00(), NewRecSize,
            pOldRec, FirstFixColOffset(pOldRec) + 1,
            pNewRec, FirstFixColOffset(pNewRec) + 1,
            MoveLen, Error);
        
        if (e_ok != Error) return;
    }
    
    if ( m_ColList[Log_FixColumn].IsEmpty() ) return;
    
    {
        Log_EntryIterWithRemove  Iter (m_ColList[Log_FixColumn]);
        const SAPDB_Byte        *pOldColumn;
        
        while ( Iter.HasMore() )
        {
            Iter.Next();
            
            pOldColumn = pOldRec->recBuf_gg00() + (FirstFixColOffset(pOldRec) + Iter.GetOffset());
            
            if ( Iter.IsEqualValue (pOldColumn, Iter.GetLength()) )
            {
                // old fix column == new value
                -- m_UsedEntries;
                Iter.Remove (); 
            }
            else
            {
                BeforeImage.InsertValue
                    (Log_FixColumn, Iter.GetOffset(), Iter.GetLength(), pOldColumn, Error);
                
                if (e_ok != Error) return;
                
                // replace the old fixed column
                Iter.MoveColToRec (pNewRec, NewRecSize, Error);
                
                if (e_ok != Error) return;
            }
        }
    }
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateKeyAndFixColPart (const tgg00_Rec  *pOldRec,
                                                  tgg00_Rec        *pNewRec,
                                                  bool              WithoutVarParts,
                                                  tgg00_BasisError &Error)         const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateKeyAndFixColPart", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    
    if (e_ok != Error) return;
    
    SAPDB_Int4 MoveLen = FirstVarColOffset (pOldRec);
    
    if (WithoutVarParts)
    {
        MoveLen = (pOldRec->recLen_gg00() < pNewRec->recLen_gg00()) ?
            pOldRec->recLen_gg00() : pNewRec->recLen_gg00();
    }
    
    MoveLen -= sizeof (pOldRec->recLen_gg00());
    
    // copy header without len, all key columns and all fixed columns or the rest of the record
    SAPDB_RangeMove (__FILE__, 4, pOldRec->recLen_gg00(), pNewRec->recLen_gg00(),
        pOldRec, sizeof (pOldRec->recLen_gg00()) + 1,
        pNewRec, sizeof (pNewRec->recLen_gg00()) + 1, MoveLen, Error);
    
    if (e_ok != Error) return;
    
    if ( ! m_ColList[Log_FixColumn].IsEmpty() )    
    {
        Log_ColEntryIter Iter (m_ColList[Log_FixColumn]);
        
        while ( Iter.HasMore() )
        {
            Iter.Next();
            
            // replace the old fixed column   
            Iter.MoveColToRec (pNewRec, pNewRec->recLen_gg00(), Error);
            
            if (e_ok != Error) return;
        }
    }
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateKeyPart (const tgg00_Rec  *pOldRec,
                                         tgg00_Rec        *pNewRec,
                                         SAPDB_Int4        NewRecSize,
                                         bool             &KeyUpdated,
                                         tgg00_BasisError &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateKeyPart", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    
    SAPDB_Int4        OldColumnLen;
    const SAPDB_Byte *pOldColumn;
    
    if (e_ok != Error) return;
    
    KeyUpdated = false;
    
    // copy header and all key columns of the old record to the new record
    SAPDB_RangeMove (__FILE__, 5, pOldRec->recLen_gg00(), NewRecSize,
        pOldRec, 1, pNewRec, 1, FirstFixColOffset (pOldRec), Error);
    
    if (e_ok != Error) return;
    
    pNewRec->recLen_gg00()       = FirstVarColOffset (pNewRec);
    pNewRec->recVarcolCnt_gg00() = 0;
    
    for (Log_ColumnType CurrList = Log_FixKeyColumn; CurrList <= Log_VarKeyColumn; ++CurrList)
    {
        if ( ! m_ColList[CurrList].IsEmpty() )
        {
            // replace changed key columns
            
            Log_EntryIterWithRemove Iter (m_ColList[CurrList]);
            
            while ( Iter.HasMore() )
            {
                Iter.Next();
                
                OldColumnLen = (Log_FixKeyColumn == CurrList) ?
                    Iter.GetLength() : pOldRec->recKeyLen_gg00() - Iter.GetOffset();
                
                pOldColumn = pOldRec->recBuf_gg00() + FirstKeyColOffset() + Iter.GetOffset();
                
                if ( Iter.IsEqualValue (pOldColumn, OldColumnLen) )
                {
                    // old fix key == new value
                    -- m_UsedEntries;
                    Iter.Remove (); 
                }
                else
                {
                    KeyUpdated = true;
                    
                    Iter.MoveColToRec (pNewRec, NewRecSize, Error);
                    
                    if (e_ok != Error) return;
                    
                    if (Log_VarKeyColumn == CurrList)
                    { 
                        pNewRec->recKeyLen_gg00() = Iter.GetOffset() + Iter.GetLength();
                        pNewRec->recLen_gg00()    = FirstVarColOffset (pNewRec);
                    }
                }
            } 
        }
    }
}

//----------------------------------------------------------------------------

 void Log_RecordColumnMap::AppendColumn (
        const tgg00_StackEntry &StackEntry,
        const tgg00_Rec        &Image,
        tgg00_BasisError       &Error)
{
     int offsetOrNumber = StackEntry.epos();
     Log_ColumnType colType;
     switch (StackEntry.etype()) 
     {
     case st_fixkey :
         colType = Log_FixKeyColumn;
         break;
     case st_varkey :
         colType = Log_VarKeyColumn;
         break;
     case st_fixcol :
         --offsetOrNumber;  
         colType = Log_FixColumn;
         break;
     case st_varcol :
         colType = Log_VarColumn;
         break;
     case st_varlongchar :
         colType = Log_VarLongColumn;
     }

     tgg00_VarColPosList varColPos;
     SAPDB_Int           colOffset;
     SAPDB_Int           colLen;

     varColPos.vpl_last = -1;
     g04locate_col (StackEntry, &Image, varColPos, colOffset, colLen);

     this->InsertValue (colType, offsetOrNumber, colLen,
         reinterpret_cast<const SAPDB_Byte*>(&Image) + colOffset - 1, Error);
     this->CalculatePersistentImage();
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateNewRecord (const tgg00_Rec  *pOldRec,
                                           tgg00_Rec        *pNewRec,
                                           tgg00_BasisError &Error)     const
{   
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateNewRecord", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    SAPDBTRACE_IF (LogAction_Trace, 6, Kernel_TraceBuffer(pOldRec,pOldRec->recLen_gg00(),"OldRecord"));
    
    if (e_ok != Error) return;
    
    const SAPDB_Int4 NewRecSize = pNewRec->recLen_gg00();
    
    const bool WithoutVarParts = 
        (m_ColList[Log_VarColumn].IsEmpty()
        &&
        m_ColList[Log_VarLongColumn].IsEmpty() );
    
    CreateKeyAndFixColPart (pOldRec, pNewRec, WithoutVarParts, Error);
    
    if (e_ok != Error) return;
    
    
    if ( ! WithoutVarParts )
    {
        SAPDB_Int4 OffsetFollowingOldPart = FirstVarColOffset (pOldRec);
        
        pNewRec->recLen_gg00()       = FirstVarColOffset (pNewRec);
        pNewRec->recVarcolCnt_gg00() = 0;
        
        CreateAnyVarColPart (m_ColList [Log_VarColumn],
            OffsetFollowingOldPart, pOldRec, pNewRec, NewRecSize, Error);
        
        if (e_ok != Error) return;

        CreateAnyVarColPart (m_ColList [Log_VarLongColumn],
            OffsetFollowingOldPart, pOldRec, pNewRec, NewRecSize, Error);
    }

    SAPDBTRACE_IF(LogAction_Trace,5,Kernel_TraceBuffer(pNewRec,pNewRec->recLen_gg00(),"NewRecord"));
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::CreateNewRecordAndBeforeImage (const tgg00_Rec     *pOldRec,
                                                         tgg00_Rec           *pNewRec,
                                                         SAPDB_Int4           NewRecSize,
                                                         Log_RecordColumnMap &BeforeImage,
                                                         tgg00_BasisError    &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::CreateNewRecordAndBeforeImage", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    SAPDBTRACE_IF(LogAction_Trace,5,Kernel_TraceBuffer(pOldRec,pOldRec->recLen_gg00(),"OldRecord"));
    
    if (e_ok != Error) return;
    
    bool KeyUpdated = false;
    
    SAPDBTRACE_IF (LogAction_Trace,6,WriteToTrace("Log_RecordColumnMap::CreateNewRecordAndBeforeImage 1"));
    
    CreateKeyPart (pOldRec, pNewRec, NewRecSize, KeyUpdated, Error);
    
    SAPDBTRACE_IF (LogAction_Trace,6,WriteToTrace("Log_RecordColumnMap::CreateNewRecordAndBeforeImage 2"));
    
    if (e_ok != Error) return;
    
    CreateFixColPartAndBeforeImage (pOldRec, pNewRec, NewRecSize, BeforeImage, Error);
    
    SAPDBTRACE_IF (LogAction_Trace,6,WriteToTrace("Log_RecordColumnMap::CreateNewRecordAndBeforeImage 3"));
    
    if (e_ok != Error) return;
    
    SAPDB_Int4 OffsetFollowingOldPart = FirstVarColOffset (pOldRec);
    
    CreateAnyVarColPartAndBeforeImage (m_ColList [Log_VarColumn],
        OffsetFollowingOldPart, pOldRec, pNewRec, NewRecSize, BeforeImage, Error);
    
    SAPDBTRACE_IF (LogAction_Trace,6,WriteToTrace("Log_RecordColumnMap::CreateNewRecordAndBeforeImage 4"));

    if (e_ok != Error) return;
    
    CreateAnyVarColPartAndBeforeImage (m_ColList [Log_VarLongColumn],
        OffsetFollowingOldPart, pOldRec, pNewRec, NewRecSize, BeforeImage, Error);
    
    SAPDBTRACE_IF (LogAction_Trace,6,WriteToTrace("Log_RecordColumnMap::CreateNewRecordAndBeforeImage 5"));

    if (e_ok != Error) return;
    
    if (KeyUpdated)
    {
        Error = e_key_update;
        return;
    }
    
    BeforeImage.CalculatePersistentImage();
    CalculatePersistentImage();
    
    if (m_EntryHeader.ImageEntries < 1) Error = e_skip_upd;
    
    SAPDBTRACE_IF(LogAction_Trace,5,Kernel_TraceBuffer(pNewRec,pNewRec->recLen_gg00(),"NewRecord"));
}

//----------------------------------------------------------------------------

bool Log_RecordColumnMap::ExistsEntry (const tgg00_StackEntry &StackEntry) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::ExistsEntry", LogAction_Trace, 5);
    const Log_ColumnType ColumnType = GetColumnType (StackEntry);
    
    if ( m_ColList [ColumnType].IsEmpty() ) return (false);
    {
        const Log_ColumnEntry *CurrEntry = m_ColList [ColumnType].GetFirst();
        
        while (NULL != CurrEntry)
        {
            if ((Log_FixKeyColumn == ColumnType) ||
                (Log_VarKeyColumn == ColumnType) ||
                (Log_FixColumn    == ColumnType) )
            {
                if ( CurrEntry->GetOffset()+1 == StackEntry.epos() ) return (true);
            }
            else
            {
                if ( CurrEntry->GetNumber() == StackEntry.ecolno() ) return (true);
            }
            
            CurrEntry = CurrEntry->GetNext();
        }
    }
    return (false);
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::InsertColumn (const tgg00_StackEntry &StackEntry,
                                        const SAPDB_Byte       *pValue,
                                        SAPDB_Int4              ValueLength,
                                        tgg00_BasisError       &Error)
{   
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::InsertColumn", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    
    if (e_ok != Error) return;
    
    const Log_ColumnType ColumnType = GetColumnType(StackEntry);
    
    const SAPDB_Int4 ColOffsetOrNumber =
        ((Log_VarColumn == ColumnType) || (Log_VarLongColumn == ColumnType)) ?
        StackEntry.ecolno() : (StackEntry.epos()-1);
    
    const SAPDB_Int4 FixedColumnLen =
        ((Log_FixColumn == ColumnType) || (Log_FixKeyColumn == ColumnType)) ? StackEntry.elen_var() : 0;
    
    SAPDB_Int4       TruncValueLen    = ValueLength;
    bool             CreateUndefValue = false;
    Log_ColumnEntry *pNewEntry        = GetFreeEntry();
    
    if ((NULL == pNewEntry) || (Log_NilColumn == ColumnType))
    {
        Error = e_stack_type_illegal;
        return;
    }
    
    TruncateValue (pValue, TruncValueLen, CreateUndefValue);
    
    if (CreateUndefValue)
    {
        pNewEntry->AllocateAndInitUndefCol
            ( *(m_pRawAllocator), ColOffsetOrNumber, FixedColumnLen, Error);
        
        if (e_ok != Error) return;
    }
    else
    {
        if ((Log_FixColumn == ColumnType) || (Log_FixKeyColumn == ColumnType))
        {
            if (TruncValueLen > FixedColumnLen)
            {
                Error = e_column_trunc; // value too large
                return;
            }
            
            if (ValueLength < FixedColumnLen)
            {
                // enlarge value with trailing defined bytes
                pNewEntry->AllocateAndInitFixCol ( *(m_pRawAllocator),
                    ColOffsetOrNumber, FixedColumnLen, ValueLength, pValue, Error);
                
                if (e_ok != Error) return;
            }
            else  
            {
                pNewEntry->Init (ColOffsetOrNumber, FixedColumnLen, pValue);
            }
        }
        else
        {
            pNewEntry->Init (ColOffsetOrNumber, TruncValueLen, pValue);
        }
    }
    
    switch (ColumnType)
    {
    case Log_FixKeyColumn:
    case Log_VarKeyColumn:
        {
            if (ColOffsetOrNumber + pNewEntry->GetLength() > SAPDB_MAX_UINT2 )
            {
                Error = e_column_trunc; // key too large
                return;
            }
            
            if (csp_undef_byte == pNewEntry->GetValue()[0])
            {
                Error = e_null_value_illegal;
                return;
            }
            
            m_ColList[ColumnType].InsertOnTop (pNewEntry);
            break;
        }
    case Log_FixColumn:
        {
            m_ColList[Log_FixColumn].InsertOnTop (pNewEntry);
            break;
        }
    case Log_VarColumn:
        {
            if (TruncValueLen > SAPDB_MAX_UINT1)
            {
                Error = e_column_trunc; // value too large
                return;
            }
            m_ColList[Log_VarColumn].InsertSorted (pNewEntry);
            break;
        }
    case Log_VarLongColumn:
        {
            if (TruncValueLen > SAPDB_MAX_UINT2)
            {
                Error = e_column_trunc; // value too large
                return;
            }
            m_ColList[Log_VarLongColumn].InsertSorted (pNewEntry);
            break;
        }
    }
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::InsertValue (Log_ColumnType    ColumnType,
                                       SAPDB_Int4        OffsetOrNumber,
                                       SAPDB_Int4        ValueLength,
                                       const SAPDB_Byte *pValue,
                                       tgg00_BasisError &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::InsertValue", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);

    if (e_ok != Error) return;
    
    Log_ColumnEntry *pNewEntry = GetFreeEntry();
    
    if ((ValueLength < 1) || (NULL == pNewEntry))
    {
        Error = e_stack_type_illegal;
        return;
    }
    
    pNewEntry->Init (OffsetOrNumber, ValueLength, pValue);
    
    switch (ColumnType)
    {
    case Log_FixColumn:
        m_ColList[ColumnType].InsertOnTop (pNewEntry);
        break;
        
    case Log_VarColumn:
    case Log_VarLongColumn:
        m_ColList[ColumnType].InsertSorted (pNewEntry);
        break;
        
    default:
        Error = e_stack_type_illegal;
        return;
    }
}

//----------------------------------------------------------------------------

void Log_RecordColumnMap::InsertVarUndefValue (Log_ColumnType    ColumnType,
                                               SAPDB_Int4        OffsetOrNumber,
                                               tgg00_BasisError &Error)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::InsertVarUndefValue", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (Error, LogAction_Trace, 5);
    
    if (e_ok != Error) return;
    
    Log_ColumnEntry *pNewEntry = GetFreeEntry();
    
    if (NULL == pNewEntry)
    {
        Error = e_stack_type_illegal;
        return;
    }
    
    const SAPDB_Int4 UndefColumnLen = 0;
    
    pNewEntry->AllocateAndInitUndefCol
        ( *(m_pRawAllocator), OffsetOrNumber, UndefColumnLen, Error);
    
    if (e_ok != Error) return;
    
    if ((Log_VarColumn     == ColumnType) ||
        (Log_VarLongColumn == ColumnType) )
    {
        m_ColList[ColumnType].InsertSorted (pNewEntry);
        return;
    }
    
    Error = e_stack_type_illegal;
    return;
}

//----------------------------------------------------------------------------

bool Log_RecordColumnMap::ReadColumnMapPersistent (Log_ActionImageReader &ImageReader,
                                                   SAPDB_Int4             PersistentLen,
                                                   SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::ReadColumnMapPersistent", LogAction_Trace, 5);
    
    PersistentColEntry     *pPersistEntries          = NULL;
    SAPDBMem_IRawAllocator *pPersistEntriesAllocator = NULL;
    
    if(!ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_EntryHeader)),sizeof(m_EntryHeader),errlist))
        return false;

    if (m_EntryHeader.ImageEntries > 0)
    {
        if (!AllocateColumnMap (ImageReader.GetAllocator(), m_EntryHeader.ImageEntries))
            return false;
        
        if(!ImageReader.PersistentRead (REINTERPRET_CAST (SAPDB_Byte*&, pPersistEntries),
            m_EntryHeader.ImageEntries * sizeof (PersistentColEntry), pPersistEntriesAllocator, errlist))
            return false;
        
        if (m_EntryHeader.LengthAllValues > 0)
        {
            if(!ImageReader.PersistentRead (m_pAllValuesForRead,
                m_EntryHeader.LengthAllValues, m_pAllValuesAllocator, errlist))
                return false;
        }
        
        SAPDB_Int4       EntryCntPerList;
        SAPDB_Int4       CurrPersist     = 0;
        SAPDB_Int4       CurrValueOffset = 0;
        Log_ColumnType   CurrList        = Log_FixColumn; // update image does not contain any altered key
        tgg00_BasisError Error           = e_ok;
        SAPDB_UInt2      EntryCnt [Log_ColumnLastEnum];
        
        EntryCnt [Log_FixColumn    ] = m_EntryHeader.FixColCnt;
        EntryCnt [Log_VarColumn    ] = m_EntryHeader.VarColCnt;
        EntryCnt [Log_VarLongColumn] = m_EntryHeader.VarLongColCnt;
        
        do
        {
            EntryCntPerList = 0;
            
            while ( EntryCntPerList < EntryCnt[CurrList] )
            { 
                InsertValue (
                    CurrList,
                    (pPersistEntries + CurrPersist)->OffsetOrNumber,
                    (pPersistEntries + CurrPersist)->ValueLength,
                    m_pAllValuesForRead + CurrValueOffset,
                    Error);
                    
                if( Error != e_ok)
                    return false;
                
                CurrValueOffset += (pPersistEntries + CurrPersist)->ValueLength;
                ++EntryCntPerList;
                ++CurrPersist;
                
                if (CurrValueOffset > m_EntryHeader.LengthAllValues)
                    return false;
            }          
            ++CurrList; 
        }
        while ( CurrList < Log_ColumnLastEnum );
        
        if ( pPersistEntriesAllocator != 0 )
            pPersistEntriesAllocator->Deallocate (pPersistEntries);
    }
    SAPDBTRACE_IF (LogAction_Trace, 6, WriteToTrace("ReadColumnMapPersistent"));
    return true;
}

//----------------------------------------------------------------------------

bool Log_RecordColumnMap::WriteColumnMapPersistent (Log_ActionImageWriter &ImageWriter,
                                                    SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_RecordColumnMap::WriteColumnMapPersistent", LogAction_Trace, 5);
    SAPDBTRACE_IF (LogAction_Trace, 6, WriteToTrace("WriteColumnMapPersistent"));

    if(!ImageWriter.PersistentWrite (&(m_EntryHeader), sizeof (m_EntryHeader), errlist))
        return false;

    if (m_EntryHeader.ImageEntries > 0)
    {
        const Log_ColumnEntry *pCurrEntry;
        Log_ColumnType         CurrColList;
        PersistentColEntry    *pPersistEntries = NULL;
        SAPDB_Int4             CurrPersist     = 0;
        
        newarray (pPersistEntries, m_EntryHeader.ImageEntries, *m_pRawAllocator);
        
        if (NULL == pPersistEntries)
        {
            errlist = Log_Exception(__CONTEXT__,LOG_ALLOCATION_FAILED);
            return false;
        }
        
        // update image does not contain any altered key, therefore FixColumn .. VarLongColumn
        
        for (CurrColList = Log_FixColumn; CurrColList < Log_ColumnLastEnum; ++CurrColList)
        {
            pCurrEntry = m_ColList[CurrColList].GetFirst();
            
            while (NULL != pCurrEntry)
            {
                (pPersistEntries + CurrPersist)->OffsetOrNumber = pCurrEntry->GetOffset();
                (pPersistEntries + CurrPersist)->ValueLength    = pCurrEntry->GetLength();
                pCurrEntry = pCurrEntry->GetNext();
                ++CurrPersist;
            }
        }
        
        if(!ImageWriter.PersistentWrite (pPersistEntries,
                                         m_EntryHeader.ImageEntries * sizeof (PersistentColEntry),
                                         errlist))
            return false;
        
        destroyarray (pPersistEntries, m_EntryHeader.ImageEntries, *(m_pRawAllocator));
        
        if (m_EntryHeader.LengthAllValues > 0)
        {
            for (CurrColList = Log_FixKeyColumn; CurrColList < Log_ColumnLastEnum; ++CurrColList)
            {
                pCurrEntry = m_ColList[CurrColList].GetFirst();
                
                while (NULL != pCurrEntry)
                {
                    if ( NULL != pCurrEntry->GetValue() )
                    {
                        if(!ImageWriter.UnalignedPersistentWrite
                            (pCurrEntry->GetValue(), pCurrEntry->GetLength(), errlist))
                            return false;
                    }
                    
                    pCurrEntry = pCurrEntry->GetNext();
                }
            }
        }
        if(!ImageWriter.AlignImage (m_EntryHeader.LengthAllValues,errlist))
            return false;
    }
    return true;
}
