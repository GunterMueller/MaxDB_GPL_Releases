/*!*********************************************************************

    @file         Catalog_FieldSequenceMarshaler.hpp

    @author       ThomasA

    @brief        stores and restores FieldSequences into and from the catalog

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

#ifndef CATALOG_FIELDSEQUENCEMARSHALER_HPP
#define CATALOG_FIELDSEQUENCEMARSHALER_HPP

#include "SAPDBCommon/SAPDB_IPersistenceStream.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

class Catalog_SessionCache;

/*!
   @brief implements the SAPDB_IPersistenceStream interface for streams contained in messblock catalog information 
 */

class Catalog_FieldSequenceMarshaler : public SAPDB_IPersistenceStream
{
public:
    /*!
      @brief constructor
     */
    Catalog_FieldSequenceMarshaler(Catalog_SessionCache& SessionCache, Catalog_Object::Catalog_Key & Key, int MaxStoredKey);
    /*!
      @brief destructor, flushes last row if needed
     */
    ~Catalog_FieldSequenceMarshaler();
    /*!
       @brief flushes the current row
     */
    void Flush();
    /*!
       @brief returns the number of rows read
     */
    int GetReadRows() const;
    /*!
       @brief returns the number of rows written
     */
    int GetWrittenRows() const;

    /*!
      @brief reads a given number of bytes.

      It is an error if the end of the stream is reached before
      RequiredBytes bytes could be read.

      @return true if call succeeded
     */
    bool ReadBytes (void* pDestination, int RequiredBytes, Msg_List& MsgList);
    /*!
      @brief writes a given number of bytes
      @return true if call succeeded
     */
    bool WriteBytes (const void* pSource, int ByteCount, Msg_List& MsgList);
   
private :
    bool AddIntoCatalog ();
    enum SerializerKind { kind_undef, kind_reader, kind_writer }; //!< different marshaler types (reader, writer)
    SerializerKind              m_kind;                           //!< marshaler type (reader, writer
    Catalog_SessionCache&       m_sessionCache;                   //!< reference to the session catalog cache
    int                         m_offset;                         //!< current offset
    int                         m_maxKey;                         //!< greatest key stored of before image
    int                         m_written;                        //!< number of written rows
    Catalog_Object::Catalog_Key m_key;                            //!< current key
    tgg00_Rec                   m_record;                         //!< current record
};

#endif

