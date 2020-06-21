/*


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




*/
#include "Oms/OMS_Defines.h"

static const   CLSID CLSID_Intro =
{0x6E822A6E, 0x91E6, 0x11D3,
{0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}};

static const   IID   IID_Intro =
{0x39C0C052, 0x91E8, 0x11D3,
{0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}};

typedef void ( __stdcall *CO_INTROSPECTFUNCTION) ( 
  unsigned short       ParmIdx,
  char                 **lplpMode,
  char                 **lplpIOtype,
  short int            *lpDatatype,
  short int            *lpSubdatatype,
  short int            *lpLength,
  short int            *lpDim,
  long                 *lpOffset,
  unsigned char        *lpFrac,
  LPGUID               *lplpGUID,
  char                 **lplpParmName);

typedef struct
{
  unsigned short        FuncIdx;
  char                  *lpMethod;
  unsigned short        ParamCount;
  CO_INTROSPECTFUNCTION lpFunction;
} t_FuncBufDesc;

typedef struct
{
  char                 *Mode;
  char                 *IOtype;
  short int            Datatype;
  short int            Subdatatype;
  short int            Length;
  short int            Dim;
  long  int            Offset;
  unsigned char        Frac;
  GUID                 guid;
  char                 *lpParmName;
} t_ParmsDesc;

static void __stdcall co_IntrospectFunction1 (
  unsigned short       ParmIdx,
  char                 **lplpMode,
  char                 **lplpIOtype,
  short int            *lpDatatype,
  short int            *lpSubdatatype,
  short int            *lpLength,
  short int            *lpDim,
  long                 *lpOffset,
  unsigned char        *lpFrac,
  LPGUID               *lplpGUID,
  char                 **lplpParmName)
{
  static const t_ParmsDesc CO_ParmDesc[] = 
  {
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"a"
  };

  if (( ParmIdx > (sizeof(CO_ParmDesc) / sizeof(t_ParmsDesc))) || ( ParmIdx < 1 ))
  {
    *lplpMode = NULL;
  }
  else
  {
    *lplpMode       = CO_ParmDesc[ParmIdx - 1].Mode;
    *lplpIOtype     = CO_ParmDesc[ParmIdx - 1].IOtype;
    *lpDatatype     = CO_ParmDesc[ParmIdx - 1].Datatype;
    *lpSubdatatype  = CO_ParmDesc[ParmIdx - 1].Subdatatype;
    *lpLength       = CO_ParmDesc[ParmIdx - 1].Length;
    *lpDim          = CO_ParmDesc[ParmIdx - 1].Dim;
    *lpOffset       = CO_ParmDesc[ParmIdx - 1].Offset;
    *lpFrac         = CO_ParmDesc[ParmIdx - 1].Frac;
    *lplpGUID       = (GUID*)&CO_ParmDesc[ParmIdx - 1].guid;
    *lplpParmName   = CO_ParmDesc[ParmIdx - 1].lpParmName;
  }
};


