/*!*****************************************************************************
  module:       hcn395.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  classes for reporting the errors for Backint for SAP DB
  last change:  2000-11-29 11:49
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

#if !defined HCN385_H
#define HCN385_H

#include "hni34.h"
#include "hni35.h"

class tcn385_EnumString
{
  public:
    tcn385_EnumString(const char *s);
    tcn385_EnumString(const tcn385_EnumString & o);
    ~tcn385_EnumString();

    const char * GiveString() const;
          int    GiveEnumValue() const;

  private:
    char * String;
    int    EnumValue;
    static int CurrentEnumValue;
};

class tcn385_Backint2Error: public tni35_AError
{
  public:
      static const tcn385_EnumString OutOfMemory,

                                     CouldNotOpenParameterFile,

                                     SyntaxErrorStage,
                                     SyntaxErrorSize,
                                     SyntaxErrorNumberFilesPerBRICall,
                                     SyntaxErrorMaxDelayOfBRICall,
                                     SyntaxErrorUseDirectoryDelimiterOnRestore,
                                     SyntaxErrorValueToLong,

                                     NoStagingAreaDefined,
                                     BRINotSpecified,
                                     BackintMustBeAFile,
                                     HistoryNotSpecified,
                                     BRIInputNotSpecified,
                                     BRIOutputNotSpecified,
                                     BRIErrorNotSpecified,

                                     NotEnoughStagingAreas,
                                     NoFittingStagingArea,
                                     NoStageForPipe,
                                     SameRestoreStage,
                                     StageIsNotExisting,
                                     TooManyDataTroughPipe,

                                     CouldNotOpenInputFile,
                                     CouldNotReadInputFile,

                                     CouldNotOpenOutFile,
                                     CouldNotWriteToOutFile,
                                     CouldNotCloseOutFile,

                                     CouldNotWriteBackintInput,
                                     CouldNotStartBackint,
                                     BackintWasUnsuccessful,
                                     SyntaxErrorInBackintOutput,
                                     BackintProcessedWrongFile,
                                     BackintProcessedUnsuccessful,
                                     CouldNotCloseBackintInput,
                                     CouldNotCreateBackintOutput,
                                     CouldNotCreateBackintErrorOutput,
                                     BackintOutputVanished,
                                     CouldNotOpenBackintOutput,
                                     CouldNotCloseBackintOutput,
                                     CouldNotRemoveBackintOutput,
                                     CouldNotRemoveBackintErrorOutput,
                                     CouldNotRemoveBackintInput,

                                     UnexpectedRunningConverters,
                                     UnsuccesfulConverterCleanUp,

                                     CouldNotWriteHistory,
                                     InsufficientHistoryInformation,

                                     NotYetImplemented,
                                     InternalError;

      tcn385_Backint2Error(const tcn385_EnumString & ErrorCase);
      ~tcn385_Backint2Error();

      virtual void FillErrorMsgBuffer() const;

  private:
      const tcn385_EnumString & ThisError;
};

class tcn385_Backint2ErrorHandler: public tni35_AErrorHandler
{
  public:
    tcn385_Backint2ErrorHandler();

    virtual void reportString(const char * string);
    int SetOutput(const char * NameOfOutputFile);

  private:
    tni34_ATextFile * OutputFile;
};

extern tcn385_Backint2ErrorHandler * TheBackint2ErrorHandler_cn385;

#endif
