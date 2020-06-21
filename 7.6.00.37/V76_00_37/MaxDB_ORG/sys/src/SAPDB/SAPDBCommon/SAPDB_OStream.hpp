/*!
  @file           SAPDB_OStream.hpp
  @author         DanielD
  @brief          Declaration of classes for stream output


  - class SAPDB_StreamBuffer
  - class SAPDB_OStream
  - Predefined output streams
  - Predefined manipulators

\if EMIT_LICENCE

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



\endif
*/




#ifndef SAPDB_OSTREAM_HPP
#define SAPDB_OSTREAM_HPP

#include <stdio.h>
#include <string.h>

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"


/*!
   @class          SAPDB_StreamBuffer
   @brief          This class serves as an abstract base class for writing output
             buffers for output streams.


                The purpose of an output buffer is to define where the output
                actually goes to.
                To write a special output buffer, one has to derive a class
                from SAPDB_StreamBuffer and overwrite the abstract function
                'Overflow()', which sends the buffer content to an output
                device.

 */


class SAPDB_StreamBuffer
{
private:
    SAPDB_Char* m_Begin;
    SAPDB_Char* m_End;
    SAPDB_Char* m_Next;

public:
    /*!
       @brief          Constructs a stream buffer.
       @param          Begin [in] A pointer to the begin of the buffer.
             another line for this parameter.
       @param          End [in] A pointer behind the end of the buffer.

     */

    SAPDB_StreamBuffer(SAPDB_Char* Begin, SAPDB_Char* End)
        : m_Begin(Begin), m_End(End), m_Next(Begin)
    {}

    /*!
       @brief          Destructs a stream buffer.

     */

    virtual ~SAPDB_StreamBuffer()
    {}

    /*!
       @brief          Sets the parameter of a new buffer.
       @param          Begin [in] A pointer to the begin of the buffer.
       @param          End [in] A pointer behind the end of the buffer.
       @returns        none

     */

    void SetBuffer(SAPDB_Char* Begin, SAPDB_Char* End)
    {
        m_Begin = Begin;
        m_End = End;
        m_Next = m_Begin;
    }

    /*!
       @brief          Returns the start address of the buffer.
       @returns        SAPDB_Char*

     */

    SAPDB_Char* Begin() { return m_Begin; }

    /*!
       @brief          Returns the address thaht points behind the buffer.

     */

    SAPDB_Char* End() { return m_End; }

    /*!
       @brief          Returns the address where the next character would be written.

     */

    SAPDB_Char* Next() { return m_Next; }

    /*!
       @brief          This abstract function shall send the buffer content to an
                 output device.

     */

    virtual void Overflow() = 0;

    /*!
       @brief          Writes a single character to the buffer.


                        If the buffer is full it is flushed.

     */

    void PutChar(SAPDB_Char ch)
    {
        *m_Next = ch;
        ++m_Next;
        if (m_Next == m_End) {
            Flush();
        }
    }

    /*!
       @brief          Writes a character string to the buffer.
       @param          st [in] The string to be output.

     */

    void PutString(const SAPDB_Char* st)
    {
        while (*st != 0) {
            PutChar(*st++);
        }
    }

    /*!
       @brief          Writes a character string to the buffer.
       @param          st [in] The string to be output.
       @param          maxLen [in] the maximal buffer length

     */

    void PutString(const SAPDB_Char* st, int maxLen)
    {
        int remaining = maxLen;

        while ((*st != 0) && (remaining > 0)) {
            PutChar(*st++);
            --remaining;
        }
    }

    /*-----------------------------------------------------------------------
    function: Flush
    description: Flushes the buffer via Overflow and resets the buffer.

                 The buffer only is flushed, if the stream buffer is active.
    -----------------------------------------------------------------------*/
    void Flush()
    {
        Overflow();
        m_Next = m_Begin;
    }
};




