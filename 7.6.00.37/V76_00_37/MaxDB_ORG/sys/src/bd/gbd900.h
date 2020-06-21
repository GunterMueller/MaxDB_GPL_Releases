/*!
  @defgroup       ObjContainer       persistent object container
*/
/*!
  @ingroup        ObjContainer
  @file           gbd900.h
  @author         FerdiF
  @brief          constants and types needed for the implementation of 
                  persistent object containers
*/

/*
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

#ifndef GBD900_H
#define GBD900_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
 
#ifndef GGG92_H
#   include "ggg92.h"
#endif
#ifndef GBD00_H
#   include "gbd00.h"
#endif
#ifndef HBD01_1_H
#   include "hbd01_1.h" // b01niltree_id
#endif
#ifndef HBD92_H
#   include "hbd92.h"
#endif
#ifndef HEO57_H
#   include "heo57.h"   // vmalloc
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h" // g01abort
#endif
#ifndef HGG01_3_H
#   include "hgg01_3.h" // g01tabid
#endif
#ifndef GSP03_H
#   include "gsp03.h" 
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h" 
#endif
#ifndef GBD05_H
#   include "gbd05.h"
#endif
#ifndef RTEMEM_ALLOCATOR_HPP
#   include "RunTime/MemoryManagement/RTEMem_Allocator.hpp" 
#endif
#ifndef CONTAINER_VECTOR_HPP
#   include "Container/Container_Vector.hpp"
#endif
#ifndef SAPDB_TYPES_HPP
#   include "SAPDBCommon/SAPDB_Types.hpp"
#endif
#ifndef KERNEL_OPMSG_HPP
#   include "KernelCommon/Kernel_OpMsg.hpp"        /* PTS 1114951 FF 2002-04-19 */
#endif
#ifndef SAPDBTRACE_USAGE_HPP
#   include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#endif
#ifndef SAPDBTRACE_TOPIC_HPP
#   include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#endif
#ifndef KERNEL_VTRACE_HPP
#   include "KernelCommon/Kernel_VTrace.hpp"        
#endif
#ifndef SAPDB_MEMCOPYMOVE_HPP
#   include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#endif
#ifndef FILEDIR_OMSKEY_HPP
#   include "FileDirectory/FileDir_OmsKey.hpp"
#endif
#ifndef FILEDIR_OMSVAR_HPP
#   include "FileDirectory/FileDir_OmsVar.hpp"
#endif
#ifndef FILEDIR_FILENO_HPP
#   include "FileDirectory/FileDir_FileNo.hpp"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#include "hta01.h"
#endif

extern SAPDBTrace_Topic ObjContainer_Trace;

/*! 
    if a variable long persistent object is shortened by an update so that it 
    occupies less than the object frame length of the currrent file times 
    BALANCE_LIMIT_GGbd900 (in %) divided by CONT_FRAME_ENLARGE_FACTOR_BD900 
    an other file with smaler frames is choosen to accomodate the the object  
    \attention: not used anymore (PTS 1121823)
*/
#define BALANCE_LIMIT_BD900                         50
   
/*! 
    factor (in %) by which the obj frames in the continuation files increase 
    from a file to the file with next larger frames  
*/
#define CONT_FRAME_ENLARGE_FACTOR_BD900             200

/*!
    maximum number of continuation files which belong to a file containing 
    variable long persistent objects                  
*/
#define MAX_NUM_CONT_OBJ_FILES_BD900                 6

/*!
    number of partitions used by a partitioned index for OmsKeyedObject's 
*/
#define NUM_KEY_PARTITIONS_BD900                    16

/*! 
    this numbers is used in the fileid if the file contains no continuation 
    object. i.e the object frames contain the object header with lock and 
    log infos        
*/
#define PRIM_CONT_OBJ_FILE_NO_BD900                 0
     
/*!
    character used to fill the body of a persistent object, if the no
    data has been stored yet into the object body
*/
#define NIL_OBJ_FILLCHAR_BD900                  0

/*!
    indicating that no garbagecolletor is working on a substructure
    the corresponding controlblock if flagged with NIL_GC_INDEX_BD900
*/
#define NIL_GC_INDEX_BD900                     -1

/*!
    size of the marker for container entries within the dump
*/
#define DUMPMARKSIZE_BD900                      8

/*!
    a 8 byte integer containing the value of MAX_UINT4
*/
#define MAX_UINT4_BD900                         STATIC_CAST(teo00_Uint8,STATIC_CAST(teo00_Uint4,-1))

/*!
    a 8 byte integer containing the value of MAX_UINT8
*/
#define MAX_UINT8_BD900                         STATIC_CAST(teo00_Uint8,-1)

/*!
    pages within a object container are sorted by sequence numbers
    biggest sequence number possible for pages is MAX_LOCK_REQUEST_SEQ_NO_BD900
*/
#define MAX_LOCK_REQUEST_SEQ_NO_BD900           MAX_UINT8_BD900

typedef FileDir_FileNo      tbd900_KeyPartitionFileNoArr[NUM_KEY_PARTITIONS_BD900]; /*!< fileid's of partitioned key index  */

