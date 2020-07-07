/* @lastChanged: "1998-05-14  10:49"
 * @filename:    vsp100.cpp
 * @purpose:     "Version"
 * @release:     7.1.0
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"



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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define  VSP100_CPP

#if defined(_WIN32)
# include <windows.h>
#else
# include <stdio.h>
#endif

#include "gsp00.h"
#include "gsp100.h"
#include "hsp100.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE  inline
#endif

#if defined(_WIN32)
 #define SPRINTF     wsprintf
#else
 #define SPRINTF     sprintf
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC void sp100_GetVersionString ( const tsp100_CompName   ComponentName,
                                      tsp100_BuildNumberFunc  pBuildNumberFunc,
                                      tsp00_Versionc          &VersionString )
{
  ROUTINE_DBG_MEO00 ("sp100_GetVersionString");

  char    Version[20];
 
  SPRINTF ( Version,
            "%d.%d.%d", 
            MAJOR_VERSION_NO_SP100,
            MINOR_VERSION_NO_SP100,
            CORRECTION_LEVEL_SP100 );

  SPRINTF ( VersionString,
            VERSION_STR_SP100,
            ComponentName,
            Version,
            BUILD_PREFIX_STR_SP100,
            pBuildNumberFunc() );
              
 
  return;
}

/*---------------------------------------------------------------------------*/

externC tsp100_VersionResult
      sp100_Compare2VersionIDs( tsp100_VersionIDType     IDType,
                                tsp100_VersionID         *pCmpVersionID1,
                                tsp100_VersionID         *pCmpVersionID2 )
{
  ROUTINE_DBG_MEO00 ("sp100_CompareVersion2ID");

  switch (IDType)
  {
  case VersionIDType0_esp100:
    if ( pCmpVersionID1->ID0.MajorVersion_sp100  < pCmpVersionID2->ID0.MajorVersion_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID0.MajorVersion_sp100  > pCmpVersionID2->ID0.MajorVersion_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID0.MinorVersion_sp100  < pCmpVersionID2->ID0.MinorVersion_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID0.MinorVersion_sp100  > pCmpVersionID2->ID0.MinorVersion_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID0.CorrLevel_sp100     < pCmpVersionID2->ID0.CorrLevel_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID0.CorrLevel_sp100     > pCmpVersionID2->ID0.CorrLevel_sp100 )
      return ( IsNewer_esp100 );
    break;

  case VersionIDType1_esp100:
    if ( pCmpVersionID1->ID1.MajorVersion_sp100  < pCmpVersionID2->ID1.MajorVersion_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID1.MajorVersion_sp100  > pCmpVersionID2->ID1.MajorVersion_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID1.MinorVersion_sp100  < pCmpVersionID2->ID1.MinorVersion_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID1.MinorVersion_sp100  > pCmpVersionID2->ID1.MinorVersion_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID1.CorrLevel_sp100     < pCmpVersionID2->ID1.CorrLevel_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID1.CorrLevel_sp100     > pCmpVersionID2->ID1.CorrLevel_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID1.BuildNumberPrefix_sp100 < pCmpVersionID2->ID1.BuildNumberPrefix_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID1.BuildNumberPrefix_sp100 > pCmpVersionID2->ID1.BuildNumberPrefix_sp100 )
      return ( IsNewer_esp100 );
    break;

  case VersionIDType2_esp100:
    if ( pCmpVersionID1->ID2.MajorVersion_sp100  < pCmpVersionID2->ID2.MajorVersion_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID2.MajorVersion_sp100  > pCmpVersionID2->ID2.MajorVersion_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID2.MinorVersion_sp100  < pCmpVersionID2->ID2.MinorVersion_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID2.MinorVersion_sp100  > pCmpVersionID2->ID2.MinorVersion_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID2.CorrLevel_sp100     < pCmpVersionID2->ID2.CorrLevel_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID2.CorrLevel_sp100     > pCmpVersionID2->ID2.CorrLevel_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID2.BuildNumberPrefix_sp100 < pCmpVersionID2->ID2.BuildNumberPrefix_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID2.BuildNumberPrefix_sp100 > pCmpVersionID2->ID2.BuildNumberPrefix_sp100 )
      return ( IsNewer_esp100 );

    if ( pCmpVersionID1->ID2.BuildNumber_sp100   < pCmpVersionID2->ID2.BuildNumber_sp100 )
      return ( IsOlder_esp100 );

    if ( pCmpVersionID1->ID2.BuildNumber_sp100   > pCmpVersionID2->ID2.BuildNumber_sp100 )
      return ( IsNewer_esp100 );
    break;
  }


  return  IsEqual_esp100;
}

