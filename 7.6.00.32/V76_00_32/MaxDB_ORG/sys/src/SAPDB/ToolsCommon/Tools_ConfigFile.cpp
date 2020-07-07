/*!
  -----------------------------------------------------------------------------
  module: Tools_ConfigFile.cpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: config file handling
  description:  some classes to handle config files

  -----------------------------------------------------------------------------


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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "ToolsCommon/Tools_System.hpp"
#include "ToolsCommon/Tools_ConfigFile.hpp"

/*
  -----------------------------------------------------------------------------
  macro definitions 
  -----------------------------------------------------------------------------
 */
#define TOOLS_CNFCHAR_LSC       '['    // left char of section marker
#define TOOLS_CNFCHAR_RSC       ']'    // right char of section marker
#define TOOLS_CNFCHAR_COM       '#'    // comment
#define TOOLS_CNFCHAR_EQU       '='    // assign character
#define TOOLS_CNFCHAR_NEL       '\\'   // next line character

#define TOOLS_CNF_MAXLINELEN 4096

#define TOOLS_CNF_ERROR         "Tools", __FILE__, __LINE__, Msg_List::Error

/*
  -----------------------------------------------------------------------------
  constructor Tools_ConfigFile :: Tools_ConfigFile
  -----------------------------------------------------------------------------
 */
Tools_ConfigFile :: Tools_ConfigFile 
  ( )
: m_hFile(NULL),
  m_nLine(0)
{
  ClearMessage();
} // end Tools_ConfigFile :: Tools_ConfigFile 

/*
  -----------------------------------------------------------------------------
  public function Tools_ConfigFile :: Open
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_ConfigFile :: Open
      ( const Tools_DynamicUTF8String & sFile )
{
  Tools_DynamicUTF8String sLocalFile = sFile;

  ClearMessage();

  m_hFile = NULL;

  m_hFile = fopen(sLocalFile.CharPtr(), "r");
  if (m_hFile == NULL) {
    Msg_List oMsg(TOOLS_CNF_ERROR, 
                              ToolsSys_errno(), 
                              ToolsSys_errnotext(), 
                              0);
    Msg_List oFile(TOOLS_CNF_ERROR, 
                               (Msg_List::MessageID) -1, 
                               "error open file %s", 
                               1,
                               sLocalFile.CharPtr());
    oFile.AppendNewMessage(oMsg);
    SetMessage(oFile);
  } // end if

  return LastMessage().IsEmpty();
} // end Tools_ConfigFile :: Open

/*
  -----------------------------------------------------------------------------
  public function Tools_ConfigFile :: Close
  -----------------------------------------------------------------------------
 */
void Tools_ConfigFile :: Close
      ( )
{
  if (m_hFile != NULL) {
    fclose(m_hFile);
  } // end if
} // end Tools_ConfigFile :: Close

