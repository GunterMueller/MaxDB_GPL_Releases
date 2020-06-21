/*!
    \file    DBMSrvBackCertFill_Output.hpp
    \author  TiloH
    \ingroup sdbfill program
    \brief   declaration of a class handling the output of sdbfill

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

#if !defined DBMBACKCERTFILL_OUTPUT_HPP
#define DBMBACKCERTFILL_OUTPUT_HPP

class DBMBackCertFill_Output
{
  public:
    enum OutputStyle
    {
        NoOutput,
        Ascii,
        Binary,
        Error
    };

    DBMBackCertFill_Output(OutputStyle newStyle=DBMBackCertFill_Output::NoOutput);

    void setStyle(OutputStyle newStyle);

    void write(
        const OutputStyle   style,
        const char        * text,
        const char        * value1=0,
        const char        * value2=0,
        const char        * value3=0) const;

  private:
    const char * check(const char * string) const;

    OutputStyle m_Style;
};

#endif
