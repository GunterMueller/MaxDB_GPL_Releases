/*!
    \file    DBMVI_Strings.hpp
    \author  MarcW
    \ingroup version information strings
    \brief   to encapsulate setbuildinfo make description

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

#ifndef DBMVI_Strings_hpp
#define DBMVI_Strings_hpp

/*!
    @brief version inforamation strings
*/
class DBMVI_Strings {
public:
    static const char* const BuildNumberString;
    static const char* const BuildPrefix;
    static const char* const CompanyName;
    static const char* const CopyRightString;
    static const char* const CorrectionLevel;
    static const char* const Formated3BuildPrefix;
    static const char* const FormatedBuildPrefix;
    static const char* const FormatedBuildVersion;
    static const char* const FormatedCorrectionLevel;
    static const char* const MajorVersion;
    static const char* const MinorVersion;
    static const char* const ProductName;

private:
    DBMVI_Strings() {}
};
#endif //DBMVI_Strings_hpp
