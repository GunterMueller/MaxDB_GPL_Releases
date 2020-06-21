/*!*********************************************************************

    @file         SAPDBFields_PropertyList.cpp

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

#include "SAPDBCommon/Fields/SAPDBFields_PropertyList.hpp"

#include <string.h>

SAPDBFields_PropertyList::Reader::
Reader (
    const void * buf)
: innerReader (buf)
{
}

/*----------------------------------------*/

bool SAPDBFields_PropertyList::Reader::
next (
    Entry & entry)
{
    const void * tmp;

    if (!this->innerReader.next (tmp, entry.namelen)) {
        return false;
    }
    entry.name = static_cast<const char *> (tmp);
    if (!this->innerReader.next (tmp, entry.valuelen)) {
        return false;
    }
    entry.value = static_cast<const char *> (tmp);
    return true;
}

/*----------------------------------------*/

SAPDBFields_PropertyList::Writer::
Writer (
    void * buf,
    int bufsize)
: writerObject (buf, bufsize)
{
    this->innerWriter = & this->writerObject;
}

/*----------------------------------------*/

SAPDBFields_PropertyList::Writer::
Writer (
    SAPDBFields_VarData::Writer * writer)
: writerObject (0, 0)
{
    this->innerWriter = writer;
}

/*----------------------------------------*/

void SAPDBFields_PropertyList::Writer::
close (
    int & fieldCount,
    int & buflen)
{
    this->innerWriter->close (fieldCount, buflen);
}

/*----------------------------------------*/

bool SAPDBFields_PropertyList::Writer::
addEntry (
    const char * name,
    int namelen,
    const char * value,
    int valuelen)
{
    if (!this->innerWriter->addField (name, namelen)) {
        return false;
    }
    if (!this->innerWriter->addField (value, valuelen)) {
        return false;
    }
    return true;
}

/*----------------------------------------*/

bool SAPDBFields_PropertyList::Writer::
addEntry (
    const char * name,
    const char * value)
{
    return this->addEntry (name, strlen (name), value, strlen (value));
}

/*----------------------------------------*/

bool SAPDBFields_PropertyList::Writer::
addEntry (
    const char * name,
    int namelen,
    const char * value)
{
    return this->addEntry (name, namelen, value, strlen (value));
}

/*----------------------------------------*/

bool SAPDBFields_PropertyList::Writer::
addEntry (
    const char * name,
    const char * value,
    int valuelen)
{
    return this->addEntry (name, strlen (name), value, valuelen);
}

/*----------------------------------------*/

