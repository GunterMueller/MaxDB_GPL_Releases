/*!
    \file         DBMSrvVol_Volumes.cpp
    \author       TiloH
    \ingroup      volume handling by DBM Server
    \brief        A class for accessing database volumes

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
*/

#include "hcn36.h"
#include "hcn51.h"
#include "hcn52.h"
#include "DBM/Srv/Volumes/DBMSrvVol_AutoExtensionValue.hpp"
#include "DBM/Srv/Volumes/DBMSrvVol_Volumes.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

bool DBMSrvVol_Volumes::calculateNameAndSizeOfNewVolume(
    int                              index,
    DBMSrvVol_Volume::ContentType    contentType,
    bool                             calculateMirror,
    int                              sizeOfAllCurrentVolumes,
    char                          *& name,
    char                          *& nameMirror,
    DBMSrvVol_VolumeSize           & size,
    DBMSrvMsg_Error                & error)
{
    int  lastIndex=0;
    char * previousName=0;
    char * previousMName=0;
    char * previousSize=0;
    char * previousFileVolumeName=0;
    char * previousMFileVolumeName=0;

    bool   rc=getIndexNameAndSizeOfLastAddedVolume(
                contentType,
                calculateMirror,
                lastIndex,
                previousName,
                previousMName,
                previousSize,
                previousFileVolumeName,
                previousMFileVolumeName,
                error);

    if(rc)
    {
        DBMSrvVol_AutoExtensionValue autoExtensionDir(DBMSrvVol_AutoExtensionValue::Directory, contentType, error, rc);

        if(rc)
        {
            rc=calculateNextVolumeName(
                autoExtensionDir.getValue(),
                index,
                lastIndex,
                previousName,
                previousFileVolumeName,
                name,
                error);
        }
    }

    if(rc && calculateMirror)
    {
        DBMSrvVol_AutoExtensionValue autoExtensionDir(DBMSrvVol_AutoExtensionValue::DirectoryMirrored, contentType, error, rc);

        if(rc)
            rc=calculateNextVolumeName(
                autoExtensionDir.getValue(),
                index,
                lastIndex,
                previousMName,
                previousMFileVolumeName,
                nameMirror,
                error);
    }
    
    if(rc)
        rc=calculateSize(contentType, previousSize, sizeOfAllCurrentVolumes, size, error);

    cn36_StrDealloc(previousName);
    cn36_StrDealloc(previousMName);
    cn36_StrDealloc(previousSize);
    cn36_StrDealloc(previousFileVolumeName);
    cn36_StrDealloc(previousMFileVolumeName);

    return rc;
}

