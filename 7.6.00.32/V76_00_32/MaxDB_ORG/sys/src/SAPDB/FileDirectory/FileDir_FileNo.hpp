/*****************************************************************************/
/*!
  @file         FileDir_FileNo.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_FileNo.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#ifndef FILEDIR_FILENO_HPP
#define FILEDIR_FILENO_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg90.h"
#include "ggg00.h"
#include "hsp77.h" // sp77encodingUCS2Native sp77encodingUCS2Swapped
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

enum {
    /* changing c_tempFileNoMaskBytePos requires load_systab for
     * existing databases, as the catalog surrogates for the shared
     * temp-tables still contain the old temp-marker position:
     */
    c_tempFileNoMaskBytePos = 0
};

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class FileDir_SharedDirectory;
class SAPDB_OStream;

/*!
 * @class FileDir_FileNo
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief This class represents the unique data access file identifiers.
 */
class FileDir_FileNo
{
private:
    union {
        tgg90_Cint<8> no;
        SAPDB_UInt8   intValue;
        char          charArray[8];
    } m_fileNo;

public:
    enum {
        c_hexBufLength = 16 ///< Length of hexdump string
                            ///  represenation of FileDir_FileNo
    };

    /// Default constructor for an invalid fileNo.
    FileDir_FileNo();

    /*!
     * @brief Copy-constructor.
     */
    FileDir_FileNo(const FileDir_FileNo& fileNo);

    /// Constructs a fileNo from an int8 value.
    FileDir_FileNo(const SAPDB_UInt8 fileNo);

    /// Constructs a fileNo from a tgg00_Surrogate.
    FileDir_FileNo(const tgg00_Surrogate& surrogate);

    /// Assigns a tgg00_Surrogate value to this fileNo.
    FileDir_FileNo& operator=(const tgg00_Surrogate& surrogate);

    /// Assignment operator.
    FileDir_FileNo& operator=(const FileDir_FileNo& fileNo);

    /// Equality operator.
    SAPDB_Bool operator==(const FileDir_FileNo& fileNo) const;

    /// Equality operator with input represented by surrogate.
    SAPDB_Bool operator==(const tgg00_Surrogate& surrogate) const;

    /// Inequality operator.
    SAPDB_Bool operator!=(const FileDir_FileNo& fileNo) const;

    /// Inequality operator with input represented by surrogate.
    SAPDB_Bool operator!=(const tgg00_Surrogate& surrogate) const;

    /*!
     * @brief Check if fileNo is valid.
     * @return true if this fileNo is valid.
     */
    SAPDB_Bool IsValid() const;

    /*!
     * @brief Check if fileNo is invalid.
     * @return true if this fileNo is invalid.
     */
    SAPDB_Bool IsInvalid() const;


    /*!
     * @brief Invalidate this fileNo.
     */
    void Invalidate();

    /*!
     * @brief Check if fileNo represents a temporary file.
     * @return true, if fileNo represents a temporary file
     */
    SAPDB_Bool IsTempFileNo() const;

    /*!
     * @brief Check if fileNo represents a persistent file.
     * @return true, if fileNo represents a persistent file
     */
    SAPDB_Bool IsPersistentFileNo() const;

    /*!
     * @brief Return a UInt8 representation of this fileNo. Take the
     * byte-swap of the cpu architecture in account so that any fileNo
     * has the same UInt8 representation regardless of the platform
     * this is running on.
     *
     * @return true, if fileNo represents a persistent file
     */
    SAPDB_UInt8 GetUInt8() const;

    /*!
     * @brief Return a UInt8 representation of this fileNo but do not
     * take byte-swap of cpu architecture into account.
     * @sa FileDir_FileNo::GetUInt8
     *
     * @return UInt8 representation of this fileNo
     */
    SAPDB_UInt8 GetRawUInt8() const;

    /*!
     * @brief Return a char[8] representation of this fileNo
     * @return pointer to char[8] representation of this fileNo
     */
    const char* GetCharArray() const;

    /*!
     * @brief Fill buffer with a hex representation of fileNo.
     *
     * @param buffer    [in] pointer to buffer to be filled
     * @param bufLength [in] length of buffer
     *
     * @return position right behind hex representation in buffer
     */
    int HexString(char* buffer, int bufLength) const;

