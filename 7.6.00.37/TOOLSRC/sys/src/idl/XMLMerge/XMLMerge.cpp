/*!*****************************************************************************

  module      : XMLMerge.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (Goertzp)

  special area: Merging XML files
  description : Main program
				
  ----------------------------------------------------------------------------

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#include <stdio.h>

#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#else
#include <iostream.h>
#include <string.h>
#include <fstream.h>
#endif

#include "idl/XMLMerge/XMLMergeErrorHandling.hpp"


/*----------------------------------------------------------------------------*/

const int maxNameLength = 250;
typedef char FileName[maxNameLength+6];
/* 250 characters, inclusive extension ".xml"
   +5: extension "__mgd" for output file
   +1: NULL terminator "\0" */

const int maxLineLength = 1024;
typedef char XMLLine[maxLineLength];

const int maxTagLength = 24;
typedef char XMLTag[maxTagLength+1];

typedef enum {
  IMPORT,
  INCLUDE
} MergeTag;

typedef enum {
  IDL2XML,
  CLASSES,
  INCCONTENT
} WrapperTag;

typedef enum {
  Import,
  NoImport,
  SkippedImport
} ImportMode;

class Element
{
public:
  Element(const char * pName);
  ~Element();
  const char * getName() {return m_pName;}
  void         setNext(Element * pElement) {m_pNext = pElement;}
  Element *    getNext() {return m_pNext;}
private:
  char *    m_pName;
  Element * m_pNext;
};

Element::Element(const char * pName) {
  m_pName = new char[strlen(pName)+1];
  strcpy( m_pName, pName );
  m_pNext = NULL;
}  

class List
{
public:
  List() : m_pFirst(NULL), m_pLast(NULL) {};
  void Append(const char * pName);
  bool Contains(const char * pName);
private:
  Element * m_pFirst;
  Element * m_pLast;
};

void List::Append(const char * pName) {
  Element * pE = new Element(pName);
  if (!m_pFirst) {
    m_pFirst = pE;
  }
  else {
    m_pLast->setNext(pE);
  }
  m_pLast = pE;
}

bool List::Contains(const char * pName) {
  Element * pCurr = m_pFirst;
  while (pCurr) {
    if (0 == strcmp( pName, pCurr->getName() )) {
      return true;
    }
    pCurr = pCurr->getNext();
  }
  return false;
}

static List ImportedFiles;

#define exitRoutine(fp) fclose(fp); \
                        rc = 1;     \
                        return rc

/*----------------------------------------------------------------------------*/

int lineLen(XMLLine line) {

  int strLen = strlen(line), termLen = 0;

  if (strLen > 0) {
    if ( (line[strLen-1] == 10) || (line[strLen-1] == 13) ) {
      termLen++;
    }
  }
  if (strLen > 1) {
    if ( (line[strLen-2] == 10) || (line[strLen-2] == 13) ) {
      termLen++;
    }
  }
  
  return (strLen - termLen);

}

/*----------------------------------------------------------------------------*/

int getFirstOccurrence( XMLLine line,
                        char * str,
                        int offset = 0) {

  int len = lineLen(line), len2 = strlen(str), i;

  for ( i = offset; i < len-len2+1; i++ ) {
    if (!strncmp( &line[i], str, len2 )) {
      return i;
    }
  }

  return -1;

}

/*----------------------------------------------------------------------------*/

bool onlyLeadingBlanks( XMLLine line,
                        int pos ) {

  bool rc = false;
  int pos2 = lineLen(line), i;
  
  if ( (0 <= pos) && (pos <= pos2) ) {
    rc = true;
    for ( i = 0; i < pos; i++ ) {
      if (!(' ' == line[i])) {
        rc = false;
        break;
      }
    }
  }

  return rc;

}

/*----------------------------------------------------------------------------*/

bool onlyTrailingBlanks( XMLLine line,
                         int pos ) {

  bool rc = false;
  int pos2 = lineLen(line), i;
  
  if ( (-1 <= pos) && (pos <= pos2-1) ) {
    rc = true;
    for ( i = pos+1; i < pos2; i++ ) {
      if (!(' ' == line[i])) {
        rc = false;
        break;
      }
    }
  }

  return rc;

}

/*----------------------------------------------------------------------------*/

bool emptyLine(XMLLine line) {

  bool rc = true;
  int len = lineLen(line), i;

  for ( i = 0; i < len; i++ ) {
    if (!(' ' == line[i])) {
      rc = false;
      break;
    }
  }

  return rc;

}

/*----------------------------------------------------------------------------*/

void initFileName(FileName fName) {

  for ( int i = 0; i < maxNameLength+6; i++ ) {
    fName[i] = '\0';
  }

}

/*----------------------------------------------------------------------------*/

