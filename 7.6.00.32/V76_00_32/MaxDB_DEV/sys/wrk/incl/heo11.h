/*!
  @file           heo11.h
  @author         JoergM
  @special area   xparam
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



#ifndef HEO11_H
#define HEO11_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp01.h"
#include "gsp11.h"
#include "heo01.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES ( pascal only )                                      *
 *===========================================================================*/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#if defined (__cplusplus)
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

externC void sqldelxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType&		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlgetxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType&		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpValueRec*		xp_value,
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlgnextxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType&		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpKeyType&		xp_next_key,		/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpValueRec*		xp_value,
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlputxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType&		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpValueRec*		xp_value,
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlxpclosexparam (
	tsp00_Int4				xp_fileno,
	char					flush_buffer,
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	bool*					ok);

externC void sqlxpopenxparam (
	tsp00_DbName&				server_db,			/* tsp00_Array<char, mxsp_c18> */
	tsp00_Int4				max_params,
	tsp00_Int4*				xp_fileno,
	tsp00_ErrText&			errtext,			/* tsp00_Array<char, mxsp_c40> */
	bool*					ok);

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#else
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

externC void sqldelxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType 		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_ErrText 			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlgetxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType 		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpValueRec*		xp_value,
	tsp00_ErrText 			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlgnextxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType 		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpKeyType 		xp_next_key,		/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpValueRec*		xp_value,
	tsp00_ErrText 			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlputxparam (
	tsp00_Int4				xp_fileno,
	tsp00_XpKeyType 		xp_key,				/* tsp00_Array<char, mxsp_c18> */
	tsp00_XpValueRec*		xp_value,
	tsp00_ErrText 			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_XpReturnCode*		xp_ret);			/* tsp00_Enum<tsp_xp_return_code_Enum, ...> */

externC void sqlxpclosexparam (
	tsp00_Int4				xp_fileno,
	tsp00_Bool  			flush_buffer,
	tsp00_ErrText 			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_BoolAddr			ok);

externC void sqlxpopenxparam (
	tsp00_DbName 				server_db,			/* tsp00_Array<char, mxsp_c18> */
	tsp00_Int4				max_params,
	tsp00_Int4*				xp_fileno,
	tsp00_ErrText 			errtext,			/* tsp00_Array<char, mxsp_c40> */
	tsp00_BoolAddr  		ok);

