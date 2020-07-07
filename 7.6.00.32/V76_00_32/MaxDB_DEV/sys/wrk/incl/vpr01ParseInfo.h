/*!**************************************************************************

  module:    vpr01ParseInfo.h

  -------------------------------------------------------------------------

  responsible: MarcoP

  special area:   interface runtime
  description : cache for information received by parsing the sql command

  This container stores SQL statements and the corresponding parse information.
  The container implements a cache algorithm for parse informations. Recently
  used SQL statements will be stored in a cache and must not parsed again. When
  the cache is full a LRU algoritm will replaced the least recently used statement
  and its parse informations from cache.

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


#ifndef VPR01ParseInfo_H
#define VPR01ParseInfo_H

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr01.h"
#include "vpr09DynaDesc.h"
#include "vpr05AnalyzeSQL.h"
#include "vpr09LRUList.h"
#include "vpr09LinkedList.h"
#include "vpr09HashTable.h"

/*==================================================================*
 *  DEFINES                                                        *
 *==================================================================*/
#define DROP_PARSEID 0
#define KEEP_PARSEID 1
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
/*!
  Declaration: tpr01ParseInfo_State
  Description: lifecycle state types of a SQLStmtDesc
 */

typedef enum tpr01ParseInfo_State {
  created_epr01ParseInfo,  /*ParseInfoDesc is new */
  hashed_epr01ParseInfo,   /*ParseInfoDesc is in cache and in hashed table*/
  parsed_epr01ParseInfo,   /*ParseInfoDesc was parsed and contains parse informations */
  uncached_epr01ParseInfo  /*ParseInfoDesc is removed from cache by LRU-algorithm */
} tpr01ParseInfo_State;

/*!
  Declaration: tpr01_ParamDesc
  Description: this data type describes input paramters or resultset parameters
 */
typedef struct tpr01_ParamDesc {
  tsp00_Int4    MaxParamCnt;               /* maximum number of parameter entries */
  tsp00_Int4    UsedParamCnt;              /* number of currently parameter entries */
  struct SQLROW sqlrow;                    /* length of the whole record described in sqlparam */
  struct SQLCOL *sqlparam;                 /* array, which entries describes the paramters */
  tsp00_Int4    sqlparamLength;            /* length of sqlparam in bytes */
} tpr01_ParamDesc;

/*!
  Declaration: tpr01_ParseInfos
  Description: structure stores data received by parsing sql command
 */
typedef struct tpr01_ParseInfos {
  tpr00_ParseId ParseId;                   /* ParseID */
  tpr00_sp1rFunctionCode sp1rFunctionCode; /* Function Code */
  tpr01_ParamDesc InputParam;              /* input parameters */
  tpr01_ParamDesc ResultSetParam;          /* select list parameters */
  tsp00_Int4      ColNamesLen;             /* maximum number of column names in result set */
  tsp00_Int4      UsedColNamesLen;         /* number of current column names */
  char *          ColNames;                /* string array which contains column names of result set*/
  void *          ColNamesEncoding;        /* string encoding of colnames*/
} tpr01_ParseInfos;

/*!
  Declaration: tpr01_ParseInfoParams
  Description: structure for storing customizable parameters of parseinfo cache
 */
typedef struct tpr01_ParseInfoParams {
    tsp00_Uint4 CacheSize;           /*maximum number of cached Statements*/
    tsp00_Uint4 HashTableLoadFactor; /*measure of how full the hash table is allowed to get
                                       before its capacity is reached value between 0 and 100*/
} tpr01_ParseInfoParams;

/*!
  Declaration: tpr01_ParseInfoDesc
  Description: structure for managing SQL-Statements

  Each SQL statement should be stored in a tpr01_ParseInfoDesc
 */
typedef struct tpr01_ParseInfoDesc {
  tpr01_DescTypeEnum              DescType;
  struct tpr01_ParseInfoContainer *ParseInfo;    /* pointer to his container */
  tpr05_String                    *SQLStatement; /* contains the SQL-Statement */
  tpr01_ParseInfos                ParseInfos;     /* parse informations */
  tsp00_Int4                      HashIndex;     /* HashIndex of tpr01_ParseInfoDesc or -1 for unknown*/
  tpr01ParseInfo_State            State;         /* current state of SQLStmtDesc */
  tsp00_Int                       referenceCnt;  /* counts number of components which holds a reference to this SQLStmt*/
  tpr09LinkedListItem             LRUListItem;   /* stores link data of this SQLStmtDesc for LRUList*/
} tpr01_ParseInfoDesc;