    /*!
     * @brief Stream operator that outputs a hex representation of fileNo.
     *
     * @param os     [in] stream to write to
     * @param fileNo [in] fileNo to write to stream
     */
    friend SAPDB_OStream& operator<<(
        SAPDB_OStream& os,
        const FileDir_FileNo& fileNo);
private:
    static char tempFileNoMaskByte;
    friend class FileDir_SharedDirectory;

    static void SetTempFileNoMaskByte(const char highestByteMask);
    static FileDir_FileNo GetFirstPersistentFileNo();
    static FileDir_FileNo GetFirstTempFileNo();
    static FileDir_FileNo AddOp(const FileDir_FileNo& fileNo,
                                const SAPDB_UInt& toAdd);

//    FileDir_FileNo(const char* value);
    void Set(const char* value);
    FileDir_FileNo& Increment();
    FileDir_FileNo& IncrementBy(SAPDB_UInt value);
    FileDir_FileNo& Decrement();

    SAPDB_Bool IsLessOrEqual(const FileDir_FileNo& fileNo) const;
    SAPDB_Bool IsLessThan(const FileDir_FileNo& fileNo) const;
};

/*************************************************************************/
/*************************************************************************/

/*!
 * @class FileDir_FileNoToString
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief Create a string represenation of a FileDir_FileNo.
 */
class FileDir_FileNoToString
{
public:
    /// Constructor.
    FileDir_FileNoToString(const FileDir_FileNo& fileNo);

    /// Destructor.
    ~FileDir_FileNoToString() {};

    /*!
     * @brief Returns a pointer to the string representation of this
     * objects fileNo.
     *
     * @return pointer to string representation
     */
    operator const char*() const;
private:
    char m_buffer[ FileDir_FileNo::c_hexBufLength+1 ];
};

/*************************************************************************/
/*************************************************************************/

inline FileDir_FileNo::FileDir_FileNo()
{
    m_fileNo.intValue = 0;
}

/**************************************************************************/

// FileDir_FileNo::FileDir_FileNo(const char* value)
// {
//     SAPDB_MemCopyNoCheck( m_fileNo.charArray, value, sizeof(m_fileNo) );
// }

/**************************************************************************/

inline FileDir_FileNo::FileDir_FileNo(const FileDir_FileNo& fileNo)
{
    m_fileNo.intValue = fileNo.m_fileNo.intValue;
}

/**************************************************************************/

inline FileDir_FileNo::FileDir_FileNo(const SAPDB_UInt8 fileNo)
{
#if sp77encodingUCS2Native == sp77encodingUCS2Swapped
    char* bytePtr = reinterpret_cast<char*>(
        const_cast<SAPDB_UInt8*>( &fileNo ) );
    for (int i=7; i >= 0; --i, ++bytePtr) {
        m_fileNo.charArray[ i ] = *bytePtr;
    }
#else
    m_fileNo.intValue = fileNo;
#endif
}

/**************************************************************************/

