/*



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
#ifndef VPA20DESC_H
#define VPA20DESC_H
/*!**********************************************************************

  module: vpa20Desc.h

  -----------------------------------------------------------------------

  responsible:  D024584

  special area: ODBC-Driver

  description:  ODBC-Descriptors - Datastructures and functions

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 1998-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa00global.h"

#include "vpa21DescRec.h"
#include "vpa62DBCSpecial.h"
#include "vpa30DiagArea.h"

#include "hsp77.h"
#include "hsp78_0.h"

/*!**********************************************************************

  Chapter: datastructures

************************************************************************/

/* descriptor states */
#define API_DESC_UNALLOCATED  (0)  /* desc D0 : unallocated           */
#define API_DESC_IALLOCATED   (1)  /* desc D1i: implicit allocated    */
#define API_DESC_EALLOCATED   (2)  /* desc D1e: explicit allocated    */

/* descriptor types */
#define API_DESC_TYPE_UNDEF   (2)  /* undefined                        */
#define API_DESC_TYPE_AD      (5)  /* application row or param desc    */
#define API_DESC_TYPE_IRD     (6)  /* implementation row descriptor    */
#define API_DESC_TYPE_IPD     (7)  /* implementation param descriptor  */

/* index of bookmark record */
#define API_DESC_BOOKMARK_INDEX (0)

typedef struct
{
    UDWORD        arraySize;       /* ARD: No of rows in rowset            */
                                   /* APD: No of values foe each parameter */
    UDWORD        previousArraySize; /* ARD: Needed for SQLExtendedFetch   */
                                   /* if rowSetSize changes between calls  */
                                   /* PTS 1104224                          */
    UWORD        *arrayStatusPtr;  /* All: Points to array of row/parameter*/
                                   /* status values                        */
  /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1136588 */
    SQLLEN       *bindOffsetPtr;   /* APD/ARD: Pointer to bind offset      */
    SDWORD        bindType;        /* APD/ARD: Column- or row-wise binding */
    UDWORD       *rowsProcessedPtr;/* IPD/IRD: No of rows processed        */
} tpa20DescHeader;


typedef struct st_desc
{
    UWORD            handleType; /* API_DESC_TYPE                            */
    SQLHDBC          parentDBC;  /* Only ARD/APD: The parent DBC             */
    SQLHSTMT         parentStmt; /* The associated stmt                      */
    SWORD            allocType; /* SQL_DESC_ALLOC_AUTO if implicit allocated */
                                /* SQL_DESC_ALLOC_USER if explizit allocated */
    UWORD            state;     /* Current state as defined in ODBC reference*/
    UWORD            descType;  /* API_DESC_TYPE_IPD, API_DESC_TYPE_IRD      */
                                /* or API_DESC_TYPE_AD                       */
    tpa30DiagArea    diagArea;  /* diagnostic area for error info            */
    tpa20DescHeader  header;    /* Header info (except allocType, count )    */

    SWORD            count;     /* No of used records in tpa20Desc.records
                                 * (excluding the bookmark record (rec-no 0) */
    tpa21DescRecord *records;   /* array of records                          */

    UWORD            allocRec;  /* allocated diag-rec in tpa20Desc.records
                                 * including the bookmark record (rec-no 0 ) */
} tpa20Desc;
  
/*!**********************************************************************

  EndChapter: datastructures

************************************************************************/

/*!**********************************************************************

  Chapter: ODBC functions

  description:
  - SQLCopyDesc
  - SQLGetDescField
  - SQLGetDescRec
  - SQLSetDescField
  - SQLSetDescRec

  For sepecification see Microsoft ODBC 3.5 reference.
*/
/*!  
  EndChapter: ODBC functions

************************************************************************/

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/

/*!
  Function: pa20AllocExplicitDesc

  description: Allocates an explicit descriptor handle (APD or ARD).

  If the hdbc is not valid the functions returns SQL_INVALID_HANDLE.
  If there is no connection open for hdbc or another error occurs it
  returns SQL_ERROR and sets the state 08003 (Connection does not
  exist) in the diag-record of hdbc. If no error occured the function
  allocates storage for a new descriptor-handle and sets the
  attributes to default-values. In this case the function return
  SQL_SUCCESS (see SQLAllocHandle in the ODBC-reference).
  
  arguments:
    HDBC      hdbc       [in]  Connection handle that owns the HDESC.
    SQLHDESC *phdesc     [out] Pointer to the allocated descriptor handle.
      
    return value: RETCODE (SQL_SUCCESS, SQL_ERROR or SQL_INVALID_HANDLE)*/


