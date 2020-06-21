/*!
  @file           SQLDBC.h
  @author         DL SAP DB INTERFACES
  @ingroup        SQLDBC
  @brief          General header file for the SAP Database Connectivity.

  SQL Database Connectivity (SQLDBC) is a runtime library that enables
  applications to execute SQL statements in the database, and to access
  and modify data.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2004 SAP AG

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
#ifndef SQLDBC_H
#define SQLDBC_H
#include "SQLDBC_Types.h" /* no check */

class IFR_Environment;
class IFR_Connection;
class IFR_ConnectProperties;
class IFR_ConnectionItem;
class IFR_ErrorHndl;
class IFR_Statement;
class IFR_PreparedStmt;
class IFR_SQLWarning;
class IFR_ResultSet;
class IFR_RowSet;
class IFR_ParameterMetaData;
class IFR_ResultSetMetaData;
class IFR_LOB;

class SAPDBMem_IRawAllocator;

/**
 * @brief The version string for the SQLDBC.H file.
 */
static const char* SQLDBCHeaderVersion = "SQLDBC.H  7.5.0    BUILD 016-113-076-904";
/*
   type maps used by the script genSQLDBC_C.pl for gerating a SQLDBC C-wrapper.
   #typemap  SQLDBC_StringEncodingType_Encoding SQLDBC_StringEncoding
   #typemap  ParameterMode SQLDBC_ParameterMetaData::ParameterMode
   #typemap  ParameterNullBehavior SQLDBC_ParameterMetaData::ParameterNullBehavior
   #typemap  ColumnNullBehavior SQLDBC_ResultSetMetaData::ColumnNullBehavior
   #typemap  SQLDBC_Statement_ResultSetType SQLDBC_Statement::ResultSetType
   #typemap  SQLDBC_Statement_ConcurrencyType SQLDBC_Statement::ConcurrencyType
*/

class SQLDBC_IRuntime;

#ifdef WIN32
#define SQLDBC_DLLEXPORT __declspec(dllexport)
#else
#define SQLDBC_DLLEXPORT
#endif

namespace SQLDBC {

class SQLDBC_Statement;

/**
 * Store for error messages and error codes.
 */
class SQLDBC_ErrorHndl
{
public:
  /**
   * @brief The constructor for an SQLDBC_ErrorHndl object.
   *
   * After construction the ErrorHndl object does not contain errors.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_ErrorHndl();

  /**
   * @brief Returns the error code from the ErrorHndl object.
   */
  SQLDBC_DLLEXPORT
  const SQLDBC_Int4 getErrorCode() const;

  /**
   * @brief Returns the SQL state from the ErrorHndl object.
   */
  SQLDBC_DLLEXPORT
  const char* getSQLState() const;

  /**
   * @brief Returns the error message stored in the ErrorHndl object.
   * @note This is an UTF8 string.
   */
  SQLDBC_DLLEXPORT
  const char *getErrorText() const;

  /**
   * @brief A Conversion operator.
   *
   * Returns true if the ErrorHndl object contains an error.
   * This operator makes it possible to write constructs as follows:
   * <code>if \(error\) { ... }</code>.
   */
  SQLDBC_DLLEXPORT
  operator SQLDBC_Bool () const;

private:
  friend class SQLDBC_ConnectionItem;
  friend struct SQLDBC_ConnectionItemStorage;
  friend class SQLDBC_Connection;

  /**
   * @brief Constructor for a ErrorHndl object.
   *
   * Constructs an error object using the internally used error object.
   */
  SQLDBC_ErrorHndl(IFR_ErrorHndl* error);

  /**
   * @brief Sets this error to the 'memory allocation failed'
   *        error.
   */
  void setMemoryAllocationFailed();


  /**
   * @brief A reference to the internally used error object.
   */
  IFR_ErrorHndl *m_error;
};

/**
 * @brief A class for handling SQL warnings.
 *
 * An SQL warning is a state after an execution of an SQL statement or a
 * function call that is not critical but, as it may cause problems
 * or be an user error. Such an SQL warning may originate from the database,
 * or from the SQLDBC interface.
 **/
class SQLDBC_SQLWarning
{
public:
  /**
   * @brief Returns the next SQLWarning from the chain.
   * @return A pointer to the next SQLWarning, or 0 if no SQLWarning exists.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_SQLWarning* getNextWarning() const;

  /**
   * @brief Returns the warning code of the SQL warning.
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_SQLWarningCode getWarningCode() const;

private:
  friend class SQLDBC_ConnectionItem;
  friend struct SQLDBC_ConnectionItemStorage;
  /**
   * @brief Constructor for a SQLWarning object.
   *
   * After construction the SQLWarning object contains no warning.
   **/
  SQLDBC_SQLWarning(IFR_SQLWarning* warning);
  IFR_SQLWarning    *m_warning;
};

struct SQLDBC_ConnectionItemStorage;

/**
 * @brief Base class to administrate the error handling within SQLDBC.
 *
 * A ConnectionItem is an object that is referenced by connection,
 * and it is directly bound to it.
 * A Statement, PreparedStatement or a ResultSet object references
 * this object..
 * Any SQLDBC_ConnectionItem has an ErrorHndl and
 * a Warning member. They are set after an, unsuccessful operation.
 */
class SQLDBC_ConnectionItem
{
public:

    /**
     * @brief Returns a reference to the ErrorHndl object.
     *
     * @note Applications should retrieve the content of the SQLDBC_ErrorHndl
     * object immediatly since an new call to any SQLDBC function except the
     * warning() method will empty SQLDBC_ErrorHndl object.
     *
     * @return An SQLDBC_ErrorHndl object.
     */
    SQLDBC_DLLEXPORT
    SQLDBC_ErrorHndl &error();

    /**
     * @brief Deletes the error has been stored.
     */
    SQLDBC_DLLEXPORT
    void clearError();

    /**
     * @brief Returns a reference to an SQLWarning object stored in
     * the SQLDBC_ConnectionItem object.
     * @note Getting the reference to the SQLWarning object will not
     * clear the ErrorHndl object. All other function calls will empty
     * the ErrorHndl object.
     * @return The SQLWarning object stored in the item.
     */
    SQLDBC_DLLEXPORT
    SQLDBC_SQLWarning* warning();

    /**
     * @brief Deletes the warning stored in the SQLWarning object.
     **/
    SQLDBC_DLLEXPORT
    void clearWarnings();

private:
    friend class SQLDBC_Environment;
    friend class SQLDBC_Connection;
    friend class SQLDBC_Statement;
    friend class SQLDBC_PreparedStatement;
    friend class SQLDBC_ResultSet;
    friend class SQLDBC_RowSet;
    friend class SQLDBC_UpdatableRowSet;

    SQLDBC_ConnectionItem(IFR_ConnectionItem *item);

    ~SQLDBC_ConnectionItem();

    SQLDBC_ConnectionItemStorage *m_citem;
};

/**
 * @brief Interface to a large object (LONG).
 *
 * This interface is used when it is required to
 */
class SQLDBC_LOB
{
public:
    /**
     * @brief Put data into a LONG value that is referenced by this LOB object.
     *
     * @param paramAddr Pointer to the data which is to be assigned to
     * the LONG value. The type of the data
     * @param paramLengthIndicator [in] Pointer to parameter length or indicator.
     * @return
     *   - #SQLDBC_OK if successful,
     *   - #SQLDBC_NOT_OK otherwise, in which case the error has been set,
     * @see nextParameter
     */
    SQLDBC_DLLEXPORT SQLDBC_Retcode putData(void *paramAddr, SQLDBC_Length  *paramLengthIndicator);

    /**
     * Closes the LOB object. No further actions can take place.
     */
    SQLDBC_DLLEXPORT SQLDBC_Retcode close();


