/*****************************************************************************/
/*!
  @file         SQLMan_InternalTrans.hpp
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Header file for class SQLMan_InternalTrans.

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

#ifndef SQLMAN_INTERNALTRANS_HPP
#define SQLMAN_INTERNALTRANS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Msg_List;
class SQLMan_Context;

/*!
 * @class SQLMan_InternalTransInfo
 *
 * Allows to create an internal transaction and to switch between
 * the original and this internal transaction.
 *
 * This class only allows friends and subclasses to create or close
 * internal transactions and can be used as an opaque object to be
 * passed through methods that should not manipulate the transaction's
 * state.
 *
 * @warning The original and the internal transaction share the
 * same session-local file directory. Therefore you must not work
 * in parallel with the internal and original transaction!
 *
 * @sa SQLMan_InternalTrans
 */
class SQLMan_InternalTransInfo
{
private:
    friend class SQLMan_InternalTrans;

    //no copy-construction or assignment
    SQLMan_InternalTransInfo(const SQLMan_InternalTransInfo&);
    SQLMan_InternalTransInfo& operator=(SQLMan_InternalTransInfo&);

    SQLMan_Context&     m_parentContext;
    SAPDB_Bool          m_transSet;
    SAPDB_Bool          m_internalTransSet;
    tgg00_UnivTrans     m_oldTrans;
    tgg00_TransContext  m_internalTransContext;

public:
    /*!
     * @brief Possibilities to close an open internal transactions.
     */
    enum CloseType {
        doCommit,
        doRollback
    };

    /*!
     * @brief Constructor.
     *
     * Calling the constructor will <em>not</em> create an internal
     * transaction. This has to be done via the NewTrans() method.
     */
    SQLMan_InternalTransInfo(SQLMan_Context& parentContext)
        : m_parentContext( parentContext ),
          m_transSet( false ),
          m_internalTransSet( false )
    {}

    /*!
     * @brief Destructor.
     *
     * It is considered a programming error to still have an internal
     * transaction open at this time. In slowknl an assertion will be
     * triggered. In kernel the transaction will be silently rolled
     * back (but any errors occuring during the rollback might get
     * lost).
     */
    virtual ~SQLMan_InternalTransInfo();

    /*!
     * @brief Switches the context to the internal transaction if it exists.
     *
     * If the context is already set to the internal transaction
     * nothing will be done.
     */
    virtual void SwitchToInternal();

    /*!
     * @brief Switches the context to the parent transaction.
     *
     * If the context is already set to the parent transaction nothing
     * will be done.
     */
    virtual void SwitchToParent();

protected:
    /*!
     * @brief Creates a new internal transaction.
     *
     * You cannot create more than one internal transactions. You must
     * have closed the first internal transaction using the Close()
     * method before calling NewTrans() a second time. Otherwise false
     * will be returned.
     *
     * @warning The original and the internal transaction share
     * the same session-local file directory. Therefore you must
     * not work in parallel with the internal and original
     * transaction!
     *
     * @return
     *  - true, on success; the internal transaction will be set as the
     *    current transaction
     *  - false, if a system error has occured
     */
    virtual SAPDB_Bool NewTrans(Msg_List& errMsg);

    /*!
     * @brief Closes an open internal transaction. Must be called
     * before destruction!
     *
     * If there is no open internal transaction nothing will be done
     * and true returned.
     *
     * If the caller currently has the parent transaction set, Close()
     * will automatically switch to the internal transaction.
     *
     * On return the parent transaction will be set.
     *
     * @param closeType [in] determines whether the internal
     *     transaction is committed or rolled back
     * @param errMsg [out] possible error messages
     *
     * @return
     *  - true, on success
     *  - false, if a system error has occured
     */
    virtual SAPDB_Bool Close(
        const CloseType closeType,
        Msg_List&       errMsg);

    /*!
     * @brief Returns true if internal trans is set.
     *
     * @return
     *  - true, if internal trans is set
     *  - false, if parent trans is set (or if there is no internal trans)
     */
    virtual SAPDB_Bool IsInternalTrans() const;
};

/**************************************************************************/

/*!
 * @class SQLMan_InternalTrans
 *
 * Allows to create an internal transaction and to switch between
 * the original and this internal transaction.
 *
 * This class should be used for the creation of an internal transaction.
 *
 * @warning The original and the internal transaction share the same
 * session-local file directory. Therefore you must not work in
 * parallel with the internal and original transaction!
 *
 * SQLMan_AutoSwitchToInternalTrans is a helper class that allows you
 * to switch to an internal transaction during the lifetime of the
 * class object.
 *
 * @sa SQLMan_AutoSwitchToInternalTrans
 */
class SQLMan_InternalTrans : public SQLMan_InternalTransInfo
{
public:
    /*!
     * @brief Constructor.
     *
     * @copydoc SQLMan_InternalTransInfo::SQLMan_InternalTransInfo
     */
    SQLMan_InternalTrans(SQLMan_Context& parentContext)
        : SQLMan_InternalTransInfo( parentContext )
    {}

    /*!
     * @brief Returns an object that allows switching between internal
     * and parent transactions but not closing or creating new
     * internal transactions.
     *
     * This can be used to obtain an opaque object with the
     * transaction's state that can be passed through methods that
     * should not be allowed to close the transaction.
     */
    SQLMan_InternalTransInfo& GetTransInfo();

