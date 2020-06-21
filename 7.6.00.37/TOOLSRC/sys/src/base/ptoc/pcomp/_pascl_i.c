/*


    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#include <ptoc.h>

typedef
  unsigned char STRING[256];
int sql__strlen();
typedef
  int SymbolTableRange;
typedef
  int KINDTYPE;
typedef
  int HdgRange;
typedef
  int ParseStackRange;
int SymtabI();
int PrimType();
int ChkEqualType();
int IdPtrIncr();
int ParmPtrIncr();
int IncrExpPtr();
int IncrConstPtr();
int IncrLabelPtr();
int MoveStacks();
int PopScope();
int upscope();
int CloseRec();
int OpenRec();
int NewSymbol();
int FindSymbol();
int FindQSymbol();
int EntSymbol();
int EntModule();
int EntBlock();
int EntConstant();
int EntVariable();
int LabelName();
int EntField();
int EntVariant();
int EntLabel();
int InitSystem();
int ChkTypeDefn();
int TypName();
int CheckType();
int CheckForwardDefn();
int DclProc();
int EntParameterList();
int FormalProcType();
int EntProcParm();
int IdLookup();
int QidLookup();
int FindIntset();
int CheckCaseConst();
int SysprocsI();
int CallPcstart();
int CallPcexit();
int CallFnil();
int CallCompare();
int CallSetOp();
int CallSetRel();
int CallInct();
int CallIntor();
int CallInt();
int CallCttot();
int CallConvertt();
int CallStringCopy();
int CallStringConcat();
int CallStringCompare();
int CallRange();
int CallNilCheck();
int EvalGoto();
int EvalDefname();
int IsProcedure();
int EvalSys();
int OpenInput();
int initialize();
int getline();
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
int PasclInitial();

struct _ptoc_FRAME_PasclInitial
  {
  char dummy;
  } ;


int PasclInitial()
    {
    struct _ptoc_FRAME_PasclInitial _ptoc_LOCAL;
    
    SymtabI();
    SysprocsI();
    P1m2I();
    }