SQLRETURN pa20AllocExplicitDesc( SQLHDBC hdbc, SQLHDESC  *phdesc );


/*!*********************************************************************
  Function: pa20AllocImplicitDesc

  description: Allocates an implicit descriptor handle (IPD or IRD).

  If the hstmt is not valid the functions returns SQL_INVALID_HANDLE.
  Whenever an error occurs the function sets an approbiate SQL-state
  value in the diag-record of hstmt and returns SQL_ERROR. If no error
  occured the function allocates storage for a new descriptor-handle
  and sets the attributes to default-values. In this case the function
  returns SQL_SUCCESS (see SQLAllocHandle in the ODBC-reference).

  arguments:
    HSTMT     hstmt      [in]  Statement handle that owns the HDESC.
    UWORD     descType   [in]  Parameter- or row-descriptor
                               (API_DESC_TYPE_PARAM or API_DESC_TYPE_ROW)
    SQLHDESC *phdesc     [out] Pointer to the allocated descriptor handle.

  return value: RETCODE (SQL_SUCCESS, SQL_ERROR or SQL_INVALID_HANDLE)
*/

SQLRETURN pa20AllocImplicitDesc( SQLHSTMT   hdbc,
                                 UWORD      descType,
                                 SQLHDESC  *phdesc);


/*!*********************************************************************
  Function: pa20AllocRecords

  description: Allocates records for a descriptor.

  arguments:
    SQLHDESC  hdesc      [in]  Descriptor handle that holds the records.
    UWORD     numRecords [in]  Number of records to allocate.

  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20AllocRecords( SQLHDESC  hdesc,
                              UWORD     numRecords );


/*!*********************************************************************
  Function: pa20CloneDesc

  description: Allocate a new desc that is a copy of a given desc.
               But don´t allocate extra storage for the long handles
               in the desc-records

  arguments:
    tpa20 *inDescPtr   [in] Pointer to the source descriptor.

  return value: tpa20Desc*  The allocated copy of inDescPtr or NULL if
                            the descriptor could not be allocated.
  */

tpa20Desc* pa20CloneDesc( tpa20Desc *inDescPtr );


/*!*********************************************************************
  Function: pa20ColAttr

  description: Return column attributes.

  arguments:
    sqldatype  *sqlda_ptr            [in]  Pointer to SQLDA
    tpa20Desc  *ird_ptr              [in]  Pointer to IRD that describes
                                          the columns
    UWORD       column               [in]  Number of column
    UWORD       description_type     [in]  SQL_COLUMN_NAME etc
    UCHAR      *description_ptr      [out] Pointer to storage for
                                          character results.
    SWORD       max_description_size [in]  Length of *description_ptr
    SWORD      *description_size_ptr [out] To return length of result
    SQLPOINTER *description_num_ptr  [out] Return numeric results
                                           COLUMN_TYPE, COLUMN_SIZE
    tsp77encoding *destEncoding      [in]  encoding type of description_ptr
    
  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20ColAttr( sqldatype       *sqlda_ptr,
                         tpa20Desc       *ird_ptr,
                         UWORD            column,
                         UWORD            description_type,
                         UCHAR           *description_ptr,
                         SWORD            max_description_size,
                         SWORD           *description_size_ptr,
                         SQLPOINTER      *description_num_ptr,
                         const tsp77encoding *destEncoding);

/*!*********************************************************************
  Function: pa20CopyDesc

  description: Copy given descriptor.

  The function alloctes a new record-area for outDesc. But the
  long handles are shared by inDesc and outDesc. The descriptor
  outDesc m u s t be freed with pa20FreeCopiedDesc not pa20FreeDesc!
  The pointers are not checked for NULL.
  
  arguments:
    tpa20Desc  *inDesc  [in]  The source desc.
    tpa20Desc  *outDesc [in]  The target desc.
    
  return value: API_RETCODE (API_OK or API_NOT_OK) */

