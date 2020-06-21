/*!
    \file    DBMSrvBackCertFill_Output.cpp
    \author  TiloH
    \ingroup sdbfill program
    \brief   implemeting the class handling the output of sdbfill

    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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

#include <stdio.h>
#include "DBM/Backint/Certification/SDBFill/DBMBackCertFill_Output.hpp"


DBMBackCertFill_Output::DBMBackCertFill_Output(OutputStyle newStyle)
    :m_Style(newStyle)
{
}

void DBMBackCertFill_Output::setStyle(OutputStyle newStyle)
{
    m_Style=newStyle;
}

void DBMBackCertFill_Output::write(
    const OutputStyle   style,
    const char        * text,
    const char        * value1,
    const char        * value2,
    const char        * value3) const
{
    if(DBMBackCertFill_Output::Error==style)
        fprintf(stderr, text, check(value1), check(value2), check(value3));
    else
        if(style==m_Style)
        {
            printf(text, check(value1), check(value2), check(value3));
            fflush(stdout);
        }
}

const char * DBMBackCertFill_Output::check(const char * string) const
{
    return string?string:"";
}
