  /*!
  @file           Data_Types.hpp
  @author         UweH
  @author         TillL
  @brief          Base types of this component

\if EMIT_LICENCE
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

\endif
*/
#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP

#include "ggg91.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

/// Class SAPDB_OStream is needed by SAPDB_OStream & operator << ( ).
class SAPDB_OStream;

/// external defined trace topic
extern SAPDBTrace_Topic     DataChain_Trace;
/// external defined trace topic
extern SAPDBTrace_Topic     DataPam_Trace;
/// external defined trace topic
extern SAPDBTrace_Topic     DataTree_Trace;
/// external defined trace topic
extern SAPDBTrace_Topic     DataIndex_Trace;
/// external defined trace topic
extern SAPDBTrace_Topic     DataPagePrim_Trace;
/// external defined trace topic
extern SAPDBTrace_Topic     DataPageInv_Trace;
/// external defined trace topic
extern SAPDBTrace_Topic     DataPageLog_Trace;

/// external defined check topic
extern SAPDBDiag_CheckTopic DataChain_Check;
/// external defined check topic
extern SAPDBDiag_CheckTopic DataPam_Check;
/// external defined check topic
extern SAPDBDiag_CheckTopic DataTree_Check;
/// external defined check topic
extern SAPDBDiag_CheckTopic DataIndex_Check;
/// external defined check topic
extern SAPDBDiag_CheckTopic DataPagePrim_Check;
/// external defined check topic
extern SAPDBDiag_CheckTopic DataPageInv_Check;
/// external defined check topic
extern SAPDBDiag_CheckTopic DataPageLog_Check;

/// Indicates how a page is cached within the PageAccessManager.
enum Data_AccessMode
{
    Data_ForRead             = 0,
    Data_ForIntend           = 1,
    Data_ForUpdate           = 2,
    Data_ForStructureChange  = 3,
    Data_AccessMode_DONT_USE = 6
};
/// This is the string representation of the previous declaration
extern const char * Data_AccessModeStrings [];
/// indicates how the persistent page is released in the PageAccessManager
enum Data_ReleaseMode
{
    Data_Changed               = 0,
    Data_Unchanged             = 1,
    Data_ReleaseMode_DONOT_USE = 2
};
/// This is the string representation of the previous declaration
extern const char * Data_ReleaseModeStrings [];
/*!
    @brief indicates how the persistent page is chained within the data cache LRU List.
    
    Data_LruNormal leaves the page at the current position
    assumed the page is not in the last 10 percent of the LRU list.
    In this case the page will be rechained to the begining.
    Data_LruMiddle puts the page in front of the last 10 percent of the LRU list
    and Data_LruLast inserts the page at the end.
 */
enum Data_LRUChainInfo
{
    Data_LruNormal             = 0, 
    Data_LruMiddle             = 1,
    Data_LruLast               = 2,
    Data_LRUChainInfo_DONT_USE = 3
};
/// This is the string representation of the previous declaration
extern const char * Data_LRUChainInfoStrings [];
/// page addressing mode within the converter
enum Data_AddressingMode
{
    Data_Static          = 0,
    Data_Dynamic         = 1,
    Data_AddressingModes = 2
};
/// page addressing mode within the converter
extern const char * Data_AddressingModeStrings [];
/// recoverability of a data page
enum Data_Recoverability
{
    Data_Recoverable    = 0, // permanent
    Data_NotRecoverable = 1, // temporary
    Data_RecoveryModes  = 2
};
/// page addressing mode within the converter
extern const char * Data_RecoverabilityStrings [];
/// storage volume mode of a data page
enum Data_VolumeMode
{
    Data_VolumeNormal     = 0,  // normal random access volume
    Data_VolumeSequential = 1,  // sequential HSM storage volume
    Data_VolumeUnknown    = 2,  // unknown volume, not specified
    Data_VolumeModes      = 3   // count
};
/// count of pages
typedef SAPDB_UInt4 Data_PageCount;
/// handles a logical page number (former: pno)
typedef SAPDB_Invalidatable<SAPDB_Int4, SAPDB_MAX_INT4> Data_PageNo;
/*!
    @class Data_PageRecoveryMode
    @brief Handles the attributes which define the recovery mode of a data page
    
    Data_PageRecoveryMode recoverymode(Data_Static,  Data_NotRecoverable);
    Data_PageRecoveryMode recoverymode(Data_Static,  Data_Recoverable);
    Data_PageRecoveryMode recoverymode(Data_Dynamic, Data_Recoverable);
    ...
    if (recoverymode.UseDynamicPageAddressing()) ...
    if (recoverymode.UseStaticPageAddressing()) ...
    if (recoverymode.PageIsRecoverable()) ...
    ...

    Defines an enumeration type Modes which offers the supported modes
 */
