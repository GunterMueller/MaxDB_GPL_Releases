/*!
  @file           IFR_Types.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          Basic types and macros
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
#ifndef IFR_TYPES_H
#define IFR_TYPES_H

#ifdef SQLDBC_FOR_KERNEL
#  define IFR_NAMESPACE SQLDBC_Kernel
#  define SQLDBC SQLDBC_Kernel
#  define SQLDBC_RUNTIME_IN_NAMESPACE
#endif

#include <string.h>
#include <math.h>
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_Types.h"

#ifdef IFR_NAMESPACE
#define IFR_BEGIN_NAMESPACE namespace IFR_NAMESPACE {
#define IFR_END_NAMESPACE };
#define IFR_USE_NAMESPACE using namespace IFR_NAMESPACE;
#else
#define IFR_BEGIN_NAMESPACE
#define IFR_END_NAMESPACE
#define IFR_USE_NAMESPACE
#endif

IFR_BEGIN_NAMESPACE

/** @ingroup IFR_Common
 * @def IFR_NEED_VIRTUAL_DELEGATION
 * This define is set if the the compiler requires that if more than
 * one virtual function is defined that has the same name, but
 * a different signature, either none or all functions need to be
 * redefined in a subclass. This is true for Sun and HP Unix.
 */
#if defined(SUN) || defined(SOLARIS) || defined(HPUX)
#  define IFR_NEED_VIRTUAL_DELEGATION  1
#else
#  define  IFR_NEED_VIRTUAL_DELEGATION 0
#endif

/** @ingroup IFR_Common
 * @def IFR_TYPENAME
 * This define expands to <code>typename</code> or to nothing,
 * depending whether the compiler requires and understands the
 * <code>typename</code> keyword, or forbids it (IA 64).
 */
#if defined(_M_IA64)
/* IA64 does not like 'typename'. */
#  define IFR_TYPENAME
#else
#  define IFR_TYPENAME typename
#endif




/* HPUX defines a MIN and MAX macro in sys/params.h */
#ifndef HPUX
#ifndef MAX
/** @ingroup IFR_Common
 * Definition of maximum computation.
 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
/** @ingroup IFR_Common
 * Definition of minimum computation. */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#endif


/** @ingroup IFR_Common
 * An 1 byte large boolean type for interfacing Pascal code. This is
 * not necessarily equivalent with the C++ type <code>bool</code>.
 */
typedef unsigned char       IFR_Bool1;        /* byte (1 byte, unsigned) */

/** @ingroup IFR_Common
 * The <code>true</code> value for the <code>IFR_Bool1</code> type. */
#define IFR_TRUE          SQLDBC_TRUE
/** @ingroup IFR_Common
 *The <code>false</code> value for the <code>IFR_Bool1</code> type. */
#define IFR_FALSE         SQLDBC_FALSE

#if (defined (AIX) && __IBMCPP__ < 500) || ( (defined (SUN) || defined (SOLARIS)) && !defined(_BOOL) ) \
  || !defined (__cplusplus)
/** @ingroup IFR_Common
 * This is defined if the <code>bool</code> type is unknown to the compiler,
 * which is the case on Sun and some AIX compilers.
 */
# define CPP_BOOL_NOT_IMPLEMENTED
#endif

#ifdef CPP_BOOL_NOT_IMPLEMENTED
#if defined(WIN32) || defined(AIX) || defined(LINUX) || defined(OSF1) \
 || defined(HPUX) || defined(NMP) || defined(SUN) || defined(SOLARIS)
/** @ingroup IFR_Common
 * Definition of the boolean type, this is either the C++ <code>bool</code>
 * type, or if missing a <tt>typedef</tt> to an <tt>unsigned char</tt>.
 */
typedef SQLDBC_Bool IFR_Bool;
/** @ingroup IFR_Common
 * @deprecated
 * Type definition for the 'bool' type as a helper for old sources
 */
typedef IFR_Bool  bool;
#endif
#else
typedef bool IFR_Bool;
#endif /* CPP_BOOL_NOT_IMPLEMENTED */

#ifdef CPP_BOOL_NOT_IMPLEMENTED
/** @ingroup IFR_Common
   The true value with a boolean cast */
