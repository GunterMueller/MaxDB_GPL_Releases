/*!**************************************************************************

  module:    vpr01ParseInfo.c

  -------------------------------------------------------------------------

  responsible: MarcoP

  special area: interface runtime
  description : cache for information received by parsing the sql command

  last changed:
  see also    :

  -------------------------------------------------------------------------





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




*****************************************************************************/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "vpr07Macro.h"
#include "gpr03.h"
#include "gpr04.h"
#include "vpr01ParseInfo.h"
#include "vpr01SQL.h"
#include "vpr01Trace.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr01ParseInfoInitCont(tpr01_ParseInfoContainer *Cont, tpr01_ConDesc *ConDesc, tsp00_Uint4 HT_capacity, tsp00_Int4 HT_LoadFactor);

static tsp00_Int4 pr01ParseInfoOpenCont(tpr01_ParseInfoContainer *Cont);

static tsp00_Int4 pr01ParseInfoGetCount(tpr01_ParseInfoContainer *Cont);

static void pr01ParseInfoCloseCont(tpr01_ParseInfoContainer *Cont);

static tpr01_ParseInfoDesc *pr01ParseInfoEnumDesc(tpr01_ParseInfoContainer *Cont);

static tsp00_Int4 pr01ParseInfoSetIndex(tpr01_ParseInfoContainer *Cont, tsp00_Int4 cbIndex);

static tpr01_ParseInfoDesc *pr01ParseInfoAddDesc(tpr01_ParseInfoContainer *ParseInfo, tpr05_String *SQLCommand);

static tsp00_Int4 pr01ParseInfoDeleteDesc(tpr01_ParseInfoDesc *ParseInfoDesc, tsp00_Int4 keepParseID);

static void pr01ParseInfoCacheDesc(tpr01_ParseInfoDesc *SQLStmt);

static void pr01ParseInfoSetReferenceCount(tpr01_ParseInfoDesc *StmtName);

static tpr01_ParseInfoDesc *pr01ParseInfoFindDesc(tpr01_ParseInfoContainer *ParseInfo, tpr05_String *SQLStatement, tsp00_Int4 HashIndex);

tsp00_Int4 pr01ParseInfoParse(tpr01_SQLDesc *SQLDesc, tsp00_Int2 *cmdfetch, tsp00_Int4 *datafound);

static void pr01ParseInfoTrace(tpr01_ParseInfoContainer *ParseInfo, sqlratype * sqlra);

static void pr01ParseInfoHashTableInsert(tpr01_ParseInfoDesc *ParseInfoDesc);

static void pr01ParseInfoAccessLRUItem(tpr01_ParseInfoDesc *SQLStmt);

static void pr01ParseInfo_ParseInfosInit(tpr01_ParseInfoDesc *ParseInfoDesc);

static void pr01ParseInfo_ParseInfosDelete(tpr01_ParseInfoDesc *ParseInfoDesc);

static void pr01ParseInfoPutParseInfos(tpr01_ParseInfoDesc *ParseInfoDesc, sqlorentry *orentry, sqlkaentry *kaentry);

static void pr01ParseInfoGetParseInfos(tpr01_ParseInfoDesc *ParseInfoDesc, sqlorentry *orentry, sqlkaentry *kaentry);

static tpr09HashValue pr01ParseInfo_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen);

static tsp00_Int4 pr01ParseInfo_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len);

static void pr01ParseInfo_printKey (const tpr09HashValue data_p, FILE* datei);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function:     pr01ParseInfoInitCont

  see also:

  Description:  initializes ParseInfo Container

  Arguments:
    Cont [in|out] pointer to container
    return value: void
 */
static void pr01ParseInfoInitCont(tpr01_ParseInfoContainer *Cont, tpr01_ConDesc *ConDesc, tsp00_Uint4 HT_capacity, tsp00_Int4 HT_LoadFactor)
{
  tpr09_HashTableError myError;
  Cont->ConDesc = ConDesc;
  Cont->Desc = pr09NewDescriptor(3, sizeof(tpr01_ParseInfoContainer));
  myError = pr09HTCreateHashTable(&(Cont->HashTable),
                                  HT_capacity,
                                  HT_LoadFactor,
                                  rehash_disable,
                                  pr01ParseInfo_CompareKey,
                                  NULL,
                                  pr01ParseInfo_GetKey,
                                  pr01ParseInfo_printKey);
  pr09LRUListInit(&(Cont->LRUList));
  Cont->CacheStat.requests        = 0;
  Cont->CacheStat.founds          = 0;
  Cont->CacheStat.inserts         = 0;
  Cont->CacheStat.deletes         = 0;
  Cont->CacheStat.collisions      = 0; /*currently not used*/
  Cont->CacheStat.weightedHitrate = 0; /*currently not used*/
  Cont->InitCont           = pr01ParseInfoInitCont;
  Cont->OpenCont           = pr01ParseInfoOpenCont;
  Cont->CloseCont          = pr01ParseInfoCloseCont;
  Cont->GetCount           = pr01ParseInfoGetCount;
  Cont->SetIndex           = pr01ParseInfoSetIndex;
  Cont->EnumDesc           = pr01ParseInfoEnumDesc;
  Cont->DeleteDesc         = pr01ParseInfoDeleteDesc;
  Cont->AddDesc            = pr01ParseInfoAddDesc;
  Cont->SetReferenceCount  = pr01ParseInfoSetReferenceCount;
  Cont->FindDesc           = pr01ParseInfoFindDesc;
  Cont->Parse              = pr01ParseInfoParse;
  Cont->PutParseInfos      = pr01ParseInfoPutParseInfos;
  Cont->GetParseInfos      = pr01ParseInfoGetParseInfos;
  Cont->TraceStatistic     = pr01ParseInfoTrace;
}


