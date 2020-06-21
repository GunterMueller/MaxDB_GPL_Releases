/* @(#)pgram/f.head.h


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
#include "../pgram/0.h"


#include "../common/csg.def"
#include "../common/hdg.def"
#include "../common/globals.ci"
#include "../common/csg.i"
#include "../common/opcode.i"
#include "../common/stab.h"
#include "../pascl/pascal_opc.i"
#include "../pascl/globals1.ci"
#include "../pascl/symtab.ch"
#include "../pascl/pascalalloc.h"
#include "../pascl/sflags.def"
#include "../pgram/yy.h"
#include "../pgram/f.decls.h"
#include "../common/machdep.h"

#ifdef PYRAMID_DOUBTFUL
#else
int EndCardNum;
#endif

extern yyerrflag;

yyactr(__np__){
	register int yyYpv;
	register int *p, *q;
	yyYpv = sp;

switch(__np__){

case 1: {
		if (ErrorCount == 0) {
			t1 = LongStatement (
				StoreOp,
				IntegerPtr,
				node3 (IdOp, IntegerPtr, _ArgcPtr, 0),
				UnaryExpression (
					LoadOp,
					IntegerPtr,
					node3 (IdOp, IntegerPtr, ArgcPtr, 0),
					0,
					0),
				0,
				0,
				-1);
			t2 = LongStatement (
				StoreOp,
				AnyPtrPtr,
				node3 (IdOp, AnyPtrPtr, _ArgvPtr, 0),
				UnaryExpression (
					LoadOp,
					AnyPtrPtr,
					node3 (IdOp, AnyPtrPtr, ArgvPtr, 0),
					0,
					0),
				0,
				0,
				-1);
			t1 = LinkChain (t1, t2);
			t2 = LongStatement (
				StoreOp,
				AnyPtrPtr,
				node3 (IdOp, AnyPtrPtr, _EnvpPtr, 0),
				UnaryExpression (
					LoadOp,
					AnyPtrPtr,
					node3 (IdOp, AnyPtrPtr, EnvpPtr, 0),
					0,
					0),
				0,
				0,
				-1);
			NodeIndex = LinkChain (t1, t2);
#ifdef PYRAMID_DOUBTFUL
#else
			STab [STab [STab [ProcIndex].SType].SBlock].SCard =
								CardNum [yyYpv + 2];
			NodeIndex = LinkChain (
				special (
					BeginOp,
					0,
					STab [STab [ProcIndex].SType].SBlock,
					CardNum [yyYpv + 2]),
				NodeIndex);
#endif
			NodeIndex = LinkChain (
				NodeIndex,
				CallPcstart (control ['r'], -1));
			if (cnt [yyYpv + 1] != 0) {
		   		NodeIndex = LinkChain (
					NodeIndex,
					EvalProgramParameters (yyYpv + 1));
			}
			/* PCLOSE is done in sql__exit */
			g1 [yyYpv + 2] = LinkChain (
				g1 [yyYpv + 2],
				CallPcexit (-1));
#ifdef PYRAMID_DOUBTFUL
#else
			g1 [yyYpv + 2] = LinkChain (
				g1 [yyYpv + 2],
				special (EndOp, 0, LevelNo, EndCardNum));
#endif
			g1 [mp] = ProcNode (
				Proc,
				STab [ProcIndex].SType,
				g1 [yyYpv + 2],
				NodeIndex,
				ProcIndex,
				CardNum [yyYpv + 2]);
			HdgHead = LinkChain (HdgHead, g1 [mp]);
		}
		ClearTemporarys ();
		PopScope (yyYpv + 1);
#ifdef PYRAMID_DOUBTFUL
#else
		XformAll(PopProcType,1);
#endif
	} break;
case 2: {
		for (t1 = ProcMark; t1 <= SymPtr; t1 = STab [t1].SNext) {
			if (
				(STab [t1].SLevel == 1) &&
				(STab [t1].SKind != LabelDcl)
			) {
				if (GetFlag (t1, ForwardFlag)) {
					DclError (
						STab [t1].SName,
						STab [t1].SCard,
						STab [t1].SFile,
						3);
				}
			}
		}
	} break;
case 3: {
		LevelNo = 0;  /* force LevelNo to '0' for YPROGRAM */
#ifdef PYRAMID_DOUBTFUL
		EntModule (yyYpv + 2, ModuleDcl);
#else
		PushProcType(&STab[ EntModule (yyYpv + 2, ModuleDcl) ]);
#endif
		EntBlock (yyYpv + 1);
		cnt [mp] = 0;
	} break;
case 4: {
		LevelNo = 0;  /* force LevelNo to '0' for YPROGRAM */
#ifdef PYRAMID_DOUBTFUL
		EntModule (yyYpv + 2, ModuleDcl);
#else
		PushProcType(&STab[ EntModule (yyYpv + 2, ModuleDcl) ]);
#endif
		EntBlock (yyYpv + 1);
		StartCnt [mp] = StartCnt [yyYpv + 4];
		cnt [mp] = cnt [yyYpv + 4];
	} break;
case 5: {
		yyPerror("Malformed program statement", PPROG);
			      /* Create a dummy entry for program */
		LevelNo = 0;  /* force LevelNo to '0' for YPROGRAM */
		val[yyYpv + 2] = EnterString("dummymain", 9);
#ifdef PYRAMID_DOUBTFUL
		EntModule (yyYpv + 2, ModuleDcl);
#else
		PushProcType(&STab[ EntModule (yyYpv + 2, ModuleDcl) ]);
#endif
		EntBlock (yyYpv + 1);
		cnt [mp] = 0;
	} break;
case 6:
		{
		 StartCnt [yyYpv + 1] = IdPtr;
		 cnt [yyYpv + 1] = 1;
		 IdPtrIncr ();
		 IdList [IdPtr] = val [yyYpv + 1];
		} break;
case 7:
		{
		 cnt [yyYpv + 1] = cnt [yyYpv + 1]+1;
		 IdPtrIncr ();
		 IdList [IdPtr] = val [yyYpv + 3];
		} break;
case 8: {
		MoveStacks (yyYpv + 2, mp);
	} break;
case 10: {
		g1 [mp] = g1 [yyYpv + 1];
	} break;
case 12: {
				yyPerror("Malformed declaration", PDECL);
			 } break;
case 13: { g1 [mp] = -1;
	
	} break;
case 14: { g1 [mp] = -1;
	
	} break;
case 15: { g1 [mp] = -1;
	
	} break;
case 16: { g1 [mp] = -1;
	
	} break;
case 17: { g1 [mp] = -1;
	
	} break;
case 19: {
		EntLabel (yyYpv + 1);
	} break;
case 20: {
		EntLabel (yyYpv + 3);
	} break;
case 21: {
		NodeIndex = val [yyYpv + 1];
		val [mp] = Node1parameter;
	} break;
case 22: {
					EntConstant(val [yyYpv + 2],yyYpv + 4);
				} break;
case 23:
				{
					EntConstant(val [yyYpv + 2],yyYpv + 4);
				} break;
case 24: {
			yyPerror("Malformed const declaration", PDECL);
		       } break;
case 25:
			{
				yyPerror("Malformed const declaration", PDECL);
			} break;
case 26:
				{
					TypName(yyYpv + 2,yyYpv + 4);
				} break;
case 27:
				{
					TypName(yyYpv + 2,yyYpv + 4);
				} break;
case 28: {
			yyPerror("Malformed type declaration", PDECL);
		      } break;
case 29: {
				yyPerror("Malformed type declaration", PDECL);
			  } break;
case 30:
			{
				EntVariable(yyYpv + 2,yyYpv + 4,0);
				if (IsFile (TypeType [yyYpv + 4])) {
					SetFlag (ProcIndex, FileFlag);
				}
				IdPtr = StartCnt [yyYpv + 2];
			} break;
case 31:
			{
				EntVariable(yyYpv + 2,yyYpv + 4,0);
				if (IsFile (TypeType [yyYpv + 4])) {
					SetFlag (ProcIndex, FileFlag);
				}
				IdPtr = StartCnt [yyYpv + 2];
			} break;
case 32: {
			yyPerror("Malformed var declaration", PDECL);
		     } break;
case 33: {
				yyPerror("Malformed var declaration", PDECL);
			 } break;
case 37: {
		ChkTypeDefn (PointerType, yyYpv + 1, yyYpv + 2, -1);
	} break;
case 38:
		{
		 CheckType (yyYpv + 1);
		} break;
case 39:
		{
		 ChkTypeDefn (EnumerativeType, yyYpv + 1, yyYpv + 2, -1);
		 IdPtr = StartCnt [yyYpv + 2];
		} break;
case 40:
		{
		 ChkTypeDefn (SubRangeType, yyYpv + 1, yyYpv + 3, -1);
		} break;
case 41: {
		ChkTypeDefn (StringType, yyYpv + 1, yyYpv + 3, -1);
	} break;
case 42: {
		ChkTypeDefn (SetType, yyYpv + 1, yyYpv + 4, -1);
	} break;
case 43: {
		ChkTypeDefn (FileType, yyYpv + 1, yyYpv + 4, -1);
	} break;