/*---------------------------------------------------------------------------*/

externC tsp100_VersionResult
      sp100_CompareVersionIDs( tsp100_VersionID  *pCmpVersionID )
{
  tsp100_VersionID        VersionID;

  ROUTINE_DBG_MEO00 ("sp100_CompareVersionID");

  VersionID.ID0.MajorVersion_sp100      = MAJOR_VERSION_NO_SP100;
  VersionID.ID0.MinorVersion_sp100      = MINOR_VERSION_NO_SP100;
  VersionID.ID0.CorrLevel_sp100         = CORRECTION_LEVEL_SP100;

  // --- no build prefix or build number compare!!
  return  sp100_Compare2VersionIDs( VersionIDType0_esp100, pCmpVersionID, &VersionID );
}

/*---------------------------------------------------------------------------*/

externC void sp100_GetVersionID ( tsp100_VersionIDType      IDType,
                                  tsp100_BuildNumberFuncEx  pBuildNumberFuncEx,
                                  tsp100_VersionID          *pVersionID )
{
  ROUTINE_DBG_MEO00 ("sp100_GetVersionIDAndBuild");


  switch (IDType)
  {
  case VersionIDType0_esp100:
    pVersionID->ID0.MajorVersion_sp100      = MAJOR_VERSION_NO_SP100;
    pVersionID->ID0.MinorVersion_sp100      = MINOR_VERSION_NO_SP100;
    pVersionID->ID0.CorrLevel_sp100         = CORRECTION_LEVEL_SP100;
    break;

  case VersionIDType1_esp100:
    pVersionID->ID1.MajorVersion_sp100      = MAJOR_VERSION_NO_SP100;
    pVersionID->ID1.MinorVersion_sp100      = MINOR_VERSION_NO_SP100;
    pVersionID->ID1.CorrLevel_sp100         = CORRECTION_LEVEL_SP100;
    pVersionID->ID1.BuildNumberPrefix_sp100 = BUILD_PREFIX_SP100;
    break;

  case VersionIDType2_esp100:
    pVersionID->ID2.MajorVersion_sp100      = MAJOR_VERSION_NO_SP100;
    pVersionID->ID2.MinorVersion_sp100      = MINOR_VERSION_NO_SP100;
    pVersionID->ID2.CorrLevel_sp100         = CORRECTION_LEVEL_SP100;
    pVersionID->ID2.BuildNumberPrefix_sp100 = BUILD_PREFIX_SP100;
    pBuildNumberFuncEx(NULL, &pVersionID->ID2.BuildNumber_sp100);
    break;
  }
  return;
}

/*---------------------------------------------------------------------------*/

externPascal void sp100_GetSenderIDVersionP  ( tsp00_C5                  &SenderIDP )
{
  ROUTINE_DBG_MEO00 ("sp100_GetSenderIDVersionP");

  tsp00_C5c             SenderID;
  char                  *pSenderID      = SenderID;
  char                  *pSenderIDP     = SenderIDP;
  short                 SizeOfSenderIDP = sizeof(SenderIDP);


  sp100_GetSenderIDVersion ( SenderID );
    
  // --- C to Pascal conversion
  while (SizeOfSenderIDP-- > 0)
  {
    if (*pSenderID == '\0')
      *pSenderIDP++ = ' ';
    else
      *pSenderIDP++ = *pSenderID++;
  }

  return;
}