# define     true    ((const IFR_Bool)1)
/** @ingroup IFR_Common
   The false value with a boolean cast */
# define     false   ((const IFR_Bool)0)
#endif /* CPP_BOOL_NOT_IMPLEMENTED */

#if defined(_WIN64)
typedef SQLDBC_ULong    IFR_ULong;      /* long (fast, unsigned) */
typedef SQLDBC_Long     IFR_Long;       /* long (fast, signed)   */
#else
/** @ingroup IFR_Common
 * An unsigned long type, which has 64 bit length on 64 bit
 * platforms, and 32 bit length on 32 bit platforms.
 */
typedef SQLDBC_ULong     IFR_ULong;      /* long (fast, unsigned) */
/** @ingroup IFR_Common
 * A signed long type, which has 64 bit length on 64 bit
 * platforms, and 32 bit length on 32 bit platforms.
 */
typedef SQLDBC_Long      IFR_Long;       /* long (fast, signed)   */
#endif

/** @ingroup IFR_Common
 * The memory alignment that is used when new memory is allocated.
 */
#define IFR_ALIGNMENT       (sizeof(void*) + 8)

/** @ingroup IFR_Common
 * The maximum value for a size.
 */
#define IFR_MAX_SIZE_T       (((IFR_ULong)((IFR_Long)(-1))))

/** @ingroup IFR_Common
 * Type definition for an unsigned byte.
 */
typedef SQLDBC_Byte       IFR_Byte;

/** @ingroup IFR_Common
 * Type definitions for an UTF8 character.
 */
typedef IFR_Byte            IFR_UTF8;

/** @ingroup IFR_Common
 * Type definition for a memory size (<tt>size_t</tt>).
 */
typedef SQLDBC_size_t       IFR_size_t;

/** @ingroup IFR_Common
 * Type definition for a memory size (<tt>size_t</tt>).
 */
typedef IFR_Long            IFR_ptrdiff_t;


/** @ingroup IFR_Common
 * Unsigned 1 byte integer. */
typedef SQLDBC_UInt1       IFR_UInt1;
/** @ingroup IFR_Common
 * Signed 1 byte integer. */
typedef SQLDBC_Int1        IFR_Int1;
/** @ingroup IFR_Common
 * Unsigned 2 byte integer. */
typedef SQLDBC_UInt2       IFR_UInt2;
/** @ingroup IFR_Common
 * Signed 2 byte integer. */
typedef SQLDBC_Int2        IFR_Int2;
/** @ingroup IFR_Common
 * Unsigned 4 byte integer. */
typedef SQLDBC_UInt4       IFR_UInt4;
/** @ingroup IFR_Common
 * Signed 4 byte integer. */
typedef SQLDBC_Int4        IFR_Int4;

#if defined(WIN32) || defined(_WIN64)
/** @ingroup IFR_Common
 * Unsigned 8 byte integer. */
typedef SQLDBC_UInt8       IFR_UInt8;
/** @ingroup IFR_Common
 * Signed 8 byte integer. */
typedef SQLDBC_Int8        IFR_Int8;
#else
/** @ingroup IFR_Common
 * Unsigned 8 byte integer. */
typedef SQLDBC_UInt8       IFR_UInt8;
/** @ingroup IFR_Common
 * Signed 8 byte integer. */
typedef SQLDBC_Int8        IFR_Int8;
#endif

/**
 * @ingroup IFR_Common
 * Type definition for the length of certain element.
 * This type is used for indicator/length variables in parameter binding.
 */
typedef SQLDBC_Length      IFR_Length;



/** @ingroup IFR_Common
 *  Standard unsigned integer type of the platform. */
typedef SQLDBC_UInt        IFR_UInt;

/** @ingroup IFR_Common
 * Standard signed integer type of the platform. */
typedef SQLDBC_Int         IFR_Int;

/** @ingroup IFR_Common
 * Maximum value for signed 1 byte integer. */
#define MAX_IFR_INT1  ((IFR_Int1)  127)
/** @ingroup IFR_Common
 * Minimum value for signed 1 byte integer. */
#define MIN_IFR_INT1  ((IFR_Int1) -128)

/** @ingroup IFR_Common
 * Maximum value for unsigned 1 byte integer. */
