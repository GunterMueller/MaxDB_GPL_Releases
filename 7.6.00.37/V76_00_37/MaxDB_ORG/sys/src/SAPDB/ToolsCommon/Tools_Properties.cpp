/*!
    @file           Tools_Properties.cpp
    @author         MarcW
    @brief          reading, storing properties to/from a file - Implementation

\if EMIT_LICENCE

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


\endif
*/

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "heo06.h"
#include "SAPDBCommon/Messages/SDBMsg_ToolsCommon.h"
#include "ToolsCommon/Tools_Properties.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

const ToolsTime_TimeFormat Tools_Properties::m_FileModificationTimeFormat("yyyymmddHHMMSS");
const char Tools_Properties::CommentPrefix('#');
tsp00_C40c Tools_Properties::m_RTETextConverter;

Tools_List<Tools_Property*>* Tools_Properties::getAllProperties()  const {
    Tools_List<Tools_Property*>* vAllProps = new Tools_List<Tools_Property*>;

    Tools_Property* it = first;
    while( it != NULL ) {
        Tools_Property* copy = new Tools_Property(it->getKey(), it->getValue());
        copy->setNext(NULL);
        vAllProps->push_back(copy);
        it = it->getNext();
    }
    return vAllProps;
}

Tools_Properties::Tools_Properties()
        : m_FileLoadedOnce(false),
          first(NULL),
          filename(NULL) {
}


Tools_Properties::Tools_Properties( const Tools_DynamicUTF8String& rawfilename )
        : m_FileLoadedOnce(false),
          first(NULL),
          filename(NULL) {
    filename = getCharPointer(rawfilename);
}

Tools_Properties::~Tools_Properties(){
    if( first != NULL )
        delete first;
    if( filename != NULL )
        delete[] filename;
}

SAPDB_Bool Tools_Properties::deleteStorage(Msg_List &errOut) {

    errOut.ClearMessageList();

    tsp05_RteFileError ferror;
    tsp05_RteFileInfo finfo;

    // nothing to do if there is no file name
    if( filename == NULL )
        return true;

    sqlfinfoc(
            filename,
            &finfo,
            &ferror);

    // nothing to do if the file does not exist
    if( ferror.sp5fe_result == vf_ok && !finfo.sp5fi_exists )
        return true;

    sqlferasec(
        filename,
        &ferror);

    // we could not erase the file for some reason
    if( ferror.sp5fe_result != vf_ok ) {
        errOut.Overrule(
                Msg_List(Msg_List::Error,
	            SDBMSG_TOOLSCOMMON_PROPFILE_DELETION,
	            Msg_Arg("FILENAME", filename),
	            Msg_Arg("ERRORTEXT", m_RTETextConverter.p2c(ferror.sp5fe_text))));
        return false;
    }
    else
        return true;
}

SAPDB_Bool Tools_Properties::load( Msg_List &errOut, SAPDB_Bool lazy, SAPDB_Bool& reloadedNow ) {

    tsp00_Int4 fhandle;
    tsp05_RteFileError ferror;
    tsp05_RteFileInfo finfo;

    // we cannot load if we have no file name
    if( filename == NULL ) {
        errOut.Overrule(
                Msg_List(Msg_List::Error,
	            SDBMSG_TOOLSCOMMON_PROPFILE_NOFILE));
        return false;
    }

    if( lazy && m_FileLoadedOnce ) {
        // check the last loading time and load only if the file was modified after that
        sqlfinfoc(
            filename,
            &finfo,
            &ferror);

        if( ferror.sp5fe_result == vf_ok && finfo.sp5fi_exists ) {
            const size_t timeSize(sizeof(finfo.sp5fi_date_modified) + sizeof(finfo.sp5fi_time_modified) + 1);
            char modificationTime[timeSize];
            strncpy(
                    modificationTime,
                    finfo.sp5fi_date_modified,
                    sizeof(finfo.sp5fi_date_modified));
            strncpy(
                    modificationTime+sizeof(finfo.sp5fi_date_modified),
                    finfo.sp5fi_time_modified,
                    sizeof(finfo.sp5fi_time_modified));
            modificationTime[timeSize-1] = '\0';
            ToolsTime_Time currentFileTime(modificationTime, m_FileModificationTimeFormat);
            if( currentFileTime > m_LastModificationTime ) {
                // we must reload
            }
            else {
                // nothing to do
                reloadedNow = false;
                return true;
            }
        }
    }

    // open file
    sqlfopenc(
        filename,
        sp5vf_text,
        sp5vf_read,
        sp5bk_buffered,
        &fhandle,
        &ferror);
    
    // we could not open the file for some reason
    if( ferror.sp5fe_result != vf_ok ) {
        errOut.Overrule(
        Msg_List(Msg_List::Error,
	        SDBMSG_TOOLSCOMMON_PROPFILE_NOFOPEN,
	        Msg_Arg("FILENAME", filename),
	        Msg_Arg("ERRORTEXT", m_RTETextConverter.p2c(ferror.sp5fe_text))));
        return false;
    }
    
    Tools_Property* p(NULL);

    // read file linewise
    while (ferror.sp5fe_result == vf_ok) {
        
        Tools_DynamicUTF8String line;
        do {
            // collect the line (which may be longer than the buffer)
            char lineBuf[1024];
            tsp00_Longint outLen;
            sqlfreadc(fhandle, lineBuf, sizeof(lineBuf), &outLen, &ferror );
            if( ferror.sp5fe_result == vf_ok )
                line += lineBuf;
        } while(ferror.sp5fe_result == vf_ok && ferror.sp5fe_warning == sp5vfw_no_eol_found); 

        if( ferror.sp5fe_result == vf_ok && line.Length()>0 && line[0]!=CommentPrefix ) {
            //we could read the line and it is not a comment
            Tools_Property* temp(createProperty(line.CharPtr()));
            if( temp != NULL )
                p = insertProperty(p, temp);
        }
        else if (ferror.sp5fe_result != vf_ok) {
            // ...
        }
    }

    // close the file
    sqlfclosec( fhandle, sp5vf_close_normal, &ferror);
    if( ferror.sp5fe_result != vf_ok ) {
        // some error occurred
        // revert all we've done in loop
        if( p != NULL )
            delete p;
        errOut.Overrule(
        Msg_List(Msg_List::Error,
	        SDBMSG_TOOLSCOMMON_PROPFILE_NOFCLOSE,
	        Msg_Arg("FILENAME", filename),
	        Msg_Arg("ERRORTEXT", m_RTETextConverter.p2c(ferror.sp5fe_text))));
        return false;
    } else {
        if( first != NULL )
            // delete the properties that we had before loading the new ones
            delete first;
        first = p;

        // loading successful
        m_FileLoadedOnce = true;
        reloadedNow = true;
        m_LastModificationTime.setTo(); // now
        return true;
    }
}

