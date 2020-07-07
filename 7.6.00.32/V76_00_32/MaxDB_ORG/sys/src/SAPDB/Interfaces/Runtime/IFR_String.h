/*!
  @file           IFR_String.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          handling strings with encoding and length
  @see            

\if EMIT_LICENCE

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



\endif
*/
#ifndef IFR_STRING_H
#define IFR_STRING_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/Util/IFRUtil_CopyTraits.h"

IFR_BEGIN_NAMESPACE

#define IFR_StringEncodingType SQLDBC_StringEncodingType

typedef IFR_StringEncodingType::Encoding IFR_StringEncoding;

IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_StringEncodingType::Encoding encoding);


/**
 * @ingroup IFR_Common
 * Unknown encoding.
 */
#define IFR_StringEncodingUnknown        IFR_StringEncodingType::Unknown


/**
 * @ingroup IFR_Common
 * Plain 8-bit ASCII encoding (each byte stands for itself).
 */
#define IFR_StringEncodingAscii        IFR_StringEncodingType::Ascii

/**
 * @ingroup IFR_Common
 * UCS2 big-endian encoding.
 */
#define IFR_StringEncodingUCS2         IFR_StringEncodingType::UCS2

/**
 * @ingroup IFR_Common
 * UCS2 little-endian encoding.
 */
#define IFR_StringEncodingUCS2Swapped  IFR_StringEncodingType::UCS2Swapped

/**
 * @ingroup IFR_Common
 * UCS2 native encoding.
 */
#if defined(WIN32) || (defined(LINUX) && !defined(SDBonPPC64) && !defined(S390X)) || defined(OSF1) || (defined(SOLARIS) && (defined(I386) || defined(X86_64)))
#define IFR_StringEncodingUCS2Native   IFR_StringEncodingType::UCS2Swapped
#else
#define IFR_StringEncodingUCS2Native   IFR_StringEncodingType::UCS2
#endif

/**
 * @ingroup IFR_Common
 * UTF8 encoding. 
 */
#define IFR_StringEncodingUTF8         IFR_StringEncodingType::UTF8

/**
 * @ingroup IFR_Common
 * Macro to make it easier to get a <code>tsp77encoding *</code> from a <code>IFR_StringEncoding</code>
 * @param A value of type <code>IFR_StringEncoding</code>.
 * @return The corresponding <code>tsp77encoding *</code> value, which may be 0.
 */
#define IFR_ENCODING(x) (                                               \
   (x) == IFR_StringEncodingType::Ascii ? sp77encodingAscii :           \
     (x) == IFR_StringEncodingType::UCS2Swapped ? sp77encodingUCS2Swapped : \
       (x) == IFR_StringEncodingType::UCS2 ? sp77encodingUCS2 :         \
         (x) == IFR_StringEncodingType::UTF8 ? sp77encodingUTF8 : 0     \
) 


/**
 * @ingroup IFR_Common
 * @brief A string class. 
 *
 * It has an own buffer, length and encoding. 
 */
class IFR_String
{
public:
    /**
     * Creates an empty string with ascii encoding that uses the given allocator.
     * @param allocator the allocator that will be used for memory allocation.
     */
    IFR_String(SAPDBMem_IRawAllocator& allocator);

    /**
     * Creates an empty string.
     * @param allocator the allocator that will be used for memory allocation.
     * @param encoding the encoding of the string.
     */
    IFR_String(IFR_StringEncoding encoding, 
               SAPDBMem_IRawAllocator& allocator);
    
    /**
     * Copy constructor.
     * @param source the string to copy from.
     * @param memory_ok The 'memory ok' flag.
     */
    IFR_String (const IFR_String& source, IFR_Bool& memory_ok);       

    /**
     * Copy constructor, that changes the allocator.
     * @param allocator The new allocator to be used.
     * @param source the string to copy from.
     */
    IFR_String (SAPDBMem_IRawAllocator& allocator,
                const IFR_String& source,
                IFR_Bool& memory_ok);       



    /**
     * Creates a string from a terminated buffer.
     * @param buffer the buffer
     * @param encoding the encoding of the buffer (and the string)
     * @param allocator the allocator that will be used for memory allocation.
     */
    IFR_String (const char *buffer,
                IFR_StringEncoding encoding, 
                SAPDBMem_IRawAllocator& allocator,
                IFR_Bool& memory_ok);