case 44: {
		t1 = StartCnt [yyYpv + 4] + cnt [yyYpv + 4] - 1;
		if (t1 == StartCnt [yyYpv + 4]) {
			TypeType [yyYpv + 4] = ExpType [t1];
			SubType [yyYpv + 4] = ExpStype [t1];
			ChkTypeDefn (ArrayType, yyYpv + 4, yyYpv + 7, yyYpv + 1);
			MoveStacks (yyYpv + 4, yyYpv + 7);
			}
		else for (;;) {
			TypeType [yyYpv + 4] = ExpType [t1];
			SubType [yyYpv + 4] = ExpStype [t1];
			ChkTypeDefn (ArrayType, yyYpv + 4, yyYpv + 7, yyYpv + 1);
			if (t1 == StartCnt [yyYpv + 4]) break;
			t1--;
			MoveStacks (yyYpv + 4, yyYpv + 7);
			}
		ExpPtr = StartCnt [yyYpv + 4] - 1;
		MoveStacks (yyYpv + 4, mp);
	} break;
case 46: {
		val [mp] = 0;
	} break;
case 47: {
		val [mp] = 1;
	} break;
case 48: {
		IncrExpPtr ();
		cnt [mp] = 1;
		ExpType [ExpPtr] = TypeType [yyYpv + 1];
		ExpStype [ExpPtr] = SubType [yyYpv + 1];
		StartCnt [mp] = ExpPtr;
	} break;
case 49: {
		IncrExpPtr ();
		cnt [mp] = cnt [mp] + 1;
		ExpType [ExpPtr] = TypeType [yyYpv + 3];
		ExpStype [ExpPtr] = SubType [yyYpv + 3];
	} break;
case 50: {
		CloseRec ();
		ChkTypeDefn (RecordType, yyYpv + 2, yyYpv + 3, -1);
		ProcMark = SaveProcmark [yyYpv + 2];
		HeadFieldLink = SaveReturnedType [yyYpv + 2];
		TailFieldLink = SaveParct [yyYpv + 2];
		MoveStacks (yyYpv + 2, mp);
	} break;
case 51: {
		OpenRec ();
		SaveProcmark [mp] = ProcMark;
		ProcMark = SymPtr + 1;
		SaveReturnedType [mp] = HeadFieldLink;
		SaveParct [mp] = TailFieldLink;
		HeadFieldLink = -1;
		TailFieldLink = -1;
	} break;
case 52: {
		g1 [mp] = HeadFieldLink;
		g2 [mp] = g1 [yyYpv + 2];
		NameofType [mp] = NameofType [yyYpv + 2];
	} break;
case 55: {
				yyPerror("Malformed record declaration");
		} break;
case 57:
		{
		 EntField(yyYpv + 1,yyYpv + 3);
		 IdPtr = StartCnt [yyYpv + 1];
		} break;
case 58:
		{
		 g1 [mp] = -1;
		} break;
case 59:
		{
		 HeadFieldLink = SaveReturnedType [yyYpv + 1];
		 TailFieldLink = SaveParct [yyYpv + 1];
		 g1 [mp] = g1 [yyYpv + 2];
		 ConstPtr = StartConst [TagPtr--];
		 NameofType [mp] = NameofType [yyYpv + 1];
		} break;
case 60:
		{
		 SaveReturnedType [mp] = HeadFieldLink;
		 SaveParct [mp] = TailFieldLink;
		 HeadFieldLink = -1;
		 TailFieldLink = -1;
		 NameofType [mp] = NameofType [yyYpv + 2];
		} break;
case 61: {
		CheckType (yyYpv + 1);
		if (! IsOrdinal (TypeType [yyYpv + 1])) {
			ErrorMsg (1, ColNo [yyYpv + 1]);
		}
		TagType [++TagPtr] = SubType [yyYpv + 1];
		StartConst [TagPtr] = ConstPtr;
	} break;
case 62: {
		CheckType (yyYpv + 3);
		if (! IsOrdinal (TypeType [yyYpv + 3])) {
			ErrorMsg (1, ColNo [yyYpv + 3]);
		}
		if (cnt [yyYpv + 1] != 1) {
			ErrorMsg (20, ColNo [yyYpv + 1]);
		}
		EntField (yyYpv + 1, yyYpv + 3);
		IdPtr = StartCnt [yyYpv + 1];
		TagType [++TagPtr] = SubType [yyYpv + 3];
		StartConst [TagPtr] = ConstPtr;
		NameofType [mp] = NameofType [yyYpv + 3];
	} break;
case 63:
		{
		 g1 [mp] = HeadFieldLink;
		} break;
case 65: {
				yyPerror("Malformed record declaration", PDECL);
		} break;
case 67:
		{
		 HeadFieldLink = SaveReturnedType [yyYpv + 1];
		 TailFieldLink = SaveParct [yyYpv + 1];
		 EntVariant (yyYpv + 1, yyYpv + 3);
		} break;
case 68:
		{
		 CheckCaseConst (yyYpv + 1);
		 ExpPtr = StartCnt [yyYpv + 1] - 1;
		 SaveReturnedType [mp] = HeadFieldLink;
		 SaveParct [mp] = TailFieldLink;
		 HeadFieldLink = -1;
		 TailFieldLink = -1;
		} break;
case 69: {
		if (ErrorCount == 0) {
			if (STab [ProcIndex].SKind == ProcDcl) {
				t3 = g1 [yyYpv + 3];
			}
			else {
				/* create function variable */
				GetString (val [yyYpv + 1], ProcName);
				sprintf(ss,FUNCVAR_NAME,ProcName);		/* DK 26.08.88 */
				t1 = EnterString (ss, GetStringLength (val [yyYpv + 1]) + 1);
				AllocNameSpace (t1);
				val [yyYpv + 2] = t1;
				t1 = IdLookup (yyYpv + 2);
				if (!GetFlag (t1, ModifiedFlag)) {
					ErrorMsg (129, -1);
				}
				if (IsStructured (ReturnedType)) {
					sprintf(ss,STRUCTFUNCVAR_NAME,ProcName);	/* DK 26.08.88 */
					t2 = EnterString (ss, GetStringLength (val [yyYpv + 1]) + 2);
					AllocNameSpace (t2);
					val [yyYpv + 2] = t2;
					t2 = IdLookup (yyYpv + 2);
					t3 = GenStore (
						node3 (IdOp, SubType [yyYpv + 1], t2, 0),
						TypeType [yyYpv + 1],
						SubType [yyYpv + 1],
						-1,
						UnaryExpression (
							LoadOp,
							TypeType [yyYpv + 1],
							node3 (IdOp, SubType [yyYpv + 1], t1, 0),
							0,
							0),
						TypeType [yyYpv + 1],
						SubType [yyYpv + 1],
						-1,
						CardNum [yyYpv + 1]);
					t2 = LongStatement (
						RtrnxOp,
						AnyPtrPtr,
						-1,
						UnaryExpression (
							LoadadrOp,
							TypeType [yyYpv + 1],
							node3 (IdOp, SubType [yyYpv + 1], t2, 0),
							0,
							0),
						ProcIndex,
						0,
#ifdef PYRAMID_DOUBTFUL
						CardNum [yyYpv + 1]);
#else
						-1);
#endif
					t3 = LinkChain (t3, t2);
				}
				else {
					t3 = LongStatement (
						RtrnxOp,
						TypeType [yyYpv + 1],
						-1,
						UnaryExpression (
							LoadOp,
							TypeType [yyYpv + 1],
							node3 (IdOp, SubType [yyYpv + 1], t1, 0),
							0,
							0),
						ProcIndex,
						0,
#ifdef PYRAMID_DOUBTFUL
						CardNum [yyYpv + 1]);
#else
						-1);
#endif
				}
				t3 = LinkChain (g1 [yyYpv + 3], t3);
			}
			if (GetFlag (ProcIndex, FileFlag)) {
				t3 = LinkChain (t3, CallPclose ());
			}
			if (control ['f']) {
				t3 = LinkChain(HdgFillList, t3);
				HdgFillList = -1;
			}
#ifdef PYRAMID_DOUBTFUL
#else
			STab [STab [STab [ProcIndex].SType].SBlock].SCard =
								CardNum [yyYpv + 3];
#endif
			t3 = LinkChain (
				special (
					BeginOp,
					0,
					STab [STab [ProcIndex].SType].SBlock,
					CardNum [yyYpv + 3]),
				t3);
			t3 = LinkChain (
				t3,
#ifdef PYRAMID_DOUBTFUL
				special (EndOp, 0, LevelNo, CardNum [yyYpv + 3]));
#else
				special (EndOp, 0, LevelNo, EndCardNum));
#endif
			g1 [mp] = ProcNode (
				Proc,
				STab [ProcIndex].SType,
				t3,
				-1,
				ProcIndex,
				CardNum [yyYpv + 3]);
			HdgHead = LinkChain (HdgHead, g1 [mp]);
		}
		ClearTemporarys ();
		PopScope (yyYpv + 1);
#ifdef PYRAMID_DOUBTFUL
#else
		PopProcType;
#endif /* PYRAMID_DOUBTFUL */
	} break;