    /**
     * @brief Retrieves the content of the specified LONG column.
     *
     * The specified LONG column value is converted to the given length and
     * to the output parameter buffer pointed to <var>paramAddr</var>.
     * Repeated calls of this method proceed with reading the LONG value,
     * as long as there is data to read available.
     *
     * @param paramAddr A pointer to the parameters output buffer.
     * @param LengthIndicator [out] Pointer to a variable that stores the
     *        column length or the indicator value #SQLDBC_NULL_DATA if the
     *        column contains the NULL value.
     *        For character data it contains on success the number of bytes
     *        copied to the buffer, except the number of bytes necessary for
     *        the zero-terminator, if the <var>Terminate</var> flag was set.
     *        If the source string exceeds the <var>Size</var> value
     *        #SQLDBC_DATA_TRUNC will be returned and
     *        <var>LengthIndicator</var> is set to the number of bytes
     *        (except the terminator bytes) needed to copy without truncation.
     * @param Size [in] Length of the parameter buffer in bytes.
     *        For character data the <var>Size</var>
     *        argument must be large enough to store the terminator byte(s) if
     *        the <var>Terminate</var> flag is set.
     * @param Terminate [in] Specifies that the output buffer must be finished
     *        with a C-style zero-terminator. The <var>Terminate</var> flag
     *        works only for the host variable type character (ASCII, UCS2 or UTF8).
     *        As a default, all character data is zero-terminated.
     *
     * @return #SQLDBC_OK on success
     *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
     *         #SQLDBC_NO_DATA_FOUND if the complete LONG value has been read.
     *         #SQLDBC_NOT_OK if a database access or conversion
     *         error occurred. In this case an error is set in the
     *         result set/prepared statement that is related to this
     *         object.
     */
    SQLDBC_DLLEXPORT SQLDBC_Retcode getData(void                 *paramAddr,
                                            SQLDBC_Length        *LengthIndicator,
                                            const SQLDBC_Length   Size,
                                            const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

    /**
     * @brief Retrieves the content of the specified LONG column.
     *
     * The specified LONG column value is converted to the given length and
     * to the output parameter buffer pointed to <var>paramAddr</var>.
     * After the operation, the internal position is the start position
     * plus the number of characters that have been read.
     *
     * @param paramAddr A pointer to the parameters output buffer.
     * @param LengthIndicator [out] Pointer to a variable that stores the
     *        column length or the indicator value #SQLDBC_NULL_DATA if the
     *        column contains the NULL value.
     *        For character data it contains on success the number of bytes
     *        copied to the buffer, except the number of bytes necessary for
     *        the zero-terminator, if the <var>Terminate</var> flag was set.
     *        If the source string exceeds the <var>Size</var> value
     *        #SQLDBC_DATA_TRUNC will be returned and
     *        <var>LengthIndicator</var> is set to the number of bytes
     *        (except the terminator bytes) needed to copy without truncation.
     * @param Size [in] Length of the parameter buffer in bytes.
     *        For character data the <var>Size</var>
     *        argument must be large enough to store the terminator byte(s) if
     *        the <var>Terminate</var> flag is set.
     * @param position The start position from where to read.
     * @param Terminate [in] Specifies that the output buffer must be finished
     *        with a C-style zero-terminator. The <var>Terminate</var> flag
     *        works only for the host variable type character (ASCII, UCS2 or UTF8).
     *        As a default, all character data is zero-terminated.
     *
     * @return #SQLDBC_OK on success
     *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
     *         #SQLDBC_NO_DATA_FOUND if the complete LONG value has been read,
     *                               or if the given position is outside the boundaries
     *                               of the value.
     *         #SQLDBC_NOT_OK if a database access or conversion
     *         error occurred. In this case an error is set in the
     *         result set/prepared statement that is related to this
     *         object.
     */
    /* #methodmap SQLDBC_LOB_getDataByPos getData */
    SQLDBC_DLLEXPORT SQLDBC_Retcode getData(void                 *paramAddr,
                                            SQLDBC_Length        *LengthIndicator,
                                            const SQLDBC_Length   Size,
                                            const SQLDBC_Length   position,
                                            const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

    /**
     * Retrieves the length of this LOB in the database. The length is
     * returned in <i>chars</i>. The LOB must be opened for reading to
     * retrieve this value.
     * @return The length of the LOB in the database, or -1 if that information is
     *         not available. If the LOB is a NULL value, 0 is returned.
     */
    SQLDBC_DLLEXPORT SQLDBC_Length getLength();

    /**
     * Get the current read/write position
     * @return The current read/write position in characters. The read/write position starts
     *         with 1. If there is no position available, 0 is returned.
     */
    SQLDBC_DLLEXPORT SQLDBC_Length getPosition();


    /**
     * Retrieves the optimal size of data for reading or writing (the
     * maximum size that can be transferred with one call to the
     * database server).  An application may use this to optimize the
     * communication, by using buffers that are multiples of the
     * preferred size.
     * @return The optimal data size, or 0 if this size is not
     * available (because the LOB is closed).
     */
    SQLDBC_DLLEXPORT SQLDBC_Length getPreferredDataSize();

private:
    IFR_LOB                  *m_lobdata;
    IFR_ConnectionItem       *m_citem;
};

/**
 * @brief Can retrieve information to find out the parameter types and
 * their properties in a PreparedStatement object.
 */
class SQLDBC_ParameterMetaData
{
public:

#ifndef __cplusplus
typedef
#endif
  enum ParameterNullBehavior {
    /**
     * The parameter must not be NULL values.
     */
    parameterNoNulls = 0,

    /**
     * The parameter allows NULL values.
     */
    parameterNullable = 1,

    /**
     * It is unknown whether the parameter allows NULL values.
     */
    parameterNullableUnknown = 2
  }
#ifndef __cplusplus
ParameterNullBehavior
#endif
;

#ifndef __cplusplus
typedef
#endif
  enum ParameterMode {

    /**
     * The input/output behaviour of the parameter is unknown.
     */
    parameterModeUnknown = 0,

    /**
     * The parameter in an input parameter.
     */
    parameterModeIn = 1,

    /**
     * The parameter in an input and output parameter.
     */
    parameterModeInOut = 2,

    /**
     * The parameter in an output parameter.
     */
    parameterModeOut = 4
  }
#ifndef __cplusplus
ParameterMode
#endif
;

  /**
   * @brief Returns the number of parameter/columns in the PreparedStatement
   * or ResultSet object.
   *
   * @return The number of parameter or columns.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int2 getParameterCount();

  /**
   * @brief Requests the name of the specified parameter.
   *
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   * @param buffer The buffer to store the name
   * @param encoding The code of the name of the specified parameter
   * @param bufferSize The buffer size in bytes of the buffer.
   * @param bufferLength [out] The number of bytes stored in the buffer, not
   *        included the number of bytes necessary for the zero-terminator.
   *        If the source string exceeds the <var>bufferSize</var> value
   *        #SQLDBC_DATA_TRUNC is returned and the <var>bufferLength</var>
   *        is set to the number of bytes required to store the name without
   *        truncation (not included the number of bytes necessary for
   *        the zero-terminator).
   * @return #SQLDBC_OK on success, #SQLDBC_DATA_TRUNC if the buffer is too small.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getParameterName(SQLDBC_Int2    param,
                                  char *buffer,
                                  const SQLDBC_StringEncoding encoding,
                                  const SQLDBC_Length  bufferSize,
                                  SQLDBC_Length *bufferLength) const;

  /**
   * @brief Returns the data type of the specified parameter.
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   * @return Data type of the parameter
   */
  SQLDBC_DLLEXPORT
  SQLDBC_SQLType getParameterType(SQLDBC_Int2 param);

  /**
   * @brief Returns the input/output behaviour of the specified parameter.
   *
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   *
   * @return The input/output behaviour of the parameter data type;
   * Possible values are: #parameterModeIn, #parameterModeOut
   * or #parameterModeInOut.
   */
  SQLDBC_DLLEXPORT
  ParameterMode getParameterMode(SQLDBC_Int2 param);

  /**
   * @brief Returns the maximum width in characters of the data type of
   * the specified parameter.
   *
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   *
   * @return The maximum width in bytes of the data type of
   *         the specified parameter
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getParameterLength(SQLDBC_Int2 param);

  /**
   * @brief Returns the number of decimal digits of the data type of the
   * specified parameter.
   *
   * For number types, getPrecision() returns the number of decimal digits.
   * For character types, it returns the maximum length in characters. For
   * binary types, it returns the maximum length in bytes.
   *
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   *
   * @return The number of decimal digits of the data type of the specified
   * parameter.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getPrecision(SQLDBC_Int2 param);

  /**
   * @brief Returns the number of decimal places of the data type of the
   * specified parameter.
   *
   * For none numeric types, the scale is set to zero.
   *
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   *
   * @return Number of decimal places of the data type of the
   *         specified parameter.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getScale(SQLDBC_Int2 param);

  /**
   * @brief Returns maximum physical width in bytes of the data type of
   * the specified parameter.
   *
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   *
   * @return Maximum physical width in bytes of the data type of the
   *         specified parameter.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getPhysicalLength(SQLDBC_Int2 param);

  /**
   * @brief Returns whether NULL values are allowed for the
   * specified parameter values.
   *
   * @param param Index of the parameter.
   * The first parameter is number 1, the second is number 2, ...
   *
   * @return Whether NULL values are allowed for the specified parameter;
   * Possible values are #parameterNoNulls, #parameterNullable
   * or #parameterNullableUnknown
   */
  SQLDBC_DLLEXPORT
  ParameterNullBehavior isNullable(SQLDBC_Int2 param);

private:
  friend class SQLDBC_PreparedStatement;
  friend struct SQLDBC_PreparedStatementStorage;
  SQLDBC_ParameterMetaData(IFR_ParameterMetaData *metadata);
  IFR_ParameterMetaData *m_metadata;
};

/**
 * @brief A SQLDBC_ResultSetMetaData class can retrieve information used
 * to find out types and properties of the columns in a ResultSet.
 */
class SQLDBC_ResultSetMetaData
{
public:
#ifndef __cplusplus
typedef
#endif
  enum ColumnNullBehavior {
    /**
     * The column must not be NULL values.
     */
    columnNoNulls = 0,

    /**
     * The column allows NULL values.
     */
    columnNullable = 1,

    /**
     * It is unknown whether the column allows NULL values.
     */
    columnNullableUnknown = 2
  }
#ifndef __cplusplus
ColumnNullBehavior
#endif
;
  /**
   * @brief Returns the number of columns in this ResultSet object.
   *
   * @return The number of columns in this result set.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int2 getColumnCount();

    /**
     * @brief Requests the name of the specified column.
     *
     * @param column Index of the column.
     *        The first column is number 1, the second is number 2, ...
     * @param buffer Buffer where the column name should be copied into
     * @param encoding Encoding of the column name that is requested.
     * @param bufferSize Size in bytes of the buffer.
     * @param bufferLength [out] The number of bytes stored in the buffer, not
     *        included the number of bytes necessary for the zero-terminator.
     *        If the source string exceeds the <var>bufferSize</var> value
     *        #SQLDBC_DATA_TRUNC is returned and the <var>bufferLength</var>
     *        is set to the number of bytes required to store the name without
     *        truncation (not included the number of bytes necessary for
     *        the zero-terminator).
     *
     * @return #SQLDBC_OK on success, #SQLDBC_DATA_TRUNC if the buffer is
     *         too small.
     */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getColumnName(SQLDBC_Int2 column,
                                 char *buffer,
                                 const SQLDBC_StringEncoding encoding,
                                 const SQLDBC_Length  bufferSize,
                                 SQLDBC_Length *bufferLength) const;


  /**
   * @brief Returns the data type of the specified column.
   *
   * @param column Index of the column.
   *               The first column is number 1, the second is number 2, ...
   *
   * @return The Data type of the specified column.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_SQLType getColumnType(SQLDBC_Int2 column);

  /**
   * @brief Returns maximum width in characters of the specified column.
   * @param column Index of the column.
   * The first column is number 1, the second is number 2, ...
   *
   * @return The maximum number of bytes allowed as the width for the
   * column values.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getColumnLength(SQLDBC_Int2 column);

  /**
   * @brief Returns the maximum number of decimal digits of the
   * specified column.
   *
   * For number types, getPrecision retrieves the number of decimal digits.
   * For character types, it retrieves the maximum length in characters. For
   * binary types, it retrieves the maximum length in bytes.
   *
   * @param column Index of the column.
   * The first column is number 1, the second is number 2, ...
   *
   * @return The maximum number of decimal digits for the column values.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getPrecision(SQLDBC_Int2 column);

  /**
   * @brief Returns the number of decimal places of the data type of the
   * specified column.
   *
   * For non-numeric types, the scale is set to zero.
   *
   * @param column Index of the column.
   * The first column is number 1, the second is number 2, ...
   *
   * @return Number of decimal places of the data type of the
   *         specified column.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getScale(SQLDBC_Int2 column);

  /**
   * @brief Returns column's maximum physical width in bytes of the specified
   * columns.
   * @param column Index of the column.
   * The first column is number 1, the second is number 2, ...
   *
   * @return Maximum number of bytes allowed as the physically width
   *          of the specified column.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getPhysicalLength(SQLDBC_Int2 column);

  /**
   * @brief Returns whether NULL values are allowed for the
   * specified column values.
   * @param column Index of the column.
   * The first column is number 1, the second is number 2, ...
   *
   * @return Whether NULL values are allowed for the specified column;
   * Possible values are #columnNoNulls, #columnNullable
   * or #columnNullableUnknown
   */
  SQLDBC_DLLEXPORT
  ColumnNullBehavior isNullable(SQLDBC_Int2 column);

  /**
   * @brief Returns whether a write operation is possible on the
   * specified column.
   *
   * @param column Index of the column.
   * The first column is number 1, the second is number 2, ...
   *
   * @return #SQLDBC_TRUE if a write operation is possible; #SQLDBC_FALSE otherwise
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Bool isWritable(SQLDBC_Int2 column);

private:
  friend class SQLDBC_PreparedStatement;
  friend class SQLDBC_ResultSet;
  friend struct SQLDBC_ResultSetStorage;
  friend struct SQLDBC_PreparedStatementStorage;
  SQLDBC_ResultSetMetaData(IFR_ResultSetMetaData *metadata);
  IFR_ResultSetMetaData *m_metadata;
};

/**
 * @brief An SQLDBC_RowSet class for representing a SQLDBC row set.
 *
 * A SQLDBC row set is a window on a result set.
 *
 * The size of the row set is determined by the
 * setRowSetSize() method of the SQLDBC_ResultSet object. The method fetch
 * retrieves the rows from the SQLDBC_RowSet object into the bound columns.
 * The getObject() method retrieves one column from the current row into the given buffer.
 * The setPos() method moves the cursor within the row set.
 **/
class SQLDBC_RowSet
  : public SQLDBC_ConnectionItem
{
public:

  /**
   * @brief Sets the cursor to row <var>pos</var> in the SQLDBC_RowSet.
   * @param pos Row number within the row set. The first row of a row
   * set is row number one.
   * @return #SQLDBC_OK if the cursor is positioned on a row;
   *         #SQLDBC_NO_DATA_FOUND otherwise;
   *         #SQLDBC_NOT_OK if a database access error
   *         occurs or the result set type is FORWARD_ONLY.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode setPos(SQLDBC_UInt4 pos);

  /**
   * @brief Writes the retrieved data to the bound columns.
   * @return #SQLDBC_OK if successful;
   * #SQLDBC_NOT_OK otherwise

   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode fetch();

  /**
   * @brief Returns the number of rows written to the bound parameters.
   * @return The number of rows fetched with the last fetch() call.
   * @note The number of fetched rows is updated by the next fetch() call.
   * Positioning of the cursor within result set with SQLDBC_ResultSet::next(), SQLDBC_ResultSet::first(), ... does not change the number of rows.
   */
  SQLDBC_DLLEXPORT
  const SQLDBC_Int4 getRowsAffected() const;

  /**
   * @brief Returns the row status array for the last fetch call.
   *
   * The row status array describes the state of each row. The maximum
   * size of the row status array is given by the setRowSetSize().
   * The row status array is filled during the fetch() call.
   * The return code of the first row matches to the first member of the
   * row status array.
   * @return A pointer to the first element of the row status array.
   * @see setRowSetSize(), getRowsAffected
   */
  SQLDBC_DLLEXPORT
  const SQLDBC_Int4 *getRowStatus() const;

  /**
   * @brief Retrieves and converts the value of the specified column
   * of the current row to a buffer.
   *
   * The specified column value in the current row of this SQLDBC_RowSet
   * object is converted to the given length and SQLDBC_HostType and written
   * to the output parameter buffer pointed
   * to <var>paramAddr</var>.
   *
   * It can be called multiple times to retrieve character or binary data in
   * parts. For fixed-length datatypes getObject retrieves the same data
   * multiple times. Mixing variable-length datatypes and fixed-length
   * datatypes may produce unexpected results.
   *
   * The current row may be set by a positioning command from
   * SQLDBC_ResultSet (current row = 1) or by the setPos method of the
   * SQLDBC_RowSet object.
   *
   * @param Index Index of the column.
   *              The first column is column number 1, the second is column
   *        number 2, ...
   * @param Type Parameter type of the output buffer.
   * @param paramAddr A pointer to the parameters output buffer.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only necessary for
   *        non-integral data types. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the host var type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success
   *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
   *         #SQLDBC_NOT_OK if a database access or conversion
   *         error occurred. In this case an error is set on this
   *         SQLDBC_RowSet object.
   *
   * @see bindColumn
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getObject(const SQLDBC_Int4     Index,
                           const SQLDBC_HostType Type,
                           void                 *paramAddr,
                           SQLDBC_Length        *LengthIndicator,
                           const SQLDBC_Length   Size,
                           const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

  /**
   * @brief Retrieves and converts the value with an start offset in
   * of the specified column from a of the current row to a buffer.
   *
   * The specified column value in the current row of this SQLDBC_RowSet
   * object is converted to the given length and SQLDBC_HostType and written
   * to the output parameter buffer pointed
   * to <var>paramAddr</var>.
   *
   * It can be called multiple times to retrieve character or binary data in
   * parts. For fixed-length datatypes getObject retrieves the same data
   * multiple times. Mixing variable-length datatypes and fixed-length
   * datatypes may produce unexpected results.
   *
   * The current row may be set by a positioning command from
   * SQLDBC_ResultSet (current row = 1) or by the setPos method of the
   * SQLDBC_RowSet object.
   *
   * @param Index Index of the column.
   *              The first column is column number 1, the second is column
   *        number 2, ...
   * @param Type Parameter type of the output buffer.
   * @param paramAddr A pointer to the parameters output buffer.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only necessary for
   *        non-integral data types. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param StartPos [in] Start position in long column from which on the
   *        data should be retrieved. Start position is counted in bytes from 1.
   *        Negative <var>StartPos</var> counts from the end of the long column.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the host var type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success
   *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
   *         #SQLDBC_NOT_OK if a database access or conversion
   *         error occurred. In this case an error is set on this
   *         SQLDBC_RowSet object.
   *
   * @see bindColumn
   **/
  /* #methodmap SQLDBC_RowSet_getObjectByPos getObject */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getObject(const SQLDBC_Int4     Index,
                           const SQLDBC_HostType Type,
                           void                 *paramAddr,
                           SQLDBC_Length        *LengthIndicator,
                           const SQLDBC_Length   Size,
                           SQLDBC_Length         StartPos,
                           const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

protected:
  friend class SQLDBC_ResultSet;
  friend struct SQLDBC_ResultSetStorage;
  SQLDBC_RowSet(IFR_ResultSet *resultset);
};

/**
 * @brief A SQLDBC_UpdatableRowSet class for representing a SQLDBC updatable row set.
 *
 * A SQLDBC updatable row set offers update, insert and delete operations on the
 * underlying result set.
 *
 * The insertAllRows() and insertOneRow() methods allow inserting either
 * a whole row set or a single row (the first one) of the row set into the parent result set.
 * The method updateRow() updates an arbitrary row of the result set while the method
 * deleteRow() deletes a single row on a given position.
 **/
class SQLDBC_UpdatableRowSet
  : public SQLDBC_RowSet
{
public:

  /**
   * @brief Performs a mass insert of the whole row set into the result set.
   * @return #SQLDBC_OK if the cursor is positioned on a row;
   *         #SQLDBC_NO_DATA_FOUND otherwise;
   *         #SQLDBC_NOT_OK if a database access error
   *         occurs or the result set type is FORWARD_ONLY.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode insertAllRows();

  /**
   * @brief Performs a single insert of the first row of the row set into the result set.
   * @return #SQLDBC_OK  if the row is inserted;
   *         #SQLDBC_NOT_OK if a database access error occurs
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode insertOneRow();

  /**
   * @brief Updates a single row with the values of the first row of the row set at
   * a given position.
   * @param position Position in the row set which row has to be updated.
   * @return #SQLDBC_OK  if the row is updated;
   *         #SQLDBC_NOT_OK if a database access error
   *         occurs or the result set type is FORWARD_ONLY.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode updateRow(int position);

  /**
   * @brief Deletes a single row of the row set at a given position.
   * @param position Position in the row set which row has to be deleted.
   * @return #SQLDBC_OK  if the row is deleted;
   *         #SQLDBC_NOT_OK if a database access error
   *         occurs or the result set type is FORWARD_ONLY.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode deleteRow(int position);

private:
  friend struct SQLDBC_ResultSetStorage;
  SQLDBC_UpdatableRowSet(IFR_ResultSet *resultset);
};

struct SQLDBC_ResultSetStorage;

/**
   @brief A class for presenting a database result set.

   A database result set is generated by executing an SQL statement that queries
   the database.

   Select statements, catalog functions, and some procedures create result sets.
   For example, the following SQL statement creates a result set containing all
   the rows and columns of the table DUAL:

     <code>SELECT * FROM DUAL</code>

   A result set can be empty, which is different from there being no result set at all. For
   example, the following SQL statement creates an empty result set:

     <code>SELECT * FROM DUAL WHERE 1 = 2</code>

   An SQLDBC_ResultSet object maintains a cursor pointing to its current row of data.
   Initially the cursor is positioned before the first row. The next()
   method moves the cursor to the next row, and as it returns
   #SQLDBC_NO_DATA_FOUND when there are no more rows in the SQLDBC_ResultSet
   object, it can be used in a WHILE loop to iterate the result set.

   Example for creating an SQLDBC_ResultSet object:

     <code>
     SQLDBC_Statement *stmt = conn->createStatement (); <br>
     stmt->execute ("SELECT * FROM DUAL"); <br>
     SQLDBC_ResultSet *rs = stmt->getResultSet (); <br>
     rs->next(); <br>
     </code>

   To reduce the time needed for retrieving the data from the database, the
   SQLDBC_ResultSet class supports so called block cursors, which can return more
   than one row at a time. The rows returned by a block cursor are called a 'row set'.
   The result set is fixed, the rowset is not. It changes position and contents
   each time a new set of rows is retrieved from the database.

   With block cursors, the method setRowSetSize() must be used
   with a parameter greater than 1.

   Navigation within the data represented by the SQLDBC_ResultSet object is possible
   using of navigation methods like first(), next(), previous(), relative() etc.

   When block cursors are used, after applying the navigation methods, the cursor
   points to the actual row set. For example assuming a result set size of 50
   and a rowset size of 10, in the following sequence the block cursor points
   to the rows indicated:

   - first()    : Rows  1 - 10 of the result set
   - next()     : Rows 11 - 20 of the result set
   - next()     : Rows 21 - 30 of the result set
   - previous() : Rows 11 - 20 of the result set
   - last()     : Rows 41 - 50 of the result set

   In order to perform operations that operate on a single row only when multiple rows have been
   fetched, the application must indicate which row is the current row. When
   a block cursor first returns a row set, the current row is the first row of that
   row set. To change the current row, the application must call the member function setPos().

   The data of a certain column in the current row can be retrieved by calling the
   method getObject().

   Data fetched from the database is passed on to the application in variables that
   the application has allocated for this purpose. Before fetching the data from the database, the
   application bind these variables to the columns of the result
   set. Applications can bind any number of columns of the result set, including binding no columns at all.

   Binding of columns is done by calling to the member function bindColumn().
   The column binding valid for all rows.

   After positioning the cursor through navigation methods, the data
   from the database is written into the bound column variables by a call to
   the member function fetch() of the row set of this result set.
   When block cursors are used, the number
   of rows actually filled can be determined with the member function
   getResultCount().

   For unbounded columns, data can be written into application variables with
   getObject(), or - in case of block cursors - by calling
   setPos() on the rowset and then calling getObject().

 */
class SQLDBC_ResultSet
  : public SQLDBC_ConnectionItem
{
public:

  /**
   * @brief Retrieves an SQLDBC_ResultSetMetaData object that
   * contains information about the columns of this
   * SQLDBC_ResultSet object.
   *
   * @return An SQLDBC_ResultSetMetaData object that describes the columns or
   * NULL if the meta data cannot be retrieved.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_ResultSetMetaData* getResultSetMetaData();

  /**
   * @brief Returns number of rows of the current result set
   * @return The number of rows in the result set, or -1 if not known.
   **/
  SQLDBC_DLLEXPORT
  const SQLDBC_Int4 getResultCount () const;

  /**
   * @brief Binds a user-supplied memory buffer to an SQL column of a
   * result set.
   *
   * Applications must use SQLDBC_ResultSetMetadata to retrieve
   * information about the type and length of the columns of a
   * result set.
   *
   * @param Index [in] Parameter number. The parameter numbers in an SQL statement are, in increasing order, starting with 1.
   * @param Type [in] Parametertype of the output buffer
   * @param paramAddr [in] A pointer to a buffer for the parameter's data.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only necessary for
   *        non-integral data types. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the host var type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success

   *         #SQLDBC_NOT_OK If a wrong argument value was
   *         passed. In this case an error is set on this
   *         SQLDBC_ResultSet object.
   * @see getObject
   * @todo: Indicator handling and truncation handling
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode bindColumn (const SQLDBC_UInt4    Index,
                             const SQLDBC_HostType Type,
                             void                 *paramAddr,
                             SQLDBC_Length        *LengthIndicator,
                             const SQLDBC_Length   Size,
                             const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

  /**
   * @brief Binds a user-supplied memory buffer to an SQL column of a
   * result set.
   *
   * Applications must use SQLDBC_ResultSetMetadata to retrieve
   * information about the type and length of the columns of a
   * result set.
   *
   * @param Index [in] Parameter number. The parameter numbers in an SQL statement are, in increasing order, starting with 1.
   * @param Type [in] Parametertype of the output buffer
   * @param paramAddr [in] A pointer to a buffer for the parameter's data.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param PositionIndicator [in] Pointer to a variable that stores the
   *        start position in bytes from which to read non-integral data.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only necessary for
   *        non-integral datatypes. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the host var type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success

   *         #SQLDBC_NOT_OK If a wrong argument value was
   *         passed. In this case an error is set on this
   *         SQLDBC_ResultSet object.
   * @see getObject
   * @todo: Indicator handling and truncation handling
   */
  /* #methodmap SQLDBC_ResultSet_bindColumnByPos bindColumn */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode bindColumn (const SQLDBC_UInt4    Index,
                             const SQLDBC_HostType Type,
                             void                 *paramAddr,
                             SQLDBC_Length        *LengthIndicator,
                             SQLDBC_Length        *PositionIndicator,
                             const SQLDBC_Length   Size,
                             const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

  /**
   * @brief Clears the information about columns that have been bound.
   *   All information about columns that
   *   have been bound before using @c bindColumn. method is cleared.
   * @return #SQLDBC_OK on success.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode clearColumns();

    /**
   * @brief Sets the desired fetch size.
   * The fetch size setting is a hint to the runtime, and not a strict setting
   * such as the row set size. If it is 1, the <code>FETCH</code> statements
   * issued by the library will not be mass statements, making additional
   * functionality,
   * such as updating using the <code>CURRENT OF</code> predicate possible.
   * @param fetchsize The new fetch size.
   */
   SQLDBC_DLLEXPORT
   void setFetchSize(SQLDBC_Int2 fetchsize);

  /**
   * @brief Sets the size of the row array of bounded parameters.
   * @param rowsetsize The number of rows.
   * @see getRowSetSize
   */
  SQLDBC_DLLEXPORT
  void setRowSetSize (SQLDBC_UInt4 rowsetsize);

  /**
   * @brief Returns the size of the row array of bounded parameters.
   * @return The size of the array of bound parameters
   * @see setRowSetSize
   */
  SQLDBC_DLLEXPORT
  const SQLDBC_UInt4 getRowSetSize () const;

  /**
   * @brief Retrieves the SQLDBC_RowSet returned by block cursor operations.
   *
   * @return A reference to an SQLDBC_RowSet object if the SQLDBC_ResultSet
   * object has an SQLDBC_RowSet or NULL otherwise.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_RowSet *getRowSet ();

  /**
   * @brief Retrieves the SQLDBC_UpdatableRowSet returned by block cursor operations.
   *
   * @return A reference to an SQLDBC_UpdatableRowSet object if the SQLDBC_ResultSet
   * object has an SQLDBC_UpdatableRowSet or NULL otherwise.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_UpdatableRowSet *getUpdatableRowSet ();

  /**
   * @brief Retrieves the SQLDBC_Statement object that belongs to the result set.
   * @return A reference to the SQLDBC_Statement object that generate
   * this result set.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Statement *getStatement();

  /**
   * @brief Returns SQLDBC_TRUE, if the result set is updatable
   * @return SQLDBC_TRUE or SQLDBC_FALSE
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Bool isUpdatable();

  /**
   * @brief Moves the cursor to the first row in
   * this SQLDBC_ResultSet object.
   *
   * @return #SQLDBC_OK if the cursor is on a valid row;
   * #SQLDBC_NO_DATA_FOUND if there are no rows in the result set;
   * #SQLDBC_NOT_OK if a database access error
   * occurs or the result set type is FORWARD_ONLY
   *
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode first();

  /**
   * @brief Moves the cursor down one row from its current position.
   *
   * A result set cursor is initially positioned
   * before the first row; the first call of the member function
   * next() positions the cursor to the first row set; the
   * second call positions the cursor on the second row set, and so on.
   *
   * An SQLDBC_ResultSet object's
   * warning chain is cleared when the next call is done.
   *
   * @return #SQLDBC_OK if the new current row is valid;
   * #SQLDBC_NO_DATA_FOUND if there are no more rows;
   * #SQLDBC_NOT_OK if a database access error occurs
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode next();

  /**
   * @brief Moves the cursor to the previous row in this
   * SQLDBC_ResultSet object.
   *
   * @return #SQLDBC_OK if the cursor is on a valid row;
   * #SQLDBC_NO_DATA_FOUND if it is outside the result set;
   * #SQLDBC_NOT_OK if a database access error
   * occurs or the result set type is FORWARD_ONLY
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode previous();

  /**
   * @brief Moves the cursor to the last row in
   * this SQLDBC_ResultSet object.
   *
   * @return #SQLDBC_OK if the cursor is on a valid row;
   * #SQLDBC_NO_DATA_FOUND if there are no rows in the result set;
   * #SQLDBC_NOT_OK if a database access error
   * occurs or the result set type is FORWARD_ONLY.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode last();

  /**
   * @brief Moves the cursor to the specified row number in
   * this SQLDBC_ResultSet object.
   *
   * <p>If the row number is positive, the cursor moves to
   * this specified row number with respect to the
   * beginning of the result set. The first row is row number 1, the second
   * is row number 2, and so on.
   *
   * <p>If the given row number is negative, the cursor moves to
   * an 'absolute' position with respect to
   * the end of the result set.  For example, calling the method
   * absolute(-1) positions the
   * cursor on the last row; calling the method absolute(-2)
   * moves the cursor to the next-to-last row, and so on.
   *
   * <p>An attempt to position the cursor beyond the first or last row in
   * the result set leaves the cursor before the first row or after
   * the last row.
   *
   * @param row Row number where the cursor is moved to.
   *        A positive number specifies the absolute row number with respect to the
   *        beginning of the result set; a negative number specifies a absolute
   *        row number respect to the end of the result set.
   * @return #SQLDBC_OK if the cursor is on the result set;
   * #SQLDBC_NO_DATA_FOUND otherwise;
   * #SQLDBC_NOT_OK if a database access error occurs.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode absolute (int row);

  /**
   * @brief Moves the cursor by a relative number of rows, either positive or negative.
   *
   * Attempting to move the cursor beyond the first or last row in the
   * result set positions the cursor before or after
   * the first or last row. Calling relative(0) is valid, but does
   * not change the cursor position.
   *
   * @note Calling the method relative(1)
   * is identical to calling the method next().
   * Calling the method relative(-1) is identical
   * to calling the method previous().
   *
   * @param relativePos An integer value specifying the number of rows by which
   *        the cursor is to be moved from its current position; a positive number moves the cursor
   *        forward; a negative number moves the cursor backward.
   * @return #SQLDBC_OK if the cursor is positioned on a row;
   *         #SQLDBC_NO_DATA_FOUND otherwise;
   *         #SQLDBC_NOT_OK if a database access error
   *         occurs or the result set type is FORWARD_ONLY.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode relative(int relativePos);

  /**
   * @brief Closes this result set. Further operations are not allowed.
   */
  SQLDBC_DLLEXPORT
  void close();

  /**
   * @brief Returns the current row number.
   * The first row is row number 1, the second row number 2, and so on.
   *
   * @return The current row number or 0 if the cursor is positioned outside the result set.
   */
  SQLDBC_DLLEXPORT
  const SQLDBC_UInt4 getRowNumber() const;

  /**
   * @brief Retrieves and converts the value of the specified column
   * of the current row to a buffer.
   *
   * The specified column value in the current row of this SQLDBC_RowSet
   * object is converted to the given length and SQLDBC_HostType and written
   * to the output parameter buffer pointed
   * to <var>paramAddr</var>.
   *
   * It can be called multiple times to retrieve character or binary data in
   * parts. For fixed-length datatypes getObject retrieves the same data
   * multiple times. Mixing variable-length datatypes and fixed-length
   * datatypes may produce unexpected results.
   *
   * The current row may be set by a positioning command from
   * SQLDBC_ResultSet (current row = 1) or by the setPos method of the
   * SQLDBC_RowSet object.
   *
   * @param Index Index of the column.
   *              The first column is column number 1, the second is column
   *        number 2, ...
   * @param Type Parameter type of the output buffer.
   * @param paramAddr A pointer to the parameters output buffer.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only neccessary for
   *        non-integral datatypes. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the hostvar type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success
   *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
   *         #SQLDBC_NOT_OK if a database access or conversion
   *         error occursed. In this case an error is set on this
   *         SQLDBC_ResultSet object.
   *
   * @see bindColumn
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getObject(const SQLDBC_Int4     Index,
                           const SQLDBC_HostType Type,
                           void                 *paramAddr,
                           SQLDBC_Length        *LengthIndicator,
                           const SQLDBC_Length   Size,
                           const SQLDBC_Bool     Terminate=SQLDBC_TRUE);


  /**
   * @brief Retrieves and converts the value with an start offset in
   * of the specified column from a of the current row to a buffer.
   *
   * The specified column value in the current row of this SQLDBC_RowSet
   * object is converted to the given length and SQLDBC_HostType and written
   * to the output parameter buffer pointed
   * to <var>paramAddr</var>.
   *
   * It can be called multiple times to retrieve character or binary data in
   * parts. For fixed-length datatypes getObject retrieves the same data
   * multiple times. Mixing variable-length datatypes and fixed-length
   * datatypes may produce unexpected results.
   *
   * The current row may be set by a positioning command from
   * SQLDBC_ResultSet (current row = 1) or by the setPos method of the
   * SQLDBC_RowSet object.
   *
   * @param Index Index of the column.
   *              The first column is column number 1, the second is column
   *        number 2, ...
   * @param Type Parameter type of the output buffer.
   * @param paramAddr A pointer to the parameters output buffer.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only necessary for
   *        non-integral datatypes. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param StartPos [in] Start position in long column from which on the
   *        data should be retrieved. Start position is counted in bytes from 1.
   *        Negative <var>StartPos</var> counts from the end of the long column.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the host var type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success
   *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
   *         #SQLDBC_NOT_OK if a database access or conversion
   *         error occurred. In this case an error is set on this
   *         SQLDBC_ResultSet object.
   *
   * @see bindColumn
   **/
  /* #methodmap SQLDBC_ResultSet_getObjectByPos getObject */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getObject(const SQLDBC_Int4     Index,
                           const SQLDBC_HostType Type,
                           void                 *paramAddr,
                           SQLDBC_Length        *LengthIndicator,
                           const SQLDBC_Length   Size,
                           SQLDBC_Length         StartPos,
                           const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

private:
  friend class SQLDBC_Statement;
  friend struct SQLDBC_StatementStorage;

  /**
   * @brief Creates SQLDBC_ResultSet object.
   */
  SQLDBC_ResultSet(SQLDBC_Statement* statement,
                   IFR_ResultSet *resultset);
  /**
    @brief Destructor.
  */
  ~SQLDBC_ResultSet();

  SQLDBC_ResultSetStorage *m_cresult;
};

struct SQLDBC_StatementStorage;

/**
   @brief An SQLDBC_Statement is a class for executing SQL statements immediately.

   With this class applications can handle all DDL and DML commands
   without parameter markers. Parameter markers are the '?' or ':\<name\>' tags.
   Use SQLDBC_Connection::createStatement from the connection object to
   get a new SQLDBC_Statement object.

   Use the execute() member function to execute the SQL statement.

   The SQL statement may contain ASCII or UCS2 characters and must not be
   zero-terminated. The execute() member function converts it automatically
   to the correct code set according to the encoding of the database.
   In this way it is possible to write portable code for UNICODE and
   non-UNICODE databases.
   To increase performance, applications should use UCS2 statements
   only for UNICODE databases.

   @par Example:
   Execution of a SQL statement

   <pre>
   SQLDBC_Statement *stmt = conn->createStatement();
   SQLDBC_Retcode rc = stmt->execute("SELECT * FROM DUAL");
   if (rc != SQLDBC_OK) {
     // Handle error ...
   }
   </pre>

   After execution, the application must check the return code of the
   command. If an SQL error occurred the error() member function
   returns error details.

   @see SQLDBC_ErrorHndl for more informations.

   Query statements like SELECT or CALL DBPROC commands may produce a result
   set. Applications should check this with the isQuery() or getResultSet()
   member function.

   @par Example:
   Check if a result set exists.

   <pre>
   if (stmt->isQuery()) {
      SQLDBC_ResultSet *result = stmt->getResultSet();
      // Retrieving result ...
   }
   </pre>

   The getResultSet() member function returns a reference to an SQLDBC_ResultSet
   object. Applications can use SQLDBC_ResultSet for retrieving data from
   the result set of the query statement or to navigate within result sets.
   An opened result set is automatically closed by reusing the execute() member
   function or by closing the object with an SQLDBC_Connection::releaseStatement().

   @see SQLDBC_ResultSet for more informations about result sets.

   Prior to executing an SQL statement, the application can set the name of the
   result set with the setCursorName() member function, or it can set the type
   of the result set with setResultSetType() to modify the behavior of the
   result set.

   For non-query SQL statements the application can check the number of
   modified rows with the getRowsAffected() member function.

  @note:
  - Applications must call SQLDBC_Connection::createStatement() to construct an new SQLDBC_Statement object.
  - Use SQLDBC_PreparedStatement for executing the SQL statement more than once or for SQL
  commands containing parameters.
  - Use isQuery() or getResultSet() to check if the SQL statement has returned
  a result set.
  - Manipulation of the result set behavior (row limits, scrollability,
  updatability) is possible with setResultSetType() and setResultSetConcurrencyType() member functions.
 */
class SQLDBC_Statement
  : public SQLDBC_ConnectionItem
{
public:
  /**
   * @brief Executes an UNICODE/ASCII coded SQL statement.
   *
   * The SQL statement is executed immediately on sending a DBS request to the
   * database. In case of an error, the SQLDBC_ErrorHndl object (to be queried using error())
   * of this object is set. If the executed SQL statement is a query, a result set is created.
   * The application can check this using isQuery(), and obtain retrieve the
   * ResultSet using getResultSet().
   *
   * @param sql The SQL statement to be executed. The coding of this string
   * argument depends from the <var>encoding</var> argument.
   * @param sqlLength The length in bytes of the of the SQL statement
   * @param encoding The character coding of the SQL statement
   * @return #SQLDBC_OK on success, or else #SQLDBC_NOT_OK.
   *         In this case check the error instance.
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode execute(const char *sql,
                         const SQLDBC_Length sqlLength,
                         const SQLDBC_StringEncoding encoding);

  /**
   * @brief Executes an UNICODE/ASCII coded zero-terminated SQL statement.
   *
   * The SQL statement is executed immediately on sending a DBS request to the
   * database. In case of an error, the SQLDBC_ErrorHndl object (to be queried using error())
   * of this object is set. If the executed SQL statement is a query, a result set is created.
   * The application can check this using isQuery(), and obtain retrieve the
   * resultset using getResultSet().
   *
   * @param sql The zero-terminated SQL statement to be executed. The coding of
   * this string argument depends from the <var>encoding</var> argument.
   * @param encoding The character coding of the SQL statement.
   * @return #SQLDBC_OK on success, or else #SQLDBC_NOT_OK.
   *         In this case check the error instance.
   **/
  /* #methodmap SQLDBC_Statement_executeNTS execute */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode execute(const char *sql,
                         const SQLDBC_StringEncoding encoding);

  /**
   * @brief Executes an ASCII coded zero-terminated SQL statement.
   *
   * The SQL statement is executed immediately on sending a DBS request to the
   * database. In case of an error, the SQLDBC_ErrorHndl object (to be queried using error())
   * of this object is set. If the executed SQL statement is a query, a result set is created.
   * The application can check this using isQuery(), and obtain retrieve the
   * resultset using getResultSet().
   * @param sql The zero-terminated ASCII coded SQL statement to be execute.
   * @return #SQLDBC_OK on success, or else #SQLDBC_NOT_OK.
   *         In this case check the error instance.
   **/
  /* #methodmap SQLDBC_Statement_executeASCII execute */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode execute(const char *sql);

  /**
   * @brief Sets the maximum number of be rows that can be fetched at once.
   *
   * Sets the maximum number of rows to be fetched from a result set. Use this
   * to manipulate the number of rows in the order interface.
   * Use a value > 0 to set the maximum number of rows.
   * Use a value <= 0 to reset this limit to the default value.
   * The default value is 'unlimited' (32767).
   * Setting this value does not affect an already executed SQL statement.
   * @param rows [in] Number of Rows to be fetched.
   * @return none
   **/

  SQLDBC_DLLEXPORT
  void setResultSetFetchSize(SQLDBC_Int2 rows);

  /**
   * @brief Limits the number of rows of an SQLDBC_ResultSet object.
   *
   * The number of rows of the result set is truncated if the result of a
   * query statement is larger than this limit.
   * The default setting is 'unlimited' (0).
   * Setting this limit does not affect an already executed SQL statement.
   * @param rows [in] Maximum number of rows of a result set
   * @return none
   * @see getMaxRows
   **/

  SQLDBC_DLLEXPORT
  void setMaxRows(SQLDBC_UInt4 rows);

  /**
   * @brief Returns the maximum number of rows allowed in a result set.
   *
   * This value may be set with setMaxRows before the execution of an SQL statement.
   * @return Maximum number of rows of a result set.
   * @see setMaxRows
   **/

  SQLDBC_DLLEXPORT
  const SQLDBC_UInt4 getMaxRows() const;

  /**
   * @brief Retrieves a reference to the SQLDBC_ResultSet object.
   *
   * @return A reference to an SQLDBC_ResultSet object if the
   * statement object has a result set, NULL otherwise.
   * @see isQuery
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_ResultSet *getResultSet();

  /**
   * @brief Sets the cursor name.
   *
   * Setting the cursorname affects only query and database procedure commands.
   * For DDL-, INSERT-, UPDATE- and DELETE- commands setting the cursorname
   * has no effect.
   * @param buffer The new cursor name.
   * @param bufferLength Length in bytes of the buffer. In case a zero-terminated string passed on in the <var>buffer</var> argument, you can set
   * the <var>bufferLength</var> to #SQLDBC_NTS.
   * @param encoding Encoding of the cursor name.
   * @return none
   * @see getCursorName
   **/
  SQLDBC_DLLEXPORT
  void setCursorName(const char    *buffer,
                     SQLDBC_Length  bufferLength,
                     const SQLDBC_StringEncoding encoding);

    /**
    * @brief Sets the command info.
    *
    * Sets the command info and a line number (for example name of programm and current line number).
    * This will be send to the database kernel as part of a parse order.
    * If the kernel runs in a special diagnose mode these information will be
    * stored in a system table and can be used to profile your application. The column info and line
    * number will be unset after the next execute of a SQL command at this statement handle.
    *
    * @param buffer The command info.
    * @param bufferLength Length in bytes of the buffer. In case a zero-terminated string passed on in the <var>buffer</var> argument, you can set
    * the <var>bufferLength</var> to #SQLDBC_NTS. Set <var>bufferLength</var> to #SQLDBC_NULL_DATA if you want to unset the command info and line number.
    * @param lineNumber The line number.
    * @return #SQLDBC_OK on success, #SQLDBC_DATA_TRUNC if the provided buffer was too long or
    * #SQLDBC_NOT_OK in case of an error.
    */
  SQLDBC_DLLEXPORT
    SQLDBC_Retcode setCommandInfo(const char *buffer,
                               SQLDBC_Length bufferLength,
                               SQLDBC_Int4 lineNumber);
  /**
   * @brief Retrieves the cursor name.
   *
   * The cursor name may be set by:
   * - The setCursorName() member function
   * - A named query statement
   * - A database procedure returning a result table
   *
   * @param buffer The buffer into which the zero-terminated cursor name is copied.
   * @param encoding Encoding of the <var>buffer</var> where the
   * cursor name is stored.
   * @param bufferSize   Maximum size of the buffer in bytes.
   * @param bufferLength [out] Number of bytes copied to the buffer,
   *        except the number of bytes necessary for the zero-terminator.
   *        If the source string exceeds the <var>bufferSize</var>.
   *        #SQLDBC_DATA_TRUNC will be returned and the <var>bufferLength</var>
   *        set to the number of bytes (except terminator bytes)
   *        needed to copy without truncation.
   * @return #SQLDBC_OK on success, #SQLDBC_DATA_TRUNC
   *         if the provided buffer wasn't able store the complete name.
   * @see setCursorName
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getCursorName(char         *buffer,
                               const SQLDBC_StringEncoding encoding,
                               const SQLDBC_Length bufferSize,
                               SQLDBC_Length *bufferLength) const;

  /**
   * @brief Retrieves the table name (for SELECT FOR UPDATE commands).
   * @param buffer The buffer in which into the table name is copied.
   * @param encoding Encoding of the <var>buffer</var> where the
   * table name is stored.
   * @param bufferSize Size (in bytes) of the buffer
   * @param bufferLength [out] Number of bytes copied to the buffer,
   *        except the number of bytes necessary for the zero-terminator.
   *        If the source string exceeds the <var>bufferSize</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and the <var>bufferLength</var>
   *        set to the number of bytes (except the terminator bytes)
   *        needed to copy the table name without truncation.
   * @return #SQLDBC_OK on success, #SQLDBC_DATA_TRUNC
   *         if the provided buffer wasn't able to store the complete name.
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getTableName(char          *buffer,
                              const SQLDBC_StringEncoding encoding,
                              const SQLDBC_Length  bufferSize,
                              SQLDBC_Length *bufferLength) const;

  /**
   * @brief Checks if the SQL statement is a query.
   *
   * A statement is a query only
   * if it returns a result set. This can be determined only after execution of the
   * statement object.
   * @note This method is overridden by the SQLDBC_PreparedStatement
   * class, which can determine the query property before the statement is
   * executed.
   * @return #SQLDBC_TRUE if the statement does produce a result
   *         set, #SQLDBC_FALSE if not, or if it cannot be determined.
   * @see getResultSet
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Bool isQuery() const;

  /**
   * @brief Returns the number of rows affected by the executed SQL statement.
   *
   * This method returns a non-zero value if more than one row was addressed
   * by the SQL statement. If the return value is lower than zero, more than one
   * rows was addressed but the exact number of addressed rows cannot
   * be determined.
   * @return Number of addressed rows
   **/
  SQLDBC_DLLEXPORT
  const SQLDBC_Int4 getRowsAffected() const;

  enum ResultSetType {
    FORWARD_ONLY = 1, //!< The result set can be scrolled forward only.
    SCROLL_SENSITIVE = 2, //!< The content of the result set may change.
    SCROLL_INSENSITIVE = 3 //!< The content of the result set cannot not change (FOR REUSE).
  };

  /**
   * @brief Sets the type of a result set.
   *
   * A result set is only created by a query command.
   *
   * There are three kind of result sets:
   * - The result set can only be scrolled forward (#FORWARD_ONLY)
   * - The result set is scrollable but may change (#SCROLL_SENSITIVE)
   * - The result set is scrollable and not change (#SCROLL_INSENSITIVE)
   *
   * The default for the result set type is #SCROLL_INSENSITIVE
   * @param type [in] The result set type.
   * Possible values are: #FORWARD_ONLY, #SCROLL_SENSITIVE, #SCROLL_INSENSITIVE
   *
   * @return none
   * @see getResultSetType
   * @todo Implement this method
   **/
  SQLDBC_DLLEXPORT
  void setResultSetType (ResultSetType type );

  /**
   * @brief Returns the type of a result set.
   *
   * If the requested result set type isn't allowed for this SQL statement, the SQLDBC changes
   * it to an allowed one. Check this with getResultSetType() if the
   * execute() member function returns with a warning.
   * @return #FORWARD_ONLY, #SCROLL_SENSITIVE or #SCROLL_INSENSITIVE
   * @see setResultSetType
   * @todo Implement this method
   **/
  SQLDBC_DLLEXPORT
  const ResultSetType getResultSetType() const;

  enum ConcurrencyType {
    CONCUR_UPDATABLE = 10, //!< The result can be updated (FOR UPDATE).
    CONCUR_READ_ONLY = 11  //!< The result cannot be updated.
  };

  /**
   * @brief Sets the type of the result set concurrency
   *
   * There are two kinds of concurrency:
   * - The result set can be updated #CONCUR_UPDATABLE
   * - The result set is read-only #CONCUR_READ_ONLY
   *
   * The default setting for the concurrency is #CONCUR_READ_ONLY
   * @param type [in] The result set type.
   * Possible values are: #CONCUR_READ_ONLY #CONCUR_UPDATABLE
   * @return none
   * @todo Implement this method
   **/
  SQLDBC_DLLEXPORT
  void setResultSetConcurrencyType (ConcurrencyType type);

  enum HoldabilityType {
    CURSOR_HOLD_OVER_COMMIT = 20, //!< The result persists after a COMMIT command.
    CURSOR_CLOSE_ON_COMMIT = 21   //!< The result is closed on COMMIT.
  };

  /**
   * @brief Retrieves the type of the result set concurrency
   *
   * There are two kinds of concurrency:
   * - The result set can be updated #CONCUR_UPDATABLE
   * - The result set is read-only #CONCUR_READ_ONLY
   *
   * @return type of the result set concurrency
   **/
  SQLDBC_DLLEXPORT
  const ConcurrencyType getResultSetConcurrencyType() const;

    /**
     * @brief Returns the row status array.
     *
     * The row status array describes the state of each row. The size of the
     * row status array is increased by each addBatch() function call.
     * @return A pointer to the first element of the row status array.
     * @see addBatch()
     */
    SQLDBC_DLLEXPORT
    const SQLDBC_Int4 *getRowStatus() const;

    /**
     * @brief Returns the size of the row status array.
     * @return The number of elements of the row status array, which is also
     *   the number of array elements that are used for batch execution.
     */
    SQLDBC_DLLEXPORT
    const SQLDBC_UInt4 getBatchSize() const;

    /**
     * @brief Adds a statement to the batch.
     *
     * Statements for batched execution must not return result sets.
     * @param sql The statement to be added
     * @param sqlLength Length in Bytes of the statement
     * @param encoding Encoding of the statement
     * @return #SQLDBC_OK on success
     * @see addBatch()
     */
    SQLDBC_DLLEXPORT
    const SQLDBC_Retcode addBatch(const char *sql,
                                  SQLDBC_Length sqlLength,
                                  SQLDBC_StringEncoding encoding);

    /**
     * Adds a statement to the batch. The must be end with a zero-terminator.
     * @param sql The statement to be added
     * @param encoding Encoding of the statement
     * @return #SQLDBC_OK on success
     */
    /* #methodmap SQLDBC_Statement_addBatchNTS addBatch */
    SQLDBC_DLLEXPORT
    const SQLDBC_Retcode addBatch(const char *sql,
                                  SQLDBC_StringEncoding encoding);

    /**
     * Adds a statement to the batch. The must be end with a zero-terminator
     * and in ASCII code.
     * @param sql The statement to be added
     * @return #SQLDBC_OK on success
     */
    /* #methodmap SQLDBC_Statement_addBatchASCII addBatch */
    SQLDBC_DLLEXPORT
    const SQLDBC_Retcode addBatch(const char *sql);

    /**
     * Executes the statements previously input via addBatch().
     * @return #SQLDBC_OK on success, #SQLDBC_NOT_OK on error,
     *         #SQLDBC_NO_DATA_FOUND if there are no SQL
     *        statements to execute.
     */
    SQLDBC_DLLEXPORT
    const SQLDBC_Retcode executeBatch();

    /**
     * Clears the array of batched statements.
     */
    SQLDBC_DLLEXPORT
    void clearBatch();

protected:
    /**
     * @internal Method to clear the result set before next execute.
     */
    void clearResultSet();

private:
  friend class SQLDBC_Connection;
  friend class SQLDBC_PreparedStatement;
  /**
   *  @brief Creates a new SQLDBC_Statement object.
   * This constructor should never called directly. Functions should use
   * SQLDBC_Connection::createStatement() to create a new SQLDBC_Statement object.
   **/
    SQLDBC_Statement(IFR_Statement *stmt);
  /**
   *  @brief Creates a new SQLDBC_Statement object.
   * This constructor is used internally, do never use it.
   */
    SQLDBC_Statement(IFR_PreparedStmt *stmt);
  ~SQLDBC_Statement();
  SQLDBC_StatementStorage *m_cstmt;
};

struct SQLDBC_PreparedStatementStorage;

/**
  @brief A class for preparing and executing SQL statements.

  A prepared SQL command can be parsed and contain input and output
  parameters. Parameters are marked with a '?' or ':\<name\>' tag.
  All DML commands can be parsed. DDL commands can be parsed, too.
  However, it is not recommended to do so.
  Prepared SQL commands increase the performance since they are parsed only
  once and executed several times.
  Applications only need to change the content of the bound parameters and
  execute the command again.

  All prepared SQL commands are stored in an internally managed
  \link pcache.html ParseInfo Cache \endlink.
  The ParseInfo Cache shares this information with different prepared
  SQL statements within the same connection.


  - Use SQLDBC_Connection::createPreparedStatement from the connection
  object to get a new SQLDBC_PreparedStatement object.
  - Use the prepare() member function to prepare the SQL statement.
  - Use the execute() member function to execute the previously prepared
  SQL statement.

  The SQL statement may contain ASCII or UCS2 characters and must not
  zero-terminated. The execute() member function converts it to
  the adequate code set considering the code of the database.
  Therefore it is possible to write portable code for UNICODE and non-UNICODE
  databases.
  @note To increase the performance, applications must use UCS2 statements
  for UNICODE databases only.

   @par Example:
   Preparation and execution of an SQL statement.

   <pre>
   SQLDBC_PrepareStatement *stmt = conn->createPreparedStatement();
   SQLDBC_Retcode rc = stmt->prepare("SELECT * FROM DUAL");
   if (rc != SQLDBC_OK) {
     // Handle error ...
   }
   rc = stmt->execute();
   if (rc != SQLDBC_OK) {
     // Handle error ...
   }
   </pre>

  @par Hints:
  - The application must call createPreparedStatement()
    to retrieve a prepared SQL statement
  - For parsing the SQL statement, call the prepare() member function,
  - For a single-row execution, use the execute() member function,
  - For mass execution, bind arrays of parameter values, use setBatchSize() to
    set the row array size to the number of array elements bound,
    and use the execute() method.
  - Use the isQuery() method to check whether the parsed
    statement creates a result set upon execution.
  - Use SQLDBC_Statement for a direct execution, since the prepared statement
    inherits all direct-execution functions from SQLDBC_Statement.
  - The current result set is deleted when
     - A new statement is parsed
     - The statement is re-executed
     - The statement is deleted
     .
  @todo binding parameter by name
 */
class SQLDBC_PreparedStatement
    : public SQLDBC_Statement
{
public:
  /**
   * @brief Parses an ASCII/UNICODE coded SQL statement.
   * @param sql SQL statement to be prepared.
   * @param sqlLength Length in bytes of the SQL statement.
   * @param encoding Character code of the SQL statement.
   *   - #SQLDBC_OK if successful,
   *   - #SQLDBC_NOT_OK otherwise, in which case the error has been set,
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode prepare(const char *sql,
                         const SQLDBC_Length sqlLength,
                         const SQLDBC_StringEncoding encoding);

  /**
   * @brief Parses an ASCII/UNICODE coded zero-terminated SQL statement.
   *
   * @param sql The zero-terminated string containing the SQL statement to
   * be prepared. The code of the given string depends from the
   * <var>encoding</var> argument.
   * @param encoding The character encoding of the <var>sql</var> argument.
   *   - #SQLDBC_OK if successful,
   *   - #SQLDBC_NOT_OK if parsing of the SQL statement failed
   *     In this case an error is set on this SQLDBC_PreparedStatement object
   */
    /* #methodmap SQLDBC_PreparedStatement_prepareNTS prepare */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode prepare(const char *sql,
                         const SQLDBC_StringEncoding encoding);

  /**
   * @brief Parses a zero-terminated ASCII coded SQL statement.
   * @param sql The zero-terminated string containing the SQL statement to
   * be prepared. The given string must be ASCII coded.
   *   - #SQLDBC_OK if successful,
   *   - #SQLDBC_NOT_OK if parsing of the SQL statement failed
   *     In this case an error is set on this SQLDBC_PreparedStatement object
   */
  /* #methodmap SQLDBC_PreparedStatement_prepareASCII prepare */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode prepare(const char *sql);

  /**
   * @brief Executes a prepared SQL statement.
   *
   * The SQL statement must be prepared beforehand, and the appropriate binding
   * variables must have been set with the bindParameter() member function.
   * @return
   *   - #SQLDBC_OK if successful,
   *   - #SQLDBC_NOT_OK otherwise, In case of an error the SQLDBC_ErrorHndl
   *     object, to be queried using error(), of this object is set.
   *   - #SQLDBC_NEED_DATA if the application is to provide the data using
   *      a nextParameter() - putData() sequence.
   */
  /* #methodmap SQLDBC_PreparedStatement_executeASCII execute */
SQLDBC_DLLEXPORT
  SQLDBC_Retcode execute();

  /**
   * @brief Retrieves an SQLDBC_ParameterMetaData object that contains
   * information about the parameters used.
   *
   * @return An SQLDBC_ParameterMetaData object that contains information
   * about the number, types and properties of the SQLDBC_PreparedStatement
   * object's parameters. If the meta data cannot be retrieved, a NULL pointer
   * is returned
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_ParameterMetaData* getParameterMetaData();

  /**
   * @brief Retrieves an SQLDBC_ResultSetMetaData object that contains
   * information about the columns of the SQLDBC_ResultSet object.
   *
   * The SQLDBC_ResultSetMetaData contains information about the columns
   * of the SQLDBC_ResultSet object that will be returned when the
   * SQLDBC_PreparedStatement object is executed.
   * Because an SQLDBC_PreparedStatement object is parsed, it is possible to
   * get information about the ResultSet object that will be returned without having to
   * execute the SQL statement.
   * Consequently, it is possible to use this method with an
   * SQLDBC_PreparedStatement object rather than executing it and then
   * calling the getResultSetMetaData() method on the returned SQLDBC_ResultSet
   * object.
   *
   * @note Using this method may be expensive due to the time needed to retrieve
   * the metadata from the underlying database kernel.
   *
   * @return An SQLDBC_ResultSetMetaData object that describes the columns or
   * NULL if the meta data cannot be retrieved.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_ResultSetMetaData* getResultSetMetaData();

  /**
   * @brief Sets the size of the row array for a batch execute.
   * @param rowarraysize [in] Number of rows for batch execution.
   * @return #SQLDBC_OK on success, #SQLDBC_NOT_OK otherwise.
   * @see getBatchSize
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode setBatchSize(SQLDBC_UInt4 rowarraysize);

  /**
   * @brief Clears the information about parameters.
   *   All information about parameters that
   *   have been bound before using either the @c bindParameter or
   *   the @c bindParameterAddr method is cleared.
   * @return #SQLDBC_OK on success, #SQLDBC_NOT_OK if the statement
   *   is currently executing a statement having <i>data at execute</i>
   *   parameters.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode clearParameters();


  /**
   * @brief Binds a user supplied memory buffer to an input and/or output
   * parameter of an SQL statement.
   *
   * A parameter is a '?' or ':\<name\>' placeholder in an SQL statement.
   * Applications should use SQLDBC_ParameterMetadata to retrieve
   * information about the type and length of the parameters in an
   * SQL statement.
   *
   * @param Index [in] Index of the parameter, starting with 1
   * @param Type [in] Host type of the parameter
   * @param paramAddr [in] A pointer to a buffer for the parameter's data
   * @param LengthIndicator [in|out] Pointer to parameter length or indicator.
   *    - Input parameter:<br>
   *        For non-integral input parameters, <var>LengthIndicator</var>
   *        must specify the length of the parameter data in bytes, or the
   *        indicator value #SQLDBC_NULL_DATA to assign a NULL value to a
   *        column. The parameter length may also be given by the special
   *        length #SQLDBC_NTS when the buffer points to a zero-terminated character string.
   *    - Output parameter:<br>
   *        For output parameters, <var>LengthIndicator</var> stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Byte length of the parameter
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the non-integral character hostvar types
   *        (#SQLDBC_HOSTTYPE_ASCII, #SQLDBC_HOSTTYPE_UCS2 or #SQLDBC_HOSTTYPE_UTF8).
   * @return #SQLDBC_OK always
   * @todo Implement various error checks
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode bindParameter (const SQLDBC_UInt2     Index,
                                const SQLDBC_HostType  Type,
                                void                  *paramAddr,
                                SQLDBC_Length         *LengthIndicator,
                                const SQLDBC_Length    Size,
                                const SQLDBC_Bool      Terminate=SQLDBC_TRUE);

  /**
   * @brief Binds an input and/or output parameter <i>by address</i>.
   *
   * Binding by adress is useful when the application uses buffers that are
   * allocated as an array of pointers (C function calloc)
   *
   * A parameter is a '?' or ':\<name\>' placeholder in an SQL statement.
   * Applications should use SQLDBC_ParameterMetadata to retrieve
   * information about the type and length of the parameters in a
   * SQL statement.
   *
   * @param Index [in] Index of the parameter, starting with 1.
   * @param Type [in] Host type of the parameter
   * @param paramAddr [in] Pointer to <i>a pointer</i> for the parameter's
   *        data.
   * @param LengthIndicator [in|out] Pointer to parameter length or indicator.
   *    - Input parameter:<br>
   *        For non-integral input parameters, <var>LengthIndicator</var>
   *        must specify the length of the parameter data in bytes, or the
   *        indicator value #SQLDBC_NULL_DATA to assign a NULL value to a
   *        column. The parameter length may also be given by the special
   *        length #SQLDBC_NTS when the buffer points to a zero-terminated
   *        character string.
   *    - Output parameter:<br>
   *        For output parameters, <var>LengthIndicator</var> stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Byte length of the parameter
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the non-integral character hostvar types
   *        (#SQLDBC_HOSTTYPE_ASCII, #SQLDBC_HOSTTYPE_UCS2 or #SQLDBC_HOSTTYPE_UTF8).
   * @return #SQLDBC_OK always
   * @todo Implement various error checks
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode bindParameterAddr(const SQLDBC_UInt2    Index,
                                   const SQLDBC_HostType Type,
                                   void                 *paramAddr,
                                   SQLDBC_Length        *LengthIndicator,
                                   const SQLDBC_Length   Size,
                                   const SQLDBC_Bool     Terminate=SQLDBC_TRUE);

  /**
   * @brief Sets the binding type for parameters.
   *
   * The value of the parameter offset is normally 0, which implies a
   * <i>column-wise</i> binding. If it is set to a value other than
   * 0, a <i>row-wise</i> binding is applied.
   * So, the address offset of the respective next parameter value is computed differently for <i>column-wise</i> and <i>row-wise</i> binding:
   *  @par Column-wise Binding
   *   byte length of the parameter
   *  @par Row-wise Binding
   *   <var>size</var>
   * @param size The parameter offset for row-wise binding, if set to 0, column-wise
   * binding is applied.
   * @return #SQLDBC_OK always.
   * @see bindParameterAddr
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode setBindingType(SQLDBC_size_t size);

  /**
   * @brief Switches to the next input parameter if the application
   * uses <i>data at execute</i> parameters.
   *
   * After calling this member function, the <var>paramIndex</var> and the <var>paramAddr</var>
   * parameters are set.
   *
   * @deprecated Use @c nextParameterByIndex instead.
   *
   * @param paramIndex Index of the next parameter.
   * @param paramAddr The data pointer that was supplied when binding the parameter, plus any offset that
   *        originates from a batch execution sequence (including row-wise binding).
   * @return
   *         - #SQLDBC_NEED_DATA if <var>paramIndex</var> and <var>paramAddr</var> contain
   *           the index or address of the next parameter that will be delivered using
   *           putData().
   *         - #SQLDBC_OK if the last parameter has been processed, and the statement was executed successfully.
   *         - #SQLDBC_NOT_OK if there is an error (in this case, the error of the SQLDBC_PreparedStatement is set).
   * @see putData
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode nextParameter(SQLDBC_Int2& paramIndex, void*& paramAddr);

  /**
   * @brief Switches to the input parameter if the application
   * uses <i>data at execute</i> parameters,
   *
   * After calling this member function, the <var>paramIndex</var> and the <var>paramAddr</var>
   * parameters are set. The <var>paramIndex</var> parameter is used to select the next parameter
   * that is processed. You cannot process parameters that are bound to LONG columns
   * before all parameters that are bound to non-LONG columns have been processed for a row.
   * @param paramIndex Index of the next parameter that shall be processed, or 0 if the application
   *                   has no preferred next parameter.
   * @param paramAddr The data pointer that was supplied when binding the parameter, plus any offset that
   *        originates from a batch execution sequence (including row-wise binding).
   * @return
   *         - #SQLDBC_NEED_DATA if <var>paramIndex</var> and <var>paramAddr</var> contain
   *           the index or address of the next parameter that will be delivered using
   *           putData().
   *         - #SQLDBC_OK if the last parameter has been processed, and the statement was executed successfully.
   *         - #SQLDBC_NOT_OK if there is an error (in this case, the error of the SQLDBC_PreparedStatement is set).
   * @see putData
   */
   SQLDBC_DLLEXPORT
   SQLDBC_Retcode nextParameterByIndex(SQLDBC_Int2& paramIndex, void*& paramAddr);


  /**
   * @brief Put data for an input parameter.
   *
   * @param paramAddr Pointer to the data which is to be assigned to
   * the SQL parameter. The type of the data must match the type assigned
   * by the bindParameter() member function.
   * @param paramLengthIndicator [in] Pointer to parameter length or indicator.
   * @return
   *   - #SQLDBC_OK if successful,
   *   - #SQLDBC_NOT_OK otherwise, in which case the error has been set,
   * @see nextParameter
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode putData(void *paramAddr, SQLDBC_Length  *paramLengthIndicator);

  /**
   * @brief Retrieves and converts the value of the specified output column
   * of the current row to a buffer.
   *
   * The specified column value in the current row of this SQLDBC_PreparedStatement
   * object is converted to the given length and SQLDBC_HostType and written
   * to the output parameter buffer pointed
   * to <var>paramAddr</var>.
   *
   * It can be called multiple times to retrieve character or binary data in
   * parts. For fixed-length datatypes getObject retrieves the same data
   * multiple times. Mixing variable-length datatypes and fixed-length
   * datatypes may produce unexpected results.
   *
   * @param Index Index of the column.
   *              The first column is column number 1, the second is column
   *        number 2, ...
   * @param Type Parameter type of the output buffer.
   * @param paramAddr A pointer to the parameters output buffer.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only necessary for
   *        non-integral data types. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the host var type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success
   *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
   *         #SQLDBC_NOT_OK if a database access or conversion
   *         error occurred. In this case an error is set on this
   *         SQLDBC_PreparedStatement object.
   *
   * @see bindColumn
   **/
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode getObject(const SQLDBC_Int4     Index,
                           const SQLDBC_HostType Type,
                           void                 *paramAddr,
                           SQLDBC_Length        *LengthIndicator,
                           const SQLDBC_Length   Size,
                           const SQLDBC_Bool     Terminate=SQLDBC_TRUE);


  /**
   * @brief Retrieves and converts the value with an start offset in
   * of the specified output column from a of the current row to a buffer.
   *
   * The specified column value in the current row of this SQLDBC_PreparedStatement
   * object is converted to the given length and SQLDBC_HostType and written
   * to the output parameter buffer pointed
   * to <var>paramAddr</var>.
   *
   * It can be called multiple times to retrieve character or binary data in
   * parts. For fixed-length datatypes getObject retrieves the same data
   * multiple times. Mixing variable-length datatypes and fixed-length
   * datatypes may produce unexpected results.
   *
   * @param Index Index of the column.
   *              The first column is column number 1, the second is column
   *        number 2, ...
   * @param Type Parameter type of the output buffer.
   * @param paramAddr A pointer to the parameters output buffer.
   * @param LengthIndicator [out] Pointer to a variable that stores the
   *        column length or the indicator value #SQLDBC_NULL_DATA if the
   *        column contains the NULL value.
   *        For character data it contains on success the number of bytes
   *        copied to the buffer, except the number of bytes necessary for
   *        the zero-terminator, if the <var>Terminate</var> flag was set.
   *        If the source string exceeds the <var>Size</var> value
   *        #SQLDBC_DATA_TRUNC will be returned and
   *        <var>LengthIndicator</var> is set to the number of bytes
   *        (except the terminator bytes) needed to copy without truncation.
   * @param Size [in] Length of the parameter buffer in bytes.
   *        The <var>Size</var> argument is only necessary for
   *        non-integral data types. For character data the <var>Size</var>
   *        argument must be large enough to store the terminator byte(s) if
   *        the <var>Terminate</var> flag is set.
   * @param StartPos [in] Start position in long column from which on the
   *        data should be retrieved. Start position is counted in bytes from 1.
   *        Negative <var>StartPos</var> counts from the end of the long column.
   * @param Terminate [in] Specifies that the output buffer must be finished
   *        with a C-style zero-terminator. The <var>Terminate</var> flag
   *        works only for the host var type character (ASCII, UCS2 or UTF8).
   *        As a default, all character data is zero-terminated.
   *
   * @return #SQLDBC_OK on success
   *         #SQLDBC_DATA_TRUNC if the output buffer was too small.
   *         #SQLDBC_NOT_OK if a database access or conversion
   *         error occurred. In this case an error is set on this
   *         SQLDBC_PreparedStatement object.
   *
   * @see bindColumn
   **/
   /* #methodmap SQLDBC_PreparedStatement_getObjectByPos getObject */
 SQLDBC_DLLEXPORT
  SQLDBC_Retcode getObject(const SQLDBC_Int4     Index,
                           const SQLDBC_HostType Type,
                           void                 *paramAddr,
                           SQLDBC_Length        *LengthIndicator,
                           const SQLDBC_Length   Size,
                           SQLDBC_Length         StartPos,
                           const SQLDBC_Bool     Terminate=SQLDBC_TRUE);


private:
  friend class SQLDBC_Connection;
  /**
   * @brief Constructor for a SQLDBC_PreparedStatement.
   *
   * This constructor should never called directly. Functions should use
   * SQLDBC_Connection::createPreparedStatement() to create a new
   * SQLDBC_PreparedStatement object.
   * @param stmt The reference to internal used IFR__PreparedStatement.
   */
  SQLDBC_PreparedStatement(IFR_PreparedStmt *stmt);
  ~SQLDBC_PreparedStatement();
  SQLDBC_PreparedStatementStorage *m_pcstmt;
};

  /**
   * @brief Specification of connection options.
   *
   * Together with the command for opening a database session you can set
   * several options. The following options are possible
   *
   * <table border="1">
   *   <tr>
   *     <td><b>Option</b></td><td><b>Data Type</b></td><td><b>Description</b></td>
   *   </tr>
   *   <tr>
   *     <td><code>COMPNAME</code></td>
   *     <td><code>VARCHAR(64)</code></td>
   *     <td>The component name used to initialise the runtime environment.
   *         If it is a runtime library, the name must start with an "R".
   *         Otherwise the system considers it an application.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>APPLICATION</code></td>
   *     <td><code>CHAR(3)</code></td>
   *     <td>The application to be connected to the database. Possible values are: CAL | CPC | CON | DOM | LOA | ODB | QUE | SQL | TPL| UTI| XCI
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>APPVERSION</code>
   *     <td><code>NUMBER(5)</code>
   *     <td>Version of the application. This is computed:
   *         major * 10000 + minor * 100 + correction_level.
   *         E.g. version 7.4.04  has the version identifier "70404".
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>SQLMODE</code>
   *     <td><code>VARCHAR</code>
   *     <td>The SQL Mode. Possible values are: INTERNAL | ORACLE | ANSI | DB2.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>UNICODE</code>
   *     <td><code>BOOLEAN</code>
   *     <td>TRUE | 1, the connection is an unicode (UCS2) client or FALSE | 0 it is an ASCII client.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>TIMEOUT</code>
   *     <td><code>NUMBER</code>
   *     <td>The maximum allowed time of inactivity after which the connection to the database is closed by the system.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>ISOLATIONLEVEL</code>
   *     <td><code>NUMBER</code>
   *     <td>The ISOLATION LEVEL specifies whether and how shared locks and exclusive locks are implicitly requested or released. For possible values see: setTransactionIsolation()
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>PACKETCOUNT</code></td>
   *     <td><code>NUMBER | STRING</code></td>
   *     <td>The number of different request packets used for the connection.
   *         Enter a positive number or UNLIMITED for an unlimited number of
   *         request packets
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>STATEMENTCACHESIZE</code></td>
   *     <td><code>NUMBER | STRING </code></td>
   *     <td>The number of prepared statements to be cached for the connection for re-use. Possible values are: \<n\>: desired number of statements 0: no statements are cached UNLIMITED: unlimited number of statements is cached.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td><code>CURSORPREFIX</code></td>
   *     <td><code>STRING</code></td>
   *     <td>The prefix to use for result tables that are automatically named.</td>
   *   </tr>
   * </table>
   */
class SQLDBC_ConnectProperties
{
public:
  /**
   * @brief Creates an empty set of options. A default allocator is
   * used.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_ConnectProperties();

  /**
   * @brief The copy constructor.
   * @param copy The options be copied.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_ConnectProperties(const SQLDBC_ConnectProperties& copy);

  /**
   * @brief The destructor.
   */
  SQLDBC_DLLEXPORT
  ~SQLDBC_ConnectProperties();


  /**
   * @brief Sets an option.
   * @param key The option to be set (ASCII string)
   * @param value The option value to be set (ASCII string)
   */
  SQLDBC_DLLEXPORT
  void setProperty(const char *key, const char *value);

  /**
   * @brief Retrieves an option.
   * @param key The option key to be requested
   * @param defaultvalue The value to be returned. If the option is not found the default value is returned.
   * @return The value found in the options set, or the default value specified.
   */
  SQLDBC_DLLEXPORT
  const char *getProperty(const char *key, const char *defaultvalue=0) const;

private:
  friend class SQLDBC_Connection;
  IFR_ConnectProperties *m_prop;
};


/**
 * @brief Connection to the database kernel.
 *
 * This class combines the functions necesary for managing a connection
 * to the database. If a new connection object is created, it is not connected.
 * The connection to the database is established when a successful connect() call was performed.
 *
 * @todo Check more parameters after connection (version).
 * @todo Distribute the results of a COMMIT or ROLLBACK statment into dependent objects (for correct handling of PUTVAL commands).
 */
class SQLDBC_Connection
  : public SQLDBC_ConnectionItem
{
public:
  /**
   * @brief Opens a new database session using an explicit URL and
   * a CONNECT statement.
   *
   * This method should be used only for custom connections together
   * with a custom implementation of the SQLDBC_IRuntime interface.
   * @param connectURL The connect URL, which may be 0.
   * @param connectURLLength The length of the connect URL, which may be
   * #SQLDBC_NTS to indicate a zero-terminated string.
   * @param connectCommand The CONNECT statement.
   * @param connectCommandLength The length of the CONNECT statement, which may
   * be #SQLDBC_NTS to indicate a zero-terminated string.
   * @param connectCommandEncoding The code of the connect statement.
   * @param connectProperties Connect properties. It is recommended that you
   * set environmental options, such as SQL mode, component name and
   * version, to correctly initialize the internal handling of the runtime,
   * since neither the URL nor your CONNECT statement are parsed fetch these
   * options.
   *
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const char *connectURL,
                         SQLDBC_Length connectURLLength,
                         const char *connectCommand,
                         SQLDBC_Length connectCommandLength,
                         SQLDBC_StringEncoding connectCommandEncoding,
                         SQLDBC_ConnectProperties& connectProperties);

  /**
   * @brief Opens a new database session using the default values for the
   * options.
   * @param servernode The name of the database server (ASCII).
   * @param servernodeLength The length in bytes of the database server name.
   * @param serverdb The name of the database instance (ASCII).
   * @param serverdbLength The length in bytes of the database instance name.
   * @param username The name of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param usernameLength The length in bytes of the database user name.
   * @param password The password of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param passwordLength The length in bytes of the database users password.
   * @param userpwdEncoding The character encoding of the <var>username</var>/<var>password</var> combination.
   *
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const char *servernode,
                         SQLDBC_Length servernodeLength,
                         const char *serverdb,
                         SQLDBC_Length serverdbLength,
                         const char *username,
                         SQLDBC_Length usernameLength,
                         const char *password,
                         SQLDBC_Length passwordLength,
                         const SQLDBC_StringEncoding userpwdEncoding);

  /**
   * @brief Opens a new database session using the default values for the options.

   * This method interprets all character arguments as zero-terminated ASCII
   * strings.
   * User name and password must be in <var>userpwdEncoding</var> code.
   * @param servernode The zero-terminated name of the database server (ASCII).
   * @param serverdb The zero-terminated name of the database instance (ASCII).
   * @param username The zero-terminated name of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param password The zero-terminated password of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param userpwdEncoding The character encoding of the <var>username</var>/<var>password</var> combination.
   *
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const char *servernode,
                         const char *serverdb,
                         const char *username,
                         const char *password,
                         const SQLDBC_StringEncoding userpwdEncoding);

  /**
   * @brief Opens a new database session using the default values for the options.

   * The method accepts all character arguments as zero-terminated ASCII
   * strings.
   * @param servernode The zero-terminated name of the database server (ASCII).
   * @param serverdb The zero-terminated name of the database instance (ASCII).
   * @param username The zero-terminated name of the database user (ASCII).
   * @param password The zero-terminated password of the database user (ASCII).
   *
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const char *servernode,
                         const char *serverdb,
                         const char *username,
                         const char *password);

  /**
   * @brief Opens a new database session using the SQLDBC_ConnectProperties object.
   *
   * For the connection options not specified, the following
   * default values are used:
   * <dl>
   *   <dt><code>COMPNAME</code></dt><dd>"R ifr80"</dd>
   *   <dt><code>APPLICATION</code></dt><dd>"ODB"</dd>
   *   <dt><code>APPVERSION</code></dt><dd>"70404"</dd>
   *   <dt><code>UNICODE</code></dt><dd>false</dd>
   *   <dt><code>SQLMODE</code></dt><dd>INTERNAL</dd>
   * </dl>
   * @param servernode The name of the database server (ASCII).
   * @param servernodeLength The length in bytes of the database server name.
   * @param serverdb The name of the database instance (ASCII).
   * @param serverdbLength The length in bytes of the database instance name.
   * @param username The name of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param usernameLength The length in bytes of the database user name.
   * @param password The password of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param passwordLength The length in bytes of the database users password.
   * @param userpwdEncoding The character encoding of the <var>username</var>/<var>password</var> combination.
   * @param properties Further connection properties.
   *
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const char* servernode,
                         SQLDBC_Length servernodeLength,
                         const char* serverdb,
                         SQLDBC_Length serverdbLength,
                         const char* username,
                         SQLDBC_Length usernameLength,
                         const char* password,
                         SQLDBC_Length passwordLength,
                         const SQLDBC_StringEncoding userpwdEncoding,
                         const SQLDBC_ConnectProperties& properties);

  /**
   * @brief Opens a new database session using the SQLDBC_ConnectProperties object.
   *
   * This method interpres all character arguments as zero-terminated ASCII
   * strings.
   * User name and password must be in <var>userpwdEncoding</var> code.
   * @param servernode The zero-terminated name of the database server (ASCII).
   * @param serverdb The zero-terminated name of the database instance (ASCII).
   * @param username The zero-terminated name of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param password The zero-terminated password of the database user.
   * The encoding of this string depends on the <var>userpwdEncoding</var>
   * argument.
   * @param userpwdEncoding The character encoding of the <var>username</var>/<var>password</var> combination.
   * @param properties Further connection properties.
   *
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
   /* #methodmap SQLDBC_Connection_connectNTS connect */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const char* servernode,
                         const char* serverdb,
                         const char* username,
                         const char* password,
                         const SQLDBC_StringEncoding userpwdEncoding,
                         const SQLDBC_ConnectProperties& properties);

  /**
   * @brief Opens a new database session using the SQLDBC_ConnectProperties object.
   *
   * This method interprets all character arguments as zero-terminated ASCII
   * strings.
   * @param servernode The zero-terminated name of the database server (ASCII).
   * @param serverdb The zero-terminated name of the database instance (ASCII).
   * @param username The zero-terminated name of the database user (ASCII).
   * @param password The zero-terminated password of the database user (ASCII).
   * @param properties Further connection properties.
   *
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  /* #methodmap SQLDBC_Connection_connectASCII connect */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const char* servernode,
                         const char* serverdb,
                         const char* username,
                         const char* password,
                         const SQLDBC_ConnectProperties& properties);


  /**
   * @brief Opens a new database session using the runtime abstraction
   * interface.
   *
   * This method attaches the connection object an existing session.
   * It is recommended that you implement the virtual method getSession and releaseSession
   * in the Runtime Abstraction Interface.
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect();

  /**
   * @brief Opens a new database session using the runtime abstraction
   * interface.
   *
   * This method attaches the connection object an existing session.
   * It is recommended that you implement the virtual method getSession and releaseSession
   * in the Runtime Abstraction Interface.
   * @param properties Connection properties.
   * @return #SQLDBC_OK if the connection to the database was established,
   *   #SQLDBC_NOT_OK if the connection failed.
   *   In this case an error is set on this SQLDBC_Connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode connect(const SQLDBC_ConnectProperties& properties);


  /**
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
  SQLDBC_DLLEXPORT
  SQLDBC_Statement *createStatement();

  /**
   * @brief Creates an SQLDBC_PreparedStatement object for sending SQL statements
   * to the database.

   * An SQLDBC_PreparedStatement object is 'prepared' in the database server and can use binding variables for input/output parameters.
   * @return The SQLDBC_PreparedStatement object created by this method.
   * @see SQLDBC_PreparedStatement
   */
  SQLDBC_DLLEXPORT
  SQLDBC_PreparedStatement *createPreparedStatement();

  /**
   * @brief Commits all changes to the database session.
   *
   * All changes made since the previous COMMIT/ROLLBACK statement
   * are stored, any database locks held by
   * this SQLDBC_Connection object are released.
   * @return #SQLDBC_OK if the COMMIT statement was successfull,
   *   #SQLDBC_NOT_OK if the COMMIT statment failed, because of an error or timeout.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode commit();

  /**
   * @brief Undoes all changes made in the current transaction and releases
   * any database locks held by this connection object.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode rollback();

  /**
   * @brief Releases an SQLDBC_Statement.
   * @param stmt A reference to an SQLDBC_Statement object to be released.
   */
  SQLDBC_DLLEXPORT
  void releaseStatement(SQLDBC_Statement *stmt);

  /**
   * @brief Releases an SQLDBC_PreparedStatement.
   * @param stmt A reference to an SQLDBC_PreparedStatement object to be released.
   */
  SQLDBC_DLLEXPORT
  void releaseStatement(SQLDBC_PreparedStatement *stmt);

  /**
   * @brief Cancels the running database request that is executed
   * on the connection. For cancellation of a running SQL command it is
   * neccessary to call the cancel method asynchronously.
   * Because of the asynchronous processing it is not guaranteed that the
   * cancellation will be succeed. The returncode of the original function
   * indicates whether the function completed or was cancelled. In case of
   * cancelation the original function returns with the sql code
   * -102 'SQL statement cancelled'. The the cancel methode returns with
   * SQLDBC_OK if it has been send the cancel request to the database.
   * @return #SQLDBC_OK When the request to cancel the database session was sent,
   *   #SQLDBC_NO_DATA_FOUND if there was no database session to be cancelled.
   * @note This is an UTF8 string.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode cancel();

  /**
   * @brief Closes the SQLDBC_Connection.

   * Releases the database and interface runtime resources immediately instead
   * of waiting for them to be released by the destructor. The current transaction
   * is rolled back.
   *
   * After you called this method, you are able to connect again using
   * one of the <code>connect</code> methods.
   *
   * @return #SQLDBC_OK When the resources are released.
   *   #SQLDBC_NOT_OK if there was an error occured.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode close();


  /**
   * @brief Closes the SQLDBC_Connection.

   * Releases the database and interface runtime resources immediately instead
   * of waiting for them to be released by the destructor. The current
   * transaction is committed.
   *
   * @return #SQLDBC_OK When the resources are released and the transaction
   * is committed. #SQLDBC_NOT_OK if there was an error occured.
   *
   * After you called this method, you are able to connect again using
   * one of the <code>connect</code> methods.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode commitWorkRelease();


  /**
   * @brief Closes the SQLDBC_Connection.

   * Releases the database and interface runtime resources immediately instead
   * of waiting for them to be released by the destructor. No SQL command that
   * handles the current transaction is sent to the database, so the transaction
   * may be implicitely  rolled back by the database server.
   *
   * After you called this method, you are able to connect again using
   * one of the <code>connect</code> methods.
   *
   * @return #SQLDBC_OK When the resources are released.
   * #SQLDBC_NOT_OK if there was an error occured.
   *
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode disconnect();


  /**
   * @brief Sets the AUTOCOMMIT mode for the connection.
   * @param autocommit The value for AUTOCOMMIT mode.
   * @see getAutoCommit
   */
  SQLDBC_DLLEXPORT
  void setAutoCommit(SQLDBC_Bool autocommit);

  /**
   * @brief Returns whether the connection is in AUTCOMMIT mode.
   * @return #SQLDBC_TRUE if the connection is in AUTOCOMMIT mode or
   *         #SQLDBC_FALSE if it is not.
   * @see setAutoCommit
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Bool getAutoCommit() const;

  /**
   * @brief Sets the SQL mode.
   * @param sqlmode The SQL mode to be used:
   * #SQLDBC_INTERNAL |
   * #SQLDBC_ANSI |
   * #SQLDBC_DB2 |
   * #SQLDBC_ORACLE
   * @see SQLDBC_ConnectProperties
   */
  SQLDBC_DLLEXPORT
  void setSQLMode(SQLDBC_SQLMode sqlmode);

  /**
   * @brief Sets the transaction isolation level.
   * @param isolationlevel The isolation level to be used.
   * Possible values for the isolation level are 0, 1, 10, 15, 2, 20, 3, or 30.
   * The lowest level is 0.
   * @return #SQLDBC_OK if the isolation level was set,
   *   #SQLDBC_NOT_OK if the isolation level was not set,
   *   In this case an error is set on this SQLDBC_Connection object.
   * @see getTransactionIsolation
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Retcode setTransactionIsolation(SQLDBC_Int4 isolationlevel);

  /**
   * @brief Returns the transaction isolation level.
   * @return The isolation level (0, 1, 10, 15, 2, 20, 3, or 30).
   * The lowest level is 0.
   * @see setTransactionIsolation
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getTransactionIsolation() const;

  /**
   * @brief Returns whether if the connection to the database was established.
   * @return #SQLDBC_TRUE if the connection was
   *   established. It is not checked whether
   *   the connection timed out or the
   *   database server is still running.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Bool isConnected() const;

  /**
   * @brief Returns the kernel version.
   *
   * It will be returned in the following
   * format:
   *   \<major_release\>\<minor_releaser\>\<correction_level\>
   *   - mayor_release (1 digit)
   *   - minor_release (2 digit)
   *   - correction_level (2 digits)
   * @return The kernel version, which is a computed \<major_release\> * 10000 +
   * \<minor_release\> * 100 + \<correction_level\>.  Example: For version 7.4.04 version number 70404 is returned.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Int4 getKernelVersion() const;

  /**
   * @brief Returns whether the database is an unicode database or
   * not.
   * @return #SQLDBC_TRUE if the database is an unicode database.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Bool isUnicodeDatabase() const;

  /**
   * @brief Returns the currently active date/time format.
   * @return The date/time format. See SQLDBC_DateTimeFormat::Format
   */
  SQLDBC_DLLEXPORT
  SQLDBC_DateTimeFormat::Format getDateTimeFormat() const;

private:
  friend class SQLDBC_Environment;
  /**
   * @brief Constructor. Applications cannot call this constructor directly.
   * It is called by the
   * SQLDBC_Environment::createConnection method of the SQLDBC_Environment
   * class.
   */
  SQLDBC_Connection(IFR_Connection *conn);
};

/**
 * @brief The SQLDBC_Environment object is the basis for initializing the SQLDBC runtime library and making general settings in the SQLDBC interface.
 */

class SQLDBC_Environment
{
public:
  /**
   * @brief Creates a new SQLDBC_Environment, using the provided interface to
   * the runtime environment.
   * @param runtime The runtime to be used, must not be 0. Implement the
   * interface using SQLDBC_IRuntime or use the SQLDBC::GetClientRuntime()
   * in a client application to obtain a valid argument for this constructor.
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Environment(SQLDBC_IRuntime* runtime);

  /**
   * @brief The destructor.
   */
  SQLDBC_DLLEXPORT
  ~SQLDBC_Environment();

  /**
   * @brief Creates a new SQLDBC_Connection object.
   * @return A reference to a SQLDBC_Connection object.
   * @see releaseConnection
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Connection *createConnection() const;

  /**
   * @brief Creates a new SQLDBC_Connection object.
   * @return A reference to a SQLDBC_Connection object.
   * @see releaseConnection
   */
  SQLDBC_DLLEXPORT
  SQLDBC_Connection *createConnection(SAPDBMem_IRawAllocator& allocator) const;

  /**
   * @brief Closes a SQLDBC_Connection.
   * @param connection The SQLDBC_Connection to be closed.
   * @see createConnection
   */
  SQLDBC_DLLEXPORT
  void releaseConnection(SQLDBC_Connection *connection) const;

  /**
   * @brief Returns the version of used SQLDBC runtime.
   *
   * This is the version of the used SQLDBC library. The version of the loaded
   * runtime may differ from the version inidacted in the used header.
   * It is our aim that newer versions
   * of the runtime will run with old applications without the need to be
   * re-compiling.
   * @return A character string in the
   * format 'libSQLDBC M.M.C    Build 002-000-000-000' containing a complete
   * version string of the (shared) linked SQLDBC library.
   */

  SQLDBC_DLLEXPORT
  const char *getLibraryVersion();

  /**
   * @brief Returns the version of the used SQLDBC API.
   *
   * It corresponds to the version indicated in the used SQLDBC header.
   * @return A character string of the format
   * 'SQLDBC.H  M.M.C    Build 002-000-000-000'
   * containing a complete version string.
   */

  SQLDBC_DLLEXPORT
  inline const char *getSDKVersion()
  const
  {
    return SQLDBCHeaderVersion;
  };

    /**
     * Requests the allocator associated to this connection item.
     */
    SQLDBC_DLLEXPORT SAPDBMem_IRawAllocator&  getAllocator();

private:
    IFR_Environment *m_env;
};

/**
 * @brief Returns a pointer to the client runtime instance.
 *
 * Two consecutive calls will return the
 * same pointer, as there is only one instance.
 * @param errorText If not 0, an occured during initialization of the environment.
 * @param errorTextSize The size in bytes of the <var>errorText</var> buffer.
 * @return A pointer to the client runtime interface or 0 if an error occured.
 */
SQLDBC_DLLEXPORT SQLDBC_IRuntime* GetClientRuntime(char *errorText, const SQLDBC_Int4 errorTextSize);

/**
 * @brief Returns a pointer to the single-threaded client runtime instance.
 *
 * Two consecutive calls will return the
 * same pointer, as there is only one instance.
 * @param errorText If not 0, an occured during initialization of the environment.
 * @param errorTextSize The size in bytes of the <var>errorText</var> buffer.
 * @return A pointer to the client runtime interface or 0 if an error occured.
 */
SQLDBC_DLLEXPORT SQLDBC_IRuntime* GetSingleThreadClientRuntime(char *errorText, const SQLDBC_Int4 errorTextSize);
} // namespace SQLDBC


#endif //SQLDBC_H
