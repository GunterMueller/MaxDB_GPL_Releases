/*!
  @file           SQLManErr_Instance.hpp
  @author         ElkeZ
  @brief          Implements the SQLManagerErrorHandling interface

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

#ifndef SQLMANERR_INSTANCE_HPP
#define SQLMANERR_INSTANCE_HPP

#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_BaseToReturnCode.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "RunTime/MemoryManagement/RTEMem_RawAllocator.hpp"
#include "Container/Container_SortableVector.hpp"
#include "gsp00.h"
#include "ggg00.h"

class SQLManErr_Instance;
/*!
   @brief creates the SQLManagerErrorHandling instance. 
          This is a singleton providing the SQLManagerErrorHandling interface.
*/
class SQLManErr_Instance : public SQLManErr_Interface, public SAPDB_SingletonTemplate<SQLManErr_Instance>
{
private :
    struct SQLManErrorObject
    {
        SAPDB_Int2          baseError; 
        SAPDB_Bool1         sqlmodeDependent;
        tsp00_Byte          sqlmode; // tsp00_sqlmode not possible because of NT-compiler, JörgM tried hard,
                                     // but did not find any solution
        SAPDB_Int2          returnCode;  
        SAPDB_Char const   *errorText;
        SAPDB_Char const   *baseErrorText;
    };

    static SQLManErrorObject errors [];

    //create container of base-error <--> returncode-combinations
    Container_SortableVector<SQLManErr_BaseToReturnCode> baseReturns;


    int BaseToReturnBinarySearch(SAPDB_Int2 baseError,
        tsp00_SqlMode_Param sqlmode);

	int ErrorObjectBinarySearch(
        int        searchedReturnCode);


public :
    /* needed for Singleton */
    SQLManErr_Instance();

    /*!
       @brief creates the error handler
     */
    virtual void Create();
    /*!
       @brief returns returncode according to basis_error and sqlmode
    */
    virtual SAPDB_Int2 GetReturnCode(SAPDB_Int2 baseError,
        tsp00_SqlMode_Param sqlmode);
    /*!
       @brief returns basis_error according to returncode and sqlmode
    */
    virtual SAPDB_Int2 GetBaseError(SAPDB_Int2 returnCode);
    /*!
       @brief returns errortext according to returncode
    */
    virtual const char* GetErrorText(SAPDB_Int2 returnCode);
    /*!
       @brief returns first errortext
    */
    virtual const char* GetFirstText(SAPDB_Int2 *returnCode);
    /*!
       @brief returns next errortext according to returncode
    */
    virtual const char* GetNextText(SAPDB_Int2 *returnCode);
    /*!
       @brief returns text for base error
    */
    virtual const char* GetBaseErrorText(SAPDB_Int2 baseError);

    friend class SAPDB_SingletonTemplate<SQLManErr_Instance>;

};

#endif
