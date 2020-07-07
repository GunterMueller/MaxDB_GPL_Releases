/*!
  @file           livecachesqldbc.h
  @author         MaxDB INTERFACES
  @brief          Provides a 'connection' for SQLDBC calls.

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

#ifndef SQL_DBC_HPP
#define SQL_DBC_HPP

#include "SQLDBC/SQLDBC.h" /* no check */

class SQL_SQLDBCRuntime;

class SQL_ConnectionImplementation; 
class SQL_SQLDBCErrorHndl;          

using namespace SQLDBC; 

/*!
   @brief Provides a SQL_SQLDBCConnection inside a C++ stored procedure embedded in the 
          MaxDB/liveCache library liboms. Standard applications and interfaces should always 
          include SQLDBC.h.
          Within a db procedure this connection can then be used to access the methods of the 
          SQLDBC interface.   
          
          Because SQL_SQLDBCConnection and SQLDBC never throws exceptions, it is neccessary
          to check the errocode after creation of an SQL_SQLDBCConnection object using 
          getErrorCode() or the bool operator () of the SQL_SQLDBCErrorHndl object.

          The constructor of the SQL_SQLDBCConnection will ever succeed but in case of an error 
          during initialization (eg. out of memory error) the error is stored in the 
          SQL_SQLDBCErrorHndl object returned by the error() method. 

          For using SQLDBC in a stored procedure use the example code below:

          <pre>
          #include <SQL_SQLDBCConnection.h>

          ...

          SQL_SQLDBCConnection con;
          if (con.error()) {
            SQL_SQLDBCErrorHndl err = con.error();
            throw DbpError (err.getErrorCode(), err.getErrorText());
          }
          SQLDBC_Statement *stmt = con.createStatement();

          ...
           </pre>
 */
class AFX_EXT_CLASS SQL_SQLDBCConnection
{
public :
    /*!
       @brief constructor, establishes the connection.
       
       @param packetSize defines the size of the packet for the communication with the kernel
     */
     SQL_SQLDBCConnection(int packetSize = 4 * 4096);
    /*!
       @brief destructor, closes the connection
     */
     virtual ~SQL_SQLDBCConnection();

    /*!
     * @brief Creates an SQLDBC_Statement object for sending SQL statements
     * to the database.
     
     * SQL statements without parameters are normally executed using 
     * SQLDBC_Statement objects.
     * Applications executing SQL statements several times or 
     * binding parameters for input and/or output must
     * use SQLDBC_PreparedStatement objects.
     * @return The SQLDBC_Statement object created by this method.
     * @see SQLDBC_Statement
     */
    SQLDBC::SQLDBC_Statement *createStatement();
  
     /*!
      * @brief Creates an SQLDBC_PreparedStatement object for sending SQL statements
      * to the database.
      
      * An SQLDBC_PreparedStatement object is 'prepared' in the database server and can use binding variables for input/output parameters. 
      * @return The SQLDBC_PreparedStatement object created by this method.
      * @see SQLDBC_PreparedStatement
      */
     SQLDBC::SQLDBC_PreparedStatement *createPreparedStatement();
    

     /*!
      * @brief Releases an SQLDBC_Statement.
      * @param stmt A reference to an SQLDBC_Statement object to be released. 
      */
     void releaseStatement(SQLDBC::SQLDBC_Statement *stmt);
  
     /*!
      * @brief Releases an SQLDBC_PreparedStatement.
      * @param stmt A reference to an SQLDBC_PreparedStatement object to be released. 
      */
     void releaseStatement(SQLDBC::SQLDBC_PreparedStatement *stmt);

     /*!
      * @brief Returns the kernel version.
      *
      * It will be returned in the following 
      * format:  
      *   \<major_release\>\<minor_releaser\>\<correction_level\>
      *   - mayor_release (1 digit)
      *   - minor_release (2 digit)
      *   - correction_level (2 digits)
      * @return The kernel version, which is a computed \<major_release\> * 10000 +
      * \<minor_release\> * 100 + \<correction_level\>. 
      *
      * <b>Example:</b> For version 7.6.00 version number 70600 is returned.
      */
     SQLDBC_Int4 getKernelVersion();

     /*!
      * @brief Returns whether the database is an unicode database or
      * not.
      * @return #SQLDBC_TRUE if the database is an unicode database.
      */
     SQLDBC_Bool isUnicodeDatabase();

    /**
     * @brief Returns a reference to the SQL_SQLDBCErrorHndl object.
     *
     * @note Applications should retrieve the content of the SQL_SQLDBCErrorHndl
     * object immediatly since an new call to any function will empty 
     * SQL_SQLDBCErrorHndl object.
     *
     * @return An SQL_SQLDBCErrorHndl object.
     */
    SQL_SQLDBCErrorHndl &error();

    /**
     * Get the statistic information about the actions done in this
     * connection.
     * @return An @c SQLDBC_Profile instance that can be queried for statistic information.
     */
    SQLDBC_Profile *getProfile();

private :
    SQLDBC::SQLDBC_Connection*    m_connection;     //!< SQLDBC connection
    SQL_ConnectionImplementation* m_implementation; //!< pointer to the class instance implementing SQL_SQLDBCConnection  
};

class SQL_ErrorHndlImplementation;  

/*!
   @brief Provides a error object used during initialisation of the SQL_SQLDBCConnection.
   
   Since SQL_SQLDBCConnection and SQLDBC never throws exceptions, check the errocode using
   getErrorCode() or the bool operator (), after creation of an SQL_SQLDBCConnection object.
   The construction of the SQL_SQLDBCConnection will succeed ever but in case of an error 
   during initialization of the SQL_SQLDBCConnection (eg. out of memory error) the error 
   is stored in the SQL_SQLDBCErrorHndl object returned by the error() methode.   
   
 */
class AFX_EXT_CLASS SQL_SQLDBCErrorHndl
{
public :
  /**
   * @brief Returns the error code from the ErrorHndl object.
   */
  const SQLDBC_Int4 getErrorCode() const;
  
  /**
   * @brief Returns the error message stored in the ErrorHndl object.
   * @note The returned string may contain UTF8 characters.
   */
  const char *getErrorText() const;

  /**
   * @brief Returns the SQL state from the ErrorHndl object.
   */
  const char* getSQLState() const;

  /**
   * @brief A Conversion operator.
   *
   * Returns true if the ErrorHndl object contains an error.
   * This operator makes it possible to write constructs as follows:
   *
   * <code> SQL_SQLDBCConnection con;<br>
   * if (con.error()) {<br>
   *   SQL_SQLDBCErrorHndl err = con.error();<br>
   *   throw DbpError (err.getErrorCode(), err.getErrorText());<br>
   * }<br>
   * </code>
   */
  operator SQLDBC_Bool () const;
  /**
   * @brief The constructor for an SQLDBC_ErrorHndl object. Internaly used
   *
   */
  SQL_SQLDBCErrorHndl(SQL_ErrorHndlImplementation* error) : m_error(error) {};


private :
  friend class SQL_ErrorHndlImplementation;
  SQL_ErrorHndlImplementation* m_error;
};

#endif
