#ifndef SQLMAN_MESSBLOCK_HPP
#define SQLMAN_MESSBLOCK_HPP

/*!
  @file           SQLMan_MessBlock.hpp
  @author         ThomasA
  @brief          encapsulates pascal type tgg00_MessBlock
 
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

#include "SQLManager/SQLMan_Types.hpp"

class SAPDBMem_IRawAllocator;
class SQLMan_Context;
class SQLMan_Strategy;

/*!
  @brief encalsulation of the pascal type tgg00_messBlock
*/

class SQLMan_MessBlock : public tgg00_MessBlock
{
  public :
     /*!
        @brief create a copy of the current mess block. Caution : the destination must provide enough memory
      */
     tgg00_BasisError Copy (SQLMan_Context& Context, SQLMan_MessBlock& Destination, bool WithoutData, SAPDBMem_IRawAllocator* pAllocator);
     /*!
        @brief creates a copy of a messblock on the heap using a given allocator
        @param source [in], the mess block to be copied
        @param allocator [in], the allocator to be used
        @param deepCopy [in] if true, the fieldlists of the source container are copied deep, otherwise the copy get a reference to the source list
        @param offset [in], offset of the  first allocated block, i.e. new mess block starts at position offset 
     */
     static void* CreateCopy(const SQLMan_MessBlock& source, SAPDBMem_IRawAllocator& allocator, int offset, bool deepCopy, SAPDB_UInt4& memoryUsage);
     
	 /*!
	   @brief deletes memory for mess block parts in given allocator
     */
	 void DestroyParts(SAPDBMem_IRawAllocator& allocator);

     /*!
        @brief returns the size of the data part
     */
     SAPDB_Int4& DataSize()  { return mb_data_size(); }

     /*!
        @brief returns a reference to the current length of the data part
     */
     SAPDB_Int4& DataLength()  { return mb_data_len(); }

     /*!
        @brief returns the current length of the data part
     */
     SAPDB_Int4  DataLength() const  { return mb_data_len(); }

     /*!
        @brief returns a pointer to the data part
     */
     tsp00_MoveObjPtr DataPtr() { return REINTERPRET_CAST(tsp00_MoveObjPtr, &mb_data()->mbp_buf()[0]); }

     /*!
        @brief returns a pointer to the work buffer part
     */
     tsp00_MoveObjPtr WorkBufPtr() { return REINTERPRET_CAST(tsp00_MoveObjPtr, mb_workbuf()); }
     
     /*!
        @brief returns a pointer to the work stack part
     */
     tsp00_MoveObjPtr WorkStackPtr() { return REINTERPRET_CAST(tsp00_MoveObjPtr, mb_work_st()); }
     
     /*!
        @brief returns a reference to the data buffer
     */
     tsp00_MoveObj& DataBuf() { return mb_data()->mbp_buf(); }

     SAPDB_Int2& FirstFree() { return mb_qual()->mfirst_free(); }

     SAPDB_Int   FirstFree() const { return mb_qual()->mfirst_free(); }

     /*!
        @brief returns the table identification 
     */
     SQLMan_Surrogate& GetTableId() const { return mb_qual()->mtree().fileTabId_gg00(); }
     /*!
       @brief invalidates the transaction info stored in the mess block.
     */
     void InvalidateTransaction();

     /*!
        @brief returns a reference to the mess block structur
      */
     SQLMan_MessStruct& MessStructure() { return mb_struct(); }
 
     /*!
        @brief returns a reference to the mess block type
      */
     SQLMan_MessType& MessType() { return mb_type(); }
 
     /*!
        @brief returns a reference to the mess block sub type
     */
     SQLMan_MessType2& MessType2() { return mb_type2(); }

     bool         GetQualBool() const { return (mb_qual()->mbool() > 0); }

     SAPDB_Int2&  QualPos() { return mb_qual()->mqual_pos(); }

     SAPDB_Int    QualPos() const { return mb_qual()->mqual_pos(); }

     SAPDB_Int    GetQualPos() const { return mb_qual()->mqual_pos(); }

     SAPDB_Int2&  QualCount() { return mb_qual()->mqual_cnt(); }

     SAPDB_Int    QualCount() const { return mb_qual()->mqual_cnt(); }

     SAPDB_Int    GetQualCount() const { return mb_qual()->mqual_cnt(); }

     SAPDB_Int2&  ResQualPos() { return mb_qual()->mresqual_pos(); }

     SAPDB_Int    ResQualPos() const { return mb_qual()->mresqual_pos(); }

     SAPDB_Int2&  ResQualCount() { return mb_qual()->mresqual_cnt(); }

     SAPDB_Int    ResQualCount() const { return mb_qual()->mresqual_cnt(); }

     SAPDB_Int4&  QualLength()  { return mb_qual_len(); }

     SAPDB_Int4   QualLength() const { return mb_qual_len(); }

     /*!
        @brief returns true if qualification contains output description
     */
     bool HasOutputCols() const
     { 
         return ( 0 != this->GetQualCount() && 0 != this->GetQualCount() &&
                 ( st_jump_output == this->StackEntry( this->GetQualPos()-1 ).etype() ) ); 
     }
     