/*!
  Declaration: tpr01_ParseInfoContainer
  Description: container for storing tpr01_ParseInfoDesc`s
 */
typedef struct tpr01_ParseInfoContainer {
  tpr09DynamicDescriptor *Desc;    /* dynydesc handle */
  struct tpr01_ConDesc   *ConDesc;    /* pointer to its corresponding connect descriptor */
  tpr09HashTable      HashTable;   /* hashtable for a fast search in ParseInfoContainer */
  tpr09LinkedList     LRUList;     /* LRUList for ParseInfoContainer */
  tpr09CacheStat      CacheStat;   /* cache statics */

  /*!
  Function:     InitCont

  see also:

  Description:  Initialized a parse info container

  Arguments:
    HT_capacity   [in] number of SQL statements that can cached
    HT_LoadFactor [in] maximal filling size of cache.

  return value: void
 */
  void                (*InitCont)   (struct tpr01_ParseInfoContainer *Cont, struct tpr01_ConDesc *ConDesc, tsp00_Uint4 HT_capacity, tsp00_Int4 HT_LoadFactor);

  /*!
  Function:     OpenCont

  see also:

  Description:  Opens the parse info container

    The iterator for parse info container will set to first position and the
    number of current stored ParseInfoDesc`s will returned.

  Arguments:
    tpr01_ParseInfoContainer   [in] pointer to tpr01_ParseInfoContainer

  return value: number of current stored SQLStmtDesc`s
 */
  tsp00_Int4          (*OpenCont)   (struct tpr01_ParseInfoContainer *Cont);

  /*!
  Function:     CloseCont

  see also:

  Description:  Closes the parse info container

    Deletes all elements in parse info container.

  Arguments:
    tpr01_ParseInfoContainer   [in] pointer to tpr01_ParseInfoContainer

  return value: void
 */
  void                (*CloseCont)  (struct tpr01_ParseInfoContainer *Cont);

  /*!
  Function:     AddDesc

  see also:

  Description:  Adds a new tpr01_ParseInfoDesc into the parse info container

    A new tpr01_ParseInfoDesc will allocates and initializes with the given
    SQL statement. The new tpr01_ParseInfoDesc will added to the parse info container.

  Arguments:
    ParseInfoContainer   [in] pointer to tpr01_ParseInfoContainer
    SQLCommand           [in] SQL command

  return value: the brand new tpr01_ParseInfoDesc
 */
  tpr01_ParseInfoDesc  *(*AddDesc)    (struct tpr01_ParseInfoContainer *ParseInfo, tpr05_String *SQLCommand);

  /*!
  Function:     DeleteDesc

  see also:

  Description:  Destructor for tpr01_ParseInfoDesc

    When nobody holds a references to the given tpr01_ParseInfoDesc (referenceCnt == 0)
    the descriptor will deleted.

  Arguments:
    tpr01_ParseInfoDesc   [in] pointer to tpr01_ParseInfoDesc

  return value: 1 when descriptor was deletet
                0 when descriptor was not deleted
 */
  tsp00_Int4          (*DeleteDesc) (struct tpr01_ParseInfoDesc *ParseInfoDesc, tsp00_Int4 keepParseID);

/*!
  Function:     GetCount

  see also:

  Description:  counts number of current used elements in parse info container

  Arguments:
    ParseInfo [in] pointer to parse info container

  return value: number of current used elements in parse info container
 */
  tsp00_Int4          (*GetCount)   (struct tpr01_ParseInfoContainer *ParseInfo);

/*!
  Function:     EnumDesc

  see also:

  Description:  Iterator Function for parse info container

  Arguments:
    ParseInfo [in] pointer to parse info container

  return value:
    <UL>
        <LI>tpr01_ParseInfoDesc* : pointer to then next parse info descriptor
                                   in the parse info container.
        <LI>null pointer : if no next item in parse info container exists
    </UL>

 */
  tpr01_ParseInfoDesc  *(*EnumDesc)   (struct tpr01_ParseInfoContainer *ParseInfo);

/*!
  Function:     SetIndex

  see also:

  Description:  sets iterator for parse info container to given index

  Arguments:
    Cont     [in] pointer to parse info container
    cbIndex  [in] index to which the iterator will be sets
  return value: index of last actual pointer
 */
  tsp00_Int4          (*SetIndex)   (struct tpr01_ParseInfoContainer *Cont, tsp00_Int4);

/*!
  Function:     SetReferenceCount

  see also:

  Description:  Increments the reference count for a parse info decriptor

  The reference count for the ParseInfoDesc will incremented. The reference
  count is used to determine if and how much descriptors holds a reference
  to this ParseInfoDesc. This function must be called if a descriptor creates
  a new reference to the ParseInfoDesc.

  Arguments:
    ParseInfoDesc [in] pointer to Parse Info descriptor
  return value: index of last actual pointer
 */
  void                (*SetReferenceCount) (struct tpr01_ParseInfoDesc *ParseInfo);

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
  tpr01_ParseInfoDesc  *(*FindDesc)   (struct tpr01_ParseInfoContainer *ParseInfo, tpr05_String *SQLStatement, tsp00_Int4 HashIndex);

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
  tsp00_Int4                (*Parse)      (struct tpr01_SQLDesc *SQLDesc, tsp00_Int2 *cmdfetch, tsp00_Int4 *datafound);

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
void                        (*PutParseInfos) (struct tpr01_ParseInfoDesc *ParseInfoDesc, sqlorentry *orentry, sqlkaentry *kaentry);

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
void                        (*GetParseInfos) (struct tpr01_ParseInfoDesc *ParseInfoDesc, sqlorentry *orentry, sqlkaentry *kaentry);



/*!
  Function:     TraceStatistic

  see also:

  Description:  traces statistic data about ParseInfoCache

  Arguments:
    ParseInfo [in] pointer to ParseInfo container
    sqlra     [in] pointer to sqlra area for trace output
  return value: void
 */
 void                 (*TraceStatistic) (struct tpr01_ParseInfoContainer *ParseInfo, sqlratype * sqlra);
} tpr01_ParseInfoContainer;
/**************************************************************************/
/*  end of tpr01_ParseInfoContainer                                       */
/**************************************************************************/


