/*!
  @file           RTE_PidListForExecutable.hpp
  @author         RobinW
  @brief          check if a given pid matches an executable path. 
                  This code is UNIX only. It depends on operating system utlities
                  like fuser or the /proc file system
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef RTE_PIDLISTFOREXECUTABLE_HPP
#define RTE_PIDLISTFOREXECUTABLE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! to avoid a memory allocation for every element of a pid list, memory is always allocated for a number of pids. This define gives this number */
#define NUMBER_OF_ELEMENTS_IN_PIDLIST 10

 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
/*!
  @class RTE_FuserAccess
  @brief class
 */
class RTE_FuserAccess
{
public:
    /*! state enumeration */
    enum fuserState
    {
        notChecked,
        inPath,
        inUsrSbin,
        inSbin,
        noAccess
    }
    ;

    /*!
      @brief get access
      @return enum value
     */
    static fuserState GetAccess(void);

private:
    static void CheckFuserExistence(void);

    static fuserState m_fuserState;
};

/*!
  @class RTE_PidArray
  @brief process id array that can be enchained
 */
class RTE_PidArray
{
public:
    /// ctor
    RTE_PidArray()
        : m_pNext(NULL)
    {}
    RTE_OSPid  m_elements[NUMBER_OF_ELEMENTS_IN_PIDLIST]; ///< pid array
    RTE_PidArray   *m_pNext; ///< next pid array
};

/*!
  @class RTE_PidList
  @brief process id list
 */
class RTE_PidList
{
public:
    /// ctor
    RTE_PidList()
        : m_nextElementToFill(0)
        , m_currentPidArray(&m_firstPidArray)
    {}

    /// dtor
    ~RTE_PidList()
    {
        RTE_PidArray *pNext = m_firstPidArray.m_pNext;
        while(pNext)
        {
            RTE_PidArray *pNextNext = pNext->m_pNext;
#ifdef RTEInternal
            (RTEInternal::RTEMem_RteAllocator::Instance()).Deallocate(pNext);
#else
            (RTEMem_RteAllocator::Instance()).Deallocate(pNext);
#endif
            pNext = pNextNext;
        }
    }

    /*!
      @brief insert new pid
      @param newPid [in]    guess what!
      @return none
     */
    void Insert(RTE_OSPid newPid)
    {
        if( NUMBER_OF_ELEMENTS_IN_PIDLIST == m_nextElementToFill )
        {
#ifdef RTEInternal
            RTE_PidArray *pNewArray = new(RTEInternal::RTEMem_RteAllocator::Instance())RTE_PidArray;
#else
            RTE_PidArray *pNewArray = new(RTEMem_RteAllocator::Instance())RTE_PidArray;
#endif
            m_currentPidArray->m_pNext = pNewArray;
            m_nextElementToFill = 0;
            m_currentPidArray = pNewArray;
        }
        m_currentPidArray->m_elements[m_nextElementToFill] = newPid;
        m_nextElementToFill++;
    }

    /*!
      @brief dump pid list to stdout
      @return none
     */
    void Dump(void)
    {
        SAPDB_UInt i;
        RTE_PidArray *pNext = m_firstPidArray.m_pNext;
        RTE_PidArray *currentArray = &m_firstPidArray;
        do
        {
            for(i = 0;i < (pNext ? NUMBER_OF_ELEMENTS_IN_PIDLIST : m_nextElementToFill);i++)
            {
                printf("%u\n",(unsigned int)currentArray->m_elements[i]);
            }
            if(pNext)
            {
                currentArray = pNext;
                pNext = pNext->m_pNext;
            }
            else
            {
                break;
            }
        }
        while(1);
    }

    /*!
      @brief check if pid given is in list
      @param pid [in] pid to check
      @return result
     */
    SAPDB_Bool Contains(RTE_OSPid pid)
    {
        SAPDB_UInt i;
        RTE_PidArray *pNext = m_firstPidArray.m_pNext;
        RTE_PidArray *currentArray = &m_firstPidArray;
        do
        {
            for(i = 0;i < (pNext ? NUMBER_OF_ELEMENTS_IN_PIDLIST : m_nextElementToFill);i++)
            {
                if( pid == currentArray->m_elements[i])
                {
                    return true;
                }
            }
            if(pNext)
            {
                currentArray = pNext;
                pNext = pNext->m_pNext;
            }
            else
            {
                break;
            }
        }
        while(1);
        return false;
    }
private:
    RTE_PidArray    m_firstPidArray;
    SAPDB_UInt  m_nextElementToFill;
    RTE_PidArray  *m_currentPidArray;
};

/*!
  @class    RTE_PidListForExecutable
  @brief    class that can be used to check if a given executable is currently running under a given pid. 
            It is currently available on Unices only. It depends on means of the operating system, like the
            proc file system (if available) or the fuser executable, which may possibly be unavailable or
            broken.
 */
class RTE_PidListForExecutable
{
public:
    /*!
      @brief ctor
      @param executablePath [in]    path of the executable the pid list is based on
     */
    RTE_PidListForExecutable(RTE_Path &executablePath);

    /// dtor
    RTE_PidListForExecutable()
    {
    }
    
    /*!
      @brief check if a given pid matches the executable path this list is based on
      @param pid        [in]    pid to be checked
      @param pArgument  [in]    if not NULL and operating system utitilies permit: 
                                check if the base executable and its first command line argument
                                match the given pid and pArgument
      @return true if the given pid is in the list OR technical problems occurred
     */
    SAPDB_Bool IsPidForExecutableAndArgument( RTE_OSPid pid, SAPDB_Char *pArgument );
private:
    SAPDB_Bool FillPidList(const SAPDB_Char *pExecutablePath,RTE_PidList &pidList);

    RTE_Path m_executablePath;

    SAPDB_Bool m_fuserCalled;
    RTE_PidList    m_pidList;
};


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif // RTE_PIDLISTFOREXECUTABLE_HPP