     /*!
        @brief returns amount of stack entries comprises output description
     */
     SAPDB_Int4 GetOutputDescCount() const 
     {
         SAPDBERR_ASSERT_STATE( this->HasOutputCols() );
         return this->StackEntry( this->GetQualPos()-1 ).epos() - 1; 
     }
     
     /*!
        @brief returns true if result qualification (more phase handling) contains output description
     */
     bool HasResultOutputCols() const
     { 
         return ( 0 != this->ResQualCount() && 0 != this->ResQualCount() &&
                 ( st_jump_output == this->StackEntry( this->ResQualPos()-1 ).etype() ) ); 
     }

     /*!
        @brief returns amount of stack entries comprises result output description
     */
     SAPDB_Int4 GetResOutputDescCount() const
     {
         SAPDBERR_ASSERT_STATE( this->HasResultOutputCols() );
         return this->StackEntry( this->ResQualPos()-1 ).epos() - 1; 
     }
     
     /*!
        @brief returns a pointer to the qual part
     */
     tsp00_MoveObjPtr QualPtr() { return REINTERPRET_CAST(tsp00_MoveObjPtr, mb_qual()); }

     SAPDB_Int4&  QualSize()  { return mb_qual_size(); }

     void SetData(void* p = NULL, SAPDB_Int4 size = 0)
     {
         mb_data() = REINTERPRET_CAST(tgg00_DataPartPtr, p);
         mb_data_size() = size;
     }

     void SetQual(void* p = NULL, SAPDB_Int4 size = 0)
     {
         mb_qual() = REINTERPRET_CAST(tgg00_QualBuf*, p);
         mb_qual_size() = size;
     }

     void SetStack(void* p = NULL, SAPDB_Int4 size = 0) { 
         mb_st() = REINTERPRET_CAST(tgg00_StackListPtr, p);
         if ((mb_qual()) && (mb_qual_size() > sizeof(tgg00_FileId) + 64 + sizeof(void*)))
         {
             mb_qual()->mst_addr() = mb_st();
         }
         mb_st_size() = size;
         mb_st_max()  = size / sizeof(SQLMan_StackEntry);
     }

      void SetStrat(void* p = NULL, SAPDB_Int4 size = 0)
     {
         mb_strat() = REINTERPRET_CAST(tsp00_MoveObjPtr, p);
         mb_strat_size() = size;
     }

     void SetWorkArea(void* wbuf   = 0, SAPDB_Int4 wbuf_size   = 0, 
                      void* wstack = 0, SAPDB_Int2 wstack_size = 0)
     {
         mb_work_st()      = REINTERPRET_CAST(tgg00_StackListPtr, wstack);
         mb_work_st_max()  = wstack_size;
         mb_workbuf()      = REINTERPRET_CAST(tsp00_MoveObjPtr, wbuf);
         mb_workbuf_size() = wbuf_size;
     }

     SQLMan_SqlMode& SqlMode() { return mb_qual()->msqlmode(); }

     SAPDB_Int4& StackSize()  { return mb_st_size(); }

     SAPDB_Int4  StackSize() const { return mb_st_size(); }

     SAPDB_Int4& StratLength()  { return mb_strat_len(); }

     SAPDB_Int4  StratLength() const { return mb_strat_len(); }

     SAPDB_Int4& StratSize()  { return mb_strat_size(); }

     SAPDB_Int4  StratSize() const  { return mb_strat_size(); }

     SAPDB_Int2& StratPos() { return mb_qual()->mstrat_pos(); }

     SAPDB_Int   StratPos() const { return mb_qual()->mstrat_pos(); }

     SAPDB_Int2& StratCnt() { return mb_qual()->mstrat_cnt(); }

     SAPDB_Int   StratCnt() const { return mb_qual()->mstrat_cnt(); }

     /*!
        @brief returns a pointer to the strat part
     */
     tsp00_MoveObjPtr StratPtr() { return REINTERPRET_CAST(tsp00_MoveObjPtr, mb_strat()); }

     /*!
        @brief returns a pointer to the actual strategy
     */
     inline SQLMan_Strategy* GetStrategyPtr();

     SAPDB_Int2& MaxWorkStack()  { return mb_work_st_max(); }
     
     SAPDB_Int   MaxWorkStack() const { return mb_work_st_max(); }
     
     SAPDB_Int4& WorkBufSize()  { return mb_workbuf_size(); }

     SAPDB_Int4  WorkBufSize() const { return mb_workbuf_size(); }

     SAPDB_Int2& StringCount() { return mb_qual()->mstring_cnt(); }

     SAPDB_Int2& StringPos() { return mb_qual()->mstring_pos(); }

     SQLMan_StackEntry& StackEntry(int idx) { return (*mb_st())[idx]; }

     const SQLMan_StackEntry& StackEntry(int idx) const { return (*mb_st())[idx]; }

     SQLMan_StackEntry* StackEntryPtr(int idx) { return &((*mb_st())[idx]); }

     SQLMan_DateTimeFormat& DateTimeFormat() { return mb_qual()->mdatetimeformat(); }

};

inline SQLMan_Strategy* SQLMan_MessBlock::GetStrategyPtr()
{
    if  ( 0 == this->StratCnt() || 0 == this->StratPos() )
        return 0;
    return  REINTERPRET_CAST( SQLMan_Strategy*, 
                this->StratPtr() + this->StackEntry( this->StratPos()-1 ).epos() - 1 );
}

#endif

