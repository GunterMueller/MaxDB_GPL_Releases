/*!
  @file           RTETst_IParser.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Runtime Test Parser interface

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

#ifndef RTETST_IPARSER_HPP
#define RTETST_IPARSER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class SAPDBErr_MessageList;
class RTETask_ITask;

/*!
  @class RTETst_IContext
  @brief context class
 */
class RTETst_IContext
{
public:
    /*! @brief public ctor */
    RTETst_IContext()
    {}
};

/*!
  @brief test execution call
  @param pTask [in] the calling task context
  @param argumentString [in] the complete test command string (not zero terminated!!!)
  @param argumentLength [in] the length of the given test command string
  @param pTestContext [in] the test context given during registration
  @param errCode [out] error code returned into transaction context
  @param errList [out] error list filled if false is returned
  @return true if test succeeded, false if test failed (errList filled) 
 */
typedef SAPDB_Bool (*RTETst_ExecutorCall)( RTETask_ITask       *pTask
                                         , SAPDB_UTF8 const    *argumentString
                                         , SAPDB_Int4           argumentLength
                                         , RTETst_IContext     *pTestContext
                                         , SAPDB_Int4 &         errCode
                                         , SAPDBErr_MessageList &errList );
/*!
  @class RTETst_IParser
  @brief RTE Test parser interface

  Each RTE test must be registered in the RTE Test parser. It will be called
  by the SQL command 'diagnose chech instance 4 'argumentString'
 */
class RTETst_IParser
{
public:
    /*!
      @brief execute a test for given task
      @param pSelf [ in ] the task interface
      @param argumentString [ in ] the test argument string including test command
      @param argumentLength [ in ] the test argument length
      @param errCode [out] error code returned into transaction context
      @param errList [out] error list filled if false is returned
      @return true if test succeeded, false if test failed
     */
    static SAPDB_Bool Execute( RTETask_ITask *pSelf
                             , SAPDB_UTF8 const *argumentString
                             , SAPDB_Int4 argumentLength
                             , SAPDB_Int4 &errCode
                             , SAPDBErr_MessageList &errList );
    /*!
      @brief register a test
      @param testCommand [in] unique test command
      @param pTestContext [in] test context pointer passed to each such test call
      @param pExecutor [in] the test command to execute
      @param errList [out] error list filled if false is returned
      @return true if registration passed, false if failed(i.e. not unique...)
     */
    static SAPDB_Bool Register( SAPDB_UTF8 const *testCommand
                              , RTETst_IContext *pTestContext
                              , RTETst_ExecutorCall pExecutor
                              , SAPDBErr_MessageList &errList );
};

#endif /* RTETST_IPARSER_HPP */
