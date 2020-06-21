/*!
  @file           veo46.c
  @author         JoergM
  @special area   error handling
  @brief          common routines for rte error handling"
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo46.h"
#include "heo47.h"
#include "geo00_0.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__ "VEO46.C:"

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
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static int eo46_last_error (void);

static void eo46_strncpy   (char       * target,
                            const char * source,
                            int          targetSize);

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


void
sqlos_errortxtc( char         *ErrorText,
                 int           ErrorTextLen )
{
    eo46_system_error_text (eo46_last_error (),
                              ErrorText, ErrorTextLen) ;
}

/*-----------------------------------------------------------*/
void sqlos_errcode_and_errtext( int         *ErrorCode,          
                                char        *ErrorText,
                                int          ErrorTextLen )
{
    *ErrorCode = eo46_last_error ();
    eo46_system_error_text (*ErrorCode, ErrorText, ErrorTextLen) ;
}

/*-----------------------------------------------------------*/

void sqlos_errtext_by_errcode( int          Errcode,          
                               char        *ErrorText,
                               int          ErrorTextLen )
{
    eo46_system_error_text (Errcode, ErrorText, ErrorTextLen) ;
}

/*-----------------------------------------------------------*/

char *eo46_system_error_text ( int           ErrorCode,
                               char         *ErrorText,
                               int           ErrorTextLen )
{
  int   ErrorTextOk;
  char  line[100] ;
  char  *TxtPtr = line;

#ifdef WIN32

  ErrorTextOk = FormatMessage( 
                   FORMAT_MESSAGE_FROM_SYSTEM ,
                   NULL,
                   ErrorCode,
                   0,
                   (LPTSTR) line,
                   sizeof ( line ),
                   NULL );

  if ( ErrorTextOk && strchr( line, 0x0d ) )
    *strchr( line, 0x0d ) = '\0';

#elif defined(LINUX) || defined(FREEBSD) || defined(AIX)
  ErrorTextOk = ErrorCode > 0;
  if ( ErrorTextOk )
  {
  	ErrorTextOk = ( 0 == strerror_r( ErrorCode, ErrorText, ErrorTextLen ) );
  }
#elif defined(SUN) || defined(SOLARIS) || defined(HPUX) 
  ErrorTextOk = ErrorCode > 0;
  if ( ErrorTextOk )
  {
  	TxtPtr = strerror( ErrorCode );
    /* PTS 1104192 */
    ErrorTextOk = ( TxtPtr != NULL );
  }
#else
  ErrorTextOk = ( (ErrorCode > 0) && (ErrorCode <= sys_nerr) );
  if ( ErrorTextOk )
  	TxtPtr = (char *)sys_errlist [ ErrorCode ] ;
#endif

  if ( !ErrorTextOk )
  {
    char   ErrCodeStr [10];

    sql47_itoa(ErrorCode, ErrCodeStr, sizeof(ErrCodeStr) ) ;
	strcpy ( line , "No system errortext for ERRNO " );
    strcat ( line, ErrCodeStr ) ;
  }

  if ( strlen(TxtPtr) >= (unsigned int)ErrorTextLen )
  { 
    strncpy ( ErrorText, TxtPtr, ErrorTextLen-1 ) ;
    ErrorText [ ErrorTextLen-1 ] = '\0' ; 
  }
  else
    strcpy ( ErrorText, TxtPtr) ;

  return ( ErrorText ) ;
}


/*-----------------------------------------------------------*/

char *eo46_rte_errtext_with_filename ( const char       *ErrTxt,
                                       const char       *Filename,
                                       char             *FullTxt,
                                       int              FullTxtLen)
{
  int LenTxt = (int)strlen(ErrTxt) ;

  if ( LenTxt < FullTxtLen )
    { 
      int LenName = (int)strlen ( Filename ) ;
      strcpy ( FullTxt, ErrTxt ) ;
      if ( LenTxt + LenName < FullTxtLen )
        strcat( FullTxt, Filename) ;
      else
        strncat (  FullTxt, Filename, FullTxtLen - LenTxt );
    }
  else
    strncpy ( FullTxt, ErrTxt, LenTxt-1 ) ;
  return ( FullTxt ) ;
}


/*-----------------------------------------------------------*/

