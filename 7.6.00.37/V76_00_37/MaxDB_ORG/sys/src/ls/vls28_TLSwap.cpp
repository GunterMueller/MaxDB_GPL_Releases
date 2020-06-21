/*! 
  -----------------------------------------------------------------------------
 
  module: vls28_TLSwap.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Server
 
  description:  Implementation of swap on pages for TABLELOAD for single table
                
  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 2000-2005 SAP AG-2002
 
  -----------------------------------------------------------------------------



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




*/
#include "gls00.h"
#include "hls07.h"
#include "hls98msg.h"
#include "hls99.h"
#include "gbd00.h"
#include "ggg00.h"

#include "hls31_odbc_Result.h"
#include "hls26_TETable.h"
#include "hls28_TLSwap.h"



//*****************************************************************************
//*
//*         L O C A L  F U N C T I O N S 
//*
//*****************************************************************************


/*
  -----------------------------------------------------------------------------
 
  function:     ls28_SwapShortStringRecords
  -----------------------------------------------------------------------------

  description:  Swaps all values on a short string page

  arguments:    pszBuffer     [IN]  - page as char buffer
                lRecCount     [IN]  - count of records on page
                pPtrListEntry [IN]  - array of positions (pointer list) of
                                       records on the page
                SSwap         [IN]  - source (extract db) swap kind
                DSwap         [IN]  - destination (target db) swap kind

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls28_SwapShortStringRecords(tsp00_Addr          pszBuffer,
                            tsp00_Int4          lRecCount,
                            tsp00_Int4          pPtrListEntry[],
                            tsp00_SwapKind_Enum SSwap,
                            tsp00_SwapKind_Enum DSwap);


/*
  -----------------------------------------------------------------------------
 
  function:     ls28_SwapPrimaryDataRecords
  -----------------------------------------------------------------------------

  description:  Swaps all values on a primary data page

  arguments:    pszBuffer     [IN]  - page as char buffer
                lRecCount     [IN]  - count of records on page
                pPtrListEntry [IN]  - array of positions (pointer list) of
                                       records on the page
                SSwap         [IN]  - source (extract db) swap kind
                DSwap         [IN]  - destination (target db) swap kind

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls28_SwapPrimaryDataRecords(tsp00_Addr          pszBuffer,
                            tsp00_Int4          lRecCount,
                            tsp00_Int4          pPtrListEntry[],
                            tsp00_SwapKind_Enum SSwap,
                            tsp00_SwapKind_Enum DSwap);


/*
  -----------------------------------------------------------------------------
 
  function:     ls28_SwapRecordHeader
  -----------------------------------------------------------------------------

  description:  Swaps all values of the record on a page

  arguments:    pszHeader     [IN]  - header as char buffer
                SSwap         [IN]  - source (extract db) swap kind
                DSwap         [IN]  - destination (target db) swap kind

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
inline void
ls28_SwapRecordHeader(tsp00_Addr pszHeader,
                      tsp00_SwapKind_Enum SSwap,
                      tsp00_SwapKind_Enum DSwap);





/*
  -----------------------------------------------------------------------------
  function:     ls28_SwapShortStringRecords
  -----------------------------------------------------------------------------
*/
static void
ls28_SwapShortStringRecords(tsp00_Addr          pszBuffer,
                            tsp00_Int4          lRecCount,
                            tsp00_Int4          pPtrListEntry[],
                            tsp00_SwapKind_Enum SSwap,
                            tsp00_SwapKind_Enum DSwap)
{
    ROUTINE_DBG_MEO00 ("ls28_SwapShortStringRecords");

    int i = 0;
    for (i; i < lRecCount; ++i)
    {
        // decrease by 1 because of pascal notation
        ls28_SwapRecordHeader(&pszBuffer[pPtrListEntry[i] - 1], SSwap, DSwap);
    }

    return;
}
// ls28_SwapShortStringRecords()