#define MAX_IFR_UINT1  ((IFR_UInt1) 255)
/** @ingroup IFR_Common
 * Minimum value for unsigned 1 byte integer. */
#define MIN_IFR_UINT1  ((IFR_UInt1) 0)

/** @ingroup IFR_Common
 * Maximum value for signed 2 byte integer. */
#define MAX_IFR_INT2   ((IFR_Int2)32767)
/** @ingroup IFR_Common
 * Minimum value for signed 2 byte integer. */
#define MIN_IFR_INT2   ((IFR_Int2)(-MAX_IFR_INT2 - 1))

/** @ingroup IFR_Common
 * Maximum value for unsigned 2 byte integer. */
#define MAX_IFR_UINT2  ((IFR_UInt2)65535)
/** @ingroup IFR_Common
 * Minimum value for unsigned 2 byte integer. */
#define MIN_IFR_UINT2  ((IFR_UInt2)0)

/** @ingroup IFR_Common
 * Maximum value for signed 4 byte integer. */
#define MAX_IFR_INT4   ((IFR_Int4)2147483647)
/** @ingroup IFR_Common
 * Minimum value for signed 4 byte integer. */
#define MIN_IFR_INT4   ((IFR_Int4)(-MAX_IFR_INT4 - 1))

/** @ingroup IFR_Common
 * Maximum value for unsigned 4 byte integer. */
#define MAX_IFR_UINT4   ((IFR_UInt4)4294967295U)
/** @ingroup IFR_Common
 * Minimum value for unsigned 4 byte integer. */
#define MIN_IFR_UINT4   ((IFR_UInt4)0)

#ifdef LINUX
#define IFR_INT64_CONSTANT(x) x##LL
#define IFR_UINT64_CONSTANT(x) x##ULL
#elif defined(WIN32)
#define IFR_INT64_CONSTANT(x) x##I64
#define IFR_UINT64_CONSTANT(x) x##UI64
#else
/** @ingroup IFR_Common
 * Macro for making a signed 64-bit constant. As this is
 * marked different for different systems, there is a helper
 * macro defined. It appends 'I64', 'L', or 'LL' to the
 * argument, depending on the platform.
 * @param x The constant number.
 */
#define IFR_INT64_CONSTANT(x) x##L
/** @ingroup IFR_Common
 * Macro for making an unsigned 64-bit constant. As this is
 * marked different for different systems, there is a helper
 * macro defined. It appends 'I64', 'L', or 'LL' to the
 * argument, depending on the platform.
 * @param x The constant number.
 */
#define IFR_UINT64_CONSTANT(x) x##UL
#endif

/** @ingroup IFR_Common
 * Maximum value for signed 8 byte integer. */
#define MAX_IFR_INT8   ((IFR_Int8)IFR_INT64_CONSTANT(9223372036854775807))


#if defined(HPUX) && defined(BIT64)
#  define MIN_IFR_INT8  ((IFR_Int8)-9223372036854775808L)
#else
/** @ingroup IFR_Common
 * Minumum value for signed 8 byte integer. */
#  define MIN_IFR_INT8   ((IFR_Int8)IFR_INT64_CONSTANT(-9223372036854775808))
#endif

/** @ingroup IFR_Common
 * Maximum value for an unsigned 8 byte integer. */
#define MAX_IFR_UINT8  ((IFR_UInt8)IFR_UINT64_CONSTANT(18446744073709551615))
/** @ingroup IFR_Common
 * Minimum value for an unsigned 8 byte integer. */
#define MIN_IFR_UINT8  ((IFR_UInt8)IFR_UINT64_CONSTANT(0))

/** @ingroup IFR_Packet
 * Type definition for the crypted password for the connect
 * operation.
 */
typedef IFR_Byte IFR_CryptPW[24];

/* detect whether a number is NAN or +/-INF. */
#if defined(WIN32)
#  include <float.h>
/** @ingroup IFR_Common
 * Gets whether the tested <code>double</code> is <code>NaN</code>.
 * @param x The value to check.
 * @return A true value, if the checked value is <code>NaN</code>.
 */
#  define ISNAN(x) _isnan(x)