void eo46_rte_error_init ( tsp01_RteError *RteError)
{
  memset(RteError, 0, sizeof(tsp01_RteError) ) ;
  RteError->RteErrCode = RTE_NO_ERROR_SP01 ;
  RteError->OsErrCode  = OS_NO_ERROR_SP01 ;
}

/*-----------------------------------------------------------*/

void eo46_set_rte_error ( tsp01_RteError *RteError,
                          int             OsErrCode,
                          const char     *ErrText,
                          const char     *Argument)
{
    if (OsErrCode == undef_ceo00 )
    {
        OsErrCode = eo46_last_error ();
    }
    if (ErrText == NULL)
    {
        ErrText = "";
    }
    RteError->RteErrCode = RTE_ERROR_SP01;
    if (Argument == NULL)
    {
        eo46_strncpy (RteError->RteErrText, ErrText,
                sizeof (RteError->RteErrText));
    }
    else
    {
        eo46_rte_errtext_with_filename (ErrText, Argument,
                RteError->RteErrText, sizeof (RteError->RteErrText));
    }
    RteError->OsErrCode  = OsErrCode ;
    eo46_system_error_text ( RteError->OsErrCode,
            RteError->OsErrText,
            sizeof ( RteError->OsErrText) ) ;
    
}

/*-----------------------------------------------------------*/

void eo46PtoC ( char             *CStr,
                tsp00_Byte const *PascalStr,
                int               Len )
{

  teo00_Byte const *PtrP = PascalStr + Len - 1 ;
  char *PtrC             = CStr + Len ;

  *PtrC-- = '\0';

  while (PtrP >= PascalStr && *PtrP == ' ')
  {
    *PtrC-- = '\0';
    PtrP--;
  }

  while (PtrP >= PascalStr)
    *PtrC-- = *PtrP--;

}

/*-----------------------------------------------------------*/

void eo46CtoP ( tsp00_Byte      *PascalStr,
                char const      *CStr,
                int             Len )
{
  while (Len-- > 0)
    if (*CStr == '\0')
      *PascalStr++ = ' ';
    else
      *PascalStr++ = *CStr++;
}

/*-----------------------------------------------------------*/

void  eo46BuildErrorStringRC ( tsp00_ErrTextc   pcErrText,
                               char const      *pszErrText,
                               teo00_Int4       rc )
{
  char   szTmpErrText [sizeof(tsp00_ErrText) + sizeof (teo00_Int4)];
  char   szRCErrText  [32 + sizeof (teo00_Int4)];

  if ( rc > 0)
  {
    sql47_itoa(rc, szRCErrText, sizeof(szRCErrText) ) ;
    strncpy (szTmpErrText, pszErrText, sizeof(tsp00_ErrText) - strlen (szRCErrText) - 3);
    szTmpErrText [ sizeof(tsp00_ErrText) - strlen (szRCErrText) - 3 ] = '\0';
    strcat (szTmpErrText, " [");
    strcat (szTmpErrText, szRCErrText);
    strcat (szTmpErrText, "]");
    strncpy( pcErrText, szTmpErrText, sizeof(tsp00_ErrText) );
  }
  else
    strncpy ( pcErrText, pszErrText, sizeof(tsp00_ErrText) );

  pcErrText[sizeof(tsp00_ErrText)] = '\0' ;
  return;
}
/*-----------------------------------------------------------*/

void  eo46BuildPascalErrorStringRC ( tsp00_Byte      *pcPascalErrText,
                                     char  const     *pszErrText,
                                     teo00_Int4      rc )
{
  tsp00_ErrTextc ErrTextC ;

  eo46BuildErrorStringRC(ErrTextC, pszErrText, rc);
  eo46CtoP ( pcPascalErrText, ErrTextC, sizeof(tsp00_ErrText) );

  return;
}

/*-----------------------------------------------------------*/
void  sql46c_build_error_string ( char       *pcPascalErrText,
                                  char const *pszErrText,
                                  teo00_Int4  rc )
{
  eo46BuildPascalErrorStringRC ( (teo00_Byte *)pcPascalErrText, pszErrText, rc ) ;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*-----------------------------------------------------------*/

static int eo46_last_error (void)
{
#if defined (_WIN32)
        return GetLastError ();
#else
        return errno;
#endif
}

/*-----------------------------------------------------------*/

static void eo46_strncpy (char * target,
                          const char * source,
                          int    targetSize)
{
    strncpy (target, source, targetSize);
    target [targetSize - 1] = '\0';
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

