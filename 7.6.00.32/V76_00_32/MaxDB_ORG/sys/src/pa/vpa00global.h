/*



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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





*/
#ifndef VPA00GLOBAL_H
#define VPA00GLOBAL_H
/*!**********************************************************************

  module: vpa00global.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  global include file with common includes

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpr100.h"
#include "heo07.h"
#include "vpa00vrc.h"
#include "vpa00enc.h"
#include "vpa00wic.h"
#include "vpa00sqlext.h"
#include "vpa00txc.h"
#include "vpa00dlc.h"
#include "geo00.h"
#include "gsp00.h"
#include "vsp001.h"
#include "gpr00.h"
#include "vpa00odc.h"
#include "vpa00sqlod32.h"
#include "vpa00odstream.h"

#include <assert.h>

/* ****** assert-macro ******** */

#ifdef DEBUG
#define API_ASSERT(expression) assert(expression)
#define API_ASSERT_PTR(ptr) assert(ptr!=NULL)
#else
#define API_ASSERT(expression)
#define API_ASSERT_PTR(ptr)
#endif

/* internal return codes      */
#define API_NOT_OK              (0)
#define API_OK                  (1)
#define API_TRUNCATE            (2)
#define API_NO_DATA_FOUND       (3)
#define API_NEED_DATA           (4)
#define API_DATA_LOSS           (5)
#define API_NOT_NUMBER          (6)
#define API_NOT_DATE            (7)
#define API_INVALID             (8)
#define API_FEW_BOUNDS          (9)
#define API_INVALID_LENGTH      (10)
#define API_CONSTRAINT_VIOLATION (11)
#define API_TIME_OUT            (12)
#define API_NO_INDICATOR        (13)
#define API_INVALID_CHAR        (14)
#define API_FRACTIONAL_TRUNC    (15)
#define API_TRUNCATE_22001      (16)      /* truncation of input parameter, 1126177 */
#define API_MARK_END            (-1)
#define API_ANY                 (9999)

/* constants */
#define MX_IDENTIFIER 64
#define API_COLUMN_NAME_MAX    MX_IDENTIFIER   /* "owner . name" max len */
#define API_TABLE_NAME_MAX     MX_IDENTIFIER   /* "owner . name" max len */
#define API_USERID_MAX_LEN     MX_IDENTIFIER
#define API_PASSWD_MAX_LEN     MX_IDENTIFIER
/* make this big because sap router strings are large */
#define API_MAX_SERVERNODE_LEN 64000

#define API_EMPTY_STRING 	""

/* statement states */
#define API_STMT_UNALLOCATED (0)  /* statement S0: unallocated  */
#define API_STMT_ALLOCATED   (1)  /* statement S1: allocated    */
#define API_STMT_PREPARED    (2)  /* statement S2: prepared     */
#define API_STMT_EXECUTED    (3)  /* statement S3: executed     */
#define API_STMT_POSITIONED  (4)  /* statement S4: positioned   */
#define API_STMT_RECONNECTED (5)  

/*****************************************************************
 * BIGINT support
 */

#ifdef ODBCINT64
#define BIGINT_SUPPORTED

typedef unsigned ODBCINT64 API_UBIGINT;
typedef ODBCINT64          API_BIGINT;

#endif

/* BIGINTs are only supported on WIN32 ans 64 bit platforms */
/* #if  defined(WIN32) || defined(BIT64)
   #define BIGINT_SUPPORTED
   #endif
*/

#ifdef BIGINT_SUPPORTED
#ifdef WIN32
/*
typedef unsigned _int64 API_UBIGINT;
typedef _int64          API_BIGINT;
*/

/* sprintf placeholder for BIGINTS */
#define PRINT_SBIGINT "%I64d"
#define PRINT_UBIGINT "%I64u"

#define API_ATOI64 _atoi64

#else /* 64 bit platforms: BIGINT == long */

/*
typedef unsigned long API_UBIGINT;
typedef long          API_BIGINT;
*/

/* sprintf placeholder for BIGINTS */
#define PRINT_SBIGINT "%ld"
#define PRINT_UBIGINT "%lu"

#define API_ATOI64 atol

#endif /* WIN32 */

#define SQLBIGINT  API_BIGINT   /* datatype for SQL_C_SBIGINT, SQL_C_UBIGINT */
#define SQLUBIGINT API_UBIGINT
#endif /* BIGINT_SUPPORTED */

/*
 * BIGINT support END
 *****************************************************************/



/* sign indicator for c datatype SQL_C_NUMERIC */
#define NUMERIC_POSITIVE_SIGN 1
#define NUMERIC_NEGATIVE_SIGN 0

/* implementation defined precisions */
#define API_SQL_NUMERIC_PRECISION  18
#define API_SQL_DECIMAL_PRECISION  18
#define API_SQL_FLOAT_PRECISION    15

/* #define MEMCHECK */
/* sqlod32.dld : msvcrt -> msvcrtd !!! */

#endif