case 70: {
		CheckForwardDefn (yyYpv + 3);
		PopScope (yyYpv + 1);
#ifdef PYRAMID_DOUBTFUL
#else
		PopProcType;
#endif
	} break;
case 72: {
#ifdef PYRAMID_DOUBTFUL
		DclProc (yyYpv + 1, yyYpv + 2, ProcDcl, IntegerPtr);
#else
		PushProcType(&STab[ DclProc (yyYpv + 1, yyYpv + 2, ProcDcl, IntegerPtr) ]);
#endif
		EntBlock (yyYpv + 1);
		ParmPtr = StartCnt [yyYpv + 2];
	} break;
case 73: {
		CheckType (yyYpv + 4);
#ifdef PYRAMID_DOUBTFUL
		DclProc (yyYpv + 1, yyYpv + 2, FuncDcl, SubType [yyYpv + 4]);
#else
		PushProcType(&STab[ DclProc (yyYpv + 1, yyYpv + 2, FuncDcl, SubType [yyYpv + 4])]);
#endif
		EntBlock (yyYpv + 1);
		/* create function variable */
		GetString (val [yyYpv + 1], ProcName);
		sprintf(ss,FUNCVAR_NAME,ProcName);		/* DK 26.08.88 */
		t1 = EnterString (ss, GetStringLength (val [yyYpv + 1]) + 1);
		AllocNameSpace (t1);
		EntSymbol (t1,SubType[yyYpv + 4],VarDcl,CardNum[yyYpv + 1]);
		STab [SymPtr].SClass = AUTO;
		if (IsStructured (TypeType [yyYpv + 4])) {
			ErrorMsg (149, ColNo [yyYpv + 4]);
		}
		ParmPtr = StartCnt [yyYpv + 2];
	} break;
case 74: {
		t1 = FindSymbol (val [yyYpv + 1]);
		if (t1 == -1) {
			ErrorMsg (28, ColNo [yyYpv + 1]);
			t1 = IntegerPtr;
		}
		else {
			t1 = STab [STab [t1].SType].SResult;
		}
		if (cnt [yyYpv + 2] != 0) {
			ErrorMsg (28, ColNo [yyYpv + 2]);
			cnt [yyYpv + 2] = 0;
		}
#ifdef PYRAMID_DOUBTFUL
		DclProc (yyYpv + 1, yyYpv + 2, FuncDcl, t1);
#else
		PushProcType(&STab[ DclProc (yyYpv + 1, yyYpv + 2, FuncDcl, t1)]);
#endif
		EntBlock (yyYpv + 1);
		/* create function variable */
		GetString (val [yyYpv + 1], ProcName);
		sprintf(ss,FUNCVAR_NAME,ProcName);	/* DK 26.08.88 */
		t2 = EnterString (ss, GetStringLength (val [yyYpv + 1]) + 1);
		AllocNameSpace (t2);
		EntSymbol (t2,t1,VarDcl,CardNum[yyYpv + 1]);
		STab [SymPtr].SClass = AUTO;
		if (IsStructured (PrimType (t1))) {
			/* create structured return spot */
			sprintf(ss,STRUCTFUNCVAR_NAME,ProcName);	/* DK 26.08.88 */
			t2 = EnterString (ss, GetStringLength (val [yyYpv + 1]) + 2);
			AllocNameSpace (t2);
			EntSymbol (t2,t1,VarDcl,CardNum[yyYpv + 1]);
			STab [SymPtr].SClass = LOCALSTATIC;
		}
		ParmPtr = StartCnt [yyYpv + 2];
	} break;
case 75: {
		MoveStacks (yyYpv + 2, mp);
		HdgFillList = -1;
	} break;
case 76: {
		MoveStacks(yyYpv + 2,mp);
		HdgFillList = -1;
	} break;
case 77: {
		StartCnt [mp] = StartCnt [yyYpv + 2];
		cnt [mp] = cnt [yyYpv + 2];
	} break;
case 78: {
		StartCnt [mp] = ParmPtr;
		cnt [mp] = 0;
	} break;
case 79: {
		StartCnt [mp] = StartCnt [yyYpv + 1];
		cnt [mp] = cnt [yyYpv + 1];
	} break;
case 80: {
		cnt [mp] = cnt [yyYpv + 1] + cnt [yyYpv + 3];
	} break;
case 81: {
		CheckType (yyYpv + 4);
		StartCnt [mp] = ParmPtr;
		cnt [mp] = EntParameterList (yyYpv + 1, yyYpv + 2, yyYpv + 4, ColNo [yyYpv + 4]);
		IdPtr = StartCnt [yyYpv + 2];
	} break;
case 82: {
		StartCnt [mp] = ParmPtr;
		cnt [mp] = EntProcParm (yyYpv + 1);
	} break;
case 83: {
		TypeType [mp] = ByRefParm;
	} break;
case 84: {
		TypeType [mp] = ByValueParm;
	} break;
case 85: {
		FormalProcType (yyYpv + 1, yyYpv + 2, ProcedureType, 0, IntegerPtr);
		ParmPtr = StartCnt [yyYpv + 2];
	} break;
case 86: {
		CheckType (yyYpv + 4);
		if (IsStructured (TypeType [yyYpv + 4])) {
			ErrorMsg (1, ColNo [yyYpv + 4]);
		}
		FormalProcType(yyYpv + 1,yyYpv + 2,FunctionType,NameofType[yyYpv + 4],SubType [yyYpv + 4]);
		ParmPtr = StartCnt [yyYpv + 2];
	} break;
case 87:
		{
		 g1 [mp] = HdgHead;
		 HdgHead = -1;
		} break;
case 88:
		{
		 HdgHead = g1 [yyYpv - 2];
		} break;
case 89: {
		g1 [mp] = HdgHead;
		HdgHead = -1;
	} break;
case 90: {
		HdgHead = g1 [yyYpv + 1];
		g1 [mp] = g1 [yyYpv + 2];
		CardNum [mp] = CardNum [yyYpv + 1];
#ifdef PYRAMID_DOUBTFUL
#else
		EndCardNum = CardNum[yyYpv + 3];
#endif
	} break;
case 91: {
		HdgHead = g1 [yyYpv + 1];
		g1 [mp] = HdgHead;
	} break;
case 92: {
		HdgHead = LinkChain (HdgHead, g1 [yyYpv + 2]);
		g1 [mp] = HdgHead;
	} break;
case 93: {
			t1 = g1 [yyYpv + 1];
			if (t1 != -1 && t1 != 0) {
				t2 = TailLinkOf (g1 [yyYpv + 1]);
				if (MajorOperatorOf(t2) == ifthenelse) {
					if (yychar < 0)
						yychar = yylex();
					if (yyshifts >= 2 && yychar == YELSE) {
						recovered();
						copy((char *) (&Y), (char *) (&OY), sizeof Y);
						yerror("Deleted ';' before keyword else");
						yychar = yylex();
					}
				}
			}
	} break;
case 94: {
		g1 [mp] = -1;
	} break;
case 95: {
		g1 [mp] = LinkChain (LabDef (yyYpv + 1), g1 [yyYpv + 3]);
	} break;
case 98: {
			g1 [mp] = -1;
			TypeType[mp] = VoidType;
			SubType[mp] = VoidType;
			yyPerror("Malformed statement", PSTAT);
		   } break;
case 99: {
			g1 [mp] = -1;
			yyPerror("Malformed statement", PSTAT);
		} break;
case 105: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
	} break;
case 106: {
		g1 [mp] = LabRef (yyYpv + 2);
	} break;
case 108: {
		check_call (g1 [yyYpv + 4], ColNo [yyYpv + 4]);
		g1 [mp] = GenStore (
			g1 [yyYpv + 1], TypeType [yyYpv + 1], SubType [yyYpv + 1], ColNo [yyYpv + 1],
			g1 [yyYpv + 4], TypeType [yyYpv + 4], SubType [yyYpv + 4], ColNo [yyYpv + 4],
			CardNum [yyYpv + 1]);
	} break;
case 109: {
		t1 = IdLookup (yyYpv + 1);
		if (t1 != -1  ) {
			if (STab [t1].SKind == FuncDcl) {
				/* assign to function variable */
				t3 = STab [t1].SName;
				sprintf(ss,FUNCVAR_NAME,GetStringPtr(t3));		/* DK 26.08.88 */
				t2 = EnterString (ss, GetStringLength (t3) + 1);
				AllocNameSpace (t2);
				val [yyYpv + 1] = t2;
				t1 = IdLookup (yyYpv + 1);
				AccessVariable (t1, yyYpv + 1, -1);
				SetFlag (t1, ModifiedFlag);
			}
			else if (STab [t1].SKind == ConDcl) {
				ErrorMsg (18, ColNo [yyYpv + 1]);
				TypeType [mp] = VoidType;
				SubType [mp] = VoidType;
				g1 [mp] = -1;
			}
			else {
				AccessVariable (t1, yyYpv + 1, -1);
			}
		}
		else {
			g1 [mp] = -1;
		}
	} break;
case 111: {
		QualifiedVar (yyYpv + 1, yyYpv + 3);
	} break;
