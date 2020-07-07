/*!
 * \file    OMS_StreamRegistry.hpp
 * \author  ThomasA
 * \brief   registers all active streams and ABAP tables.
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

#ifndef __OMS_STREAMREGISTRY_HPP
#define __OMS_STREAMREGISTRY_HPP

class OMS_StreamBody;
class OMS_Session;

/*!
  @brief registry of active oms streams. This registry is used to transport multiple
         streams via one commuction to/from the application (see OMS_Stream.cpp)
 */
class OMS_StreamRegistry
{
public :
    /*!
       @brief defines 2 double chained lists of registered streams. The first list holds
              input streams, the other holds output streams.
     */
    class List
    {
    public :
         /*!
            @brief constructor
          */
         List(bool IsHead = false);
         /*!
            @brief returns the next stream, or null, if none exists
          */
         OMS_StreamBody* GetNextStream() const;
          /*!
            @brief returns the previous stream, or null, if none exists
          */
         OMS_StreamBody* GetPrevStream() const;
         /*!
            @brief sets the next pointer
          */
         void SetNextStream(OMS_StreamBody* pStream);
         /*!
            @brief sets the prev pointer
          */
         void SetPrevStream(OMS_StreamBody* pStream );
    private :
         OMS_StreamBody* m_prev; //!< pointer to the previous stream descriptor
         OMS_StreamBody* m_next; //!< pointer to the next stream descriptor
    };
    /*!
       @brief constructor, constructs an empty registry 
     */
    OMS_StreamRegistry(bool ClientSupportsOptimizedProtocol);
    /*!
       @brief destructor
     */
    ~OMS_StreamRegistry();
    /*!
       @brief returns the first registered stream
       @param IsInStream [in] true, the first input stream is returned; false the first output stream
     */
    OMS_StreamBody* Begin(bool IsInStream) const;
    /*!
      @brief points behind the last registered stream 
      @param IsInStream [in] true, points behind last input stream; false, points behind last output stream
     */
    const OMS_StreamBody* End(bool IsInStream) const;
    /*!
      @brief enters a stream descriptor into a list of registered streams
      @param IsInStream [in] true, enter into list of input streams; false, enter into list of output streams
     */
    void Enter  (OMS_StreamBody* p, bool IsInStream);
    /*!
      @brief removes a stream descriptor from a list of registered streams
      @param IsInStream [in] true, remove from list of input streams; false, remove from list of output streams
     */
    void Remove (OMS_StreamBody* p);
public :
    bool m_disabled; //!< if true, do not allow any streams to register
    List m_head[2];  //!< two double chained lists of registered streams, the first for input streams, the second for output streams
};

#endif
