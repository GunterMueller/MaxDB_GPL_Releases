/*!
  @file           KSQL_Connection.hpp
  @author         ThomasA
  @brief          Implements class for a virtual database connection.

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

#ifndef KSQL_CONNECTION_HPP
#define KSQL_CONNECTION_HPP

#include "SQLManager/KernelSQL/KSQL_SQLDBC.hpp"
#include "SQLManager/KernelSQL/KSQL_Statement.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"


using namespace SQLDBC;

/*!
 * @brief A virtual connection from a session to the database kernel.
 * 
 * an instance of this class is the anchor for sql statements of the kernel running in the kernel.
 * the connect instance is 'connected' automatically.
 */

class KSQL_Connection
{
   public :
      typedef SQLDBC_ErrorHndl KSQL_Error;     //!< defines the error class 
      /*!
        @brief constructor, creates a connected connection
      */
      KSQL_Connection();
      /*!
        @brief destructor
      */
      ~KSQL_Connection();
      /*!
        @brief provides a sql statement ready for parse/execute 
       */
      KSQL_PreparedStatement createPreparedStatement();
      /*!
        @brief provides a sql statement
       */
      KSQL_Statement createStatement();
      /*!
        @brief returns a description of the last error
       */
      const KSQL_Error& getError() const;
      /*!
        @brief returns true, if the connection is etablished
       */
      bool isConnected() const;
      /*!
        @brief returns true, if no error occured
       */
      bool isOk() const;
   private :
     bool Connect(); //!< establishes the connection
     int                        m_dateTimeFormat;
     SQLDBC_Connection*         m_connection;
     SQLMan_Context&            m_context;
     SQLDBC::SQLDBC_Environment m_environment;
};

#endif
