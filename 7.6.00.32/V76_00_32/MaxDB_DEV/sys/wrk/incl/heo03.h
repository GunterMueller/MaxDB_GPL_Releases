/* @lastChanged: "1998-05-18  11:32"
 * @filename     heo03.h
 * @purpose:     "component communication"
 * @release:     7.1.0.0
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

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



\endif

 */


#ifndef HEO03_H
#define HEO03_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp01.h"
#include "vsp001.h"
#include "geo003.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
/* PTS 1104899 */

externC tsp00_Bool 
SqlIsSaprouterString ( char                        *szServerNode ); /* '\0' terminated node or saprouterstring */

externC tsp01_CommErr_Enum 
SqlAConnect (   char                               *szServerNode,   /* '\0' terminated node or saprouterstring */
                tsp00_DbNamec VAR_ARRAY_REF         szServerDB,     /* '\0' terminated dbname                  */
                tsp01_Service_Enum                  service,
                tsp00_Int4                          packet_cnt,	
                tsp00_Int4 VAR_VALUE_REF            reference,
                tsp00_Int4 VAR_VALUE_REF            sql_packet_size,
                void                              **sql_packet_list,    /* tsp00_Array<tsp1_packet_ptr, ...> */
                tsp00_ErrTextc VAR_ARRAY_REF        szErrText );        /* '\0' terminated error string   */


externC tsp01_CommErr_Enum
SqlDBConnect ( char                               *szServerNode,       /* '\0' terminated node or saprouterstring */
               tsp00_DbNamec VAR_ARRAY_REF         szServerDB,         /* '\0' terminated dbname                  */
               tsp01_Service_Enum                  service,
               tsp00_Int4                          packet_cnt,	
               tsp00_Int4 VAR_VALUE_REF            reference,
               tsp00_Int4 VAR_VALUE_REF            max_reply_packet_size,
               tsp00_Int4 VAR_VALUE_REF            sql_packet_size,
               void                              **sql_packet_list,    /* tsp00_Array<tsp1_packet_ptr, ...> */
               tsp00_ErrTextc VAR_ARRAY_REF        szErrText );        /* '\0' terminated error string   */

externC void  sqlaconnect(
  tsp00_TaskId                  uid,
  tsp00_NodeId  VAR_ARRAY_REF   servernode,         /* tsp00_Array<char, mxsp_c64> */
  tsp00_DbName  VAR_ARRAY_REF   serverdb,           /* tsp00_Array<char, mxsp_c18> */
  tsp01_Service_Enum            service,
  tsp00_Int4                    packet_cnt,     
  tsp00_Int4 VAR_VALUE_REF      reference,
  tsp00_Int4 VAR_VALUE_REF      sql_packet_size,
  void                        **sql_packet_list,   /* tsp00_Array<tsp1_packet_ptr, ...> */
  tsp00_ErrText VAR_ARRAY_REF   errtext,           /* tsp00_Array<char, mxsp_c40> */
  tsp01_CommErr VAR_VALUE_REF   returncode);       /* tsp00_Enum<tsp1_comm_error_Enum, ...> */

externC void sqlarelease (
  tsp00_Int4                    reference);


externC void sqlarequest (
  tsp00_Int4                    reference,
  void                         *sql_packet_ptr,
  tsp00_Int4                    sql_packet_length,
  tsp00_ErrText VAR_ARRAY_REF   errtext,            /* tsp00_Array<char, mxsp_c40> */
  tsp01_CommErr VAR_VALUE_REF   returncode);        /* tsp00_Enum<tsp1_comm_error_Enum, ...> */


externC void sqlareplyavailable (
  tsp00_Int4                    reference,
  tsp00_ErrText VAR_ARRAY_REF   errtext,            /* tsp00_Array<char, mxsp_c40> */
  tsp01_CommErr VAR_VALUE_REF   returncode);        /* tsp00_Enum<tsp1_comm_error_Enum, ...> */