API_RETCODE pa20CopyDesc( tpa20Desc *inDesc, tpa20Desc *outDesc );


/*!*********************************************************************
  Function: pa20DeleteDesc

  description: Delete diag-area and records of the descriptor

  arguments:
    tpa20 *DescPtr   [in] Pointer to descriptor that should be freed.

  return value: API_RETCODE (API_OK or API_NOT_OK) */

API_RETCODE pa20DeleteDesc( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20DescribeCol

  description: Low-level function for SQLDescribeCol

  arguments:
    sqldatype *sqlda_ptr     [in]  Pointer to SQLDA
    tpa20Desc *ird_ptr       [in]  Pointer to implementation row desc
    UWORD      column        [in]  Column number
    UCHAR     *name_ptr      [out] Buffer for column name
    SWORD      max_name_size [in]  Size of *name_ptr
    SWORD     *name_size_ptr [out] Buffer for name size
    SWORD     *datatype_ptr  [out] Buffer for SQL-datatype
    SQLULEN   *precision_ptr [out] Buffer for precision
    SWORD     *scale_ptr     [out] Buffer for scale
    SWORD     *nullable_ptr  [out] Buffer for nullable indicator
    tsp77encoding *destEncoding [in]  encoding type of name_ptr

  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20DescribeCol( sqldatype      *sqlda_ptr,
                             tpa20Desc      *ird_ptr,
                             UWORD           column,
                             UCHAR          *name_ptr,
                             SWORD           max_name_size,
                             SWORD          *name_size_ptr,
                             SWORD          *datatype_ptr,
                             SQLULEN        *precision_ptr,
                             SWORD          *scale_ptr,
                             SWORD          *nullable_ptr,
                             const tsp77encoding *destEncoding);

/*!*********************************************************************
  Function: pa20DescribeParam

  description: Low level function for SQLDescribeParam

  arguments:
    tpa20Desc *ipd_ptr        [in]  Pointer to IPD
    UWORD      column         [in]  Column number
    SWORD     *datatype_ptr   [out] Buffer for SQL datatype
    SQLULEN   *precision_ptr  [out] Buffer for precision
    SWORD     *scale_ptr      [out] Buffer for scale
    SWORD     *nullable_ptr   [out] Buffer for nullable
    UDWORD     passthrough    [in]  Passtrough statement attribute
                                    (TRUE or FALSE)     
                               
  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20DescribeParam( tpa20Desc *ipd_ptr,
                               UWORD      column,
                               SWORD     *datatype_ptr,
                               SQLULEN   *precision_ptr,
                               SWORD     *scale_ptr,
                               SWORD     *nullable_ptr,
                               UDWORD     passthrough );


/*!*********************************************************************
  Function: pa20FreeDesc

  description: Free descriptor.

  Frees storage for a descriptor-handle. All associated DescRec´s and
  the long handles in the DescRec´s are also freed. If an error occurs
  the desc-record is set (see SQLFreeHandle in the ODBC-reference).
  
  arguments:
    SQLHDESC  hdesc  [in]  Descriptor-Handle that should be freed

  return value: RETCODE (SQL_SUCCESS, SQL_ERROR or SQL_INVALID_HANDLE)
*/

SQLRETURN pa20FreeDesc( SQLHDESC hdesc );


/*!*********************************************************************
  Function: pa20FreeCopiedDesc

  description: Free a descriptor that was allocated with pa20CopyDesc.

  Does the same as pa20FreeDesc but keeps the long handles that are
  stored in the desc-records and the diag-area.
  This is useful if desc has been allocated with pa20CopyDesc and the
  source descriptor should be used further because pa20CopyDesc does
  not copy the storage for the long handles and diag-areas.

  This function does not check if descPtr is a valid descriptor pointer.
  
  arguments:
    tpa20Desc  *descPtr  [in]  Descriptor that should be freed

  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20FreeCopiedDesc( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20_FreeLongHandles

  description: Frees storage for the long handles in all records.

  arguments:
    tpa20Desc  *descPtr  [in]  The descriptor.
    
  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20FreeLongHandles( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20GetDescField

  description: Get a header or record field

  arguments:
    tpa20Desc *descPtr     [in]  Descriptor-handle (must be valid)
    SWORD      recNo       [in]  Number of record
    SWORD      fieldId     [in]  SQL_DESC_...
    PTR        valuePtr    [out] Pointer to value buffer
    SDWORD     len         [in]  Length of *valuePtr
    SDWORD    *lenPtr      [out] Pointer to buffer for the total length
    UWORD     *sqlStatePtr [out] Pointer sql state variable
    tsp77encoding *destEncoding [in] encoding for strings contained in valuePtr
    
  return value: RETCODE (SQL_SUCCESS, SQL_SUCCESS_WITH_INFO,
                SQL_ERROR or SQL_INVALID_HANDLE)
  
  Caution: It is not checked if descPtr is a valid descriptor!  */

SQLRETURN pa20GetDescField( tpa20Desc *descPtr,
                            SWORD      recNo,
                            SWORD      fieldId,
                            PTR        valuePtr,
                            SDWORD     len,
                            SDWORD    *lenPtr,
                            UWORD     *sqlStatePtr,
                            const tsp77encoding *destEncoding );


/*!*********************************************************************
  Function: pa20GetDescType

  description: Determine the type of a descriptor (APD,ARD,IPD or IRD)

  arguments:
    tpa20Desc  *descPtr  [in]  Pointer to descriptor (must be valid)
    
  return value: UWORD (API_DESC_TYPE_IPD/..._IRD/..._AD)
  
  Caution: It is not checked if descPtr is a valid descriptor-pointer!
  */

UWORD pa20GetDescType( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20GetParentDBC

  description: Returns the connection in which the descriptor "lives"

  arguments:
    HDESC  *descPtr  [in]  Descriptor-handle (must be valid)
    
  return value: tpa40DBC*   Pointer to connection-block
  
  Caution: It is not checked if hdesc is a valid descriptor-handle!  */

SQLHDBC pa20GetParentDBC( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20GetParentEnv

  description: Returns the environment in which the descriptor "lives"

  arguments:
    HDESC  *descPtr  [in]  Descriptor-handle (must be valid)
    
  return value: tpa50Env*   Pointer to environment-block
  
  Caution: It is not checked if hdesc is a valid descriptor-handle!  */

SQLHENV pa20GetParentEnv( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20GetRecord

  description: Returns the descriptor record with given index

  Record-number 0 returns the bookmark record. If there exists no
  record with the given index NULL is returned.
  
  arguments:
    tpa20Desc  *descPtr  [in]  The descriptor that holds the record.
    SWORD       recNo    [in]  The index of the record to fetch 
    
  return value: tpa21DescRecord* (the fetched record or NULL)
  */

tpa21DescRecord* pa20GetRecord( tpa20Desc *descPtr, SWORD recNo);


/*!*********************************************************************
  Function: pa20InitApplDesc

  description: Initialisation of an application descriptor.

  The fields that are used in application descriptors are set to
  default values as described in the MS ODBC-reference. For implicitely
  allocated handles hdbc must be NULL and hstmt must be a valid
  stmt-handle. For explicitely allocaed handles hdbc must be a valid
  connection handle and hstmt must be NULL.

  Because the handles are not checked (!!!) the function returns
  always API_OK.

  arguments:
    SQLHDESC  hdesc [out]  The descriptor handle that should be
                           initialized
    HDBC      hdbc  [in]   The connection handle that should own hdesc
    HSTMT     hstmt [in]   The connection handle that should own hdesc
    
  return value: API_RETODE (API_OK) */


API_RETCODE pa20InitApplDesc( SQLHDESC hdesc, SQLHDBC hdbc, SQLHSTMT hstmt );


/*!*********************************************************************
  Function: pa20InitCount

  description: Decreases count to the index of the last bound record.

  arguments:
    tpa20Desc descPtr [in/out]  Pointer to descriptor

  return value: API_RETCODE (API_OK)
  */

API_RETCODE pa20InitCount( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20InitImplDesc

  description: Initialisation of an implementaion descriptor.

  The fields that are used in implementation descriptors are set to
  default values as described in the MS ODBC-reference.

  Because the handles are not checked (!!!) the function returns
  always API_OK.
  
  arguments:
    SQLHDESC  hdesc    [out]  The descriptor handle that should be
                              initialized
    HDBC      hdbc     [in]   The connection handle that should own hdesc
    HSTMT     hstmt    [in]   The statement handle that should own hdesc
    UWORD     descType [in]   API_DESC_TYPE_ROW or API_DESC_TYPE_PARAM

  return value: API_RETODE (API_OK) */


API_RETCODE pa20InitImplDesc( SQLHDESC hdesc,
                              SQLHDBC  hdbc,
                              SQLHSTMT hstmt,
                              UWORD    descType );


/*!*********************************************************************
  Function: pa20InitLongHandles

  description: Set all long handles to NULL.

  arguments:
    tpa20Desc  *descPtr  [in]  The descriptor.
    
  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20InitLongHandles( tpa20Desc *descPtr );


/*!*********************************************************************
  Function: pa20Move

  description: move fields from sqlda areas to user fields

  arguments:
    tpa20Desc       *appl_desc_ptr   [in/out] Pointer to ARD/APD
    tpa20Desc       *impl_desc_ptr   [in/out] Pointer to IRD/IPD
    UWORD            number_cols     [in]     Number of columns
    tpa62DBCSpecial *special_dbc_ptr [in]
                      
  return value: API_RETODE (API_OK or AI_NOT_OK)
  */
     
API_RETCODE pa20Move( tpa20Desc       *appl_desc_ptr,
                      tpa20Desc       *impl_desc_ptr,
                      UWORD            number_cols,
                      tpa62DBCSpecial *special_dbc_ptr);


/*!*********************************************************************
  Function: pa20PopulateDesc

  description: Populate the attribute blocks with descriptive data

  arguments:
    sqldatype       *sqlda_ptr       [in]     Pointer to SQLDA
    tpa20Desc       *impl_desc_ptr   [in/out] Descriptor to populate
    UWORD            number_cols     [in]     Number of columns
    UDWORD          *size_ptr        [out]    Buffer for tuple size
    tpa62DBCSpecial *special_dbc_ptr [in]
    SQLUSMALLINT     unicode_catalog [in]     unicode_catalog info

  return value: API_RETODE (API_OK or AI_NOT_OK)
  */


API_RETCODE pa20PopulateDesc( sqldatype       *sqlda_ptr,
                              tpa20Desc       *impl_desc_ptr,
                              UWORD            number_cols,
                              UDWORD          *size_ptr,
                              tpa62DBCSpecial *special_dbc_ptr,
                              SQLUSMALLINT     unicode_catalog );


/*!*********************************************************************
  Function: pa20PutDesc

  description: Put fields of multiple records from user fields to
               sqlda areas.

  arguments:
    tpa20Desc *apd_ptr        [in]  Pointer to APD
    tpa20Desc *ipd_ptr        [in]  Pointer to IPD
    UWORD      number_parms   [in]  Number of parameters
    UWORD      sqlmode        [in]
                         
  return value: API_RETODE (API_OK or AI_NOT_OK)
  */

API_RETCODE pa20PutDesc( tpa20Desc *apd_ptr,
                         tpa20Desc *ipd_ptr,
                         UWORD      number_parms,
                         UWORD      sqlmode );


/*!*********************************************************************
  Function: pa20PutError

  description: Add a new error record to hdesc.

  arguments:
    SQLHDESC  hdesc     [out]  Descriptor handle
    UWORD     sqlState  [in]   SQL-State number (API_ODBC_S1001, ...)

  return value: API_RETODE
    API_OK      --  the error record was successfully created
    API_NOT_OK  --  the error record could not be created
  */

API_RETCODE pa20PutError( SQLHDESC hdesc, UWORD sqlState );


/*!*********************************************************************
  Function: pa20PutRec

  description: Put fields of one record from user fields to sqlda areas

  arguments:
    tpa21DescRec *appl_desc_rec_ptr        [in]  Pointer to APD/ARD
    tpa21DescRec *impl_desc_rec_ptr        [in]  Pointer to IPD/IRD
    UWORD         sqlmode                  [in]
                         
  return value: API_RETODE (API_OK or AI_NOT_OK)
  */

API_RETCODE pa20PutRec( tpa21DescRecord *appl_desc_rec_ptr,
                        tpa21DescRecord *impl_desc_rec_ptr,
                        UWORD            sqlmode );

    
/*!*********************************************************************
  Function: pa20ReallocRecords

  description: Resizes the record area if neccesary.

  The record area is only reallocated if the current size is less than
  numRecords.
  
  arguments:
    tpa20Desc *descPtr    [in]  Descriptor that holds the records.
    UWORD      numRecords [in]  Number of records.

  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20ReallocRecords( tpa20Desc *descPtr,
                                UWORD      numRecords );


/*!*********************************************************************
  Function: pa20SetAttr

  description: Set attributes of IRD ot IPD

  arguments:
    sqldatype  *sqlda_ptr       [in]      Pointer to SQLDA
    tpa20Desc  *impl_desc_ptr   [in/out]  Pointer to IRD/IPD
    UWORD       attr_array_cnt  [in]      Number of columns/parameters
    UCHAR      *buffer_ptr      [in]
    UDWORD      buffer_size     [in]
    SWORD       iotype          [in]      unused!!!

  return value: API_RETCODE (API_OK or API_NOT_OK)
  */

API_RETCODE pa20SetAttr( sqldatype  *sqlda_ptr,
                         tpa20Desc  *impl_desc_ptr,
                         UWORD       attr_array_cnt,
                         UCHAR      *buffer_ptr,
                         UDWORD      buffer_size,
                         SWORD       iotype );


/*!*********************************************************************
  Function: pa20SetCount

  description: Set count field of IRD or IPD

  arguments:
    tpa20Desc  *descPtr   [in/out]  Pointer to IRD/IPD
    SWORD       count     [in]      Count (== number of used records)
                                    (compare tpa20Desc.count)
  return value: VOID
  */

VOID pa20SetCount( tpa20Desc *descPtr, SWORD count );


/*!*********************************************************************
  Function: pa20SetAttrAddr

  description: Set addresses for array commands.

  pa06SetAttrAddr belegt die Addressen des mit attrp uebergebenen
  Attributeblocks fuer array kommandos. Als Basis wird der mit
  base_desc_ptr uebergebene Attribute block angesehen. Wobei bind_type
  bestimmt ob die Adressen Row wise oder Column wise erhoeht werden. Der
  Parameter pos bestimmt die Anzahl der Zeilen, um die der neue
  Attribute block entfernt sein soll. Der Attributeblock muss vorher mit
  pa20CloneDesc angelegt worden sein.  Ist der Basis Attributeblock
  NULL, so kann der Attributeblock mit pos relativ positioniert werden.
  
  arguments:
    tpa20Desc *base_desc_ptr  [in]     Pointer to ARD/APD or NULL
    tpa20Desc *appl_desc_ptr  [in/out] Pointer to target ARD/APD
    tpa20Desc *impl_desc_ptr  [in/out] Pointer to target IRD/IPD
    UWORD      number_cols    [in]     Number of columns/parameters
    SDWORD     bind_type      [in]     SQL_BIND_BY_COLUMN or size of
                                       structure for one row (row-wise
                                       binding)
    SDWORD     pos            [in]     relative position
    UDWORD     irow           [in]
    
  return value: VOID */

VOID pa20SetAttrAddr( tpa20Desc *base_desc_ptr,
                      tpa20Desc *appl_desc_ptr,
                      tpa20Desc *impl_desc_ptr,                      
                      UWORD      number_cols,
                      SDWORD     bind_type,
                      SDWORD     pos,
                      UDWORD     irow );


/*!*********************************************************************
  Function: pa20SetDescField

  description: Set value for a desscriptor header or record field.

  arguments:
    tpa20Desc *descPtr     [out]  Descriptor handle
    SWORD      recNo       [in]
    SWORD      recNo       [in]
    PTR        valuePtr    [in]
    SDWORD     len         [in]
    UWORD     *sqlStatePtr [out]  Pointer to SQL state variable
    tsp77encoding *destEncoding [in] encoding for strings contained in valuePtr
    
  return value: RETODE
    SQL_SUCCESS  --  the value was  succesfully set
    SQL_ERROR    --  something went wrong (see SQLSetDescField in the
                     ODBC reference)

  Caution: This function only checks if the values are correct.
           It does not check for access rights or null-pointers.
           */

SQLRETURN pa20SetDescField( tpa20Desc  *descPtr,
                            SWORD      recNo,
                            SWORD      fieldId,
                            PTR        valuePtr,
                            SDWORD     len,
                            UWORD     *sqlStatePtr,
                            const tsp77encoding *destEncoding );


/*!*********************************************************************
  Function: pa20SMap

  description: Normally just copy the data using apgetos; but, in
               certain special cases convert/split the data for the
               catalog accessing functions.

  arguments:
    tpa20Desc       *impl_desc_ptr   [in]  Pointer to IRD
    UWORD            icol            [in]  Column number
    tpa62DBCSpecial *dbc_special_ptr [in]
    SWORD            c_type          [in]  C-datatype
    UWORD            numericScale    [in]  Scale for SQL_C_NUMERIC.
    UCHAR           *c_data_ptr      [out] Pointer to buffer for C-data
    SQLLEN           c_data_max_len  [in]  Length of *c_data_ptr
    SQLLEN          *c_data_used_ptr [out] Buffer to return the number
                                           of bytes returned in
                                           *c_data_ptr
    
  return value: API_RETODE (API_OK or AI_NOT_OK)
  */

API_RETCODE pa20SMap( tpa20Desc       *impl_desc_ptr,
                      UWORD            icol,
                      tpa62DBCSpecial *dbc_special_ptr,
                      SWORD            c_type,
                      UWORD            numericScale,
                      UCHAR           *c_data_ptr,
                      SQLLEN           c_data_max_len,
                      SQLLEN          *c_data_used_ptr );


/*!*********************************************************************
  Function: pa20Unset

  description: Checks type information conciseType, type,
               datetimeIntervalCode. See MS ODBC-reference 3.0 p. 941.

  arguments:
    tpa20Desc *descPtr  [in]  Pointer to IPD/IRD
    SWORD      recNo    [in]  Record number
    
  return value: API_RETCODE (API_OK, API_NOT_OK)
  */

API_RETCODE pa20TypesAreConsistent( tpa20Desc *descPtr, SWORD recNo );


/*!*********************************************************************
  Function: pa20Unset

  description: Unlock long handles

  arguments:
    tpa20Desc *impl_desc_ptr  [in]  Pointer to IPD/IRD
    UWORD      attr_array_cnt [in]  Number of desc records (cols/params)
    
  return value: VOID
  */

VOID pa20Unset( tpa20Desc *impl_desc_ptr,
                UWORD      attr_array_cnt );


/*!*********************************************************************
  Function: pa20Verify

  description: Check if descriptor is valid.

  arguments:
    HDESC  hdesc  [in]  Descriptor-handle to check.
    
  return value: API_RETCODE (API_OK or API_NOT_OK)
*/

API_RETCODE pa20VerifyDesc( SQLHDESC hdesc );


API_RETCODE pa20AllocLong( tpa20Desc *desc_ptr,
                           UWORD      num_attr );


/* if column/parameter is bound and SQL_DESC_BIND_OFFSET_PTR is set
 * we dereference BindOffsetPtr and add the value to the dataPtr,
 * octetLengthPtr and indicatorPtr descriptor record fields */

#define PA20_BINDOFFSET(rec) (SQLLEN) \
 ((rec->bound && rec->parentDesc->header.bindOffsetPtr != NULL) ? \
   *(rec->parentDesc->header.bindOffsetPtr) : 0)

/* for debugging: replace macro with function */
/*
SQLLEN pa20BindOffset (tpa21DescRecord *rec); 
#define PA20_BINDOFFSET(rec) pa20BindOffset(rec) 
*/

#define PA20_OCTETLENGTH_PTR(rec) \
 ((SQLLEN*) (((char*) rec->octetLengthPtr) + PA20_BINDOFFSET(rec)))

/* for debugging: replace macro with function */
/*
SQLLEN* pa20OctetLengthPtr (tpa21DescRecord *rec);
#define PA20_OCTETLENGTH_PTR(rec) pa20OctetLengthPtr (rec)
*/

#define PA20_DATA_PTR(rec)  (rec->dataPtr + PA20_BINDOFFSET(rec))

#define PA20_INDICATOR_PTR(rec) \
 ((SQLLEN*) (((char*) rec->indicatorPtr) + PA20_BINDOFFSET(rec)))


/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/

#endif