/*!
  Function:     pr01ParseInfoDeleteCont

  see also:

  Description:  deletes a parse info container

  Arguments:
    ParseInfo [in|out] pointer to a container
    return value: void
 */
void pr01ParseInfoDeleteCont(tpr01_ParseInfoContainer *ParseInfo)
{
  if (ParseInfo) {
    if (pr01ParseInfoOpenCont(ParseInfo)) {
      tpr01_ParseInfoDesc *ParseInfoDesc;
      while(ParseInfoDesc = pr01ParseInfoEnumDesc(ParseInfo)) {
        ParseInfoDesc->referenceCnt = 0;
	    pr01ParseInfoDeleteDesc(ParseInfoDesc, KEEP_PARSEID);
	  }
    }
    pr09HTDeleteHashTable (&(ParseInfo->HashTable));
    if (ParseInfo->Desc)
      pr09DeleteDescriptor(ParseInfo->Desc);
      pr03mFree((tsp00_Uint1*)ParseInfo, "ParseInfo");
  }
}

/*!
  Function:     pr01ParseInfoNewCont

  see also:

  Description:  creates a new ParseInfo Container

  Arguments:

  return value: pointer to the new tpr01_ParseInfoContainer
 */
tpr01_ParseInfoContainer *pr01ParseInfoNewCont(tpr01_ConDesc *ConDesc, tsp00_Uint4 HT_capacity, tsp00_Int4 HT_LoadFactor)
{
  tpr01_ParseInfoContainer *ParseInfo;
  ParseInfo = pr03mAllocat(sizeof(tpr01_ParseInfoContainer), "ParseInfo");
  if (ParseInfo)
    pr01ParseInfoInitCont(ParseInfo, ConDesc, HT_capacity, HT_LoadFactor);
  return ParseInfo;
}

/*!
  Function:     pr01ParseInfoOpenCont

  see also:

  Description:  opens a ParseInfo Container

  Sets iterator to first element of container

  Arguments:
    ParseInfo [in|out] pointer to container
  return value: number of current used elements in ParseInfo Container
 */
static tsp00_Int4 pr01ParseInfoOpenCont(tpr01_ParseInfoContainer *ParseInfo)
{
  if (ParseInfo->Desc) {
    pr09SetIndex(ParseInfo->Desc, -1);
  }
  return(ParseInfo->GetCount(ParseInfo));
}

/*!
  Function:     pr01ParseInfoAddDesc

  see also:

  Description:  adds a new ParseInfo Descriptor to a ParseInfo Container

  Arguments:
    ParseInfo   [in] pointer to ParseInfoContainer
    SQLCommand  [in] pointer to SQLCommand

  return value: tpr01_ParseInfoDesc - pointer to the new SQLStatement Descriptor
                which was inserted into the ParseInfo Container

 */
static tpr01_ParseInfoDesc *pr01ParseInfoAddDesc(tpr01_ParseInfoContainer *ParseInfo, tpr05_String *SQLCommand)
{
  M90TRACE(M90_TR_ENTRY, "pr01ParseInfoAddDesc", 0);
  {
    tpr01_ParseInfoDesc *ParseInfoDesc;
    ParseInfoDesc = pr09AddItem(ParseInfo->Desc);
    ParseInfoDesc->DescType = ParseInfoDesc_epr01;
    ParseInfoDesc->ParseInfo= ParseInfo;
    ParseInfoDesc->referenceCnt= 1;

    /*copy stmt*/
    ParseInfoDesc->SQLStatement = pr05IfCom_String_NewDynString(SQLCommand->cbLen, SQLCommand->encodingType);
    pr05IfCom_String_strcpy (ParseInfoDesc->SQLStatement, SQLCommand);

    /*insert into hashinfos*/
    ParseInfoDesc->State     = created_epr01ParseInfo;
    ParseInfoDesc->HashIndex = -1;
    pr01ParseInfoHashTableInsert(ParseInfoDesc);

    /*insert into LRU list*/
    ParseInfoDesc->LRUListItem.data_p = ParseInfoDesc;
    ParseInfoDesc->LRUListItem.prev_p = NULL;
    ParseInfoDesc->LRUListItem.next_p = NULL;
    pr01ParseInfoAccessLRUItem(ParseInfoDesc);

    /*initializes ParseInfos*/
    pr01ParseInfo_ParseInfosInit(ParseInfoDesc);

    ParseInfo->CacheStat.inserts++;
    M90TRACE(M90_TR_EXIT, "pr01ParseInfoAddDesc", 0);
    M90TRACE(M90_TR_HANDLE, "ParseInfoDesc", &ParseInfoDesc);
    return(ParseInfoDesc);
  }
}