/*
  -----------------------------------------------------------------------------
  public function Tools_ConfigFile :: Analyze
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_ConfigFile :: Analyze
      ( )
{
  ClearMessage();

  Tools_DynamicUTF8String sLine;
  Tools_DynamicUTF8String sSection;
  Tools_DynamicUTF8String sKey;
  Tools_DynamicUTF8String sEndKey;
  Tools_DynamicUTF8String sValue;

  Tools_ConfigObject      * pObject = NULL;

  while (GetNextLine(sLine) && (!feof(m_hFile) || !sLine.Empty())) {
    if (IsSectionBegin(sLine, sSection)) {
      pObject = CreateObject(sSection);
      if (pObject == NULL) {
        Tools_DynamicUTF8String snLine;
        snLine.ConvertFromInt(m_nLine);
        Msg_List oMsg(TOOLS_CNF_ERROR, (Msg_List::MessageID) -1, "error at line %s: \"%s\"", 1, snLine.CharPtr(), sLine.CharPtr());
        oMsg.AppendNewMessage(LastMessage ());
        SetMessage(oMsg);
        break;
      } // end if
    } else {
      if (pObject != NULL) {
        SplitKeyValue(sLine, sKey, sValue);
        if (pObject->CheckKey(sKey, sEndKey)) {
          if (!sEndKey.Empty()) {
            if (!ReadMultiLineValue(sEndKey, sValue)) {
              break;
            } // end if
          } // end if
          if (!pObject->AddKey(sKey, sValue)) {
            Tools_DynamicUTF8String snLine;
            snLine.ConvertFromInt(m_nLine);
            Msg_List oMsg(TOOLS_CNF_ERROR, (Msg_List::MessageID) -1, "error at line %s: \"%s\"", 1, snLine.CharPtr(), sLine.CharPtr());
            oMsg.AppendNewMessage(pObject->LastMessage ());
            SetMessage(oMsg);
            break;
          } // end if
        } else {
          Tools_DynamicUTF8String snLine;
          snLine.ConvertFromInt(m_nLine);
          Msg_List oMsg(TOOLS_CNF_ERROR, (Msg_List::MessageID) -1, "error at line %s: \"%s\"", 1, snLine.CharPtr(), sLine.CharPtr());
          oMsg.AppendNewMessage(pObject->LastMessage ());
          SetMessage(oMsg);
          break;
        } // end if
      } // end if

    } // end if
    sLine.Erase();
  } // end while

  return LastMessage().IsEmpty();
} // end Tools_ConfigFile :: Analyze

/*
  -----------------------------------------------------------------------------
  private function Tools_ConfigFile :: GetNextLine
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_ConfigFile :: GetNextLine
      ( Tools_DynamicUTF8String & sLine )
{
  ClearMessage();

  char sBuffer[TOOLS_CNF_MAXLINELEN];

  Tools_DynamicUTF8String sNext;

  memset(sBuffer, 0, TOOLS_CNF_MAXLINELEN);

  while (!feof(m_hFile) && !ferror(m_hFile)) {
    if (fgets(sBuffer, TOOLS_CNF_MAXLINELEN, m_hFile) != NULL) {
      ++m_nLine;
      sLine.ConvertFromASCII_Latin1(sBuffer, sBuffer + strlen(sBuffer));
      sLine.Trim();
      if (!sLine.Empty()) { // skip empty lines
        if (sLine[0] != TOOLS_CNFCHAR_COM) { // skip comments
          if (sLine[sLine.Length()-1] == TOOLS_CNFCHAR_NEL) { // add follow lines
            while (!feof(m_hFile) && !ferror(m_hFile)) {
              if (fgets(sBuffer ,TOOLS_CNF_MAXLINELEN, m_hFile) != NULL) {
                ++m_nLine;
                sNext.ConvertFromASCII_Latin1(sBuffer, sBuffer + strlen(sBuffer));
                sNext.Trim();
                if (!sNext.Empty()) {
                  sLine = sLine.SubStr(0, sLine.Length() - 1);
                  sLine.Append(sNext);
                  if (sNext[sNext.Length()-1] != TOOLS_CNFCHAR_NEL) {
                    break;
                  } // end if
                } else {
                  break;
                } // end if
              } // end if
            } // end while
          } // end if
          break;
        } // end if
      } // end if
    } // end if
  } // end while

  if (ferror(m_hFile)) {
    Msg_List oMsg(TOOLS_CNF_ERROR,
                              ferror(m_hFile), 
                              "file read error", 
                              0);
    SetMessage(oMsg);
  } // end if

  return LastMessage().IsEmpty();
} // end Tools_ConfigFile :: GetNextLine

/*
  -----------------------------------------------------------------------------
  private function Tools_ConfigFile :: ReadMultiLineValue
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_ConfigFile :: ReadMultiLineValue
  ( const Tools_DynamicUTF8String & sEndKey,
          Tools_DynamicUTF8String & sValue )
{
  ClearMessage();

  char sBuffer[TOOLS_CNF_MAXLINELEN];

  Tools_DynamicUTF8String sNext;
  Tools_DynamicUTF8String sLine;
  Tools_DynamicUTF8String sTrimLine;

  memset(sBuffer, 0, TOOLS_CNF_MAXLINELEN);
  sValue.Erase();

  while (!feof(m_hFile) && !ferror(m_hFile)) {
    if (fgets(sBuffer, TOOLS_CNF_MAXLINELEN, m_hFile) != NULL) {
      ++m_nLine;
      sLine.ConvertFromASCII_Latin1(sBuffer, sBuffer + strlen(sBuffer));
      sLine.TrimTrailing();
      if (!sLine.Empty()) { // skip empty lines
        if (sLine[sLine.Length()-1] == TOOLS_CNFCHAR_NEL) { // add follow lines
          while (!feof(m_hFile) && !ferror(m_hFile)) {
            if (fgets(sBuffer ,TOOLS_CNF_MAXLINELEN, m_hFile) != NULL) {
              ++m_nLine;
              sNext.ConvertFromASCII_Latin1(sBuffer, sBuffer + strlen(sBuffer));
              sNext.TrimTrailing();
              if (!sNext.Empty()) {
                sLine = sLine.SubStr(0, sLine.Length() - 1);
                sLine.Append(sNext);
                if (sNext[sNext.Length()-1] != TOOLS_CNFCHAR_NEL) {
                  sValue.Append(sLine);
                  sValue.Append("\n");
                  break;
                } // end if
              } else {
                sValue.Append(sLine);
                sValue.Append("\n");
                break;
              } // end if
            } // end if
          } // end while
        } else {
          sTrimLine = sLine;
          sTrimLine.Trim();
          if (sTrimLine == sEndKey) {
            break;
          } else {
            sValue.Append(sLine);
            sValue.Append("\n");
          } // end if
        } // end if
      } else {
        sValue.Append("\n");
      } // end if
    } // end if
  } // end while

  if (ferror(m_hFile)) {
    Msg_List oMsg(TOOLS_CNF_ERROR,
                              ferror(m_hFile), 
                              "file read error", 
                              0);
    SetMessage(oMsg);
  } else {
    sValue.TrimTrailing();
  } // end if

  return LastMessage().IsEmpty();
} // end Tools_ConfigFile :: ReadMultiLineValue

/*
  -----------------------------------------------------------------------------
  private function Tools_ConfigFile :: IsSectionBegin
  -----------------------------------------------------------------------------
 */
