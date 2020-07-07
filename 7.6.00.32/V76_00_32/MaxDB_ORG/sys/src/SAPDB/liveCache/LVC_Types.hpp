/*!**************************************************************************

  module      : LVC_Types.hpp

  -------------------------------------------------------------------------

  responsible : d025251

  special area: 
  description : 


  last changed: 2001-05-21  16:24
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef LVC_TYPES_HPP
#define LVC_TYPES_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "gbd900.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define LVC_ROOT_ARRAY_SIZE (sizeof(Data_PageNo) * MAX_NUM_CONT_OBJ_FILES_BD900)
#define LVC_FILENO_ARRAY_SIZE (sizeof(FileDir_FileNo) * NUM_KEY_PARTITIONS_BD900)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!-----------------------------------------------------------------------------
declaration: SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( LVC )
description: defines common exception class of this component
             
             - class LVC_Exception
------------------------------------------------------------------------------*/
SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( LVC );

class LVC_RootArray
{
private:
    Data_PageNo    m_Root [MAX_NUM_CONT_OBJ_FILES_BD900];
    SAPDB_UInt     m_ActArrSize;
public:
	
    /*!-------------------------------------------------------------------------
    function:     LVC_RootArray()
    description:  default constructor
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
	LVC_RootArray ()
	{
        m_ActArrSize = 0;
	}
	
    /*!-------------------------------------------------------------------------
    function:     Fill)
    description:  used by redo
    arguments:    image  [in]
                  length [in]
    return value: none
    --------------------------------------------------------------------------*/
    void Fill (const SAPDB_Byte* image,
               SAPDB_UInt        length)
	{
        memcpy (m_Root, image, length);
        m_ActArrSize = length / sizeof(Data_PageNo);
		SAPDBERR_ASSERT_STATE(m_ActArrSize <= MAX_NUM_CONT_OBJ_FILES_BD900);
	}
	
	/*!-------------------------------------------------------------------------
    function:     operator[]()
    description:  access operator
    arguments:    i : index
    return value: page number stored at the wanted index position
    --------------------------------------------------------------------------*/
	Data_PageNo & operator[] (SAPDB_UInt i)
	{
		SAPDBERR_ASSERT_STATE((i + 1)<= MAX_NUM_CONT_OBJ_FILES_BD900);
        if ( (i + 1) > m_ActArrSize ) {
            for (SAPDB_UInt ix=i; ix < i+1; ix++ )
                m_Root[i] = NIL_PAGE_NO_GG00;
            m_ActArrSize = i + 1;
        }
        return m_Root[i];
	}

	/*!-------------------------------------------------------------------------
    function:     const operator[]()
    description:  access operator
    arguments:    i : index
    return value: page number stored at the wanted index position
    --------------------------------------------------------------------------*/
	const Data_PageNo & operator[] (SAPDB_UInt i) const
	{
		SAPDBERR_ASSERT_STATE((i + 1)<= MAX_NUM_CONT_OBJ_FILES_BD900);
		return m_Root[i];
	}

	/*!-------------------------------------------------------------------------
    function:     GetNumUsedElements()
    description:  this is used by the logging to write the info to the log
    arguments:    none
    return value: number of used array elements
    --------------------------------------------------------------------------*/
	SAPDB_UInt GetNumUsedElements() const
	{
		return m_ActArrSize;
	}
	/*!-------------------------------------------------------------------------
    function:     GetPersistentLength()
    description:  this is used by the logging to write the info to the log
    arguments:    none
    return value: length
    --------------------------------------------------------------------------*/
	SAPDB_UInt GetPersistentLength() const
	{
		return m_ActArrSize * sizeof(Data_PageNo);
	}
	/*!-------------------------------------------------------------------------
    function:     GetPersistentFormat()
    description:  this is used by the logging to write the info to the log
    arguments:    none
    return value: const SAPDB_Byte*
    --------------------------------------------------------------------------*/
	const SAPDB_Byte* GetPersistentFormat() const
	{
		return reinterpret_cast<const SAPDB_Byte*> (m_Root);
	}

};

class LVC_FileNoArray
{
private:
    FileDir_FileNo m_FileNo [NUM_KEY_PARTITIONS_BD900];
    SAPDB_UInt     m_ActArrSize;
public:
	
    /*!-------------------------------------------------------------------------
    function:     LVC_FileNoArray()
    description:  default constructor
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
	LVC_FileNoArray ()
	{
        m_ActArrSize = 0;
	}
	
    /*!-------------------------------------------------------------------------
    function:     Fill()
    description:  used by redo
    arguments:    image  [in]
                  length [in]
    return value: none
    --------------------------------------------------------------------------*/
    void Fill (const SAPDB_Byte* image,
               SAPDB_UInt        length)
	{
        memcpy (m_FileNo, image, length);
        m_ActArrSize = length / sizeof(FileDir_FileNo);
		SAPDBERR_ASSERT_STATE(m_ActArrSize <= NUM_KEY_PARTITIONS_BD900);
	}
	
	/*!-------------------------------------------------------------------------
    function:     operator[]()
    description:  access operator
    arguments:    i : index
    return value: page number stored at the wanted index position
    --------------------------------------------------------------------------*/
	FileDir_FileNo & operator[] (SAPDB_UInt i)
	{
		SAPDBERR_ASSERT_STATE((i + 1)<= NUM_KEY_PARTITIONS_BD900);
        if ( (i + 1) > m_ActArrSize ) {
            FileDir_FileNo nullFileNo;
            for (SAPDB_UInt ix=i; ix < i+1; ix++ )
                m_FileNo[i] = nullFileNo;
            m_ActArrSize = i + 1;
        }
        return m_FileNo[i];
	}

	/*!-------------------------------------------------------------------------
    function:     const operator[]()
    description:  access operator
    arguments:    i : index
    return value: page number stored at the wanted index position
    --------------------------------------------------------------------------*/
	const FileDir_FileNo & operator[] (SAPDB_UInt i) const
	{
		SAPDBERR_ASSERT_STATE((i + 1)<= NUM_KEY_PARTITIONS_BD900);
		return m_FileNo[i];
	}

	/*!-------------------------------------------------------------------------
    function:     GetNumUsedElements()
    description:  this is used by the logging to write the info to the log
    arguments:    none
    return value: number of used array elements
    --------------------------------------------------------------------------*/
	SAPDB_UInt GetNumUsedElements() const
	{
		return m_ActArrSize;
	}
	/*!-------------------------------------------------------------------------
    function:     GetPersistentLength()
    description:  this is used by the logging to write the info to the log
    arguments:    none
    return value: length
    --------------------------------------------------------------------------*/
	SAPDB_UInt GetPersistentLength() const
	{
		return m_ActArrSize * sizeof(FileDir_FileNo);
	}
	/*!-------------------------------------------------------------------------
    function:     GetPersistentFormat()
    description:  this is used by the logging to write the info to the log
    arguments:    none
    return value: const SAPDB_Byte*
    --------------------------------------------------------------------------*/
	const SAPDB_Byte* GetPersistentFormat() const
	{
		return reinterpret_cast<const SAPDB_Byte*> (m_FileNo);
	}

};


#endif  /* LVC_TYPES_HPP */
