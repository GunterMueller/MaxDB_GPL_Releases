#ifndef JOIN_ACCESSDESC_HPP
#define JOIN_ACCESSDESC_HPP
/*!
 * @file
 * @brief encapsulate type tgg00_MessBlock
 *
 * Join_AccessDesc describes access to base table. tgg00_MessBlock contains
 * which columns should delivered, which access mehod (key/index) is used etc.
 * 
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_AccessOperator.hpp
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_AutoPtr.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Join/Join_Types.hpp"
#include "ggg00.h"
#include "ggg07.h"
#include "hak70.h"
#include "hak101.h"

//! encapsulation for tgg00_MessBlock
class Join_AccessDesc 
{
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_AccessDesc(SQLMan_Context& acv);

    //! destructor
    ~Join_AccessDesc();
    //@}
    
    //! @name properties 
    //@{
    //! is key access to base table?
    SAPDB_Bool IsKeyAccess() const
    { return ( a70glob_key_strats.includes( m_strategy->str_strategy ) ); }
    
    //! is index access to base table?
    SAPDB_Bool IsIndexAccess() const
    { return ( a70glob_inv_strats.includes( m_strategy->str_strategy ) ); }
    
    //! could we fetch results or we have to build result file
    SAPDB_Bool IsKnownAccess() const
    { return a70glob_accessop_known_strats.includes( m_strategy->str_strategy ); }
    
    //! is there a join access method to base table (special strategy names)
    SAPDB_Bool IsJoinAccess() const
    { return a70glob_join_strats.includes( m_strategy->str_strategy ); }
    
    //! is there a join access method via key to base table
    SAPDB_Bool IsJoinKeyAccess() const
    { return a70glob_join_key_strats.includes( m_strategy->str_strategy ); }
    
    //! is there a join access method via index to base table
    SAPDB_Bool IsJoinInvAccess() const
    { return a70glob_join_inv_strats.includes( m_strategy->str_strategy ); }
    
    //! return file IDs of base table und used index
    tgg00_TwoFileIds& FileIDs() { return m_fileids; }
    
    //! return encapsulated message block
    tgg00_MessBlock& MBlock() { return m_mblock; }
    
    //! return SQL mode
    tsp00_SqlMode_Enum SQLMode() const { return m_mblock.mb_qual()->msqlmode(); }

    //! return whole strategy information
    const tgg07_StrategyInfo* Strategy() const { return m_strategy; }

    //! return whole strategy information
    tgg07_StrategyInfo* GetStrategyPtr() { return m_strategy; }

    //! return whole one table strategy information
    const tgg07_StrategyInfo* OneTableStrategy() const { return m_onetab_strategy; }

    //! return whole one table strategy information
    tgg07_StrategyInfo* GetOneTableStrategyPtr() { return m_onetab_strategy; }

    //! is index access to base table?
    SAPDB_Bool IsOneTableIndexAccess() const
    { return ( a70glob_inv_strats.includes( m_onetab_strategy->str_strategy ) ); }
    
    //! get record length of selected records
    SAPDB_UInt2 GetRecordLength() const { return m_strategy->str_rec_len; }
    
    //! get key length of selected records
    SAPDB_UInt2 GetKeyLength() const { return m_strategy->str_key_len; }

    //! get record length of selected records
    SAPDB_Int2& RecordLength() { return m_strategy->str_rec_len; }
    
    //! comprise base table several files?
    SAPDB_Bool IsVirtualFile() const;

    //! Returns true if hash join should be used.
    SAPDB_Bool UseHashJoin() const;

    //! Returns true if parallel index access should be used.
    SAPDB_Bool UseParallelIndexAccess() const;

    //! get start/stop keys corresponding to given access strategy
    tgg00_BasisError CreateKeys(
            Join_TwoKeys& startkey,
            Join_TwoKeys& stopkey );
    
    //! true if output columns
    bool HasOutputColumns() const 
    { return st_jump_output == (*m_mblock.mb_st())[m_mblock.mb_qual()->mqual_pos()-1].etype(); }
    //@}

    //! @name manipulation
    //@{
    //! if base table comprises several files switch to next file
    void SetNextFileID() 
    { 
        a101_GetLogicalFilename(
            m_acv.TransContext(),
            m_fileids.file_id);
        a101_SetTempFileLevel(
            m_acv,
            m_fileids.file_id,
            a101_GetTempFileLevel( m_acv, m_fileids.file_id) - 1
            );
        m_fileids.file_id.fileRoot_gg00() = NIL_PAGE_NO_GG00;
        a101_GetTempFileInstance(
            m_acv.TransContext(),
            m_fileids.file_id);
    }

    //! if base table comprises several files switch to first file
    void ResetFileID() 
    {
        m_fileids.file_id = m_mblock.mb_qual()->mtree();
        m_fileids.file_id.fileLeafNodes_gg00() = cgg_nil_leafnodes;
    }
    void PrepareLegacyAccess()
    {
        if ( 4 == m_mblock.mb_qual()->mstrat_cnt() )
            m_mblock.mb_qual()->mstrat_pos() = 
                m_mblock.mb_qual()->mstrat_pos() + cgg07_OneTabStrat_Offset;

        m_mblock.mb_qual()->mstrat_cnt() = 1;
        // so far we need no restoration of old state 
        
        if ( 1 == ( m_onetab_strategy->str_rec_len % 2 ) ) 
                // to allow pointing into buffer
                // |-- record --| <aligment space> |--record--|
                ++m_onetab_strategy->str_rec_len;
    }
    //@}
    
private:
    // member methods
    tgg00_BasisError init_mblock();

    // member variables
    SQLMan_Context&         m_acv;
    tgg00_MessBlock         m_mblock;
    tgg07_StrategyInfo*     m_strategy;
    tgg07_StrategyInfo*     m_onetab_strategy;
    tgg00_TwoFileIds        m_fileids;
    tsp00_Int2              m_segment_ptr1;
    SAPDB_AutoPtr<tgg04_subq_context> m_segment_ptr2;
};

/*************************************************************************/

inline SAPDB_Bool Join_AccessDesc::UseHashJoin() const
{
    return m_strategy->str_access_mod.includes(strmod_hash);
}

/*************************************************************************/

inline SAPDB_Bool Join_AccessDesc::UseParallelIndexAccess() const
{
    return m_strategy->str_access_mod.includes(strmod_parallel);
}

#endif
