/*!
  @file    Msg_ProtConv.cpp
  @author  Robin
  @ingroup Messages
  @brief   Message file processing class
  
  processing of message list files (pseudo-xml to well-formed xml)

  -------------------------------------------------------------------------

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

#include "Messages/Msg_FileProcessor.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"
#include "heo01.h"
#include "SAPDBCommon/SAPDB_StandardArgumentParser.hpp"
#include "Messages/Msg_ProtFileInfo.hpp"
#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "KernelCommon/Kernel_Version.hpp"
#include <stdio.h>
#include <stdlib.h>


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  @class ProtConvArgParser
  @brief helper class for argument parsing of protconv
 */
class  ProtConvArgParser : public SAPDB_StandardArgumentParser
{
public:
    /*!
        @brief  ctor
        @param  argc    [in]    the argc of main()
        @param  argv    [in]    the argv of main()
    */
    ProtConvArgParser(  const SAPDB_Int4            argc,
                        const RTE_ArgChar* const*   argv );

    enum { optID_Help, optID_DBName, optID_SortMode, optID_OutputFormat, optID_OutputFile, optID_FileKey, optID_Path, optID_Verbose };

    /*!
        @brief  get the database name specified in the command line arguments
        @return the database name 
    */
    RTE_DBName &                GetDBName()             { return m_DBName; }

    /*!
        @brief  get the format specified in the command line arguments
        @return the format
    */
    Msg_FileProcessor::Format   GetFormat()             { return m_Format; }

    /*!
        @brief  get the destination file name specified in the command line arguments
        @return the destination file name 
    */
    RTE_Path &                  GetDestinationFileName(){ return m_destinationFileName; }

    /*!
        @brief  get the path specified in the command line arguments
        @return the path
    */
    RTE_Path &                  GetPath()               { return m_Path; }

    /*!
        @brief  indicate whether verbose output was specified in the command line arguments
        @return true if verbose output was specified, false if not
    */
    SAPDB_Bool                  Verbose()               { return m_Verbose;}

    /*!
        @brief  get the file tag specified in the command line arguments
        @return the file tag 
    */
    Msg_ProtFileInfo           &GetFileInfo()           { return m_fileTag;}
protected:
    virtual SAPDB_Bool HandleNextOption ( const SAPDB_Int4       optionID,
                                          const RTE_ArgChar*     optionArgument = 0 );

    virtual SAPDB_Bool ParseFinished    ( const RTE_ArgChar* const*  argvRemaining,
                                          SAPDB_Int4                 argsRemaining,
                                          SAPDB_Int4                 argsParsed,
                                          SAPDB_Bool                 breakOptionFound );
private:
    RTE_DBName                  m_DBName;
    Msg_FileProcessor::Format   m_Format;
    RTE_Path                    m_destinationFileName;
    RTE_Path                    m_Path;
    SAPDB_Bool                  m_Verbose;
    Msg_ProtFileInfo            m_fileTag;
};

/*---------------------------------------------------------------------------*/

