#ifndef DEBUG
#line 21 "vpa08msc"
#endif

#include "vpa40DBC.h"

#include "vpr05IfCom_String.h"
#include "hsp78_0.h"

typedef struct ComboBoxListStruct {
   UCHAR Item[19];
   SDWORD ItemID;
   SWORD Default;
} pa08ComboBoxListType;

/* Prototypes */
API_RETCODE pa08FromListGetString(pa08ComboBoxListType *pList, SDWORD cbValue, UCHAR FAR *szOutStr, SWORD cbOutStrMax);
API_RETCODE pa08FromListGetInt4(pa08ComboBoxListType *pList, SQLWCHAR *szInStr, SDWORD *cbValue);
API_RETCODE pa08GetDSNOptions (tpa40DBC      *dbc_ptr, 
                               tpr05_String  *profileStr,
                               const tsp77encoding *encodingType);
BOOL pa08GetODBCProfile (UCHAR FAR *section,
                         UCHAR FAR *entry,
                         UCHAR FAR *profstr_ptr,
                         SWORD profstr_max,
                         SWORD FAR *profstr_len,
                         SWORD FAR *default_dsn,
                         tpr05_String *profileStr);
BOOL pa08GetODBCProfileW (SQLWCHAR FAR *section,
                          SQLCHAR  FAR *entry,
                          SQLWCHAR FAR *profstr_ptr,
                          SWORD profstr_max,
                          SWORD FAR *profstr_len,
                          SWORD FAR *default_dsn,
                          SQLWCHAR FAR *szProfileStr);

SWORD pa08GetProfileString (UCHAR FAR *lpszSection,
                            UCHAR FAR *lpszEntry,
                            UCHAR FAR *lpszDefault,
                            UCHAR FAR *lpszReturnBuffer,
                            SDWORD cbReturnBuffer,
                            UCHAR FAR *szProfileStr);
SWORD pa08GetProfileStringW ( SQLWCHAR *lpszSection, 
                              SQLWCHAR *lpszEntry, 
                              SQLWCHAR *lpszDefault,
                              SQLWCHAR *lpszReturnBuffer, 
                              SDWORD cbReturnBuffer, 
                              SQLWCHAR *szProfileStr);

UCHAR FAR* pa08compare(UCHAR FAR *s, UCHAR FAR *szString, char c);
VOID pa08DefaultConnection(tpa40DBC FAR * dbc_ptr);

SWORD pa08GetProfileString_Tpr05String(UCHAR FAR *lpszSection, UCHAR FAR *lpszEntry, UCHAR FAR *lpszDefault, tpr05_String *lpszReturnStr, UCHAR FAR *szProfileStr);
tsp81_UCS2Char* pa08compareW (tsp81_UCS2Char *s, tsp81_UCS2Char *szString, tsp81_UCS2Char *c);

void pa08UnqoutedStringtoUpper (tpr05_String *);
void pa08rmspace_tpr05String   (tpr05_String *);



