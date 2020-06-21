/* @(#)pgram/pgram.y

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

/*
 * TERMINAL DECLARATIONS
 *
 * Some of the terminal declarations are out of the most natural
 * alphabetic order because the error recovery
 * will guess the first of equal cost non-terminals.
 * This makes, e.g. YTO preferable to YDOWNTO.
 */

%term
	FIRSTKEYWORD
	YAND		YARRAY		YBEGIN		YCASE
	YCONST		YDIV		YDO		Ydotdot
	YTO		YELSE		YEND		YFILE
	YFOR		YPROCEDURE	YGOTO		
	YIF		YIN				YLABEL
	YMOD		YNOT				YOF
	YOR		YPACKED		YNIL		YFUNCTION
	YPROGRAM	YRECORD		YREPEAT		YSET
	YOTHERWISE	YTHEN		YDOWNTO		YTYPE	
	YUNTIL		YVAR		YWHILE		YWITH
	LASTSTANDARDKEYWORD
	id		number		string		number
	character	Yillch 		Yassert
	LASTKEYWORD

%left '+' '-' YOR '|'
%left '*' '/' YDIV YMOD YAND '&'

%{
##
#include "../pgram/0.h"
#include "../pgram/yy.h"
#include "../pgram/f.decls.h"
#include "../common/machdep.h"

#ifdef PYRAMID_DOUBTFUL
#else
int EndCardNum;
#endif
int XformAll(long,int);

%}

%%

program:
	program_head block '.' = {
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
								CardNum [$2];
			NodeIndex = LinkChain (
				special (
					BeginOp,
					0,
					STab [STab [ProcIndex].SType].SBlock,
					CardNum [$2]),
				NodeIndex);
#endif
			NodeIndex = LinkChain (
				NodeIndex,
				CallPcstart (control ['r'], -1));
			if (cnt [$1] != 0) {
		   		NodeIndex = LinkChain (
					NodeIndex,
					EvalProgramParameters ($1));
			}
			/* PCLOSE is done in sql__exit */
			g1 [$2] = LinkChain (
				g1 [$2],
				CallPcexit (-1));
#ifdef PYRAMID_DOUBTFUL
#else
			g1 [$2] = LinkChain (
				g1 [$2],
				special (EndOp, 0, LevelNo, EndCardNum));
#endif
			g1 [$$] = ProcNode (
				Proc,
				STab [ProcIndex].SType,
				g1 [$2],
				NodeIndex,
				ProcIndex,
				CardNum [$2]);
			HdgHead = LinkChain (HdgHead, g1 [$$]);
		}
		ClearTemporarys ();
		PopScope ($1);
#ifdef PYRAMID_DOUBTFUL
#else
		XformAll(PopProcType,1);
#endif
	} |

	dcl_list = {
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
	} ;


program_head:
	YPROGRAM id ';' = {
		LevelNo = 0;  /* force LevelNo to '0' for YPROGRAM */
#ifdef PYRAMID_DOUBTFUL
		EntModule ($2, ModuleDcl);
#else
		PushProcType(&STab[ EntModule ($2, ModuleDcl) ]);
#endif
		EntBlock ($1);
		cnt [$$] = 0;
	} |

	YPROGRAM id '(' id_list ')' ';' = {
		LevelNo = 0;  /* force LevelNo to '0' for YPROGRAM */
#ifdef PYRAMID_DOUBTFUL
		EntModule ($2, ModuleDcl);
#else
		PushProcType(&STab[ EntModule ($2, ModuleDcl) ]);
#endif
		EntBlock ($1);
		StartCnt [$$] = StartCnt [$4];
		cnt [$$] = cnt [$4];
	} |

	YPROGRAM error = {
		yyPerror("Malformed program statement", PPROG);
			      /* Create a dummy entry for program */
		LevelNo = 0;  /* force LevelNo to '0' for YPROGRAM */
		val[$2] = EnterString("dummymain", 9);
#ifdef PYRAMID_DOUBTFUL
		EntModule ($2, ModuleDcl);
#else
		PushProcType(&STab[ EntModule ($2, ModuleDcl) ]);
#endif
		EntBlock ($1);
		cnt [$$] = 0;
	} ;


id_list:	id =
		{
		 StartCnt [$1] = IdPtr;
		 cnt [$1] = 1;
		 IdPtrIncr ();
		 IdList [IdPtr] = val [$1];
		} |

		id_list ',' id =
		{
		 cnt [$1] = cnt [$1]+1;
		 IdPtrIncr ();
		 IdList [IdPtr] = val [$3];
		} ;


block:

	dcl_list  begin_block = {
		MoveStacks ($2, $$);
	} |

	begin_block ;


dcl_list:
	dcl = {
		g1 [$$] = g1 [$1];
	} |

	dcl_list  dcl |
	
	dcl_list error = {
				yyPerror("Malformed declaration", PDECL);
			 }
	;


dcl:
	label_decl = { g1 [$$] = -1;
	
	} |

	const_decl = { g1 [$$] = -1;
	
	} |

	type_decl = { g1 [$$] = -1;
	
	} |

	var_decl = { g1 [$$] = -1;
	
	} |

	proc_dcl = { g1 [$$] = -1;
	
	} ;


label_decl:
	YLABEL label_list ';'
	;


label_list:
	label = {
		EntLabel ($1);
	} |

	label_list ',' label = {
		EntLabel ($3);
	} ;


label:
	number = {
		NodeIndex = val [$1];
		val [$$] = Node1parameter;
	} ;


const_decl:
	YCONST id '=' exp ';' = {
					EntConstant(val [$2],$4);
				}
	|

	const_decl id '=' exp ';' =
				{
					EntConstant(val [$2],$4);
				}
	|

	YCONST error = {
			yyPerror("Malformed const declaration", PDECL);
		       }
	|

	const_decl error =
			{
				yyPerror("Malformed const declaration", PDECL);
			}
	;