/*
  -----------------------------------------------------------------------------
  function:     ls28_SwapPrimaryDataRecords
  -----------------------------------------------------------------------------
*/
static void
ls28_SwapPrimaryDataRecords(tsp00_Addr      pszBuffer,
                            tsp00_Int4      lRecCount,
                            tsp00_Int4      pPtrListEntry[],
                            tsp00_SwapKind_Enum SSwap,
                            tsp00_SwapKind_Enum DSwap)
{
    ROUTINE_DBG_MEO00 ("ls28_SwapPrimaryDataRecords");

    tgg00_HeaderRec *pHeader   = NULL;
    tsp00_Addr       pszRecord = NULL;
    
    tsp00_Int2 lRecordLength  = 0;
    tsp00_Int2 lKeyLength     = 0;
    tsp00_Int2 lVarColCnt     = 0;      // count of the short var length columns in record (there may be long var length cols)
    tsp00_Int2 lVarColOffset  = 0;      // offset of the var length columns in record
    char       pszLongVarLength[2];

    tsp00_SwapKind_Enum CurrSwap  = ls07DefineSwap(NULL);
    
    int i = 0;
    int j = 0;
    for (i; i < lRecCount; ++i)
    {
        pszRecord = &pszBuffer[pPtrListEntry[i] - 1];           // decrease by 1 because of pascal notation
        
        pHeader = REINTERPRET_CAST(tgg00_HeaderRec*, pszRecord);
    
        if (CurrSwap == SSwap)      // save values for later use
        {
            lRecordLength = pHeader->hrecLen_gg00();
            lKeyLength    = pHeader->hrecKeyLen_gg00();
            lVarColOffset = pHeader->hrecVarcolOffset_gg00();
            lVarColCnt    = pHeader->hrecVarcolCnt_gg00();
        }

        ls28_SwapRecordHeader(pszRecord, SSwap, DSwap);
        
        if (CurrSwap == DSwap)      // save values for later use
        {
            lRecordLength = pHeader->hrecLen_gg00();
            lKeyLength    = pHeader->hrecKeyLen_gg00();
            lVarColOffset = pHeader->hrecVarcolOffset_gg00();
            lVarColCnt    = pHeader->hrecVarcolCnt_gg00();
        }
    
        pszRecord += cgg_rec_key_offset + lKeyLength + lVarColOffset;   // move pointer to variable columns

        // Skip variable long columns
        for (j = 0; j < lVarColCnt; ++j)
        {
            pszRecord += STATIC_CAST(unsigned char, *pszRecord) + sizeof(char);     // update ptr (+ length byte)
        }

        // swap the long variable long column length
        while ( (pszRecord - REINTERPRET_CAST(tsp00_Addr, pHeader) ) < lRecordLength )
        {
            // Save length value for later use;
            // length might not be 2Byte aligned - so do the assignment the hard way
            if (CurrSwap == SSwap)                      
            {
                pszLongVarLength[0] = pszRecord[0];
                pszLongVarLength[1] = pszRecord[1];
            }
            else
            {
                pszLongVarLength[0] = pszRecord[1];
                pszLongVarLength[1] = pszRecord[0];
            }

            ls07Swap2BCharInPlace(pszRecord, SSwap, DSwap);

            pszRecord += *(REINTERPRET_CAST(tsp00_Int2*, pszLongVarLength)) + sizeof(tsp00_Int2);
        }

        // Reinitialize
        lRecordLength  = 0;
        lKeyLength     = 0;
        lVarColCnt     = 0;
        lVarColOffset  = 0;

    }   // for loop

    return;
}
// ls28_SwapPrimaryDataRecords()


/*
  -----------------------------------------------------------------------------
  function:     ls28_SwapRecordHeader
  -----------------------------------------------------------------------------
*/
void
ls28_SwapRecordHeader(tsp00_Addr          pszHeader,
                      tsp00_SwapKind_Enum SSwap,
                      tsp00_SwapKind_Enum DSwap)
{

    ls07Swap2BCharInPlace(pszHeader, SSwap, DSwap);
    ls07Swap2BCharInPlace(pszHeader + sizeof(tsp00_Int2), SSwap, DSwap);
    ls07Swap2BCharInPlace(pszHeader + 2*sizeof(tsp00_Int2), SSwap, DSwap);
    ls07Swap2BCharInPlace(pszHeader + 3*sizeof(tsp00_Int2), SSwap, DSwap);

    return;
}



//*****************************************************************************
//*
//*         G L O B A L  F U N C T I O N S 
//*
//*****************************************************************************

