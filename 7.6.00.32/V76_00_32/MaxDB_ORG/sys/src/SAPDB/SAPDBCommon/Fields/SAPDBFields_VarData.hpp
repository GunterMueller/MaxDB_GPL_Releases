/*!*********************************************************************

    @file         SAPDBFields_VarData.hpp

    @author       DanielD

    @brief        Handling of field buffers as used for SQL communication

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#ifndef SAPDBFIELDS_VARDATA_HPP
#define SAPDBFIELDS_VARDATA_HPP

/*!

  @class SAPDBFields_VarData
  @brief access to a buffer containing fields with variable length

  \code
  buffer {
    u2 fieldCount
    field [] fields
  }
  field {
    u1 charCount (<= csp1_fi_max_1byte_length)
    char [] characters
  |
    u1 indicator
  |
    u1 indicator (= csp1_fi_2byte_length)
    u2 charCount
    char [] characters
  }
  \endcode

 */
class SAPDBFields_VarData
{
public:
    /*!
      @class Reader
      @brief allows to iterate over the fields

      Access to individual fields through an index cannot be implemented
      efficiently. If this capability is needed, the programmer should use
      one of the existing SAPDBFields_FieldSequence implementations,
      initialising it by iterating once through a Reader.
     */
    class Reader
    {
    public:
        /*!
            @brief create a new Reader

            All data like the number of fields is encoded in the buffer.
         */
        Reader (const void * buf);

        /*!
            @brief initialize an empty Reader (to be used when a method
                uses a Reader as an output parameter)
         */
        Reader ();

        /*!
            @brief gets the data of the next field
            @returns true if another field could be read, false if the
                end of the data has been reached
         */
        bool next (const void * & fieldptr, int & fieldlen, int & indicator);

        /*!
            @brief gets the data of the next field when no indicator is expected
            @returns true if another field could be read, false if the
                end of the data has been reached or an indicator has been read
         */
        bool next (const void * & fieldptr, int & fieldlen);

        /*!
            @brief skips over fields
         */
        bool skip (int count = 1);
    protected:
        int fieldsRemaining;
        const unsigned char * readPos;
    };
    /*!
      @class Writer
      @brief allow to fill a SAPDBFields_VarData incrementally
     */
    class Writer
    {
    public:
        /*!
            @brief used only to declare a variable to be filled by openSubfields
         */
        Writer ();

        /*!
            @brief creates a new SAPDBFields_VarData

            The memory in buf is not in a consistent state until method close is called.
         */
        Writer (void * buf, int bufsize);

        /*!
            @brief adds a new data field
         */
        bool addField (const void * field, unsigned int fieldlen);

        /*!
            @brief adds a new data field, allowing for an indicator
         */
        bool addField (const void * field, unsigned int fieldlen, int indicator);

        /*!
            @brief adds a new field with a special value (NULL, DEFAULT, ...)
         */
        bool addSpecial (int indicator);

        /*!
            @brief adds a new field, returning the pointer
         */
        void * reserveField (unsigned int fieldlen);

        /*!
            @brief brings the buffer into a consisten state
         */
        void close (int & fieldCount, int & buflen);

        /*!
            @brief initiailizes a new writer

            This allows to write multiple fields into one field of
            the main SAPDBFields_VarData.
         */
        void openSubfields (Writer & subfields);

        /*!
            @brief closes Writer subfields and adds the data of subfields
            to the main Writer

            Implementation note: The inner writer uses the memory of the
            outer Writer. Depending on the size of the resulting field,
            memory might be copied because the length indicator is
            of varying size.
         */
        void closeSubfields (Writer & subfields);

        /*
            @brief copy all the fields from a Reader into this writer
         */
        bool copyReader (Reader & input);

    private:
        int             fieldCount;
        unsigned char * startPos;
        unsigned char * writePos;
        unsigned char * endPos;
    };

    /*!
        @brief verify that fields don't exceed buflen
     */
    static bool Verify (const void * buf, int buflen, bool allowUnderflow = false);

    /*!
        @brief return the number of fields
     */
    static unsigned int GetFieldCount (const void * buf);

    /*!
        @brief return the number of bytes used
     */
    static unsigned int GetTotalLength (const void * buf);


private:
    SAPDBFields_VarData () {};
};

#endif