int checkXMLFile(char * name) {

  int rc = 0;
  int len = strlen(name);

  if (len < 4) {
    rc = 1;
  }
  else if ( !(name[len-4] == '.') ||
            !(name[len-3] == 'x') ||
            !(name[len-2] == 'm') ||
            !(name[len-1] == 'l') ) {
    rc = 1;
  }
  if (rc == 1) {
    XMLMerge_ErrorMsg(C_NO_XML_FILE);
  }

  return rc;

}

/*----------------------------------------------------------------------------*/

int analyzeMergeTag( XMLLine line,
                     int & sp,
                     int & ep,
                     MergeTag mTag,
                     WrapperTag & wrapper,
                     FileName currFile ) {

  int rc = 0;
  int len = lineLen(line), i;

  /* get start and end position */
  sp = ep = 0;
  for ( i = 0; i < len; i++ ) {
    if ( line[i] == '"' ) {
      sp = i + 1;
      break;
    }
  }
  for ( i = i + 1; i < len; i++ ) {
    if ( line[i] == '"' ) {
      ep = i - 1;
      break;
    }
  }
  if (!( (0 < sp) && (sp < ep) && (ep < len) )) {
    XMLMerge_ErrorMsg( C_FILENAME_NOT_FOUND, currFile);
    rc = 1;
    return rc;
  }

  /* check for additional double quotes */
  for ( i = i + 1; i < len; i++ ) {
    if (line[i] == '"') {
      if (mTag == INCLUDE) {
        if (getFirstOccurrence( line, "WELLFORMED=\"YES\"" ) == (i - strlen("WELLFORMED="))) {
          i = i + strlen("YES\"");
          continue;
        }
        if (getFirstOccurrence( line, "WELLFORMED=\"NO\"" ) == (i - strlen("WELLFORMED="))) {
          i = i + strlen("NO\"");
          continue;
        }
      }
      else { // mTag == IMPORT
        if (getFirstOccurrence( line, "WRAPPER=\"IDL2XML\"" ) == (i - strlen("WRAPPER="))) {
          wrapper = IDL2XML;
          i = i + strlen("IDL2XML\"");
          continue;
        }
        if (getFirstOccurrence( line, "WRAPPER=\"CLASSES\"" ) == (i - strlen("WRAPPER="))) {
          wrapper = CLASSES;
          i = i + strlen("CLASSES\"");
          continue;
        }
      }
      XMLMerge_ErrorMsg( C_TOO_MANY_DOUBLE_QUOTES, currFile );
      rc = 1;
      return rc;
    }
  }

  /* check for blanks in name */
  for ( i = sp; i < ep + 1; i++ ) {
    if ( line[i] == ' ' ) {
      XMLMerge_ErrorMsg( C_BLANK_IN_FILENAME, currFile );
      rc = 1;
      return rc;
    }
  }

  /* check name length */
  if ((ep-sp+1) > maxNameLength) {
    XMLMerge_ErrorMsg( C_FILENAME_TOO_LONG );
    rc = 1;
    return rc;
  }

  return rc;

}

/*----------------------------------------------------------------------------*/

int processInput( int argC,
                  char * argV[],
                  FileName inFile,
                  FileName outFile ){

  int rc = 0;
  int len, argN, i;

  if (argC < 2) {
    XMLMerge_ErrorMsg(C_TOO_FEW_INPUT_PARAMS);
    rc = 1;
    return rc;
  }

  len = strlen(argV[1]);

//  if (argV[1][0] == '-') {
//    XMLMerge_ErrorMsg(C_XML_FILE_FIRST);
//    rc = 1;
//    return rc;
//  }

  if (len < 2) {
    rc = 1;
  }
  else if //(!( (argV[1][0] == '-') && (argV[1][1] == 'F') )) {
          (strncmp( &argV[1][0], "-F", 2)) {
    rc = 1;
  }
  if (1 == rc) {
    XMLMerge_ErrorMsg(C_XML_FILE_FIRST);
    return rc;
  }

  if (len-2 > maxNameLength) {
    XMLMerge_ErrorMsg(C_FILENAME_TOO_LONG);
    rc = 1;
    return rc;
  }

  rc = checkXMLFile(&argV[1][2]);
  if (rc) {
    return rc;
  }

  if (!strncmp( &argV[1][len-9], "__mgd", 5)) {
    XMLMerge_ErrorMsg(C_BAD_INPUT_FILENAME);
    rc = 1;
    return rc;
  }

  for ( i = 2; i < len; i++) {
    if (argV[1][i] == ' ') {
      XMLMerge_ErrorMsg(C_BLANK_IN_INPUT_FILENAME);
      rc = 1;
      return rc;
    }
  }

  if (argC > 2) {
    for ( argN = 2; argN < argC; argN++ ) {
      if (strlen(&argV[argN][0]) < 2) {
        rc = 1;
      }
      else if (strncmp( &argV[argN][0], "-I", 2)) {
        rc = 1;
      }
      if (rc == 1) {
        XMLMerge_ErrorMsg(C_BAD_INPUT);
        return rc;
      }
    }
  }

  strcpy( &inFile[0], &argV[1][2] );
  strncpy( &outFile[0], &argV[1][2], len-2-4 );
  strcpy( &outFile[len-2-4], "__mgd.xml" );

  return rc;

}