case 112: {
		g1 [yyYpv + 1] = UnaryExpression (
			LoadOp,
			TypeType [yyYpv + 1],
			g1 [yyYpv + 1],
			0,
			0);
		DerefVar (yyYpv + 1);
	} break;
case 114: {
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		IndexedVar (yyYpv + 1, yyYpv + 3);
	} break;
case 115: {
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		IndexedVar (yyYpv + 1, yyYpv + 3);
	} break;
case 116: {
		t1 = IdLookup (yyYpv + 1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				cnt [yyYpv + 1] = 0;
				if(IsProcedure(STab [t1].SSysSpecific)) {
				   EvalSys (SprocCall, STab [t1].SSysSpecific,
				       yyYpv + 1, yyYpv + 1);
				} else {
				   switch (yychar) {

				   case ';':
				   case YEND:
				   case YELSE:
				   case YUNTIL:
					   ErrorMsg (26, ColNo [yyYpv + 1]);
					   break;

				   default:
					   /* syntax error will be */
					   /* caught by yacc       */
					   break;
				   }
				   g1 [mp] = -1;
				   TypeType [mp] = VoidType;
				   SubType [mp] = VoidType;
				}
				break;

			case ProcDcl:
			case ForwardProcDcl:
			case ProcParmDcl:
				GenCall (t1, yyYpv + 1, -1);
				break;

			default:
				switch (yychar) {

				case ';':
				case YEND:
				case YELSE:
				case YUNTIL:
					ErrorMsg (26, ColNo [yyYpv + 1]);
					break;

				default:
					/* syntax error will be */
					/* caught by yacc       */
					break;
				}
				g1 [mp] = -1;
				TypeType [mp] = VoidType;
				SubType [mp] =VoidType;
				break;
			}
		}
	} break;
case 117: {
		t1 = IdLookup (yyYpv + 1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				cnt [yyYpv + 1] = 0;
				if(IsProcedure(STab [t1].SSysSpecific)) {
				   EvalSys (SprocCall, STab [t1].SSysSpecific,
				      yyYpv + 1, yyYpv + 3);
				} else {
				   ErrorMsg (26, ColNo [yyYpv + 1]);
				   g1 [mp] = -1;
				   TypeType [mp] = VoidType;
				   SubType [mp] = VoidType;
				}
				break;

			case ProcDcl:
			case ForwardProcDcl:
			case ProcParmDcl:
				GenCall (t1, yyYpv + 1, yyYpv + 3);
				break;

			default:
				ErrorMsg (26, ColNo [yyYpv + 1]);
				g1 [mp] = -1;
				TypeType [mp] = VoidType;
				SubType [mp] =VoidType;
				break;
			}
		}
		ExpPtr = StartCnt [yyYpv + 3] - 1;
	} break;
case 118: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		g1 [mp] = EvalAssert (yyYpv + 1, yyYpv + 2);
	} break;
case 119: {
		g1 [mp] = ComplexStmnt (
			ifthenelse,
			0,
			g1 [yyYpv + 1],
			g1 [yyYpv + 2],
			-1,
			CardNum [yyYpv + 1]);
	} break;
case 120: {
		g1 [mp] = ComplexStmnt (
			ifthenelse,
			0,
			g1 [yyYpv + 1],
			g1 [yyYpv + 2],
			g1 [yyYpv + 4],
			CardNum [yyYpv + 1]);
	} break;
case 121: {
		if (! CompatibleTypes (TypeType [yyYpv + 1], TypeType [yyYpv + 4])) {
			ErrorMsg (1, ColNo [yyYpv + 1]);
		}
		NodeIndex = g1 [yyYpv + 1];
		if ( ! control['y']) {
		  g1 [yyYpv + 6] = EvalConvert ( TypeType [yyYpv + 1], CallCaseErr (yyYpv + 1));
		  g1 [yyYpv + 4] = LinkChain (g1 [yyYpv + 4], BuildDefault (yyYpv + 1, yyYpv + 6));
		}
		SwitchBody = g1 [yyYpv + 4];
		CaseRoot = val [yyYpv + 1];
		g1 [mp] = g1 [yyYpv + 1];
	} break;
case 122: {
		if (! CompatibleTypes (TypeType [yyYpv + 1], TypeType [yyYpv + 4])) {
			ErrorMsg (1, ColNo [yyYpv + 1]);
		}
		g1 [yyYpv + 4] = LinkChain (g1 [yyYpv + 4], BuildDefault (yyYpv + 7, yyYpv + 8));
		NodeIndex = g1 [yyYpv + 1];
		SwitchBody = g1 [yyYpv + 4];
		CaseRoot = val [yyYpv + 1];
		g1 [mp] = g1 [yyYpv + 1];
	} break;
case 123: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		if (TypeType [yyYpv + 2] != BooleanPtr) {
			ErrorMsg (1, ColNo [yyYpv + 2]);
		}
		MoveStacks(yyYpv + 2,mp);
	} break;
case 124: {
	} break;
case 125: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		g1 [mp] = ComplexStmnt (
			Switch,
			0,
			EvalConvert (IntegerPtr, g1 [yyYpv + 2]),
			-1,
			-1,
			CardNum [yyYpv + 1]);
		TypeType [mp] = TypeType [yyYpv + 2];
		val[mp] = CaseRoot;
		CaseRoot = -1;
	} break;
case 126: {
		HdgHead = g1 [yyYpv + 1];
		g1 [mp] = HdgHead;
	} break;
case 127: {
		if ((g1 [yyYpv + 1] != -1) && (g1 [yyYpv + 3] != -1)) {
			if (TypeType [yyYpv + 1] != TypeType [yyYpv + 3]) {
				ErrorMsg (1, ColNo [yyYpv + 3]);
			}
			HdgHead = LinkChain (HdgHead, g1 [yyYpv + 3]);
			g1 [mp] = HdgHead;
		}
	} break;
case 128:
		{
		 g1 [mp] = -1;
		} break;
case 129:
		{
		 g1 [mp] = BuildCase (yyYpv + 1, yyYpv + 3);
		 ExpPtr = StartCnt [yyYpv + 1] - 1;
		} break;
case 130:
		{
		 IncrExpPtr ();
		 StartCnt [mp] = ExpPtr;
		 ExpType [ExpPtr] = TypeType [yyYpv + 1];
		 ExpStype [ExpPtr] = SubType [yyYpv + 1];
		 ExpVal [ExpPtr] = g1 [yyYpv + 1];
		 ExpColNo [ExpPtr] = ColNo [yyYpv + 1];
		 IncrExpPtr ();
		 ExpVal [ExpPtr] = g2 [yyYpv + 1];
		 ExpColNo [ExpPtr] = ColNo [yyYpv + 1];
		 cnt [mp] = 1;
		} break;
case 131:
		{
/*               if (TypeType [$1] != TypeType [$3]) {  */
/*  		    ErrorMsg (1, ColNo [$3]);  */
/*  		    }  */
		 IncrExpPtr ();
		 ExpType [ExpPtr] = TypeType [yyYpv + 3];
		 ExpStype [ExpPtr] = SubType [yyYpv + 3];
		 ExpVal [ExpPtr] = g1 [yyYpv + 3];
		 ExpColNo [ExpPtr] = ColNo [yyYpv + 3];
		 IncrExpPtr ();
		 ExpVal [ExpPtr] = g2 [yyYpv + 3];
		 ExpColNo [ExpPtr] = ColNo [yyYpv + 3];
		 cnt [mp] = cnt [yyYpv + 1] + 1;
		} break;
case 132:
		{
		 BuildCaseConst (yyYpv + 1, yyYpv + 1);
		} break;
case 135: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		if (TypeType [yyYpv + 2] != BooleanPtr) {
			ErrorMsg (1, ColNo [yyYpv + 2]);
		}
		MoveStacks(yyYpv + 2,mp);
		g1 [mp] = ComplexStmnt(loop, 0, g1 [yyYpv + 1],g1 [yyYpv + 4],-1,CardNum [yyYpv + 1]);
#ifdef RASX /*RAS05 pop loop scope pushed in lexer */
                PopLoopScope();
#endif
	} break;
case 138:
		{
		 EvalFor (yyYpv + 1, yyYpv + 2);
		} break;
case 139: {
		check_call (g1 [yyYpv + 5], ColNo [yyYpv + 5]);
		check_call (g1 [yyYpv + 7], ColNo [yyYpv + 7]);
		EvalTo (yyYpv + 1, yyYpv + 2, yyYpv + 5, yyYpv + 7);
	} break;
case 140: {
		check_call (g1 [yyYpv + 5], ColNo [yyYpv + 5]);
		check_call (g1 [yyYpv + 7], ColNo [yyYpv + 7]);
		EvalDownto (yyYpv + 1, yyYpv + 2, yyYpv + 5, yyYpv + 7);
	} break;
case 141: {
		g1 [mp] = HdgHead;
		HdgHead = -1;
	} break;