    /**
     * Creates a string from a buffer. Only a given number of bytes is examined, and at most
     * this number is used. If the terminator is found before, the string will be shorter.
     * @param buffer the buffer
     * @param length the maximum number of bytes to examine.
     * @param encoding the encoding of the buffer (and the string)
     * @param allocator the allocator that will be used for memory allocation.
     */
    IFR_String (const char *buffer, 
                IFR_Length length,
                IFR_StringEncoding encoding, 
                SAPDBMem_IRawAllocator& allocator,
                IFR_Bool& memory_ok);

    /**
     * Destructor. Deallocates the memory.
     */
    ~IFR_String();

    /**
     * Clears the contents of the string.
     * @param encoding The encoding of the string.
     */
    inline void clear(IFR_StringEncoding encoding=IFR_StringEncodingAscii)
    {
        IFR_Bool ignored_memory_ok=true;
        setBuffer("", 0, encoding, ignored_memory_ok);
    }

    /**
     * Sets the data of the string from a buffer. The data is copied.
     * @param buffer the buffer
     * @param length the maximum number of bytes to examine.
     * @param encoding the encoding of the buffer (and the string)
     */
    void setBuffer(const char *buffer,
                   IFR_Length length,
                   IFR_StringEncoding encoding,
                   IFR_Bool& memory_ok);
    /**
     * Sets the data of the string from a (terminated) buffer. The data is copied.
     * @param buffer the buffer
     * @param encoding the encoding of the buffer (and the string)
     */
    inline void setBuffer(const char *buffer,
                          IFR_StringEncoding encoding,
                          IFR_Bool& memory_ok)
    {
        setBuffer(buffer, (IFR_Length) IFR_NTS, encoding, memory_ok);
    }
    
    /**
     * Gets the buffer.
     * @return The raw buffer of the string, or a buffer containing
     *         the empty string, in the correct encoding. The result
     *         is never 0.
     */
    inline const char *getBuffer() const
    {
        if(m_buffer) {
            return m_buffer;
        } else {
            static char buf[] = { '\0', '\0' };
            return (const char *)buf;
        }
    }

    /**
     * Gets the number of bytes without the terminator char(s).
     * @return The length in bytes that the stored string does occupy.
     */
    inline IFR_size_t getLength() const
    {
        return (IFR_size_t)m_length_in_bytes;
    }
    
    /**
     * Gets the number of characters (not bytes).
     * @return The number of characters in the string.
     */
    IFR_Length getStrLen() const;
    
    /**
     * Gets the encoding.
     * @return The character encoding of the string.
     */
    inline IFR_StringEncoding getEncoding() const
    {
        return m_encoding_type;
    }
    
    
    /**
     * Makes room for more data. The buffer is increased so that it can
     * hold the given number of bytes, if necessary.
     * @param newbuffersize the required buffer size.
     */
    void expand(IFR_Length newbuffersize, IFR_Bool& memory_ok);
    
    /**
     * Appends another string. 
     * @param to_add the string to append. 
     * @return <code>true</code> if the <code>to_add</code> can added
     * or <code>false</code> if the <code>to_add</code> can't be
     * converted.
     */
    IFR_Bool append(const IFR_String& to_add, IFR_Bool& memory_ok);

    /**
     * Appends another character buffer.
     * @param to_add The character buffer to append.
     * @param encoding The encoding of the character buffer.
     * @param length The maximum length (in bytes) of <code>to_add</code>, 
     *        the default is to assume <code>to_add</code> is zero-terminated.
     * @return The method returns <code>true</code> if the
     * <code>to_add</code> can added or <code>false</code> if
     * <code>to_add</code> cannot be converted.
     */
    IFR_Bool append(const char *to_add, 
                    IFR_StringEncoding encoding, //  = IFR_StringEncodingAscii, 
                    IFR_size_t length, // =(IFR_size_t)IFR_NTS);
                    IFR_Bool& memory_ok);
    
    /**
     * Compares two strings.
     * @param a The first string.
     * @param b The second string.
     * @return Similar to <code>strcmp</code>, 0 if both strings are equals, a number
     *  &lt;0 if the <code>a&lt;b</code>, a number &gt;0 if <code>a&gt;b</code>.
     */
    static IFR_Int4 compare(const IFR_String& a, const IFR_String& b, IFR_Bool& memory_ok);

    
    /**
     * Checks whether the string equals the given ascii string. This string
     * must be ascii too.
     * @param str The string to compare.
     * @return @c true if found.
     */
    IFR_Bool equalsAscii(const char *str) const;
    

