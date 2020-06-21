/*!*****************************************************************************
  module:       hcn381.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  basic classes for Backint SAP DB
  last change:  2000-11-29 19:15
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

#if !defined HCN381_H
#define HCN381_H

#include "hcn38.h"

class tcn381_InputEntry;

class tcn381_APipeToFileConverter
{
  public:
    char                     * CompleteStageName;
    char                     * EndOfStageName;
    char                     * DestinationStageDirectory;
    char                     * CompleteDestinationStage;
    char                     * EndOfCompleteDestinationStage;
    
    tni33_ABackgroundProcess * Process;
    tni34_APipe              * InPipe;
    tni34_APipe              * OutPipe;


    int                        WasRunning;
    
    enum AState{ConverterIsWorking,
                ConverterFinishedStage,
                BackintIsWorking,
                StageIsFree};

    AState                     State;
    tcn381_InputEntry        * InputInfo;

    tcn381_APipeToFileConverter(void);
    ~tcn381_APipeToFileConverter();
};

class tcn381_InputEntry
{
  public:
    tcn381_InputEntry(int IsAPipe, const char *TheName);
    tcn381_InputEntry(int IsAPipe, const char *TheName, size_t NameLength);
    tcn381_InputEntry(const char * ALine,
                      const char * Function);
    tcn381_InputEntry(const char * Type,
                      size_t       TypeLength,
                      const char * TheName,
                      size_t       NameLength,
                      const char * TheBID,
                      size_t       BIDLength,
                      const char * TheStageName,
                      size_t       StageNameLength,
                      const char * TheStageSize,
                      size_t       StageSizeLength,
                      const char * PartialBIDs);

    ~tcn381_InputEntry();

    char *      Name;               // TODO: make private
    char *      StageName;          // TODO: make private
    teo00_Uint8 StageSize;          // TODO: make private
    char *      RestoreStageName;   // TODO: make private
    char *      DestinationName;    // TODO: make private

    int    IsPipe;
    char   BID[17];
    int    ProcessedSuccessful;
    ni31_List<char *> ListOfPartialBIDs;
    int    NumberOfParts;
    int    RestorePart;

    teo00_Uint8 BytesWrittenToDisk;
    
    tcn381_APipeToFileConverter * Converter;
    int    FoundInThisBackintOutput;
    int   *FoundPartInThisBackintOutput;

    int    IsGood;

    int SetBID(const char * TheBID, size_t LengthOfBID);

    int EqualsToBIDAndName(const char   * TheBID,
                                 size_t   LengthOfTheBID,
                           const char   * TheName,
                                 size_t   LengthOfTheName);
    int EqualsToBIDAndName(const tcn381_InputEntry & Other);
    
    int FillHistoryInformation(const char * Type,
                               size_t       TypeLength,
                               const char * TheStageName,
                               size_t       StageNameLength,
                               const char * TheStageSize,
                               size_t       StageSizeLength,
                               const char * PartialBIDs);

  private:
    void SetName(const char *NewName, size_t NameLength);
    void SetDestination(const char *NewDest, size_t DestLength);
    void Set(char * & MemberToSet, const char *NewValue, size_t ValueLength);
    void InitAllMembers(void);
    int  SetTypeTo(const char *Type, size_t LengthOfType);
    
    int  SetPartialBIDs(const char * PartialBIDs);
    void DeletePartialBIDs(void);
};

class tcn381_OutputLine
{
  public:
    enum ErrorType{Error, NotFound};
    enum SuccessType{Saved, Restored, Found, NotDeleted};

    tcn381_OutputLine(ErrorType Type, const char * Name);
    tcn381_OutputLine(SuccessType Type, const char * BID,  const char * Name=0);
    tcn381_OutputLine(      SuccessType   Type,
                      const char        * BID,
                      const char        * Name,
                      const teo00_Uint8   Size);

    ~tcn381_OutputLine();

    char * Content;
    int    IsGood;

  private:
    const char * KeyWordFor(SuccessType Type);
};

#endif