/*!
    @brief     get the 8 byte unsigned int sequence number out of the page
    header
    @param     hi [in]            high 4 bytes
    @param     lo [in]             low 4 bytes
    @return    unsigned 8 bytes sequence number
*/
static teo00_Uint8 bd900GetPageSeqNo(const tbd00_LockRequestSeqNo &hi, 
                                     const tbd00_LockRequestSeqNo &lo)
{
    return ((STATIC_CAST(teo00_Uint4,hi) * (MAX_UINT4_BD900 + 1)) + 
        STATIC_CAST(teo00_Uint4,lo));
};

/*!
    @brief     set the 8 byte unsigned int sequence number out of the page
               header
    @param     seqno [in]         unsigned 8 bytes sequence number
    @param     hi [out]           high 4 bytes
    @param     lo [out]           low 4 bytes
    @return    none
*/
static void bd900SetPageSeqNo(const teo00_Uint8         seqno,
                              tbd00_LockRequestSeqNo   &hi, 
                              tbd00_LockRequestSeqNo   &lo)
{
    hi = STATIC_CAST(tbd00_LockRequestSeqNo,seqno / (MAX_UINT4_BD900 + 1));
    lo = STATIC_CAST(tbd00_LockRequestSeqNo,seqno % (MAX_UINT4_BD900 + 1));
};

static void bd900GetFileNo(const tbd_nodeptr   &pPage,
                           FileDir_FileNo      &fileno)
{
    union {
        struct {
            tsp00_C4    no1;
            tsp00_C4    no2;
        } C4;
        tsp00_C8  fno;
    } uFileno;

    uFileno.C4.no1 = pPage->nd_obj_file_no1();
    uFileno.C4.no2 = pPage->nd_obj_file_no2();
    fileno = *reinterpret_cast<FileDir_FileNo*>(&uFileno.fno);

//    tsp00_C8 cFileNo;
//    SAPDB_MemCopyNoCheck(&cFileNo[0], &pPage->nd_obj_file_no1(), sizeof(pPage->nd_obj_file_no1()));
//    SAPDB_MemCopyNoCheck(&cFileNo[4], &pPage->nd_obj_file_no2(), sizeof(pPage->nd_obj_file_no2()));
//    fileno = cFileNo;
};

static void bd900SetFileNo(tbd_nodeptr    &pPage,
                           const tsp00_C8 &fileno)
{
    union {
        struct {
            tsp00_C4    no1;
            tsp00_C4    no2;
        } C4;
        tsp00_C8  fno;
    } uFileno;

    uFileno.fno = fileno;
    pPage->nd_obj_file_no1() = uFileno.C4.no1;
    pPage->nd_obj_file_no2() = uFileno.C4.no2;

//	SAPDB_MemCopyNoCheck(&pPage->nd_obj_file_no1(), &fileno[0], sizeof(pPage->nd_obj_file_no1()));
//	SAPDB_MemCopyNoCheck(&pPage->nd_obj_file_no2(), &fileno[4], sizeof(pPage->nd_obj_file_no2()));
};

inline static void 
bd900MapFileDirError(tgg00_TransContext &Trans,
                     FileDir_ReturnCode fd_rc,
                     tgg00_BasisError   b_err,
                     char*              file,
                     unsigned int       line)
{
    if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
    {
        Kernel_VTrace() << file << ":" << line 
            << " bd900MapFileDirError:" << fd_rc 
            << " onto b_err:" << b_err 
            << " old b_err:" << Trans.trError_gg00;
    }
    Trans.trError_gg00 = b_err;
};

/*!
    defines the structure of an persistent object frame
*/
struct tbd900_ObjFrame
{
    tgg92_ObjHeader     objHeader_bd900;    /*!< object frame header */
    tgg91_ObjAnyBody    objBody_bd900;      /*!< object frame body   */

    /*!
        initialize object header when object frame is allocated by 
        application
        @param ObjFrameLen [in] size of the object frame
        @param KeyLen [in]  key length if omsKeyedObject else 0         
        @param ObjFrameVers [in] frame version 
        @param bFrameIsForVarObjects [in] flag: variable long object 
        @return none
    */
    inline void  bd900SetToReservedFrame (tsp00_Int4         ObjFrameLen,
                                          tsp00_Int4         KeyLen,
                                          tgg00_ObjFrameVers ObjFrameVers,
                                          tsp00_Bool         bFrameIsForVarObjects);
};

/*!
    defines the structure of a persistent object header for 
    variable objects ( used within continuous container ) 
*/
struct tbd900_ContObjHeader
{
    /*! 
        page number of the OID of the variable persistent object
        ( page number within primary container )
    */
    tsp00_PageNo        cvoRootOIdPno_bd900;    
    /*! 
        page position of the OID of the variable persistent object    
    */
    tgg00_ObjPagePos    cvoRootOIdPos_bd900;
    /*! 
        chain of free frames           
    */
    tgg00_ObjPagePos    cvoNextFreeObj_bd900;   
};

