/*!
  @file           veo401.c
  @author         JoergM
  @special area   TCP/IP routines via SAP NI ( network interface )
  @brief          description ...
  @see            example.html ...

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

/*--------------------------------------------------------------------*/
/* SAP includes                                                       */
/*--------------------------------------------------------------------*/
#ifdef WIN32
  #define SAPonNT
#endif

#ifdef _IBMR2
  #define SAPonRS6000
#endif

#if PA11 || PA20W
  #define SAPonHPPA
#endif

#if defined (HP_IA64)
  #define SAPonHPPA
  #define SAPonHPIA64
#endif

#if defined(OSF1)
  #define SAPonALPHA
#endif

#if (defined(SVR4) && defined(SUN)) || defined(SOLARIS)
  #define SAPonSUN
#endif

#if defined(LINUX)
#define SAPonLIN
#endif

#if defined(FREEBSD)
#define SAPonFREEBSD
/* defined somewhere else ... */
#endif

#if defined(SVR4) && defined(NMP)
  #define SAPonRM600
#endif

/* UNICODE */
#include "sap/700/saptype.h"	/* nocheck */
#include "sap/700/saptypeb.h"	/* nocheck */
#include "sap/700/saptypec.h"	/* nocheck */

/* NI */
#include "sap/700/nixx.h"		/* nocheck */
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo401.h"
#include "SAPDBCommon/SAPDB_string.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*===========================================================================*/

externC int
eo40NiIsSaprouterSring     (  char                   *StrToCheck )
{
  return( memcmp (StrToCheck, NI_HOSTSEP, NI_SEPSIZ) == 0 );
}

/*===========================================================================*/

externC int
eo40NiExtractDBNodeFromSaprouterSring ( char                        *SaprouterString,
                                        tsp00_NodeIdc VAR_ARRAY_REF  ServerNode  )
{
  int IsSaprouterString = eo40NiIsSaprouterSring(SaprouterString);

  if ( IsSaprouterString )
  {
    int   NodeLen ;
    char *NodeBeg = SaprouterString + NI_SEPSIZ ,
         *Ptr = NodeBeg ;

    while ( (Ptr = strstr ( Ptr, NI_HOSTSEP )) != NULL )
    {
      NodeBeg = Ptr + NI_SEPSIZ;
      Ptr     = NodeBeg ;
    }

    Ptr = strchr(NodeBeg, *NI_HOSTSEP ) ;/* end of host */

    NodeLen = Ptr ? (int)(Ptr - NodeBeg) : (int)strlen(NodeBeg);

    if ( NodeLen >= sizeof(tsp00_NodeIdc) )
      NodeLen = sizeof(tsp00_NodeIdc) - 1 ;

    SAPDB_memcpy( ServerNode, NodeBeg, NodeLen );
    ServerNode [ NodeLen+1 ] = '\0' ;
    }

  return ( IsSaprouterString ) ;
}

/*===========================================================================*/

externC int
eo40NiExtractServiceFromSaprouterSring ( char                        *SaprouterString,
                                         unsigned short              *Service )
{
  int IsSaprouterString = eo40NiIsSaprouterSring(SaprouterString);

  if ( IsSaprouterString )
  {
    char *LastHost = SaprouterString ;
    char *Ptr      = LastHost;

    /* search last host in routerstring ==> server node */

    while ( (Ptr = strstr ( LastHost+1, NI_HOSTSEP ) ) )
      LastHost = Ptr ;

    if ( (Ptr = strstr ( LastHost, NI_SERVSEP )) == NULL )
    {
      *Service = 0 ;
    }
    else
    {
      /* There is only one NI_HOSTSEP AND a NI_SERVSEP */
      int i;
      char szService[6] ;

      for ( i=0, Ptr += NI_SEPSIZ ;
            i < sizeof(szService)-1 && isdigit(*Ptr) ;
            i++, Ptr++ )
        szService[i] = *Ptr ;

       szService[i] = '\0' ;

      *Service = (unsigned short) atoi(szService);
    }
  }
  return ( IsSaprouterString ) ;
}

/*===========================================================================*/