SAPDB_Bool Tools_Properties::load( Msg_List &errOut, SAPDB_Bool lazy ) {
    SAPDB_Bool neverSeen(false);
    return load(errOut, lazy, neverSeen);
}

SAPDB_Bool Tools_Properties::load( Msg_List &errOut, const Tools_DynamicUTF8String& rawfilename, SAPDB_Bool lazy ) {
    Tools_DynamicUTF8String temp(filename);
    m_FileLoadedOnce = false;
    setFileName( rawfilename );
    if( !load(errOut, lazy) ) {
        // set old file name again
        setFileName( temp );
        return false;
    }
    return true;
}

Tools_Property* Tools_Properties::createProperty( const char* line ) const {
    // points to '=' character
    const char* val = strchr( line, '=' );
    if( val == NULL )
        return NULL;
    // check if there is a kay that makes sense
    if( ((int)(val-line)) <= NULL )
        return NULL;

    val++;
    // now val points behind the '=' character

    // create a NULL-terminated string for the key
    char* key = new char[(int)(val-line)];
    strncpy(key, line, (int)(val-line)-1);

    key[-1 + (int)(val-line)] = 0;
    Tools_Property* p = new Tools_Property( key, val );
    delete[] key;
    return p;
}

SAPDB_Bool Tools_Properties::store(Msg_List &errOut) {
    tsp00_Int4 fhandle;
    tsp05_RteFileError ferror;

    // do nothing if we have no file name
    if( filename == NULL )
        return false;

    // try to open the file for writing, overwrite old content
    sqlfopenc(
        filename,
        sp5vf_text,
        sp5vf_write,
        sp5bk_buffered,
        &fhandle,
        &ferror);
    
    // something went wrong opening
    if( ferror.sp5fe_result != vf_ok ) {
        errOut.Overrule(
            Msg_List(Msg_List::Error,
	        SDBMSG_TOOLSCOMMON_PROPFILE_NOFOPEN,
	        Msg_Arg("FILENAME", filename),
	        Msg_Arg("ERRORTEXT", m_RTETextConverter.p2c(ferror.sp5fe_text))));
        return false;
    }

    Tools_Property* curr = first;

    // for each property, create a line and write it. Key and value
    // are separated by '=' char.
    while( ferror.sp5fe_result == vf_ok && curr != NULL ) {
        Tools_DynamicUTF8String line(curr->getKey());
        line += "=";
        line += curr->getValue();
        // write the line        
        sqlfwritec( fhandle, line.CharPtr(), line.Length(), &ferror);
        curr = curr->getNext();
    }

    sqlfclosec( fhandle, sp5vf_close_normal, &ferror);    
    if( ferror.sp5fe_result != vf_ok ) {
        errOut.Overrule(
            Msg_List(Msg_List::Error,
	        SDBMSG_TOOLSCOMMON_PROPFILE_NOFCLOSE,
	        Msg_Arg("FILENAME", filename),
	        Msg_Arg("ERRORTEXT", m_RTETextConverter.p2c(ferror.sp5fe_text))));
        return false;
    }
    else {
        m_LastModificationTime.setTo();
        m_FileLoadedOnce = true;
        return true;
    }
}