SAPDB_Bool Tools_ConfigFile :: IsSectionBegin
  ( const Tools_DynamicUTF8String & sLine,
          Tools_DynamicUTF8String & sSection ) 
{
  if (!sLine.Empty()) {
    if ((sLine[0] == TOOLS_CNFCHAR_LSC) && 
        (sLine[sLine.Length()-1] == TOOLS_CNFCHAR_RSC)) {
      sSection = sLine.SubStr (1, sLine.Length() - 2);
      sSection.ToUpper();
      return true;
    } // end if
  } // end if
  return false;
} // end Tools_ConfigFile :: IsSectionBegin

/*
  -----------------------------------------------------------------------------
  private function Tools_ConfigFile :: SplitKeyValue
  -----------------------------------------------------------------------------
 */
void Tools_ConfigFile :: SplitKeyValue
  ( const Tools_DynamicUTF8String & sLine,
          Tools_DynamicUTF8String & sName,
          Tools_DynamicUTF8String & sValue )
{
  Tools_DynamicUTF8String::BasisElementIndex n = sLine.FindFirstOf(TOOLS_CNFCHAR_EQU);
  if (n != Tools_DynamicUTF8String::NPos) {
    sName  = sLine.SubStr(0, n).Trim();
    sValue = sLine.SubStr(n+1).Trim();
  } else {
    sName = sLine;
    sValue = "";
  } // end if

  sName.ToUpper();
} // end Tools_ConfigFile :: SplitKeyValue
