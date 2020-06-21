/*!
  @file           DBMCliEvtDisp_Properties.hpp
  @author         MarcW
  @brief

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef DBMCliEvtDisp_Properties_hpp
#define DBMCliEvtDisp_Properties_hpp

#include "ToolsCommon/Tools_Properties.hpp"

/*!
    @brief humble extension to be able to read properties that have '=' chars in the key

    '=' chars that are enclosed in '(' and ')' brackets are ignored. The first '=' that is
    not enclosed, is considered to be the separator between key and value of the property
*/
class DBMCliEvtDisp_Properties: public Tools_Properties {
public:
    DBMCliEvtDisp_Properties();
    DBMCliEvtDisp_Properties( const Tools_DynamicUTF8String& rawfilename );

    ~DBMCliEvtDisp_Properties() {}

    static bool migrateFormat(Msg_List& errOut, const char* rawfilename);
    static const char* const FileVersionKey;

private:
    Tools_Property* createProperty( const char* line) const;
};

#endif //DBMCliEvtDisp_Properties_hpp