/*!
   @class          SAPDB_OStream
   @brief          This class is an output stream.
   @code
    ...
    SAPDB_OStream MyStream(MyBuffer);
    int i = 10;
    MyOwnClass x;
    ...
    MyStream << "Hello world!" << NewLine;
    MyStream << "This is an integer: " << ToStr(i) << NewLine;
    MyStream << "The value of x is: " << x << NewLine;
    MyStream << "The value of x is: " << x.Format() << NewLine;
   @endcode


                     Its purpose is to send a stream of characters to an output
                     device. The output device is determined by the
                     'SAPDB_StreamBuffer' member.
                     Basic data types of C++ can be formatted to character strings
                     by means of class 'SAPDB_ToString'.
                     Other classes can be streamed by providing a special friend
                     method, e.g. for class X the method looks like:
                         friend SAPDB_OStream&amp; operator &lt;&lt;(SAPDB_OStream&amp; os, X x);
                     If different formats should be supported, one or more
                     format methods must be supplied, e.g.
                         X&amp; X::Format(...) {
                             // remember the arguments in static members
                             ...
                             // Output this instance
                             return *this;
                         }

 */


class SAPDB_OStream
{
private:
    SAPDB_StreamBuffer& m_Buffer;

public:
    /*!
       @brief          Constructs an output stream.
       @param          Buffer [in] A reference to the output buffer used.

     */

    SAPDB_OStream(SAPDB_StreamBuffer& Buffer)
        : m_Buffer(Buffer)
    {}

    /*!
       @brief          Destructs a stream buffer.

     */

    ~SAPDB_OStream()
    {}

    /*!
       @brief          A manipulator is a function, with has only an output stream
                 as an argument and manipulates the state of the stream.

     */

    SAPDB_OStream& operator << (SAPDB_OStream& (*func)(SAPDB_OStream&))
        { return (*func)(*this); }

    /*!
       @brief          This operator outputs a variable to the output stream.


                        There is one operator for each basic type, with following
                        exceptions:
       - bool: There is no conversion from bool to the strings                             'true' and 'false'. The formatting option in                             SAPDB_ToString is not yet implemented.
       - SAPDB_Int8 and SAPDB_UInt8: There is not string conversion                                                          for these type in SAPDB_ToString.

     */