case 142: {
		HdgHead = g1 [yyYpv + 1];
		check_call (g1 [yyYpv + 4], ColNo [yyYpv + 4]);
		if (TypeType [yyYpv + 4] != BooleanPtr) {
			ErrorMsg (1, ColNo [yyYpv + 4]);
		}

		g1 [mp] = ComplexStmnt (
			RepeatUntil,
			0,
			g1 [yyYpv + 4],
			g1 [yyYpv + 2],
			-1,
			CardNum [yyYpv + 1]);
#ifdef RASX /*RAS05 pop loop scope pushed in lexer */
                PopLoopScope();
#endif
	} break;
case 143:
		{
		 EndWith (yyYpv + 1);
		 g1 [mp] = LinkChain (g1 [yyYpv + 1], g1 [yyYpv + 2]);
		} break;
case 145:
		{
		 val [yyYpv + 1] = 0;
		 g1 [yyYpv + 1] = -1;
		 EvalWith (yyYpv + 1, yyYpv + 2);
		 ForWithLevel += 1;
		} break;
case 146:
		{
		 EvalWith (yyYpv + 1, yyYpv + 3);
		} break;
case 148: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalIn (yyYpv + 1, yyYpv + 3);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 149: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalRelational (yyYpv + 1,yyYpv + 3,SaveProcmark [yyYpv + 2]);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 150: {
			g1 [mp] = -1;
			yyPerror("Missing or malformed expression", PEXPR);
		} break;
case 151:
		{
		 SaveProcmark [mp] = EqOp;
		} break;
case 152:
		{
		 SaveProcmark [mp] = LtOp;
		} break;
case 153:
		{
		 SaveProcmark [mp] = GtOp;
		} break;
case 154:
		{
		 SaveProcmark [mp] = LeOp;
		} break;
case 155:
		{
		 SaveProcmark [mp] = GeOp;
		} break;
case 156:
		{
		 SaveProcmark [mp] = NeOp;
		} break;
case 158: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalBinary (yyYpv + 1,yyYpv + 3,AddOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 159: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalBinary (yyYpv + 1,yyYpv + 3,SubtOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 160: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalBinary (yyYpv + 1,yyYpv + 3,MultOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 161: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalDivide (yyYpv + 1,yyYpv + 3,DivOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 162: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalModDiv (yyYpv + 1,yyYpv + 3,DivOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 163: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalModDiv (yyYpv + 1,yyYpv + 3,ModuloOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 164: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalBinary (yyYpv + 1,yyYpv + 3,AndOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 165: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalBinary (yyYpv + 1,yyYpv + 3,OrOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 166: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalBinary(yyYpv + 1,yyYpv + 3,AndOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 167: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		EvalBinary(yyYpv + 1,yyYpv + 3,OrOp);
		ColNo [mp] = ColNo [yyYpv + 2];
	} break;
case 168: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		t1 = STab [TypeFieldOf (g1 [yyYpv + 2])].SType;
		if (t1 != IntegerType &&
		    t1 != ShortType   &&
		    t1 != UshortType  &&
		    t1 != UlongType   &&
		    t1 != LongType    &&
		    t1 != FloatType   &&
		    t1 != DoubleType )
		    ErrorMsg (1, ColNo [yyYpv + 2]);
		MoveStacks(yyYpv + 2,mp);
	} break;
case 169: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		EvalUnary(yyYpv + 2,UminusOp);
		MoveStacks(yyYpv + 2,mp);
	} break;
case 171: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		EvalUnary(yyYpv + 2,NotOp);
		MoveStacks(yyYpv + 2,mp);
	} break;
case 172: {
		GenAddress (mp, yyYpv + 2);
	} break;
case 174: {
			g1 [mp] = -1;
			yyPerror("Missing or malformed expression", PEXPR);
		   } break;
case 175: {
		NodeIndex = g1 [yyYpv + 1];
		if (
			(MinorOperator != LitOp) &&
			(MinorOperator != CallOp) &&
			(MinorOperator != Sys0Op)
		) {
			g1 [mp] = UnaryExpression (
				LoadOp,
				TypeType [yyYpv + 1],
				g1 [yyYpv + 1],
				0,
				0);
			if (SubType [yyYpv + 1] == BooleanPtr && control ['r'])
				g1 [mp] = RangeCheck (SubType [yyYpv + 1], g1 [mp]);
			SubType [mp] = SubType [yyYpv + 1];
			TypeType [mp] = TypeType [yyYpv + 1];
		}
	} break;
case 176: {
		t1 = IdLookup (yyYpv + 1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				EvalSys (SfuncCall, STab [t1].SSysSpecific, yyYpv + 1, yyYpv + 3);
				break;

			case FuncDcl:
			case ForwardFuncDcl:
			case FuncParmDcl:
				GenCall (t1, yyYpv + 1, yyYpv + 3);
				break;

			default:
				ErrorMsg (34, ColNo [yyYpv + 1]);
				g1 [mp] = -1;
				TypeType [mp] = VoidType;
				SubType [mp] = VoidType;
				break;
			}
		}
		ExpPtr = StartCnt [yyYpv + 3] - 1;
	} break;
case 177: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		MoveStacks (yyYpv + 2, mp);
	} break;
case 178: {
		g1 [mp] = node3 (LitOp, AnyPtrPtr, 0, 0);
		TypeType [mp] = AnyPtrPtr;
		SubType [mp] = AnyPtrPtr;
	} break;
case 179: {
		check_call (g1 [yyYpv + 2], ColNo [yyYpv + 2]);
		EvalUnary(yyYpv + 2,NotOp);
		MoveStacks(yyYpv + 2,mp);
	} break;
case 180: {
		EvalSetConstructor (mp, yyYpv + 2);
	} break;
case 181: {
		g1 [yyYpv + 2] = MakeList (ExpList, 0, -1, -1, -1);
		SetMinor (g1 [yyYpv + 2], ListOp);
		EvalSetConstructor (mp, yyYpv + 2);
	} break;
case 182: {
		t1 = IdLookup (yyYpv + 1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				cnt [yyYpv + 1] = 0;
				EvalSys (SfuncCall, STab [t1].SSysSpecific, yyYpv + 1, yyYpv + 1);
				break;

			case ProcDcl:
			case ForwardProcDcl:
			case ProcParmDcl:
			case FuncDcl:
			case ForwardFuncDcl:
			case FuncParmDcl:
				GenCall (t1, yyYpv + 1, -1);
				break;

			default:
				AccessVariable (t1, yyYpv + 1, -1);
				break;
			}
		}
		else {
			g1 [mp] = -1;
		}
	} break;
case 184: {
		QualifiedVar (yyYpv + 1, yyYpv + 3);
	} break;
case 185: {
		DerefVar (yyYpv + 1);
	} break;
case 187: {
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		IndexedVar (yyYpv + 1, yyYpv + 3);
	} break;
case 188: {
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		IndexedVar (yyYpv + 1, yyYpv + 3);
	} break;
case 189: {
		NodeIndex = MakeList (ExpList, TypeType [yyYpv + 1], g1 [yyYpv + 1], -1, -1);
		SetMinor (NodeIndex, ListOp);
		if (
			!(IsConstant (g1 [yyYpv + 1])  ||
			  IsConstantExpr ( g1 [yyYpv + 1]) ||
			  (MajorOperatorOf (g1 [yyYpv + 1]) == ExpList &&
			   MinorOperatorOf (g1 [yyYpv + 1]) == ListOp)
			)
		) {
			SetMinor (NodeIndex, VarCtOp);	
		}
		g1 [mp] = NodeIndex;
	} break;
case 190: {
/*  This looks redundant with the IsOrdinal test performed in the s_exp
 *  reduction
 *  Not redundant for -q - Flag, WB 18-05-88
 *              if (TypeType [$1] != TypeType [$3]) {
 */
		if (TypeType [yyYpv + 1] != TypeType [yyYpv + 3] &&
		!((TypeType [yyYpv + 1] == IntegerPtr ||
		TypeType [yyYpv + 1] == LongPtr ||
		TypeType [yyYpv + 1] == BytePtr ||
		TypeType [yyYpv + 1] == ShortPtr ||
		TypeType [yyYpv + 1] == UbytePtr ||
		TypeType [yyYpv + 1] == UshortPtr) &&
		(TypeType [yyYpv + 3] == IntegerPtr ||
		TypeType [yyYpv + 3] == LongPtr ||
		TypeType [yyYpv + 3] == BytePtr ||
		TypeType [yyYpv + 3] == ShortPtr ||
		TypeType [yyYpv + 3] == UbytePtr ||
		TypeType [yyYpv + 3] == UshortPtr))) {
			ErrorMsg (0, ColNo [yyYpv + 3]);
		}
		AppendList (ExpList, TypeType [yyYpv + 3], g1 [yyYpv + 1], g1 [yyYpv + 3]);
		if (
			!(IsConstant (g1 [yyYpv + 3]) ||
			  IsConstantExpr (g1 [yyYpv + 3]) ||
			  (MajorOperatorOf (g1 [yyYpv + 3]) == ExpList &&
			   MinorOperatorOf (g1 [yyYpv + 3]) == ListOp)
			)
		) {
			SetMinor (g1 [mp], VarCtOp);
		}
	} break;
case 191: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		if (!IsOrdinal (TypeType [yyYpv + 1])) {
			ErrorMsg (1, ColNo [yyYpv + 1]);
			TypeType [yyYpv + 1] = VoidType;
		}
		else {
			g1 [yyYpv + 1] = EvalConvert (IntegerPtr, g1 [yyYpv + 1]);
		}
	} break;
case 192: {
		check_call (g1 [yyYpv + 1], ColNo [yyYpv + 1]);
		check_call (g1 [yyYpv + 3], ColNo [yyYpv + 3]);
		if (!IsOrdinal (TypeType [yyYpv + 1])) {
			ErrorMsg (1, ColNo [yyYpv + 1]);
			TypeType [yyYpv + 1] = VoidType;
		}
		else {
			g1 [yyYpv + 1] = EvalConvert (IntegerPtr, g1 [yyYpv + 1]);
		}
		if (!IsOrdinal (TypeType [yyYpv + 3])) {
			ErrorMsg (1, ColNo [yyYpv + 3]);
			TypeType [yyYpv + 3] = VoidType;
		}
		else {
			g1 [yyYpv + 3] = EvalConvert (IntegerPtr, g1 [yyYpv + 3]);
		}
		if (!CompatibleTypes (TypeType [yyYpv + 1], TypeType [yyYpv + 3])) {
			ErrorMsg (0, ColNo [yyYpv + 3]);
		}
		NodeIndex = MakeList (ExpList, TypeType [yyYpv + 1], g1 [yyYpv + 1], g1 [yyYpv + 3], -1);
		if ((IsConstant (g1 [yyYpv + 1]) || IsConstantExpr(g1 [yyYpv + 1])) &&
		    (IsConstant (g1 [yyYpv + 3]) || IsConstantExpr(g1 [yyYpv + 3])))
		{
			SetMinor (NodeIndex, ListOp);
		} else {
			SetMinor (NodeIndex, VarCtOp);
		}
		g1 [mp] = NodeIndex;
	} break;
case 193: {
		IncrExpPtr ();
		cnt [mp] = 1;
		ExpType [ExpPtr] = TypeType [yyYpv + 1];
		ExpStype [ExpPtr] = SubType [yyYpv + 1];
		ExpVal [ExpPtr] = g1 [yyYpv + 1];
		ExpColNo [ExpPtr] = ColNo [yyYpv + 1];
		StartCnt [mp] = ExpPtr;
		ColNo [mp] = ColNo [yyYpv + 1];
	} break;
case 194: {
		IncrExpPtr ();
		cnt [mp] = cnt [yyYpv + 1]+1;
		ExpType [ExpPtr] = TypeType [yyYpv + 3];
		ExpStype [ExpPtr] = SubType [yyYpv + 3];
		ExpVal [ExpPtr] = g1 [yyYpv + 3];
		ExpColNo [ExpPtr] = ColNo [yyYpv + 3];
	} break;
case 196: {
		if (!IsIntegral (TypeType [yyYpv + 3])) {
			ErrorMsg (30, ColNo [yyYpv + 3]);
		}
		t1 = EvalConvert (IntegerPtr, g1 [yyYpv + 3]);
		if (control ['r']) {
		    t1 = RangeCheck (WFormatPtr, t1);
		}
		g1 [mp] = UnaryExpression (
			WexpOp,
			SubType [yyYpv + 1],
			g1 [yyYpv + 1],
			t1,	
			-1);
	} break;
case 197: {
		if (!IsIntegral (TypeType [yyYpv + 3])) {
			ErrorMsg (30, ColNo [yyYpv + 3]);
		}
		if (!IsIntegral (TypeType [yyYpv + 5])) {
			ErrorMsg (30, ColNo [yyYpv + 5]);
		}
		if (TypeType [yyYpv + 1] != FloatPtr &&
		    TypeType [yyYpv + 1] != LongPtr &&
		    TypeType [yyYpv + 1] != DoublePtr) {
			ErrorMsg (30, ColNo [yyYpv + 5]);
		}
		t1 = EvalConvert (IntegerPtr, g1 [yyYpv + 3]);
		t2 = EvalConvert (IntegerPtr, g1 [yyYpv + 5]);
		if (control ['r']) {
		    t1 = RangeCheck (WFormatPtr, t1);
		    t2 = RangeCheck (WFormatPtr, t2);
		}
		g1 [mp] = UnaryExpression (
			WexpOp,
			SubType [yyYpv + 1],
			g1 [yyYpv + 1],
			t1,
			t2);
	} break;
case 198: {
		g1 [mp] = UnaryExpression (
			WocthexOp,
			SubType [yyYpv + 1],
			g1 [yyYpv + 1],
			-1,
			val [yyYpv + 2]);
	} break;
case 199: {
		if (!IsIntegral (TypeType [yyYpv + 3])) {
			ErrorMsg (30, ColNo [yyYpv + 3]);
		}
		t1 = EvalConvert (IntegerPtr, g1 [yyYpv + 3]);
		if (control ['r']) {
		    t1 = RangeCheck (WFormatPtr, t1);
		}
		g1 [mp] = UnaryExpression (
			WocthexOp,
			SubType [yyYpv + 1],
			g1 [yyYpv + 1],
			t1,
			val [yyYpv + 4]);
	} break;
case 200: {
		if ((val [yyYpv + 1] != OctString) && (val [yyYpv + 1] != HexString)) {
			ErrorMsg (28, ColNo [yyYpv + 1]);
			val [mp] = HexString;
		}
	} break;
case 202:
		{
		 MoveStacks (yyYpv + 2, mp);
		} break;
case 203:
		{
		 EvalUnary (yyYpv + 2, UminusOp);
		 MoveStacks (yyYpv + 2, mp);
		} break;
case 205: {
		t1 = IdLookup (yyYpv + 1);
		AccessVariable (t1, yyYpv + 1, -1);
	} break;
case 206:
		{
		 NodeIndex = val [yyYpv + 1];
		 TypeType [mp] = TypeField;
		 SubType [mp] = TypeField;
		 g1 [mp] = NodeIndex;
		 NameofConst [mp] = GenConstName(Node1parameter);
		} break;
case 207:
		{
		 TypeType [mp] = CharPtr;
		 SubType [mp] = CharPtr;
		 g1 [mp] = node3 (LitOp, CharPtr, val [yyYpv + 1], 1);
		 NameofConst [mp] = GenCharName(val [yyYpv + 1]);
		} break;
case 208:
		{
		 TypeType [mp] = PacPtr;
		 SubType [mp] = PacPtr;
		 g1 [mp] = node3 (
		    LitOp,
		    PacPtr,
		    val [yyYpv + 1],
		    GetStringLength (val [yyYpv + 1]));
		} break;
}
}