    /**
     * Compares two strings.
     * @param a The first string.
     * @param b The second string.
     * @return <code>true</code> if the two strings are equal including their encoding.
     */
    static IFR_Bool equalsWithEncoding(const IFR_String& a, const IFR_String& b);

    /**
     * Computes a hash code on the string.
     * @return The computed hash code.
     */
    IFR_Int4 hashCode() const;

    /**
     * Copies this string into a buffer.
     * @param buffer The output buffer. Zero terminated string.
     * @param encoding Encoding of <var>buffer</var>, be set to the encoding of buffer.
     * @param bufferSize The size in bytes of the target buffer.
     * @param bufferLength [out] The number of bytes copied to the buffer, 
     *        except the number of bytes necessary for the zero terminator. 
     *        If the source string exceeds the <code>bufferSize</code>.
     *        IFR_DATA_TRUNC will returned and the <code>bufferLength</code>
     *        is set to the number of bytes (except the terminator bytes) 
     *        needed to copy without truncation.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code> 
     * if the buffer does not fit to the <code>bufferSize</code>.
     */
    IFR_Retcode copyInto(char *buffer, IFR_StringEncoding& encoding, IFR_Length bufferSize, IFR_Length *bufferLength) const;

    /**
     * Copies this string into a buffer, hereby possible doing a recoding.
     * @param buffer The output buffer.
     * @param encoding The requested encoding.
     * @param bufferSize The size in bytes of the target buffer.
     * @param bufferLength [out] The number of bytes copied to the buffer, 
     *        except the number of bytes necessary for the zero terminator. 
     *        If the source string exceeds the <code>bufferSize</code>.
     *        IFR_DATA_TRUNC will returned and the <code>bufferLength</code>
     *        is set to the number of bytes (except the terminator bytes) 
     *        needed to copy without truncation.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code> if the buffer does not fit.
     */
    IFR_Retcode convertInto(char *buffer, IFR_StringEncoding encoding, IFR_Length bufferSize, IFR_Length *bufferLength) const;

    /**
     * Get the allocator used for this string.
     * @return The allocator used for this string.
     */
    inline SAPDBMem_IRawAllocator& getAllocator() const
    {
        return (SAPDBMem_IRawAllocator&) m_allocator;
    }
    
    /**
     * Assignment function.
     * @param source The source string.
     * @param memory_ok The 'memory ok' flag.
     */
    void assign(const IFR_String& source, IFR_Bool& memory_ok);

private:
    IFR_String(const IFR_String& copy);
    IFR_String& operator =(const IFR_String& source);

    SAPDBMem_IRawAllocator& m_allocator;            //!< Allocator instance used for getting the memory.
    char                  *m_buffer;                //!< Character buffer.
    IFR_Length             m_length_in_bytes;       //!< Data length in bytes. This does not include trailing '0's.
    IFR_Length             m_bufsize;               //!< Size of the buffer. Size of data <i>plus</i> length of terminator.
    IFR_StringEncoding     m_encoding_type;         //!< Encoding structure.
    IFR_Length             m_strlen;                //!< Cache for number of characters.
    static char            m_static_empty_buffer[2];//!< Static empty string.
};

class IFR_TraceStream;

/**
 * @ingroup IFR_Trace
 * Traces a string onto the trace stream.
 * @param s The target stream.
 * @param p The string to trace.
 * @return <code>s</code>
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_String& p );

/**
 * @ingroup IFR_Trace
 * Traces a string encoding tag onto the trace stream.
 * @param s The target stream.
 * @param p The string encoding to trace.
 * @return <code>s</code>
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_StringEncoding *p );


/**
 * Copy traits for @c IFR_String. 
 * These copy traits handle the memory check.
 */
template<> class IFRUtil_CopyTraits<IFR_String>
{
public:
    typedef SAPDBMem_IRawAllocator& ConstructorType;
    typedef IFRUtil_FalseType IsDefaultConstructible;
    typedef IFRUtil_FalseType IsCopyConstructible;

    static inline void copy(IFR_String& target, const IFR_String& source, IFR_Bool& check)
    {
        if(check) {
            target.assign(source, check);
        }
        return;
    }
    
    static inline ConstructorType init_helper(SAPDBMem_IRawAllocator* allocator, const IFR_String& source)
    {
        return allocator==0 ? source.getAllocator() : *allocator;
    }

};

IFR_END_NAMESPACE

#endif //IFR_STRING_H
