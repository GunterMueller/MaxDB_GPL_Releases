/*!
 * \file    OMS_Stream.hpp
 * \author  ThomasA
 * \brief   OMS streams
 */
/*

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



*/


#ifndef OMS_STREAM_HPP
#define OMS_STREAM_HPP

#include "Oms/OMS_AFX.h"
#include "livecachetypes.h" // nocheck

class OmsHandle;
class OMS_Session;

class OMS_StreamBody; 


/*-------------------------------------------------------------------------*/
/// Base class for OmsIStream and OmsOStream
/*! 
** Base class for OmsIStream and OmsOStream. These classes provide a
** stream oriented access to ABAP tables residing at the client side.
*/
class AFX_EXT_CLASS OmsStream 
{
protected :
  /*-------------------------------------------------------------------------*/
  /// Constructor
  /*!
  ** Constructs a OmsStream instance
  **
  ** \param h [in] Handle
  ** \param sh [in] reference to an ABAP table handle or a stream handle respectively.
  **             this handle contains information about the size of a stream row
  **             as well as swap and row construction info.
  **             This info is needed, if the client layout and the liveCache layout
  **             of a row differ.
  ** \param rsz [in] size of a raw of the stream in liveCache layout
  ** \param sz [in]
  ** \param istream [in] if true, the stream is constructed for an input stream, i.e
  **             rows are tranfered from the client to the liveCache.
  **             Otherwise the stream is constructed for an output stream, i.e.
  **             rows are tranfered from the liveCache to the client.
  */
  OmsStream(OmsHandle* h, OmsTypeABAPTabHandle& sh, long rsz, long sz, bool istream = true);
  OmsStream(OmsHandle* h, OmsTypeStreamHandle& sh, long rsz, long sz, bool istream = true);

  /*-------------------------------------------------------------------------*/
  /// Destructor
  /*!
  ** Destructs an OmsStream instance. In case of an output stream
  ** rows that are still buffered are NOT transfered to the client.
  ** The stream must therefore closed explicitely.
  ** All memory hold by the stream instance is released.
  */
  ~OmsStream();

  /*-------------------------------------------------------------------------*/
  /// Closes the stream
  /*!
  ** Closes the stream. In case of an output stream, all buffered
  ** rows are transfered to the client. After the stream has been
  ** closed, no more rows can be read or written.
  */
  void           omsClose();
  void           omsCloseBytes();

  /*-------------------------------------------------------------------------*/
  /// Flushes the stream
  /*!
  ** In case of an output stream, all buffered rows are transfered to the client. 
  */
  void omsFlush();
  void omsFlushBytes();
  /*-------------------------------------------------------------------------*/
  /// hides a column from the stream
  /*!
  ** do not transfer this column any more 
  */
  void omsHide(unsigned int columnNo);

  /*-------------------------------------------------------------------------*/
  /// Returns pointer to next row
  /*!
  ** Yields a pointer to the next row of the stream.  
  **
  ** \return pointer to the next row of the stream, NULL if end of stream
  **         has been reached
  */
  unsigned char* omsNxt();

  /*-------------------------------------------------------------------------*/
  /// Writes a row into the stream
  /*!
  ** Writes a row identified by a pointer into the stream.  
  **
  ** \param pRow [in] a pointer to the row to be written into the stream
  */
  void omsWrite (unsigned char* pRow);
  void omsWriteBytes (const unsigned char* pRow, unsigned int bufLen);
  OmsTypeUInt4 omsReadBytes(unsigned char *buf, OmsTypeUInt4 bufLen);

  /*------ PTS 1131114, PG -------------------------------------------------*/
  /// Resets an input stream
  /*!
  ** 
  */
  void omsIReset();

  /*------ PTS 1131555, PG -------------------------------------------------*/
  /// Resets an output stream
  /*!
  ** 
  */
  void omsOReset();

  /*------ PTS 1135058, PG -------------------------------------------------*/
  /// Disable compression of default values.
  /*!
  ** 
  */
  void omsDisableDefaultCompression();

  /*------ PTS 1135058, PG -------------------------------------------------*/
  /// Check if default value compression is enabled.
  /*!
  ** 
  */
  bool omsDefaultCompressionEnabled() const;

private:
  OmsStream(const OmsStream&); // no copy
  OMS_StreamBody* m_body; // all implementing details are hidden from the user
                          // (body handle idiom)
  OMS_Session *m_pSession;
};

/*! endclass: OmsStream */




/*-------------------------------------------------------------------------*/
/// abap table input stream
/*! 
** Transforms a stream handle or a handle for an internal ABAP table to an 
** input data stream.
**
** \see \ref example_OmsIStream
*/
template <class HandleType, class RowType> 
class OmsIStream : public OmsStream {
public:
  /*-------------------------------------------------------------------------*/
  /// Constructor
  /*! 
  ** Constructor for an input stream of structures of the type RowType. The 
  ** cached_rows parameter defines the size of a buffer area for reading rows 
  ** from the output stream. If “cached_rows” is not specified, the OMS uses 
  ** the size of the communications buffer to determine the size of the buffer 
  ** area.
  */
  OmsIStream (OmsHandle* h, HandleType& th, long cached_rows = -1) :
       OmsStream (h, th.Handle, sizeof(RowType), cached_rows)
  {
    RowType* p = th.RowDef;
    p;
  }