/*! @brief option array */
static ProtConvArgParser::OptionDesc optionDesc[] = 
       {{ ProtConvArgParser::optID_Help,            RTE_ArgChar('h'),      (RTE_ArgChar*)("help"),           OptionArgumentIsOptional | OptionIsOptional,  (RTE_ArgChar*)("[<Option> | LONG]"),   (RTE_ArgChar*)("help") }
       ,{ ProtConvArgParser::optID_Help,            RTE_ArgChar('?'),      (RTE_ArgChar*)("help"),           OptionArgumentIsOptional | OptionIsOptional,  (RTE_ArgChar*)("[<Option> | LONG]"),   (RTE_ArgChar*)("help") }
       ,{ ProtConvArgParser::optID_DBName,          RTE_ArgChar('d'),      (RTE_ArgChar*)("DBName"),         OptionHasAnArgument | OptionIsOptional,       (RTE_ArgChar*)("<DBName>"),            (RTE_ArgChar*)("name of the database") }
       ,{ ProtConvArgParser::optID_OutputFile,      RTE_ArgChar('o'),      (RTE_ArgChar*)("OutputFile"),     OptionHasAnArgument | OptionIsOptional,       (RTE_ArgChar*)("<OutputFile>"),        (RTE_ArgChar*)("output file") }
       ,{ ProtConvArgParser::optID_OutputFormat,    RTE_ArgChar('f'),      (RTE_ArgChar*)("OutputFormat"),   OptionHasAnArgument | OptionIsOptional,       (RTE_ArgChar*)("<OutputFormat>"),      (RTE_ArgChar*)("output format (classic(default)|plain)") }
       ,{ ProtConvArgParser::optID_FileKey,         RTE_ArgChar('t'),      (RTE_ArgChar*)("FileKey"),        OptionHasAnArgument | OptionIsOptional,       (RTE_ArgChar*)("<FileKey>"),           (RTE_ArgChar*)("file key") }
       ,{ ProtConvArgParser::optID_Path,            RTE_ArgChar('p'),      (RTE_ArgChar*)("Path"),           OptionHasAnArgument | OptionIsOptional,       (RTE_ArgChar*)("<InputFilePath>"),     (RTE_ArgChar*)("path to read input files from") }
       ,{ ProtConvArgParser::optID_SortMode,        RTE_ArgChar('s'),      (RTE_ArgChar*)("SortMode"),       OptionHasAnArgument | OptionIsOptional,       (RTE_ArgChar*)("<SortMode>"),          (RTE_ArgChar*)("sort mode (!ignored!)") }
       ,{ ProtConvArgParser::optID_Verbose,         RTE_ArgChar('v'),      (RTE_ArgChar*)("Verbose"),        OptionIsOptional,                             (RTE_ArgChar*)(""),                    (RTE_ArgChar*)("display message description from supplied files") }
       };

/*---------------------------------------------------------------------------*/