    SAPDB_OStream& operator << (const SAPDB_Char* st)
    {
        m_Buffer.PutString(st);
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_Char ch)
    {
        m_Buffer.PutChar(ch);
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_Int1 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_Int2 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_Int4 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
#ifndef BIT64
    SAPDB_OStream& operator << (const SAPDB_Int8 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
#endif
    SAPDB_OStream& operator << (const SAPDB_Long i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_UInt1 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_UInt2 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_UInt4 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
#ifndef BIT64
    SAPDB_OStream& operator << (const SAPDB_UInt8 i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
#endif
    SAPDB_OStream& operator << (const SAPDB_ULong i)
    {
        m_Buffer.PutString(ToStr(i));
        return *this;
    }
    SAPDB_OStream& operator << (const SAPDB_Real8 r)
    {
        m_Buffer.PutString(ToStr(r));
        return *this;
    }

    /*!
       @brief          Writes a character string to the buffer.
       @param          st [in] The string to be output.

     */

    void PutString (const char * st)
    {
        m_Buffer.PutString (st);
    }

    /*!
       @brief          Writes a character string to the buffer.
       @param          st [in] The string to be output.
       @param          maxLen [in] The maximal length of the string.

     */
    void PutString (const char * st, int maxLen)
    {
        m_Buffer.PutString (st, maxLen);
    }

    /*!
       @brief          writes buffered content to the output medium

     */

    void Flush ()
    {
        m_Buffer.Flush();
    }

private:
    // avoid copying streams, because two streams should not share the same buffer.
    SAPDB_OStream(const SAPDB_OStream& os)
        : m_Buffer(os.m_Buffer) {}
};




// Predefined output streams
extern SAPDB_OStream Console;

/*! @name Stream Manipulators */
 /*@{*/

/*!
   @brief          write a new line to a stream
   @code
    stream << "Hello, world!" << NewLine;
   @endcode

 */

extern SAPDB_OStream& NewLine(SAPDB_OStream& os);

/*!
   @brief          write a new line to a stream and flush the stream
   @code
    stream << "Hello, world!" << FlushLine;
   @endcode

 */

extern SAPDB_OStream& FlushLine(SAPDB_OStream& os);


/*!
   @class          SAPDB_Manipulator
   @brief          Abstract class used to overload operator &lt;&lt;.


          Subclasses don't have to overload operator&lt;&lt;(stream) themselves
          because they use
          SAPDB_OStream&amp; operator&lt;&lt;(SAPDB_OStream&amp;, const SAPDB_Manipulator&amp; )
          implicitely.

 */


class SAPDB_Manipulator
{
public:
     virtual SAPDB_OStream& ToStream (SAPDB_OStream& os) const = 0;
protected:
     SAPDB_Manipulator () {};
     ~SAPDB_Manipulator () {};
};



// operator << for output of manipulators
inline SAPDB_OStream& operator <<(SAPDB_OStream& os,
                                  const SAPDB_Manipulator& manipulator)
{
     manipulator.ToStream (os);
     return os;
}


/*!
   @class          SAPDB_Blanks
   @brief          A manipulator used to write blanks to a stream
   @code
    stream << SAPDB_Blanks(4) << "some text";
   @endcode

 */


class SAPDB_Blanks
    : public SAPDB_Manipulator
{
private:
    int m_BlankCount;
public:
    SAPDB_Blanks (int Count) : m_BlankCount(Count) {}
    virtual ~SAPDB_Blanks () {}
    virtual SAPDB_OStream& ToStream (SAPDB_OStream& os) const;
};



/*!
   @interface      SAPDB_Indent
   @brief          A manipulator used to write data with indentation
   @code
    // write two indents of four blanks
    stream << SAPDB_Indent(2) << "some text";
    // write one indent of 3 blanks
    stream << SAPDB_Indent(1, 3) << "some text";
   @endcode

 */

class SAPDB_Indent
     : public SAPDB_Blanks
{
public:
    SAPDB_Indent (int stepsParm, int stepSizeParm = 4)
     : SAPDB_Blanks (stepsParm * stepSizeParm)
    {}
};



/*!
   @class          SAPDB_StringJ
   @brief          abstract base class for formatted strings

 */

class SAPDB_StringJ
    : public SAPDB_Manipulator
{
protected:
    SAPDB_StringJ (const char * dataParm,
                   int          dataLenParm,
                   int          fieldWidthParm);

protected:
    const char * data;
    int          dataLen;
    int          fieldWidth;
};




/*!
   @class          SAPDB_LeftJ
   @brief          to write left aligned strings to a &lt;Class>SAPDBB_OStream&lt;/>
   @code
    // write 'some string         '
    stream << SAPDB_Left ("some string", 20);
    // write 'some                '
    stream << SAPDB_Left ("some string", 4, 20);
    // write 'some'
    stream << SAPDB_Left ("some string", 4, 4);
   @endcode


     Objects of this class are to be used only as temporary values.
     They become invalid when the pointer passed t o the constructor
     becomes invalid

 */

class SAPDB_LeftJ
    : public SAPDB_StringJ
{
public:
    SAPDB_LeftJ (const char * dataParm, int dataLenParm, int fieldWidthParm);
    SAPDB_LeftJ (const char * dataParm, int fieldWidthParm);
    virtual SAPDB_OStream& ToStream (SAPDB_OStream& os) const;
};



/*!
   @class          SAPDB_RightJ
   @brief          to write right aligned strings to a &lt;Class>SAPDBB_OStream&lt;/>
   @code
    // write '         some string'
    stream << SAPDB_Right ("some string", 20);
    // write '                some'
    stream << SAPDB_Right ("some string", 4, 20);
    // write 'some'
    stream << SAPDB_Right ("some string", 4, 4);
   @endcode


   Objects of this class are to be used only as temporary values.
   They become invalid when the pointer passed t o the constructor
   becomes invalid

 */

class SAPDB_RightJ
    : public SAPDB_StringJ
{
public:
    SAPDB_RightJ (const char * dataParm, int dataLenParm, int fieldWidthParm);
    SAPDB_RightJ (const char * dataParm, int fieldWidthParm);
    virtual SAPDB_OStream& ToStream (SAPDB_OStream& os) const;
};


/*@}*/


/***********************************************************************

  I M P L E M E N T A T I O N

 **********************************************************************/

/* workaround for possible compiler bug on Linux on PPC */

/***********************************************************************
 class SAPDB_Blanks
 **********************************************************************/
#if defined(SDBonPPC64)
inline SAPDB_OStream& SAPDB_Blanks::ToStream(SAPDB_OStream& os) const
{
    static const int maxBlanksC = 64;
    static const char * blanksC =
        "                                                                ";
    int blanksRemaining = m_BlankCount;
    while (blanksRemaining > 0) {
        int thisChunk = (blanksRemaining > maxBlanksC)
            ? maxBlanksC : blanksRemaining;
        os << (blanksC + maxBlanksC - thisChunk);
        blanksRemaining -= thisChunk;
    }
    return os;
}

#endif

/***********************************************************************
 class SAPDB_StringJ
 **********************************************************************/

inline SAPDB_StringJ::SAPDB_StringJ (
    const char * dataParm,
    int          dataLenParm,
    int          fieldWidthParm)
{
    this->data = dataParm;
    if (dataLenParm == -1) {
        this->dataLen = (int) strlen (dataParm);
    }
    else {
        this->dataLen = dataLenParm;
    }
    this->fieldWidth = fieldWidthParm;
}

/***********************************************************************
 class SAPDB_LeftJ
 **********************************************************************/

inline SAPDB_LeftJ::SAPDB_LeftJ (
    const char * dataParm,
    int dataLenParm,
    int fieldWidthParm)
: SAPDB_StringJ (dataParm, dataLenParm, fieldWidthParm)
{
}

/*----------------------------------------*/

inline SAPDB_LeftJ::SAPDB_LeftJ (
    const char * dataParm,
    int fieldWidthParm)
: SAPDB_StringJ (dataParm, -1, fieldWidthParm)
{
}

/*----------------------------------------*/

inline SAPDB_OStream& SAPDB_LeftJ::ToStream (
    SAPDB_OStream& os) const
{
    int blankCount = this->fieldWidth - this->dataLen;

    os.PutString (this->data, this->dataLen);
    if (blankCount > 0) {
        os << SAPDB_Blanks (blankCount);
    }
    return os;
}

/***********************************************************************
 class SAPDB_RightJ
 **********************************************************************/

inline SAPDB_RightJ::SAPDB_RightJ (
    const char * dataParm,
    int dataLenParm,
    int fieldWidthParm)
: SAPDB_StringJ (dataParm, dataLenParm, fieldWidthParm)
{
}

/*----------------------------------------*/

inline SAPDB_RightJ::SAPDB_RightJ (
    const char * dataParm,
    int fieldWidthParm)
: SAPDB_StringJ (dataParm, -1, fieldWidthParm)
{
}

/*----------------------------------------*/

inline SAPDB_OStream& SAPDB_RightJ::ToStream (
    SAPDB_OStream& os) const
{
    int blankCount = this->fieldWidth - this->dataLen;

    if (blankCount > 0) {
        os << SAPDB_Blanks (blankCount);
    }
    os.PutString (this->data, this->dataLen);
    return os;
}

#endif // SAPDB_OSTREAM_HPP