type_decl:
	YTYPE id '=' type_def ';' =
				{
					TypName($2,$4);
				}
	|

	type_decl id '=' type_def ';' =
				{
					TypName($2,$4);
				}
	|

	YTYPE error = {
			yyPerror("Malformed type declaration", PDECL);
		      }
	|

	type_decl error = {
				yyPerror("Malformed type declaration", PDECL);
			  }
	;


var_decl:
	YVAR id_list ':' type_def ';' =
			{
				EntVariable($2,$4,0);
				if (IsFile (TypeType [$4])) {
					SetFlag (ProcIndex, FileFlag);
				}
				IdPtr = StartCnt [$2];
			}
	|

	var_decl id_list ':' type_def ';' =
			{
				EntVariable($2,$4,0);
				if (IsFile (TypeType [$4])) {
					SetFlag (ProcIndex, FileFlag);
				}
				IdPtr = StartCnt [$2];
			}
	|

	YVAR error = {
			yyPerror("Malformed var declaration", PDECL);
		     }
	|

	var_decl error = {
				yyPerror("Malformed var declaration", PDECL);
			 }
	;


type_def:
	simple_type |

	string_type |

	structured_type |

	'^' id = {
		ChkTypeDefn (PointerType, $1, $2, -1);
	} ;


simple_type:	id =
		{
		 CheckType ($1);
		} |

		'(' id_list ')' =
		{
		 ChkTypeDefn (EnumerativeType, $1, $2, -1);
		 IdPtr = StartCnt [$2];
		} |

		const_exp Ydotdot const_exp =
		{
		 ChkTypeDefn (SubRangeType, $1, $3, -1);
		} ;


string_type:
	id '[' const_exp ']' = {
		ChkTypeDefn (StringType, $1, $3, -1);
	} ;


structured_type:
	packed YSET YOF simple_type = {
		ChkTypeDefn (SetType, $1, $4, -1);
	} |

	packed YFILE YOF type_def = {
		ChkTypeDefn (FileType, $1, $4, -1);
	} |

	packed YARRAY '[' array_domain ']' YOF type_def = {
		t1 = StartCnt [$4] + cnt [$4] - 1;
		if (t1 == StartCnt [$4]) {
			TypeType [$4] = ExpType [t1];
			SubType [$4] = ExpStype [t1];
			ChkTypeDefn (ArrayType, $4, $7, $1);
			MoveStacks ($4, $7);
			}
		else for (;;) {
			TypeType [$4] = ExpType [t1];
			SubType [$4] = ExpStype [t1];
			ChkTypeDefn (ArrayType, $4, $7, $1);
			if (t1 == StartCnt [$4]) break;
			t1--;
			MoveStacks ($4, $7);
			}
		ExpPtr = StartCnt [$4] - 1;
		MoveStacks ($4, $$);
	} |

	record_type;


packed:
	/* empty */ = {
		val [$$] = 0;
	} |

	YPACKED = {
		val [$$] = 1;
	} ;


array_domain:
	type_def = {
		IncrExpPtr ();
		cnt [$$] = 1;
		ExpType [ExpPtr] = TypeType [$1];
		ExpStype [ExpPtr] = SubType [$1];
		StartCnt [$$] = ExpPtr;
	} |

	array_domain ',' type_def = {
		IncrExpPtr ();
		cnt [$$] = cnt [$$] + 1;
		ExpType [ExpPtr] = TypeType [$3];
		ExpStype [ExpPtr] = SubType [$3];
	} ;


record_type:
	packed record_head field_list YEND = {
		CloseRec ();
		ChkTypeDefn (RecordType, $2, $3, -1);
		ProcMark = SaveProcmark [$2];
		HeadFieldLink = SaveReturnedType [$2];
		TailFieldLink = SaveParct [$2];
		MoveStacks ($2, $$);
	} ;


record_head:
	YRECORD = {
		OpenRec ();
		SaveProcmark [$$] = ProcMark;
		ProcMark = SymPtr + 1;
		SaveReturnedType [$$] = HeadFieldLink;
		SaveParct [$$] = TailFieldLink;
		HeadFieldLink = -1;
		TailFieldLink = -1;
	} ;


field_list:
	fixed_part variant_part = {
		g1 [$$] = HeadFieldLink;
		g2 [$$] = g1 [$2];
		NameofType [$$] = NameofType [$2];
	} ;


fixed_part:	field |

		fixed_part ';' field |
		
		fixed_part error = {
				yyPerror("Malformed record declaration");
		} ;


field:		/* empty */ |

		id_list ':' type_def  =
		{
		 EntField($1,$3);
		 IdPtr = StartCnt [$1];
		} ;


variant_part:	/* empty */ =
		{
		 g1 [$$] = -1;
		} |

		variant_case variant_list =
		{
		 HeadFieldLink = SaveReturnedType [$1];
		 TailFieldLink = SaveParct [$1];
		 g1 [$$] = g1 [$2];
		 ConstPtr = StartConst [TagPtr--];
		 NameofType [$$] = NameofType [$1];
		} ;


variant_case:	YCASE variant_tag YOF =
		{
		 SaveReturnedType [$$] = HeadFieldLink;
		 SaveParct [$$] = TailFieldLink;
		 HeadFieldLink = -1;
		 TailFieldLink = -1;
		 NameofType [$$] = NameofType [$2];
		} ;


variant_tag:
	id = {
		CheckType ($1);
		if (! IsOrdinal (TypeType [$1])) {
			ErrorMsg (1, ColNo [$1]);
		}
		TagType [++TagPtr] = SubType [$1];
		StartConst [TagPtr] = ConstPtr;
	} |

	id_list ':' id = {
		CheckType ($3);
		if (! IsOrdinal (TypeType [$3])) {
			ErrorMsg (1, ColNo [$3]);
		}
		if (cnt [$1] != 1) {
			ErrorMsg (20, ColNo [$1]);
		}
		EntField ($1, $3);
		IdPtr = StartCnt [$1];
		TagType [++TagPtr] = SubType [$3];
		StartConst [TagPtr] = ConstPtr;
		NameofType [$$] = NameofType [$3];
	} ;