externC void sqlareceive (
  tsp00_Int4                    reference,
  void                        **res_packet_ptr,
  tsp00_Int4 VAR_VALUE_REF      res_packet_length,
  tsp00_ErrText VAR_ARRAY_REF   errtext,            /* tsp00_Array<char, mxsp_c40> */
  tsp01_CommErr VAR_VALUE_REF   returncode);        /* tsp00_Enum<tsp1_comm_error_Enum, ...> */


externC void sqlacancel (
    tsp00_Int4                  reference);

/* PTS 1105116 */
externC tsp01_CommErr_Enum SqlACancel (
  tsp00_Int4                          reference ,
  char *                              szSapRouterString, /* '\0' terminated node or saprouterstring */
  tsp00_ErrTextc VAR_ARRAY_REF        szErrText );       /* '\0' terminated error string   */

/* PTS 1105295 */
externC tsp01_CommErr_Enum  SqlAPing ( 
  char *                            pszServerNode,                 /* '\0' terminated node or saprouterstring */
  SAPDB_Bool                        captureServerCert,
  tsp00_Versionc VAR_ARRAY_REF      szServerVersion,               /* '\0' terminated version from answering server */
  SAPDB_Byte VAR_VALUE_REF          pCertificateBuffer,
  SAPDB_UInt4                       lenCertificateBuffer,
  tsp00_ErrTextc VAR_ARRAY_REF      errText);                      /* '\0' terminated error string   */

externC void sqlhostname (
  tsp00_NodeId  VAR_ARRAY_REF   host,               /* tsp00_Array<char, mxsp_c64> */
  tsp00_ErrText VAR_ARRAY_REF   errtext,            /* tsp00_Array<char, mxsp_c40> */
  tsp01_CommErr VAR_VALUE_REF   returncode);        /* tsp00_Enum<tsp1_comm_error_Enum, ...> */


externC void sqlnodename (
  tsp00_NodeId  VAR_ARRAY_REF   host,               /* tsp00_Array<char, mxsp_c64> */
  tsp00_NodeId  VAR_ARRAY_REF   node,               /* tsp00_Array<char, mxsp_c64> */
  tsp00_ErrText VAR_ARRAY_REF   errtext,            /* tsp00_Array<char, mxsp_c40> */
  tsp01_CommErr VAR_VALUE_REF   returncode);        /* tsp00_Enum<tsp1_comm_error_Enum, ...> */


externC tsp01_CommErr_Enum  SqlPingConnect (    
  char*                              szServerNode,      /* '\0' terminated node or saprouterstring */
  char*                              szServerDB,        /* '\0' terminated dbname                  */
  tsp00_Int4     VAR_VALUE_REF       reference,         /* connection reference                    */
  tsp00_ErrTextc VAR_ARRAY_REF       pErrText );        /* '\0' terminated error string            */

externC tsp01_CommErr_Enum SqlDBPing ( 
  tsp00_Int4                         reference,         /* connection reference                          */
  tsp00_Uint4     VAR_VALUE_REF      sendBufferSize,    /*  size of the send buffer                      */
  tsp00_Uint1     VAR_VALUE_REF      hops,              /* maximum number of hops to the destination     */
  tsp00_DbNamec   VAR_ARRAY_REF      szServer,          /* '\0' terminated name of the answering server  */
  tsp00_Versionc  VAR_ARRAY_REF      szServerVersion,   /* '\0' terminated version from answering server */
  tsp00_ErrTextc  VAR_ARRAY_REF      pErrText );        /* '\0' terminated error string                  */

externC void        SqlPingRelease  (
  tsp00_Int4                         reference );       /* connection reference                          */

externC SAPDB_Bool  SqlSSLAvailable (
  tsp00_ErrTextc VAR_ARRAY_REF       errText );         /* \0' terminated error string                   */

externC SAPDB_Bool  SqlBasicAuthenticationOnly ( 
  tsp00_Int4                         reference,         /* connection reference                          */
  tsp00_ErrTextc VAR_ARRAY_REF       errText );         /* \0' terminated error string                   */

externC tsp01_CommErr_Enum SqlDBDump (
  tsp00_Int4                         reference,         /* connection reference                          */
  tsp00_ErrTextc VAR_ARRAY_REF       errText );         /* \0' terminated error string                   */


#endif  /* HEO03_H */