int yyact[] = {0,-262,8204,-271,8210,-275,8203,-282,8211,-283
,8196,-290,8205,-292,8206,4096,0,16384,4096,-260
,8216,-262,8204,-271,8210,-275,8203,-282,8211,-290
,8205,-292,8206,4096,0,12290,-256,8218,-262,8204
,-271,8210,-275,8203,-282,8211,-290,8205,-292,8206
,4096,-256,8220,-296,8219,4096,12298,12301,0,12302
,-256,8222,-260,12302,-262,12302,-271,12302,-275,12302
,-282,12302,-290,12302,-292,12302,-296,8221,4096,0
,12303,-256,8224,-260,12303,-262,12303,-271,12303,-275
,12303,-282,12303,-290,12303,-292,12303,-296,8223,4096
,0,12304,-256,8226,-260,12304,-262,12304,-271,12304
,-275,12304,-282,12304,-290,12304,-292,12304,-296,8227
,4096,12305,-297,8230,4096,-256,8232,-296,8231,4096
,-256,8234,-296,8233,4096,-256,8236,-296,8227,4096
,-256,8238,-59,8237,4096,-40,8240,12366,-296,8242
,4096,-296,8243,4096,-46,8244,4096,-256,8218,-260
,8216,-262,8204,-271,8210,-275,8203,-282,8211,-290
,8205,-292,8206,4096,12297,-256,8253,-260,8216,-261
,8272,-268,12382,-270,8277,-272,8256,-273,8271,-285
,8278,-293,8266,-294,8279,-296,8252,-297,8230,-301
,8263,-59,12382,4096,12377,12299,12300,-59,8280,-40
,8281,4096,12293,-61,8282,4096,12313,-61,8283,4096
,12317,-44,8284,-58,8285,4096,12321,12294,-59,8286
,-44,8287,4096,12307,12309,-61,8288,4096,12312,-61
,8289,4096,12316,-44,8284,-58,8290,4096,12320,-260
,8216,-262,8204,-271,8210,-275,8203,-282,8211,-290
,8205,-292,8206,-296,8292,4096,12359,12360,-271,8210
,-282,8211,-292,8297,12372,-58,8300,12362,12363,12364
,12289,12296,-268,8301,-59,8302,4096,12379,-256,8253
,-260,8216,-261,8272,-268,12382,-270,8277,-272,8256
,-273,8271,-285,8278,-291,12382,-293,8266,-294,8279
,-296,8252,-297,8230,-301,8263,-59,12382,4096,-58
,8304,4096,12384,12385,-256,8305,-267,12404,-268,12404
,-287,12404,-291,12404,-46,12397,-59,12404,-40,8306
,-58,12397,-94,12397,-91,12397,4096,12387,12392,12393
,-297,8230,4096,12395,12388,12389,12390,12391,-46,8309
,-58,8308,-94,8310,-91,8311,4096,-256,8314,-277
,8326,-281,8325,-296,8322,-297,8328,-298,8330,-299
,8329,-43,8316,-45,8317,-40,8324,-91,8327,-126
,8319,-64,8320,4096,-256,8253,-260,8216,-261,8272
,-267,12382,-268,12382,-270,8277,-272,8256,-273,8271
,-285,8278,-287,12382,-291,12382,-293,8266,-294,8279
,-296,8252,-297,8230,-301,8263,-59,12382,4096,12375
,12424,12425,12398,-256,8253,-260,8216,-261,8272,-270
,8277,-272,8256,-273,8271,-285,8278,-291,12382,-293
,8266,-294,8279,-296,8252,-297,8230,-301,8263,-59
,12382,4096,-264,8341,-44,8342,4096,-44,8344,-93
,8343,4096,-296,8345,4096,12429,-277,8326,-281,8325
,-296,8322,-297,8328,-298,8330,-299,8329,-40,8324
,-91,8327,4096,12291,-296,8227,4096,-280,8363,-296
,8355,-297,8328,-298,8330,-299,8329,-43,8361,-45
,8362,-40,8356,-94,8354,12334,-296,8365,4096,12306
,-59,8371,4096,-59,8372,4096,-59,8374,-41,8373
,4096,12367,12370,12371,-296,8378,4096,12378,12381,12380
,12386,-256,8314,-277,8326,-281,8325,-296,8322,-297
,8328,-298,8330,-299,8329,-43,8316,-45,8317,-40
,8324,-91,8327,-126,8319,-64,8320,4096,12394,-61
,8383,4096,-296,8384,4096,12400,-274,8386,-61,8388
,-60,8389,-62,8390,12406,-258,8399,-263,8395,-276
,8396,-279,8400,-43,8391,-45,8392,-124,8398,-42
,8393,-47,8394,-38,8397,12435,12438,12445,-277,8326
,-281,8325,-296,8322,-297,8328,-298,8330,-299,8329
,-43,8316,-45,8317,-40,8324,-91,8327,-126,8319
,-64,8320,4096,-94,8403,12458,-277,8326,-281,8325
,-296,8322,-297,8328,-298,8330,-299,8329,-40,8324
,-91,8327,4096,12461,-256,8406,-258,12470,-263,12470
,-264,12470,-265,12470,-266,12470,-267,12470,-268,12470
,-274,12470,-276,12470,-278,12470,-279,12470,-287,12470
,-288,12470,-289,12470,-291,12470,-296,12470,-43,12470
,-45,12470,-124,12470,-42,12470,-47,12470,-38,12470
,-46,12470,-59,12470,-40,8407,-41,12470,-44,12470
,-61,12470,-58,12470,-94,12470,-91,12470,-93,12470
,-60,12470,-62,12470,4096,-46,8408,-91,8409,12463
,12466,-256,8314,-277,8326,-281,8325,-296,8322,-297
,8328,-298,8330,-299,8329,-43,8316,-45,8317,-40
,8324,-91,8327,-93,8413,-126,8319,-64,8320,4096
,12494,12495,12496,12471,-44,8417,-93,8416,4096,-267
,8419,12407,-278,8420,4096,-264,8421,-274,8386,-61
,8388,-60,8389,-62,8390,4096,12431,-274,8386,-288
,8422,-61,8388,-60,8389,-62,8390,4096,-274,8386
,-61,8388,-60,8389,-62,8390,12413,12426,-291,8423
,-59,8302,4096,12432,12401,-58,8426,4096,-46,8408
,-94,12463,-91,8409,12433,-94,8403,4096,-41,8427
,-44,8284,4096,-274,8386,-59,8428,-61,8388,-60
,8389,-62,8390,4096,-59,8429,4096,12322,12323,12324
,-296,8430,4096,-265,12493,-91,8431,12326,-265,8433
,4096,-259,8436,-269,8435,-284,8438,-286,8434,4096
,12333,12489,-296,8440,-297,8328,-298,8330,-299,8329
,4096,12335,12492,12295,-59,8442,4096,12308,-274,8386
,-59,8443,-61,8388,-60,8389,-62,8390,4096,-59
,8444,4096,-59,8445,4096,12357,12358,12365,-271,8210
,-282,8211,-292,8297,12372,-44,8284,-58,8447,4096
,12373,-58,8448,4096,12361,12383,-41,8449,-44,8450
,4096,12481,-274,8386,-296,8453,-61,8388,-58,8451
,-60,8389,-62,8390,12483,12399,-274,8386,-61,8388
,-60,8389,-62,8390,12402,12439,-61,8457,-62,8458
,12440,-61,8459,12441,-258,8399,-263,8395,-276,8396
,-42,8393,-47,8394,-38,8397,12456,-258,8399,-263
,8395,-276,8396,-42,8393,-47,8394,-38,8397,12457
,12473,-94,8403,12459,-94,8403,12460,12462,-296,8471
,4096,-274,8386,-41,8473,-61,8388,-60,8389,-62
,8390,4096,-94,8403,12467,-44,8475,-93,8474,4096
,12469,12477,-265,8476,-274,8386,-61,8388,-60,8389
,-62,8390,12479,12474,12412,-256,8314,-268,12416,-277
,8326,-281,8325,-287,12416,-296,8322,-297,8328,-298
,8330,-299,8329,-43,8316,-45,8317,-59,12416,-40
,8324,-91,8327,-126,8319,-64,8320,4096,12411,-46
,8408,-94,12463,-91,8409,12434,-274,8386,-61,8388
,-60,8389,-62,8390,12403,-61,8486,4096,-59,8487
,4096,12311,12315,12325,-296,8440,-297,8328,-298,8330
,-299,8329,-43,8361,-45,8362,4096,-41,8489,-44
,8284,4096,-278,8491,4096,-278,8492,4096,-91,8493
,4096,-296,8227,12344,12339,12490,12493,12491,12319,12310
,12314,12318,12368,-296,8498,4096,-296,8499,4096,12405
,-256,8314,-277,8326,-281,8325,-296,8322,-297,8328
,-298,8330,-299,8329,-43,8316,-45,8317,-40,8324
,-91,8327,-126,8319,-64,8320,4096,12486,12488,-274
,8386,-61,8388,-60,8389,-62,8390,12396,-258,8399
,-263,8395,-276,8396,-279,8400,-43,8391,-45,8392
,-124,8398,-42,8393,-47,8394,-38,8397,12436,-258
,8399,-263,8395,-276,8396,-279,8400,-43,8391,-45
,8392,-124,8398,-42,8393,-47,8394,-38,8397,12437
,12442,12444,12443,-258,8399,-263,8395,-276,8396,-42
,8393,-47,8394,-38,8397,12446,-258,8399,-263,8395
,-276,8396,-42,8393,-47,8394,-38,8397,12447,12448
,12449,12450,12451,12452,-258,8399,-263,8395,-276,8396
,-42,8393,-47,8394,-38,8397,12453,12454,-258,8399
,-263,8395,-276,8396,-42,8393,-47,8394,-38,8397
,12455,-41,8502,-44,8450,4096,12472,-274,8386,-61
,8388,-60,8389,-62,8390,12475,12465,12468,-256,8314
,-277,8326,-281,8325,-296,8322,-297,8328,-298,8330
,-299,8329,-43,8316,-45,8317,-40,8324,-91,8327
,-126,8319,-64,8320,4096,-274,8386,-61,8388,-60
,8389,-62,8390,12476,12408,-59,8506,12376,12414,-44
,8508,-58,8507,4096,12418,-274,8386,-61,8388,-60
,8389,-62,8390,12420,12423,-274,8386,-61,8388,-60
,8389,-62,8390,12430,12292,-93,8510,4096,12327,12328
,-296,8512,-297,8328,-298,8330,-299,8329,-43,8361
,-45,8362,-40,8356,4096,-280,8363,-296,8355,-297
,8328,-298,8330,-299,8329,-43,8361,-45,8362,-40
,8356,-94,8354,12334,-268,8516,4096,-256,8519,-261
,8521,-268,12346,-59,8518,-41,12346,4096,12341,-44
,8284,-58,8522,4096,12369,12374,12482,-274,8386,-296
,8453,-61,8388,-58,8523,-60,8389,-62,8390,12484
,12464,12478,-274,8386,-61,8388,-60,8389,-62,8390
,12480,-268,8525,12375,-256,8314,-268,12416,-277,8326
,-281,8325,-287,12416,-296,8322,-297,8328,-298,8330
,-299,8329,-43,8316,-45,8317,-59,12416,-40,8324
,-91,8327,-126,8319,-64,8320,4096,-256,8253,-260
,8216,-261,8272,-268,12382,-270,8277,-272,8256,-273
,8271,-285,8278,-287,12382,-293,8266,-294,8279,-296
,8252,-297,8230,-301,8263,-59,12382,4096,-256,8314
,-277,8326,-281,8325,-296,8322,-297,8328,-298,8330
,-299,8329,-43,8316,-45,8317,-40,8324,-91,8327
,-126,8319,-64,8320,4096,-266,8530,-274,8386,-289
,8531,-61,8388,-60,8389,-62,8390,4096,12329,12330
,-265,12493,12326,12331,-44,8533,-93,8532,4096,12336
,12338,12340,-296,8227,12344,12343,-256,8314,-268,12354
,-277,8326,-281,8325,-296,8322,-297,8328,-298,8330
,-299,8329,-43,8316,-45,8317,-59,12354,-40,8324
,-41,12354,-91,8327,-126,8319,-64,8320,4096,-296
,8540,4096,12487,12409,-287,8545,4096,12415,12417,12419
,-278,8548,4096,12342,-256,8551,-268,12347,-59,8550
,-41,12347,4096,12351,-40,8552,4096,-44,8508,-58
,8553,4096,-278,8554,4096,-278,12349,12294,-44,8284
,-58,8555,4096,12345,-274,8386,-61,8388,-60,8389
,-62,8390,12485,-58,8557,12421,-264,8558,-274,8386
,-61,8388,-60,8389,-62,8390,4096,-264,8559,-274
,8386,-61,8388,-60,8389,-62,8390,4096,12337,-256
,8314,-268,12354,-277,8326,-281,8325,-296,8322,-297
,8328,-298,8330,-299,8329,-43,8316,-45,8317,-59
,12354,-40,8324,-41,12354,-91,8327,-126,8319,-64
,8320,4096,12353,12356,12348,-296,8563,4096,-59,8302
,12376,12422,12427,12428,12332,12352,-41,8565,4096,12350
,-268,8566,4096,12355,12410,-1};