ProtConvArgParser::ProtConvArgParser (  const SAPDB_Int4            argc,
                                        const RTE_ArgChar* const*   argv )
    :SAPDB_StandardArgumentParser ( argc, argv, ProtConvArgParser::optID_Help, optionDesc, NUM_OF_OPTIONS_DESC(optionDesc) )
    ,m_Format(Msg_FileProcessor::Classic)
    ,m_Verbose(false)
{
    m_DBName[0] = '\0';
    m_destinationFileName[0] = '\0';
    m_Path[0]   = '\0';
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool ProtConvArgParser::HandleNextOption (    const SAPDB_Int4       optionID,
                                                    const RTE_ArgChar*     optionArgument )
{
    SAPDB_Bool   ok = true;
    Msg_List errList;

    switch (optionID)
    {
        case optID_DBName:
            strncpy ( m_DBName, optionArgument, sizeof(m_DBName) - 1 );
            m_DBName[sizeof(m_DBName) - 1] = '\0';
            break;

        case optID_OutputFormat:
            if( 0 == strcmp(optionArgument,"plain"))
            {
                m_Format = Msg_FileProcessor::Plain;
            }
            else if( 0 == strcmp(optionArgument,"xml"))
            {
                m_Format = Msg_FileProcessor::Xml;
            }
            else
            {
                m_Format = Msg_FileProcessor::Classic;
            }
            break;

        case optID_OutputFile:
            SAPDB_strncpy_zero_terminated(m_destinationFileName,optionArgument,sizeof(RTE_Path));
            break;

        case optID_FileKey:
            m_fileTag = Msg_ProtFileInfo::ByTagName((SAPDB_Char *)optionArgument);
            break;

        case optID_Path:
            SAPDB_strncpy_zero_terminated(m_Path,optionArgument,sizeof(m_Path));
            break;

        case optID_Verbose:
            m_Verbose = true;
            break;

        case optID_SortMode:
            break;

        default:
            printf ( ("Unknown option identfier '%d'\n"), optionID  );
            ok = false;
            break;
    }
    return ok;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool ProtConvArgParser::ParseFinished( const RTE_ArgChar* const*  argvRemaining,
                                             SAPDB_Int4                 argsRemaining,
                                             SAPDB_Int4                 argsParsed,
                                             SAPDB_Bool                 breakOptionFound ) 
{ 
    if ( (0 == argsParsed && 0 == argsRemaining) || argsRemaining > 1 )
    {
        Help();
        return false;
    }
    if ( 1 == argsRemaining )
    {
        RTE_Filename fileName;
        SAPDB_strncpy_zero_terminated(fileName,(const char *)argvRemaining[0],sizeof(RTE_Filename));
        SAPDB_Char *lastDotInArgument = strrchr(fileName,'.');
        if( lastDotInArgument )
        {
            if  (  0 == strcmp(lastDotInArgument,SAPDB_OUTPUTFILE_STARTING_PHASE_SUFFIX)
                || 0 == strcmp(lastDotInArgument,SAPDB_OUTPUTFILE_RUNNING_PHASE_SUFFIX)
                || 0 == strcmp(lastDotInArgument,SAPDB_OUTPUTFILE_TERMINATING_PHASE_SUFFIX)
                || 0 == strcmp(lastDotInArgument,SAPDB_OUTPUTFILE_COMPLETE_SUFFIX))
            {
                *lastDotInArgument = 0;
            }
        }
        m_fileTag = Msg_ProtFileInfo::ByFileName(fileName);
    }
    return true;
}

/*!
    @brief  helper function to a complete path for a given database name and protocol file name
*/
static SAPDB_Bool BuildFilePath(RTE_DBName &DBName,const SAPDB_Char *fileName,RTE_Path filePath)
{
    const SAPDB_Char *pDBName = (const SAPDB_Char *)&DBName;
    Msg_List err;
    RTEConf_Parameter parameterSet( pDBName );
    if(!parameterSet.GetRundirectoryAbsolutePath( (RTEConf_Parameter::String)filePath,err ))
    {
        return false;
    }

    SAPDB_Char delimiter[2]={PATH_DELIMITER_EO01,0};
    if(strlen(filePath)+strlen(delimiter)+strlen(fileName) > sizeof(RTE_Path))
    {
        return false;
    }
    else
    {
        strcat(filePath,delimiter);
        strcat(filePath,fileName);
        return true;
    }
}


/*!
    @brief  helper function to a complete path for a given path begin and protocol file name
*/
static SAPDB_Bool BuildFilePath(const RTE_Path &path,const SAPDB_Char *fileName,RTE_Path filePath)
{
    SAPDB_strncpy(filePath,path,sizeof(RTE_Path));
    if(filePath[strlen(filePath)-1] != PATH_DELIMITER_EO01)
    {
        SAPDB_Char delimiter[2]={PATH_DELIMITER_EO01,0};
        if(strlen(filePath)+strlen(delimiter) > sizeof(RTE_Path))
        {
            return false;
        }
        strcat(filePath,delimiter);
    }
    if(strlen(filePath)+strlen(fileName) > sizeof(RTE_Path))
    {
        return false;
    }
    else
    {
        strcat(filePath,fileName);
        return true;
    }
}

/*---------------------------------------------------------------------------*/

/*!
    @brief  helper function to tell if a given database writes its protocol files in the modern way (KnlMsg/KnlMsgArchive vs. knldiag.start/klndiag.run/knldiag.stop/knldiag_err.comp)
*/
static SAPDB_Bool IsAModernDatabase(RTE_DBName &dbName)
{
    if( 0 == dbName[0] )    // if no database name is given, assume a modern database
    {
        return true;
    }
    else
    {
        tsp00_C256c  kernelversionFromParamFile;
        tsp00_ErrText   errText;
        RTEConf_ParamaterAccessGetParam  ( *((tsp00_DbNamec *)&dbName)      /*const tsp00_DbNamec &DBNameC */,
                                        *((tsp00_C256c *)KERNELPARAM_KERNELVERSION) /*const tsp00_C256c   &XpKeyC*/,
                                        kernelversionFromParamFile       /*tsp00_C256c   &XpValueC*/,
                                        errText                          /*tsp00_ErrText &ErrText */);
        Kernel_Version version = Kernel_Version(*(Kernel_Version::VersionString *)&kernelversionFromParamFile);
        if( ( version.MajorVersion() > 7 ) || (( version.MajorVersion() == 7 ) &&
                ( ( version.MinorVersion() > 7 ) || ((version.MinorVersion() == 7 ) &&
                    ( ( version.CorrectionLevel() > 0 ) || (version.CorrectionLevel() == 0 ) &&
                        ( version.BuildNo() >= 1 ))))))
        {
            return true;
        }
        if( ( version.MajorVersion() > 7 ) || (( version.MajorVersion() == 7 ) &&
                ( ((version.MinorVersion() == 6 ) &&
                    ( ( version.CorrectionLevel() > 0 ) || (version.CorrectionLevel() == 0 ) &&
                        ( version.BuildNo() >= 16 ))))))
        {
            return true;
        }
    }
    return false;
}


/*---------------------------------------------------------------------------*/
#include "hcn42.h"
/*! define not found in hcn42.h" */
#define FGET_NEWKNLMSG_CN42        (*(tsp00_Pathc *) "KNLMSG"    )

/*!
    @brief  guess what...
*/
int main(int argc,char **argv)
{
    ProtConvArgParser ArgParser( argc,argv );

    RTE_Filename fileName = {0};
    SAPDB_Bool argsPassedSuccessful = ArgParser.StartParsing();
    Msg_ProtFileInfo fileInfo;

    if(argsPassedSuccessful)
    {
        RTE_Path filePath;
        SAPDB_Bool filePathBuilt = false; 
        if(!fileName[0])
        {
            fileInfo = ArgParser.GetFileInfo();
            if(fileInfo.GetTypeString() && ( ( 0 == strcmp(fileInfo.GetTypeString(),FGET_KNLDIAG_CN42 ) 
                                           ||( 0 == strcmp(fileInfo.GetTypeString(),FGET_KNLMSG_CN42 ) ))))
            {
                if(IsAModernDatabase(ArgParser.GetDBName()))
                {
                    fileInfo = Msg_ProtFileInfo::ByTagName(MSG_PROTFILEINFO_KNLMSG);
                }

            }
            if(fileInfo.GetTypeString() && ( ( 0 == strcmp(fileInfo.GetTypeString(),FGET_KNLDIAGOLD_CN42 ) 
                                           ||( 0 == strcmp(fileInfo.GetTypeString(),FGET_KNLMSGOLD_CN42 ) ))))
            {
                if(IsAModernDatabase(ArgParser.GetDBName()))
                {
                    fileInfo = Msg_ProtFileInfo::ByTagName(MSG_PROTFILEINFO_KNLMSGOLD);
                }
            }
            if(fileInfo.GetTypeString() && ( ( 0 == strcmp(fileInfo.GetTypeString(),FGET_KNLDIAGERR_CN42 ) 
                                           ||( 0 == strcmp(fileInfo.GetTypeString(),FGET_KNLMSGERR_CN42 ) ))))
            {
                if(IsAModernDatabase(ArgParser.GetDBName()))
                {
                    fileInfo = Msg_ProtFileInfo::ByTagName(MSG_PROTFILEINFO_KNLMSGARCHIVE);
                }
            }
            fileInfo.GetCompleteFileName(fileName,ArgParser.GetDBName());
            if( 0 == fileName[0])
            {
                SAPDB_strncpy_zero_terminated(fileName , "knldiag",sizeof(fileName));
            }
        }
        if(ArgParser.GetDBName()[0])
        {
            filePathBuilt = BuildFilePath(ArgParser.GetDBName(),fileName,filePath);
            if(ArgParser.GetPath()[0])
            {
                printf("don't mix -d and -p!\n");
                exit(0);
            }
        }
        else if(ArgParser.GetPath()[0])
        {
            filePathBuilt = BuildFilePath(ArgParser.GetPath(),fileName,filePath);
        }
        else
        {
            SAPDB_strncpy_zero_terminated(filePath,fileName,sizeof(RTE_Path));
            filePathBuilt = true;
        }

        if(filePathBuilt)
        {
            Msg_FileProcessor fileProcessor(filePath,fileInfo.GetStyle(),fileInfo.LongColumns());
            RTE_Path &destinationFileName = ArgParser.GetDestinationFileName();
            Msg_List errList;
            if(destinationFileName[0]) 
                fileProcessor.CreateDestinationFile(destinationFileName,errList);
            fileProcessor.BuildWellFormedXMLFile(errList,Msg_FileProcessor::MessageListsByNo,ArgParser.GetFormat(),Msg_FileProcessor::Classic == ArgParser.GetFormat()/*classic header*/,ArgParser.Verbose());
        }
    }
}

