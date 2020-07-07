/*!**********************************************************************

  module: vpa83staticNT.c

  -----------------------------------------------------------------------

  responsible:  ThomasK

  special area: ODBC-Driver

  description:  missing objects for static linking under WinNT (PTS 1109265)
                NOTE: Under Windows usually the ODBC driver manager
                      is used, i.e. dynamically linking of ODBC drivers
                      is the default. Therefore, static linking of the
                      ODBC driver serves only for very special applications
                      and lacks many of the ODBC connections features
                      that the driver manager offers.
                      

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




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




************************************************************************/

/* Windows only */
#ifdef WIN32

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include "vpa00global.h"


/* globals */
UCHAR pa30_sql_driver_name[128] = "";
UCHAR pa30_sql_driver_ver[64] = "";
UCHAR pa30_sql_dbms_name[128] = "";
UCHAR pa30_sql_dbms_ver[64] = "";
HANDLE s_hModule = NULL; /* from vpa30wc; used to get driver path */




/* compare vpa40DBC.c */
tsp00_Bool SqlIsSapRouterString (unsigned char *node)
{
    return (false);
} /* SqlIsSapRouterString */


/* compare vpa08msc and <odbcinst.h>, respectively */
SWORD __stdcall SQLGetPrivateProfileString (UCHAR *lpszSection,
                                  UCHAR *lpszEntry,
                                  UCHAR *lpszDefault,
                                  UCHAR *lpszReturnBuffer,
                                  SWORD  cbReturnBuffer,
                                  UCHAR *lpszFileName)
{
    /* dummy, never called in static linking 
       only SQLDriverConnect is supported where all needed
       information is passed to ODBC */
    return (0);
}

/* compare vpa08msc and <odbcinst.h>, respectively */
SWORD __stdcall SQLGetPrivateProfileStringW (UCHAR *lpszSection,
                                  UCHAR *lpszEntry,
                                  UCHAR *lpszDefault,
                                  UCHAR *lpszReturnBuffer,
                                  SWORD  cbReturnBuffer,
                                  UCHAR *lpszFileName)
{
    /* dummy, never called in static linking 
       only SQLDriverConnect is supported where all needed
       information is passed to ODBC */
    return (0);
}