/*---------------------------------------------------------------------------*/

externC void sp100_GetSenderIDVersion ( tsp00_C5c                 &SenderID )
{
  ROUTINE_DBG_MEO00 ("sp100_GetSenderIDVersion");

 
  SPRINTF ( SenderID,
            "%-1.1d%-2.2d%-2.2d", 
            MAJOR_VERSION_NO_SP100,
            MINOR_VERSION_NO_SP100,
            CORRECTION_LEVEL_SP100 );

  return;
}

/*---------------------------------------------------------------------------*/

externC void sp100_GetPingReplyVersionString ( tsp100_CompName const   ComponentName,
                                            tsp100_BuildNumberFunc  pBuildNumberFunc,
                                            tsp00_Versionc         &VersionString )
{
  ROUTINE_DBG_MEO00 ("sp100_GetPingReplyVersionString");

  char    Version[20];
 
  SPRINTF ( Version,
            "%d.%d.%02d", 
            MAJOR_VERSION_NO_SP100,
            MINOR_VERSION_NO_SP100,
            CORRECTION_LEVEL_SP100 );

  SPRINTF ( VersionString,
            VERSION_STR_SP100,
            ComponentName,
            Version,
            BUILD_PREFIX_STR_SP100,
            pBuildNumberFunc() );
              
 
  return;
}

/*---------------------------------------------------------------------------*/

externC void sp100_ConvertToVersionString ( tsp00_Versionc & VersionString )
{
  ROUTINE_DBG_MEO00 ("sp100_ConvertToVersionString");

  char    Version[END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 2];
  char   *pVal;
  int     dotsFound = 0;

  memcpy( &Version[0],
          &VersionString[BEG_OF_1ST_VERS_SUBSTR_SP100],
          END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1 );
  Version[END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1] = 0; /* add trailing zero */

  for ( pVal = &Version[0]; *pVal; ++pVal )
  {
      if ( '.' == *pVal )
      {
          ++dotsFound;
      }
      else if ( dotsFound == 2 )
      {
          /* If correction level is finally found add a leading zero, but only if needed */
          if ( *(pVal+1) == ' ' )
          {
              *(pVal+1) = *pVal; /* transfer the single digit */
              *pVal = '0';       /* replace original digit with zero */
              memcpy( &VersionString[BEG_OF_1ST_VERS_SUBSTR_SP100],
                      &Version[0], 
                      END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1 );
          }
          break;
      }
  }
}

/*---------------------------------------------------------------------------*/

externC void sp100_ConvertToPingReplyVersionString ( tsp00_Versionc & VersionString )
{
  ROUTINE_DBG_MEO00 ("sp100_ConvertToPingReplyVersionString");

  char    Version[END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 2];
  char   *pVal;
  int     dotsFound = 0;

  memcpy( &Version[0],
          &VersionString[BEG_OF_1ST_VERS_SUBSTR_SP100],
          END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1 );
  Version[END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1] = 0;

  for ( pVal = &Version[0]; *pVal; ++pVal )
  {
      if ( '.' == *pVal )
      {
          ++dotsFound;
      }
      else if ( dotsFound == 2 )
      {
          /* If correction level is finally found add a leading zero, but only if needed */
          if ( '0' == *pVal && *(pVal+1) != ' ' )
          {
              *pVal = *(pVal+1); /* transfer the single digit */
              *(pVal+1) = ' ';  /* replace original digit with blank */
              memcpy( &VersionString[BEG_OF_1ST_VERS_SUBSTR_SP100],
                      &Version[0],
                      END_OF_1ST_VERS_SUBSTR_SP100 - BEG_OF_1ST_VERS_SUBSTR_SP100 + 1 );
          }
          break;
      }
  }
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