/*
  -----------------------------------------------------------------------------
  function:     ls28SwapHeader
  -----------------------------------------------------------------------------
*/
void
ls28SwapHeader(tsp00_SwapKind        SourceSwap,       // swap from tableextract
               tls26_DescPageHeader *pHeader)
{
    ROUTINE_DBG_MEO00 ("ls28SwapHeader");

    tsp00_SwapKind_Enum CurrSwap = ls07DefineSwap(NULL);
    if (CurrSwap != SourceSwap)
    {
        pHeader->dphPageNo_ls26 = ls07Swap4BInt(pHeader->dphPageNo_ls26,
                                                SourceSwap,
                                                CurrSwap);

        pHeader->dphPageType_ls26 = STATIC_CAST(tls00_TEPageType, ls07Swap4BInt(pHeader->dphPageType_ls26,
                                                                                SourceSwap,
                                                                                CurrSwap));

        pHeader->dphSpecialID_ls26 = ls07Swap4BInt(pHeader->dphSpecialID_ls26,
                                                   SourceSwap,
                                                   CurrSwap);

        pHeader->dphMore_ls26 = ls07Swap4BInt(pHeader->dphMore_ls26,
                                              SourceSwap,
                                              CurrSwap);

        pHeader->dphFillerInt_ls26 = ls07Swap4BInt(pHeader->dphFillerInt_ls26,
                                                   SourceSwap,
                                                   CurrSwap);
    }

    return;
}
// ls28SwapHeader()


/*
  -----------------------------------------------------------------------------
  function:     ls28SwapPageValues
  -----------------------------------------------------------------------------
*/
void
ls28SwapPageValues(tsp00_Addr      pszBuffer,
                   tsp00_Int4      lPageSize,
                   tls00_TloadRec *pTLRec)
{
    ROUTINE_DBG_MEO00 ("ls28SwapPageValues");

    if (tsp00_SwapKind_Enum(pTLRec->tlrSwapFromDb_ls00) == tsp00_SwapKind_Enum(pTLRec->tlrTargetSwap_ls00) )
    {
        return;
    }

    tsp00_SwapKind_Enum  CurrSwap      = ls07DefineSwap(NULL);
    tsp00_Int4           lRecCount     = 0;
    tsp00_Int4          *pPtrListEntry = NULL;

    // define page ptr
    tbd_node *pPage = REINTERPRET_CAST(tbd_node*, pszBuffer);

    // Swap bottom value
    pPage->nd_bottom() = ls07Swap4BInt(pPage->nd_bottom(), pTLRec->tlrSwapFromDb_ls00,
                                                           pTLRec->tlrTargetSwap_ls00);

    // Swap record count value and save it for later use
    if (CurrSwap == pTLRec->tlrSwapFromDb_ls00)     // nd_record_cnt gives us the information on the
    {                                               // count of records on the page and on the count
        lRecCount = pPage->nd_record_cnt();         // of pointers in the pointer list
    }

    pPage->nd_record_cnt() = ls07Swap4BInt(pPage->nd_record_cnt(), pTLRec->tlrSwapFromDb_ls00, pTLRec->tlrTargetSwap_ls00);
    
    if (CurrSwap == pTLRec->tlrTargetSwap_ls00)
    {
        lRecCount = pPage->nd_record_cnt();
    }

    // Swap pointer list members and save it for use in records
    int i = 0;
    tsp00_Int4 lListPtrPos = lPageSize - PAGE_CHECK_TRAILER_MXGG00 - POINTERSIZE_BD00;

    pPtrListEntry = new tsp00_Int4[lRecCount];      // PtrListEntry contains members beginning at the end of the list!
    for (i = 0; i < lRecCount; ++i, (lListPtrPos -= POINTERSIZE_BD00))
    {
        if (CurrSwap == pTLRec->tlrSwapFromDb_ls00)
        {
            pPtrListEntry[i] = *(REINTERPRET_CAST(tsp00_Int4*, &pszBuffer[lListPtrPos]));
        }

        ls07Swap4BCharInPlace(&pszBuffer[lListPtrPos], pTLRec->tlrSwapFromDb_ls00,
                                                       pTLRec->tlrTargetSwap_ls00);
            
        if (CurrSwap == pTLRec->tlrTargetSwap_ls00)
        {
            pPtrListEntry[i] = *(REINTERPRET_CAST(tsp00_Int4*, &pszBuffer[lListPtrPos]));
        }
    }

    if (tptLongString_els00 != pTLRec->tlrPageTypeExtracting_ls00)
    {
        if (tptShortString_els00 == pTLRec->tlrPageTypeExtracting_ls00)
        {
            ls28_SwapShortStringRecords(pszBuffer,
                                        lRecCount,
                                        pPtrListEntry,
                                        pTLRec->tlrSwapFromDb_ls00,
                                        pTLRec->tlrTargetSwap_ls00);
        }
        else if (tptData_els00 == pTLRec->tlrPageTypeExtracting_ls00)
        {
            ls28_SwapPrimaryDataRecords(pszBuffer,
                                        lRecCount,
                                        pPtrListEntry,
                                        pTLRec->tlrSwapFromDb_ls00,
                                        pTLRec->tlrTargetSwap_ls00);
        }
    }

    delete [] pPtrListEntry;

    return;
}
// ls28SwapPageValues()