bool DBMSrvVol_Volumes::getIndexNameAndSizeOfLastAddedVolume(
    DBMSrvVol_Volume::ContentType    contentType,
    bool                             isMirrored,
    int                            & index,
    char                          *& name,
    char                          *& nameMirror,
    char                          *& size,
    char                          *& previousFileVolumeName,
    char                          *& previousMFileVolumeName,
    DBMSrvMsg_Error                & error)
{
    DBMSrvMsg_Error oFileError;
    bool           rc=true;
    bool           fileOK=true;
    bool           endOfFile=false;
    bool           foundName=false;
    bool           foundMName=!isMirrored;
    bool           foundSize=false;
    bool           foundLastFileVolumeName=false;
    bool           foundLastMFileVolumeName=!isMirrored;

    int indexLastFileVolume=0;
    int indexLastMFileVolume=0;

    const char * parameterForName=PAN_DATADEV_N;
    const char * parameterForMName=0;
    const char * parameterForSize=PAN_DATADEV_N_SIZE;
    const char * parameterForType=PAN_DATADEV_N_TYPE;
    const char * parameterForMType=0;

    if(DBMSrvVol_Volume::Log==contentType)
    {
        parameterForName=PAN_ARCHIVE_LOG_N;
        parameterForSize=PAN_ARCHIVE_LOG_N_SIZE;
        parameterForType=PAN_ARCHIVE_LOG_N_TYPE;
        
        if(isMirrored)
        {
            parameterForMName=PAN_M_ARCHIVE_LOG_N;
            parameterForMType=PAN_M_ARCHIVE_LOG_N_TYPE;
        }
    }

    index=0;

    cn52ParamHistoryRecord Record(cn00DBMServerData::vcontrol()->dbname);

    fileOK = Record.readFirst(oFileError, endOfFile);

    while(rc && fileOK && !endOfFile && (!foundName || !foundMName || !foundSize || !foundLastFileVolumeName || !foundLastMFileVolumeName))
    {
        if(cn52ParamHistoryRecord::StChanged==Record.getState() ||
           cn52ParamHistoryRecord::StActive==Record.getState()    )
        {
            //look for name and size of last added
            if(0==strncmp(parameterForName, Record.getName().asCharp(), strlen(parameterForName))) //is current parameter a volume name
            {
                if(getOrCheckIndex(index, Record.getName().asCharp()+strlen(parameterForName)))
                    rc=allocCopyOnce(foundName, name, Record.getNewValue(), error);

                if(checkIndex(indexLastFileVolume, Record.getName().asCharp()+strlen(parameterForName))) //use volume name only, if we found type F before
                    rc=allocCopyOnce(foundLastFileVolumeName, previousFileVolumeName, Record.getNewValue(), error);
            }
            else
            if(!foundMName &&
               0!=parameterForMName &&
               0==strncmp(parameterForMName, Record.getName().asCharp(), strlen(parameterForMName)))
            {
                if(getOrCheckIndex(index, Record.getName().asCharp()+strlen(parameterForMName)))
                    rc=allocCopyOnce(foundMName, nameMirror, Record.getNewValue(), error);

                if(checkIndex(indexLastMFileVolume, Record.getName().asCharp()+strlen(parameterForMName))) //use mirrored volume name only, if we found type F before
                    rc=allocCopyOnce(foundLastMFileVolumeName, previousMFileVolumeName, Record.getNewValue(), error);
            }
            else
            if(0==strncmp(parameterForSize, Record.getName().asCharp(), strlen(parameterForSize)) &&
               getOrCheckIndex(index, Record.getName().asCharp()+strlen(parameterForSize))           )
            {
                rc=allocCopyOnce(foundSize, size, Record.getNewValue(), error);
            }

            //look for index of last file volume
            if(!foundLastFileVolumeName &&
               0==strncmp(parameterForType, Record.getName().asCharp(), strlen(parameterForType)) &&
               0==stricmp(Record.getNewValue(), "F"))
            {
                indexLastFileVolume=atoi(Record.getName().asCharp()+strlen(parameterForType));
            }

            //look for index of last mirrored file volume
            if(!foundLastMFileVolumeName &&
               0==strncmp(parameterForMType, Record.getName().asCharp(), strlen(parameterForMType)) &&
               0==stricmp(Record.getNewValue(), "F"))
            {
                indexLastMFileVolume=atoi(Record.getName().asCharp()+strlen(parameterForMType));
            }
        }

        if(!foundName || !foundMName || !foundSize || !foundLastFileVolumeName || !foundLastMFileVolumeName)
            fileOK=Record.readNext(oFileError, endOfFile);
    }

    if(!rc)
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
  
    if(rc && !fileOK) //a problem with the file, but not just EOF -> must be something serious
    {
        rc=false;
        error.Overrule(oFileError);
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    }

    if(!foundName || !foundMName || !foundSize)
    {
        rc=false;
        index=0;

        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_VOLLASTADDEDNFOUNDXPHIS,
                                       Msg_Arg(SDBMSGTAG_DBMSRV_VOLLASTADDEDNFOUNDXPHIS__VOLUME_TYPE, DBMSrvVol_Volume::Log==contentType?"LOG":"DATA")));
    }

    return rc;
}

bool DBMSrvVol_Volumes::allocCopyOnce(
    bool            & copiedBefore,
    char           *& string,
    const char      * original,
    DBMSrvMsg_Error & error)
{
    bool rc=true;

    if(!copiedBefore)
    {
        if(!cn36_StrAllocCpy(string, original))
        {
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
            rc=false;
        }
        else
            copiedBefore=true;
    }

    return rc;
}

bool DBMSrvVol_Volumes::getOrCheckIndex(
    int        & index,
    const char * numberInParameterName)
{
    bool rc=false;
    int number=atoi(numberInParameterName); //get the number

    if(0<number) //index is always a positiv number
    {
        if(0==index) //had no real index before
            index=number;

        if(index==number)
            rc=true;
    }

    return rc;
}

bool DBMSrvVol_Volumes::checkIndex(
        int          index,
        const char * numberInParameterName)
{
    bool rc=false;
    int number=atoi(numberInParameterName); //get the number

    if(0<number && index==number) //index is always a positiv number
        rc=true;

    return rc;
}