/*!
    defines the structure of a persistent object frame for 
    variable objects ( within continuous container )  
*/
struct tbd900_ContObjFrame
{
    tbd900_ContObjHeader cofHeader_bd900; /*!< header */
    tgg91_ObjAnyBody     cofBody_bd900;   /*!< body   */
};

/*!
    defines the structure of the body of variable object
    frames within the primary container
*/
class tbd900_VarObjBodyHeader
{
public:
    tsp00_Int4          vhdObjSize_bd900;           /*!< object size                  */
    tsp00_Int4          vhdContObjFileNo_bd900;     /*!< continue object container no */
    /*! 
        page number of page within continous container where variable 
        object body is stored
        if object is stored within primary container cvoRootOIdPno_bd900
        is set to NIL_PAGE_NO_GG00
    */
    tsp00_PageNo        vhdContObjPno_bd900;        
    /*! 
        position on page cvoRootOIdPno_bd900 where variable object 
        frame is stored 
        if object is stored within primary container cvoRootOIdPno_bd900
        is set to NIL_OBJ_PAGE_POS_GG92
    */
    tgg00_ObjPagePos    vhdContObjPos_bd900;
    /*!
        variable is set to true, if an object, which is too large to be stored 
        completely in the primary container, is split across the primary and the 
        continuation container.
        if the object is either stored completely in the primary or the continuation
        container, then the variable is set to false.
        \since PTS 1121823
    */
    tsp00_Bool          vhdObjectIsSplit_bd900;
    /*!
        Variable sized objects, which are larger than a single page, are split
        into several pages (by the liboms), which are linked internally by oids.
        The first page of such a chain is called the head and all the others are
        called continuation pages. This information is needed for the oid-iterator.
        The variable is set to 1 if the object is head and 2 if it is a continuation
        object. If the variable equals 0, then it is unknown (migration of old data).
        \since PTS 1124278
    */
    enum eVarPrimObjType {
        VarPrimUndefType   = 0,      // old initialized filler
        VarPrimHeadObj     = 1,      // head object (defines OID of OmsVarObject)
        VarPrimContObj     = 2       // continuation object
    };
    tsp00_Uint1         vhdObjectIsCont_bd900;           

    /*!
        initialize header for variable objects 
        @return none
    */
    void bd900SetToEmptyHeader()
    {
        vhdContObjPno_bd900    = NIL_PAGE_NO_GG00;
        vhdContObjPos_bd900    = NIL_OBJ_PAGE_POS_GG92;
        vhdContObjFileNo_bd900 = PRIM_CONT_OBJ_FILE_NO_BD900;
        vhdObjSize_bd900       = 0;
        vhdObjectIsSplit_bd900 = false;   // PTS 1121823
        vhdObjectIsCont_bd900  = 0;       // PTS 1124278
    }
};

/*!
    defines the structure of the body of variable object
    frames within the primary container
*/
struct tbd900_VarObjBody
{
    tbd900_VarObjBodyHeader vbdHeader_bd900;    /*!< header */
    tgg91_ObjAnyBody        vbdBody_bd900;      /*!< body   */
};

typedef tsp00_Int4       tbd900_PageCounter;    /*!< page counter  */

/*!
    subroot page header information
*/
class cbd900_SubRootPageData
{
    friend class cbd900_SubRootPageInfo;
    
    tbd900_PageCounter     spdFreePageCnt_bd900;       //!< # pages with free objetcs 
};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/

inline void
tbd900_ObjFrame::bd900SetToReservedFrame (tsp00_Int4         ObjFrameLen,
                                          tsp00_Int4         KeyLen,
                                          tgg00_ObjFrameVers ObjFrameVers,
                                          tsp00_Bool         bFrameIsForVarObjects)
{
    objHeader_bd900.ohdNextFreeObj_gg92  = NIL_OBJ_PAGE_POS_GG92;
    objHeader_bd900.ohdFrameVersion_gg92 = ObjFrameVers;
    objHeader_bd900.ohdState_gg92.becomes((KeyLen==0)?obsReserved_egg00:obsKeyReserved_egg00);
    
    /* initialize the object body, i.e. the first four bytes  */
    /* of the body are filled with                            */
    const int InitStringLen             = 11;
    char      InitString[InitStringLen] = "AAAAAAAAAA";
    int       ObjSize                   = ObjFrameLen - KeyLen - sizeof(tgg92_ObjHeader);
    
    if (bFrameIsForVarObjects)
    {
        ObjSize -= sizeof (tbd900_VarObjBodyHeader);
        tbd900_VarObjBody & VarObjBody = *((tbd900_VarObjBody*) (objBody_bd900));
        VarObjBody.vbdHeader_bd900.bd900SetToEmptyHeader();
        SAPDB_MemCopyNoCheck (VarObjBody.vbdBody_bd900, InitString, SAPDB_MIN (InitStringLen, ObjSize));
    }
    else
        SAPDB_MemCopyNoCheck (objBody_bd900, InitString, SAPDB_MIN (InitStringLen, ObjSize));  
}

#endif  /* GBD900_H */