variant_list:	variant =
		{
		 g1 [$$] = HeadFieldLink;
		} |

		variant_list ';' variant |
		
		variant_list error = {
				yyPerror("Malformed record declaration", PDECL);
		} ;


variant:	/* empty */ |

		variant_head '(' field_list ')' =
		{
		 HeadFieldLink = SaveReturnedType [$1];
		 TailFieldLink = SaveParct [$1];
		 EntVariant ($1, $3);
		} ;


variant_head:	case_const_list ':' =
		{
		 CheckCaseConst ($1);
		 ExpPtr = StartCnt [$1] - 1;
		 SaveReturnedType [$$] = HeadFieldLink;
		 SaveParct [$$] = TailFieldLink;
		 HeadFieldLink = -1;
		 TailFieldLink = -1;
		} ;


proc_dcl:

	proc_name ';' block ';' = {
		if (ErrorCount == 0) {
			if (STab [ProcIndex].SKind == ProcDcl) {
				t3 = g1 [$3];
			}
			else {
				/* create function variable */
				GetString (val [$1], ProcName);
				sprintf(ss,FUNCVAR_NAME,ProcName);		/* DK 26.08.88 */
				t1 = EnterString (ss, GetStringLength (val [$1]) + 1);
				AllocNameSpace (t1);
				val [$2] = t1;
				t1 = IdLookup ($2);
				if (!GetFlag (t1, ModifiedFlag)) {
					ErrorMsg (129, -1);
				}
				if (IsStructured (ReturnedType)) {
					sprintf(ss,STRUCTFUNCVAR_NAME,ProcName);	/* DK 26.08.88 */
					t2 = EnterString (ss, GetStringLength (val [$1]) + 2);
					AllocNameSpace (t2);
					val [$2] = t2;
					t2 = IdLookup ($2);
					t3 = GenStore (
						node3 (IdOp, SubType [$1], t2, 0),
						TypeType [$1],
						SubType [$1],
						-1,
						UnaryExpression (
							LoadOp,
							TypeType [$1],
							node3 (IdOp, SubType [$1], t1, 0),
							0,
							0),
						TypeType [$1],
						SubType [$1],
						-1,
						CardNum [$1]);
					t2 = LongStatement (
						RtrnxOp,
						AnyPtrPtr,
						-1,
						UnaryExpression (
							LoadadrOp,
							TypeType [$1],
							node3 (IdOp, SubType [$1], t2, 0),
							0,
							0),
						ProcIndex,
						0,
#ifdef PYRAMID_DOUBTFUL
						CardNum [$1]);
#else
						-1);
#endif
					t3 = LinkChain (t3, t2);
				}
				else {
					t3 = LongStatement (
						RtrnxOp,
						TypeType [$1],
						-1,
						UnaryExpression (
							LoadOp,
							TypeType [$1],
							node3 (IdOp, SubType [$1], t1, 0),
							0,
							0),
						ProcIndex,
						0,
#ifdef PYRAMID_DOUBTFUL
						CardNum [$1]);
#else
						-1);
#endif
				}
				t3 = LinkChain (g1 [$3], t3);
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
								CardNum [$3];
#endif
			t3 = LinkChain (
				special (
					BeginOp,
					0,
					STab [STab [ProcIndex].SType].SBlock,
					CardNum [$3]),
				t3);
			t3 = LinkChain (
				t3,
#ifdef PYRAMID_DOUBTFUL
				special (EndOp, 0, LevelNo, CardNum [$3]));
#else
				special (EndOp, 0, LevelNo, EndCardNum));
#endif
			g1 [$$] = ProcNode (
				Proc,
				STab [ProcIndex].SType,
				t3,
				-1,
				ProcIndex,
				CardNum [$3]);
			HdgHead = LinkChain (HdgHead, g1 [$$]);
		}
		ClearTemporarys ();
		PopScope ($1);
#ifdef PYRAMID_DOUBTFUL
#else
		PopProcType;
#endif /* PYRAMID_DOUBTFUL */
	} |

	proc_name ';' id ';' = {
		CheckForwardDefn ($3);
		PopScope ($1);
#ifdef PYRAMID_DOUBTFUL
#else
		PopProcType;
#endif
	} |
	
	proc_name error ;


proc_name:

	proc_head parameters = {
#ifdef PYRAMID_DOUBTFUL
		DclProc ($1, $2, ProcDcl, IntegerPtr);
#else
		PushProcType(&STab[ DclProc ($1, $2, ProcDcl, IntegerPtr) ]);
#endif
		EntBlock ($1);
		ParmPtr = StartCnt [$2];
	} |

	func_head parameters ':' id  = {
		CheckType ($4);
#ifdef PYRAMID_DOUBTFUL
		DclProc ($1, $2, FuncDcl, SubType [$4]);
#else
		PushProcType(&STab[ DclProc ($1, $2, FuncDcl, SubType [$4])]);
#endif
		EntBlock ($1);
		/* create function variable */
		GetString (val [$1], ProcName);
		sprintf(ss,FUNCVAR_NAME,ProcName);		/* DK 26.08.88 */
		t1 = EnterString (ss, GetStringLength (val [$1]) + 1);
		AllocNameSpace (t1);
		EntSymbol (t1,SubType[$4],VarDcl,CardNum[$1]);
		STab [SymPtr].SClass = AUTO;
		if (IsStructured (TypeType [$4])) {
			ErrorMsg (149, ColNo [$4]);
		}
		ParmPtr = StartCnt [$2];
	} |

	func_head parameters = {
		t1 = FindSymbol (val [$1]);
		if (t1 == -1) {
			ErrorMsg (28, ColNo [$1]);
			t1 = IntegerPtr;
		}
		else {
			t1 = STab [STab [t1].SType].SResult;
		}
		if (cnt [$2] != 0) {
			ErrorMsg (28, ColNo [$2]);
			cnt [$2] = 0;
		}
#ifdef PYRAMID_DOUBTFUL
		DclProc ($1, $2, FuncDcl, t1);
#else
		PushProcType(&STab[ DclProc ($1, $2, FuncDcl, t1)]);
#endif
		EntBlock ($1);
		/* create function variable */
		GetString (val [$1], ProcName);
		sprintf(ss,FUNCVAR_NAME,ProcName);	/* DK 26.08.88 */
		t2 = EnterString (ss, GetStringLength (val [$1]) + 1);
		AllocNameSpace (t2);
		EntSymbol (t2,t1,VarDcl,CardNum[$1]);
		STab [SymPtr].SClass = AUTO;
		if (IsStructured (PrimType (t1))) {
			/* create structured return spot */
			sprintf(ss,STRUCTFUNCVAR_NAME,ProcName);	/* DK 26.08.88 */
			t2 = EnterString (ss, GetStringLength (val [$1]) + 2);
			AllocNameSpace (t2);
			EntSymbol (t2,t1,VarDcl,CardNum[$1]);
			STab [SymPtr].SClass = LOCALSTATIC;
		}
		ParmPtr = StartCnt [$2];
	} ;