/*----------------------------------------------------------------------------*/

FILE * openFile( FileName fName,
                 int argC,
                 char * argV[] ) {

  FILE * fp = NULL;
  FileName fName2;
  int argN, len, len2, addSlash;

  /* try to open file directly */
  fp = fopen( fName, "r" );

  /* in case of failure, try to open file by adding leading search path */
  if ( (NULL == fp) && (argC > 2) ) {
    for ( argN = 2; argN < argC; argN++ ) {
      addSlash = 0;
      initFileName(fName2);
      len = strlen(&argV[argN][0]);
      if (!( ('/' == argV[argN][len-1]) || ('\\' == argV[argN][len-1]) )) {
        addSlash = 1;
      }
      len2 = len-2 + addSlash + strlen(fName);
      if (len2 > maxNameLength) {
        XMLMerge_WarnMsg(C_PATH_TOO_LONG);
        continue;
      }
      strncpy( &fName2[0], &argV[argN][2], len-2 );
      if (addSlash) {
        fName2[len-2] = '/';
      }
      strcpy( &fName2[len-1], fName );
      fp = fopen( fName2, "r" );
      if (fp) {
        break;
      }
    }
  }

  /* return file pointer */
  return fp;

}

/*----------------------------------------------------------------------------*/

int skipHeaderLines( FILE * fp,
                     FileName file,
                     WrapperTag wrapper ) {

  int rc = 1,
      pos1 = -1, pos2 = -1,
      strLen = 0;
  XMLLine line;
  bool xmldeclStartFound = false, xmldeclEndFound = false,
       wstStartFound = false, wstEndFound = false;
  XMLTag wstStart, wstEnd, wstEnd_h, wstEnd_hpp;

  /* XML declaration tag must be first tag */
  while (!( xmldeclStartFound && xmldeclEndFound )) {
    if (!fgets( line, maxLineLength, fp )) {
      XMLMerge_ErrorMsg( C_FGETS_FAILED, file );
      break;
    }
    /* skip empty line */
    if (emptyLine(line)) {
      continue;
    }
    /* line is not empty */
    pos1 = pos2 = -1;
    if (!xmldeclStartFound) {
      pos1 = getFirstOccurrence( line, "<" );
      if (pos1 > -1) {
        pos2 = getFirstOccurrence( line, "<?xml" );
        /* '<' must be followed directly by '?xml' */
        if (pos1 != pos2) {
          XMLMerge_ErrorMsg( C_FIRSTLAB_NOT_XMLDECLLAB, file );
          return rc;
        }
        /* only leading blanks before '<?xml' allowed */
        else if (!onlyLeadingBlanks( line, pos2 )) {
          XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEFORE_XMLDECLLAB, file );
          return rc;
        }
        /* behind '<?xml' no further '<' allowed */
        else if (getFirstOccurrence( line, "<", pos2+1 ) > -1) {
          XMLMerge_ErrorMsg( C_ADDLAB_IN_XMLDECLLAB_LINE, file );
          return rc;
        }
        /* ok */
        else {
          xmldeclStartFound = true;
        }
      }
      else {
        XMLMerge_ErrorMsg( C_NONEMPTYLINE_BEFORE_XMLDECLSTART, file );
        return rc;
      }
    }
    if (xmldeclStartFound) {
      /* no '<' allowed, if no more in start line of tag */
      if (pos1 == -1) {
        if (getFirstOccurrence( line, "<" ) > -1) {
          XMLMerge_ErrorMsg( C_ADDLAB_INSIDE_XMLDECL, file);
          return rc;
        }
      }
      /* no '<' occurs, or still in start line of tag */
      pos1 = getFirstOccurrence( line, ">" );
      if (pos1 > -1) {
        pos2 = getFirstOccurrence( line, "?>" );
        /* '?' must be directly in front of '>' */
        if (!( (-1 < pos2) && (pos2 == pos1-1) )) {
          XMLMerge_ErrorMsg( C_FIRSTRAB_NOT_XMLDECLRAB, file );
          return rc;
        }
        /* only trailing blanks behind '?>' allowed */
        else if (!onlyTrailingBlanks( line, pos2+1 )) {
          XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEHIND_XMLDECLRAB, file );
          return rc;
        }
        /* ok */
        else {
          xmldeclEndFound = true;
        }
      }
    }
  }
  if (!( xmldeclStartFound && xmldeclEndFound )) {
    XMLMerge_ErrorMsg( C_XMLDECLTAG_NOT_FOUND, file );
    return rc;
  }

  /*  wrapper start tag must be second tag */
  switch (wrapper) {
    case IDL2XML :
      strcpy( wstStart, "<IDL2XML" );
      strcpy( wstEnd, "XMLSchema-instance\">" );
      break;
    case CLASSES :
      strcpy( wstStart, "<CLASSES" );
      strcpy( wstEnd, "XMLSchema-instance\">" );
      break;
    case INCCONTENT :
      strcpy( wstStart, "<INCCONTENT" );
      strcpy( wstEnd, "INCCONTENT>" );
      strcpy( wstEnd_h, ".h\">" );     // PTS 1140184
      strcpy( wstEnd_hpp, ".hpp\">" ); // PTS 1140184
      break;
  }
  while (!( wstStartFound && wstEndFound )) {
    if (!fgets( line, maxLineLength, fp )) {
      XMLMerge_ErrorMsg( C_FGETS_FAILED, file );
      break;
    }
    /* skip empty line */
    if (emptyLine(line)) {
      continue;
    }
    /* line is not empty */
    pos1 = pos2 = -1;
    if (!wstStartFound) {
      pos1 = getFirstOccurrence( line, "<" );
      if (pos1 > -1) {
        pos2 = getFirstOccurrence( line, wstStart );
        /* '<' must be followed directly by 'wrapper' */
        if (pos1 != pos2) {
          XMLMerge_ErrorMsg( C_SECONDLAB_NOT_WRAPPERSTARTLAB, file, wstStart );
          return rc;
        }
        /* only leading blanks before '<wrapper' allowed */
        else if (!onlyLeadingBlanks( line, pos2 )) {
          XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEFORE_WRAPPERSTARTLAB, file, wstStart );
          return rc;
        }
        /* behind '<wrapper' no further '<' allowed */
        else if (getFirstOccurrence( line, "<", pos2+1 ) > -1) {
          XMLMerge_ErrorMsg( C_ADDLAB_IN_WRAPPERSTART_LABLINE, file, wstStart );
          return rc;
        }
        /* ok */
        else {
          wstStartFound = true;
        }
      }
      else {
        XMLMerge_ErrorMsg( C_NOTONLY_EMPTYLINES_BETWEEN_XMLDECLEND_AND_WRAPPERSTART, file, wstStart );
        return rc;
      }
    }
    if (wstStartFound) {
      switch (wrapper) {
        case IDL2XML :
        case CLASSES : {
          /* no '<' allowed, if no more in start line of tag */
          if (pos1 == -1) {
            if (getFirstOccurrence( line, "<" ) > -1) {
              XMLMerge_ErrorMsg( C_ADDLAB_INSIDE_WRAPPERSTART, file, wstStart);
              return rc;
            }
          }
          /* no '<' occurs, or still in start line of tag */
          pos1 = getFirstOccurrence( line, ">" );
          if (pos1 > -1) {
            pos2 = getFirstOccurrence( line, wstEnd ); /* PTS 1133796 */
            if (pos2 > -1) {
              pos2 = pos2 + strlen(wstEnd) - 2;
            }
            else if ( (wrapper == IDL2XML) && (pos2 == -1) && (!strncmp( file, "livecachetypes.xml", 18 )) ) {
              pos2 = getFirstOccurrence( line, "IDL2XML>" );
              if (pos2 > -1) {
                pos2 = pos2 + strlen("IDL2XML>") - 2;
              }
            }
            /* '"' resp. 'L' must be directly in front of '>' */
            if (!( (-1 < pos2) && (pos2 == pos1-1) )) {
              XMLMerge_ErrorMsg( C_SECONDRAB_NOT_WRAPPERSTARTRAB, file, wstEnd );
              return rc;
            }
            /* only trailing blanks behind '>' allowed */
            else if (!onlyTrailingBlanks( line, pos2+1 )) {
              XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEHIND_WRAPPERSTARTRAB, file, wstEnd );
              return rc;
            }
            /* ok */
            else {
              wstEndFound = true;
            }
          }
          break;
        }
        case INCCONTENT : { // PTS 1140184
          /* no '<' allowed, if no more in start line of tag */
          if (pos1 == -1) {
            if (getFirstOccurrence( line, "<" ) > -1) {
              XMLMerge_ErrorMsg( C_ADDLAB_INSIDE_WRAPPERSTART, file, wstStart);
              return rc;
            }
          }
          /* no '<' occurs, or still in start line of tag */
          pos1 = getFirstOccurrence( line, ">" );
          XMLTag actWstEnd;
          if (pos1 > -1) {
            pos2 = getFirstOccurrence( line, wstEnd );
            if (pos2 > -1) {
              strcpy( actWstEnd, wstEnd );
            }
            else {
              pos2 = getFirstOccurrence( line, wstEnd_h );
              if (pos2 > -1) {
                strcpy( actWstEnd, wstEnd_h );
              }
              else {
                pos2 = getFirstOccurrence( line, wstEnd_hpp );
                if (pos2 > -1) {
                  strcpy( actWstEnd, wstEnd_hpp );
                }
              }
            }
            if (pos2 > -1) {
              pos2 = pos2 + strlen(actWstEnd) - 2;
            }
            /* '"' resp. 'T' must be directly in front of '>' */
            if (!( (-1 < pos2) && (pos2 == pos1-1) )) {
              XMLMerge_ErrorMsg( C_SECONDRAB_NOT_WRAPPERSTARTRAB, file, actWstEnd );
              return rc;
            }
            /* only trailing blanks behind '>' allowed */
            else if (!onlyTrailingBlanks( line, pos2+1 )) {
              XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEHIND_WRAPPERSTARTRAB, file, actWstEnd );
              return rc;
            }
            /* ok */
            else {
              wstEndFound = true;
            }
          }
          break;
        }
      }
    }
  }
  if (!( wstStartFound && wstEndFound )) {
    XMLMerge_ErrorMsg( C_WRAPPERSTARTTAG_NOT_FOUND, file, wstStart, wstEnd );
    return rc;
  }

  /* header lines skipped */
  rc = 0;
  return rc;

}

