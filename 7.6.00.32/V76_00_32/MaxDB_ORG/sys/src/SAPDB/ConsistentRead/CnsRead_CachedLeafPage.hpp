
/*!***************************************************************************

  module:       CnsRead_CachedLeafPage.hpp

  responsible : ThomasA

  special area: Consistent Read

  description:  represents a data leaf page lying in the data cache

  last changed: 2000-08-10  12:27

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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


#ifndef CNSREAD_CACHEDLEAFPAGE_HPP
#define CNSREAD_CACHEDLEAFPAGE_HPP

#include "gbd00.h"

/*!
   @brief repesents a data leaf page currently lying in the data cache. The rows contained in the
          page may contain swizzeled pointers to history chains
 */
class CnsRead_CachedLeafPage : public tbd_node
{
public :
    /*!
       @brief releases the memory of all trans history info of the page
     */
    void FreeTransHistoryInfo(tsp00_TaskId);
                              
    inline static void FreeTransHistoryInfo(tsp00_TaskId    TaskId,
                                            tbd_node&       Node);
    /*!
       @brief removes the history info of all rows inside the page and restores the original
              row headers
     */
    void RemoveTransHistroyInfo(tsp00_TaskId TaskId);
    /*!
       @brief create the transient trans and history information of the rows contained in the page. This
              method is called when a page is wrotten from disk.
     */
    void RestoreTransHistoryInfo (tsp00_TaskId      TaskId,
                                  tgg00_BasisError& TrError);
    /*!
       @brief saves the transient trans history of the rows inside the page to disk. If possible, the
              information is stored on the page itsself, otherwise a new row is used.
     */
    void SaveTransHistoryInfo (tsp00_TaskId  TaskId,
                               bool          ReleaseTransHistoryInfo,
                               tbd_node&     copyArea,
                               bool&         pageCopied);

    inline static void RestoreTransHistoryInfo (tsp00_TaskId   TaskId,
                                  tbd_node&             Node,
                                  tgg00_BasisError&     TrError);

    inline static void SaveTransHistoryInfo (tsp00_TaskId  TaskId,
                               tbd_node&            Node,
                               bool                 ReleaseTransHistoryInfo,
                               tbd_node&            copyArea,
                               bool&                pageCopied);
private :
    inline bool IsLeafPage() const;
};

//-----------------------------------------------------------------------------

inline void  CnsRead_CachedLeafPage::FreeTransHistoryInfo(tsp00_TaskId    TaskId,
                                                          tbd_node&       Node)
{
    reinterpret_cast<CnsRead_CachedLeafPage*>(&Node)->FreeTransHistoryInfo (TaskId);
}

//-----------------------------------------------------------------------------

inline void CnsRead_CachedLeafPage::RestoreTransHistoryInfo (tsp00_TaskId   TaskId,
                                                      tbd_node&             Node,
                                                      tgg00_BasisError&     TrError)
{
    reinterpret_cast<CnsRead_CachedLeafPage*>(&Node)->RestoreTransHistoryInfo (TaskId, TrError);
}

//-----------------------------------------------------------------------------

inline void CnsRead_CachedLeafPage::SaveTransHistoryInfo (tsp00_TaskId  TaskId,
                               tbd_node&            Node,
                               bool                 ReleaseTransHistoryInfo,
                               tbd_node&            copyArea,
                               bool&                pageCopied)
{
    reinterpret_cast<CnsRead_CachedLeafPage*>(&Node)->SaveTransHistoryInfo (TaskId, ReleaseTransHistoryInfo, copyArea, pageCopied);
}

#endif