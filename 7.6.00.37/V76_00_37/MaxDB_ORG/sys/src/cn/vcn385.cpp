/*******************************************************************************
  module:       vcn385.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  implementation of classes for reporting the errors for Backint
                for SAP DB
  last change:  2000-11-29 16:16
  version:      7.2.5
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2000-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



*******************************************************************************/

#include <stdio.h>

#include "SAPDBCommon/SAPDB_string.h"

#include "hcn36.h"
#include "hcn385.h"

int tcn385_EnumString::CurrentEnumValue=0;

tcn385_EnumString::tcn385_EnumString(const char *s)
  :String(0)
{ 
    if(0!=s)
    {
        String=new char [strlen(s)+1];

        if(0!=String)
            SAPDB_strcpy(String, s);
        else
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
    }

    EnumValue=CurrentEnumValue++;
}

tcn385_EnumString::tcn385_EnumString(const tcn385_EnumString & o)
  :String(0)
{
    if(0!=o.String)
    {
        String=new char [strlen(o.String)+1];

        if(0!=String)
            SAPDB_strcpy(String, o.String);
        else
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
    }

    EnumValue=o.EnumValue;
}


tcn385_EnumString::~tcn385_EnumString()
{
    if(0!=String)
        delete [] String;
}

const char * tcn385_EnumString::GiveString() const
{
    return String;
}

int tcn385_EnumString::GiveEnumValue() const
{
    return EnumValue;
}


const tcn385_EnumString tcn385_Backint2Error::OutOfMemory("Out of memory.");

const tcn385_EnumString tcn385_Backint2Error::CouldNotOpenParameterFile("Could not open the parameter file.");