/*----------------------------------------------------------------------------*/

bool importStartTagOK( XMLLine line,
                       FileName fName ) {

  bool rc = false;
  int pos1 = -1, pos2 = -1;

  /* line must contain import start tag */
  pos1 = getFirstOccurrence( line, "<IMPORT FILE=" );
  if (!(pos1 > -1)) {
    XMLMerge_ErrorMsg(C_BAD_CALL_OF_IMPORTSTARTTAGOK);
    return rc;
  }

  /* only leading blanks allowed */
  if (!onlyLeadingBlanks( line, pos1 )) {
    XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEFORE_IMPORTSTARTLAB, fName );
    return rc;
  }

  /* no additional '<' may occur in line */
  if (getFirstOccurrence( line, "<", pos1+1 ) > -1) {
    XMLMerge_ErrorMsg( C_ADDLAB_IN_IMPORTSTARTLINE, fName );
    return rc;
  }

  /* '>' must occur in same line */
  pos2 = getFirstOccurrence( line, ">", pos1+1 );
  if (!(pos2 > -1)) {
    XMLMerge_ErrorMsg( C_IMPORTSTARTRAB_MISSING, fName );
    return rc;
  }

  /* no additional '>' may occur in line */
  if (getFirstOccurrence( line, ">", pos2+1 ) > -1) {
    XMLMerge_ErrorMsg( C_ADDRAB_IN_IMPORTSTARTLINE, fName );
    return rc;
  }

  /* '>' may not have '/' in front */
  if (getFirstOccurrence( line, "/>" ) == pos2-1) {
    XMLMerge_ErrorMsg( C_BAD_IMPORTSTARTRAB, fName );
    return rc;
  }

  /* only trailing blanks allowed */
  if (!onlyTrailingBlanks( line, pos2 )) {
    XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEHIND_IMPORTSTARTRAB, fName );
    return rc;
  }

  /* ok */
  rc = true;
  return rc;

}