proc_head:

	YPROCEDURE id = {
		MoveStacks ($2, $$);
		HdgFillList = -1;
	} ;


func_head:

	YFUNCTION id = {
		MoveStacks($2,$$);
		HdgFillList = -1;
	} ;


parameters:

	'(' parameter_list ')' = {
		StartCnt [$$] = StartCnt [$2];
		cnt [$$] = cnt [$2];
	} |

	/* empty */ = {
		StartCnt [$$] = ParmPtr;
		cnt [$$] = 0;
	} ;


parameter_list:

	formal_parameter = {
		StartCnt [$$] = StartCnt [$1];
		cnt [$$] = cnt [$1];
	} |

	parameter_list ';' formal_parameter = {
		cnt [$$] = cnt [$1] + cnt [$3];
	} ;


formal_parameter:

	passed_by id_list ':' id = {
		CheckType ($4);
		StartCnt [$$] = ParmPtr;
		cnt [$$] = EntParameterList ($1, $2, $4, ColNo [$4]);
		IdPtr = StartCnt [$2];
	} |

	formal_proc = {
		StartCnt [$$] = ParmPtr;
		cnt [$$] = EntProcParm ($1);
	} ;


passed_by:

	YVAR = {
		TypeType [$$] = ByRefParm;
	} |

	/* empty */ = {
		TypeType [$$] = ByValueParm;
	} ;


formal_proc:

	proc_head parameters = {
		FormalProcType ($1, $2, ProcedureType, 0, IntegerPtr);
		ParmPtr = StartCnt [$2];
	} |

	func_head parameters ':' id = {
		CheckType ($4);
		if (IsStructured (TypeType [$4])) {
			ErrorMsg (1, ColNo [$4]);
		}
		FormalProcType($1,$2,FunctionType,NameofType[$4],SubType [$4]);
		ParmPtr = StartCnt [$2];
	} ;


push_root:	/* empty */ =
		{
		 g1 [$$] = HdgHead;
		 HdgHead = -1;
		} ;


pop_root:	/* empty */ =
		{
		 HdgHead = g1 [$-2];
		} ;


	/* begin_block:
	push_root YBEGIN statement_list pop_root YEND = {
		g1 [$$] = g1 [$3];
		CardNum [$$] = CardNum [$2];
	} ;
	*/

begin:
	YBEGIN = {
		g1 [$$] = HdgHead;
		HdgHead = -1;
	};


begin_block:
	begin statement_list YEND = {
		HdgHead = g1 [$1];
		g1 [$$] = g1 [$2];
		CardNum [$$] = CardNum [$1];
#ifdef PYRAMID_DOUBTFUL
#else
		EndCardNum = CardNum[$3];
#endif
	} ;
	


statement_list:
	statement = {
		HdgHead = g1 [$1];
		g1 [$$] = HdgHead;
	} |

	statement_lsth statement = {
		HdgHead = LinkChain (HdgHead, g1 [$2]);
		g1 [$$] = HdgHead;
	} ;


