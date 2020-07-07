/*!
  @file           RTEConf_ParameterAccess.cpp
  @author         RobinW
  @brief          RTE configuration parameters: class for kernel configurable parameter access


              This class allows to handle XParam-Files and its individual parameters.
              Supports parameters of type Integer (8-byte), Real (8-byte), String
              (variable length) and CryptInfo.
              A new file format is introduced which supports the new parameter
              features.
              Old style XParam files can be read, but for writing files always the
              new file format is used.
              Additionallay, an Iterator class is implemented to receive a list
              of all parameters.
  first created  2000-03-22  17:49

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
---------------------------------------------------------------------*/

/*PTS 1115041*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"

#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include "SAPDBCommon/SAPDB_ToString.hpp"

#include "SAPDBCommon/SAPDB_sprintf.h"

#include <errno.h>
#include <string.h>

#include "gsp01.h"  /* tsp01_RteError */

#include "RunTime/Configuration/RTEConf_BuildParamFileName.h"

#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "RunTime/System/RTESys_Time.h"

#include "heo01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/* the structure of the "old-style" parameter records */
/* 'old-style' format of XParam file was a array of fixed length entries */
/* each entry consists of an identifier (18 Character array not Zero terminated */
/* followed by a value record structure containing dummy entries, a tag and a */
/* union of each basic types with a maximum of 64 Characters */
/* The first entry in the file is a special 'header' entry */

/*! define */
#define LENGTH_OF_OLDSYTLE_XPARAM_RECORD_ 86

/*! define */
#define OFFSET_OF_OLDSTYLE_IDENTIFIER_  0
/*! define */
#define LENGTH_OF_IDENTIFIER_          18

//
// there is a three byte gap between identifier and value type tag, which maybe filled with garbage
// that garbage must be ignored!!!!
//
// old xp_value_record format
//
//  struct          xp_value_record
//  {
//        tsp00_Int2        xp_info ;       <<<----- ignored
//        unsigned char   xp_fill2;       <<<----- ignored
//        unsigned char   xp_value_t ;    <<<----- OFFSET_OF_OLDSTYLE_TYPE
//        union           of_xp_value_t   <<<----- OFFSET_OF_OLDSTYLE_VALUE
//        {
//                tsp00_Int2        xp_int2 ;
//                tsp00_Int4        xp_int4 ;
//                tsp00_C8          xp_c8 ;
//                tsp00_C18         xp_c18 ;
//                tsp00_C24         xp_c24 ;
//                tsp00_C40         xp_c40 ;
//                tsp00_C64         xp_c64 ;
//                tsp00_CryptName   xp_crypt ;
//        }  xp_type ;
//  };
//

/*! define */
#define OFFSET_OF_OLDSTYLE_TYPE_       21
/*! define */
#define LENGTH_OF_OLDSTYLE_TYPE_        1

/*! define */
#define OFFSET_OF_OLDSTYLE_VALUE_      22
/*! define */
#define LENGTH_OF_OLDSTYLE_VALUE_      64

/*! define */
#define OLDSTYLE_HEADER_IDENTIFIER_ "XP_FST_XP_HEADER_0"

/*! the "magic" byte that is written to a parameter file to mark the end of the file. It is checked to make sure that the whole file has been read correctly */
#define END_MARKER_ ((SAPDB_Byte)(0x1b))

/*! define */
#define NEWSTYLE_HEADER_IDENTIFIER_    "SAPDB_XPARAM_FILE_"
/*! define */
#define NEWSTYLE_HEADER_IDENTIFIER_V02 NEWSTYLE_HEADER_IDENTIFIER_"02RW"
/*! define */
#define NEWSTYLE_HEADER_IDENTIFIER_V03 NEWSTYLE_HEADER_IDENTIFIER_"03RW"
/*! define */
#define NEWSTYLE_HEADER_IDENTIFIER_V04 NEWSTYLE_HEADER_IDENTIFIER_"04RW"
/*! define */
#define NEWSTYLE_HEADER_IDENTIFIER_V05 NEWSTYLE_HEADER_IDENTIFIER_"05RW"

/*! define */
#define INDEX_OF_VERIFY_TIMESTAMP_  OFFSET_OF_OLDSTYLE_VALUE_
/*! define */
#define LENGTH_OF_VERIFY_TIMESTAMP_ (8*2)

/*! define */
#define INDEX_OF_CRASH_FLAG_        (OFFSET_OF_OLDSTYLE_VALUE_ + LENGTH_OF_VERIFY_TIMESTAMP_)
/*! define */
#define LENGTH_OF_CRASH_FLAG_       1

/*! define */
#define INDEX_OF_NAME_LENGTH_       (INDEX_OF_CRASH_FLAG_ + LENGTH_OF_CRASH_FLAG_)
/*! define */
#define LENGTH_OF_NAME_LENGTH_      4

/*! define */
#define INDEX_OF_PARAM_LENGTH_      (INDEX_OF_NAME_LENGTH_ + LENGTH_OF_NAME_LENGTH_)
/*! define */
#define LENGTH_OF_PARAM_LENGTH_     4

/*! define */
#define INDEX_OF_CHECKSUM_          (INDEX_OF_PARAM_LENGTH_ + LENGTH_OF_PARAM_LENGTH_)
/*! define */
#define LENGTH_OF_CHECKSUM_         8

/*! As one single list for all parameters would be quite long, the parameters are internally stored in more than one list. The list
   a certain parameter is stored in is chosen by looking at the character at a special position within the name of the parameter.
   KEY_INDEX_ gives the position to look at. For the current set of kernel parameters, 6 is a good choice in order to put about the
   same number of parameters in each list.
*/
#define KEY_INDEX_ 6

/*! the following defines are needed for calculating the number of lists that are needed and for choosing the right list for a
   given parameter. As the characters from 'A' to 'Z' and '0' to '9' and the underscore ('_') are allowed in parameter names,
   one list has to provided for each of those characters. An additional list is provided for parameters with names that are
   shorter than KEY_INDEX_ . Choosing the list for a given parameter name is done in CalcIndex().
*/

/*! define */
#define INDEX_OF_LIST_FOR_SHORT_NAMES_               0 /* leave the list with index 0 for short names and special characters */
/*! define */
#define INDEX_OF_FIRST_LIST_FOR_CHARS_FROM_A_TO_Z_   1
/*! define */
#define INDEX_OF_FIRST_LIST_FOR_CHARS_FROM_0_TO_9_   (INDEX_OF_FIRST_LIST_FOR_CHARS_FROM_A_TO_Z_ + NUMBER_OF_CHARS_FROM_A_TO_Z_)
/*! define */
#define INDEX_OF_LIST_FOR_UNDERSCORE_                (INDEX_OF_FIRST_LIST_FOR_CHARS_FROM_0_TO_9_ + NUMBER_OF_CHARS_FROM_0_TO_9_)

/*! size of one buffer used in reading strings from the parameter file. If a string is longer than this value, additional buffers are used */
#define SIZE_OF_ONE_BUFFER_ 256

/*! the codes for the identifiers for the different types of parameters in the old style XParam files */
/*! define */
#define OLDSTYLE_CODE_FOR_INT2_         0x0                     /*! < */
/*! define */
#define OLDSTYLE_CODE_FOR_INT4_         0x1                     /*! < */
/*! define */
#define OLDSTYLE_CODE_FOR_C8_           0x2                     /*! < */
/*! define */
#define OLDSTYLE_CODE_FOR_C18_          0x3                     /*! < */
/*! define */
#define OLDSTYLE_CODE_FOR_C24_          0x4                     /*! < */
/*! define */
#define OLDSTYLE_CODE_FOR_C40_          0x5                     /*! < */
/*! define */
#define OLDSTYLE_CODE_FOR_C64_          0x6                     /*! < */
/*! define */
#define OLDSTYLE_CODE_FOR_CRYPTINFO_    0x7                     /*! < */

/*! the header of a parameter file is in the format of a parameter record of an old style XParam file,
    but a special identifier code is used to tell that the file is in the new format */
#define NEWSTYLE_CODE_FOR_HEADER_       'W'                     /*! < */

/* if the state of a flag is to be saved in a parameter file, those values are written to signalize the state of the flag */
/*! define */
#define BYTECODE_FOR_TRUE_              ((SAPDB_Byte)(0xff))
/*! define */
#define BYTECODE_FOR_FALSE_             ((SAPDB_Byte)(0))       
/*! define */
#define BYTECODE_FOR_UPDATE_BY_ADMIN_   ((SAPDB_Byte)(1))       
/*! define */
#define BYTECODE_FOR_UPDATE_BY_USER_    ((SAPDB_Byte)(3))      
/* these defines are for RTESys_IOOpen()    */
/*! define */
#define DO_CREATE_      true
/*! define */
#define DO_NOT_CREATE_  false

/*! opening a parameter fail may file when another process is currently accessing the file */
/*! If this case is detected, just sleep for a given interval and try again */
#define NUMBER_OF_ATTEMPTS_ON_OPEN_ 600
/*! the sleep interval between open attempts in 100 milliseconds  */
#define WAIT_INTERVAL_ON_OPEN_      5

#ifdef _WIN32
/*! Windows system error when file open fails because another process is currently accessing the file */
#define SYSTEM_ERROR_FILE_IS_IN_USE_    ERROR_SHARING_VIOLATION
#else
/*! Unix system error when file open fails because another process is currently accessing the file */
#define SYSTEM_ERROR_FILE_IS_IN_USE_    EACCES
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*! define */
#define ALLOCATE_ARRAY(TypeOfElement,NumberOfElements) (TypeOfElement *)AllocateMemory(sizeof(TypeOfElement) * (NumberOfElements))

/*! define */
#define DEALLOCATE_MEMORY(address) { RTEMem_RteAllocator::Instance().Deallocate((void *)(address)); address = NULL; }

#ifdef WIN32
/*! define */
#define LAST_SYSTEM_ERROR_          ((SAPDB_ULong)GetLastError())
#else
/*! define */
#define LAST_SYSTEM_ERROR_          errno
#endif

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*! @brief  This function uses the RteAllocator to allocate the desired amount of memory. If the allocation fails, 
            RTE_Crash() is called. All implemented classes have a new operator that uses this function. 
*/
static inline void *AllocateMemory(size_t size);

static SAPDB_Bool isRelativePath(RTEConf_Parameter::String path);

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  @class RTEConf_ParameterRecord
  @brief parameter record
 */
class RTEConf_ParameterRecord
{
private:
    /*!
      @brief complete ctor
      @param name [in] parameter name
      @param paramClassIndex [in] parameter class
      @param updateOnlinePossible [in] online changable
      @param permanentChange [in] permanent changable
      @param updateFunc [in] update function
      @param updateContext [in] update context
     */
    RTEConf_ParameterRecord(
        RTEConf_Parameter::Name name = NULL,
        SAPDB_Byte paramClassIndex = 0,
        SAPDB_Bool updateOnlinePossible = false,
        SAPDB_Bool permanentChange = true,
        RTEConf_Parameter::UpdateFuncType updateFunc = NULL,
        void *updateContext = NULL) 
    :
        m_Next(NULL),
        m_UpdateOnline(updateOnlinePossible),
        m_ClassIndex(paramClassIndex),
        m_PermanentChange(permanentChange),
        m_UpdateFunc(updateFunc),
        m_UpdateContext(updateContext)
	{
        if(name)
        {
            m_Name = RTEConf_MakeACopyOfString( name );
        }
        else
        {
            m_Name = NULL;
        }
	}

    /*!
      @brief assignment operator
      @param templateParameterRecord [in] template parameter
     */
    RTEConf_ParameterRecord & operator=(const RTEConf_ParameterRecord &templateParameterRecord)
    {
        m_Name              = RTEConf_MakeACopyOfString( templateParameterRecord.m_Name );
        m_Next              = NULL;
        m_UpdateOnline      = templateParameterRecord.m_UpdateOnline;
        m_ClassIndex        = templateParameterRecord.m_ClassIndex;
        m_PermanentChange   = templateParameterRecord.m_PermanentChange;
        m_UpdateFunc        = templateParameterRecord.m_UpdateFunc;
        m_UpdateContext     = templateParameterRecord.m_UpdateContext;
        
        return (RTEConf_ParameterRecord&)templateParameterRecord;
    }

    /*!
      @brief virtual dtor
     */
	virtual ~RTEConf_ParameterRecord();

    RTEConf_ParameterRecord *m_Next; ///< next parameter

    /*!
      @brief new operator
      @param size [in] implizit size parameter
      @return new RTEConf_ParameterRecord
     */
	void *operator new(size_t size);

    /*!
      @brief delete operator
      @param address [in] implizit address parameter
      @return none
     */
	void operator delete(void *address);

	RTEConf_Parameter::Name             m_Name;
    SAPDB_Byte                          m_ClassIndex;
    SAPDB_Bool                          m_UpdateOnline;
    SAPDB_Bool                          m_PermanentChange;
    RTEConf_Parameter::UpdateFuncType   m_UpdateFunc;
    RTEConf_Parameter::Type             m_Type;
    void                               *m_UpdateContext;
friend class RTEConf_Parameter;
friend class RTEConf_ParameterIterator;
friend class RTEConf_ParameterRecordInteger;
friend class RTEConf_ParameterRecordReal;
friend class RTEConf_ParameterRecordString;
friend class RTEConf_ParameterRecordCryptInfo;
};

/*!
  @class RTEConf_ParameterRecordInteger
  @brief integer parameter record
 */
class RTEConf_ParameterRecordInteger : public RTEConf_ParameterRecord
{
private:
    /*!
      @brief default ctor 
     */
    RTEConf_ParameterRecordInteger()
	{
		m_intValue = 0;
		m_Type = RTEConf_Parameter::t_Integer;
	};

    /*!
      @brief complete ctor 
      @param name [in] parameter name
      @param paramClassIdex [in] parameter class
      @param intValue [in] integer value string
      @param updateOnlinePossible [in] indicate online changable parameter
      @param permanentChange [in] indicate permanent changes
      @param updateFunc [in] update function
      @param updateContext [in] update context
     */
    RTEConf_ParameterRecordInteger(RTEConf_Parameter::Name name
        ,SAPDB_Byte paramClassIndex
        ,void *intValue
        ,SAPDB_Bool updateOnlinePossible
        ,SAPDB_Bool permanentChange,
        RTEConf_Parameter::UpdateFuncType updateFunc = NULL
        ,void *updateContext = NULL)
        : RTEConf_ParameterRecord(name,paramClassIndex,updateOnlinePossible,permanentChange,updateFunc,updateContext)
    {
        SAPDB_MemCopyNoCheck(&m_intValue,intValue,sizeof(RTEConf_Parameter::Integer));
		m_Type = RTEConf_Parameter::t_Integer;
    }
    
    /*!
      @dtor
     */
    ~RTEConf_ParameterRecordInteger()
	{
	};