/*!
  Function:     pr01ParseInfoDeleteDesc

  see also:

  Description:  deletes a ParseInfo Descriptor but don`t drops the parseid

  Arguments:
    ParseInfoDesc   [in] pointer to ParseInfoDesc
    keepParseID     [in] determines if the parseid will be droped (DROP_PARSEID) or not (KEEP_PARSEID) 

  return value: 1 if descriptor was deleted
                0 if descriptor was not deleted
*/
static tsp00_Int4 pr01ParseInfoDeleteDesc(tpr01_ParseInfoDesc *ParseInfoDesc, tsp00_Int4 keepParseID)
{
  if (ParseInfoDesc) {  
      int cbIndex = pr09ItemFindIndex(ParseInfoDesc->ParseInfo->Desc, ParseInfoDesc);
      tpr01_ConDesc    *ConDesc    = ParseInfoDesc->ParseInfo->ConDesc;
    
      ParseInfoDesc->referenceCnt--;
      if (cbIndex >= 0
          && ParseInfoDesc->referenceCnt <= 0) {
      /* deletes SQLStatement and frees memory, but only
         if SQLStatement is not referenced by hashtable or StmtNameDesc */
         if (ConDesc && keepParseID == DROP_PARSEID) {
           /* reset mfetch buffers and drop the parseid */
           sqlxatype *sqlxa = ConDesc->sqlxa;
           sqlcatype *sqlca = ConDesc->sqlca;
           sqlgaentry *ga = ConDesc->ga;
           p01bdrop_parsid (sqlca, sqlxa, (ParseInfoDesc->ParseInfos.ParseId));
           pr06ParseIdDropIntoCache(sqlca, sqlxa, ga, (tpr06_ParseIDCache *)ConDesc->ParseIdCache, &(ParseInfoDesc->ParseInfos.ParseId));
         }
         pr01ParseInfo_ParseInfosDelete(ParseInfoDesc);
         if (   ParseInfoDesc->State != uncached_epr01ParseInfo
             && ParseInfoDesc->State != created_epr01ParseInfo) {
         /*remove from hashtable and LRU List*/
           tsp00_Uint4  hKeyLen;
           tpr09HashKey hKey = pr01ParseInfo_GetKey((tpr09HashValue) ParseInfoDesc, &hKeyLen);
           pr09HTRemoveHashItemByIndex (&(ParseInfoDesc->ParseInfo->HashTable),
                                        ParseInfoDesc->HashIndex,
                                        hKey,
                                        hKeyLen
                                       );
           pr09LRUListRemoveItem(&(ParseInfoDesc->ParseInfo->LRUList), &(ParseInfoDesc->LRUListItem));
         }
         pr05IfCom_String_DeleteString(ParseInfoDesc->SQLStatement);
        PR07_HEAPCHECK(NULL);
        /*pr01TracePrintf(ConDesc->sqlca->sqlrap, "DELETING PARSEINFODESC: %x", ParseInfoDesc);*/
        pr09CloseItemEx(ParseInfoDesc->ParseInfo->Desc, cbIndex);
        ParseInfoDesc->ParseInfo->CacheStat.deletes++;
        return (1);
      }
      else
        return (0);
      }
  return (0);  
}

/*!
  Function:     pr01ParseInfoGetCount

  see also:

  Description:  counts number of current used elements in  ParseInfo container

  Arguments:
    ParseInfo [in] pointer to ParseInfo Container

  return value: number of current used elements in ParseInfo Container
 */
static tsp00_Int4 pr01ParseInfoGetCount(tpr01_ParseInfoContainer *ParseInfo)
{
  return(pr09GetItemCount(ParseInfo->Desc));
}

/*!
  Function:     pr01ParseInfoEnumDesc

  see also:

  Description:  Iterator Function for ParseInfo Container

  Arguments:
    *ParseInfo [in] pointer to ParseInfo Container

  return value:
    <UL>
        <LI>tpr01_ParseInfoDesc* : pointer to then next StatementDescriptor
                in the Statement Container.
        <LI>null pointer : if no next item in Statement Container exists
    </UL>

 */
static tpr01_ParseInfoDesc *pr01ParseInfoEnumDesc(tpr01_ParseInfoContainer *ParseInfo)
{
  tpr01_ParseInfoDesc *ParseInfoDesc=pr09GetNextItem(ParseInfo->Desc);
  return(ParseInfoDesc);
}

/*!
  Function:     pr01ParseInfoCloseCont

  see also:

  Description:  deletes all elements in  ParseInfo container

  Arguments:
    ParseInfo [in] pointer to ParseInfo Conatainer

  return value: void
 */
static void pr01ParseInfoCloseCont(tpr01_ParseInfoContainer *ParseInfo)
{
  pr09CloseAllItems(ParseInfo->Desc);
}

/*!
  Function:     pr01ParseInfoSetIndex

  see also:

  Description:  sets iterator for ParseInfo container to given index

  Arguments:
    ParseInfo [in] pointer to ParseInfo Conatainer
    cbIndex   [in] index to which the iterator will be sets
  return value: index of last actual pointer
 */
static tsp00_Int4 pr01ParseInfoSetIndex(tpr01_ParseInfoContainer *ParseInfo, tsp00_Int4 cbIndex)
{
  return(pr09SetIndex(ParseInfo->Desc, cbIndex));
}

/*!
  Function:     pr01ParseInfoSetReferenceCount

  see also:

  Description:  Increments the reference count for a ParseInfoDesc

  The reference count for the ParseInfoDesc will incremented. The reference
  count is used to determine if and how much descriptors hold a reference
  to this ParseInfoDesc. This function must be called if a descriptor creates
  a new reference to the ParseInfoDesc.

  Arguments:
    ParseInfoDesc [in] pointer to ParseInfo descriptor
  return value: index of last actual pointer
 */
static void pr01ParseInfoSetReferenceCount(tpr01_ParseInfoDesc *ParseInfoDesc)
{
  ParseInfoDesc->referenceCnt++;
}

/*!
  Function:     pr01ParseInfoFindDesc

  see also:

  Description:  searchs for a ParseInfoDesc in a container

  Note, if a hashindex is given this will be used for searching instead of SQL command.

  Arguments:
    ParseInfo    [in] pointer to ParseInfo Container
    SQLStatement [in] SQL command, for which ParseInfoDesc you are looking for
    HashIndex    [in] hash index of SQL command
  return value: if founded a pointer to tpr01_ParseInfoDesc else a null pointer
 */
