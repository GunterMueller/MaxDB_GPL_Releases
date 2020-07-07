/*! 
  -----------------------------------------------------------------------------
 
  module: DBMSrvBHist_LineProducer.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  TiloH
 
  special area: backup history handling by the DBMServer
   
  description:  implementing a class supplying basic functions for creating
                formated backup history lines

  version:      7.4.4 and higher 
  -----------------------------------------------------------------------------
 
                          Copyright (c) 2002-2005 SAP AG
 
  -----------------------------------------------------------------------------



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

//
//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//

#include <string.h>
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_LineProducer.hpp"
#include "SAPDBCommon/SAPDB_string.h"


//
//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------
//

const char * DBMSrvBHist_LineProducer::ColumnSeparator="|";
const size_t DBMSrvBHist_LineProducer::ColumnSeparatorLength=strlen(DBMSrvBHist_LineProducer::ColumnSeparator);


//
//-----------------------------------------------------------------------------
// members class DBMSrvBHist_LineProducer
//-----------------------------------------------------------------------------
//

size_t DBMSrvBHist_LineProducer::LengthOfPart(const char * Part,
                                              size_t       WishedLength)
{
    size_t rc=strlen(Part);

    if(rc<WishedLength)  //parts will be filled up with spaces, if necessary
        rc=WishedLength;

    rc+=ColumnSeparatorLength; //parts end with a separator

    return rc;
}

void DBMSrvBHist_LineProducer::AppendFilledUpPart(char       *& Destination,
                                                  const char *  Part,
                                                  size_t        WishedLength,
                                                  size_t      & MaxLengthOfDestination,
                                                  Alignment     PartAlignment)
{
    size_t PartLength=strlen(Part);

    //print needed spaces of right aligned columns
    if(Right==PartAlignment && PartLength<WishedLength)
        InsertSpaces(Destination, MaxLengthOfDestination, WishedLength-PartLength);

    //print the value
    SAPDB_strcpy(Destination, Part);

    MaxLengthOfDestination-=PartLength;
    Destination+=PartLength;

    //print needed spaces of left aligned columns
    if(Left==PartAlignment && PartLength<WishedLength)
        InsertSpaces(Destination, MaxLengthOfDestination, WishedLength-PartLength);

    //add a column separator
    SAPDB_strcpy(Destination, ColumnSeparator);

    MaxLengthOfDestination-=ColumnSeparatorLength;
    Destination+=ColumnSeparatorLength;
};

void DBMSrvBHist_LineProducer::InsertSpaces(char *& Destination, size_t & MaxLengthOfDestination, size_t NumberOfSpaces)
{
    memset(Destination, ' ', NumberOfSpaces);
    Destination[NumberOfSpaces]='\0';

    MaxLengthOfDestination-=NumberOfSpaces;
    Destination+=NumberOfSpaces;
}