#endif


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#if defined (__cplusplus)
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
externC tsp11_XpReturnCode
        sqlXparamOpen   ( tsp00_DbNamec      &acServerDB,
                          tsp00_Int4         *plXParamFd,
                          tsp00_Int4          OpenMode,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamClose  ( tsp00_Int4          lXParamFd,
                          tsp00_Bool          fFlush,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamPut    ( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec   &acXParamKey,
                          tsp00_XpValueRec   *prcXParamValue,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
         sqlXparamGet   ( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec   &acXParamKey,
                          tsp00_XpValueRec   *prcXParamValue,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamGetNext( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec   &acXParamKey,
                          tsp00_XpKeyTypec   &acXParamNextKey,
                          tsp00_XpValueRec   *prcXParamValue,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamDel    ( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec   &acXParamKey,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        eo11ResetXparamChangedFlag ( tsp00_DbNamec   &DBNameC,
                                     tsp01_RteError  *RteError );


externC tsp11_XpReturnCode 
        eo11PutParam    ( tsp00_DbNamec      &DBNameC ,
                          tsp11_XpKeyTypec   &XpKeyC,
                          tsp00_XpValueRec   *XpValue,
                          tsp01_RteError     *RteError );


externC tsp00_Bool 
        sqlXParamGetServiceParamFilename ( tsp00_Pathc    &ServiceParamFilename,
                                           tsp01_RteError  *RteError ) ;

externC tsp00_Bool 
        sqlXParamCheckExists ( tsp00_DbNamec   &DBNameC) ;


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#else
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
externC tsp11_XpReturnCode
        sqlXparamOpen   ( tsp00_DbNamec       acServerDB,
                          tsp00_Int4         *plXParamFd,
                          tsp00_Int4          OpenMode,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamClose  ( tsp00_Int4          lXParamFd,
                          tsp00_Bool          fFlush,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamPut    ( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec    acXParamKey,
                          tsp00_XpValueRec   *prcXParamValue,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
         sqlXparamGet   ( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec    acXParamKey,
                          tsp00_XpValueRec   *prcXParamValue,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamGetNext( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec    acXParamKey,
                          tsp00_XpKeyTypec    acXParamNextKey,
                          tsp00_XpValueRec   *prcXParamValue,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        sqlXparamDel    ( tsp00_Int4          lXParamFd,
                          tsp00_XpKeyTypec    acXParamKey,
                          tsp01_RteError     *RteError );

externC tsp11_XpReturnCode 
        eo11ResetXparamChangedFlag ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                     tsp01_RteError               *RteError );


externC tsp11_XpReturnCode 
        eo11PutParam    ( tsp00_DbNamec     VAR_ARRAY_REF  DBNameC ,
                          tsp11_XpKeyTypec  VAR_ARRAY_REF  XpKeyC,
                          tsp00_XpValueRec                *XpValue,
                          tsp01_RteError                  *RteError );


externC tsp00_Bool 
        sqlXParamGetServiceParamFilename ( tsp00_Pathc   VAR_ARRAY_REF  ServiceParamFilename,
                                           tsp01_RteError              *RteError ) ;

externC tsp00_Bool 
        sqlXParamCheckExists ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC) ;

externC tsp00_Bool sqlXparamFsize ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                    tsp00_Int4                   *Fsize,
                                    tsp01_RteError               *RteError );


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#endif /* defined (__cplusplus) */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

externC tsp11_XpReturnCode 
        eo11PutParamWithoutChgFlag ( tsp00_DbNamec    VAR_ARRAY_REF  DBNameC ,
                                     tsp11_XpKeyTypec VAR_ARRAY_REF  XpKeyC,
                                     tsp00_XpValueRec               *XpValue,
                                     tsp01_RteError                 *RteError );

externC tsp11_XpReturnCode 
        sqlXparamOpenVersion   ( tsp00_DbNamec  VAR_ARRAY_REF    DBNameC,
                                 tsp00_Int4                      VersionToOpen,
		  	 		             tsp00_Int4                     *pXparamId,
		  	 		             tsp00_Int4                      OpenMode,
                                 tsp01_RteError                 *RteError );

externC tsp00_Bool 
        sqlXParamRemoveAll  ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                              tsp01_RteError               *RteError ) ;
 
externC tsp00_Bool 
        sqlXParamRemoveAllVersions ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                     tsp01_RteError               *RteError) ;

externC tsp00_Bool 
        sqlXParamRemoveOneParamfile ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                      tsp00_Int4                    VersionToRemove,
                                      tsp01_RteError               *RteError);

externC tsp00_Bool  
        eo11SaveActParamFileAsVersion ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                        tsp01_RteError               *RteError);

/*===========================================================================*
 *  old style FUNCTION PROTOTYPES moved from veo001c and have to be changed  *
 *===========================================================================*/

typedef tsp11_XpKeyType            XP_KEY_TYPE ;
typedef tsp11_XpKeyTypec           XP_KEY_TYPEC ;

struct          xp_value_record
{
        tsp00_Int2        xp_info ;
        BOOLEAN         xp_fill2 ;
        unsigned char   xp_value_t ;
        union           of_xp_value_t
        {
                tsp00_Int2        xp_int2 ;
                tsp00_Int4        xp_int4 ;
                tsp00_C8          xp_c8 ;
                tsp00_C18         xp_c18 ;
                tsp00_C24         xp_c24 ;
                tsp00_C40         xp_c40 ;
                tsp00_C64         xp_c64 ;
                tsp00_CryptName   xp_crypt ;
        }  xp_type ;
 }  ;

typedef struct xp_value_record                  XP_VALUE_REC ;

typedef XP_VALUE_REC*                           XP_PVALUE_REC;
/* --- xp_key --- */

#define XP_HEADER_0             "XP_FST_XP_HEADER_0"
#define XP_DEL_KEY              "XP_FST_XP_DELETE__"
#define XP_PARAM_CHANGED        "__PARAM_CHANGED___"
#define XP_PARAM_VERSION_TIME   "__PARAM_VERIFIED__"
#define XP_FLUSH               1


/* --- xp_value_type --- */

#define XP_INT2_TYPE            0
#define XP_INT4_TYPE            1
#define XP_C8_TYPE              2
#define XP_C18_TYPE             3
#define XP_C24_TYPE             4
#define XP_C40_TYPE             5
#define XP_C64_TYPE             6
#define XP_CRYPT_TYPE           7

/* --- xp_ret_code --- */

#define XP_OK                   0
#define XP_KEY_NOT_FOUND        1
#define XP_EOF                  2
#define XP_NOT_OK               3

/* --- xp MACRO --- */

#define CMP_XP_TYPE(_Val1, _Val2) \
        ( memcmp(&_Val1, &_Val2, sizeof(_Val1) ) == 0 )

#define IS_XP_TYPE_EQ( _Key1, _Key2) \
        ( memcmp ( _Key1, _Key2, sizeof(XP_KEY_TYPE) ) == 0 )

#define IS_XP_VALUE_EQ(_Val1, _Val2) \
        ( ( _Val1.xp_value_t == XP_INT2_TYPE && _Val1.xp_type.xp_int2 == _Val2.xp_type.xp_int2 ) || \
          ( _Val1.xp_value_t == XP_INT4_TYPE && _Val1.xp_type.xp_int4 == _Val2.xp_type.xp_int4 ) || \
            CMP_XP_TYPE(_Val1.xp_type.xp_c64 , _Val2.xp_type.xp_c64 ) )

#define IS_PARAM_EQ( _Param1, _Param2 ) \
        ( IS_XP_TYPE_EQ( _Param1, _Param2 ) && \
          IS_XP_VALUE_EQ ( ( (XP_VALUE_REC *) (_Param1 + sizeof(XP_KEY_TYPE)) ), \
                           ( (XP_VALUE_REC *) (_Param2 + sizeof(XP_KEY_TYPE)) ) ) )

#define IS_DEL_KEY(key_ptr) \
		(!memcmp ( XP_DEL_KEY , key_ptr , sizeof ( XP_KEY_TYPE ) ) )

#define IS_HEADER_KEY(key_ptr) \
		(!memcmp ( XP_HEADER_0 , key_ptr , sizeof ( XP_KEY_TYPE ) ) )

#define XP_HFILE                        tsp00_Int4 
#define XP_PHFILE                       XP_HFILE *
#define XP_RETURNCODE                   tsp00_XpReturnCode 
#define XP_PRETURNCODE                  XP_RETURNCODE *


externC void   sql11c_OpenXParam    (   tsp00_Pathc       acServerDB,
                                        XP_PHFILE         plXParamFd,
                                        tsp00_ErrText       acErrorText,
                                        tsp00_Int4          OpenMode,
                                        tsp00_BoolAddr    pbOk                    );

externC void   sql11c_CloseXParam    (  XP_HFILE          lXParamFd,
                                        BOOLEAN           fFlush,
                                        tsp00_ErrText       acErrorText,
                                        tsp00_BoolAddr    pbOk                    );

externC XP_HFILE  sql11c_InitXparamStruc (  char     *XparamData,
                                            tsp00_Int4  XparamSize );

externC void     sql11c_FreeXparamStruc  (   XP_HFILE        lXParamFd );

externC void     sql11c_RenameXParam     (  XP_HFILE        lXParamFd,
                                            XP_KEY_TYPEC    acXParamOldKeyC,
                                            XP_KEY_TYPEC    acXParamNewKeyC,
                                            tsp00_ErrText     acErrorText,
                                            XP_PRETURNCODE  pucXParamReturnCode);

externC void     sql11c_PutXParam        (  XP_HFILE        lXParamFd,
                                            XP_KEY_TYPEC    acXParamKey,
                                            XP_PVALUE_REC   prcXParamValue,
                                            tsp00_ErrText     acErrorText,
                                            XP_PRETURNCODE  pucXParamReturnCode );

externC void      sql11c_GetXParam       (  XP_HFILE        lXParamFd,
                                            XP_KEY_TYPEC    acXParamKey,
                                            XP_PVALUE_REC   prcXParamValue,
                                            tsp00_ErrText     acErrorText,
                                            XP_PRETURNCODE  pucXParamReturnCode );

externC void      sql11c_GetNextXParam    ( XP_HFILE        lXParamFd,
                                            XP_KEY_TYPEC    acXParamKey,
                                            XP_KEY_TYPEC    acXParamNextKey,
                                            XP_PVALUE_REC   prcXParamValue,
                                            tsp00_ErrText     acErrorText,
                                            XP_PRETURNCODE  pucXParamReturnCode );

externC void      sql11c_SeqNextXParam    ( XP_HFILE        lXParamFd,
                                            XP_KEY_TYPE     acXParamKey,
                                            XP_PVALUE_REC   prcXParamValue,
                                            tsp00_ErrText     acErrorText,
                                            XP_PRETURNCODE  pucXParamReturnCode );

externC void      sql11c_DeleteXParam     ( XP_HFILE        lXParamFd,
                                            XP_KEY_TYPEC    acXParamKey,
                                            tsp00_ErrText     acErrorText,
                                            XP_PRETURNCODE  pucXParamReturnCode );
#endif  /* HEO11_H */