static tpr01_ParseInfoDesc *pr01ParseInfoFindDesc(tpr01_ParseInfoContainer *ParseInfo, tpr05_String *SQLStatement, tsp00_Int4 HashIndex)
{
  M90TRACE(M90_TR_ENTRY, "pr01ParseInfoFindDesc", 0);
  {
    tpr09HashValue* tmpHashItem=NULL;
    tpr01_ParseInfoDesc *ParseInfoDesc=NULL;

    ParseInfo->CacheStat.requests++;

    if (!SQLStatement && HashIndex < 0)
      return(NULL);

    if (HashIndex < 0) {
        /* searching for SQL statement in hashtable. The SQL string will used as key */
        tmpHashItem = pr09HTGetHashItemByKey (&(ParseInfo->HashTable),
                                               (tpr09HashKey) SQLStatement->rawString,
                                                SQLStatement->cbLen
                                             );
    }
    else {
        /* searching for SQL statement in hashtable. The HashIndex will used as key */
        tmpHashItem = pr09HTGetHashItemByIndex (&(ParseInfo->HashTable),
                                               (tpr09HashIndex) HashIndex,
                                               (tpr09HashKey) SQLStatement->rawString,
                                                SQLStatement->cbLen
                                                );
    }

    if (tmpHashItem) {
       ParseInfoDesc = (tpr01_ParseInfoDesc*) tmpHashItem;
       ParseInfo->CacheStat.founds++;
       if (ParseInfoDesc->State != created_epr01ParseInfo) {
         /* When ParseInfoDesc is hashed, put founded descriptor to end of LRU list */
         pr01ParseInfoAccessLRUItem(ParseInfoDesc);
       }
    }
    M90TRACE(M90_TR_EXIT, "pr01ParseInfoFindDesc", 0);
    M90TRACE(M90_TR_HANDLE, "ParseInfoDesc", &ParseInfoDesc);
    return(ParseInfoDesc);
  }
}

/*!
  Function:     pr01ParseInfoParse

  see also:

  Description:  parses the sql command corresponding to given ParseInfoDesc
                and stores the parse informations in the ParseInfoDesc

  Arguments:
    SQLDesc             [in|out] pointer to SQL Descriptor
    cmdfetch            [in|out] ???
    datafound           [in|out] flag for data found
    tpr01_ParseInfoDesc [out]    **pointer which returns the founded parseinfo descriptor
  return value: void
 */