SAPDB_Bool Tools_Properties::store( Msg_List &errOut, const Tools_DynamicUTF8String& rawfilename ) {
    Tools_DynamicUTF8String temp(filename);
    setFileName( rawfilename );
    if( !store(errOut) ) {
        // set old file name again
        setFileName( temp );
        return false;
    }
    return true;
}

void Tools_Properties::setProperty( const Tools_DynamicUTF8String& key, const Tools_DynamicUTF8String& value ) {
    char* pkey = getCharPointer(key);
    char* pvalue =  getCharPointer(value);

    first = insertProperty(first, new Tools_Property( pkey, pvalue ));

    delete[] pkey;
    delete[] pvalue;
}

Tools_DynamicUTF8String Tools_Properties::getProperty( const Tools_DynamicUTF8String& key ) const {
    Tools_DynamicUTF8String tds;
    char* pkey = getCharPointer(key);
    Tools_Property* p = findProperty(first, pkey);
    if( p != NULL )
        tds = _Tools_UTF8Ptr(p->getValue());

    delete[] pkey;
    return tds;
}

void Tools_Properties::removeProperty( const Tools_DynamicUTF8String& key ) {
    char* pkey = getCharPointer(key);
    if( pkey == NULL )
        return;
    
    Tools_Property* p = findProperty(first, pkey);
    if( p == NULL )
        return;

    if( p == first ) {
        if( first->getNext() != NULL ) {
            first = first->getNext();
            p->setNext(NULL);
            delete p;
        } else {
            delete first;
            first = NULL;
        }
    } else {
        Tools_Property* p1 = first;
     
        while( p1->getNext() != p )
            p1 = p1->getNext();

        p1->setNext(p->getNext());
        p->setNext(NULL);
        delete p;
    }
}

Tools_DynamicUTF8String Tools_Properties::getProperty( const Tools_DynamicUTF8String& key, const Tools_DynamicUTF8String& defval ) const {
    Tools_DynamicUTF8String realval(getProperty(key));
    if( realval.Empty() )
        return defval;
    else
        return realval;
}

Tools_Property* Tools_Properties::findProperty(
        Tools_Property* head,
        const char* key ) const {
    for( Tools_Property* curr = head; curr != NULL; curr = curr->getNext() ) {
        if( 0 == strcmp(curr->getKey(), key) )
            return curr;
    }
    return NULL;
}

Tools_Property* Tools_Properties::insertProperty(Tools_Property* start,
                                                 Tools_Property* newp) const {
    if( start == NULL )
        return newp;
    if( newp == NULL )
        return start;  
    const char* newkey = newp->getKey();
    Tools_Property* curr(NULL);

    curr = findProperty(start, newkey);
    if (curr != NULL ) {
        // key already exists
        curr->setValue(newp->getValue());
        delete newp;
        return start;
    }

    // key of new property is smaller than start
    if( strcmp(newkey, start->getKey()) < 0 ) {
        // newp will be predecessor of start
        newp->setNext(start);
        start = newp;
        return start;
    }

    curr = start;
    while( curr != NULL ) {
        Tools_Property* currnext = curr->getNext();
        // check if newp is between curr and currnext
        if( strcmp(newkey,curr->getKey() ) > 0 ) {
            if( currnext == NULL ) {
                // newp will be the last one
                curr->setNext(newp);
                newp->setNext(NULL);
                return start;
            }
            else if( strcmp(newkey, currnext->getKey()) < 0 ) {
                newp->setNext(currnext);
                curr->setNext(newp);
                return start;
            }
        }
        curr = currnext;
    }
    // we should never get here...
    return NULL;
}

void Tools_Properties::setFileName( const Tools_DynamicUTF8String& s ) {
    if( filename != NULL )
        delete[] filename;
    filename = getCharPointer(s);
}

void Tools_Properties::dump(FILE* const f) const {
    char line[8192];
    Tools_Property* curr(first);
    
    // dump file name if there is one
    if( filename != NULL )
        fprintf(f, "properties (%s):\n", filename);
    else
        fprintf(f, "properties (%s):\n", "undefined location");

    // dump properties
    while( curr != NULL ) {
        int keylen = (int)strlen(curr->getKey());
        int vallen = (int)strlen(curr->getValue());
        SAPDB_strcpy(line, curr->getKey() );
        line[keylen] = '=';
        SAPDB_strcpy(line+keylen+1, curr->getValue() );
        fprintf(f, "%s\n", line);
        curr = curr->getNext();
    }
    fprintf(f, "\n");
}

char* Tools_Properties::getCharPointer( const Tools_DynamicUTF8String& s ) const {
    // there seems to be no const method in Tools_DynamicUTF8String that returns the
    // string as a char*...
    char* p = NULL;
    int size = s.Size();
    p = new char[size+1];
    *(p+size) = 0;
    ToolsMeMa_GuardedOperation::MemCopy(p, s.Data(), size, __FILE__, __LINE__);
    return p;
}

int Tools_Properties::getCount() const {
    Tools_Property* curr(first);
    int count(0);
    while( curr != NULL ) {
        ++count;
        curr = curr->getNext();
    }
    return count;
}