inline FileDir_FileNo::FileDir_FileNo(const tgg00_Surrogate& surrogate)
{
    SAPDB_MemCopyNoCheck(
        &m_fileNo.intValue, &surrogate, sizeof(m_fileNo.intValue) );
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::operator==(
    const FileDir_FileNo& fileNo) const
{
    return m_fileNo.intValue == fileNo.m_fileNo.intValue;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::operator==(
    const tgg00_Surrogate& surrogate) const
{
    return memcmp( &surrogate, m_fileNo.charArray, sizeof(m_fileNo) ) == 0;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::operator!=(
    const FileDir_FileNo& fileNo) const
{
    return m_fileNo.intValue != fileNo.m_fileNo.intValue;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::operator!=(
    const tgg00_Surrogate& surrogate) const
{
    return memcmp( &surrogate, m_fileNo.charArray, sizeof(m_fileNo) ) != 0;
}

/**************************************************************************/

inline FileDir_FileNo& FileDir_FileNo::operator=(
    const tgg00_Surrogate& surrogate)
{
    SAPDB_MemCopyNoCheck(
        &m_fileNo.intValue, &surrogate, sizeof(m_fileNo.intValue) );
    return *this;
}

/**************************************************************************/

inline FileDir_FileNo& FileDir_FileNo::operator=(const FileDir_FileNo& fileNo)
{
    m_fileNo.intValue = fileNo.m_fileNo.intValue;
    return *this;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::IsValid() const
{
    return m_fileNo.intValue != 0;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::IsInvalid() const
{
    return m_fileNo.intValue == 0;
}

/**************************************************************************/

inline void FileDir_FileNo::Invalidate()
{
    m_fileNo.intValue = 0;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::IsTempFileNo() const
{
    return
        (m_fileNo.charArray[c_tempFileNoMaskBytePos] & tempFileNoMaskByte)
        ==
        tempFileNoMaskByte;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::IsPersistentFileNo() const
{
    return
        (m_fileNo.charArray[c_tempFileNoMaskBytePos] & tempFileNoMaskByte)
        !=
        tempFileNoMaskByte;
}

/**************************************************************************/

inline void FileDir_FileNo::Set(const char* value)
{
    m_fileNo.no.becomes( value );
}

/**************************************************************************/

inline FileDir_FileNo& FileDir_FileNo::Increment()
{
    ++m_fileNo.no;
    return *this;
}

/**************************************************************************/

inline FileDir_FileNo& FileDir_FileNo::IncrementBy(SAPDB_UInt value)
{
    for (SAPDB_UInt i = 0; i < value; ++i) {
        ++m_fileNo.no;
    }
    return *this;
}

/**************************************************************************/

inline FileDir_FileNo& FileDir_FileNo::Decrement()
{
    --m_fileNo.no;
    return *this;
}

/**************************************************************************/

inline void FileDir_FileNo::SetTempFileNoMaskByte(const char highestByteMask)
{
    tempFileNoMaskByte = highestByteMask;
}

/**************************************************************************/

inline FileDir_FileNo FileDir_FileNo::GetFirstTempFileNo()
{
    FileDir_FileNo tempFileNo;

    tempFileNo.m_fileNo.charArray[c_tempFileNoMaskBytePos] |= tempFileNoMaskByte;

    return tempFileNo;
}

/**************************************************************************/

inline FileDir_FileNo FileDir_FileNo::GetFirstPersistentFileNo()
{
    FileDir_FileNo fileNo;

    ++fileNo.m_fileNo.no;

    return fileNo;
}

/**************************************************************************/

inline FileDir_FileNo FileDir_FileNo::AddOp(
     const FileDir_FileNo& fileNo,
     const SAPDB_UInt& toAdd)
{
    FileDir_FileNo tmp(fileNo);
    return tmp.IncrementBy( toAdd );
}

/**************************************************************************/

inline SAPDB_UInt8 FileDir_FileNo::GetUInt8() const
{
#if sp77encodingUCS2Native == sp77encodingUCS2Swapped
    SAPDB_UInt8 val;
    char* bytePtr = reinterpret_cast<char*>( &val );
    for (int i=7; i >= 0; --i, ++bytePtr) {
        *bytePtr = m_fileNo.charArray[ i ];
    }
    return val;
#else
    return m_fileNo.intValue;
#endif
}

/**************************************************************************/

inline SAPDB_UInt8 FileDir_FileNo::GetRawUInt8() const
{
    return m_fileNo.intValue;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::IsLessOrEqual(
    const FileDir_FileNo& fileNo) const
{
    return ! (m_fileNo.no > fileNo.m_fileNo.no);
}

/**************************************************************************/

inline SAPDB_Bool FileDir_FileNo::IsLessThan(
    const FileDir_FileNo& fileNo) const
{
    return ! (m_fileNo.no >= fileNo.m_fileNo.no);
}

/**************************************************************************/

inline FileDir_FileNoToString::operator const char*() const
{
    return m_buffer;
}

/**************************************************************************/

inline const char* FileDir_FileNo::GetCharArray() const
{
    return m_fileNo.charArray;
}

/**************************************************************************/

#endif // FILEDIR_FILENO_HPP