statement_lsth:
	statement_list ';' = {
			t1 = g1 [$1];
			if (t1 != -1 && t1 != 0) {
				t2 = TailLinkOf (g1 [$1]);
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
	} ;


statement:
	/* empty */ = {
		g1 [$$] = -1;
	} |

	label ':' statement = {
		g1 [$$] = LinkChain (LabDef ($1), g1 [$3]);
	} |

	simple_stmnt |

	struc_stmnt |
	
	id error = {
			g1 [$$] = -1;
			TypeType[$$] = VoidType;
			SubType[$$] = VoidType;
			yyPerror("Malformed statement", PSTAT);
		   }
	|

	error = {
			g1 [$$] = -1;
			yyPerror("Malformed statement", PSTAT);
		}
	;



struc_stmnt:	conditional |

		loop |

		with_stmnt |

		begin_block;


simple_stmnt:
	assignment |

	procedure_call = {
		check_call (g1 [$1], ColNo [$1]);
	} |

	YGOTO label = {
		g1 [$$] = LabRef ($2);
	} |

	assert;


assignment:
	lval ':' '=' exp = {
		check_call (g1 [$4], ColNo [$4]);
		g1 [$$] = GenStore (
			g1 [$1], TypeType [$1], SubType [$1], ColNo [$1],
			g1 [$4], TypeType [$4], SubType [$4], ColNo [$4],
			CardNum [$1]);
	} ;


lval:
	id = {
		t1 = IdLookup ($1);
		if (t1 != -1  ) {
			if (STab [t1].SKind == FuncDcl) {
				/* assign to function variable */
				t3 = STab [t1].SName;
				sprintf(ss,FUNCVAR_NAME,GetStringPtr(t3));		/* DK 26.08.88 */
				t2 = EnterString (ss, GetStringLength (t3) + 1);
				AllocNameSpace (t2);
				val [$1] = t2;
				t1 = IdLookup ($1);
				AccessVariable (t1, $1, -1);
				SetFlag (t1, ModifiedFlag);
			}
			else if (STab [t1].SKind == ConDcl) {
				ErrorMsg (18, ColNo [$1]);
				TypeType [$$] = VoidType;
				SubType [$$] = VoidType;
				g1 [$$] = -1;
			}
			else {
				AccessVariable (t1, $1, -1);
			}
		}
		else {
			g1 [$$] = -1;
		}
	} |

	lvalarray |

	lval '.' id = {
		QualifiedVar ($1, $3);
	} |

	lval '^' = {
		g1 [$1] = UnaryExpression (
			LoadOp,
			TypeType [$1],
			g1 [$1],
			0,
			0);
		DerefVar ($1);
	} ;


lvalarray      :lvalarrayhead ']';


lvalarrayhead:
	lval '[' exp = {
		check_call (g1 [$3], ColNo [$3]);
		IndexedVar ($1, $3);
	} |

	lvalarrayhead ',' exp = {
		check_call (g1 [$3], ColNo [$3]);
		IndexedVar ($1, $3);
	} ;


procedure_call:
	id = {
		t1 = IdLookup ($1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				cnt [$1] = 0;
				if(IsProcedure(STab [t1].SSysSpecific)) {
				   EvalSys (SprocCall, STab [t1].SSysSpecific,
				       $1, $1);
				} else {
				   switch (yychar) {

				   case ';':
				   case YEND:
				   case YELSE:
				   case YUNTIL:
					   ErrorMsg (26, ColNo [$1]);
					   break;

				   default:
					   /* syntax error will be */
					   /* caught by yacc       */
					   break;
				   }
				   g1 [$$] = -1;
				   TypeType [$$] = VoidType;
				   SubType [$$] = VoidType;
				}
				break;

			case ProcDcl:
			case ForwardProcDcl:
			case ProcParmDcl:
				GenCall (t1, $1, -1);
				break;

			default:
				switch (yychar) {

				case ';':
				case YEND:
				case YELSE:
				case YUNTIL:
					ErrorMsg (26, ColNo [$1]);
					break;

				default:
					/* syntax error will be */
					/* caught by yacc       */
					break;
				}
				g1 [$$] = -1;
				TypeType [$$] = VoidType;
				SubType [$$] =VoidType;
				break;
			}
		}
	} |

	id '(' w_exp_list ')' = {
		t1 = IdLookup ($1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				cnt [$1] = 0;
				if(IsProcedure(STab [t1].SSysSpecific)) {
				   EvalSys (SprocCall, STab [t1].SSysSpecific,
				      $1, $3);
				} else {
				   ErrorMsg (26, ColNo [$1]);
				   g1 [$$] = -1;
				   TypeType [$$] = VoidType;
				   SubType [$$] = VoidType;
				}
				break;

			case ProcDcl:
			case ForwardProcDcl:
			case ProcParmDcl:
				GenCall (t1, $1, $3);
				break;

			default:
				ErrorMsg (26, ColNo [$1]);
				g1 [$$] = -1;
				TypeType [$$] = VoidType;
				SubType [$$] =VoidType;
				break;
			}
		}
		ExpPtr = StartCnt [$3] - 1;
	} ;


assert:
	Yassert exp = {
		check_call (g1 [$2], ColNo [$2]);
		g1 [$$] = EvalAssert ($1, $2);
	} ;


conditional:
	if_clause statement = {
		g1 [$$] = ComplexStmnt (
			ifthenelse,
			0,
			g1 [$1],
			g1 [$2],
			-1,
			CardNum [$1]);
	} |

	if_clause statement else_clause statement = {
		g1 [$$] = ComplexStmnt (
			ifthenelse,
			0,
			g1 [$1],
			g1 [$2],
			g1 [$4],
			CardNum [$1]);
	} |

	case_clause push_root YOF case_body pop_root YEND = {
		if (! CompatibleTypes (TypeType [$1], TypeType [$4])) {
			ErrorMsg (1, ColNo [$1]);
		}
		NodeIndex = g1 [$1];
		if ( ! control['y']) {
		  g1 [$6] = EvalConvert ( TypeType [$1], CallCaseErr ($1));
		  g1 [$4] = LinkChain (g1 [$4], BuildDefault ($1, $6));
		}
		SwitchBody = g1 [$4];
		CaseRoot = val [$1];
		g1 [$$] = g1 [$1];
	} |

	case_clause push_root YOF case_body pop_root
	push_root otherwise_opt_colon statement_list pop_root YEND = {
		if (! CompatibleTypes (TypeType [$1], TypeType [$4])) {
			ErrorMsg (1, ColNo [$1]);
		}
		g1 [$4] = LinkChain (g1 [$4], BuildDefault ($7, $8));
		NodeIndex = g1 [$1];
		SwitchBody = g1 [$4];
		CaseRoot = val [$1];
		g1 [$$] = g1 [$1];
	} ;


if_clause:
	YIF exp YTHEN = {
		check_call (g1 [$2], ColNo [$2]);
		if (TypeType [$2] != BooleanPtr) {
			ErrorMsg (1, ColNo [$2]);
		}
		MoveStacks($2,$$);
	} ;

else_clause:
	YELSE = {
	} ;


case_clause:
	YCASE exp = {
		check_call (g1 [$2], ColNo [$2]);
		g1 [$$] = ComplexStmnt (
			Switch,
			0,
			EvalConvert (IntegerPtr, g1 [$2]),
			-1,
			-1,
			CardNum [$1]);
		TypeType [$$] = TypeType [$2];
		val[$$] = CaseRoot;
		CaseRoot = -1;
	} ;


case_body:
	case_stmnt = {
		HdgHead = g1 [$1];
		g1 [$$] = HdgHead;
	} |

	case_body  ';' case_stmnt = {
		if ((g1 [$1] != -1) && (g1 [$3] != -1)) {
			if (TypeType [$1] != TypeType [$3]) {
				ErrorMsg (1, ColNo [$3]);
			}
			HdgHead = LinkChain (HdgHead, g1 [$3]);
			g1 [$$] = HdgHead;
		}
	} ;


