/*****************************************************************************/
/*!
  @file         Data_KeyBoundedPageAccess.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class Data_KeyBoundedPageAccess.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#ifndef DATA_KEYBOUNDEDPAGEACCESS_HPP
#define DATA_KEYBOUNDEDPAGEACCESS_HPP

/*========================================================================*
 *  INCLUDES                                                              *
 *========================================================================*/

#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_PrimPage.hpp"
#include "DataAccess/Data_ITreeAccess.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "ggg00.h"
#include "gbd00.h"
#include "gsp00.h"

/*========================================================================*
 *  DEFINES                                                               *
 *========================================================================*/

/*========================================================================*
 *  MACROS                                                                *
 *========================================================================*/

/*========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                *
 *========================================================================*/

/*!
 * @brief Iterates over a region of pages of a file that is bounded by
 * a start and stop key
 *
 * @author MartinKi
 */
class Data_KeyBoundedPageAccess : public Data_ITreeAccess
{
public:
    /*!
     * @brief Constructor.
     *
     * @param current [in/out] current describing the tree
     * @param startKey    [in] value of start key
     * @param startKeyLen [in] length of start key
     * @param stopKey     [in] value of stop key
     * @param stopKeyLen  [in] length of stop key
     * @param lruChainInfo [in] cache strategy for pages that are released
     */
    Data_KeyBoundedPageAccess(
        tbd_current_tree& current,
        tsp00_Key&        startKey,
        const SAPDB_Int2  startKeyLen,
        tsp00_Key&        stopKey,
        const SAPDB_Int2  stopKeyLen,
        Data_LRUChainInfo lruChainInfo = Data_LruNormal);

    /*!
     * @brief Destructor.
     *
     * Releases the tree.
     */
    ~Data_KeyBoundedPageAccess();

    /*!
     * @copydoc Data_ITreeAccess::GetNext
     */
    Data_PrimPage& GetNext();

    /*!
     * @copydoc Data_ITreeAccess::GetNumberOfEstimatedLeafNodes
     */
    Data_PageCount GetNumberOfEstimatedLeafNodes() const;

    /*!
     * @copydoc Data_ITreeAccess::GetFileNo
     */
    FileDir_FileNo GetFileNo() const;

private:
    // no copy construction
    Data_KeyBoundedPageAccess(const Data_KeyBoundedPageAccess&);
    Data_KeyBoundedPageAccess& operator=(const Data_KeyBoundedPageAccess&);

    Data_PrimPage& SetToStartPage();

    SAPDB_Bool IsPageInScope();

    SAPDB_Bool GetNextLeafNode();

    Data_LRUChainInfo m_lruChainInfo;

    void ReleasePage(Data_PrimPage& page, Msg_List& errlist);
protected:
    tbd_current_tree&      m_Current;
    Data_PageAccessManager m_pageAccessManager;
    Data_PrimPage          m_page;
    tsp00_Key              m_startKey;
    SAPDB_Int2             m_startKeyLen;
    tsp00_Key              m_stopKey;
    SAPDB_Int2             m_stopKeyLen;
};

/**************************************************************************/

inline Data_PageCount
Data_KeyBoundedPageAccess::GetNumberOfEstimatedLeafNodes() const
{
    return 0;
}

#endif // DATA_KEYBOUNDEDPAGEACCESS_HPP