class Data_PageRecoveryMode
{
public:
    /*!
        @brief Default Constructor;  optionally sets mode
        @param addrmode [in] addressing mode (default: Data_Dynamic)
        @param recmode [in] recovery mode (default: Data_Recoverable)
     */
    Data_PageRecoveryMode
        (Data_AddressingMode            addrmode = Data_Dynamic,
         Data_Recoverability            recmode  = Data_Recoverable)
        : 
        m_AddressingMode(addrmode),
        m_Recoverability(recmode)
    {}
    /*!
        @brief  sets mode
        @param  addrmode [in] addressing mode
        @param  recmode [in] recovery mode
     */
    void SetMode  (Data_AddressingMode            addrmode,
                   Data_Recoverability            recmode)
    {
        m_AddressingMode = addrmode;
        m_Recoverability = recmode;
    }
    /// Checks whether page is recoverable
    SAPDB_Bool PageIsRecoverable() const
    {
        return m_Recoverability == Data_Recoverable;
    }
    /// Checks whether static page addressing is to be used
    SAPDB_Bool UseStaticPageAddressing() const
    {
        return m_AddressingMode == Data_Static;
    }
    /// Checks whether dynamic page addressing is to be used
    SAPDB_Bool UseDynamicPageAddressing() const
    {
        return m_AddressingMode == Data_Dynamic;
    }
    /// Returns page addressing mode
    Data_AddressingMode GetAddressingMode() const
    {
        return m_AddressingMode;
    }
    /// Returns page recoverability
    Data_Recoverability GetRecoverability() const
    {
        return m_Recoverability;
    }
    /// op ==
    bool operator == (const Data_PageRecoveryMode& Other) const
    {
        return Other.m_AddressingMode == m_AddressingMode
               &&
               Other.m_Recoverability == m_Recoverability;
    }
    /// op !=
    bool operator != (const Data_PageRecoveryMode& Other) const
    {
        return ! this->operator == (Other);
    }
private:
    /// addressing mode
    Data_AddressingMode     m_AddressingMode;
    /// recovery mode
    Data_Recoverability     m_Recoverability;
};

/// get page recovery mode strings for given recoveryMode
inline const SAPDB_ToStringClass SAPDB_ToString( const Data_PageRecoveryMode   &recMode )
{
    if( Data_Static      == recMode.GetAddressingMode() && 
        Data_Recoverable == recMode.GetRecoverability())
    {
        return SAPDB_ToStringClass( "static" );
    }
    if( Data_Dynamic     == recMode.GetAddressingMode() &&
        Data_Recoverable == recMode.GetRecoverability())
    {
        return SAPDB_ToStringClass( "perm" );
    }
    if( Data_Dynamic        == recMode.GetAddressingMode() &&
        Data_NotRecoverable == recMode.GetRecoverability())
    {
        return SAPDB_ToStringClass( "temp" );
    }
    return SAPDB_ToStringClass( "unknown" );
}

/// output page recovery mode strings to given SAPDB_OStream
SAPDB_OStream & operator<<( SAPDB_OStream &, const Data_PageRecoveryMode & );

/*!
    @class  Data_PageId
    @brief  page no plus recoverymode
 */

class Data_PageId
{
public:
    /// default constructor does nothing
    Data_PageId()
    {}
    /// Constructor, assigns page recovery mode (pageno is undefined)
    Data_PageId
        (const Data_PageRecoveryMode&   mode)
        : 
        m_RecoveryMode(mode)
    {}
    /// Constructor, assigns page number and recovery mode
    Data_PageId
        (const Data_PageNo&             pno, 
         const Data_PageRecoveryMode&   mode)
        : 
        m_PageNo(pno), 
        m_RecoveryMode(mode)
    {}
    /// Assignment operator
    Data_PageId& operator=
        (const Data_PageId&             other)
    {
        m_PageNo       = other.m_PageNo;
        m_RecoveryMode = other.m_RecoveryMode;
        return *this;
    }
    /// Sets logical page number
    void SetPageNo
        (const Data_PageNo&             pno)
    {
        m_PageNo = pno;
    }
	/// Sets page recovery mode
    void SetPageRecoveryMode
        (const Data_PageRecoveryMode&   mode)
    {
        m_RecoveryMode = mode;
    }
    /// Returns const reference to pageno member
    const Data_PageNo& PageNo() const
    {
        return m_PageNo;
    }
	/// Returns const reference to page recovery mode member
    const Data_PageRecoveryMode& PageRecoveryMode() const
    {
        return m_RecoveryMode;
    }
    /// Invalidates the PageId
    void Invalidate()
    {
        m_PageNo.Invalidate();
    }
    /// returns true, if PageId is valid
    bool IsValid() const
    {
        return m_PageNo.IsValid();
    }
private:
    /// logical page number
    Data_PageNo            m_PageNo;
	/// page recovery mode
    Data_PageRecoveryMode  m_RecoveryMode;
};
/// page offset in bytes within one page
typedef SAPDB_UInt4 Data_PageOffset;
/// the length of a field within a page
typedef Data_PageOffset Data_RecordLength;
/// the length of a split field.
typedef SAPDB_UInt4 Data_SplitRecordLength;
/// the depth of a b*tree. Note that leaf pages are of level LEAF_LEVEL_BD00.
typedef SAPDB_UInt1 Data_Level;
/// specifies a slot within the pointer list which contains the
/// Data_PageOffset of a record. Note that the record with the 
/// smallest key has always a Data_RecordIndex of FIRST_REC_INDEX_BD00.
typedef SAPDB_UInt4 Data_RecordIndex;
/// specifies a number if records
typedef SAPDB_UInt4 Data_RecordCount;

/// This represents the contents of parameter INDEX_LEAF_CACHING.
/// For description look int o cserv.pcf.
extern SAPDB_Int4 Data_IndexLeafCaching;

#endif // DATA_TYPES_HPP