/** @ingroup IFR_Common
 * Gets whether the tested <code>double</code> is <code>+/-INF</code>.
 * @param x The value to check.
 * @return A true value, if the checked value is <code>INF</code>.
 */
#  define ISINF(x) (!_finite(x))
#elif defined(LINUX) || defined(HPUX)
#  define ISNAN(x) isnan(x)
#  define ISINF(x) isinf(x)
#elif defined(SUN) || defined(SOLARIS) || defined(SNI)
#include <ieeefp.h>
#  define ISNAN(x) isnan(x)
#  define ISINF(x) (!finite(x))
#elif defined(OSF1) || defined(AIX)
#  define ISNAN(x) isnan(x)
#  define ISINF(x) (!finite(x))
#else
#error Platform not supported.
#endif

/** @ingroup IFR_Packet
 * The length of a parse id in bytes.
 */
const int IFR_ParseID_Size = 12;


/** @ingroup IFR_Packet
 * A parse id is returned by the database server after preparing
 * or parsing a command, and refers to this command.
 *
 */
class IFR_ParseID
{
public:
    /**
     * Default constructor. Creates an invalid parse id.
     */
    inline IFR_ParseID()
    {
        memset(data, 0, IFR_ParseID_Size);
        connectCount = -1;
    }

    /**
     * Copy constructor.
     * @param parseid The parse id to copy.
     */
    inline IFR_ParseID(const IFR_ParseID& parseid)
    :connectCount(parseid.connectCount)
    {
        memcpy(data, parseid.data, IFR_ParseID_Size);
    }

    /**
     * Assignment operator. Copies the data from a
     * memory area.
     * @param d Pointer to the data of the parse id.
     * @return <code>*this</code>.
     */
    inline IFR_ParseID &operator=(const unsigned char *d)
    {
        memcpy(data, d, IFR_ParseID_Size);
        return *this;
    }

    /**
     * Changes forcibly the parse id.
     * @param d Pointer to the parse id data.
     */
    inline void setParseID(const unsigned char *d)
    {
        memcpy(data, d, IFR_ParseID_Size);
    }

    /**
     * Gets the parse id data.
     * @return A pointer to a buffer of <code>IFR_ParseID_Size</code> length.
     */
    inline const unsigned char *getParseID() const
    {
        return data;
    }

    /**
     * Gets the connection id that is stored in the parse id,
     * that is, the 32 bit integer stored in the first 4 bytes.
     */
    inline IFR_Int4 getConnectionID() const
    {
#if defined(WIN32) || defined(LINUX)
        return *((const IFR_Int4 *)data);
#else
        IFR_Int4 tmp;
        memcpy(&tmp, data, 4);
        return tmp;
#endif
    }

    /**
     * Assignment operator.
     * @param parseid The other parse id.
     * @return <code>*this</code>
     */
    inline IFR_ParseID& operator =(const IFR_ParseID& parseid)
    {
        if(&parseid != this) {
            memcpy(data, parseid.data, IFR_ParseID_Size);
            connectCount = parseid.connectCount;
        }
        return *this;
    }

    /**
     * Returns whether this parse id is a mass command.
     * @return <code>true</code> is the next-to-last byte has
     * <code>mass_statement</code> set.
     */
    inline IFR_Bool isMassCommand() const
    {
        return data[10] >= 70;
    }
    
    /**
     * Returns whether the command is already executed.
     * @return @c true if the command was already executed
     *   during parsing.
     */
    inline IFR_Bool isExecuted() const
    {
        return data[10] == 1;
    }

    /**
     * Modifies the 'next-to-last' byte of the parse
     * id so that a parse id of a mass command will
     * again represent the single command.
     */
    inline void setSingleCommand()
    {
        if(data[10] >= 70) {
            data[10] -=70;

        }
    }

    /**
     * Invalidates the parse id by deleting the connection id.
     */
    inline void invalidate()
    {
        memset(data, 0, 4);
    }

    /**
     * Checks that the parse id is valid, i.e. the connection id is not 0.
     * It will also check whether a connection is the same as the one that was
     * used when parsing the command, if a @c connectcount is provided.
     * @param connectcount The connect count of the current connection.
     * @return <code>true</code> if the parse id is valid, <code>false</code>
     *         otherwise.
     */
    inline IFR_Bool isValid(IFR_Int4 connectcount=-1) const
    {
        return (getConnectionID() != 0) && (connectcount==-1 || connectcount == this->connectCount);
    }

