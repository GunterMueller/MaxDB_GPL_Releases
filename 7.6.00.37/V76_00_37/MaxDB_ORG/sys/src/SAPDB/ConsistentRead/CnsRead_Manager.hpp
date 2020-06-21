/*!***************************************************************************

  module:       CnsRead_Manager.hpp

  responsible : ThomasA

  special area: Consistent Read

  description:  Interface to consistent read globals 

  last changed: 2000-08-10  12:27
  see also    :

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

#ifndef CNSREAD_MANAGER_HPP
#define CNSREAD_MANAGER_HPP

#include "ggg00.h"

class CnsRead_ManagerImplementation;
class Msg_List;

/*!
   @brief provides functions for consistent read support.
 */
class CnsRead_Manager
{
public :
    /*!
       @brief creates an index key
     */
    static void BuildInvKey (const tgg00_Lkey& SourceKey, tgg00_Lkey& Key, const tgg91_TransNo&  UpdateTrans);
    /*!
       @brief returns true, if consistent read is enabled
     */
    inline static bool IsConsistentEnabled();
    /*!
       @brief returns true, if consistent read is configurated
     */
    inline static bool IsConsistentRead_Configurated(); 
    /*!
       @brief enables consistent read for sql data
     */
    static void EnableConsistentRead(tsp00_TaskId TaskId);
    /*!
       @brief returns the number of history writes into the paging file
     */
    static SAPDB_UInt8 GetHistorySavedInFileCount();
    /*!
       @brief returns the number of history writes which could be stored the page itsself
     */
    static SAPDB_UInt8 GetHistorySavedInPageCount();
    /*!
       @brief increments the counter for history writes which could be stored the page itsself
     */
    static void IncHistorySavedInPageCount();
    
    static tgg91_TransNo GetMinTrans();
    /*!
       @brief checks if a given key from an index is consistent for the current trans
     */
    static int IsIndexKeyConsistent (tgg00_TransContext& Trans, SAPDB_Byte* pKey, int KeyLength);
    /*!
       @brief reads the trans and history information of a page from the paging file
     */
    static bool ReadTransHistoryInfo (tsp00_TaskId TaskId, tgg00_Rec& Rec, Msg_List& MsgList);
    static void SetMinTrans (const tgg91_TransNo& MinTrans);
     /*!
       @brief writes the trans and history information of a page into the paging file
     */
    static bool WriteTransHistoryInfo(tsp00_TaskId TaskId, const tgg00_Rec& Rec, Msg_List& MsgList);
private :
    static bool                           m_consistentReadEnabled; //!< true, if consistent read is enabled
    static CnsRead_ManagerImplementation* m_impl;                  //!< pointer to the managers implementation
};

//-----------------------------------------------------------------------------

inline bool CnsRead_Manager::IsConsistentEnabled()
{
    return m_consistentReadEnabled;
}

//-----------------------------------------------------------------------------

inline bool CnsRead_Manager::IsConsistentRead_Configurated()
{
    return false;
}

#endif