int yypact[] = {0,1,16,19,34,51,56,57,58,79
,100,121,122,125,130,135,140,145,145,148
,151,154,157,174,175,204,205,206,207,212
,213,216,217,220,221,226,227,228,233,234
,235,238,239,242,243,248,249,266,267,268
,275,278,279,280,281,282,287,288,319,322
,323,324,347,348,349,350,353,354,355,356
,357,358,367,394,429,367,430,431,394,432
,367,367,394,433,462,467,472,475,476,493
,494,367,497,516,497,519,350,367,497,497
,520,523,526,531,494,532,533,145,145,534
,537,538,539,394,540,541,568,569,572,575
,367,576,585,606,607,608,608,633,636,636
,653,654,725,367,730,636,731,760,761,762
,763,764,769,772,775,786,787,798,807,808
,813,476,814,367,815,818,825,828,833,844
,847,848,849,850,853,494,858,861,870,871
,872,872,881,882,883,884,887,888,899,902
,905,906,907,908,915,920,921,924,925,926
,931,932,367,945,946,608,608,955,956,961
,608,608,608,608,608,608,608,608,608,608
,964,977,990,991,994,997,541,998,367,1001
,1012,1015,1020,1021,1022,1033,367,394,1034,1035
,394,1068,367,1069,1076,1085,1088,1091,1092,1093
,1094,1107,1094,1112,1115,1118,1121,1124,1125,1126
,1127,1128,1129,1130,1131,1132,1133,1136,1139,1140
,367,1167,1168,1169,1178,1199,1220,1221,1222,1223
,1236,1249,1250,1251,1252,1253,1254,1267,1268,1281
,1286,1287,1296,1297,1298,367,1325,1334,1335,1338
,1339,1344,1345,1354,1355,367,1364,1365,1368,1369
,1370,497,1385,1404,1407,1418,1419,1424,1425,1426
,1427,1440,1441,1442,1451,1454,1487,1518,1545,1558
,1559,1560,1563,1564,1569,1570,1571,1572,1575,1576
,1609,497,367,1612,1613,1614,1617,1618,1619,367
,367,1620,497,1623,1624,1633,1634,1637,1642,1645
,1648,1653,1654,175,1663,1666,1677,497,1688,1689
,1722,1121,1723,1724,1725,1728,1731,1732,1733,1734
,1735,1736,1739,1740,1743,1744,-1};

