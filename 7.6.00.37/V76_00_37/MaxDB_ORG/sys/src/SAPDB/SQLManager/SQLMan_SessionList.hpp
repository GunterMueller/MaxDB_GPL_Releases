/*****************************************************************************/
/*!
  @file         SQLMan_SessionList.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLMan_SessionList.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

#ifndef SQLMAN_SESSIONLIST_HPP
#define SQLMAN_SESSIONLIST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "Container/Container_List.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPool.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class Msg_List;
class RTESync_IRWRegion;
class SQLMan_Context;
class SQLMan_SessionIterator;
class SQLMan_SessionList;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @brief Multi-thread-safe list of SQL sessions
 *
 * Use
 *
 * @code
 * SQLMan_SessionList& sessionList = SQLMan_SessionListSingleton::Instance()
 * @endcode
 *
 * to access the session list if you want to add or remove sessions
 * from the list. If you need an iterator over the session list, use
 * SQLMan_SessionIterator.
 */
class SQLMan_SessionList : public SAPDB_Singleton
{
private:
    class Iterator;
    friend class Iterator;
    friend class SQLMan_SessionIterator;

    //no copy-construction or assignment
    SQLMan_SessionList(const SQLMan_SessionList&);
    SQLMan_SessionList operator=(SQLMan_SessionList&);

    static SQLMan_SessionList* m_instance;

    typedef Container_List<SQLMan_Context*> SessionList;

    SessionList          m_sessionList;
    RTESync_SpinlockPool m_spinlockPool;
    RTESync_IRWRegion*   m_pRwLock;

    SessionList::Iterator Find(SQLMan_Context& sessionContext);

    /*!
     * Iterator over the session list. This iterator will set a share
     * lock on the session list during its lifetime!
     */
    class Iterator
    {
    private:
        SAPDB_Bool            m_isLocked;
        SAPDB_Bool            m_advanceIt;
        SQLMan_SessionList*   m_pParent;
        SessionList::Iterator m_iterator;

        Iterator(const Iterator&);
        Iterator& operator=(const Iterator&);
    public:
        Iterator(SQLMan_SessionList& parent);

        ~Iterator();

        SAPDB_Bool Next(SQLMan_Context*& sessionContext);
    };

    static SQLMan_SessionList& CreateInstance();

    /*!
     * @brief Constructor.
     */
    SQLMan_SessionList();
public:
    /// Return code of Add operation
    enum AddReturnCode {
        arcOk,
        arcDuplicate,
        arcOutOfMemory
    };

    /*!
     * @brief Returns the session list-
     *
     * Automatically instantiates the session list if necessary.
     */
    static SQLMan_SessionList& Instance();

    /*!
     * @brief Destructor.
     */
    ~SQLMan_SessionList();

    /*!
     * @brief Adds a session to the session list.
     *
     * @warning This operation acquires a write lock on the session
     * list and might block!
     */
    AddReturnCode Add(SQLMan_Context& sessionContext, Msg_List& errMsg);

    /*!
     * @brief Removes a session to the session list.
     *
     * @warning This operation acquires a write lock on the session
     * list and might block!
     */
    SAPDB_Bool Remove(SQLMan_Context& sessionContext, Msg_List& errMsg);
};

/**************************************************************************/

inline SQLMan_SessionList& SQLMan_SessionList::Instance()
{
    if (m_instance == 0)
    {
        SQLMan_SessionList::CreateInstance();
    }

    return *m_instance;
}

#endif // SQLMAN_SESSIONLIST_HPP
