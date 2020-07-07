/*!***************************************************************************

  module      : SQLManErr_Interface.hpp

  -------------------------------------------------------------------------

  responsible : ElkeZ

  special area: SQLManagerErrorHandling
  description : Interface

  last changed: 2003-10-17  07:26
  see also    :
  first created:2003-10-17  07:26

  -------------------------------------------------------------------------

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

*****************************************************************************/

#ifndef SQLMANERR_INTERFACE_HPP
#define SQLMANERR_INTERFACE_HPP

#include "SAPDBCommon/SAPDB_Types.h"
#include "gsp00.h"

/*!
   @brief defines the interface of the SQLManagerErrorHandling
*/

class SQLManErr_Interface
{
  public :
    /*!
       @brief creates the error handler
     */
    virtual void Create() = 0;
    /*!
       @brief returns returncode according to basis_error and sqlmode
    */
    virtual SAPDB_Int2 GetReturnCode(SAPDB_Int2 baseError,
        tsp00_SqlMode_Param sqlmode) = 0;
    /*!
       @brief returns basis_error according to returncode and sqlmode
    */
    virtual SAPDB_Int2 GetBaseError(SAPDB_Int2 returnCode) = 0;
    /*!
       @brief returns errortext according to returncode
    */
    virtual const char* GetErrorText(SAPDB_Int2 returnCode) = 0;
    /*!
       @brief returns first errortext
    */
    virtual const char* GetFirstText(SAPDB_Int2 *returnCode) = 0;
    /*!
       @brief returns next errortext according to returncode
    */
    virtual const char* GetNextText(SAPDB_Int2 *returnCode) = 0;
    /*!
       @brief returns text for base error
    */
    virtual const char* GetBaseErrorText(SAPDB_Int2 baseError) = 0;
    /*!
      @brief returns a reference to the instance implementing the interface, see SQLManErr_Instance.cpp
     */
    static SQLManErr_Interface& GetInstance();
};

#endif