tsp00_Int4 pr01ParseInfoParse(tpr01_SQLDesc *SQLDesc, tsp00_Int2 *cmdfetch, tsp00_Int4 *datafound)
{
  tpr01_ParseInfoContainer *ParseInfoCache = SQLDesc->ConDesc->ParseInfoCache;
  tpr01_StmtNameDesc * StmtNameDesc = SQLDesc->StmtNameDesc;
  sqlcatype *sqlca = SQLDesc->SQL->GetSqlca(SQLDesc);
  sqlxatype *sqlxa = SQLDesc->SQL->GetSqlxa(SQLDesc);
  tsp00_Int4 ret = FALSE;
  tpr01_ParseInfoDesc **ParseInfoDesc = &StmtNameDesc->ParseInfoDesc;
  M90TRACE(M90_TR_ENTRY, "pr01ParseInfoParse", 0);
  M90TRACE(M90_TR_HANDLE, "SQLDesc", SQLDesc);
  {
    (*ParseInfoDesc) = NULL;
    pr06ParseIdKnlSessionIDCopy(&(StmtNameDesc->KnlSessionID), &(SQLDesc->ConDesc->ga->gaKnlSessionID));
    if ( StmtNameDesc->StmtTypeDesc.Cacheable == notcacheable_epr01
        || ! ParseInfoCache ) {
       M90TRACE(M90_TR_EXIT, "pr01ParseInfoParse", 0);
       ret =  pr01cParse(SQLDesc, cmdfetch, datafound);   
       if (sqlca->sqlcode == 0 && SQLDesc->Attribute.parspre) {
         /* fill kaprasid and orcolpointer, set kastate = execute */
         p04GetSFInfo(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
         /* fill  orrespointer, */
         p01resallocinfo(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
         /* fill  orresnampointer,*/
         pr04ColNames2ore(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
       }
       return ret;
    }    
    else {
      /*searching for SQL command in the parse info cache*/
      *ParseInfoDesc = ParseInfoCache->FindDesc(ParseInfoCache, StmtNameDesc->SQLStatement, -1);
      if (!(*ParseInfoDesc)) {
        *ParseInfoDesc = ParseInfoCache->AddDesc(ParseInfoCache, StmtNameDesc->SQLStatement);
        ParseInfoCache->SetReferenceCount (*ParseInfoDesc);
        /*pr01TracePrintf(sqlca->sqlrap, "ADDING NEW PARSEINFODESC: %x", *ParseInfoDesc);*/
      }
      else if ((*ParseInfoDesc)->State == parsed_epr01ParseInfo
               && pr06ParseIdCheckKnlSessionID(&SQLDesc->ConDesc->ga->gaKnlSessionID, &((*ParseInfoDesc)->ParseInfos).ParseId) 
               && ! sqlca->sqlrap->raparse_again) {
        /*use cached parse infos*/
        StmtNameDesc->ParseInfoDesc = *ParseInfoDesc;
        ParseInfoCache->SetReferenceCount (*ParseInfoDesc);
        pr01ParseInfoGetParseInfos((*ParseInfoDesc), SQLDesc->ore, SQLDesc->ka);

        /*make trace output*/
        if (SQLDesc->Attribute.massopen) {
          pr01TracePrintf(sqlca->sqlrap, " ");
          pr01TracePrintf(sqlca->sqlrap, "MASS STATEMENT :  ");
        }  
        pr01TraceCmd(sqlca->sqlrap, StmtNameDesc->SQLStatement);
        p01pparsidtrace(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ka->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
        p01pparsidtrace(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ka->kaParseInfo.ParseId, TraceParseIDFromCache_epr00);
        *cmdfetch = 0;
        *datafound = FALSE;
        ret = TRUE;
      }
      else 
         (*ParseInfoDesc)->State = hashed_epr01ParseInfo;
    }
    /*parse command and store parse infos in parseinfo cache*/
    if ((*ParseInfoDesc)->State != parsed_epr01ParseInfo) {
      ret = pr01cParse(SQLDesc, cmdfetch, datafound);
      if (sqlca->sqlcode == 0) {
        if(SQLDesc->Attribute.parspre) {
          /* fill kaprasid, orcolpointer, set kastate = execute */
          p04GetSFInfo(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
          /* fill  orrespointer, */
          p01resallocinfo(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
          /* fill  orresnampointer,*/
          pr04ColNames2ore(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
        }
        /*update parse infos in parseinfo cache*/
        ParseInfoCache->PutParseInfos((*ParseInfoDesc), SQLDesc->ore, SQLDesc->ka);
        (*ParseInfoDesc)->State = parsed_epr01ParseInfo;
      }
      
      if (sqlca->sqlrap->raparse_again){
      /*after -8 parse again, update parseinfos in StmtNameDescs which use this cache entry*/
        tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
        tpr01_StmtNameDesc *tmpStmtNameDesc=NULL;
        StmtName->ResetIterator(StmtName);
        while(tmpStmtNameDesc=StmtName->EnumDesc(StmtName)) {
          /*loop over all StmtNameDesc*/
          if (tmpStmtNameDesc != StmtNameDesc
              && tmpStmtNameDesc->ParseInfoDesc == (*ParseInfoDesc)
              && (*ParseInfoDesc)->SQLStatement->cbLen == tmpStmtNameDesc->SQLStatement->cbLen ) {
            if (! memcmp((*ParseInfoDesc)->SQLStatement->rawString, tmpStmtNameDesc->SQLStatement->rawString,(*ParseInfoDesc)->SQLStatement->cbLen) ) {
              /*StmtName with old parseid found -> update       
              pr01TracePrintf(sqlca->sqlrap, "UPDATING        STMTNAMEDESC %s", tmpStmtNameDesc->StmtNameStruct.stStmtName.rawString);
              pr01TracePrintf(sqlca->sqlrap, "PARSEINFOS FROM STMTNAMEDESC %s", StmtNameDesc->StmtNameStruct.stStmtName.rawString);
              pr01TracePrintf(sqlca->sqlrap, "tempAdr: %x curAdr: %x", tmpStmtNameDesc->ParseInfoDesc, StmtNameDesc->ParseInfoDesc);
              pr01TracePrintf(sqlca->sqlrap, "PARSEINFODESC->SQL: %s", (*ParseInfoDesc)->SQLStatement->rawString);*/
              pr01ParseInfoGetParseInfos(StmtNameDesc->ParseInfoDesc, tmpStmtNameDesc->ore, tmpStmtNameDesc->ka);
            }  
          }
	}
      }  
    }  
    M90TRACE(M90_TR_EXIT, "pr01ParseInfoParse", 0);
  }
  return ret;
}


/*!
  Function:     pr01ParseInfoParseTrace

  see also:

  Description:  traces statistic data about ParseInfoCache

  Arguments:
    ParseInfo [in] pointer to ParseInfo container
    sqlra     [in] pointer to sqlra area for trace output
  return value: void
 */
static void pr01ParseInfoTrace(tpr01_ParseInfoContainer *ParseInfo, sqlratype * sqlra)
{
  M90TRACE(M90_TR_ENTRY, "pr01ParseInfoTrace", 0);
  M90TRACE(M90_TR_HANDLE, "ParseInfo", ParseInfo);
  {
    if (sqlra->rasqltap->tatrout != CPR_TRACE_OFF) {
      tpr09HashTable *HashTable = &(ParseInfo->HashTable);
      tsp00_Int2 *t80len  = &sqlra->rasqltap->tastr80l;
      char       *tastr80 = &sqlra->rasqltap->tastr80[0];
      struct SQLERROR errmsg;

      *t80len = 0;
      p03tvfwritetrace (sqlra, &errmsg);

      *t80len = 24;
      memcpy (tastr80, "PARSEINFOCACHE STATISTIC", *t80len);
      p03tvfwritetrace (sqlra, &errmsg);

      *t80len += sprintf(tastr80, "MAX SIZE   : %d entries\nMAX FILLING: %d entries\nLOAD FACTOR: %d%%",
                         HashTable->cbMaxCapacity, HashTable->cbMaxElem, HashTable->cbLoadFactor);
      p03tvfwritetrace (sqlra, &errmsg);

      *t80len = 80;
      memcpy (tastr80, "      | INSERTS | DELETES | FILLING | REQUESTS | FOUNDS | COLLISIONS | HITRATE |", *t80len);
      p03tvfwritetrace (sqlra, &errmsg);

      *t80len = 80;
      memcpy (tastr80, "------|---------|---------|---------|----------|--------|------------|---------|", *t80len);
      p03tvfwritetrace (sqlra, &errmsg);

      *t80len += sprintf(tastr80, "CACHE |%9lu|%9lu|%8.1f%%|%10lu|%8lu|            |%8.1f%%|" \
                    ,ParseInfo->CacheStat.inserts\
                    ,ParseInfo->CacheStat.deletes\
                    ,(float)(100*HashTable->cbActElem)/(float)HashTable->cbMaxElem\
                    ,ParseInfo->CacheStat.requests\
                    ,ParseInfo->CacheStat.founds\
                    ,(ParseInfo->CacheStat.requests==0)?0:(double)((tsp00_Longuint)(100*ParseInfo->CacheStat.founds))/(double)((tsp00_Longuint)ParseInfo->CacheStat.requests));
      p03tvfwritetrace (sqlra, &errmsg);

      *t80len = 80;
      memcpy (tastr80, "------|---------|---------|---------|----------|--------|------------|---------|", *t80len);
      p03tvfwritetrace (sqlra, &errmsg);

      *t80len += sprintf(tastr80, "HASHT |%9lu|%9lu|%8.1f%%|%10lu|%8lu|%12lu|%8.1f%%|" \
                    ,HashTable->statistic.inserts\
                    ,HashTable->statistic.deletes\
                    ,(float)(100*HashTable->cbActElem)/(float)HashTable->cbMaxElem\
                    ,HashTable->statistic.requests\
                    ,HashTable->statistic.founds\
                    ,HashTable->statistic.collisions\
                   ,(HashTable->statistic.requests==0)?0:(double)((tsp00_Longuint)(100*HashTable->statistic.founds))/(double)((tsp00_Longuint)HashTable->statistic.requests));
      p03tvfwritetrace (sqlra, &errmsg);
    }
    M90TRACE(M90_TR_EXIT, "pr01ParseInfoTrace", 0);
  }
}

/*!
  Function:     pr01ParseInfo_ParseInfosInit

  see also:

  Description:  initializes a tpr01_ParseInfos descriptor

  Arguments:
    ParseInfoDesc   [in] pointer to ParseInfoDesc

  return value: void

 */
static void pr01ParseInfo_ParseInfosInit(tpr01_ParseInfoDesc *ParseInfoDesc)
{
  tpr01_ParseInfos *ParseInfos = &(ParseInfoDesc->ParseInfos);
  pr06ParseIdCopy(&(ParseInfos->ParseId),(tpr00_ParseId*) CPR_PARSIDNULL);
  ParseInfos->sp1rFunctionCode = csp1_nil_fc;
  
  ParseInfos->InputParam.MaxParamCnt  = 0;
  ParseInfos->InputParam.UsedParamCnt = 0;
  ParseInfos->InputParam.sqlrow.ireclen = 0;
  ParseInfos->InputParam.sqlrow.oreclen = 0;
  ParseInfos->InputParam.sqlparam = NULL;
  ParseInfos->InputParam.sqlparamLength = 0;

  ParseInfos->ResultSetParam.MaxParamCnt  = 0;
  ParseInfos->ResultSetParam.UsedParamCnt = 0;
  ParseInfos->ResultSetParam.sqlrow.ireclen = 0;
  ParseInfos->ResultSetParam.sqlrow.oreclen = 0;
  ParseInfos->ResultSetParam.sqlparam = NULL;
  ParseInfos->ResultSetParam.sqlparamLength = 0;

  ParseInfos->ColNamesLen = 0;
  ParseInfos->UsedColNamesLen = 0;
  ParseInfos->ColNames = NULL;
  ParseInfos->ColNamesEncoding = NULL;
}

/*!
  Function:     pr01ParseInfo_ParseInfosDelete

  see also:

  Description:  deletes a tpr01_ParseInfos descriptor

  Arguments:
    ParseInfoDesc [in] pointer to ParseInfoDesc

  return value: void

 */
static void pr01ParseInfo_ParseInfosDelete(tpr01_ParseInfoDesc *ParseInfoDesc)
{
  tpr01_ParseInfos *ParseInfos = &(ParseInfoDesc->ParseInfos);

  pr06ParseIdCopy(&(ParseInfos->ParseId),(tpr00_ParseId*) CPR_PARSIDNULL);
  ParseInfos->sp1rFunctionCode = csp1_nil_fc;
  
  ParseInfos->InputParam.MaxParamCnt  = 0;
  ParseInfos->InputParam.UsedParamCnt = 0;
  ParseInfos->InputParam.sqlrow.ireclen = 0;
  ParseInfos->InputParam.sqlrow.oreclen = 0;
  if (ParseInfos->InputParam.sqlparam) {
    pr03mFree(ParseInfos->InputParam.sqlparam, "ParseInfos->sqlparam");
    ParseInfos->InputParam.sqlparam = NULL;
  }
  ParseInfos->InputParam.sqlparamLength = 0;

  ParseInfos->ResultSetParam.MaxParamCnt  = 0;
  ParseInfos->ResultSetParam.UsedParamCnt = 0;
  ParseInfos->ResultSetParam.sqlrow.ireclen = 0;
  ParseInfos->ResultSetParam.sqlrow.oreclen = 0;
  if (ParseInfos->ResultSetParam.sqlparam) {
    pr03mFree(ParseInfos->ResultSetParam.sqlparam, "ParseInfos->sqlparam");
    ParseInfos->ResultSetParam.sqlparam = NULL;
  }
  ParseInfos->ResultSetParam.sqlparamLength = 0;
  
  ParseInfos->ColNamesLen  = 0;
  ParseInfos->ColNamesEncoding = NULL;
  ParseInfos->UsedColNamesLen = 0;
  if (ParseInfos->ColNames ) {
    pr03mFree(ParseInfos->ColNames, "ParseInfos->ColNames");
    ParseInfos->ColNames = NULL;
  }
}

/*!
  Function:     pr01ParseInfo_GetParseInfos

  see also:

  Description:  copies parse Infos from ore- and ka-area

  Arguments:
    ParseInfoDesc   [in|out] pointer to ParseInfoDesc
    sqlorentry      [in] pointer to ore area
    sqlkaentry      [in] pointer to ka area

  return value: void
 */
static void pr01ParseInfoPutParseInfos(tpr01_ParseInfoDesc *ParseInfoDesc, sqlorentry *orentry, sqlkaentry *kaentry)
{
  tpr01_ParseInfos *ParseInfos = &(ParseInfoDesc->ParseInfos);

  /*get parseid, functioncode from ka*/
  pr06ParseIdCopy(&(ParseInfos->ParseId), &((kaentry->kaParseInfo).ParseId));
  ParseInfos->sp1rFunctionCode = (kaentry->kaParseInfo).sp1rFunctionCode;

  /*get sfi data from ore*/
  if (ParseInfos->InputParam.sqlparamLength < orentry->orcollen) {
    ParseInfos->InputParam.MaxParamCnt    = orentry->orcolcnt;
    ParseInfos->InputParam.sqlparamLength = orentry->orcollen;
    if (ParseInfos->InputParam.sqlparam) {
      ParseInfos->InputParam.sqlparam = 
	pr03mReallocat(ParseInfos->InputParam.sqlparam, ParseInfos->InputParam.sqlparamLength, "InputParam.sqlparam");
    }
    else
      ParseInfos->InputParam.sqlparam = pr03mAllocat(ParseInfos->InputParam.sqlparamLength, "InputParam.sqlparam");
  }
  if (orentry->orcolptr)
    memcpy(ParseInfos->InputParam.sqlparam, orentry->orcolptr, orentry->orcollen);
  ParseInfos->InputParam.UsedParamCnt     = orentry->orcolcntacc;
  ParseInfos->InputParam.sqlrow.ireclen = orentry->orsqlrow.ireclen;
  ParseInfos->InputParam.sqlrow.oreclen = orentry->orsqlrow.oreclen;

  if (ParseInfos->ResultSetParam.sqlparamLength < orentry->orreslen) {
    ParseInfos->ResultSetParam.MaxParamCnt    = orentry->orrescnt;
    ParseInfos->ResultSetParam.sqlparamLength = orentry->orreslen;
    if (ParseInfos->ResultSetParam.sqlparam) {
      ParseInfos->ResultSetParam.sqlparam = 
	pr03mReallocat(ParseInfos->ResultSetParam.sqlparam, ParseInfos->ResultSetParam.sqlparamLength, "ResultSetParam.sqlparam");
    }
    else
      ParseInfos->ResultSetParam.sqlparam = pr03mAllocat(ParseInfos->ResultSetParam.sqlparamLength, "ResultSetParam.sqlparam");
  }
  if (orentry->orresptr)
  memcpy(ParseInfos->ResultSetParam.sqlparam, orentry->orresptr, orentry->orreslen);
  ParseInfos->ResultSetParam.UsedParamCnt   = orentry->orrescntacc;
  ParseInfos->ResultSetParam.sqlrow.ireclen = orentry->orressqlrow.ireclen;
  ParseInfos->ResultSetParam.sqlrow.oreclen = orentry->orressqlrow.oreclen;

  if (ParseInfos->ColNamesLen < orentry->orresnamlen) {
    ParseInfos->ColNamesLen = orentry->orresnamlen;
    if (ParseInfos->ColNames) {
      ParseInfos->ColNames = pr03mReallocat(ParseInfos->ColNames, ParseInfos->ColNamesLen, "ParseInfos->ColNames");
    }
    else
      ParseInfos->ColNames = pr03mAllocat(ParseInfos->ColNamesLen, "ParseInfos->ColNames");
  }
  if (orentry->orresnamptr)
    memcpy(ParseInfos->ColNames, orentry->orresnamptr, orentry->orresnamlen);
  ParseInfos->UsedColNamesLen   = orentry->orresnamacc;
  ParseInfos->ColNamesEncoding = orentry->orresnamencoding;
}

/*!
  Function:     pr01ParseInfoGetParseInfos

  see also:

  Description:  copies parse Infos from parseinfo descriptor to ore- and ka-area

  Arguments:
    ParseInfoDesc   [in]     pointer to ParseInfoDesc
    sqlorentry      [in|out] pointer to ore area
    sqlkaentry      [in|out] pointer to ka area

  return value: void
 */
static void pr01ParseInfoGetParseInfos(tpr01_ParseInfoDesc *ParseInfoDesc, sqlorentry *orentry, sqlkaentry *kaentry)
{
  tpr01_ParseInfos *ParseInfos = &(ParseInfoDesc->ParseInfos);

  /*get parseid, functioncode from ka*/
  pr06ParseIdCopy(&((kaentry->kaParseInfo).ParseId), &(ParseInfos->ParseId));
  (kaentry->kaParseInfo).sp1rFunctionCode = ParseInfos->sp1rFunctionCode;

  /*get sfi data from parseinfodesc*/
  if (orentry->orcollen < ParseInfos->InputParam.sqlparamLength) {
    orentry->orcolcnt = ParseInfos->InputParam.MaxParamCnt;
    orentry->orcollen = ParseInfos->InputParam.sqlparamLength;
    if (orentry->orcolptr) {
      orentry->orcolptr = pr03mReallocat(orentry->orcolptr, orentry->orcollen, "ore->orcolptr");
    }
    else
      orentry->orcolptr = pr03mAllocat(orentry->orcollen, "ore->orcolptr");
  }
  if (ParseInfos->InputParam.sqlparam)
    memcpy(orentry->orcolptr, ParseInfos->InputParam.sqlparam, ParseInfos->InputParam.sqlparamLength);
  orentry->orcolcntacc = ParseInfos->InputParam.UsedParamCnt;
  orentry->orsqlrow.ireclen = ParseInfos->InputParam.sqlrow.ireclen;
  orentry->orsqlrow.oreclen = ParseInfos->InputParam.sqlrow.oreclen;

  if (orentry->orreslen < ParseInfos->ResultSetParam.sqlparamLength) {
    orentry->orrescnt = ParseInfos->ResultSetParam.MaxParamCnt;
    orentry->orreslen = ParseInfos->ResultSetParam.sqlparamLength;
    if (orentry->orresptr) {
      orentry->orresptr = pr03mReallocat(orentry->orresptr, orentry->orreslen, "ore->orresptr");
    }
    else
      orentry->orresptr = pr03mAllocat(orentry->orreslen, "ore->orresptr");
  }
  if (ParseInfos->ResultSetParam.sqlparam)
    memcpy(orentry->orresptr, ParseInfos->ResultSetParam.sqlparam, ParseInfos->ResultSetParam.sqlparamLength);
  orentry->orrescntacc = ParseInfos->ResultSetParam.UsedParamCnt;
  orentry->orressqlrow.ireclen = ParseInfos->ResultSetParam.sqlrow.ireclen;
  orentry->orressqlrow.oreclen = ParseInfos->ResultSetParam.sqlrow.oreclen;
  
  if ( orentry->orresnamlen < ParseInfos->ColNamesLen ) {
    orentry->orresnamlen = ParseInfos->ColNamesLen;
    if (orentry->orresnamptr) {
      orentry->orresnamptr = pr03mReallocat(orentry->orresnamptr, orentry->orresnamlen, "orentry->orresnamptr");
    }
    else
      orentry->orresnamptr = pr03mAllocat(orentry->orresnamlen, "orentry->orresnamptr");
  }
  if (ParseInfos->ColNames)
    memcpy(orentry->orresnamptr, ParseInfos->ColNames, ParseInfos->ColNamesLen);
  orentry->orresnamacc = ParseInfos->UsedColNamesLen;
  orentry->orresnamencoding = ParseInfos->ColNamesEncoding;
}

/*!
  Function:     pr01ParseInfoHashTableInsert

  see also:

  Description:  inserts a ParseInfoDesc into hashtable and sets the HashIndex

  Arguments:
    ParseInfoDesc   [in] pointer to ParseInfoDesc

  return value: void

 */
static void pr01ParseInfoHashTableInsert(tpr01_ParseInfoDesc *ParseInfoDesc)
{
    tpr01_ParseInfoContainer* ParseInfo= ParseInfoDesc->ParseInfo;
    tpr09_HashTableError myError;
    do {
        if (ParseInfoDesc->HashIndex < 0) {
            myError= pr09HTInsertHashItemByValue (&(ParseInfo->HashTable),
                                                  (tpr09HashKey) ParseInfoDesc->SQLStatement->rawString,
                                                  ParseInfoDesc->SQLStatement->cbLen,
                                                  ParseInfoDesc,
                                                  (tpr09HashIndex*)&(ParseInfoDesc->HashIndex));
        }
        else {
            tsp00_Uint4  hKeyLen; 
            tpr09HashKey hKey = pr01ParseInfo_GetKey ( (tpr09HashValue)ParseInfoDesc, &hKeyLen);
            myError= pr09HTInsertHashItemByIndex (&(ParseInfo->HashTable),
                                                  (tpr09HashIndex*)&(ParseInfoDesc->HashIndex),
                                                  hKey,
                                                  hKeyLen,
                                                  ParseInfoDesc);
        }

        if (myError==HashTableFull_epr09) {
           tpr01_ParseInfoDesc *displaceParseInfoDesc = (tpr01_ParseInfoDesc*) (pr09LRUListRemove(&(ParseInfo->LRUList))->data_p);
           tsp00_Uint4  hKeyLen; 
           tpr09HashKey hKey = pr01ParseInfo_GetKey ( (tpr09HashValue)displaceParseInfoDesc, &hKeyLen);
           /* hash table is full, remove the least recently used item from hash table */
           pr09HTRemoveHashItemByIndex (&(ParseInfo->HashTable),
                                        displaceParseInfoDesc->HashIndex,
                                        hKey,
                                        hKeyLen);
           displaceParseInfoDesc->State = uncached_epr01ParseInfo;
           ParseInfo->DeleteDesc(displaceParseInfoDesc, DROP_PARSEID);
        }
    } while (myError == HashTableFull_epr09); /* normally the loop will passes max. two times*/
    ParseInfoDesc->State = hashed_epr01ParseInfo;
}

/*!
  Function:     pr01ParseInfoAccessLRUItem

  see also:

  Description:  inserts or updates a ParseInfoDesc in LRU list

  Arguments:
    ParseInfoDesc   [in] pointer to ParseInfoDesc

  return value: void

 */
static void pr01ParseInfoAccessLRUItem(tpr01_ParseInfoDesc *ParseInfoDesc)
{
    pr09LRUListInsert(&(ParseInfoDesc->ParseInfo->LRUList), &(ParseInfoDesc->LRUListItem));
    return;
}

static tpr09HashValue pr01ParseInfo_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen){
  tpr05_String* string1=(tpr05_String*)((tpr01_ParseInfoDesc*)data_p)->SQLStatement;
  *HashKeyLen = string1->cbLen;
  return string1->rawString;
}

static tsp00_Int4 pr01ParseInfo_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len){
   if (Item1Len != Item2Len)
     return 0;
   return (! memcmp(Item1, Item2, Item1Len));
}

static void pr01ParseInfo_printKey (const tpr09HashValue data_p, FILE* datei){
  tpr05_String* string1=(tpr05_String*)((tpr01_ParseInfoDesc*)data_p)->SQLStatement;
  fprintf(datei, "<ParseInfoDesc addr=0x%p name=\"%.*s\" />", data_p, string1->cbLen, string1->rawString);
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