/*----------------------------------------------------------------------------*/

bool importEndTagOK( XMLLine line,
                     FileName fName ) {

  bool rc = false;
  int pos = -1;

  /* line must contain import end tag */
  pos = getFirstOccurrence( line, "</IMPORT>" );
  if (!(pos > -1)) {
    XMLMerge_ErrorMsg(C_BAD_CALL_OF_IMPORTENDTAGOK);
    return rc;
  }

  /* only leading blanks allowed */
  if (!onlyLeadingBlanks( line, pos )) {
    XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEFORE_IMPORTENDLAB, fName );
    return rc;
  }
  
  /* only trailing blanks allowed */
  if (!onlyTrailingBlanks( line, pos+strlen("</IMPORT") )) {
    XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEHIND_IMPORTENDRAB, fName );
    return rc;
  }

  /* ok */
  rc = true;
  return rc;

}

/*----------------------------------------------------------------------------*/

int importFile( FILE * fpOut,
                int argC,
                char * argV[],
                FileName impFile,
                WrapperTag wrapper ) {

  int rc = 0, pos = -1;

  FILE * fpImp = NULL;
  XMLLine line;

  bool wrapperEndTagFound = false;
  ImportMode impMode = NoImport;
  XMLTag EndTag;

  /* open file to be imported */
  fpImp = openFile( impFile, argC, argV );
  if (NULL == fpImp) {
    XMLMerge_ErrorMsg( C_FILE_OPEN_ERROR, impFile );
    rc = 1;
    return rc;
  }

  /* skip header lines */
  rc = skipHeaderLines( fpImp, impFile, wrapper );
  if (rc) {
    fclose(fpImp);
    return rc;
  }

  /* determine end tag */
  switch (wrapper) {
    case IDL2XML : {
      strcpy( EndTag, "</IDL2XML>" );
      EndTag[strlen("</IDL2XML>")] = 0;
      break;
    }
    case CLASSES : {
      strcpy( EndTag, "</CLASSES>" );
      EndTag[strlen("</CLASSES>")] = 0;
      break;
    }
    default : {
      /* should never occur, as before calling importFile analyzeMergeTag with MergeTag IMPORT is called */
      /* in analyzeMergeTag for MergeTag IMPORT only wrappers IDL2XML and CLASSES are accepted           */
      XMLMerge_ErrorMsg( C_BAD_IMPORT_WRAPPER, impFile );
      exitRoutine(fpImp);
    }
  }

  /* import relevant content */
  while ( fgets( line, maxLineLength, fpImp ) && (!wrapperEndTagFound) ) {
    /* in import mode only empty line or end tag allowed */
    if (impMode != NoImport) {
      if (emptyLine(line)) {
        continue;
      }
      else {
        if (getFirstOccurrence( line, "</IMPORT>" ) > -1) {
          if (importEndTagOK( line, impFile )) {
            /* write (Import) or skip (SkippedImport) import end tag and continue */
            if (impMode == Import) {
              fputs( line, fpOut );
            }
            impMode = NoImport;
            continue;
          }
          else {
            exitRoutine(fpImp);
          }
        }
        else {
          XMLMerge_ErrorMsg( C_NOTONLY_EMPTYLINES_BETWEEN_IMPORT_START_AND_END, impFile );
          exitRoutine(fpImp);
        }
      }
    }
    /* check for illegal INCLUDE tag */
    if (getFirstOccurrence( line, "<INCLUDE FILE=" ) > -1) {
      XMLMerge_ErrorMsg( C_BAD_INCLUDE_TAG, impFile);
      exitRoutine(fpImp);
    }
    /* tag CPPSQLDBC only supported for livecachetypes.xml, but isn't copied, PTS 1138438 */
    if (getFirstOccurrence( line, "<CPPSQLDBC" ) > -1 ) {
      if (0 == strcmp( impFile, "livecachetypes.xml" )) {
        continue;
      }
      else {
        XMLMerge_ErrorMsg( C_CPPSQLDBC_NOT_SUPPORTED, impFile );
        exitRoutine(fpImp);
      }
    }
    /* EndTag may not be copied */
    pos = getFirstOccurrence( line, EndTag );
    if (pos > -1) {
      if (!onlyLeadingBlanks( line, pos) ) {
        XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEFORE_WRAPPERENDLAB, impFile, EndTag );
        exitRoutine(fpImp);
      }
      wrapperEndTagFound = true;
      break;
    }
    /* import */
    pos = getFirstOccurrence( line, "<IMPORT FILE=" );
    if (pos > -1) {
      if(importStartTagOK( line, impFile )) {
        /* determine name of file to be imported and wrapper */
        int sp = 0, ep = 0;
        WrapperTag wt = IDL2XML;
        rc = analyzeMergeTag( line, sp, ep, IMPORT, wt, impFile );
        if (rc) {
          return rc;
        }
        FileName fn;
        strncpy( fn, &line[sp], ep-sp+1 );
        fn[ep-sp+1] = '\0';
        rc = checkXMLFile(fn);
        if (rc) {
          return rc;
        }
        /* check if already imported */
        if (!ImportedFiles.Contains(fn)) {
          fputs( line, fpOut );
          ImportedFiles.Append(fn);
          rc = importFile( fpOut, argC, argV, fn, wt );
          /* import was successful */
          if (0 == rc) {
            impMode = Import;
            continue;
          }
          /* import was not successful */
          else {
            exitRoutine(fpImp);
          }
        }
        else {
          impMode = SkippedImport;
          continue;
        }
      }
      else {
        exitRoutine(fpImp);
      }
    }
    else {
      fputs( line, fpOut );
    }
  }
  
  /* check if EndTag was found */
  if (!wrapperEndTagFound) {
    XMLMerge_ErrorMsg( C_WRAPPEREND_NOT_FOUND, impFile, EndTag );
    exitRoutine(fpImp);
  }

  /* finish subroutine */
  fclose(fpImp);
  return rc;

}