static void __stdcall co_IntrospectFunction2 (
  unsigned short       ParmIdx,
  char                 **lplpMode,
  char                 **lplpIOtype,
  short int            *lpDatatype,
  short int            *lpSubdatatype,
  short int            *lpLength,
  short int            *lpDim,
  long                 *lpOffset,
  unsigned char        *lpFrac,
  LPGUID               *lplpGUID,
  char                 **lplpParmName)
{
  static const t_ParmsDesc CO_ParmDesc[] = 
  {
   (char*)"OPT", (char*)"IN", 28, 16, 0, 31, 0, 0, {0x6E822A6F, 0x91E6, 0x11D3, {0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}}, (char*)"hier_inctab",
   (char*)"OPT", (char*)"IN", 28, 16, 0, 31, 0, 0, {0x6E822A6F, 0x91E6, 0x11D3, {0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}}, (char*)"hiertab",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"act_level",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"end_level",
   (char*)"OPT", (char*)"IN", 28, 16, 0, 2, 0, 0, {0x39C0C051, 0x91E8, 0x11D3, {0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}}, (char*)"sver_in",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"hiesid_in",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"start_succ",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"start_is_succ"
  };

  if (( ParmIdx > (sizeof(CO_ParmDesc) / sizeof(t_ParmsDesc))) || ( ParmIdx < 1 ))
  {
    *lplpMode = NULL;
  }
  else
  {
    *lplpMode       = CO_ParmDesc[ParmIdx - 1].Mode;
    *lplpIOtype     = CO_ParmDesc[ParmIdx - 1].IOtype;
    *lpDatatype     = CO_ParmDesc[ParmIdx - 1].Datatype;
    *lpSubdatatype  = CO_ParmDesc[ParmIdx - 1].Subdatatype;
    *lpLength       = CO_ParmDesc[ParmIdx - 1].Length;
    *lpDim          = CO_ParmDesc[ParmIdx - 1].Dim;
    *lpOffset       = CO_ParmDesc[ParmIdx - 1].Offset;
    *lpFrac         = CO_ParmDesc[ParmIdx - 1].Frac;
    *lplpGUID       = (GUID*)&CO_ParmDesc[ParmIdx - 1].guid;
    *lplpParmName   = CO_ParmDesc[ParmIdx - 1].lpParmName;
  }
};


static void __stdcall co_IntrospectFunction3 (
  unsigned short       ParmIdx,
  char                 **lplpMode,
  char                 **lplpIOtype,
  short int            *lpDatatype,
  short int            *lpSubdatatype,
  short int            *lpLength,
  short int            *lpDim,
  long                 *lpOffset,
  unsigned char        *lpFrac,
  LPGUID               *lplpGUID,
  char                 **lplpParmName)
{
  static const t_ParmsDesc CO_ParmDesc[] = 
  {
   (char*)"OPT", (char*)"IN", 28, 16, 0, 31, 0, 0, {0x6E822A6F, 0x91E6, 0x11D3, {0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}}, (char*)"hier_inctab",
   (char*)"OPT", (char*)"IN", 28, 16, 0, 31, 0, 0, {0x6E822A6F, 0x91E6, 0x11D3, {0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}}, (char*)"hiertab",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"act_level",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"end_level1",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"end_level2",
   (char*)"OPT", (char*)"IN", 28, 16, 0, 2, 0, 0, {0x39C0C051, 0x91E8, 0x11D3, {0xA4, 0x69, 0x00, 0x08, 0xC7, 0x1E, 0x43, 0x05}}, (char*)"sver_in",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"hiesid_in",
   (char*)"MAN", (char*)"IN", 22, 0, 0, 0, 0, 0, {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, (char*)"start_succ"
  };

  if (( ParmIdx > (sizeof(CO_ParmDesc) / sizeof(t_ParmsDesc))) || ( ParmIdx < 1 ))
  {
    *lplpMode = NULL;
  }
  else
  {
    *lplpMode       = CO_ParmDesc[ParmIdx - 1].Mode;
    *lplpIOtype     = CO_ParmDesc[ParmIdx - 1].IOtype;
    *lpDatatype     = CO_ParmDesc[ParmIdx - 1].Datatype;
    *lpSubdatatype  = CO_ParmDesc[ParmIdx - 1].Subdatatype;
    *lpLength       = CO_ParmDesc[ParmIdx - 1].Length;
    *lpDim          = CO_ParmDesc[ParmIdx - 1].Dim;
    *lpOffset       = CO_ParmDesc[ParmIdx - 1].Offset;
    *lpFrac         = CO_ParmDesc[ParmIdx - 1].Frac;
    *lplpGUID       = (GUID*)&CO_ParmDesc[ParmIdx - 1].guid;
    *lplpParmName   = CO_ParmDesc[ParmIdx - 1].lpParmName;
  }
};


#define PROCEDURE_NUM 3
#define VERSION_NUMBER 1