/*
  -----------------------------------------------------------------------------
  function:     ls28SwapTableDescription
  -----------------------------------------------------------------------------
*/
void
ls28SwapTableDescription(tsp7_table_description & TD,
                         tsp00_SwapKind_Enum      SSwap)
{
    ROUTINE_DBG_MEO00 ("ls28SwapTableDescription");

    tsp00_SwapKind_Enum CurrSwap = ls07DefineSwap(NULL);

    if (SSwap != CurrSwap)
    {
        TD.sp7t_field_cnt()      = ls07Swap2BInt(TD.sp7t_field_cnt(), SSwap, CurrSwap);
        TD.sp7t_kernel_loc()     = ls07Swap2BInt(TD.sp7t_kernel_loc(), SSwap, CurrSwap);
        TD.sp7t_constraint_cnt() = ls07Swap2BInt(TD.sp7t_constraint_cnt(), SSwap, CurrSwap);

        tsp00_Int2 i = 0;
        for (i; i < TD.sp7t_field_cnt(); ++i)
        {
            tsp7_ffield_info *pFI = &TD.sp7t_fields()[i];
            
            pFI->sp7fi_stmt_col_no = ls07Swap2BInt(pFI->sp7fi_stmt_col_no, SSwap, CurrSwap);
            pFI->sp7fi_len         = ls07Swap2BInt(pFI->sp7fi_len, SSwap, CurrSwap);
            pFI->sp7fi_in_out_l    = ls07Swap2BInt(pFI->sp7fi_in_out_l, SSwap, CurrSwap);
            pFI->sp7fi_def_col_no  = ls07Swap2BInt(pFI->sp7fi_def_col_no, SSwap, CurrSwap);
        }
    }

    return;
}
// ls28SwapTableDescription()


/*
  -----------------------------------------------------------------------------
  function:     ls28SwapIndexDescription
  -----------------------------------------------------------------------------
*/
void
ls28SwapIndexDescription(tsp7_index_description & ID,
                         tsp00_SwapKind_Enum      SSwap)
{
    ROUTINE_DBG_MEO00 ("ls28SwapIndexDescription");

    tsp00_SwapKind_Enum CurrSwap = ls07DefineSwap(NULL);

    if (SSwap != CurrSwap)
    {
        ID.sp7i_first_mult_index = ls07Swap2BInt(ID.sp7i_first_mult_index, SSwap, CurrSwap);
        ID.sp7i_mult_index_cnt   = ls07Swap2BInt(ID.sp7i_mult_index_cnt, SSwap, CurrSwap);

        int i = 0;
        int j = 0;

        // single index descriptions
        for (i = 0; i < ID.sp7i_single_index_cnt; ++i)
        {
            ID.sp7i_single_index()[i].sp7si_def_col_no = ls07Swap2BInt(ID.sp7i_single_index()[i].sp7si_def_col_no,
                                                                       SSwap,
                                                                       CurrSwap);
        }

        // multiple index descriptions
        for (i = 0; i < ID.sp7i_mult_index_cnt; ++i)
        {
            tsp7_info_mult_index *pMI = &ID.sp7i_mult_index()[i];

            pMI->sp7mi_index_id = ls07Swap2BInt(pMI->sp7mi_index_id, SSwap, CurrSwap);
            for (j = 0; j < pMI->sp7mi_col_cnt; ++j)
            {
                pMI->sp7mi_def_col_no[j] = ls07Swap2BInt(pMI->sp7mi_def_col_no[j], SSwap, CurrSwap);
            }
        }
    }

    return;
}
// ls28SwapIndexDescription()