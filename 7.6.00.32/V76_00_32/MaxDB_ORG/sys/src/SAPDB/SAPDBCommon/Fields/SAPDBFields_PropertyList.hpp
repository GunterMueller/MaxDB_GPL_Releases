/*!*********************************************************************

    @file         SAPDBFields_PropertyList.hpp

    @author       DanielD

    @brief        a SAPDBFields_VarData list consisting of pairs of
                    property name / property value

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#ifndef SAPDBFIELDS_PROPERTYLIST_HPP
#define SAPDBFIELDS_PROPERTYLIST_HPP

#include "SAPDBCommon/Fields/SAPDBFields_VarData.hpp"

/*!

  @class SAPDBFields_PropertyList
  @brief like SAPDBFields_VarData, but pairs ot name / value
         are considered one entry

 */
class SAPDBFields_PropertyList
{
public:
    SAPDBFields_PropertyList ();

    typedef struct Entry {
        const char * name;
        int          namelen;
        const char * value;
        int          valuelen;
    } Entry;

    /*!

      @class Reader

     */
    class Reader
    {
    public:
        Reader (const void * buf);

        bool next (Entry & entry);

    private:
        SAPDBFields_VarData::Reader innerReader;
    };

    /*!

      @class Writer

     */
    class Writer
    {
    public:
        Writer (void * buf, int bufsize);

        Writer (SAPDBFields_VarData::Writer * writer);

        void close (int & fieldCount, int & buflen);

        bool addEntry (const char * name, int namelen, const char * value, int valuelen);

        bool addEntry (const char * name, const char * value);

        bool addEntry (const char * name, int namelen, const char * value);

        bool addEntry (const char * name, const char * value, int valuelen);

    private:
        SAPDBFields_VarData::Writer * innerWriter;
        SAPDBFields_VarData::Writer   writerObject;
    };

};

#endif

