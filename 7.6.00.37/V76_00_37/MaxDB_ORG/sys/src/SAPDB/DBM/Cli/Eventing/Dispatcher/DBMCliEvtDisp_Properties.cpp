/*!
  @file           DBMCliEvtDisp_Properties.cpp
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

#include <stdlib.h>
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp"
#include "DBM/VersionInfo/DBMVI_Strings.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"
#include "hsp100.h"

DBMCliEvtDisp_Properties::DBMCliEvtDisp_Properties()
        : Tools_Properties() {
}

DBMCliEvtDisp_Properties::DBMCliEvtDisp_Properties( const Tools_DynamicUTF8String& rawfilename )
        : Tools_Properties(rawfilename) {
}

Tools_Property* DBMCliEvtDisp_Properties::createProperty( const char* line) const {
    // just from creation of the property from the line
    Tools_DynamicUTF8String inLine(line);
    if( inLine.Length() <=1 )
        return NULL;

    Tools_DynamicUTF8String::BasisElementIndex bracketPos(inLine.Find('('));
    if( bracketPos != Tools_DynamicUTF8String::NPos ) {
        Tools_DynamicUTF8String::BasisElementIndex equalsPos(inLine.Find('='));
        
        if( equalsPos < bracketPos )
            return Tools_Properties::createProperty(line);
        else {
            int count(1);
            while( count > 0) {
            bracketPos = inLine.FindFirstOf("()", bracketPos+1);
            if( inLine[bracketPos] == '(')
                ++count;
            else
                --count;
            }
            equalsPos = inLine.Find("=", bracketPos+1);
            if( equalsPos == Tools_DynamicUTF8String::NPos )
                return NULL;
            else
                return new Tools_Property(
                        inLine.SubStr(0, equalsPos).CharPtr(),
                        inLine.SubStrElem(equalsPos+1).CharPtr());
        }

    }
    else
        return Tools_Properties::createProperty(line);
}

bool DBMCliEvtDisp_Properties::migrateFormat(Msg_List& errOut, const char* rawfilename) {
    // laden
    Tools_DynamicUTF8String file(rawfilename);
    Tools_DynamicUTF8String fileVersionString;
    DBMCliEvtDisp_Properties migrant(file);
   
    bool success(false);
    do {
        if( !migrant.load(errOut) )
            break;

        Tools_DynamicUTF8String versionKey(FileVersionKey);
        fileVersionString = migrant.getProperty(versionKey);
        if( fileVersionString.Empty() )
            break;

        // check the version id
        // compile the file version id
        tsp100_VersionID1 fileVersionID;
        Tools_DynamicUTF8String::BasisElementIndex from(0);
        Tools_DynamicUTF8String::BasisElementIndex end(0);
        Tools_DynamicUTF8String versionPart;
        end = fileVersionString.Find('.', from);
        versionPart = fileVersionString.SubStr(from, end-from);
        fileVersionID.MajorVersion_sp100 = (unsigned char)(atoi(versionPart.CharPtr()));
        from = end+1;
        end = fileVersionString.Find('.', from);
        versionPart = fileVersionString.SubStr(from, end-from);
        fileVersionID.MinorVersion_sp100 = (unsigned char)(atoi(versionPart.CharPtr()));
        from = end+1;
        end = fileVersionString.Find('.', from);
        versionPart = fileVersionString.SubStr(from, end-from);
        fileVersionID.CorrLevel_sp100 = (unsigned char)(atoi(versionPart.CharPtr()));
        from = end+1;
        versionPart = fileVersionString.SubStr(from);
        fileVersionID.BuildNumberPrefix_sp100 = (unsigned char)(atoi(versionPart.CharPtr()));

        // compile the code version id
        tsp100_VersionID1 codeVersionID;
        sp100_GetVersionID(VersionIDType1_esp100, s100buildnumberEx, &codeVersionID);

        // compare them
        tsp100_VersionResult compareResult;
        compareResult = sp100_Compare2VersionIDs(VersionIDType1_esp100, &fileVersionID, &codeVersionID);
        if( compareResult == IsOlder_esp100 ) {
            // the file is older, we can migrate
            // not much at the moment...
            migrant.setProperty(versionKey, Tools_DynamicUTF8String(DBMVI_Strings::FormatedBuildVersion));
            if( !migrant.store(errOut) )
                break;
        }
        else if( compareResult == IsNewer_esp100 ) {
            // the file is newer, we cannot migrate
            errOut.Overrule(
                Msg_List(Msg_List::Error,
                SDBMSG_DBMED_CONFIGFORMAT,
                Msg_Arg("DISPCONFIG", rawfilename),
                Msg_Arg("FMT_BUILD_VERSION",fileVersionString.CharPtr())));
            break;
        }
        else {
            // version is okay, do nothing
        }
        
        success = true;
        break;
    } while (true);

    if( !success ) {
        errOut.Overrule(
            Msg_List(Msg_List::Error,
            SDBMSG_DBMED_CONFIGMIGRATION,
            Msg_Arg("DISPCONFIG", rawfilename),
            Msg_Arg("FMT_BUILD_VERSION",fileVersionString.CharPtr())));
    }
    return success;
}

const char* const DBMCliEvtDisp_Properties::FileVersionKey("FILEVERSION");