/*!
  Function:     pr01ParseInfoNewCont

  see also:

  Description:  Creates a new tpr01_ParseInfoContainer

  This function allocates and initializes a new parse info container. A pointer
  to this new container will returned. The container uses a cache algorithm to managing
  the SQL statements. For initializing the cache some paramters are needed.
  The first paramter determines the number of SQL statements which can be cached.
  The second paramter determines the maximal filling size of the cache. This
  has an impact on performance/space requirements of the cache.

  Arguments:
    HT_capacity   [in] corresponding connect descriptor
    HT_capacity   [in] number of SQL statements that can cached
    HT_LoadFactor [in] maximal filling size of cache.

  return value: pointer to the initialized tpr01_ParseInfoContainer
 */
tpr01_ParseInfoContainer *pr01ParseInfoNewCont(struct tpr01_ConDesc *ConDesc, tsp00_Uint4 HT_capacity, tsp00_Int4 HT_LoadFactor);

/*!
  Function:     pr01ParseInfoDeleteCont

  see also:

  Description:  Deletes a tpr01_ParseInfoContainer

  The parse info container deletes first all contained ParseInfoDesc`s and
  than himself.

  Arguments:
    tpr01_ParseInfoContainer   [in] pointer to a tpr01_ParseInfoContainer

  return value: void
 */
void pr01ParseInfoDeleteCont(tpr01_ParseInfoContainer *Cont);

tsp00_Int4 pr01ParseInfoParse(struct tpr01_SQLDesc *SQLDesc, tsp00_Int2 *cmdfetch, tsp00_Int4 *datafound);



#endif /* VPR01ParseInfo_H */