/*----------------------------------------------------------------------------*/

bool includeTagOK( XMLLine line,
                   FileName fName,
                   bool & wellFormed ) {

  bool rc = false;
  int pos1 = -1, pos2 = -1;

  wellFormed = false;

  pos1 = getFirstOccurrence( line, "<INCLUDE FILE=" );

  /* line must contain include start tag */
  if (!(pos1 > -1)) {
    XMLMerge_ErrorMsg(C_BAD_CALL_OF_INCLUDETAGOK);
    return rc;
  }

  /* only leading blanks allowed */
  if (!onlyLeadingBlanks( line, pos1 )) {
    XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEFORE_INCLUDELAB, fName );
    return rc;
  }

  /* no additional '<' may occur in line */
  if (getFirstOccurrence( line, "<", pos1+1 ) > -1) {
    XMLMerge_ErrorMsg( C_ADDLAB_IN_INCLUDELINE, fName );
    return rc;
  }

  /* '/>' must occur in same line */
  pos2 = getFirstOccurrence( line, "/>", pos1+1 );
  if (!(pos2 > -1)) {
    XMLMerge_ErrorMsg( C_INCLUDERAB_MISSING, fName );
    return rc;
  }

  /* only trailing blanks allowed */
  if (!onlyTrailingBlanks( line, pos2+1 )) {
    XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEHIND_INCLUDERAB, fName );
    return rc;
  }

  /* search for attribute WELLFORMED */
  pos1 = getFirstOccurrence( line, "WELLFORMED=" );

  /* WELLFORMED specified */
  if (pos1 > -1) {
    /* WELLFORMED="YES" */
    pos2 = getFirstOccurrence( line, "=\"YES\"", pos1+1 );
    if (pos2 == pos1+strlen("WELLFORMED")) {
      wellFormed = true;
      rc = true;
      return rc;
    }
    /* WELLFORMED="NO" */
    pos2 = getFirstOccurrence( line, "=\"NO\"", pos1+1 );
    if (pos2 == pos1+strlen("WELLFORMED")) {
      rc = true;
      return rc;
    }
    /* bad value */
    XMLMerge_ErrorMsg( C_BAD_WELLFORMED_VALUE, fName );
    return rc;
  }

  /* WELLFORMED not specified, no errors */
  rc = true;
  return rc;

}

