#ifndef VPA00SQLOD32_H
#define VPA00SQLOD32_H
/*!**********************************************************************

  module: vpa00sqlod32.h

  -----------------------------------------------------------------------

  responsible:  ThomasK

  special area: ODBC-Driver

  description:  defines for SAP-DB specific extensions to ODBC

  see also:

  -----------------------------------------------------------------------



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





************************************************************************/

/* GetInfo attributes
 */

#define SQL_SQLOD32_GETINFO_OFFSET 20000

/* driver name with full path */
#define SQL_DRIVER_PATH 20001

#define SQL_CONNECT_OPT_DRVR_START 1000

/* driver defined connect options.
 */
#define SQL_PASSTHROUGH     SQL_CONNECT_OPT_DRVR_START+1
#define SQL_SQLMODE         SQL_CONNECT_OPT_DRVR_START+2

/* this will overwrites the member sp1h_application of the packet */
#define SQL_COMPONENT       SQL_CONNECT_OPT_DRVR_START+3
enum sql_tsp_component { sql_csp_comp_call, sql_csp_comp_cpc, sql_csp_comp_cobpc, sql_csp_comp_control, sql_csp_comp_domain, sql_csp_comp_easy, sql_csp_comp_load, sql_csp_comp_odbc, sql_csp_comp_oci, sql_csp_comp_plrun, sql_csp_comp_query, sql_csp_comp_sqlpl, sql_csp_comp_tplrun, sql_csp_comp_util, sql_csp_comp_wb, sql_csp_comp_xci};

#define SQL_CANCELCALLBACK  SQL_CONNECT_OPT_DRVR_START+4
#define SQL_SERVICE         SQL_CONNECT_OPT_DRVR_START+5

/* overwrites the sp1c_mess_type of the segment header. See vsp001 for
   possible values  */
#define SQL_MESSAGE_CODE    SQL_CONNECT_OPT_DRVR_START+6

#define SQL_UTILITY_SESSION SQL_CONNECT_OPT_DRVR_START+7

/* Pointer to structure, that is used if ODBC runs in the DB-Kernel */
#define SQL_KERNEL_DIRECT   SQL_CONNECT_OPT_DRVR_START+8

/* Read only connect attribute to check for NI connections */
#define SQL_IS_NI_CONNECTION SQL_CONNECT_OPT_DRVR_START+9

/* Set SAP Router String for SQLCancel over NI Connection */
#define SQL_CANCEL_ROUTER_STRING SQL_CONNECT_OPT_DRVR_START+10

/* Set session-timeout for connection */
#define SQL_ATTR_SESSION_TIMEOUT SQL_CONNECT_OPT_DRVR_START+11

/* overwrites the member sp1c_producer of the segement header. See vsp001 for
   possible values */
#define SQL_PRODUCER        SQL_CONNECT_OPT_DRVR_START+12

/* Codepage used for UCS2<->ASCII conversion. Codepage is selected from the DB.
   In Windows: default codepage is active codepage of the client.
   In non-Windows: default codepage is WINDOWS-1252 (contains Euro-symbol). */
/* a codepage is valid for all(!) connections of the affected driver. If
   SQLSetConnectAttr is called a second time with a different codepage,
   the driver could crash, if a conversion is simultaneously running. */
#define SQL_ATTR_CODEPAGE   SQL_CONNECT_OPT_DRVR_START+13

/* for this attribute SQL_TRUE is returned, if the DB supports Unicode,
   otherwise SQL_FALSE */
#define SQL_ATTR_UNICODEDB   SQL_CONNECT_OPT_DRVR_START+14

/* encryption for the connection. 0 means no encryption. */
#define SQL_ATTR_ENCRYPT   SQL_CONNECT_OPT_DRVR_START+15


#define SQL_ATTR_ENCRYPT_OFF   0    /* no encryption */
#define SQL_ATTR_ENCRYPT_SSL   1    /* SSL */


/* this value for SQL_ATTR_SESSION_TIMEOUT means that no 'TIMEOUT <to>'
 * is appended to connect string */
#define SQL_DEFAULT_SESSION_TIMEOUT  ((UDWORD) (1 << 30))

/* determines how much caching should be done by the driver when using
 * SQLExtendedFetch */
#define SQL_ATTR_DATA_CACHING      (3000)
#define SQL_ATTR_GROUP_BY_COLUMNS  (3001) /* retrieve group by columns */
#define SQL_ATTR_OPTIMIZE_CURSOR   (3002)
#define SQL_ATTR_READ_LONG_POS     (3003) /* positional reading with
                                           * SQLGetData */

#define API_DATA_CACHING_DEFAULT 250 /* get 250 rows with one fetch */

/* Driver defined field of diag-header. Used in conjunction with
 * SQLGetDiagField to retrieve the out connect string of the last
 * SQLDriverConnect. This string is also available if the connect failed. */
#define SQL_DIAG_CONNECT_STRING    (1001)
/* Check if servernode of previous SQLDriverConnect is a SAP router string */
#define SQL_DIAG_IS_NI_CONNECTION  (1002)

/* factor that indicates output parameters. The real sql type is calculated
 * as sqlType * SQL_OUTPRM_FACTOR, if it is an output parameter.
 */
#define SQL_OUTPRM_FACTOR 100
#define SQL_INOUTPRM_OFFSET 10000

/* Offset for driver defined dynamic function codes as returned by
 * SQLGetDiagField with SQL_DIAG_DYNAMIC_FUNCTION_CODE.
 * The driver defined codes (not included in the ODBC 3.5 standard)
 * are calculated from the SAPDB specific (derived from ORACLE call interface)
 * function codes (see vsp001) by adding this offset.
 */
#define SQL_FUNCTION_CODE_OFFSET 1000

#endif