case_stmnt:	/* empty */ =
		{
		 g1 [$$] = -1;
		} |

		case_const_list ':' statement  =
		{
		 g1 [$$] = BuildCase ($1, $3);
		 ExpPtr = StartCnt [$1] - 1;
		} ;


case_const_list:
		case_const =
		{
		 IncrExpPtr ();
		 StartCnt [$$] = ExpPtr;
		 ExpType [ExpPtr] = TypeType [$1];
		 ExpStype [ExpPtr] = SubType [$1];
		 ExpVal [ExpPtr] = g1 [$1];
		 ExpColNo [ExpPtr] = ColNo [$1];
		 IncrExpPtr ();
		 ExpVal [ExpPtr] = g2 [$1];
		 ExpColNo [ExpPtr] = ColNo [$1];
		 cnt [$$] = 1;
		} |

		case_const_list ',' case_const =
		{
/*               if (TypeType [$1] != TypeType [$3]) {  */
/*  		    ErrorMsg (1, ColNo [$3]);  */
/*  		    }  */
		 IncrExpPtr ();
		 ExpType [ExpPtr] = TypeType [$3];
		 ExpStype [ExpPtr] = SubType [$3];
		 ExpVal [ExpPtr] = g1 [$3];
		 ExpColNo [ExpPtr] = ColNo [$3];
		 IncrExpPtr ();
		 ExpVal [ExpPtr] = g2 [$3];
		 ExpColNo [ExpPtr] = ColNo [$3];
		 cnt [$$] = cnt [$1] + 1;
		} ;

case_const:	exp =
		{
		 BuildCaseConst ($1, $1);
		} ;


otherwise_opt_colon:

	YOTHERWISE |

	YOTHERWISE ':';


loop:
	YWHILE exp YDO statement = {
		check_call (g1 [$2], ColNo [$2]);
		if (TypeType [$2] != BooleanPtr) {
			ErrorMsg (1, ColNo [$2]);
		}
		MoveStacks($2,$$);
		g1 [$$] = ComplexStmnt(loop, 0, g1 [$1],g1 [$4],-1,CardNum [$1]);
#ifdef RASX /*RAS05 pop loop scope pushed in lexer */
                PopLoopScope();
#endif
	} |

	for_statement |

	repeat_statement ;


for_statement:	for_head statement =
		{
		 EvalFor ($1, $2);
		} ;


for_head:
	YFOR id ':' '=' exp YTO exp YDO = {
		check_call (g1 [$5], ColNo [$5]);
		check_call (g1 [$7], ColNo [$7]);
		EvalTo ($1, $2, $5, $7);
	} |

	YFOR id ':' '=' exp YDOWNTO exp YDO = {
		check_call (g1 [$5], ColNo [$5]);
		check_call (g1 [$7], ColNo [$7]);
		EvalDownto ($1, $2, $5, $7);
	} ;


repeat:
	YREPEAT = {
		g1 [$$] = HdgHead;
		HdgHead = -1;
	} ;


repeat_statement:
	repeat statement_list YUNTIL exp = {
		HdgHead = g1 [$1];
		check_call (g1 [$4], ColNo [$4]);
		if (TypeType [$4] != BooleanPtr) {
			ErrorMsg (1, ColNo [$4]);
		}

		g1 [$$] = ComplexStmnt (
			RepeatUntil,
			0,
			g1 [$4],
			g1 [$2],
			-1,
			CardNum [$1]);
#ifdef RASX /*RAS05 pop loop scope pushed in lexer */
                PopLoopScope();
#endif
	} ;


with_stmnt:	with_head statement =
		{
		 EndWith ($1);
		 g1 [$$] = LinkChain (g1 [$1], g1 [$2]);
		} ;


with_head:	with_list YDO;


with_list:	YWITH vref =
		{
		 val [$1] = 0;
		 g1 [$1] = -1;
		 EvalWith ($1, $2);
		 ForWithLevel += 1;
		} |

		with_list ',' vref =
		{
		 EvalWith ($1, $3);
		} ;


exp:
	arith_exp |

	exp YIN arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalIn ($1, $3);
		ColNo [$$] = ColNo [$2];
	} |

	exp relation arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalRelational ($1,$3,SaveProcmark [$2]);
		ColNo [$$] = ColNo [$2];
	} |

	error = {
			g1 [$$] = -1;
			yyPerror("Missing or malformed expression", PEXPR);
		}
	;


relation:	'=' =
		{
		 SaveProcmark [$$] = EqOp;
		} |

		'<' =
		{
		 SaveProcmark [$$] = LtOp;
		} |

		'>' =
		{
		 SaveProcmark [$$] = GtOp;
		} |

		'<' '=' =
		{
		 SaveProcmark [$$] = LeOp;
		} |

		'>' '=' =
		{
		 SaveProcmark [$$] = GeOp;
		} |

		'<' '>' =
		{
		 SaveProcmark [$$] = NeOp;
		} ;