  /*-------------------------------------------------------------------------*/
  /// hides a column from the istream
  /*!
  ** this column will not be tranferred from application any more 
  */
  void omsHide(unsigned int columnNo) { OmsStream::omsHide(columnNo); }

  /*-------------------------------------------------------------------------*/
  /// Gets next row from input stream
  /*! 
  ** Provides the next row in the input stream. If not all the rows of the buffer 
  ** have been read, the next row of the buffer area is provided. If this is 
  ** not the case, the buffer area is first filled again by means of communication 
  ** with the application, and the first row of the buffer area is provided.
  ** omsRead provides the NULL pointer when the end of the input stream has been 
  ** reached.
  */
  RowType* omsRead() { return (RowType*) omsNxt(); };
  OmsTypeUInt4 omsReadBytes(unsigned char *buf, OmsTypeUInt4 bufLen) { return OmsStream::omsReadBytes(buf, bufLen); };

  /*----- PTS 1131114, PG --------------------------------------------------*/
  /// Resets input stream
  /*! 
  **  Clears local buffer and request first package again from client.
  */
  void omsReset() { omsIReset(); };

  /*------ PTS 1135058, PG -------------------------------------------------*/
  /// Disable compression of default values.
  /*! 
  **
  */
  void omsDisableDefaultCompression() { OmsStream::omsDisableDefaultCompression(); }

  /*------ PTS 1135058, PG -------------------------------------------------*/
  /// Check if default value compression is enabled.
  /*! 
  **
  */
  bool omsDefaultCompressionEnabled() const { return OmsStream::omsDefaultCompressionEnabled(); }
};





/*-------------------------------------------------------------------------*/
/// abap table output stream
/*!
** Transforms a stream handle or a handle for an internal ABAP table to an 
** output data stream.
**
** \see \ref example_OmsOStream
*/
template <class HandleType, class RowType> 
class OmsOStream : public OmsStream {
public :
  /*-------------------------------------------------------------------------*/
  /// Constructor
  /*! 
  ** Constructor for an output stream of structures of the type RowType. 
  ** The cached_rows parameter defines the size of a buffer area for writing 
  ** rows of the output stream. If “cached_rows” is not specified, the OMS uses 
  ** the size of the communications buffer to determine the size of the buffer 
  ** area.
  */
  OmsOStream (OmsHandle* h, HandleType& th, long cached_rows = -1) :
       OmsStream (h, th.Handle, sizeof(RowType), cached_rows, false) 
  {
    RowType* p = th.RowDef;
    p;
  };

  /*-------------------------------------------------------------------------*/
  /// Closes the stream
  /*!
  ** Copies all the rows in the buffer area of the output stream to the application 
  ** and closes the output stream.
  ** If an output stream is not closed with omsClose, it cannot be ensured that 
  ** all the instances written to the stream are transferred to the application.
  */
  void omsClose () { OmsStream::omsClose (); };
  void omsCloseBytes () { OmsStream::omsCloseBytes (); };

  /*-------------------------------------------------------------------------*/
  /// hides a column from the ostream
  /*!
  ** this column will not be tranferred to the application any more 
  */
  void omsHide(unsigned int columnNo) { OmsStream::omsHide(columnNo); }

  /*-------------------------------------------------------------------------*/
  /// Writes to the stream
  /*!
  ** Writes an instance of the type RowType to the output stream. If this completely 
  ** fills the buffer area, the buffer is transferred to the application. The buffer 
  ** is transferred asynchronously; in other words, the system does not wait for the 
  ** result of the transfer.
  */
  void omsWrite (const RowType& row) { OmsStream::omsWrite ((unsigned char*) &row); };     
  void omsWriteBytes (const unsigned char *buf, unsigned int bufLen) { OmsStream::omsWriteBytes (buf, bufLen); };     

  /*----- PTS 1131555, PG --------------------------------------------------*/
  /// Resets output stream
  /*! 
  **  Reset local buffer and signal client restarted sending of packet(s).
  */
  void omsReset() { omsOReset(); };

  /*------ PTS 1135058, PG -------------------------------------------------*/
  /// Disable compression of default values.
  /*! 
  **
  */
  void omsDisableDefaultCompression() { OmsStream::omsDisableDefaultCompression(); }

  /*------ PTS 1135058, PG -------------------------------------------------*/
  /// Check if default value compression is enabled.
  /*! 
  **
  */
  bool omsDefaultCompressionEnabled() const { return OmsStream::omsDefaultCompressionEnabled(); }
};

#endif