    /*!
      @brief get integer value
      @return value
     */
	RTEConf_Parameter::Integer Value(void)
	{
		return m_intValue;
	};

    /*!
      @brief compare integer value
      @return true if identical, false if different
     */
	SAPDB_Bool IsDifferent(RTEConf_Parameter::Integer intValue)
	{
		return (m_intValue != intValue) ;
	};

    /*!
      @brief assign integer value
      @return none
     */
    void Assign(void *intValue)
    {
        SAPDB_MemCopyNoCheck(&m_intValue,intValue,sizeof(RTEConf_Parameter::Integer));
    };

	RTEConf_Parameter::Integer m_intValue; ///< the value
friend class RTEConf_Parameter;
friend class RTEConf_ParameterIterator;
};

/*!
  @class RTEConf_ParameterRecordReal
  @brief parameter record for floating pointer number value parameter
 */
class RTEConf_ParameterRecordReal : public RTEConf_ParameterRecord
{
private:
    /*!
      @brief default ctor 
     */
    RTEConf_ParameterRecordReal()
	{
		m_realValue = 0.0;
		m_Type = RTEConf_Parameter::t_Real;
	};

    /*!
      @brief complete ctor 
      @param name [in] parameter name
      @param paramClassIdex [in] parameter class
      @param realValue [in] integer value string
      @param updateOnlinePossible [in] indicate online changable parameter
      @param permanentChange [in] indicate permanent changes
      @param updateFunc [in] update function
      @param updateContext [in] update context
     */
    RTEConf_ParameterRecordReal(RTEConf_Parameter::Name name
        ,SAPDB_Byte paramClassIndex
        ,void *realValue
        ,SAPDB_Bool updateOnlinePossible
        ,SAPDB_Bool permanentChange
        ,RTEConf_Parameter::UpdateFuncType updateFunc = NULL,void *updateContext = NULL)
        : RTEConf_ParameterRecord(name,paramClassIndex,updateOnlinePossible,permanentChange,updateFunc,updateContext)
    {
        SAPDB_MemCopyNoCheck(&m_realValue,realValue,sizeof(RTEConf_Parameter::Real));
		m_Type = RTEConf_Parameter::t_Real;
    };

    /*!
      @dtor
     */
    ~RTEConf_ParameterRecordReal()
	{
	};

    /*!
      @brief get real value
      @return value
     */
	RTEConf_Parameter::Real Value(void)
	{
		return m_realValue;
	};

    /*!
      @brief compare real value
      @return true if identical, false if different
     */
	SAPDB_Bool IsDifferent(RTEConf_Parameter::Real realValue)
	{
		return (m_realValue != realValue) ;
	};

    /*!
      @brief assign real value
      @return none
     */
    void Assign(void *realValue)
    {
        SAPDB_MemCopyNoCheck(&m_realValue,realValue,sizeof(RTEConf_Parameter::Real));
    };

	RTEConf_Parameter::Real m_realValue; ///< the value
friend class RTEConf_Parameter;
friend class RTEConf_ParameterIterator;
};

/*!
  @class RTEConf_ParameterRecordString
  @brief string value parameter record
 */
class RTEConf_ParameterRecordString : public RTEConf_ParameterRecord
{
private:
    /*!
      @brief default ctor 
     */
    RTEConf_ParameterRecordString()
	{
		m_stringValue = NULL;
		m_Type = RTEConf_Parameter::t_String;
	};

    /*!
      @brief complete ctor 
      @param name [in] parameter name
      @param paramClassIdex [in] parameter class
      @param stringValue [in] integer value string
      @param updateOnlinePossible [in] indicate online changable parameter
      @param permanentChange [in] indicate permanent changes
      @param updateFunc [in] update function
      @param updateContext [in] update context
     */
    RTEConf_ParameterRecordString(RTEConf_Parameter::Name name
        ,SAPDB_Byte paramClassIndex
        ,RTEConf_Parameter::String stringValue
        ,SAPDB_Bool updateOnlinePossible
        ,SAPDB_Bool permanentChange
        ,RTEConf_Parameter::UpdateFuncType updateFunc = NULL
        ,void *updateContext = NULL)
        : RTEConf_ParameterRecord(name,paramClassIndex,updateOnlinePossible,permanentChange,updateFunc,updateContext)
    {
        m_stringValue = ALLOCATE_ARRAY(SAPDB_UTF8,STRLEN_UTF8(stringValue)+1);
        STRCPY_UTF8(m_stringValue,stringValue);
		m_Type = RTEConf_Parameter::t_String;
    };

    /*!
      @dtor
     */
    ~RTEConf_ParameterRecordString();

    /*!
      @brief get string value
      @return value
     */
    RTEConf_Parameter::String Value(void)
	{
		return m_stringValue;
	};

    /*!
      @brief compare string value
      @return true if identical, false if different
     */
	SAPDB_Bool IsDifferent(RTEConf_Parameter::String stringValue)
	{
        return (STRCMP_UTF8(m_stringValue, stringValue) != 0) ;
	};

    /*!
      @brief assign string value
      @return none
     */
    void Assign(RTEConf_Parameter::String stringValue)
    {
        m_stringValue = ALLOCATE_ARRAY(SAPDB_UTF8,STRLEN_UTF8(stringValue)+1);
        STRCPY_UTF8(m_stringValue,stringValue);
    };

	RTEConf_Parameter::String m_stringValue; ///< the value
friend class RTEConf_Parameter;
friend class RTEConf_ParameterIterator;
};

/*!
  @class RTEConf_ParameterRecordCryptInfo
  @brief crypt info parameter record
 */
class RTEConf_ParameterRecordCryptInfo : public RTEConf_ParameterRecord
{
private:
    /*! 
        @brief default ctor 
     */
    RTEConf_ParameterRecordCryptInfo()
        : m_Value(0)
	{
		m_Type = RTEConf_Parameter::t_CryptInfo;
	};
    
    /*!
      @brief complete ctor 
      @param name [in] parameter name
      @param paramClassIdex [in] parameter class
      @param cryptInfoValue [in] crypt info value
      @param updateOnlinePossible [in] indicate online changable parameter
      @param permanentChange [in] indicate permanent changes
      @param updateFunc [in] update function
      @param updateContext [in] update context
     */
    RTEConf_ParameterRecordCryptInfo(RTEConf_Parameter::Name name
        ,SAPDB_Byte paramClassIndex
        ,RTEConf_Parameter::CryptInfo *cryptInfoValue
        ,SAPDB_Bool updateOnlinePossible
        ,SAPDB_Bool permanentChange
        ,RTEConf_Parameter::UpdateFuncType updateFunc = NULL
        ,void *updateContext = NULL)
        : RTEConf_ParameterRecord(name,paramClassIndex,updateOnlinePossible,permanentChange,updateFunc,updateContext)
    {
        m_Value = (RTEConf_Parameter::CryptInfo *)AllocateMemory(sizeof(RTEConf_Parameter::CryptInfo));
        SAPDB_MemCopyNoCheck(m_Value,cryptInfoValue,sizeof(RTEConf_Parameter::CryptInfo));
		m_Type = RTEConf_Parameter::t_CryptInfo;
    };

    /*!
      @dtor
     */
	~RTEConf_ParameterRecordCryptInfo();

    /*!
      @brief get crypt info value
      @return value
     */
	RTEConf_Parameter::CryptInfo *Value(void)
	{
		return m_Value;
	};

    /*!
      @brief compare crypt info value
      @return true if identical, false if different
     */
	SAPDB_Bool IsDifferent(const RTEConf_Parameter::CryptInfo *cryptInfoValue)
	{
		return (memcmp(m_Value, cryptInfoValue, sizeof(RTEConf_Parameter::CryptInfo)) != 0) ;
	};

    /*!
      @brief assign crypt info value
      @return none
     */
    void Assign(RTEConf_Parameter::CryptInfo *cryptInfoValue)
    {
        m_Value = (RTEConf_Parameter::CryptInfo *)AllocateMemory(sizeof(RTEConf_Parameter::CryptInfo));
        SAPDB_MemCopyNoCheck(m_Value,cryptInfoValue,sizeof(RTEConf_Parameter::CryptInfo));
    };

	RTEConf_Parameter::CryptInfo *m_Value; ///< the value
friend class RTEConf_Parameter;
friend class RTEConf_ParameterIterator;
};

/*! < */
enum IndependentPathType
{
    IndependentPrograms,
    IndependentData
};

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
/* < */
SAPDB_Char *RTEConf_MakeACopyOfString(const SAPDB_Char *urString)
{
    SAPDB_Char *newString;
    newString = ALLOCATE_ARRAY(SAPDB_Char ,strlen(urString)+1);
    strcpy(newString,urString);
    return newString;
}

/*---------------------------------------------------------------------------*/

/* < */
const SAPDB_UTF8 *RTEConf_MakeACopyOfString(const SAPDB_UTF8 *urString)
{
    const SAPDB_UTF8 *newString;
    newString = ALLOCATE_ARRAY(SAPDB_UTF8 ,strlen((SAPDB_Char *)urString)+1);
    strcpy((SAPDB_Char *)newString,(SAPDB_Char *)urString);
    return newString;
}

/*---------------------------------------------------------------------------*/

static inline void *AllocateMemory(size_t size)
{
    void *address = RTEMem_RteAllocator::Instance().Allocate(size);
    if(!address)
    {
        SAPDBErr_MessageList err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_ALLOCATION_FAILED,SAPDB_ToString(size));
        RTE_Crash(err);
    }
    return address;
}

/*---------------------------------------------------------------------------*/

SAPDB_Char *RTEConf_Parameter::TypeName(Type type)
{
    switch(type)
    {
    case t_Integer:
        return (SAPDB_Char *)"Integer";
        break;
    case t_Real:
        return (SAPDB_Char *)"Real";
        break;
    case t_String:
        return (SAPDB_Char *)"String";
        break;
    case t_CryptInfo:
        return (SAPDB_Char *)"CryptInfo";
        break;
    default:
        return (SAPDB_Char *)"unknown type";
        break;
    }
}

/*---------------------------------------------------------------------------*/



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                          class RTEConf_Parameter                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

RTEConf_Parameter::RTEConf_Parameter(const SAPDB_Char *dbname, SAPDB_Int4 version,SAPDB_Int4 maxLengthOfName,SAPDB_Int4 maxLengthOfStringValue) :
    m_DirtyFlag(false),
    m_CrashFlag(false),
    m_maxLengthOfName(maxLengthOfName),
    m_maxLengthOfStringValue(maxLengthOfStringValue),
    m_binaryData(RTEMem_RteAllocator::Instance()),
    m_positionInBinaryData(0),
    m_ClassList(NULL),
    m_maxClassIndex(0),
    m_checkSumFromHeader(0)
{
    if(sp77encodingUCS2Native == sp77encodingUCS2Swapped)
    {
        m_IntelByteOrder = true;
    }
    else
    {
        m_IntelByteOrder = false;
    }

    for(SAPDB_Int4 counter=0;counter < TOTAL_NUMBER_OF_LISTS_;counter++)
        m_ParameterRecords[counter] = NULL;

    m_FileName[0] = 0;
    m_dbnamec.rawAssign(dbname);
    m_version = version;

    memset(&m_VerifyTimeStamp,0,sizeof(TimeStamp));

    m_FileHasBeenRead = true;
    m_FileInOldFormat = false;
}

/*---------------------------------------------------------------------------*/

