/*!========================================================================

  @ingroup      Sec
  @file         RTESec_SSL.h
  @author       StefanP

  @brief        

  @since        2005-02-11  12:17
  @sa           


  =======================================================================

  \if EMIT_LICENCE
    
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
  ----------------------------------------------------------------------------*/


#ifndef RTESEC_SSL_H
#define RTESEC_SSL_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gsp01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/

/*!
    @brief      The function determinates if the servernode is given as URI for a SSL connection 
                (cf. liveCache URI Schema).     

    @param      pszServerNode [in/out]  - In: Servernode 
                                          Out: 
                                          If the servernode was given as URI for SSL:
                                            i.  For remoteSSL communication: <host> [":" <port> ]
                                            ii. For sapniSSL communication: SAP Router String 
                                          else the servernode leaves unchanged  
    @param      isSSL [out]             - true: A SSL connection is requested (remoteSSL or sapniSSL communication)
    @param      isNISSL [out]           - true: A SSL connection over a SAP Router string is requested (this implies that isSSL is true too)
    @param      ignoreHostnameInServerCert [out] - true: Hostname of Server Certificate should be ignored 
    @param      errText [out]           - Error text

    @return     commErrOk_esp01, commErrNotOk_esp01   
*/

externC tsp01_CommErr_Enum  RTESec_IsSSLConnection (SAPDB_Char *                    pszServerNode,
                                                    SAPDB_Bool VAR_VALUE_REF        isSSL,
                                                    SAPDB_Bool VAR_VALUE_REF        isNISSL,
                                                    SAPDB_Bool VAR_VALUE_REF        ignoreHostnameInServerCert,
                                                    tsp00_ErrTextc VAR_ARRAY_REF    errText);

/*!
    @brief          

    @param      isClient [in]
    @param      errText [out]           - Error text

    @return     commErrOk_esp01, commErrNotOk_esp01   
*/

externC tsp01_CommErr_Enum  RTESec_SSLInit (tsp00_Pathc VAR_ARRAY_REF               traceFile,
                                            tsp00_ErrTextc VAR_ARRAY_REF            errText);

/*!
  @brief initialize SSL utility
 */
externC tsp01_CommErr_Enum  RTESec_SSLUtilInit (tsp00_Pathc VAR_ARRAY_REF           traceFile,
                                                tsp00_ErrTextc VAR_ARRAY_REF        errText);
#endif  /* RTESEC_SAPSSL_H */