/*----------------------------------------------------------------------------*/

int includeFile( FILE * fpOut,
                 XMLLine lineIn,
                 bool isWellFormed,
                 int argC,
                 char * argV[],
                 FileName currFile ) {

  int rc = 0,
      i = 0, sp = 0, ep = 0,
      pos = -1;
  XMLLine line;
  FileName incFile;
  FILE * fpInc = NULL;
  bool wellFormed = false,
       inccontentEndTagFound = false;

  /* determine name of file to be included */
  WrapperTag wt;
  rc = analyzeMergeTag( lineIn, sp, ep, INCLUDE, wt, currFile );
  if (rc) {
    return rc;
  }

  /* open file to be included */
  strncpy( incFile, &lineIn[sp], ep-sp+1 );
  incFile[ep-sp+1] = '\0';
  rc = checkXMLFile(incFile);
  if (rc) {
    return rc;
  }
  fpInc = openFile( incFile, argC, argV );
  if (NULL == fpInc) {
    XMLMerge_ErrorMsg( C_FILE_OPEN_ERROR, incFile );
    rc = 1;
    return rc;
  }

  /* if well-formed, skip header lines */
  if (isWellFormed) {
    rc = skipHeaderLines( fpInc, incFile, INCCONTENT );
    if (rc) {
      fclose(fpInc);
      return rc;
    }
  }

  /* include content */
  while ( fgets( line, maxLineLength, fpInc ) && (!inccontentEndTagFound) ) {
    pos = -1;
    /* check for illegal IMPORT tag */
    if (getFirstOccurrence( line, "<IMPORT FILE=" ) > -1 ) {
      XMLMerge_ErrorMsg( C_BAD_IMPORT_TAG, incFile );
      exitRoutine(fpInc);
    }
    /* check for illegal CPPSQLDBC tag, PTS 1138438 */
    if (getFirstOccurrence( line, "<CPPSQLDBC" ) > -1 ) {
      XMLMerge_ErrorMsg( C_CPPSQLDBC_NOT_SUPPORTED, incFile );
      exitRoutine(fpInc);
    }
    /* if well-formed, INCCONTENT end tag may not be copied */
    if (isWellFormed) {
      pos = getFirstOccurrence( line, "</INCCONTENT>" );
      if (pos > -1) {
        if (!onlyLeadingBlanks( line, pos) ) {
          XMLMerge_ErrorMsg( C_NOTONLY_BLANKS_BEFORE_WRAPPERENDLAB, incFile, "</INCCONTENT>" );
          exitRoutine(fpInc);
        }
        inccontentEndTagFound = true;
        break;
      }
    }
    /* include file */
    if (getFirstOccurrence( line, "<INCLUDE FILE=" ) > -1 ) {
      if(includeTagOK( line, incFile, wellFormed )) {
        rc = includeFile( fpOut, line, wellFormed, argC, argV, incFile );
        if (1 == rc) {
          exitRoutine(fpInc);
        }
      }
      else {
        exitRoutine(fpInc);
      }
    }
    /* copy line */
    else {
      fputs( line, fpOut );
    }
  }

  /* if well-formed, check if INCCONTENT end tag was found */
  if ( isWellFormed && (!inccontentEndTagFound) ) {
    XMLMerge_ErrorMsg( C_WRAPPEREND_NOT_FOUND, incFile, "</INCCONTENT>" );
    exitRoutine(fpInc);
  }

  /* finish subroutine */
  fclose(fpInc);
  return rc;

}