    /**
     * Sets the connect count.
     * @param count The connect count value.
     */
    inline void setConnectCount(IFR_Int4 count)
    {
        connectCount = count;
    }

    /**
     * Retrieve the connect count.
     * @return The connect count, or
     *         -1 if the connect count is not set.
     */
    inline IFR_Int4 getConnectCount() const
    {
        return connectCount;
    }

    
private:
    unsigned char data[IFR_ParseID_Size]; //!< Parse ID data.
    IFR_Int4      connectCount;
};

class IFR_TraceStream;

/**
 * @ingroup IFR_Trace
 * Operator that traces a parse id. A parse id is traced as hex dump, grouped
 * every 4 bytes, like:
 * <pre>ABD500F4 56A57E4F 000067A1</pre>.
 * @param s The output stream.
 * @param p The parse id.
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_ParseID& p);

/** @ingroup IFR_Statement
 * Indicator of NULL data in input/output.
 */
#define IFR_NULL_DATA SQLDBC_NULL_DATA

/** @ingroup IFR_Statement
 * Indicator that the data and length will be present only at execute.
 */
#define IFR_DATA_AT_EXEC SQLDBC_DATA_AT_EXEC

/** @ingroup IFR_Statement
 * Indicator flag / 'Length' of a null-terminated string.
 */
#define IFR_NTS       SQLDBC_NTS

/** @ingroup IFR_Statement
 * Indicator flag that the driver cannot determine the total number of bytes.
 */
#define IFR_NO_TOTAL  SQLDBC_NO_TOTAL

/** @ingroup IFR_Statement
 * Indicator flag that the default parameter shall be used.
 */
#define IFR_DEFAULT_PARAM  SQLDBC_DEFAULT_PARAM

/** @ingroup IFR_Statement
 * Indicator flag that the value shall be ignored for output.
 */
#define IFR_IGNORE  SQLDBC_IGNORE

/**
 * @ingroup IFR_Statement
 * Offset for IFR_LEN_DATA_AT_EXEC.
 */
#define IFR_LEN_DATA_AT_EXEC_OFFSET SQLDBC_LEN_DATA_AT_EXEC_OFFSET

/**
 * @ingroup IFR_Statement
 * Macro for late binding.
 * @param length The length that the parameter will have. This is ignored.
 */
#define IFR_LEN_DATA_AT_EXEC(length) SQLDBC_LEN_DATA_AT_EXEC(length)

/** @ingroup IFR_Common
 * Swap kind (or byte order).
 */
enum IFR_ClientSwap
{
    SwapUnknown,   //!< Unknown byte order.
    SwapNormal,    //!< Big endian.
    SwapFull,      //!< Little endian.
    SwapHalf       //!< Mixed byte order.
};

/**
 * @ingroup IFR_Common
 * possible SQL modes.
 */
enum IFR_SQLMode {
     IFR_Nil            = 0,                  //!< Unknown SQL Mode.
     IFR_SessionSqlmode = 1,                  //!< SQL Mode of current session.
     IFR_INTERNAL       = SQLDBC_INTERNAL,    //!< SQL mode internal.
     IFR_ANSI           = SQLDBC_ANSI    ,    //!< @deprecated SQL mode ansi.
     IFR_DB2            = SQLDBC_DB2     ,    //!< @deprecated SQL mode db2.
     IFR_ORACLE         = SQLDBC_ORACLE  ,    //!< SQL mode oracle.
     IFR_SAPR3          = SQLDBC_SAPR3        //!< SQL mode used only for SAP R/3.
};

/**
 * @ingroup IFR_Common
 * Return code of functions. This is not an error code,
 * it only indicates the status of the function call.
 */