arith_exp:

	exp_part |

	arith_exp '+' arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalBinary ($1,$3,AddOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp '-' arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalBinary ($1,$3,SubtOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp '*' arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalBinary ($1,$3,MultOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp '/' arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalDivide ($1,$3,DivOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp YDIV arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalModDiv ($1,$3,DivOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp YMOD arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalModDiv ($1,$3,ModuloOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp '&' arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalBinary ($1,$3,AndOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp '|' arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalBinary ($1,$3,OrOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp YAND arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalBinary($1,$3,AndOp);
		ColNo [$$] = ColNo [$2];
	} |

	arith_exp YOR arith_exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		EvalBinary($1,$3,OrOp);
		ColNo [$$] = ColNo [$2];
	} |

	'+' arith_exp = {
		check_call (g1 [$2], ColNo [$2]);
		t1 = STab [TypeFieldOf (g1 [$2])].SType;
		if (t1 != IntegerType &&
		    t1 != ShortType   &&
		    t1 != UshortType  &&
		    t1 != UlongType   &&
		    t1 != LongType    &&
		    t1 != FloatType   &&
		    t1 != DoubleType )
		    ErrorMsg (1, ColNo [$2]);
		MoveStacks($2,$$);
	} |

	'-' arith_exp = {
		check_call (g1 [$2], ColNo [$2]);
		EvalUnary($2,UminusOp);
		MoveStacks($2,$$);
	} ;


exp_part:

	primary |

	'~' primary = {
		check_call (g1 [$2], ColNo [$2]);
		EvalUnary($2,NotOp);
		MoveStacks($2,$$);
	} |

	'@' primary = {
		GenAddress ($$, $2);
	} ;


primary:
	constant |

	id error = {
			g1 [$$] = -1;
			yyPerror("Missing or malformed expression", PEXPR);
		   }
	|

	vref = {
		NodeIndex = g1 [$1];
		if (
			(MinorOperator != LitOp) &&
			(MinorOperator != CallOp) &&
			(MinorOperator != Sys0Op)
		) {
			g1 [$$] = UnaryExpression (
				LoadOp,
				TypeType [$1],
				g1 [$1],
				0,
				0);
			if (SubType [$1] == BooleanPtr && control ['r'])
				g1 [$$] = RangeCheck (SubType [$1], g1 [$$]);
			SubType [$$] = SubType [$1];
			TypeType [$$] = TypeType [$1];
		}
	} |

	id '(' w_exp_list ')' = {
		t1 = IdLookup ($1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				EvalSys (SfuncCall, STab [t1].SSysSpecific, $1, $3);
				break;

			case FuncDcl:
			case ForwardFuncDcl:
			case FuncParmDcl:
				GenCall (t1, $1, $3);
				break;

			default:
				ErrorMsg (34, ColNo [$1]);
				g1 [$$] = -1;
				TypeType [$$] = VoidType;
				SubType [$$] = VoidType;
				break;
			}
		}
		ExpPtr = StartCnt [$3] - 1;
	} |

	'(' exp ')' = {
		check_call (g1 [$2], ColNo [$2]);
		MoveStacks ($2, $$);
	} |

	YNIL = {
		g1 [$$] = node3 (LitOp, AnyPtrPtr, 0, 0);
		TypeType [$$] = AnyPtrPtr;
		SubType [$$] = AnyPtrPtr;
	} |

	YNOT primary = {
		check_call (g1 [$2], ColNo [$2]);
		EvalUnary($2,NotOp);
		MoveStacks($2,$$);
	} |

	'[' s_exp_list ']' = {
		EvalSetConstructor ($$, $2);
	} |

	'[' ']' = {
		g1 [$2] = MakeList (ExpList, 0, -1, -1, -1);
		SetMinor (g1 [$2], ListOp);
		EvalSetConstructor ($$, $2);
	} ;


vref:
	id = {
		t1 = IdLookup ($1);
		if (t1 != -1) {
			switch (STab [t1].SKind) {

			case SysProcDcl:
				cnt [$1] = 0;
				EvalSys (SfuncCall, STab [t1].SSysSpecific, $1, $1);
				break;

			case ProcDcl:
			case ForwardProcDcl:
			case ProcParmDcl:
			case FuncDcl:
			case ForwardFuncDcl:
			case FuncParmDcl:
				GenCall (t1, $1, -1);
				break;

			default:
				AccessVariable (t1, $1, -1);
				break;
			}
		}
		else {
			g1 [$$] = -1;
		}
	} |

	arrayref |

	vref '.' id = {
		QualifiedVar ($1, $3);
	} |

	primary '^' = {
		DerefVar ($1);
	} ;


arrayref:
	arrayhead ']' ;


arrayhead:
	vref '[' exp = {
		check_call (g1 [$3], ColNo [$3]);
		IndexedVar ($1, $3);
	} |

	arrayhead ',' exp = {
		check_call (g1 [$3], ColNo [$3]);
		IndexedVar ($1, $3);
	} ;


s_exp_list:
	s_exp = {
		NodeIndex = MakeList (ExpList, TypeType [$1], g1 [$1], -1, -1);
		SetMinor (NodeIndex, ListOp);
		if (
			!(IsConstant (g1 [$1])  ||
			  IsConstantExpr ( g1 [$1]) ||
			  (MajorOperatorOf (g1 [$1]) == ExpList &&
			   MinorOperatorOf (g1 [$1]) == ListOp)
			)
		) {
			SetMinor (NodeIndex, VarCtOp);	
		}
		g1 [$$] = NodeIndex;
	} |

	s_exp_list ',' s_exp = {
/*  This looks redundant with the IsOrdinal test performed in the s_exp
 *  reduction
 *  Not redundant for -q - Flag, WB 18-05-88
 *              if (TypeType [$1] != TypeType [$3]) {
 */
		if (TypeType [$1] != TypeType [$3] &&
		!((TypeType [$1] == IntegerPtr ||
		TypeType [$1] == LongPtr ||
		TypeType [$1] == BytePtr ||
		TypeType [$1] == ShortPtr ||
		TypeType [$1] == UbytePtr ||
		TypeType [$1] == UshortPtr) &&
		(TypeType [$3] == IntegerPtr ||
		TypeType [$3] == LongPtr ||
		TypeType [$3] == BytePtr ||
		TypeType [$3] == ShortPtr ||
		TypeType [$3] == UbytePtr ||
		TypeType [$3] == UshortPtr))) {
			ErrorMsg (0, ColNo [$3]);
		}
		AppendList (ExpList, TypeType [$3], g1 [$1], g1 [$3]);
		if (
			!(IsConstant (g1 [$3]) ||
			  IsConstantExpr (g1 [$3]) ||
			  (MajorOperatorOf (g1 [$3]) == ExpList &&
			   MinorOperatorOf (g1 [$3]) == ListOp)
			)
		) {
			SetMinor (g1 [$$], VarCtOp);
		}
	} ;


s_exp:
	exp = {
		check_call (g1 [$1], ColNo [$1]);
		if (!IsOrdinal (TypeType [$1])) {
			ErrorMsg (1, ColNo [$1]);
			TypeType [$1] = VoidType;
		}
		else {
			g1 [$1] = EvalConvert (IntegerPtr, g1 [$1]);
		}
	} |

	exp Ydotdot exp = {
		check_call (g1 [$1], ColNo [$1]);
		check_call (g1 [$3], ColNo [$3]);
		if (!IsOrdinal (TypeType [$1])) {
			ErrorMsg (1, ColNo [$1]);
			TypeType [$1] = VoidType;
		}
		else {
			g1 [$1] = EvalConvert (IntegerPtr, g1 [$1]);
		}
		if (!IsOrdinal (TypeType [$3])) {
			ErrorMsg (1, ColNo [$3]);
			TypeType [$3] = VoidType;
		}
		else {
			g1 [$3] = EvalConvert (IntegerPtr, g1 [$3]);
		}
		if (!CompatibleTypes (TypeType [$1], TypeType [$3])) {
			ErrorMsg (0, ColNo [$3]);
		}
		NodeIndex = MakeList (ExpList, TypeType [$1], g1 [$1], g1 [$3], -1);
		if ((IsConstant (g1 [$1]) || IsConstantExpr(g1 [$1])) &&
		    (IsConstant (g1 [$3]) || IsConstantExpr(g1 [$3])))
		{
			SetMinor (NodeIndex, ListOp);
		} else {
			SetMinor (NodeIndex, VarCtOp);
		}
		g1 [$$] = NodeIndex;
	} ;


w_exp_list:
	w_exp = {
		IncrExpPtr ();
		cnt [$$] = 1;
		ExpType [ExpPtr] = TypeType [$1];
		ExpStype [ExpPtr] = SubType [$1];
		ExpVal [ExpPtr] = g1 [$1];
		ExpColNo [ExpPtr] = ColNo [$1];
		StartCnt [$$] = ExpPtr;
		ColNo [$$] = ColNo [$1];
	} |

	w_exp_list ',' w_exp = {
		IncrExpPtr ();
		cnt [$$] = cnt [$1]+1;
		ExpType [ExpPtr] = TypeType [$3];
		ExpStype [ExpPtr] = SubType [$3];
		ExpVal [ExpPtr] = g1 [$3];
		ExpColNo [ExpPtr] = ColNo [$3];
	} ;


w_exp:
	exp |

	exp ':' exp = {
		if (!IsIntegral (TypeType [$3])) {
			ErrorMsg (30, ColNo [$3]);
		}
		t1 = EvalConvert (IntegerPtr, g1 [$3]);
		if (control ['r']) {
		    t1 = RangeCheck (WFormatPtr, t1);
		}
		g1 [$$] = UnaryExpression (
			WexpOp,
			SubType [$1],
			g1 [$1],
			t1,	
			-1);
	} |

	exp ':' exp ':' exp = {
		if (!IsIntegral (TypeType [$3])) {
			ErrorMsg (30, ColNo [$3]);
		}
		if (!IsIntegral (TypeType [$5])) {
			ErrorMsg (30, ColNo [$5]);
		}
		if (TypeType [$1] != FloatPtr &&
		    TypeType [$1] != LongPtr &&
		    TypeType [$1] != DoublePtr) {
			ErrorMsg (30, ColNo [$5]);
		}
		t1 = EvalConvert (IntegerPtr, g1 [$3]);
		t2 = EvalConvert (IntegerPtr, g1 [$5]);
		if (control ['r']) {
		    t1 = RangeCheck (WFormatPtr, t1);
		    t2 = RangeCheck (WFormatPtr, t2);
		}
		g1 [$$] = UnaryExpression (
			WexpOp,
			SubType [$1],
			g1 [$1],
			t1,
			t2);
	} |

	exp octhex = {
		g1 [$$] = UnaryExpression (
			WocthexOp,
			SubType [$1],
			g1 [$1],
			-1,
			val [$2]);
	} |

	exp ':' exp octhex = {
		if (!IsIntegral (TypeType [$3])) {
			ErrorMsg (30, ColNo [$3]);
		}
		t1 = EvalConvert (IntegerPtr, g1 [$3]);
		if (control ['r']) {
		    t1 = RangeCheck (WFormatPtr, t1);
		}
		g1 [$$] = UnaryExpression (
			WocthexOp,
			SubType [$1],
			g1 [$1],
			t1,
			val [$4]);
	} ;


octhex:
	id = {
		if ((val [$1] != OctString) && (val [$1] != HexString)) {
			ErrorMsg (28, ColNo [$1]);
			val [$$] = HexString;
		}
	} ;


const_exp:	const_primary |

		'+' const_primary =
		{
		 MoveStacks ($2, $$);
		} |

		'-' const_primary =
		{
		 EvalUnary ($2, UminusOp);
		 MoveStacks ($2, $$);
		} ;


const_primary:
	constant |

	id = {
		t1 = IdLookup ($1);
		AccessVariable (t1, $1, -1);
	} ;


constant:	number =
		{
		 NodeIndex = val [$1];
		 TypeType [$$] = TypeField;
		 SubType [$$] = TypeField;
		 g1 [$$] = NodeIndex;
		 NameofConst [$$] = GenConstName(Node1parameter);
		} |

		character =
		{
		 TypeType [$$] = CharPtr;
		 SubType [$$] = CharPtr;
		 g1 [$$] = node3 (LitOp, CharPtr, val [$1], 1);
		 NameofConst [$$] = GenCharName(val [$1]);
		} |

		string =
		{
		 TypeType [$$] = PacPtr;
		 SubType [$$] = PacPtr;
		 g1 [$$] = node3 (
		    LitOp,
		    PacPtr,
		    val [$1],
		    GetStringLength (val [$1]));
		} ;
%%