int yyr1[] = {0,1,1,2,2,2,5,5,3,3
,4,4,4,7,7,7,7,7,8,13
,13,14,9,9,9,9,10,10,10,10
,11,11,11,11,16,16,16,16,17,17
,17,18,19,19,19,19,21,21,22,22
,23,24,25,26,26,26,28,28,27,27
,29,31,31,30,30,30,32,32,33,12
,12,12,35,35,35,36,38,37,37,39
,39,40,40,41,41,42,42,43,44,45
,6,46,46,48,47,47,47,47,47,47
,50,50,50,50,49,49,49,49,54,57
,57,57,57,58,59,59,55,55,56,51
,51,51,51,61,62,63,64,64,66,66
,34,34,67,65,65,52,52,52,68,70
,70,71,69,53,72,73,73,15,15,15
,15,76,76,76,76,76,76,75,75,75
,75,75,75,75,75,75,75,75,75,75
,77,77,77,78,78,78,78,78,78,78
,78,78,74,74,74,74,81,82,82,80
,80,83,83,60,60,84,84,84,84,84
,85,20,20,20,86,86,79,79,79,-1};

int yyr2[] = {0,3,1,3,6,2,1,3,2,1
,1,2,2,1,1,1,1,1,3,1
,3,1,5,5,2,2,5,5,2,2
,5,5,2,2,1,1,1,2,1,3
,3,4,4,4,7,1,0,1,1,3
,4,1,2,1,3,2,0,3,0,2
,3,1,3,1,3,2,0,4,2,4
,4,2,2,4,2,2,2,3,0,1
,3,4,1,1,0,2,4,0,0,1
,3,1,2,2,0,3,1,1,2,1
,1,1,1,1,1,1,2,1,4,1
,1,3,2,2,3,3,1,4,2,2
,4,6,10,3,1,2,1,3,0,3
,1,3,1,1,2,4,1,1,2,8
,8,1,4,2,2,2,3,1,3,3
,1,1,1,1,2,2,2,1,3,3
,3,3,3,3,3,3,3,3,2,2
,1,2,2,1,2,1,4,3,1,2
,3,2,1,1,3,2,2,3,3,1
,3,1,3,1,3,1,3,5,2,4
,1,1,2,2,1,1,1,1,1,-1};

int yygo[] = {0,-1,1,-1,2,45,99,-1,20,0
,3,-1,21,9,33,14,43,89,156,103
,183,164,240,329,349,-1,305,2,22,21
,53,45,22,-1,69,3,25,21,25,-1
,5,-1,6,-1,7,-1,8,-1,9,-1
,10,-1,36,11,37,64,115,95,175,-1
,57,71,120,74,143,79,145,80,146,90
,157,96,176,114,190,119,193,132,218,135
,223,152,233,191,262,215,190,217,280,225
,285,231,293,258,190,259,309,283,223,284
,312,294,317,331,351,338,354,339,355,-1
,291,93,174,97,177,98,178,300,321,301
,323,330,350,341,357,356,368,-1,158,299
,319,-1,159,-1,160,-1,161,239,296,241
,298,-1,165,-1,166,-1,322,-1,167,-1
,245,360,370,-1,302,-1,303,-1,325,326
,342,-1,304,-1,328,-1,343,-1,347,358
,369,-1,344,-1,345,328,346,358,346,-1
,289,-1,15,48,106,182,106,-1,16,17
,49,106,184,107,185,-1,47,48,107,182
,107,-1,17,-1,101,182,254,-1,102,-1
,103,-1,104,313,334,-1,142,364,372,-1
,313,-1,23,82,148,352,364,-1,54,56
,111,72,141,77,144,81,147,112,187,226
,286,229,292,315,336,-1,55,-1,56,-1
,58,-1,59,-1,66,-1,67,-1,68,-1
,62,-1,63,-1,65,-1,70,-1,78,-1
,84,215,278,-1,188,-1,72,-1,226,-1
,73,-1,287,-1,352,314,335,-1,288,316
,337,-1,290,-1,75,-1,76,-1,81,-1
,82,-1,77,-1,83,87,154,150,232,-1
,131,124,209,125,210,194,263,195,264,199
,268,200,269,201,270,202,271,203,272,204
,273,205,274,206,275,207,276,208,277,-1
,121,-1,195,-1,123,87,155,127,212,128
,213,134,219,150,155,-1,126,91,172,93
,172,97,172,98,172,169,172,170,172,239
,172,241,172,299,172,300,172,301,172,330
,172,341,172,356,172,-1,129,-1,220,-1
,139,-1,140,283,311,-1,222,258,308,-1
,189,309,332,-1,260,169,247,170,249,-1
,168,-1};

int yypgo[] = {0,1,3,5,9,13,27,35,41,43
,45,47,49,51,53,61,111,129,133,135
,137,143,145,147,149,151,155,157,159,163
,165,167,169,173,175,181,183,189,197,203
,205,209,211,213,217,221,223,229,247,249
,251,253,255,257,259,261,263,265,267,269
,271,275,277,279,281,283,285,289,293,295
,297,299,301,303,305,311,341,343,345,357
,387,389,391,393,397,401,405,-1};


yyEactr(__np__, var)
int __np__;
char *var;
{
return (1);
}