enum IFR_Retcode {
     IFR_OK                = SQLDBC_OK,     //!< Function call went ok.
     IFR_NOT_OK            = SQLDBC_NOT_OK, //!< Function call went not ok. Further information
                                            //!< is in the corresponding error object.
     IFR_DATA_TRUNC        = SQLDBC_DATA_TRUNC, //!< Data was truncated during the call.
     IFR_OVERFLOW          = SQLDBC_OVERFLOW,   //!< Signal of a numeric overflow.
     IFR_SUCCESS_WITH_INFO = SQLDBC_SUCCESS_WITH_INFO, //!< The method succeeded, but there were warnings.
     IFR_NO_DATA_FOUND     = SQLDBC_NO_DATA_FOUND, //!< Expected data was not found.
     IFR_NEED_DATA         = SQLDBC_NEED_DATA      //!< Late binding, data is needed for execution.
};

/**
 * @ingroup IFR_Common
 * Row status for mass operations. The status for
 * an inserted row is either one of these constants, or the row count
 * of the individual column.
 */
enum IFR_Rowstatus
{
    IFR_EXECUTE_FAILED  = SQLDBC_EXECUTE_FAILED,  //!< Execute of this row failed.
    IFR_SUCCESS_NO_INFO = SQLDBC_SUCCESS_NO_INFO, //!< Execute succeeded, no information about
                                                  //!< affected rows available.
    IFR_DELETED         = SQLDBC_DELETED,         //!< affected rows deleted,
    IFR_UPDATED         = SQLDBC_UPDATED,         //!< affected rows updated,
    IFR_NOROW           = SQLDBC_NOROW,           //!< affected rows not contained in result set,
    IFR_ADDED           = SQLDBC_ADDED,           //!< affected rows added
    IFR_ROW_IGNORE      = SQLDBC_ROW_IGNORE       //!< ignore this row.
};

/**
 * Sets the memory failure.
 * @param var The 'out of memory' indicator.
 */
#define IFR_MFAIL(var) var=false

#define IFR_MFAIL_DBUG_RETURN(var, retval) if(!var) DBUG_RETURN(retval)

#if (7 == 7) && (6 >=6)
#  define IFR_HAS_VARDATA 1
#else
#  define IFR_HAS_VARDATA 0
#endif


#if (7 == 7) && (6 >=6)
#  define IFR_HAS_FEATUREPART  1
#  define IFR_HAS_CLIENTIDPART 1
#else
#  define IFR_HAS_FEATUREPART  0
#  define IFR_HAS_CLIENTIDPART 0
#endif