RTEConf_Parameter::RTEConf_Parameter(const SAPDB_Char *dbname) :
    m_DirtyFlag(false),
    m_CrashFlag(false),
    m_maxLengthOfName(RTECONF_MAXNAMELENGTH),
    m_maxLengthOfStringValue(RTECONF_MAXSTRINGLENGTH),
    m_binaryData(RTEMem_Allocator::Instance()),
    m_positionInBinaryData(0),
    m_ClassList(NULL),
    m_maxClassIndex(0),
    m_checkSumFromHeader(0)
{
    if(sp77encodingUCS2Native == sp77encodingUCS2Swapped)
    {
        m_IntelByteOrder = true;
    }
    else
    {
        m_IntelByteOrder = false;
    }

    for(SAPDB_Int4 counter=0;counter < TOTAL_NUMBER_OF_LISTS_;counter++)
        m_ParameterRecords[counter] = NULL;

    m_FileName[0] = 0;
    m_dbnamec.rawAssign(dbname);
    m_version = 0;

    memset(&m_VerifyTimeStamp,0,sizeof(TimeStamp));

    m_FileHasBeenRead = true;
    m_FileInOldFormat = false;
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::DeleteRecords()
{
    for(SAPDB_Int4 counter = 0;counter < TOTAL_NUMBER_OF_LISTS_;counter++)
    {
        if(m_ParameterRecords[counter])
        {
            RTEConf_ParameterRecord *nextRecord;
            RTEConf_ParameterRecord *currentRecord;
            currentRecord = m_ParameterRecords[counter];
            do
            {
                nextRecord = currentRecord->m_Next;
                delete currentRecord;
                currentRecord = nextRecord;
            }
            while(NULL != currentRecord);
        }
    }
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::CopyRecords(const RTEConf_Parameter &templateParameter)
{
    for(SAPDB_Int4 counter=0;counter<TOTAL_NUMBER_OF_LISTS_;counter++)
    {
        if(NULL != templateParameter.m_ParameterRecords[counter])
        {
            RTEConf_ParameterRecord *templateRecord = templateParameter.m_ParameterRecords[counter];
            RTEConf_ParameterRecord *newRecord;
            RTEConf_ParameterRecord *prevRecord = NULL;
            while(NULL != templateRecord)
            {
                newRecord = NULL;
                switch(templateRecord->m_Type)
                {
                case t_Integer:
                    {
                        Integer copyOfValue = ((RTEConf_ParameterRecordInteger *)templateRecord)->Value();
                        newRecord = new RTEConf_ParameterRecordInteger;
                        ((RTEConf_ParameterRecordInteger *)newRecord)->Assign(&copyOfValue);
                        break;
                    }
                case t_Real:
                    {
                        Real copyOfValue = ((RTEConf_ParameterRecordReal *)templateRecord)->Value();
                        newRecord = new RTEConf_ParameterRecordReal;
                        ((RTEConf_ParameterRecordReal *)newRecord)->Assign(&copyOfValue);
                        break;
                    }
                case t_String:
                    {
                        String copyOfValue;
                        SAPDB_Bool valueTooLong;
                        newRecord = new RTEConf_ParameterRecordString;
                        copyOfValue = RTEConf_MakeACopyOfString( ((RTEConf_ParameterRecordString *)templateRecord)->Value() );
                        CheckIfStringValueTooLong(copyOfValue,valueTooLong);
                        if(valueTooLong)
                        {
                            delete newRecord;
                            newRecord = NULL;
                            DEALLOCATE_MEMORY( copyOfValue );
                        }
                        else
                        {
                            ((RTEConf_ParameterRecordString *)newRecord)->Assign(copyOfValue);
                        }
                        break;
                    }
                case t_CryptInfo:
                    {
                        SAPDB_Int4 *copyOfValue;
                        newRecord = new RTEConf_ParameterRecordCryptInfo;
                        copyOfValue = ALLOCATE_ARRAY( SAPDB_Int4 , RTECONF_CRYPT_ARRAY_SIZE );
                        SAPDB_MemCopyNoCheck(copyOfValue,((RTEConf_ParameterRecordCryptInfo *)templateRecord)->Value(),sizeof(CryptInfo));
                        ((RTEConf_ParameterRecordCryptInfo *)newRecord)->Assign((CryptInfo *)copyOfValue);
                        break;
                    }
                }
                if(NULL != newRecord)
                {
                    *newRecord = *templateRecord;
                }
                templateRecord = templateRecord->m_Next;


                if(NULL == prevRecord)
                {
                    m_ParameterRecords[counter] = newRecord;
                }
                else
                {
                    prevRecord->m_Next = newRecord;
                }
                prevRecord = newRecord;
            }
        }
        else
        {
            m_ParameterRecords[counter] = NULL;
        }
    }

}

/*---------------------------------------------------------------------------*/

/*!
  class list entry
 */
struct RTEConf_Parameter::ClassListEntry
{
    SAPDB_Char     *name ; ///< name
    ClassListEntry *next;  ///< next
    SAPDB_Byte      index; ///< index
    /*!
    @brief           operator new
                        uses RTEMem_RteAllocator to allocate size bytes.
                        If allocation fails, RTECrash() is called

    @param           size [in] number of bytes to allocate

    */
    void *operator new(size_t size)
    {
        return AllocateMemory(size);
    }

    /*!
    @brief           operator delete
                        uses RTEMem_RteAllocator to deallocate the memory block at address.

    @param           address [in] base address of the memory block to deallocate

    */

    void operator delete(void *address)
    {
        DEALLOCATE_MEMORY(address);
    }

}

;
/*---------------------------------------------------------------------------*/

RTEConf_Parameter::RTEConf_Parameter(const RTEConf_Parameter &templateParameter) :
    m_DirtyFlag(templateParameter.m_DirtyFlag),
    m_CrashFlag(templateParameter.m_CrashFlag),
    m_maxLengthOfName(templateParameter.m_maxLengthOfName),
    m_maxLengthOfStringValue(templateParameter.m_maxLengthOfStringValue),
    m_binaryData(RTEMem_RteAllocator::Instance()),
    m_positionInBinaryData(0)
{
    if(0 == templateParameter.m_FileName[0])
    {
        m_FileName[0] = 0;
    }
    else
    {
        strcpy(m_FileName,templateParameter.m_FileName);
    }

    ClassListEntry *pCurrentClassEntry = templateParameter.m_ClassList;
    while(pCurrentClassEntry)
    {
        AddNewParamClass(pCurrentClassEntry->name,pCurrentClassEntry->index);
        pCurrentClassEntry = pCurrentClassEntry->next;
    }


    CopyRecords(templateParameter);
}

/*---------------------------------------------------------------------------*/

RTEConf_Parameter & RTEConf_Parameter::operator=(const RTEConf_Parameter &templateParameter)
{
    m_VerifyTimeStamp = templateParameter.m_VerifyTimeStamp;

    /* the dirty-flag signalizes that the file has not been saved. So this operator can be used to make a temporary backup copy of */
    /* the parameter set */
    m_DirtyFlag = true;

    m_CrashFlag = templateParameter.m_CrashFlag;

    DeleteRecords();

    ClassListEntry *pCurrentClassEntry = templateParameter.m_ClassList;
    while(pCurrentClassEntry)
    {
        AddNewParamClass(pCurrentClassEntry->name,pCurrentClassEntry->index);
        pCurrentClassEntry = pCurrentClassEntry->next;
    }

    CopyRecords(templateParameter);
    return (RTEConf_Parameter&)templateParameter;
}

/*---------------------------------------------------------------------------*/

RTEConf_Parameter::~RTEConf_Parameter()
{
    DeleteRecords();
    ClassListEntry *pCurrentClassEntry = m_ClassList;
    ClassListEntry *pNextClassEntry;
    while(pCurrentClassEntry)
    {
        pNextClassEntry = pCurrentClassEntry->next;
        delete pCurrentClassEntry->name;
        delete pCurrentClassEntry;
        pCurrentClassEntry = pNextClassEntry;
    }
    m_ClassList->next;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int2 RTEConf_Parameter::ReadInt2FromByteArray(SAPDB_Byte *ByteArray)
{
    return ByteArray[0]+(ByteArray[1]<<8);
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::WriteInt2ToByteArray(SAPDB_Byte *ByteArray, SAPDB_Int2 nNum)
{
    ByteArray[0] = nNum % 0x100;
    ByteArray[1] = nNum / 0x100;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::BuildFileNameIfNecessary(SAPDBErr_MessageList & err)
{
    if(0 == m_FileName[0])
    {
        tsp00_Pathc     paramFileNameWithPath;
        tsp01_RteError  rteError;
        if(!RTEConf_BuildParamFileNameWithPath ( m_dbnamec,m_version,paramFileNameWithPath,&rteError))
        {
            if ( rteError.RteErrCode != RTE_NO_ERROR_SP01 )
            {
                err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_GENERAL,rteError.RteErrText);
            }
            return false;
        }
        else
        {
            if(strlen(paramFileNameWithPath.asCharp()) > sizeof(RTE_Path))
            {
                err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_PATH_TOO_LONG,paramFileNameWithPath.asCharp());
                return false;
            }
            else
            {
                sp77sprintf(m_FileName,sizeof(RTE_Path),"%s",paramFileNameWithPath.asCharp());
            }
        }
    }
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetTypeAndValueDirect(
    Name        name, 
    Type        &type, 
    Integer     &intValue, 
    Real        &realValue, 
    String      stringValue, 
    CryptInfo   &cryptValue,  
    SAPDBErr_MessageList & err)
{
    if(!BuildFileNameIfNecessary(err))
        return false;

    tRTESys_IOHandle fileHandle;

    tsp00_VfReturn_Param returnValueFromOpen;
    RTESys_IOOpen ( fileHandle,m_FileName,RTESys_IOReadOnly,DO_NOT_CREATE_,DEFAULT_SECURITY_RTESYS_IO,returnValueFromOpen );
    if(vf_notok == returnValueFromOpen)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_OPEN_FILE_FOR_READ,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    SAPDB_Bool found = false;

    SAPDB_UTF8 nameFromFile[RTECONF_MAXNAMELENGTH+1];
    Type typeFromFile;

    union alignedValueStore
    {
        SAPDB_Byte b[RTECONF_MAXSTRINGLENGTH+1];
        CryptInfo c;
        Real r;
        Integer i;
        void *v;
    } u;
    SAPDB_Byte *value = u.b;

    /* the header of a new style parameter file is in the same format as the records of the old style XParam files. */
    /* So, the routine that reads the records of old style files is always used to read the header.                 */
    /* The contents of that record tell if it is an old style file or a new one.                                    */

    ReadOldStyleXParamRecord(nameFromFile,typeFromFile,value,fileHandle);

    if(0 == STRCMP_UTF8(nameFromFile,OLDSTYLE_HEADER_IDENTIFIER_))  /* old style XParam file */
    {
        m_FileInOldFormat = true;
        do
        {
            ReadOldStyleXParamRecord(nameFromFile,typeFromFile,value,fileHandle);
            if(0 == STRCMP_UTF8(nameFromFile,name))
            {
                type = typeFromFile;
                switch(type)
                {
                case t_String:
                    strcpy((SAPDB_Char *)stringValue,(SAPDB_Char *)value);
                    break;
                case t_Integer:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&intValue,(SAPDB_Byte *)value,sizeof(Integer));
                    break;
                case t_Real:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&realValue,(SAPDB_Byte *)value,sizeof(Real));
                    break;
                case t_CryptInfo:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&cryptValue[0],(SAPDB_Byte *)value,sizeof(CryptInfo));
                    break;
                }
                found = true;
            }
        }
        while(nameFromFile[0] && !found);
    }
    else if(0 == STRNCMP_UTF8(nameFromFile,NEWSTYLE_HEADER_IDENTIFIER_,LENGTH_OF_IDENTIFIER_))  /* new style XParam file */
    {
        SAPDB_Byte *currentElement = value;

        SAPDB_Byte version = atoi((SAPDB_Char *)currentElement);
        currentElement += (strlen(NEWSTYLE_HEADER_IDENTIFIER_V03) - strlen((SAPDB_Char *)nameFromFile));

        currentElement += sizeof(SAPDB_Int2);   // read over the timestamp
        currentElement += sizeof(SAPDB_Int2);
        currentElement += sizeof(SAPDB_Int2);
        currentElement += sizeof(SAPDB_Int2);
        currentElement += sizeof(SAPDB_Int2);
        currentElement += sizeof(SAPDB_Int2);
        currentElement += sizeof(SAPDB_Int2);
        currentElement += sizeof(SAPDB_Int2);

        currentElement += LENGTH_OF_CRASH_FLAG_;   // read over the crash flag

        SAPDB_UInt4 maxLengthOfNameInFile;
        SAPDB_MemCopyNoCheck(&maxLengthOfNameInFile,currentElement,sizeof(SAPDB_Int4));
        currentElement += LENGTH_OF_NAME_LENGTH_;

        if(0 != maxLengthOfNameInFile && maxLengthOfNameInFile > m_maxLengthOfName)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMNAME_LENGTH_TOO_BIG,SAPDB_ToString(maxLengthOfNameInFile),SAPDB_ToString(m_maxLengthOfName));
            tsp00_VfReturn_Param returnValueFromClose;
            RTESys_IOClose(fileHandle,returnValueFromClose);
            if(vf_ok != returnValueFromClose)
            {
                err = err + SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            }
            return false;
        }

        SAPDB_UInt4 maxLengthOfStringValueInFile;
        SAPDB_MemCopyNoCheck(&maxLengthOfStringValueInFile,currentElement,sizeof(SAPDB_Int4));

        if(0 != maxLengthOfStringValueInFile && maxLengthOfStringValueInFile > m_maxLengthOfStringValue)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMVALUE_LENGTH_TOO_BIG,SAPDB_ToString(maxLengthOfStringValueInFile),SAPDB_ToString(m_maxLengthOfStringValue));
            tsp00_VfReturn_Param returnValueFromClose;
            RTESys_IOClose(fileHandle,returnValueFromClose);
            if(vf_ok != returnValueFromClose)
            {
                err = err + SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            }
            return false;
        }

        SAPDB_Bool endReachedCorrectly;

        do
        {
            SAPDB_Bool updateOnlinePossible;
            SAPDB_Byte paramClassIndex;
            if(!ReadNewStyleXParamRecord(version,nameFromFile,updateOnlinePossible,paramClassIndex,typeFromFile,value,&endReachedCorrectly,fileHandle))
            {
                break;
            }

            if(name && 0 == STRCMP_UTF8(name,nameFromFile))
            {
                type = typeFromFile;
                switch(type)
                {
                case t_String:
                    strcpy((SAPDB_Char *)stringValue,(SAPDB_Char *)value);
                    break;
                case t_Integer:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&intValue,(SAPDB_Byte *)value,sizeof(Integer));
                    break;
                case t_Real:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&realValue,(SAPDB_Byte *)value,sizeof(Real));
                    break;
                case t_CryptInfo:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&cryptValue[0],(SAPDB_Byte *)value,sizeof(CryptInfo));
                    break;
                }
                found = true;
            }
        }
        while(nameFromFile[0] && !found);
        if(!endReachedCorrectly && !found)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEWARN_CONF_ENDMARKER_NOT_FOUND);
        }
    }
    else if(nameFromFile && 0 == STRCMP_UTF8(nameFromFile,"KERNELVERSION")) /* due to a compiler/optimizer bug, on hp machines parameter files without header were produced */
    {
        do
        {
            ReadOldStyleXParamRecord(nameFromFile,typeFromFile,value,fileHandle);
            if(0 == STRCMP_UTF8(nameFromFile,name))
            {
                type = typeFromFile;
                switch(type)
                {
                case t_String:
                    strcpy((SAPDB_Char *)stringValue,(SAPDB_Char *)value);
                    break;
                case t_Integer:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&intValue,(SAPDB_Byte *)value,sizeof(Integer));
                    break;
                case t_Real:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&realValue,(SAPDB_Byte *)value,sizeof(Real));
                    break;
                case t_CryptInfo:
                    SAPDB_MemCopyNoCheck((SAPDB_Byte *)&cryptValue[0],(SAPDB_Byte *)value,sizeof(CryptInfo));
                    break;
                }
                found = true;
            }
        }
        while(nameFromFile && !found);
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_UNKNOWN_FILETYPE,m_FileName);
    }

    tsp00_VfReturn_Param returnValueFromClose;
    RTESys_IOClose(fileHandle,returnValueFromClose);
    if(vf_ok != returnValueFromClose)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }
    if(!found)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
    }
    return found;
}

/*---------------------------------------------------------------------------*/

