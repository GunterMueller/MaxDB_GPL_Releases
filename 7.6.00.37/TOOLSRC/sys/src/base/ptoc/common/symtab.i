(* @(#)common/symtab.i

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

*)

(* symtab.i *)

const
   SymSize = 32768;
   SymLast = SymSize - 1;

type
   SymbolTableRange = integer;


(* primitive types *)

type
   PRIMTYPE = integer;

const
   SubRangeType	= 0;
   ShortType	= -1;
   UshortType   = -2;
   LongType	= -3;
   UlongType    = -4;
   IntegerType	= -5;
   UnsignedType	= -6;
   BooleanType	= -7;
   CharType	= -8;
   UcharType    = -9;
   EnumerativeType	= -10;
   FloatType	= -11;
   DoubleType	= -12;
   ArrayType	= -13;
   RecordType	= -14;
   FileType	= -15;
   SetType	= -16;
   PointerType	= -17;
   FunctionType	= -18;
   UnionType	= -19;
   StringType	= -20;
   VoidType	= -21;
   BlockType    = -22;
   ProcedureType= -23;
   CharacterType= -24;
   LastType     = -24;

(* primitive sizes *)

const
   ShortSize	= 16;
   UshortSize   = 16;
   LongSize	= 64;
   UlongSize    = 64;
   IntegerSize	= 32;
   UnsignedSize	= 32;
   CharSize	= 8;
   UcharSize    = 8;
   FloatSize	= 32;
   DoubleSize	= 64;
   PointerSize	= 32;
   CharacterSize= 8;


(* primitive alignments *)

const
   CharAlignment	= 8;
   UcharAlignment       = 8;
   CharacterAlignment	= 8;
   ShortAlignment	= 16;
   UshortAlignment      = 16;

   LongAlignment	= LongSize;
   UlongAlignment       = UlongSize;
   IntegerAlignment	= 32;
   UnsignedAlignment	= 32;
   FloatAlignment	= 32;
   DoubleAlignment	= DoubleSize;
   RecordAlignment	= 32;
   FileAlignment	= 32;
   PointerAlignment	= 32;

(* entry kinds *)

type
   KINDTYPE = integer;

const
   DummyDcl	= 0;
   ModuleDcl	= 2;
   ProcDcl	= 3;
   FuncDcl	= 4;
   ForwardProcDcl	= 5;
   ForwardFuncDcl	= 6;
   TypeDcl	= 7;
   ConDcl	= 8;
   VarDcl	= 9;
   LabelDcl	= 10;
   RecordElementDcl	= 11;
   RecordVariantDcl	= 12;
   PackedTypeDcl	= 13;
   ParmDcl	= 14;
   SysProcDcl	= 15;
   TypeNameDcl	= 16;
   CBlockDcl	= 17;
   CBlockElementDcl	= 18;
   FuncParmDcl	= 19;
   ProcParmDcl	= 20;
   BlockDcl     = 21;
   NameListDcl  = 22;
   TagNameDcl   = 23;


(* storage class *)

type
   CLASSTYPE = integer;

const
   SNULL	= 0;
   AUTO		= 1;
   EXTERN	= 2;
   FILESTATIC	= 3;
   LOCALSTATIC	= 4;
   REGISTER	= 5;
   EXTDEF	= 6;
   PARAM	= 7;
   AUTONR	= 10;


(* parameter subclasses *)

type
   SUBCLASSTYPE = integer;

const
   ByValueParm  = 1;
   ByRefParm    = 2;
   InitializedVar	= 4;
   UninitializedVar	= 0;


(* symbol table entry *)

type

   MACHDEP = record
      case KINDTYPE of
         DummyDcl,
         ConDcl,
         SysProcDcl,
	 TypeNameDcl : (
            dummy1: integer;
            dummy2: integer;
            dummy3: integer
         );
         TypeDcl,
         RecordElementDcl,
         RecordVariantDcl: (
            SAlign: integer;
            SSize: integer;
            SOffset: integer
         );
         BlockDcl  (* kludge for ProcedureType and FunctionType *): (
            STempIndex: integer;
         );
	 CBlockDcl,
	 CBlockElementDcl,
	 ModuleDcl,
	 FuncDcl,
	 ProcDcl,
	 ForwardFuncDcl,
	 ForwardProcDcl,
	 NameListDcl,
         VarDcl,
         ParmDcl,
         LabelDcl: (
            SRegister: integer;
         );
   end;

   SYMTAB = record
      SNext: integer;
      SName: integer;
      SLink: integer;
      SFile: integer;
      SType: integer;
      SFlags: integer;
      SCard: integer;
      SLevel: integer;
      STname: integer;
      SOrder: integer;
      SFeld1: integer;
      SFeld2: integer;
      SRecVar: integer;
      SFeld3: integer;
      SFeld4: integer;
      SFeld5: integer;
      STmyname: integer;	
      case SKind: KINDTYPE of
         DummyDcl: (
            DummyInfo: MACHDEP
         );
         TypeDcl,
         RecordVariantDcl: (
            TypeInfo: MACHDEP;
            case PRIMTYPE of
               SubRangeType,
               ShortType,
	       UshortType,
               IntegerType,
               UnsignedType,
               BooleanType,
               CharType,
	       UcharType,
               EnumerativeType,
	       CharacterType: (
                  SLowRange: integer;
                  SHighRange: integer;
		  SFirstEnum: SymbolTableRange
               );
               FloatType,
	       UlongType,
               LongType,
               DoubleType: (
               );
               ArrayType: (
                  SIndex: SymbolTableRange;
                  SElement: SymbolTableRange
               );
               RecordType,
               UnionType: (
                  SFirstField: SymbolTableRange;
                  SFirstVariant: SymbolTableRange;
                  SNextVariant: SymbolTableRange
               );
               FileType: (
                  SComponent: SymbolTableRange
               );
               SetType: (
                  SBase: SymbolTableRange
               );
               PointerType: (
                  SDomain: SymbolTableRange
               );
	       ProcedureType,
               FunctionType: (
                  SResult: SymbolTableRange;
            	  SParmCnt: SymbolTableRange;
            	  SBlock: SymbolTableRange;
               );
	       BlockType : (
                  SBlockLen  : integer;
		  SElemAlign : integer
               );
               StringType: (
                  SLength : integer;
		  SElem   : SymbolTableRange;
               );
         );
         ModuleDcl,
         ProcDcl,
         FuncDcl,
         ForwardProcDcl,
         ForwardFuncDcl,
	 CBlockDcl,
	 CBlockElementDcl,
	 NameListDcl,
         VarDcl,
         ParmDcl,
         LabelDcl: (
            AddrInfo: MACHDEP;
            SClass: CLASSTYPE;
            SSubClass: SUBCLASSTYPE;
            SParent: SymbolTableRange
         );
	 BlockDcl: (
            AllocInfo: MACHDEP;  (* kludge for ProcedureType and FunctionType *)
            SFuncBlock: SymbolTableRange;
            SLastEntry: SymbolTableRange
	 );
         RecordElementDcl: (
            ElementInfo: MACHDEP;
            SNextField: SymbolTableRange;
         );
         ConDcl: (
            SCon1val: integer;
            SCon2val: integer;
	    SNextEnum: SymbolTableRange
         );
         SysProcDcl: (
            SysProcInfo: MACHDEP;
            SSysSpecific: integer
         );
         TypeNameDcl: (
            TypeNameInfo: MACHDEP
         );
   end;

var
   SymPtr: SymbolTableRange;
   STab: array [0..SymLast] of SYMTAB;