    /*!
     * @brief Creates a new internal transaction.
     *
     * @copydoc SQLMan_InternalTransInfo::NewTrans
     */
    SAPDB_Bool NewTrans(Msg_List& errMsg);

    /*!
     * @brief Closes an open internal transaction. Must be called
     * before destruction!
     *
     * @copydoc SQLMan_InternalTransInfo::Close
     */
    SAPDB_Bool Close(
        const CloseType closeType,
        Msg_List&       errMsg);

    /*!
     * @brief Switches the context to the internal transaction if it exists.
     *
     * @copydoc SQLMan_InternalTransInfo::SwitchToInternal
     */
    void SwitchToInternal();

    /*!
     * @brief Switches the context to the parent transaction.
     *
     * @copydoc SQLMan_InternalTransInfo::SwitchToParent
     */
    void SwitchToParent();

    /*!
     * @brief Commits the internal transaction and opens a new
     * internal transaction.
     *
     * If the parent transaction is the current transaction, then the
     * parent transaction will still be set on return.
     */
    SAPDB_Bool CommitInternal(Msg_List& errMsg);

    /*!
     * @brief Rolls back the internal transaction and opens a new
     * internal transaction.
     *
     * If the parent transaction is the current transaction, then the
     * parent transaction will still be set on return.
     */
    SAPDB_Bool RollbackInternal(Msg_List& errMsg);

private:
    //no copy-construction or assignment
    SQLMan_InternalTrans(const SQLMan_InternalTrans&);
    SQLMan_InternalTrans& operator=(SQLMan_InternalTrans&);

    SAPDB_Bool CommitRollbackInternal(
        const CloseType closeType,
        Msg_List&       errMsg);
};

/**************************************************************************/

/*!
 * @class SQLMan_AutoSwitchToInternalTrans
 *
 * @brief Switches to an internal transaction within the lifetime of
 * this object.
 */
class SQLMan_AutoSwitchToInternalTrans
{
private:
    SQLMan_InternalTransInfo& m_internalTransInfo;

    // no assignment or copy-construction
    SQLMan_AutoSwitchToInternalTrans(
        const SQLMan_AutoSwitchToInternalTrans&);
    SQLMan_AutoSwitchToInternalTrans& operator=(
        const SQLMan_AutoSwitchToInternalTrans&);
public:
    /*!
     * @brief Constructor. Switches to the internal transaction
     * described by internalTrans.
     */
    SQLMan_AutoSwitchToInternalTrans(
        SQLMan_InternalTrans& internalTrans);

    /*!
     * @brief Constructor. Switches to the internal transaction
     * described by internalTransInfo.
     */
    SQLMan_AutoSwitchToInternalTrans(
        SQLMan_InternalTransInfo& internalTransInfo);

    /*!
     * @brief Destructor. Switches back to the parent transaction.
     */
    ~SQLMan_AutoSwitchToInternalTrans();
};

/**************************************************************************/
/**************************************************************************/

inline SQLMan_InternalTransInfo&
SQLMan_InternalTrans::GetTransInfo()
{
    return static_cast<SQLMan_InternalTransInfo&>( *this );
}

/**************************************************************************/

inline SAPDB_Bool
SQLMan_InternalTrans::NewTrans(Msg_List& errMsg)
{
    return SQLMan_InternalTransInfo::NewTrans( errMsg );
}

/**************************************************************************/

inline SAPDB_Bool
SQLMan_InternalTrans::Close(
    const CloseType closeType,
    Msg_List&       errMsg)
{
    return SQLMan_InternalTransInfo::Close( closeType, errMsg );
}

/**************************************************************************/

inline void
SQLMan_InternalTrans::SwitchToInternal()
{
    SQLMan_InternalTransInfo::SwitchToInternal();
}

/**************************************************************************/

inline void
SQLMan_InternalTrans::SwitchToParent()
{
    SQLMan_InternalTransInfo::SwitchToParent();
}

/**************************************************************************/

inline SAPDB_Bool
SQLMan_InternalTrans::CommitInternal(Msg_List& errMsg)
{
    return this->CommitRollbackInternal( doCommit, errMsg );
}

/**************************************************************************/

inline SAPDB_Bool
SQLMan_InternalTrans::RollbackInternal(Msg_List& errMsg)
{
    return this->CommitRollbackInternal( doRollback, errMsg );
}

/**************************************************************************/
/**************************************************************************/

inline SQLMan_AutoSwitchToInternalTrans::SQLMan_AutoSwitchToInternalTrans(
    SQLMan_InternalTrans& internalTrans)
    : m_internalTransInfo( internalTrans.GetTransInfo() )
{
    m_internalTransInfo.SwitchToInternal();
}

/**************************************************************************/

inline SQLMan_AutoSwitchToInternalTrans::SQLMan_AutoSwitchToInternalTrans(
    SQLMan_InternalTransInfo& internalTransInfo)
    : m_internalTransInfo( internalTransInfo )
{
    m_internalTransInfo.SwitchToInternal();
}

/**************************************************************************/

inline SQLMan_AutoSwitchToInternalTrans::~SQLMan_AutoSwitchToInternalTrans()
{
    m_internalTransInfo.SwitchToParent();
}

/**************************************************************************/

#endif // SQLMAN_INTERNALTRANS_HPP