enum IFR_ProfileCountValues {
 IFR_ALLOCATECOUNT           = SQLDBC_ALLOCATECOUNT,              /*!< nummber of calling allocate                                                          */   
 IFR_DEALLOCATECOUNT         = SQLDBC_DEALLOCATECOUNT,            /*!< nummber of calling deallocate                                                        */  
 IFR_CONNECTIONCOUNT         = SQLDBC_CONNECTIONCOUNT,            /*!< number of objects of type SQLDBC_Connection                                          */  
 IFR_COMMITCOUNT             = SQLDBC_COMMITCOUNT,                /*!< nummber of calling Commit                                                            */  
 IFR_ROLLBACKCOUNT           = SQLDBC_ROLLBACKCOUNT,              /*!< nummber of calling Rollback                                                          */  
 IFR_SETISOLATIONCOUNT       = SQLDBC_SETISOLATIONCOUNT,          /*!< nummber of calling setIsolationLevel                                                 */  
 IFR_SENDCOUNT               = SQLDBC_SENDCOUNT,                  /*!< number of order packets send to the kernel                                           */
 IFR_SENDSIZE                = SQLDBC_SENDSIZE,                   /*!< number of bytes send to the kernel                                                   */  
 IFR_RECEIVESIZE             = SQLDBC_RECEIVESIZE,                /*!< number of bytes received from the kernel                                             */  
 IFR_CANCELCOUNT             = SQLDBC_CANCELCOUNT,                /*!< nummber of calling cancel                                                            */  
 IFR_STATEMENTCOUNT          = SQLDBC_STATEMENTCOUNT,             /*!< number of objects of type SQLDBC_Statement                                           */  
 IFR_PREPAREDSTATEMENTCOUNT  = SQLDBC_PREPAREDSTATEMENTCOUNT,     /*!< number of objects of type SQLDBC_PreparedStatement                                   */  
 IFR_NOTSELECTFETCHOPTIMIZED = SQLDBC_NOTSELECTFETCHOPTIMIZED,    /*!< number of SQL statements that wasn't Select/Fetch optimized                          */  
 IFR_DROPPARSEIDCOUNT        = SQLDBC_DROPPARSEIDCOUNT,           /*!< number of parseids that were dropped                                                 */  
 IFR_DROPCURSORCOUNT         = SQLDBC_DROPCURSORCOUNT,            /*!< number of cursor that were closed                                                    */  
 IFR_DROPLONGDESCCOUNT       = SQLDBC_DROPLONGDESCCOUNT,          /*!< number of long descriptors that were closed                                          */  
 IFR_PREPARECOUNT            = SQLDBC_PREPARECOUNT,               /*!< nummber of calling prepare                                                           */  
 IFR_PARSEINFOHITCOUNT       = SQLDBC_PARSEINFOHITCOUNT,          /*!< hitcount of the parseinfo cache                                                      */  
 IFR_PARSINFOMISSCOUNT       = SQLDBC_PARSINFOMISSCOUNT,          /*!< miscount of the parseinfo cache                                                      */  
 IFR_PARSEAGAINCOUNT         = SQLDBC_PARSEAGAINCOUNT,            /*!< number of internal ParseAgain loops                                                  */  
 IFR_PARSEINFOSQUEEZECOUNT   = SQLDBC_PARSEINFOSQUEEZECOUNT,      /*!< squeeze count of the parseinfo cache                                                 */  
 IFR_EXECUTECOUNT            = SQLDBC_EXECUTECOUNT,               /*!< nummber of calling Executes                                                          */  
 IFR_INSERTCOUNT             = SQLDBC_INSERTCOUNT,                /*!< number of INSERT statements                                                          */  
 IFR_UPDATECOUNT             = SQLDBC_UPDATECOUNT,                /*!< number of UPDATE statements                                                          */  
 IFR_SELECTCOUNT             = SQLDBC_SELECTCOUNT,                /*!< number of SELECT statements                                                          */  
 IFR_CALLDBPROCEDURECOUNT    = SQLDBC_CALLDBPROCEDURECOUNT,        /*!< number of DBProcedure calls                                                          */
 IFR_DELETECOUNT             = SQLDBC_DELETECOUNT,                /*!< number of DELETE statements                                                          */  
 IFR_FETCHCOUNT              = SQLDBC_FETCHCOUNT,                 /*!< number of FETCH statements                                                           */  
 IFR_SQLERRORCOUNT           = SQLDBC_SQLERRORCOUNT,              /*!< number of SQL errors                                                                 */  
 IFR_ROWNOTFOUNDCOUNT        = SQLDBC_ROWNOTFOUNDCOUNT,           /*!< number of SELECT statements that return Row not found                                */  
 IFR_FETCHROWCOUNT           = SQLDBC_FETCHROWCOUNT,              /*!< number of all fetched rows                                                           */  
 IFR_DATAREADSIZE            = SQLDBC_DATAREADSIZE,               /*!< number of all databytes retrieved from the kernel                                    */  
 IFR_DATAWRITESIZE           = SQLDBC_DATAWRITESIZE,              /*!< number of all databytes send to the kernel                                           */  
 IFR_LONGDATAREADSIZE        = SQLDBC_LONGDATAREADSIZE,           /*!< number of bytes of all long data retrieved from the kernel                           */  
 IFR_LONGDATAWRITESIZE       = SQLDBC_LONGDATAWRITESIZE,          /*!< number of bytes of all long data send to the kernel                                  */  
 IFR_PROFILE_MAX             = SQLDBC_PROFILE_MAX                /*!< Maximum value for checks. */
};                                                                                                                                 

// Static type checking.

template <bool> struct IFR_AssertionFailure;
template <> struct IFR_AssertionFailure<true> {};
template <int x> struct IFR_AssertionTest{};

#define IFR_STATIC_ASSERT_L(X, L) typedef IFR_AssertionTest<sizeof(IFR_AssertionFailure<(bool) ( X ) >)> assertion_failed_##L
#define IFR_STATIC_ASSERT(X) IFR_STATIC_ASSERT_L((X), __LINE__)

IFR_END_NAMESPACE

#endif
