/*!
  @file           RTETst_Parser.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Runtime Test Parser implementation

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

#ifndef RTETST_PARSER_HPP
#define RTETST_PARSER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Test/RTETst_IParser.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"

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
class RTE_ITask;
class RTETst_IContext;

/*!
  @brief test registration structure
  Each registered tests is stored in one of these allocated using RTEMem_RteAllocator
 */
struct RTETst_CallEntry
{
    RTETst_CallEntry *pNext;          ///< next in list
    SAPDB_UTF8 *testCommand;           ///< copy of unique testCommand 
    RTETst_IContext *pContext;        ///< context during test call
    RTETst_ExecutorCall Executor;     ///< the test code
};

/*!
  @class RTETst_Parser
  @brief RTE Test parser implementation

  Each RTE test must be registered in the RTE Test parser using the RTE_ITestParser
  interface. It will be called by the SQL command 'diagnose chech instance 4 'argumentString'

  Since no deregistration happens, no spinlock is needed for registration.
  The registration uniqueness check first automically copies the head pointer

  During registration the found list is 
 */
class RTETst_Parser : public SAPDB_Singleton
{
public:
    /*!
      @brief Test parser singleton access
     */
    static RTETst_Parser & Instance();

private:
    /*!
      @brief ctor
      @return none
     */
    RTETst_Parser()
        : m_FirstTestEntry(0)
        , m_DefaultTestEntry(0)
    {}

    static RTETst_Parser *m_Instance; ///< the test parser implementation
    RTETst_CallEntry *m_FirstTestEntry;   ///< head of registered tests list
    RTETst_CallEntry *m_DefaultTestEntry; ///< entry for test without argument string

    friend class RTETst_IParser;
};

#endif /* RTETST_PARSER_HPP */