/* compare vpa31DiagRec.c */
unsigned int LoadString_static (SQLUSMALLINT sqlState,
                                UCHAR        *buffer,
                                unsigned int length)
{
    static api_text_tab textTab [] = {
            API_ODBC_00000, API_TEXT_00000,
            API_ODBC_01002, API_TEXT_01002,
            API_ODBC_01004, API_TEXT_01004,
            API_ODBC_01006, API_TEXT_01006,
            API_ODBC_01S00, API_TEXT_01S00,
            API_ODBC_07001, API_TEXT_07001,
            API_ODBC_07006, API_TEXT_07006,
            API_ODBC_08001, API_TEXT_08001,
            API_ODBC_08002, API_TEXT_08002,
            API_ODBC_08003, API_TEXT_08003,
            API_ODBC_08004, API_TEXT_08004,
            API_ODBC_08007, API_TEXT_08007,
            API_ODBC_08S01, API_TEXT_08S01,
            API_ODBC_21S01, API_TEXT_21S01,
            API_ODBC_21S02, API_TEXT_21S02,
            API_ODBC_22001, API_TEXT_22001,
            API_ODBC_22003, API_TEXT_22003,
            API_ODBC_22005, API_TEXT_22005,
            API_ODBC_22008, API_TEXT_22008,
            API_ODBC_22012, API_TEXT_22012,
            API_ODBC_22026, API_TEXT_22026,
            API_ODBC_23000, API_TEXT_23000,
            API_ODBC_24000, API_TEXT_24000,
            API_ODBC_25000, API_TEXT_25000,
            API_ODBC_28000, API_TEXT_28000,
            API_ODBC_34000, API_TEXT_34000,
            API_ODBC_37000, API_TEXT_37000,
            API_ODBC_3C000, API_TEXT_3C000,
            API_ODBC_40001, API_TEXT_40001,
            API_ODBC_42000, API_TEXT_42000,
            API_ODBC_70100, API_TEXT_70100,
            API_ODBC_IM001, API_TEXT_IM001,
            API_ODBC_IM002, API_TEXT_IM002,
            API_ODBC_IM003, API_TEXT_IM003,
            API_ODBC_IM004, API_TEXT_IM004,
            API_ODBC_IM005, API_TEXT_IM005,
            API_ODBC_IM006, API_TEXT_IM006,
            API_ODBC_IM007, API_TEXT_IM007,
            API_ODBC_IM008, API_TEXT_IM008,
            API_ODBC_IM009, API_TEXT_IM009,
            API_ODBC_S0001, API_TEXT_S0001,
            API_ODBC_S0002, API_TEXT_S0002,
            API_ODBC_S0011, API_TEXT_S0011,
            API_ODBC_S0012, API_TEXT_S0012,
            API_ODBC_S0021, API_TEXT_S0021,
            API_ODBC_S0022, API_TEXT_S0022,
            API_ODBC_S1000, API_TEXT_S1000,
            API_ODBC_S1001, API_TEXT_S1001,
            API_ODBC_S1002, API_TEXT_S1002,
            API_ODBC_S1003, API_TEXT_S1003,
            API_ODBC_S1004, API_TEXT_S1004,
            API_ODBC_S1008, API_TEXT_S1008,
            API_ODBC_S1009, API_TEXT_S1009,
            API_ODBC_S1010, API_TEXT_S1010,
            API_ODBC_S1012, API_TEXT_S1012,
            API_ODBC_S1015, API_TEXT_S1015,
            API_ODBC_S1090, API_TEXT_S1090,
            API_ODBC_S1091, API_TEXT_S1091,
            API_ODBC_S1092, API_TEXT_S1092,
            API_ODBC_S1093, API_TEXT_S1093,
            API_ODBC_S1094, API_TEXT_S1094,
            API_ODBC_S1095, API_TEXT_S1095,
            API_ODBC_S1096, API_TEXT_S1096,
            API_ODBC_S1097, API_TEXT_S1097,
            API_ODBC_S1098, API_TEXT_S1098,
            API_ODBC_S1099, API_TEXT_S1099,
            API_ODBC_S1100, API_TEXT_S1100,
            API_ODBC_S1101, API_TEXT_S1101,
            API_ODBC_S1102, API_TEXT_S1102,
            API_ODBC_S1103, API_TEXT_S1103,
            API_ODBC_S1106, API_TEXT_S1106,
            API_ODBC_S1107, API_TEXT_S1107,
            API_ODBC_S1108, API_TEXT_S1108,
            API_ODBC_S1109, API_TEXT_S1109,
            API_ODBC_S1110, API_TEXT_S1110,
            API_ODBC_S1C00, API_TEXT_S1C00,
            API_ODBC_S1DE0, API_TEXT_S1DE0,
            API_ODBC_S1T00, API_TEXT_S1T00,
            API_ODBC_01S02, API_TEXT_01S02,
            API_ODBC_S1011, API_TEXT_S1011,
            API_ODBC_01000, API_TEXT_01000,
            API_ODBC_01S01, API_TEXT_01S01,
            API_ODBC_01S03, API_TEXT_01S03,
            API_ODBC_01S04, API_TEXT_01S04,
            API_ODBC_01S05, API_TEXT_01S05,
            /* ODBC 3.x states */
            API_ODBC_01001,       API_TEXT_01001,
            API_ODBC_HY019,       API_TEXT_HY019,
            API_ODBC_22007,       API_TEXT_22007,
            API_ODBC_22018,       API_TEXT_22018,
            API_ODBC_07005,       API_TEXT_07005,
            API_ODBC_HY018,       API_TEXT_HY018,
            API_ODBC_42S01,       API_TEXT_42S01,
            API_ODBC_42S02,       API_TEXT_42S02,
            API_ODBC_42S11,       API_TEXT_42S11,
            API_ODBC_42S12,       API_TEXT_42S12,
            API_ODBC_42S21,       API_TEXT_42S21,
            API_ODBC_42S22,       API_TEXT_42S22,
            API_ODBC_HY000,       API_TEXT_HY000,
            API_ODBC_HY001,       API_TEXT_HY001,
            API_ODBC_07009,       API_TEXT_07009,
            API_ODBC_HY003,       API_TEXT_HY003,
            API_ODBC_HY004,       API_TEXT_HY004,
            API_ODBC_HY008,       API_TEXT_HY008,
            API_ODBC_HY009,       API_TEXT_HY009,
            API_ODBC_HY024,       API_TEXT_HY024,
            API_ODBC_HY092,       API_TEXT_HY092,
            API_ODBC_HY007,       API_TEXT_HY007,
            API_ODBC_HY010,       API_TEXT_HY010,
            API_ODBC_HY011,       API_TEXT_HY011,
            API_ODBC_HY012,       API_TEXT_HY012,
            API_ODBC_HY090,       API_TEXT_HY090,
            API_ODBC_HY091,       API_TEXT_HY091,
            API_ODBC_HY096,       API_TEXT_HY096,
            API_ODBC_HY097,       API_TEXT_HY097,
            API_ODBC_HY098,       API_TEXT_HY098,
            API_ODBC_HY099,       API_TEXT_HY099,
            API_ODBC_HY100,       API_TEXT_HY100,
            API_ODBC_HY101,       API_TEXT_HY101,
            API_ODBC_HY103,       API_TEXT_HY103,
            API_ODBC_HY104,       API_TEXT_HY104,
            API_ODBC_HY105,       API_TEXT_HY105,
            API_ODBC_HY106,       API_TEXT_HY106,
            API_ODBC_HY107,       API_TEXT_HY107,
            API_ODBC_HY109,       API_TEXT_HY109,
            API_ODBC_HY110,       API_TEXT_HY110,
            API_ODBC_HY111,       API_TEXT_HY111,
            API_ODBC_HYC00,       API_TEXT_HYC00,
            API_ODBC_HYT00,       API_TEXT_HYT00,
            API_ODBC_01S07,       API_TEXT_01S07,
            API_ODBC_01S08,       API_TEXT_01S08,
            API_ODBC_01S09,       API_TEXT_01S09,
            API_ODBC_07002,       API_TEXT_07002,
            API_ODBC_07S01,       API_TEXT_07S01,
            API_ODBC_22015,       API_TEXT_22015,
            API_ODBC_22019,       API_TEXT_22019,
            API_ODBC_22025,       API_TEXT_22025,
            API_ODBC_25S01,       API_TEXT_25S01,
            API_ODBC_25S02,       API_TEXT_25S02,
            API_ODBC_25S03,       API_TEXT_25S03,
            API_ODBC_40002,       API_TEXT_40002,
            API_ODBC_40003,       API_TEXT_40003,
            API_ODBC_44000,       API_TEXT_44000,
            API_ODBC_HY013,       API_TEXT_HY013,
            API_ODBC_HY014,       API_TEXT_HY014,
            API_ODBC_HY015,       API_TEXT_HY015,
            API_ODBC_HY016,       API_TEXT_HY016,
            API_ODBC_HY017,       API_TEXT_HY017,
            API_ODBC_HY020,       API_TEXT_HY020,
            API_ODBC_HY021,       API_TEXT_HY021,
            API_ODBC_HY095,       API_TEXT_HY095,
            API_ODBC_HYT01,       API_TEXT_HYT01,
            API_ODBC_HY108,       API_TEXT_HY108,
            API_ODBC_IM010,       API_TEXT_IM010,
            API_ODBC_IM011,       API_TEXT_IM011,
            API_ODBC_IM012,       API_TEXT_IM012,
            API_ODBC_IM013,       API_TEXT_IM013,
            API_ODBC_IM014,       API_TEXT_IM014,
            API_ODBC_IM015,       API_TEXT_IM015,
            API_ODBC_01S10,       API_TEXT_01S10,
            API_ODBC_22002,       API_TEXT_22002
    };
        /* does not totally check for */
        /* an access beyond the array!*/
#ifdef DEBUG
        {
            UWORD i;
            for (i=0; i<(UWORD)(sizeof(text_tab)/sizeof(api_text_tab)); i++) {
                if (i != textTab[i].key) {
                    API_TRACE(API_TR_UWORD, "invalid text_tab in LL_static", &i);
                    API_EXIT(99);
                } /* if */
            } /* for */
        }
#endif /* DEBUG */
        if (sqlState == textTab[sqlState].key ) {
            API_STRCPY( (UCHAR *) buffer,
                        (UCHAR *) textTab[sqlState].text);
            length = (SWORD) API_STRLEN(buffer);
        } else {
            length = 0;
        } /* else */
        
        return (length);
}



#endif  /* of WIN32 */