/*! define */
#define BUFFERSIZE_FOR_READ_ 8192
SAPDB_Bool RTEConf_Parameter::Read(SAPDB_Bool &dataRead, SAPDBErr_MessageList & err)
{
    if(!BuildFileNameIfNecessary(err))
        return false;

    bool  DoesExist;
    bool  IsDirectory;

    dataRead = false;

    tsp00_VfReturn_Param returnValueFromExistsCall;
    RTESys_IOCheckExist( m_FileName, &DoesExist, &IsDirectory,returnValueFromExistsCall );
    if(!DoesExist)
    {
        if(vf_ok == returnValueFromExistsCall)  /* if the file does not exist, the return value is true and dataRead is false */
        {
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_OPEN_FILE_FOR_READ,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }
    }

    tRTESys_IOHandle fileHandle = INVALID_HANDLE_RTESYS_IO; // only for -Wall

    tsp00_VfReturn_Param returnValueFromOpen = vf_notok;
    tsp00_VfReturn_Param returnValueFromRead = vf_notok;
    tsp00_VfReturn_Param returnValueFromClose = vf_notok;
    
    // If the file is currently being accessed by another process, it cannot be opened. As the file is accessed for a short time only,
    // wait a little while and try again
    for(SAPDB_UInt attempts=0;attempts < NUMBER_OF_ATTEMPTS_ON_OPEN_;attempts++)
    {
        RTESys_IOOpen ( fileHandle,m_FileName,RTESys_IOReadOnly,DO_NOT_CREATE_,DEFAULT_SECURITY_RTESYS_IO,returnValueFromOpen );
        if ( (vf_ok == returnValueFromOpen) ||  ( SYSTEM_ERROR_FILE_IS_IN_USE_ != RTESys_GetLastError() ))
            break;
        SAPDB_Bool dummyFlag = false;
        RTESys_Timeout(&dummyFlag, WAIT_INTERVAL_ON_OPEN_+1);
    }

    if(vf_notok == returnValueFromOpen)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_OPEN_FILE_FOR_READ,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }
    else
    {
        SAPDB_Byte buffer[BUFFERSIZE_FOR_READ_];
        tRTESys_IOPosition receivedSizeInBytes;
        do
        {
            RTESys_IORead ( fileHandle,buffer,BUFFERSIZE_FOR_READ_,receivedSizeInBytes,returnValueFromRead );
            if(vf_ok == returnValueFromRead)
            {
                if(!AddBinaryData(buffer,(SAPDB_Int4)receivedSizeInBytes,err))
                {
                    return false;
                }
            }
        }
        while(vf_ok == returnValueFromRead);

        RTESys_IOClose(fileHandle,returnValueFromClose);
        if(vf_ok != returnValueFromClose)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }

        SAPDB_Bool result;

        result = InterpretBinaryData(dataRead,err);
        if(!result)
        {
            m_FileHasBeenRead = false;
        }

        return result;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::AddBinaryData(SAPDB_Byte *binaryData,SAPDB_UInt4 length,SAPDBErr_MessageList & err)
{
    SAPDB_Byte *data = (SAPDB_Byte *)AllocateMemory(length);
    SAPDB_MemCopyNoCheck(data,binaryData,length);
    BinaryDataBlock block;
    block.data = data;
    block.length = length;
    m_binaryData.InsertEnd(block);
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::InterpretBinaryData(SAPDB_Bool &dataRead,SAPDBErr_MessageList & err)
{
    SAPDB_Bool result = true;
    dataRead = false;

    SAPDB_UTF8 name[RTECONF_MAXNAMELENGTH+1];

    Type type;

    union alignedValueStore
    {
        SAPDB_Byte b[RTECONF_MAXSTRINGLENGTH+1];
        CryptInfo c;
        Real r;
        Integer i;
        void *v;
    } u;
    SAPDB_Byte *value = u.b;

    /* the header of a new style parameter file is in the same format as the records of the old style XParam files. */
    /* So, the routine that reads the records of old style files is always used to read the header.                 */
    /* The contents of that record tell if it is an old style file or a new one.                                    */
    ReadOldStyleXParamRecord(name,type,value);

    if(0 == STRCMP_UTF8(name,OLDSTYLE_HEADER_IDENTIFIER_))  /* old style XParam file */
    {
        m_FileInOldFormat = true;
        do
        {
            ReadOldStyleXParamRecord(name,type,value);
            if(0 != name[0])
            {
                /* if a parameter with this name already exists, delete it first */
                RTEConf_ParameterRecord *pFoundRecord = LookupRecordByName(name);
                if(NULL != pFoundRecord)
                {
                    SAPDBErr_MessageList errorInDelete;
                    if(!Delete(name,errorInDelete))
                        if(RTEERR_CONF_NOT_FOUND != errorInDelete.ID())
                            err = err + errorInDelete;
                }
                SAPDB_Bool nameTooLong,valueTooLong;
                InsertRecord(name,NULL,false,true,type,value,nameTooLong,valueTooLong);
            }
        }
        while(name[0]);
        dataRead = true;
    }
    else if(name && (0 == STRNCMP_UTF8(name,NEWSTYLE_HEADER_IDENTIFIER_,LENGTH_OF_IDENTIFIER_)))  /* new style XParam file */
    {
        SAPDB_Byte *currentElement = value;

        SAPDB_Byte version = atoi((SAPDB_Char *)currentElement);
        currentElement += (strlen(NEWSTYLE_HEADER_IDENTIFIER_V03) - strlen((SAPDB_Char *)name));

        m_VerifyTimeStamp.Year = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);
        m_VerifyTimeStamp.Month = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);
        m_VerifyTimeStamp.DayOfWeek = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);
        m_VerifyTimeStamp.Day = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);
        m_VerifyTimeStamp.Hour = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);
        m_VerifyTimeStamp.Minute = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);
        m_VerifyTimeStamp.Second = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);
        m_VerifyTimeStamp.Milliseconds = ReadInt2FromByteArray(currentElement);
        currentElement += sizeof(SAPDB_Int2);

        m_CrashFlag = (BYTECODE_FOR_TRUE_ == *currentElement);
        currentElement += LENGTH_OF_CRASH_FLAG_;

        SAPDB_UInt4 maxLengthOfNameInFile;
        SAPDB_MemCopyNoCheck(&maxLengthOfNameInFile,currentElement,sizeof(SAPDB_Int4));
        currentElement += LENGTH_OF_NAME_LENGTH_;

        if(0 != maxLengthOfNameInFile && maxLengthOfNameInFile > m_maxLengthOfName)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMNAME_LENGTH_TOO_BIG,SAPDB_ToString(maxLengthOfNameInFile),SAPDB_ToString(m_maxLengthOfName));
            result = false;
        }

        SAPDB_UInt4 maxLengthOfStringValueInFile;
        SAPDB_MemCopyNoCheck(&maxLengthOfStringValueInFile,currentElement,sizeof(SAPDB_Int4));

        if(0 != maxLengthOfStringValueInFile && maxLengthOfStringValueInFile > m_maxLengthOfStringValue)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMVALUE_LENGTH_TOO_BIG,SAPDB_ToString(maxLengthOfStringValueInFile),SAPDB_ToString(m_maxLengthOfStringValue));
            result = false;
        }

        currentElement += LENGTH_OF_PARAM_LENGTH_;
        SAPDB_MemCopyNoCheck(&m_checkSumFromHeader,currentElement,LENGTH_OF_CHECKSUM_);


        SAPDB_Bool endReachedCorrectly;

        do
        {
            SAPDB_Bool updateOnlinePossible;
            SAPDB_Byte paramClassIndex;
            if(!ReadNewStyleXParamRecord(version,name,updateOnlinePossible,paramClassIndex,type,value,&endReachedCorrectly))
            {
                result = false;
                break;
            }

            if(name[0])
            {
                /* if a parameter with this name already exists, delete it first */
                RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
                if(NULL != foundRecord)
                {
                    SAPDBErr_MessageList errorInDelete;
                    if(!Delete(name,errorInDelete))
                        if(RTEERR_CONF_NOT_FOUND != errorInDelete.ID())
                            err = err + errorInDelete;
                }
                SAPDB_Bool nameTooLong,valueTooLong;
                if( RTECONF_BYTECODE_FOR_PARAMCLASS_ == type)
                {
                    AddNewParamClass((SAPDB_Char *)&name[0],paramClassIndex);
                }
                else
                {
                    InsertRecord(name,GetClassName(paramClassIndex),updateOnlinePossible,true,type,value,nameTooLong,valueTooLong);
                }
            }
        }
        while(name[0]);
        if(!endReachedCorrectly)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEWARN_CONF_ENDMARKER_NOT_FOUND);
        }
        dataRead = true;
        if(version >= 5)
        {
            if(m_checkSumFromHeader != CalcChecksum())
            {
                err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_GENERAL,"checksum does not match, parameter file may be corrupt");
                result = false;
            }
        }
    }
    else if(name && 0 == STRCMP_UTF8(name,"KERNELVERSION")) /* due to a compiler/optimizer bug, on hp machines parameter files without header were produced */
    {
        /* if a parameter with this name already exists, delete it first */
        RTEConf_ParameterRecord *pFoundRecord = LookupRecordByName(name);
        if(NULL != pFoundRecord)
        {
            SAPDBErr_MessageList errorInDelete;
            if(!Delete(name,errorInDelete))
                if(RTEERR_CONF_NOT_FOUND != errorInDelete.ID())
                    err = err + errorInDelete;
        }
        SAPDB_Bool nameTooLong,valueTooLong;
        InsertRecord(name,NULL,false,true,type,value,nameTooLong,valueTooLong);
        do
        {
            ReadOldStyleXParamRecord(name,type,value);
            if(0 != name[0])
            {
                /* if a parameter with this name already exists, delete it first */
                RTEConf_ParameterRecord *pFoundRecord = LookupRecordByName(name);
                if(NULL != pFoundRecord)
                {
                    SAPDBErr_MessageList errorInDelete;
                    if(!Delete(name,errorInDelete))
                        if(RTEERR_CONF_NOT_FOUND != errorInDelete.ID())
                            err = err + errorInDelete;
                }
                SAPDB_Bool nameTooLong,valueTooLong;
                InsertRecord(name,NULL,false,true,type,value,nameTooLong,valueTooLong);
            }
        }
        while(name[0]);
        dataRead = true;
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_UNKNOWN_FILETYPE,m_FileName);
        result = false;
    }
    if(!m_binaryData.IsEmpty())
    {
        DEALLOCATE_MEMORY((*(m_binaryData.Begin())).data);   // No: switch to the next block
        m_binaryData.DeleteFront();
    }
    return result;
}

SAPDB_UInt8 RTEConf_Parameter::CalcChecksum(void)
{
    RTEConf_ParameterIterator iterator(*this);
    Name parameterName;
    Type parameterType;
    SAPDB_Byte paramClassIndex;
    SAPDB_Bool endOfListReached;
    SAPDBErr_MessageList errorMessageFromIteratorNextCall;
    SAPDBErr_MessageList errorMessageFromGetValue;
    SAPDBErr_MessageList errorMessagesWhileWriting;
    SAPDB_UInt8 checkSum=0;
    SAPDB_UInt numberOfElements=0;
    while(iterator.Next(parameterName,parameterType,paramClassIndex,endOfListReached,errorMessageFromIteratorNextCall))
    {
        numberOfElements++;
        switch(parameterType)
        {
        case t_Integer:
            {
                Integer integerValue;
                if(GetValue(parameterName,integerValue,errorMessageFromGetValue))
                {
                    checkSum += integerValue;
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting + errorMessageFromGetValue;
                }
            }
            break;
        case t_Real:
            {
                Real realValue;
                if(GetValue(parameterName,realValue,errorMessageFromGetValue))
                {
                    checkSum += *((Integer *)(&realValue));
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting + errorMessageFromGetValue;
                }
            }
            break;
        case t_String:
            {
                SAPDB_Int4 counter;
                String stringValue;
                if(GetReference(parameterName,stringValue,errorMessageFromGetValue))
                {
                    counter = 0;
                    while(stringValue[counter])
                    {
                        checkSum += stringValue[counter];
                        counter++;
                    }
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting +  errorMessageFromGetValue;
                }
            }
            break;
        case t_CryptInfo:
            {
                SAPDB_Int4 counter;
                CryptInfo cryptInfoValue;
                if(GetValue(parameterName,cryptInfoValue,errorMessageFromGetValue))
                {
                    for(counter = 0;counter < 6;counter++)
                    {                         
                        checkSum += cryptInfoValue[counter];
                    }
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting + errorMessageFromGetValue;
                }
            }
            break;
        default:
            break;
        }
    }
    return checkSum;
}


