/*!
  @file           Catalog_IndexDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          yields information about an index

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

#ifndef CATALOG_INDEXDESCRIPTOR_HPP
#define CATALOG_INDEXDESCRIPTOR_HPP

#include "SQLManager/Catalog/Catalog_ColumnDescriptor.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

struct tak_mindexrecord;
struct tak_multindex;
class  SQLMan_Context;
class  Catalog_IndexIterator;
class  Catalog_TableDescriptor;
class  Msg_List;
class  SQLMan_Identifier;

/*!
   @brief defines a descriptor which allows to get information about indexes stored in the catalog
 */
class Catalog_IndexDescriptor
{
private:
    enum AllReturnCodes
    {
        rcOk = 0,
        rcFileNotFound,
        rcSystemError
    };
public :
    class ColumnIterator 
    {
    public :
        /*!
           @brief steps to the next column
         */
        void operator++();
        /*!
          @brief true, if the iterator points to a column; otherwise false
         */
        operator bool() const;
        /*!
           @brief returns true, if 2 iterators are equal
         */
        bool operator== (const ColumnIterator& Compare) const;
        /*!
           @brief returns true, if 2 iterators are not equal
         */
        bool operator!= (const ColumnIterator& Compare) const;
        /*!
          @brief yields a column descriptor for the current column
         */
        const Catalog_ColumnDescriptor operator*() const;
    private :
        /*!
           @brief constructor
         */
        ColumnIterator (const Catalog_TableDescriptor& Table, const tak_multindex* Index, bool IsKey);
        
        const Catalog_TableDescriptor& m_table;  
        const tak_multindex*           m_index;
        int                            m_curr;
        bool                           m_isKey;
        friend class Catalog_IndexDescriptor;
    };
    /*!
       @brief returns an iterator pointing to the first column of the index
     */
    ColumnIterator Begin() const;
    /*!
       @brief returns the logical index number
     */
    int GetIndexNo() const;
    /*!
       @brief possible return codes of method GetIndexFileNo
    */
    enum GetIndexFileNoRc
    {
        gifnRcOk           = rcOk,
        gifnRcFileNotFound = rcFileNotFound,
        gifnRcSystemError  = rcSystemError
    };
    /*!
      @brief returns the index fileno in the file directory; the returned
             fileno is invalid on error
    */
    GetIndexFileNoRc GetIndexFileNo(
        FileDir_FileNo& indexFileNo,
        Msg_List&       errMsg) const;
    /*!
       @brief returns true, if the index is a function based index
     */
    bool IsFunctionBaseIndex() const;
    /*!
       @brief returns the index name.
     */
    SQLMan_Identifier GetName() const;
    /*!
       @brief returns true if the index decribes the key
     */
    bool IsKey() const;
    /*!
       @brief returns true for a unique index, false otherwise
     */
    bool IsUniqueIndex() const;
    /*!
       @brief true, if descriptor is valid; false otherwise
     */
    operator bool() const;
    /*!
       @brief possible return codes of methods Get...Count
    */
    enum GetFileCounterRc
    {
        gfcRcOk           = rcOk,
        gfcRcFileNotFound = rcFileNotFound,
        gfcRcSystemError  = rcSystemError
    };
    /*!
       @brief returns the current number of secondary keys stored in
       the index
       @return
         - gfcRcOk, on success
         - gfcRcFileNotFound, if no entry for the index could be found in
           the file directory
         - gfcRcSystemError, on system error; errMsg will be set
    */
    GetFileCounterRc GetSecondaryKeyCount(
        SAPDB_Int& secondaryKeyCount,
        Msg_List&  errMsg) const;
    /*!
       @brief returns the current number of leaf pages (including
       subtrees) in the index
       @return
         - gfcRcOk, on success
         - gfcRcFileNotFound, if no entry for the index could be found in
           the file directory
         - gfcRcSystemError, on system error; errMsg will be set
    */
    GetFileCounterRc GetLeafPageCount(
        SAPDB_Int& leafPageCount,
        Msg_List&  errMsg) const;
private :
    Catalog_IndexDescriptor (SQLMan_Context& Context, const Catalog_TableDescriptor& TableDescriptor, tak_mindexrecord* pIndex, int Idx, bool); 
    SQLMan_Context*                m_pContext; //!< pointer to the sql manager context
    tak_mindexrecord*              m_pIndex;   //!< pointer to the catalog object
    int                            m_index;    //!< index in catalog record
    const Catalog_TableDescriptor* m_table;    //!< table containing the index
    bool                           m_isKey;    //!< key is described as an index
    friend class Catalog_TableDescriptor;
    friend class Catalog_IndexIterator;
};

#endif