/*----------------------------------------------------------------------------*/

int mergeFiles( FILE * fpIn,
                FILE * fpOut,
                FileName inFile,
                int argC,
                char * argV[]) {

  int rc = 0;
  XMLLine line;
  bool wellFormed = false;
  ImportMode impMode = NoImport;

  while (fgets( line, maxLineLength, fpIn )) {
    /* in import mode */
    if (impMode != NoImport) {
      if (emptyLine(line)) {
        continue;
      }
      else {
        if (getFirstOccurrence( line, "</IMPORT>" ) > -1) {
          if (importEndTagOK( line, inFile )) {
            /* write (Import) or skip (SkippedImport) import end tag and continue */
            if (impMode == Import) {
              fputs( line, fpOut );
            }
            impMode = NoImport;
            continue;
          }
          else {
            rc = 1;
            break;
          }
        }
        else {
          XMLMerge_ErrorMsg( C_NOTONLY_EMPTYLINES_BETWEEN_IMPORT_START_AND_END, inFile );
          rc = 1;
          break;
        }
      }
    }
    /* check if import mode to be started */
    if (getFirstOccurrence( line, "<IMPORT FILE=" ) > -1) {
      if (importStartTagOK( line, inFile )) {
        /* determine name of file to be imported and wrapper */
        int sp = 0, ep = 0;
        WrapperTag wt = IDL2XML;
        rc = analyzeMergeTag( line, sp, ep, IMPORT, wt, inFile );
        if (rc) {
          return rc;
        }
        FileName fn;
        strncpy( fn, &line[sp], ep-sp+1 );
        fn[ep-sp+1] = '\0';
        rc = checkXMLFile(fn);
        if (rc) {
          return rc;
        }
        /* check if already imported */
        if (!ImportedFiles.Contains(fn)) {
          fputs( line, fpOut );
          ImportedFiles.Append(fn);
          rc = importFile( fpOut, argC, argV, fn, wt );
          /* import was successful */
          if (0 == rc) {
            impMode = Import;
            continue;
          }
          /* import was not successful */
          else {
            rc = 1;
            break;
          }
        }
        else {
          impMode = SkippedImport;
          continue;
        }
      }
      else {
        rc = 1;
        break;
      }
    }
    /* check if file to be included */
    if (getFirstOccurrence( line, "<INCLUDE FILE=" ) > -1) {
      if (includeTagOK( line, inFile, wellFormed )) {
        /* include file */
        rc = includeFile( fpOut, line, wellFormed, argC, argV, inFile );
        /* include was successful */
        if (0 == rc) {
          continue;
        }
        /* include was not successful */
        else {
          rc = 1;
          break;
        }
      }
      else {
        rc = 1;
        break;
      }
    }
    /* CPPSQLDBC only allowed in OMS_LiveCacheTypes.xml, PTS 1138438 */
    if ( (getFirstOccurrence( line, "<CPPSQLDBC" ) > -1) &&
         (0 != strcmp( inFile, "OMS_LiveCacheTypes.xml" )) ) {
      XMLMerge_ErrorMsg( C_CPPSQLDBC_NOT_SUPPORTED, inFile );
      rc = 1;
      break;
    }
    /* copy line */
    fputs( line, fpOut );
  }

  return rc;

}

/*----------------------------------------------------------------------------*/

//extern "C" __stdcall DebugBreak();

int main( int argC , char * argV[] ) {

//  DebugBreak();

  int rc, rc2, rc3;

  FileName inFile;
  FileName outFile;
  FILE * fpIn  = NULL;
  FILE * fpOut = NULL;

  /* process input */
  rc = processInput( argC, argV, inFile, outFile );
  if (rc) {
    return rc;
  }  

  /* open files */
  fpIn = fopen( inFile, "r" );
  if (NULL == fpIn) {
    XMLMerge_ErrorMsg( C_FILE_OPEN_ERROR, inFile );
    rc = 1;
    return rc;
  }
  fpOut = fopen( outFile, "w" );
  if (NULL == fpOut) {
    fclose(fpIn);
    XMLMerge_ErrorMsg( C_FILE_OPEN_ERROR, outFile);
    rc = 1;
    return rc;
  }

  /* process input file */
  rc = mergeFiles( fpIn, fpOut, inFile, argC, argV );

  /* close files */
  rc2 = fclose(fpIn);
  rc3 = fclose(fpOut);
  if (rc2) {
    XMLMerge_ErrorMsg( C_FILE_CLOSE_ERROR, inFile );
    rc = 1;
  }
  if (rc3) {
    XMLMerge_ErrorMsg( C_FILE_CLOSE_ERROR, outFile );
    rc = 1;
  }

  return rc;

}