bool DBMSrvVol_Volumes::calculateNextVolumeName(
    const char      *  autoExtensionDir,
    int                newIndex,
    int                previousIndex,
    const char      *  previousVolumeName,
    const char      *  previousFileVolumeName,
    char            *& nextVolumeName,
    DBMSrvMsg_Error  & error)
{
    bool   rc=true;
    char * newName=0;
    char * newDirectory=0;

    SAPDB_ToStringClass previousIndexAsString(previousIndex);
    SAPDB_ToStringClass newIndexAsString(newIndex);

    // calculate new name
    const char * previousBaseName=cn36_BaseNameFrom(previousVolumeName);
    const char * numberInPreviousBaseName=cn36_StrRStr(previousBaseName, (const char *)previousIndexAsString);

    size_t numberLeadingZeros=0;

    if(0!=numberInPreviousBaseName)
    {
        while(numberInPreviousBaseName!=previousBaseName && '0'==numberInPreviousBaseName[-1])
        {
            numberInPreviousBaseName--;
            numberLeadingZeros++;
        }
    }

    if(0==numberInPreviousBaseName)
    {
        rc=cn36_StrAlloc(newName, strlen(previousBaseName)+1+strlen((const char *)newIndexAsString));

        if(rc)
            sprintf(newName, "%s.%s", previousBaseName, (const char *)newIndexAsString);
    }
    else
    {
        size_t lenNewNumber=strlen((const char *)newIndexAsString);
        size_t lenOldNumberWithLeadingZeros=strlen((const char *)previousIndexAsString)+numberLeadingZeros;
        size_t numberNewLeadingZeros=0;

        if(0!=numberLeadingZeros && lenNewNumber<lenOldNumberWithLeadingZeros)
        {
            numberNewLeadingZeros=lenOldNumberWithLeadingZeros-lenNewNumber;
        }

        SAPDB_ToStringClass newNumberWithLeadingZeros=SAPDB_ToString(newIndex, (int)(numberNewLeadingZeros+lenNewNumber), SAPDB_ToStringClass::right, SAPDB_ToStringClass::showpoint);

        rc=cn36_StrAlloc(newName, strlen(previousBaseName)
                                   -lenOldNumberWithLeadingZeros
                                   +strlen((const char *)newNumberWithLeadingZeros));

        if(rc)
        {
            size_t pos=numberInPreviousBaseName-previousBaseName;

            SAPDB_strncpy(newName, previousBaseName, pos);
            sprintf(newName+pos, "%s%s", (const char *)newNumberWithLeadingZeros, previousBaseName+pos+lenOldNumberWithLeadingZeros);
        }
    }

    //calculate new directory
    if(rc)
    {
        if(0!=autoExtensionDir && '\0'!=autoExtensionDir[0])
        {
            rc=cn36_StrAllocCpy(newDirectory, autoExtensionDir);
        }
        else if(0!=previousFileVolumeName && '\0'!=previousFileVolumeName)
        {
            rc=cn36_StrNAllocCpy(newDirectory, previousFileVolumeName, cn36_EndOfDirectory(previousFileVolumeName)-previousFileVolumeName);
        }
        else
            rc=cn36_StrAllocCpy(newDirectory, ""); //if anything fails, use the rundirectory
    }

    // concatenate path and name
    if(rc)
    {
        if(0!=newDirectory && '\0'!=newDirectory[0])
            rc=cn36_ConstructCompletePath(nextVolumeName, newDirectory, newName);
        else
            rc=cn36_StrAllocCpy(nextVolumeName, newName);
    }

    if(!rc) //so far there can only be out of memory errors
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));

    //free temporary resources
    cn36_StrDealloc(newDirectory);
    cn36_StrDealloc(newName);

    if(rc && RTECONF_MAXSTRINGLENGTH<strlen(nextVolumeName))
    {
        rc=false;
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_ADDVOLNAMETOOLONG,
                                        Msg_Arg(SDBMSGTAG_DBMSRV_ADDVOLNAMETOOLONG__VOLUME_NAME, nextVolumeName),
                                        Msg_Arg(SDBMSGTAG_DBMSRV_ADDVOLNAMETOOLONG__LIMIT, RTECONF_MAXSTRINGLENGTH)));
    }

    return rc;
}

bool DBMSrvVol_Volumes::calculateSize(
    DBMSrvVol_Volume::ContentType    contentType,
    const char                    *  previousSize,
    int                              sizeOfAllCurrentVolumes,
    DBMSrvVol_VolumeSize           & size,
    DBMSrvMsg_Error                & error)
{
    bool rc=true;

    DBMSrvVol_AutoExtensionValue autoExtSize(DBMSrvVol_AutoExtensionValue::Size, contentType, error, rc);

    if(rc)
    {
        if(autoExtSize.isEmpty())
            rc=size.setTo(previousSize, error);
        else
        {
            if(autoExtSize.endsWithPercentageSign())
            {
                rc=size.setTo(autoExtSize.getValue(),
                            autoExtSize.getPositionOfLastPercentageSign(),
                            sizeOfAllCurrentVolumes,
                            error); //calculate size as percentage of size all currently existing volumes
            }
            else
                rc=size.setTo(autoExtSize.getValue(), error);

            if(!rc)
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_ILLEGALVOLSIZE,
                    Msg_Arg(SDBMSGTAG_DBMSRV_ILLEGALVOLSIZE__VALUE, autoExtSize.getValue()),
                    Msg_Arg(SDBMSGTAG_DBMSRV_ILLEGALVOLSIZE__DBMSRV_CONFIGVAR, autoExtSize.getName())));
            }
        }
    }

    return rc;
}