const tcn385_EnumString tcn385_Backint2Error::SyntaxErrorStage("A stage is not defined correctly in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::SyntaxErrorSize("A size of a stage is not defined correctly in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::SyntaxErrorNumberFilesPerBRICall("The number of files per call of Backint for Oracle is not defined correctly in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::SyntaxErrorMaxDelayOfBRICall("The maximal allowed delay for the call of Backint for Oracle is not defined correctly in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::SyntaxErrorUseDirectoryDelimiterOnRestore("Please use YES or NO for specifying in the parameter file if final directory delimiters should be omitted or not on restores.");
const tcn385_EnumString tcn385_Backint2Error::SyntaxErrorValueToLong("A value in the Backint for SAP DB parameter file is to long.");

const tcn385_EnumString tcn385_Backint2Error::NoStagingAreaDefined("No staging area is defined in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::BRINotSpecified("The path of Backint for Oracle is not defined in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::BackintMustBeAFile("Backint for Oracle must be a file.");
const tcn385_EnumString tcn385_Backint2Error::HistoryNotSpecified("The name of the history file is not defined in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::BRIInputNotSpecified("The name of the input file of Backint for Oracle is not defined in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::BRIOutputNotSpecified("The name of the output file of Backint for Oracle is not defined in the parameter file.");
const tcn385_EnumString tcn385_Backint2Error::BRIErrorNotSpecified("The name of the error output file of Backint for Oracle is not defined in the parameter file.");

const tcn385_EnumString tcn385_Backint2Error::NotEnoughStagingAreas("Not enough staging areas defined.");
const tcn385_EnumString tcn385_Backint2Error::NoFittingStagingArea("Could not find a fitting staging area for at least one pipe.");
const tcn385_EnumString tcn385_Backint2Error::NoStageForPipe("Could not assign a staging area to every pipe.");
const tcn385_EnumString tcn385_Backint2Error::SameRestoreStage("Could not assign non conflicting stage names to the pipes.");
const tcn385_EnumString tcn385_Backint2Error::StageIsNotExisting("A stage file is not existing.");
const tcn385_EnumString tcn385_Backint2Error::TooManyDataTroughPipe("Too many data was transferred trough a pipe.");

const tcn385_EnumString tcn385_Backint2Error::CouldNotOpenInputFile("Could not open the input file.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotReadInputFile("Could not read from the input file.");

const tcn385_EnumString tcn385_Backint2Error::CouldNotOpenOutFile("Could open the output file.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotWriteToOutFile("Could not write to the output file.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotCloseOutFile("Could not close the output file.");

const tcn385_EnumString tcn385_Backint2Error::CouldNotWriteBackintInput("Could not write to the input file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotStartBackint("Could not start Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::BackintWasUnsuccessful("Backint for Oracle was unsuccessful.");
const tcn385_EnumString tcn385_Backint2Error::SyntaxErrorInBackintOutput("There is a syntax error in the output of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::BackintProcessedWrongFile("Backint for Oracle has processed a wrong file.");
const tcn385_EnumString tcn385_Backint2Error::BackintProcessedUnsuccessful("Backint for Oracle has not processed all files as requested.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotCloseBackintInput("Could not close the input file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotCreateBackintOutput("Could not create output file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotCreateBackintErrorOutput("Could not create error output file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::BackintOutputVanished("The output file of Backint for Oracle does not exist.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotOpenBackintOutput("Could not open the output file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotCloseBackintOutput("Could not close the output file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotRemoveBackintOutput("Could not remove the output file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotRemoveBackintErrorOutput("Could not remove the error output file of Backint for Oracle.");
const tcn385_EnumString tcn385_Backint2Error::CouldNotRemoveBackintInput("Could not remove the input file of Backint for Oracle.");

const tcn385_EnumString tcn385_Backint2Error::UnexpectedRunningConverters("There are unexpected pipe2file converter processes running.");
const tcn385_EnumString tcn385_Backint2Error::UnsuccesfulConverterCleanUp("Could not free all system resources used by converter processes (stages, pipe2file processes).");

const tcn385_EnumString tcn385_Backint2Error::CouldNotWriteHistory("Could not write history file.");
const tcn385_EnumString tcn385_Backint2Error::InsufficientHistoryInformation("Could not find all needed information in the backup history.");

const tcn385_EnumString tcn385_Backint2Error::NotYetImplemented("This feature is not jet implemented.");
const tcn385_EnumString tcn385_Backint2Error::InternalError("Internal Error.");


tcn385_Backint2Error::tcn385_Backint2Error(const tcn385_EnumString & ErrorCase)
  :ThisError(ErrorCase)
{
}

tcn385_Backint2Error::~tcn385_Backint2Error()
{
}

void tcn385_Backint2Error::FillErrorMsgBuffer() const
{
    if(0!=ThisError.GiveString())
        cn36_StrNCpy(ErrorMsgBuffer, ThisError.GiveString(), ErrorMsgBufferLength_ni35);
    else
        ErrorMsgBuffer[0]='\0';
}


tcn385_Backint2ErrorHandler   TheRealBackint2ErrorHandler;
tcn385_Backint2ErrorHandler * TheBackint2ErrorHandler_cn385=&TheRealBackint2ErrorHandler;

tcn385_Backint2ErrorHandler::tcn385_Backint2ErrorHandler()
  :OutputFile(0)
{
}

void tcn385_Backint2ErrorHandler::reportString(const char * string)
{
    if(0==OutputFile ||
       !OutputFile->WriteLine(string))
    {
        fprintf(stderr, "%s\n", string);
    }
}

int tcn385_Backint2ErrorHandler::SetOutput(const char * NameOfOutputFile)
{
    int rc=1;

    if(0!=OutputFile)
        delete OutputFile;

    if(0==NameOfOutputFile)
        OutputFile=0;
    else
    {
        OutputFile=new tni34_ATextFile(NameOfOutputFile, tni34_AFile::ToDiskAppend, 1, 0);

        if(0==OutputFile || !OutputFile->IsGood())
        {
            fprintf(stderr, "Out of memory.\n"); //can not use the ErrorHandler in this situation
            rc=0;

            if(0!=OutputFile)
            {
                delete OutputFile;
                OutputFile=0;
            }
        }
        else
            rc=OutputFile->IsGood();
    }

    return rc;
}