/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::Write(SAPDBErr_MessageList & err)
{
    if(!m_DirtyFlag)
    {
        return true;
    }

    tRTESys_IOHandle fileHandle = INVALID_HANDLE_RTESYS_IO; // only for -Wall
    tsp00_VfReturn_Param returnValueFromFileAccess = vf_notok;

    if(!BuildFileNameIfNecessary(err))
        return false;

//    RTESys_IOUnlink ( m_FileName,returnValueFromFileAccess );   // remove old file to avoid leftovers...
    // If the file is currently being accessed by another process, it cannot be opened. As the file is accessed for a short time only,
    // wait a little while and try again
    for(SAPDB_UInt attempts=0;attempts < NUMBER_OF_ATTEMPTS_ON_OPEN_;attempts++)
    {
        RTESys_IOOpen ( fileHandle,m_FileName,RTESys_IOWriteOnly,DO_CREATE_,DEFAULT_SECURITY_RTESYS_IO,returnValueFromFileAccess );
        if ( (vf_ok == returnValueFromFileAccess) ||  ( SYSTEM_ERROR_FILE_IS_IN_USE_ != RTESys_GetLastError() ))
            break;
        SAPDB_Bool dummyFlag = false;
        RTESys_Timeout(&dummyFlag, WAIT_INTERVAL_ON_OPEN_+1);
    }

    if(vf_ok != returnValueFromFileAccess )
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_OPEN_FILE_FOR_WRITE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    SAPDB_Byte header[LENGTH_OF_OLDSYTLE_XPARAM_RECORD_];
    memset(header,0,LENGTH_OF_OLDSYTLE_XPARAM_RECORD_);

    SAPDB_Byte c_Integer    = RTECONF_BYTECODE_FOR_INTEGER_;
    SAPDB_Byte c_Real       = RTECONF_BYTECODE_FOR_REAL_;
    SAPDB_Byte c_String     = RTECONF_BYTECODE_FOR_STRING_;
    SAPDB_Byte c_CryptInfo  = RTECONF_BYTECODE_FOR_CRYPTINFO_;

    strcpy((SAPDB_Char *)header,NEWSTYLE_HEADER_IDENTIFIER_V05);

    if(m_CrashFlag)
        header[INDEX_OF_CRASH_FLAG_] = BYTECODE_FOR_TRUE_;
    else
        header[INDEX_OF_CRASH_FLAG_] = BYTECODE_FOR_FALSE_;

    SAPDB_MemCopyNoCheck(&header[INDEX_OF_NAME_LENGTH_],&m_maxLengthOfName,LENGTH_OF_NAME_LENGTH_);
    SAPDB_MemCopyNoCheck(&header[INDEX_OF_PARAM_LENGTH_],&m_maxLengthOfStringValue,LENGTH_OF_PARAM_LENGTH_);

    SAPDB_UInt8 checkSum = CalcChecksum();
    SAPDB_MemCopyNoCheck(&header[INDEX_OF_CHECKSUM_],&checkSum,LENGTH_OF_CHECKSUM_);

    tRTESys_IOPosition writtenSizeInBytes;
    tRTESys_IOPosition wantedSizeInBytes  = LENGTH_OF_OLDSYTLE_XPARAM_RECORD_;
    RTESys_IOWrite ( fileHandle,header,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );

    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }
    RTEConf_ParameterIterator iterator(*this);
    Name parameterName;
    Type parameterType;
    SAPDBErr_MessageList errorMessageFromIteratorNextCall;
    SAPDBErr_MessageList errorMessageFromGetValue;
    SAPDBErr_MessageList errorMessagesWhileWriting;

    SAPDB_Bool endOfListReached;

    /* write the list of paramClasses to the file */
    ClassListEntry *pCurrentClassEntry = m_ClassList;
    while( pCurrentClassEntry )
    {
        wantedSizeInBytes  = STRLEN_UTF8(pCurrentClassEntry->name) + 1;    /* write the string and the terminating 0 */
        RTESys_IOWrite ( fileHandle,pCurrentClassEntry->name,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }
    
        RTESys_IOWrite ( fileHandle,&pCurrentClassEntry->index,1,writtenSizeInBytes,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != 1)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }

        SAPDB_Byte byteCodeToWrite;
        byteCodeToWrite = BYTECODE_FOR_FALSE_;
        RTESys_IOWrite ( fileHandle,&byteCodeToWrite,1,writtenSizeInBytes,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != 1)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }
        byteCodeToWrite = RTECONF_BYTECODE_FOR_PARAMCLASS_;
        RTESys_IOWrite ( fileHandle,&byteCodeToWrite,1,writtenSizeInBytes,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != 1)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }

        pCurrentClassEntry = pCurrentClassEntry->next;
    }    

    SAPDB_Byte paramClassIndex;
    while(iterator.Next(parameterName,parameterType,paramClassIndex,endOfListReached,errorMessageFromIteratorNextCall))
    {
        wantedSizeInBytes  = STRLEN_UTF8(parameterName) + 1;    /* write the string and the terminating 0 */
        RTESys_IOWrite ( fileHandle,parameterName,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }

        SAPDB_Byte byteCodeToWrite;
        if(UpdateOnlinePossible(parameterName))
        {
            byteCodeToWrite = BYTECODE_FOR_UPDATE_BY_USER_;
        }
        else
        {
            byteCodeToWrite = BYTECODE_FOR_FALSE_;
        }
        RTESys_IOWrite ( fileHandle,&paramClassIndex,1,writtenSizeInBytes,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != 1)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }

        RTESys_IOWrite ( fileHandle,&byteCodeToWrite,1,writtenSizeInBytes,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != 1)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }

        switch(parameterType)
        {
        case t_Integer:
            {
                Integer integerValue;
                if(GetValue(parameterName,integerValue,errorMessageFromGetValue))
                {
                    wantedSizeInBytes = sizeof(SAPDB_Byte);
                    RTESys_IOWrite ( fileHandle,&c_Integer,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                    {
                        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                        return false;
                    }
                    wantedSizeInBytes = sizeof(Integer);
                    RTESys_IOWrite ( fileHandle,&integerValue,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                    {
                        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                        return false;
                    }
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting + errorMessageFromGetValue;
                }
            }
            break;
        case t_Real:
            {
                Real realValue;
                if(GetValue(parameterName,realValue,errorMessageFromGetValue))
                {
                    if(!m_IntelByteOrder)
                    {
                        ByteOrderSwapReal(&realValue);
                    }
                    wantedSizeInBytes = sizeof(SAPDB_Byte);
                    RTESys_IOWrite ( fileHandle,&c_Real,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                    {
                        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                        return false;
                    }
                    wantedSizeInBytes = sizeof(Real);
                    RTESys_IOWrite ( fileHandle,&realValue,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                    {
                        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                        return false;
                    }
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting + errorMessageFromGetValue;
                }
            }
            break;
        case t_String:
            {
                String stringValue;
                if(GetReference(parameterName,stringValue,errorMessageFromGetValue))
                {
                    wantedSizeInBytes = sizeof(SAPDB_Byte);
                    RTESys_IOWrite ( fileHandle,&c_String,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                    {
                        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                        return false;
                    }
                    wantedSizeInBytes = STRLEN_UTF8(stringValue)+1;
                    RTESys_IOWrite ( fileHandle,stringValue,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                    {
                        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                        return false;
                    }
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting +  errorMessageFromGetValue;
                }
            }
            break;
        case t_CryptInfo:
            {
                SAPDB_Int4 counter;
                CryptInfo cryptInfoValue;
                if(GetValue(parameterName,cryptInfoValue,errorMessageFromGetValue))
                {
                    wantedSizeInBytes = sizeof(SAPDB_Byte);
                    RTESys_IOWrite ( fileHandle,&c_CryptInfo,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                    {
                        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                        return false;
                    }
                    for(counter = 0;counter < RTECONF_CRYPT_ARRAY_SIZE;counter++)
                    {
                        if(!m_IntelByteOrder)
                        {
                            //ByteOrderSwapInt(&cryptInfoValue[counter]);
                        }
                        wantedSizeInBytes = sizeof(SAPDB_Int4);
                        RTESys_IOWrite ( fileHandle,&cryptInfoValue[counter],wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
                        if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
                        {
                            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
                            return false;
                        }
                    }
                }
                else
                {
                    errorMessagesWhileWriting = errorMessagesWhileWriting + errorMessageFromGetValue;
                }
            }
            break;
        default:
            break;
        }
    }
    if(!errorMessagesWhileWriting.IsEmpty())
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEINFO_INCONSISTENCIES_WHILE_WRITING,m_FileName);
        err = err + errorMessagesWhileWriting;
    }

    if(RTEINFO_CONF_NO_MORE_PARAMETERS_ID != errorMessageFromIteratorNextCall.ID())  /*  RTEINFO_CONF_NO_MORE_PARAMETERS_ID is the error message when the end of the parameter list has been reached. To be ignored, as this is no real error. */
    {
        err = err + errorMessageFromIteratorNextCall;
    }

    SAPDB_Byte endMarker = END_MARKER_;
    wantedSizeInBytes = sizeof(SAPDB_Byte);
    RTESys_IOWrite ( fileHandle,&endMarker,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
    {
        err = err + SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    RTESys_IOClose(fileHandle,returnValueFromFileAccess);
    if(vf_ok != returnValueFromFileAccess)
    {
        err = err + SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    m_DirtyFlag = false;
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::MakeSureParamFileIsInNewFormat(tRTESys_IOHandle &fileHandle,SAPDBErr_MessageList & err )
{
    SAPDB_Byte recordBuffer[18];
    tRTESys_IOPosition receivedSizeInBytes;
    tsp00_VfReturn_Param returnStatus;

    RTESys_IORead ( fileHandle,recordBuffer,18,receivedSizeInBytes,returnStatus );
    if(vf_ok != returnStatus)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_READ_FROM_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    if(0 == memcmp(recordBuffer,OLDSTYLE_HEADER_IDENTIFIER_,18))
    {
        m_DirtyFlag = true;
        tsp00_VfReturn_Param returnValueFromFileAccess;
        RTESys_IOClose(fileHandle,returnValueFromFileAccess);
        if(vf_ok != returnValueFromFileAccess)
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
            return false;
        }

        if(!m_FileHasBeenRead)
        {
            SAPDB_Bool dataRead;
            if(!Read(dataRead,err))
            {
                return false;
            }
        }
        if(!Write(err))
        {
            return false;
        }

        RTESys_IOOpen ( fileHandle,m_FileName,RTESys_IOReadWrite,DO_NOT_CREATE_,DEFAULT_SECURITY_RTESYS_IO,returnValueFromFileAccess );
        if(vf_ok != returnValueFromFileAccess)
        {
            return false;
        }
    }
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::WriteCrashFlag(SAPDB_Byte value,SAPDBErr_MessageList & err)
{
    SAPDB_Bool result;
    tRTESys_IOHandle fileHandle;
    tsp00_VfReturn_Param returnValueFromFileAccess;
    if(!BuildFileNameIfNecessary(err))
        return false;

    RTESys_IOOpen ( fileHandle,m_FileName,RTESys_IOReadWrite,DO_NOT_CREATE_,DEFAULT_SECURITY_RTESYS_IO,returnValueFromFileAccess );
    if(vf_ok != returnValueFromFileAccess)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_OPEN_FILE_FOR_WRITE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    if(!MakeSureParamFileIsInNewFormat(fileHandle,err))
    {
        return false;
    }

    tRTESys_IOPosition newPosition;
    RTESys_IOSeek ( fileHandle,INDEX_OF_CRASH_FLAG_,RTESys_IOSeekSet,newPosition,returnValueFromFileAccess );
    if(INDEX_OF_CRASH_FLAG_ != newPosition)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_SEEK_IN_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        result = false;
    }
    tRTESys_IOPosition writtenSizeInBytes;
    tRTESys_IOPosition wantedSizeInBytes = sizeof(SAPDB_Byte);
    RTESys_IOWrite ( fileHandle,&value,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        result = false;
    }
    RTESys_IOClose(fileHandle,returnValueFromFileAccess);
    if(vf_ok != returnValueFromFileAccess)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        result = false;
    }
    result = true;
    return result;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::SetCrashFlag(SAPDBErr_MessageList & err)
{
    m_CrashFlag = true;
    return WriteCrashFlag(BYTECODE_FOR_TRUE_,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::MarkStoppedCorrectly(SAPDBErr_MessageList & err)
{
    m_CrashFlag = false;
    return WriteCrashFlag(BYTECODE_FOR_FALSE_,err);
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterRecord::~RTEConf_ParameterRecord()
{
    if(m_Name)
        DEALLOCATE_MEMORY (m_Name);
    m_Next = (RTEConf_ParameterRecord *)0;
}

/*---------------------------------------------------------------------------*/

void *RTEConf_ParameterRecord::operator new(size_t size)
{
    return AllocateMemory(size);
}

/*---------------------------------------------------------------------------*/

void RTEConf_ParameterRecord::operator delete(void *address)
{
    DEALLOCATE_MEMORY(address);
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterRecordCryptInfo::~RTEConf_ParameterRecordCryptInfo()
{
    if(m_Value)
        DEALLOCATE_MEMORY(m_Value);
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterRecordString::~RTEConf_ParameterRecordString()
{
    if(m_stringValue)
        DEALLOCATE_MEMORY( m_stringValue );
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterRecord *RTEConf_Parameter::LookupRecordByName(Name name)
{
    RTEConf_ParameterRecord *currentRecord;
    SAPDB_Int4 indexOfList;
    indexOfList = CalcIndex((SAPDB_Char *)name);
    currentRecord = m_ParameterRecords[indexOfList];
    if(NULL != currentRecord)
    {
        while(NULL != currentRecord &&(0 != STRCMP_UTF8((currentRecord->m_Name),name)))
        {
            currentRecord = currentRecord->m_Next;
        }
        if(NULL != currentRecord)
        {
            return currentRecord;
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::CheckIfStringValueTooLong(const SAPDB_UTF8 *stringValue,SAPDB_Bool &valueTooLong)
{
    SAPDB_UInt4 lengthOfStringValue = (SAPDB_UInt4)STRLEN_UTF8(stringValue);
    valueTooLong = (lengthOfStringValue > m_maxLengthOfStringValue);
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::AddNewParamClass(const SAPDB_Char* className,SAPDB_Byte index)
{
    ClassListEntry *pNewClassEntry;
    pNewClassEntry = new ClassListEntry;
    pNewClassEntry->name = RTEConf_MakeACopyOfString(className);
    pNewClassEntry->next = m_ClassList;
    pNewClassEntry->index = index;
    m_ClassList = pNewClassEntry;
}

SAPDB_Byte RTEConf_Parameter::GetClassIndex(const SAPDB_Char* className)
{
    if( NULL == className )
    {
        return 0;
    }
    SAPDB_Int index = 0;
    ClassListEntry *pCurrentClassEntry = m_ClassList;
    while( pCurrentClassEntry )
    {
        if( 0 == strcmp(className,pCurrentClassEntry->name) )
            break;
        pCurrentClassEntry = pCurrentClassEntry->next;
    }
    if( NULL == pCurrentClassEntry )    // new entry
    {
        AddNewParamClass(className,++m_maxClassIndex);
        index = m_maxClassIndex;
    }
    else
    {
        index = pCurrentClassEntry->index;
    }
    return index;
}

/*---------------------------------------------------------------------------*/

const SAPDB_Char* RTEConf_Parameter::GetClassName(SAPDB_Byte classIndex)
{
    ClassListEntry *pCurrentClassEntry = m_ClassList;
    while( pCurrentClassEntry )
    {
        if( classIndex == pCurrentClassEntry->index )
            return pCurrentClassEntry->name;
        pCurrentClassEntry = pCurrentClassEntry->next;
    }
    return NULL;
}
/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::InsertRecord(Name name,const SAPDB_Char *paramClass,SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,Type type,SAPDB_Byte *value,SAPDB_Bool &nameTooLong,SAPDB_Bool &valueTooLong)
{
    nameTooLong = false;
    valueTooLong = false;

    SAPDB_UInt4 lengthOfName = (SAPDB_UInt4)STRLEN_UTF8(name);
    if(lengthOfName > m_maxLengthOfName)
    {
        nameTooLong = true;
        return;
    }
    SAPDB_Int4 indexOfList;
    indexOfList = CalcIndex((SAPDB_Char *)name);

    RTEConf_ParameterRecord *newRecord = NULL;

    switch(type)
    {
    case t_Integer:
        newRecord = new RTEConf_ParameterRecordInteger(name,GetClassIndex(paramClass),value,updateOnlinePossible,permanentChange);
        break;
    case t_Real:
        newRecord = new RTEConf_ParameterRecordReal(name,GetClassIndex(paramClass),value,updateOnlinePossible,permanentChange);
        break;
    case t_String:
        CheckIfStringValueTooLong(value,valueTooLong);
        if(valueTooLong)
        {
            return;
        }
        else
        {
            newRecord = new  RTEConf_ParameterRecordString(name,GetClassIndex(paramClass),value,updateOnlinePossible,permanentChange);
        }
        break;
    case t_CryptInfo:
        newRecord = new RTEConf_ParameterRecordCryptInfo(name,GetClassIndex(paramClass),((CryptInfo *)value),updateOnlinePossible,permanentChange);
        break;
    default:
        return;
    }
    newRecord->m_Next = m_ParameterRecords[indexOfList];

    m_ParameterRecords[indexOfList] = newRecord;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetType (Name name, Type & type,SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord;
    foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        type = foundRecord->m_Type;
        return true;
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
        return false;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name,Integer & value, SAPDBErr_MessageList & err)
{
    SAPDB_Bool updateOnline,permanentChange; // dummies
    return GetValue(name,value,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name, Real & value,      SAPDBErr_MessageList & err)
{
    SAPDB_Bool updateOnline,permanentChange; // dummies
    return GetValue(name,value,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name, String  value,   SAPDBErr_MessageList & err)
{
    SAPDB_Bool updateOnline,permanentChange; // dummies
    return GetValue(name,value,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name,Integer & value,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange, SAPDBErr_MessageList & err)
{
    if(!m_FileHasBeenRead)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_GENERAL,"parameter file could not be read");
        return false;
    }
    RTEConf_ParameterRecord *foundRecord;
    foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        if(t_Integer == foundRecord->m_Type)
        {
            value = ((RTEConf_ParameterRecordInteger *)foundRecord)->Value();
            updateOnline = foundRecord->m_UpdateOnline;
            permanentChange = foundRecord->m_PermanentChange;
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_Integer),TypeName(foundRecord->m_Type));
        }
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name, Real & value,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,      SAPDBErr_MessageList & err)
{
    if(!m_FileHasBeenRead)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_GENERAL,"parameter file could not be read");
        return false;
    }
    RTEConf_ParameterRecord *foundRecord;
    foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        if(t_Real == foundRecord->m_Type)
        {
            value = ((RTEConf_ParameterRecordReal *)foundRecord)->Value();
            updateOnline = foundRecord->m_UpdateOnline;
            permanentChange = foundRecord->m_PermanentChange;
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_Real),TypeName(foundRecord->m_Type));
        }
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name, String  value,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,   SAPDBErr_MessageList & err)
{
    if(!m_FileHasBeenRead)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_GENERAL,"parameter file could not be read");
        return false;
    }
    RTEConf_ParameterRecord *foundRecord;
    foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        if(t_String == foundRecord->m_Type)
        {
            STRCPY_UTF8(value,((RTEConf_ParameterRecordString *)foundRecord)->Value());
            updateOnline = foundRecord->m_UpdateOnline;
            permanentChange = foundRecord->m_PermanentChange;
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_String),TypeName(foundRecord->m_Type));
        }
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetReference(Name name, String & reference,   SAPDBErr_MessageList & err)
{
    SAPDB_Bool updateOnline;SAPDB_Bool permanentChange;  // dummies
    return GetReference(name, reference, updateOnline,permanentChange, err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetReference(Name name, String & reference,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange, SAPDBErr_MessageList & err)
{
    if(!m_FileHasBeenRead)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_GENERAL,"parameter file could not be read");
        return false;
    }
    RTEConf_ParameterRecord *foundRecord;
    foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        if(t_String == foundRecord->m_Type)
        {
            reference = ((RTEConf_ParameterRecordString *)foundRecord)->Value();
            updateOnline = foundRecord->m_UpdateOnline;
            permanentChange = foundRecord->m_PermanentChange;
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_String),TypeName(foundRecord->m_Type));
        }
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name, CryptInfo & value, SAPDBErr_MessageList & err)
{
    SAPDB_Bool updateOnline,permanentChange; // dummies
    return GetValue(name,value,updateOnline,permanentChange,err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetValue(Name name, CryptInfo & value,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange, SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord;
    foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        if(t_CryptInfo == foundRecord->m_Type)
        {
            SAPDB_MemCopyNoCheck(value,((RTEConf_ParameterRecordCryptInfo *)foundRecord)->Value(),sizeof(RTEConf_Parameter::CryptInfo));
            updateOnline = foundRecord->m_UpdateOnline;
            permanentChange = foundRecord->m_PermanentChange;
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_CryptInfo),TypeName(foundRecord->m_Type));
        }
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetRundirectoryAbsolutePath(String value, SAPDBErr_MessageList & err)
{
    tsp00_Pathc     indepDataPath;
    tsp01_RteError  rteError;
    Type            type;
    union
    {
        RTEConf_Parameter::Integer   integerValue;
        RTEConf_Parameter::Real      realValue;
        RTEConf_Parameter::CryptInfo cryptValue;
        SAPDB_UTF8                   rundirectoryParam[RTECONF_MAXSTRINGLENGTH+1];
    } u;

    if(!GetTypeAndValueDirect(KERNELPARAM_RUNDIRECTORY,type,u.integerValue,u.realValue,u.rundirectoryParam,u.cryptValue,err) || RTEConf_Parameter::t_String != type )
    {
        return false;
    }

    if(isRelativePath(u.rundirectoryParam))
    {
        if( !sqlGetIndependentDataPath  (   indepDataPath,
                                            TERM_WITH_DELIMITER_EO01,
                                            &rteError ))
        {
            return false;
        }
        SAPDB_sprintf((SAPDB_Char *)value,sizeof(RTE_Path),"%s%s",indepDataPath.asCharp(),u.rundirectoryParam);
    }
    else
    {
        strcpy((SAPDB_Char *)value,(SAPDB_Char *)u.rundirectoryParam);
    }
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::SetValue(Name name, const Integer & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange, SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL == foundRecord)
    {
        Name copyOfName = RTEConf_MakeACopyOfString( name );
        SAPDB_Byte *copyOfValue = ALLOCATE_ARRAY( SAPDB_Byte, sizeof(Integer) );
        SAPDB_MemCopyNoCheck(copyOfValue,&value,sizeof(Integer));

        SAPDB_Bool nameTooLong,valueTooLong;

        InsertRecord(copyOfName,NULL/*paramClass*/,updateOnlinePossible,permanentChange,t_Integer,copyOfValue,nameTooLong,valueTooLong);

        if(nameTooLong) /* valueTooLong==true can only happen when the value is of type t_String! */
        {
            DEALLOCATE_MEMORY(copyOfName);
            DEALLOCATE_MEMORY(copyOfValue);
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMETER_NAME_TOO_LONG,(SAPDB_Char *)name,SAPDB_ToString(m_maxLengthOfName));
            return false;
        }

        if(!m_DirtyFlag)
        {
            m_DirtyFlag = true;
        }
        return true;
    }
    else
    {
        if (t_Integer == foundRecord->m_Type)
        {
            if (((RTEConf_ParameterRecordInteger *)foundRecord)->IsDifferent(value))
            {
                ((RTEConf_ParameterRecordInteger *)foundRecord)->Assign((void *)&value);
                foundRecord->m_PermanentChange = permanentChange;
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            if ( foundRecord->m_UpdateOnline != updateOnlinePossible )
            {
                foundRecord->m_UpdateOnline = updateOnlinePossible;
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_Integer),TypeName(foundRecord->m_Type));
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::SetValue(Name name, const Real & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL == foundRecord)
    {
        Name copyOfName = RTEConf_MakeACopyOfString( name );

        SAPDB_Byte *copyOfValue = ALLOCATE_ARRAY( SAPDB_Byte, sizeof(Real) );
        SAPDB_MemCopyNoCheck(copyOfValue,&value,sizeof(Real));
        SAPDB_Bool nameTooLong,valueTooLong;
        InsertRecord(copyOfName,NULL/*paramClass*/,updateOnlinePossible,permanentChange,t_Real,copyOfValue,nameTooLong,valueTooLong);

        if(nameTooLong) /* valueTooLong==true can only happen when the value is of type t_String! */
        {
            DEALLOCATE_MEMORY(copyOfName);
            DEALLOCATE_MEMORY(copyOfValue);
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMETER_NAME_TOO_LONG,(SAPDB_Char *)name,SAPDB_ToString(m_maxLengthOfName));
            return false;
        }

        if(!m_DirtyFlag)
        {
            m_DirtyFlag = true;
        }
        return true;
    }
    else
    {
        if (t_Real == foundRecord->m_Type)
        {
            if (((RTEConf_ParameterRecordReal *)foundRecord)->IsDifferent(value))
            {
                ((RTEConf_ParameterRecordReal *)foundRecord)->Assign((void *)&value);
                foundRecord->m_PermanentChange = permanentChange;
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            if ( foundRecord->m_UpdateOnline != updateOnlinePossible )
            {
                foundRecord->m_UpdateOnline = updateOnlinePossible;
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_Real),TypeName(foundRecord->m_Type));
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::SetValue(Name name, const String & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,SAPDBErr_MessageList & err)
{
    SAPDB_Bool nameTooLong,valueTooLong;
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL == foundRecord)
    {
        Name copyOfName = RTEConf_MakeACopyOfString( name );

        String copyOfValue = RTEConf_MakeACopyOfString( value );

        InsertRecord(copyOfName,NULL/*paramClass*/,updateOnlinePossible,permanentChange,t_String,(SAPDB_Byte *)copyOfValue,nameTooLong,valueTooLong);
        if(nameTooLong || valueTooLong)
        {
            DEALLOCATE_MEMORY(copyOfName);
            DEALLOCATE_MEMORY(copyOfValue);
            if(nameTooLong)
            {
                err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_PARAMETER_NAME_TOO_LONG,(SAPDB_Char *)name,SAPDB_ToString(m_maxLengthOfName));
                return false;
            }
            if(valueTooLong)
            {
                err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_STRING_VALUE_TOO_LONG,(SAPDB_Char *)value,SAPDB_ToString(m_maxLengthOfStringValue));
                return false;
            }
        }

        if(!m_DirtyFlag)
        {
            m_DirtyFlag = true;
        }
        return true;
    }
    else
    {
        if (t_String == foundRecord->m_Type)
        {
            if (((RTEConf_ParameterRecordString *)foundRecord)->IsDifferent(value))
            {
                String newValue;

                String oldValue;
                oldValue = ((RTEConf_ParameterRecordString *)foundRecord)->Value();
                CheckIfStringValueTooLong(value,valueTooLong);
                if(valueTooLong)
                {
                    err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_STRING_VALUE_TOO_LONG,(SAPDB_Char *)value,SAPDB_ToString(m_maxLengthOfStringValue));
                    return false;
                }
                else
                {
                    newValue = RTEConf_MakeACopyOfString( value );
                    ((RTEConf_ParameterRecordString *)foundRecord)->Assign(newValue);
                    DEALLOCATE_MEMORY ( oldValue ); // TODO: remove this, accepting a memory leak but allowing parallel access??
                }
                foundRecord->m_PermanentChange = permanentChange;
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            if ( foundRecord->m_UpdateOnline != updateOnlinePossible )
            {
                foundRecord->m_UpdateOnline = updateOnlinePossible;
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_String),TypeName(foundRecord->m_Type));
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::SetValue(Name name, const CryptInfo & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL == foundRecord)
    {
        Name copyOfName = RTEConf_MakeACopyOfString( name );
        SAPDB_Int4 *CopyOfValue = ALLOCATE_ARRAY ( SAPDB_Int4 , RTECONF_CRYPT_ARRAY_SIZE );
        SAPDB_MemCopyNoCheck((void *)CopyOfValue,(void *)&value,sizeof(CryptInfo));

        SAPDB_Bool nameTooLong,valueTooLong;
        InsertRecord(copyOfName,NULL/*paramClass*/,updateOnlinePossible,permanentChange,t_CryptInfo,(SAPDB_Byte *)CopyOfValue,nameTooLong,valueTooLong);
        if(!m_DirtyFlag)
        {
            m_DirtyFlag = true;
        }
        return true;
    }
    else
    {
        if (t_CryptInfo == foundRecord->m_Type)
        {
            if (((RTEConf_ParameterRecordCryptInfo *)foundRecord)->IsDifferent(&value))
            {
                SAPDB_MemCopyNoCheck(((RTEConf_ParameterRecordCryptInfo *)foundRecord)->Value(),value,sizeof(CryptInfo));
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            if ( foundRecord->m_UpdateOnline != updateOnlinePossible )
            {
                foundRecord->m_UpdateOnline = updateOnlinePossible;
                foundRecord->m_PermanentChange = permanentChange;
                if(!m_DirtyFlag)
                {
                    m_DirtyFlag = true;
                }
            }
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_WRONG_TYPE,(SAPDB_Char *)name,TypeName(t_CryptInfo),TypeName(foundRecord->m_Type));
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::SetUpdateOnline(Name name, SAPDB_Bool updateOnlinePossible,   SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL == foundRecord)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
        return false;
    }
    else
    {
        foundRecord->m_UpdateOnline = updateOnlinePossible;
        return true;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::UpdateOnlinePossible(Name name)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        return foundRecord->m_UpdateOnline;
    }
    else
    {
        return true;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::SetPermanentChange(Name name, SAPDB_Bool permanentChange,   SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL == foundRecord)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
        return false;
    }
    else
    {
        foundRecord->m_PermanentChange = permanentChange;
        return true;
    }
}

/*---------------------------------------------------------------------------*/

RTEConf_Parameter::UpdateFuncType RTEConf_Parameter::GetUpdateFunc(Name name,void *&context)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL != foundRecord)
    {
        context =   foundRecord->m_UpdateContext;
        return      foundRecord->m_UpdateFunc;
    }
    else
    {
        return NULL;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::RegisterUpdateFunction(Name name, UpdateFuncType updateFunc,  void *context, SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *foundRecord = LookupRecordByName(name);
    if(NULL == foundRecord)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
        return false;
    }
    else
    {
        foundRecord->m_UpdateFunc = updateFunc;
        foundRecord->m_UpdateContext = context;
        return true;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::Delete(Name name, SAPDBErr_MessageList & err)
{
    RTEConf_ParameterRecord *currentRecord;
    RTEConf_ParameterRecord *foundRecord = NULL;
    SAPDB_Int4 indexOfList;
    indexOfList = CalcIndex((SAPDB_Char *)name);
    currentRecord = m_ParameterRecords[indexOfList];
    if(NULL != currentRecord)
    {
        if(0 == STRCMP_UTF8((currentRecord->m_Name),name))
        {
            foundRecord = currentRecord;
            m_ParameterRecords[indexOfList] = currentRecord->m_Next;
        }
        else
        {
            RTEConf_ParameterRecord *previousRecord;

            do
            {
                previousRecord = currentRecord;
                currentRecord = currentRecord->m_Next;
            }
            while(NULL != currentRecord && (0 != STRCMP_UTF8((currentRecord->m_Name),name)));
            if(NULL != currentRecord)
            {
                foundRecord = currentRecord;
                previousRecord->m_Next = foundRecord->m_Next;
            }
        }
    }
    if(NULL != foundRecord)
    {
        if(!m_DirtyFlag)
        {
            m_DirtyFlag = true;
        }
        delete foundRecord;
        return true;
    }
    else
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_NOT_FOUND,(SAPDB_Char *)name);
        return false;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::GetLastVerifyTime(TimeStamp & timeStamp)
{
    timeStamp = m_VerifyTimeStamp;
    return ( (m_VerifyTimeStamp.Year         != 0) ||
             (m_VerifyTimeStamp.Month        != 0) ||
             (m_VerifyTimeStamp.DayOfWeek    != 0) ||
             (m_VerifyTimeStamp.Day          != 0) ||
             (m_VerifyTimeStamp.Hour         != 0) ||
             (m_VerifyTimeStamp.Minute       != 0) ||
             (m_VerifyTimeStamp.Second       != 0) ||
             (m_VerifyTimeStamp.Milliseconds != 0)    ) && !m_DirtyFlag;
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterIterator RTEConf_Parameter::GetIterator(SAPDB_UTF8 *beginningOfParameterName)
{
    RTEConf_ParameterIterator iterator(*this,beginningOfParameterName);
    return iterator;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::WriteInt2ToFile(SAPDB_Int2 theInt,tRTESys_IOHandle fileHandle,SAPDBErr_MessageList & err)
{
    SAPDB_Byte lsb = theInt & 0xff;
    tRTESys_IOPosition writtenSizeInBytes;
    tRTESys_IOPosition wantedSizeInBytes  = sizeof(SAPDB_Byte);
    tsp00_VfReturn_Param returnValueFromFileAccess;
    RTESys_IOWrite ( fileHandle,&lsb,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    SAPDB_Byte msb = theInt >> 8;
    RTESys_IOWrite ( fileHandle,&msb,wantedSizeInBytes,writtenSizeInBytes,returnValueFromFileAccess );
    if(vf_ok != returnValueFromFileAccess || writtenSizeInBytes != wantedSizeInBytes)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_WRITE_TO_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::MarkAsVerified(SAPDBErr_MessageList & err)
{
    RTE_ISystem::GetLocalDateTime(m_VerifyTimeStamp);

    tRTESys_IOHandle fileHandle;
    tsp00_VfReturn_Param returnValueFromFileAccess;
    SAPDB_Bool result;
    if(!BuildFileNameIfNecessary(err))
        return false;

    RTESys_IOOpen ( fileHandle,m_FileName,RTESys_IOReadWrite,DO_NOT_CREATE_,DEFAULT_SECURITY_RTESYS_IO,returnValueFromFileAccess );
    if(vf_ok != returnValueFromFileAccess)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_OPEN_FILE_FOR_WRITE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        return false;
    }

    if(!MakeSureParamFileIsInNewFormat(fileHandle,err))
    {
        return false;
    }

    result =  true;
    tRTESys_IOPosition newPosition;
    RTESys_IOSeek ( fileHandle,INDEX_OF_VERIFY_TIMESTAMP_,RTESys_IOSeekSet,newPosition,returnValueFromFileAccess );
    if(INDEX_OF_VERIFY_TIMESTAMP_ != newPosition)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_SEEK_IN_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        result = false;
    }
    if(!WriteInt2ToFile(m_VerifyTimeStamp.Year,fileHandle,err)) result = false;
    if(!WriteInt2ToFile(m_VerifyTimeStamp.Month,fileHandle,err)) result = false;
    if(!WriteInt2ToFile(m_VerifyTimeStamp.DayOfWeek,fileHandle,err)) result = false;
    if(!WriteInt2ToFile(m_VerifyTimeStamp.Day,fileHandle,err)) result = false;
    if(!WriteInt2ToFile(m_VerifyTimeStamp.Hour,fileHandle,err)) result = false;
    if(!WriteInt2ToFile(m_VerifyTimeStamp.Minute,fileHandle,err)) result = false;
    if(!WriteInt2ToFile(m_VerifyTimeStamp.Second,fileHandle,err)) result = false;
    if(!WriteInt2ToFile(m_VerifyTimeStamp.Milliseconds,fileHandle,err)) result = false;

    RTESys_IOClose(fileHandle,returnValueFromFileAccess);
    if(vf_ok != returnValueFromFileAccess)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_COULD_NOT_CLOSE_FILE,m_FileName,SAPDB_ToString(LAST_SYSTEM_ERROR_));
        result = false;
    }
    return result;
}


/*---------------------------------------------------------------------------*/

/* as explained, the parameters are not stored in one single lists, but in an array of lists. For a given parameter name,
   this function calculates the list (that is the index to the array of lists) to store or search the parameter in.
   It does so by looking at the character at position KEY_INDEX_ in the parameter name
*/
SAPDB_Int4 RTEConf_Parameter::CalcIndex(SAPDB_Char *parameterName)
{
    SAPDB_Char key;
/*  If the parameter name has less than KEY_INDEX_ characters, store it in list number 0 */
    if(strlen(parameterName) <= KEY_INDEX_)
        return INDEX_OF_LIST_FOR_SHORT_NAMES_;
/*  Otherwise look at the character at position KEY_INDEX_ */
    key = parameterName[KEY_INDEX_];
    if(key >= 'A' && key <= 'Z')
        return INDEX_OF_FIRST_LIST_FOR_CHARS_FROM_A_TO_Z_ + (key-'A');
    if(key >= '0' && key <= '9')
        return INDEX_OF_FIRST_LIST_FOR_CHARS_FROM_0_TO_9_ + (key-'0');
    if(key == '_')
        return INDEX_OF_LIST_FOR_UNDERSCORE_;
/*  If no list could be found for the parameter name, put it in list number 0 */
    return INDEX_OF_LIST_FOR_SHORT_NAMES_;
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::ByteOrderSwapInt(SAPDB_Int4 *theInt4)
{
    SAPDB_Int4 result=0;
    result =  (*theInt4 & 0x000000ff) << 24;
    result += (*theInt4 & 0x0000ff00) << 8;
    result += (*theInt4 & 0x00ff0000) >> 8;
    result += (*theInt4 & 0xff000000) >> 24;
    *theInt4 = result;
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::ByteOrderSwapInt(SAPDB_Int8 *theInt8)
{
    SAPDB_Int8 result=0;
    SAPDB_Int8 mask = 0xff;
    result =  (*theInt8 & mask) << 56;
    mask <<= 8;
    result += (*theInt8 & mask) << 40;
    mask <<= 8;
    result += (*theInt8 & mask) << 24;
    mask <<= 8;
    result += (*theInt8 & mask) <<  8;
    mask <<= 8;
    result += (*theInt8 & mask) >>  8;
    mask <<= 8;
    result += (*theInt8 & mask) >> 24;
    mask <<= 8;
    result += (*theInt8 & mask) >> 40;
    mask <<= 8;
    result += (*theInt8 & mask) >> 56;
    *theInt8 = result;
}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::ByteOrderSwapReal(SAPDB_Real8 *theReal8)
{
    //ByteOrderSwapInt((SAPDB_Int8 *)theReal8);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::m_IntelByteOrder;

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::ReadFromBinaryData ( SAPDB_Byte *recordBuffer,SAPDB_Int4 bytesToRead,SAPDB_Int4 &bytesRead,SAPDB_Bool &returnStatus )
{
    bytesRead = 0;

    SAPDB_Int4 bytesToReadFromCurrentBlock;
    do
    {
        if(m_binaryData.IsEmpty())  // no read possible if there are no blocks anymore...
        {
            return;
        }
        BinaryDataBlock currentDataBlock = *(m_binaryData.Begin());

        // read from the current block as much as there is in it - or as much as desired if there is enough
        bytesToReadFromCurrentBlock = min (bytesToRead - bytesRead , (SAPDB_Int4)(currentDataBlock.length - m_positionInBinaryData));
        SAPDB_MemCopyNoCheck(recordBuffer + bytesRead, currentDataBlock.data + m_positionInBinaryData, bytesToReadFromCurrentBlock);

        bytesRead += bytesToReadFromCurrentBlock;

        if(bytesRead < bytesToRead) // could the desired amount of memory be read?
        {
            DEALLOCATE_MEMORY((*(m_binaryData.Begin())).data);   // No: switch to the next block
            m_binaryData.DeleteFront();
            m_positionInBinaryData = 0;
        }
    }
    while(bytesRead < bytesToRead);
    m_positionInBinaryData += bytesToReadFromCurrentBlock;
    returnStatus = true;
}

void RTEConf_Parameter::ReadOldStyleXParamRecord(Name name,Type & type,SAPDB_Byte *value,tRTESys_IOHandle fileHandle)
{
    SAPDB_Byte recordBuffer[LENGTH_OF_OLDSYTLE_XPARAM_RECORD_];
    SAPDB_Byte *beginningOfRecordBuffer = recordBuffer;
    SAPDB_Byte *currentByte;
    SAPDB_Int4 lengthOfStringValue;
    SAPDB_Int4 receivedSizeInBytes;
    SAPDB_Bool returnStatus;

    memset((SAPDB_Byte *)name,0,m_maxLengthOfName+1);
    memset(value,0,m_maxLengthOfStringValue+1);

    if(NULL == fileHandle)
    {
        ReadFromBinaryData ( recordBuffer,LENGTH_OF_OLDSYTLE_XPARAM_RECORD_,receivedSizeInBytes,returnStatus );
    }
    else
    {
        RTE_FileOffset rsib;
        tsp00_VfReturn_Param returnValueFromRead;
        RTESys_IORead ( fileHandle,recordBuffer,LENGTH_OF_OLDSYTLE_XPARAM_RECORD_,rsib,returnValueFromRead );
        returnStatus = (vf_ok == returnValueFromRead);
        receivedSizeInBytes = (SAPDB_Int4)rsib;
    }

    if(returnStatus && LENGTH_OF_OLDSYTLE_XPARAM_RECORD_ == receivedSizeInBytes)
    {
        currentByte = beginningOfRecordBuffer+LENGTH_OF_IDENTIFIER_-1;
        while(*currentByte == ' ')
            currentByte--;
        lengthOfStringValue = (SAPDB_Int4)(currentByte+1-recordBuffer);

        SAPDB_MemCopyNoCheck((SAPDB_Byte *)name,beginningOfRecordBuffer,lengthOfStringValue);

        if(0 == STRCMP_UTF8(name,NEWSTYLE_HEADER_IDENTIFIER_))
        {
            SAPDB_Int4 lengthOfHeaderInformation
                = (SAPDB_Int4)(strlen(NEWSTYLE_HEADER_IDENTIFIER_V03) - strlen((SAPDB_Char *)name))
                + LENGTH_OF_VERIFY_TIMESTAMP_
                + LENGTH_OF_CRASH_FLAG_
                + LENGTH_OF_NAME_LENGTH_
                + LENGTH_OF_PARAM_LENGTH_
                + LENGTH_OF_CHECKSUM_;
            SAPDB_MemCopyNoCheck(value,beginningOfRecordBuffer + LENGTH_OF_IDENTIFIER_,lengthOfHeaderInformation);
        }
        else
        {
            SAPDB_Byte typeCodeFromFile = beginningOfRecordBuffer[OFFSET_OF_OLDSTYLE_TYPE_];
            switch(typeCodeFromFile)
            {
            case OLDSTYLE_CODE_FOR_INT2_:
                type = RTEConf_Parameter::t_Integer;
                memset(value,0,sizeof(Integer));
                SAPDB_MemCopyNoCheck(value,beginningOfRecordBuffer+OFFSET_OF_OLDSTYLE_VALUE_,2);
                if(!RTEConf_Parameter::m_IntelByteOrder)
                {
                    *((Integer *)value) = (Integer)(*((SAPDB_Int2 *)value));
                }
                break;
            case OLDSTYLE_CODE_FOR_INT4_:
                type = RTEConf_Parameter::t_Integer;
                memset(value,0,sizeof(Integer));
                SAPDB_MemCopyNoCheck(value,beginningOfRecordBuffer+OFFSET_OF_OLDSTYLE_VALUE_,4);
                if(!RTEConf_Parameter::m_IntelByteOrder)
                {
                    *((Integer *)value) = (Integer)(*((SAPDB_Int4 *)value));
                }
                break;
            case OLDSTYLE_CODE_FOR_C8_:
            case OLDSTYLE_CODE_FOR_C18_:
            case OLDSTYLE_CODE_FOR_C24_:
            case OLDSTYLE_CODE_FOR_C40_:
            case OLDSTYLE_CODE_FOR_C64_:
                type = RTEConf_Parameter::t_String;
                currentByte = beginningOfRecordBuffer+OFFSET_OF_OLDSTYLE_VALUE_+LENGTH_OF_OLDSTYLE_VALUE_-1;
    #ifdef HPUX
                if(*currentByte == 0)
                    currentByte--;
    #endif
                while(*currentByte == ' ')
                    currentByte--;
                lengthOfStringValue = (SAPDB_Int4)(currentByte+1-(beginningOfRecordBuffer+OFFSET_OF_OLDSTYLE_VALUE_));

                SAPDB_MemCopyNoCheck(value,beginningOfRecordBuffer+OFFSET_OF_OLDSTYLE_VALUE_,lengthOfStringValue);
                value[lengthOfStringValue]=0;
                break;
            case OLDSTYLE_CODE_FOR_CRYPTINFO_:
                type = RTEConf_Parameter::t_CryptInfo;
                SAPDB_MemCopyNoCheck(value,beginningOfRecordBuffer+OFFSET_OF_OLDSTYLE_VALUE_,sizeof(CryptInfo));

                break;
            default:
                type = RTEConf_Parameter::t_Invalid;
                break;
            }
        }
    }
    else
    {
        value = 0;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEConf_Parameter::ReadNewStyleXParamRecord(SAPDB_Int4 version,Name name,SAPDB_Bool &updateOnlinePossible,SAPDB_Byte &paramClassIndex,Type & type,SAPDB_Byte *value,SAPDB_Bool *endReachedCorrectly,tRTESys_IOHandle fileHandle)
{
    *endReachedCorrectly = false;
    SAPDB_Int4 receivedSizeInBytes;
    SAPDB_Bool returnStatus;

    memset((SAPDB_Byte *)name,0,m_maxLengthOfName+1);
    memset(value,0,m_maxLengthOfStringValue+1);

    if(NULL == fileHandle)
    {
        ReadStringFromBinaryData(name,m_maxLengthOfName,endReachedCorrectly);
    }
    else
    {
        ReadStringFromFile(fileHandle,name,m_maxLengthOfName,endReachedCorrectly);
    }

    if(*endReachedCorrectly)
        return true;

    SAPDB_Byte currentChar;

    if( version >= 4 )  /* including parameter class */
    {
        if(NULL == fileHandle)
        {
            ReadFromBinaryData ( &paramClassIndex,sizeof(SAPDB_Byte),receivedSizeInBytes,returnStatus );
        }
        else
        {
            RTE_FileOffset rsib;
            tsp00_VfReturn_Param returnValueFromRead;
            RTESys_IORead ( fileHandle,&paramClassIndex,sizeof(SAPDB_Byte),rsib,returnValueFromRead );
            returnStatus = (vf_ok == returnValueFromRead);
            receivedSizeInBytes = (SAPDB_Int4)rsib;
        }
    }

    if(version >= 3)    /* including update online flag */
    {
        if(NULL == fileHandle)
        {
            ReadFromBinaryData ( &currentChar,sizeof(SAPDB_Byte),receivedSizeInBytes,returnStatus );
        }
        else
        {
            RTE_FileOffset rsib;
            tsp00_VfReturn_Param returnValueFromRead;
            RTESys_IORead ( fileHandle,&currentChar,sizeof(SAPDB_Byte),rsib,returnValueFromRead );
            returnStatus = (vf_ok == returnValueFromRead);
            receivedSizeInBytes = (SAPDB_Int4)rsib;
        }

        if(returnStatus && sizeof(SAPDB_Byte) == receivedSizeInBytes)
        {
            if(BYTECODE_FOR_FALSE_ == currentChar)
            {
                updateOnlinePossible = false;
            }
            else
            {
                updateOnlinePossible = true;
            }
        }
    }

    if(NULL == fileHandle)
    {
        ReadFromBinaryData ( &currentChar,sizeof(SAPDB_Byte),receivedSizeInBytes,returnStatus );
    }
    else
    {
        RTE_FileOffset rsib;
        tsp00_VfReturn_Param returnValueFromRead;
        RTESys_IORead ( fileHandle,&currentChar,sizeof(SAPDB_Byte),rsib,returnValueFromRead );
        returnStatus = (vf_ok == returnValueFromRead);
        receivedSizeInBytes = (SAPDB_Int4)rsib;
    }

    if(returnStatus && sizeof(SAPDB_Byte) == receivedSizeInBytes)
    {
        switch(currentChar)
        {
        case RTECONF_BYTECODE_FOR_INTEGER_:
            type = t_Integer;
            if(NULL == fileHandle)
            {
                ReadFromBinaryData ( value,sizeof(Integer),receivedSizeInBytes,returnStatus );
            }
            else
            {
                RTE_FileOffset rsib;
                tsp00_VfReturn_Param returnValueFromRead;
                RTESys_IORead ( fileHandle,value,sizeof(Integer),rsib,returnValueFromRead );
                returnStatus = (vf_ok == returnValueFromRead);
                receivedSizeInBytes = (SAPDB_Int4)rsib;
            }

            if(!returnStatus || sizeof(Integer) != receivedSizeInBytes)
            {
                return false;
            }
            if(!m_IntelByteOrder)
            {
                //ByteOrderSwapInt((Integer *)*value);
            }
            break;
        case RTECONF_BYTECODE_FOR_REAL_:
            type = t_Real;
            if(NULL == fileHandle)
            {
                ReadFromBinaryData ( value,sizeof(Real),receivedSizeInBytes,returnStatus );
            }
            else
            {
                RTE_FileOffset rsib;
                tsp00_VfReturn_Param returnValueFromRead;
                RTESys_IORead ( fileHandle,value,sizeof(Real),rsib,returnValueFromRead );
                returnStatus = (vf_ok == returnValueFromRead);
                receivedSizeInBytes = (SAPDB_Int4)rsib;
            }

            if(!returnStatus || sizeof(Real) != receivedSizeInBytes)
            {
                return false;
            }
            if(!m_IntelByteOrder)
            {
                ByteOrderSwapReal((Real *)*value);
            }
            break;
        case RTECONF_BYTECODE_FOR_STRING_:

            SAPDB_Bool endMarkerRead;
            if(NULL == fileHandle)
            {
                ReadStringFromBinaryData((String)value,m_maxLengthOfStringValue,&endMarkerRead);
            }
            else
            {
                ReadStringFromFile(fileHandle,(String)value,m_maxLengthOfStringValue,&endMarkerRead);
            }

            type = t_String;
            break;
        case RTECONF_BYTECODE_FOR_CRYPTINFO_:
            if(NULL == fileHandle)
            {
                ReadFromBinaryData ( value,sizeof(CryptInfo),receivedSizeInBytes,returnStatus );
            }
            else
            {
                RTE_FileOffset rsib;
                tsp00_VfReturn_Param returnValueFromRead;
                RTESys_IORead ( fileHandle,value,sizeof(CryptInfo),rsib,returnValueFromRead );
                returnStatus = (vf_ok == returnValueFromRead);
                receivedSizeInBytes = (SAPDB_Int4)rsib;
            }

            if(!returnStatus || sizeof(CryptInfo) != receivedSizeInBytes)
            {
                return false;
            }
            if(!m_IntelByteOrder)
            {
                for(SAPDB_Int4 counter = 0;counter < RTECONF_CRYPT_ARRAY_SIZE;counter++)
                {
                    //ByteOrderSwapInt(&(((SAPDB_Int4 *)(*value))[counter]));
                }
            }
            type = t_CryptInfo;
            break;
        case RTECONF_BYTECODE_FOR_PARAMCLASS_:
/*
            if(NULL == fileHandle)
            {
                ReadFromBinaryData ( value,sizeof(SAPDB_Byte),receivedSizeInBytes,returnStatus );
            }
            else
            {
                RTE_FileOffset rsib;
                tsp00_VfReturn_Param returnValueFromRead;
                RTESys_IORead ( fileHandle,value,sizeof(SAPDB_Byte),rsib,returnValueFromRead );
                returnStatus = (vf_ok == returnValueFromRead);
                receivedSizeInBytes = (SAPDB_Int4)rsib;
            }
*/
            type = t_ParamClass;
            break;
        default:
            type = t_Invalid;
            break;
        }
    }
    return true;
}

void RTEConf_Parameter::ReadStringFromBinaryData(String string,SAPDB_UInt4 maxLength,SAPDB_Bool *endMarkerRead)
{
    SAPDB_Int4 byteCounter;
    SAPDB_Int4 buffersFilled=0;
    SAPDB_UTF8 currentChar;
    SAPDB_Int4 receivedSizeInBytes;
    SAPDB_Bool returnStatus;

    SAPDB_Byte *target = (SAPDB_Byte *)string;

    *endMarkerRead = false;

    ReadFromBinaryData ( &currentChar,sizeof(SAPDB_UTF8),receivedSizeInBytes,returnStatus );
    if(!returnStatus || sizeof(SAPDB_UTF8) != receivedSizeInBytes)
    {
        return;
    }
    /* the end marker is a special charachter that must not appear in a string. If the first character read is  */
    /* the end marker, the end of the file has been reached correctly                                           */
    if(END_MARKER_ == currentChar)
    {
        *endMarkerRead = true;
        return;
    }
    target[0] = currentChar;
    byteCounter = 1;

    while(currentChar != 0)
    {
        ReadFromBinaryData ( &currentChar,sizeof(SAPDB_UTF8),receivedSizeInBytes,returnStatus );
        if(!returnStatus || sizeof(SAPDB_UTF8) != receivedSizeInBytes)
        {
            return;
        }
        target[byteCounter] = currentChar;
        byteCounter++;
    }

}

/*---------------------------------------------------------------------------*/

void RTEConf_Parameter::ReadStringFromFile(tRTESys_IOHandle fileHandle,String string,SAPDB_UInt4 maxLength,SAPDB_Bool *endMarkerRead)
{
    SAPDB_UInt4 byteCounter;
    SAPDB_Int4 buffersFilled=0;
    SAPDB_UTF8 currentChar;
    SAPDB_Int4 receivedSizeInBytes;
    SAPDB_Bool returnStatus;

    SAPDB_Byte *target = (SAPDB_Byte *)string;

    *endMarkerRead = false;

    RTE_FileOffset rsib;
    tsp00_VfReturn_Param returnValueFromRead;
    RTESys_IORead ( fileHandle,&currentChar,sizeof(SAPDB_UTF8),rsib,returnValueFromRead );
    returnStatus = (vf_ok == returnValueFromRead);
    receivedSizeInBytes = (SAPDB_Int4)rsib;

    if(!returnStatus || sizeof(SAPDB_UTF8) != receivedSizeInBytes)
    {
        return;
    }
    /* the end marker is a special charachter that must not appear in a string. If the first character read is  */
    /* the end marker, the end of the file has been reached correctly                                           */
    if(END_MARKER_ == currentChar)
    {
        *endMarkerRead = true;
        return;
    }

    target[0] = currentChar;
    byteCounter = 1;
    while(currentChar != 0 && byteCounter <= maxLength)
    {
        RTESys_IORead ( fileHandle,&currentChar,sizeof(SAPDB_UTF8),rsib,returnValueFromRead );
        returnStatus = (vf_ok == returnValueFromRead);
        receivedSizeInBytes = (SAPDB_Int4)rsib;
        if(!returnStatus || sizeof(SAPDB_UTF8) != receivedSizeInBytes)
        {
            return;
        }
        target[byteCounter] = currentChar;
        byteCounter++;
    }
}

SAPDB_ToStringClass
SAPDB_ToString( RTEConf_Parameter::Type type)
{ 
    switch(type)
    {
    case RTEConf_Parameter::t_Integer:
        return SAPDB_ToString("Integer");
    case RTEConf_Parameter::t_String:
        return SAPDB_ToString("String");
    case RTEConf_Parameter::t_CryptInfo:
        return SAPDB_ToString("Crypted Password");
    case RTEConf_Parameter::t_Real:
        return SAPDB_ToString("Real");
    default:
        return SAPDB_ToString("Unknown");
    }
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                      class RTEConf_ParameterIterator                      */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

SAPDB_Bool RTEConf_ParameterIterator::Next(RTEConf_Parameter::Name & name, RTEConf_Parameter::Type & type, SAPDB_Byte & paramClassIndex, SAPDB_Bool &endOfList, SAPDBErr_MessageList & err )
{
    if(NULL == m_LastVisitedRecord)
    {
        m_LastVisitedRecord = m_FirstRecord;
    }
    else
    {
        m_LastVisitedRecord = m_LastVisitedRecord->m_Next;
    }
    if(NULL == m_LastVisitedRecord)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEINFO_CONF_NO_MORE_PARAMETERS);
        return false;
    }
    else
    {
        name            = m_LastVisitedRecord->m_Name;
        type            = m_LastVisitedRecord->m_Type;
        paramClassIndex = m_LastVisitedRecord->m_ClassIndex;
        endOfList       = (NULL == m_LastVisitedRecord->m_Next);
        return true;
    }
}

/*---------------------------------------------------------------------------*/

void RTEConf_ParameterIterator::IterateThroughOneList(RTEConf_Parameter &param,SAPDB_Int4 listToIterate,SAPDB_UTF8 *beginningOfParameterName,SAPDB_Int4 lengthToCompare)
{
    if(NULL != param.m_ParameterRecords[listToIterate])
    {
        RTEConf_ParameterRecord *currRecord = param.m_ParameterRecords[listToIterate];
        while(NULL != currRecord)
        {
            if((NULL == beginningOfParameterName)||(0 == strncmp((SAPDB_Char *)(currRecord->m_Name),(SAPDB_Char *)beginningOfParameterName,lengthToCompare)))
            {
                RTEConf_ParameterRecord *newRecord = NULL;
                switch(currRecord->m_Type)
                {
                case RTEConf_Parameter::t_Integer:
                    newRecord = new RTEConf_ParameterRecordInteger;
                    break;
                case RTEConf_Parameter::t_Real:
                    newRecord = new RTEConf_ParameterRecordReal;
                    break;
                case RTEConf_Parameter::t_String:
                    newRecord = new RTEConf_ParameterRecordString;
                    break;
                case RTEConf_Parameter::t_CryptInfo:
                    newRecord = new RTEConf_ParameterRecordCryptInfo;
                    break;
                }
                if(NULL != newRecord)
                {
                    *newRecord = *currRecord;
                    newRecord->m_Next = m_FirstRecord;
                    m_FirstRecord = newRecord;
                }
            }
            currRecord = currRecord->m_Next;
        }
    }
}

RTEConf_ParameterIterator::RTEConf_ParameterIterator(RTEConf_Parameter &param,SAPDB_UTF8 *beginningOfParameterName)
{
    m_FirstRecord = NULL;
    SAPDB_Int4 currentList = 0; // only for -Wall
    SAPDB_Int4 lengthToCompare = 0;
    SAPDB_Bool searchInOneListOnly = false;

    if(beginningOfParameterName)
    {
        lengthToCompare = (SAPDB_Int4)strlen((SAPDB_Char *)(beginningOfParameterName));
        currentList = param.CalcIndex((SAPDB_Char *)beginningOfParameterName);
        if(INDEX_OF_LIST_FOR_SHORT_NAMES_ != currentList)
        {
            searchInOneListOnly = true;
        }
    }

    if(searchInOneListOnly)
    {
        IterateThroughOneList(param,currentList,beginningOfParameterName,lengthToCompare);
    }
    else
    {
        for(currentList = 0 ; currentList < TOTAL_NUMBER_OF_LISTS_ ; currentList++)
        {
           IterateThroughOneList(param,currentList,beginningOfParameterName,lengthToCompare);
        }
    }
    m_LastVisitedRecord = NULL;
}

/*---------------------------------------------------------------------------*/

void RTEConf_ParameterIterator::DeleteIteratorRecords()
{
    RTEConf_ParameterRecord *nextRecord;
    RTEConf_ParameterRecord *currentRecord;
    if(m_FirstRecord)
    {
        currentRecord = m_FirstRecord;
        do
        {
            nextRecord = currentRecord->m_Next;
            delete currentRecord;
            currentRecord = nextRecord;
        }
        while(NULL != currentRecord);
    }
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterIterator::~RTEConf_ParameterIterator()
{
    DeleteIteratorRecords();
}

/*---------------------------------------------------------------------------*/

void RTEConf_ParameterIterator::CopyIteratorRecords(const RTEConf_ParameterIterator &templateIterator)
{
    RTEConf_ParameterRecord *currentRecord;
    RTEConf_ParameterRecord *previousRecord=NULL;
    RTEConf_ParameterRecord *newRecord;

    m_FirstRecord = NULL;
    currentRecord = templateIterator.m_FirstRecord;
    while(NULL != currentRecord)
    {
        newRecord = NULL;

        switch(currentRecord->m_Type)
        {
        case RTEConf_Parameter::t_Integer:
            newRecord = new RTEConf_ParameterRecordInteger;
            break;
        case RTEConf_Parameter::t_Real:
            newRecord = new RTEConf_ParameterRecordReal;
            break;
        case RTEConf_Parameter::t_String:
            newRecord = new RTEConf_ParameterRecordString;
            break;
        case RTEConf_Parameter::t_CryptInfo:
            newRecord = new RTEConf_ParameterRecordCryptInfo;
            break;
        }
        if(NULL != newRecord)
        {
            newRecord->m_Name = RTEConf_MakeACopyOfString( currentRecord->m_Name );
        }
        currentRecord = currentRecord->m_Next;

        if(NULL == previousRecord)
        {
            m_FirstRecord = newRecord;
            previousRecord = m_FirstRecord;
        }
        else
        {
            previousRecord->m_Next = newRecord;
            previousRecord = previousRecord->m_Next;
        }
    }
    m_LastVisitedRecord = NULL;
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterIterator::RTEConf_ParameterIterator(const RTEConf_ParameterIterator &templateIterator)
{
    CopyIteratorRecords(templateIterator);
}

/*---------------------------------------------------------------------------*/

RTEConf_ParameterIterator & RTEConf_ParameterIterator::operator=(const RTEConf_ParameterIterator &templateIterator)
{
    DeleteIteratorRecords();
    CopyIteratorRecords(templateIterator);
    return (RTEConf_ParameterIterator&)templateIterator;
}

/*---------------------------------------------------------------------------*/

void *RTEConf_ParameterIterator::operator new(size_t size)
{
    return AllocateMemory(size);
}

/*---------------------------------------------------------------------------*/

void RTEConf_ParameterIterator::operator delete(void *address)
{
    DEALLOCATE_MEMORY(address);
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool isRelativePath(RTEConf_Parameter::String path)
{
#ifdef _WIN32
    return path[0]!='\\' && (path[1]!=':' || path[2]!='\\');
#else
    return path[0]!='/';
#endif
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

