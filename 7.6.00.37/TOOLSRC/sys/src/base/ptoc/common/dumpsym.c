/* @(#)common/dumpsym.c

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

#include <stdio.h>
#include <string.h>
#include "../common/stab.h"
#include "../common/symtab.ci"

static char *
typestring (i)
int i;
{
	static char *typetable[] = {
		"Subrange        ",
		"Short           ",
		"Unsigned Short  ",
		"Long            ",
		"Unsigned Long   ",
		"Integer         ",
		"Unsigned        ",
		"Boolean         ",
		"Char            ",
		"Unsigned Char   ",
		"Enumeration     ",
		"Float           ",
		"Double          ",
		"Array           ",
		"Record          ",
		"File            ",
		"Set             ",
		"Pointer         ",
		"Function        ",
		"Union           ",
		"String		 ",
		"Void            ",
		"Block           ",
		"Procedure       ",
		"Character       "
	};
	static char str[16];
	if(i < 0) return(typetable[-i]);
	else {
		sprintf(str,"%-16d",i);
		return(str);
	}
}

DumpSymbol(i, debug_flag)
SymbolTableRange i;
int debug_flag;
{
	static char *Kind[] = {
		"Dummy          ",
		"Unused         ",
		"Module         ",
		"Procedure      ",
		"Function       ",
		"Forward Proc   ",
		"Forward Func   ",
		"Type           ",
		"Constant       ",
		"Variable       ",
		"Label          ",
		"Record Element ",
		"Record Variant ",
		"Packed Type    ",
		"Parameter      ",
		"System Proc    ",
		"Type Name      ",
		"CBlockDcl      ",
		"Block Element  ",
		"FuncParmDcl    ",
		"ProcParmDcl    ",
		"BlockDcl       ",
		"NameListDcl	"			/* DK 17.10.88 */
	};
	static char *Class[] = {
		"SNULL",
		"Auto",
		"Extern",
		"F Static",
		"L Static",
		"Register",
		"Ext Def",
		"Param",
		"Typedef",
		"FuncDef",
		"AutoNR",
		"Inline ASM"
	};
	static char *SubClass[] = {
		"Variable",
		"Value",
		"Reference",
		"Result",
		"Initialized"
	};
	char str[256];
#ifdef RAS /*RASE err.4 fix string overflow */
	char index [ 20 ];
#else
	char index[4];
#endif

	GetString(STab[i].SName, str);
	printf("%d.(0x%p)  %s\tnext = %d\n", i, &STab[i], str,STab[i].SNext);
	printf("\tlink = %d\t", STab[i].SLink);
	printf("\tcard = %d", STab[i].SCard);
	printf("\tfile = %d", STab[i].SFile);
	printf("\n\ttype = %s", typestring(STab[i].SType));
	printf("\tlevel = %d", STab[i].SLevel);
	printf("\tflags = %x", STab[i].SFlags);
	printf("\n\tkind = %s", Kind[STab[i].SKind]);
	printf("\n\tSTname = %d\tSFeld1 = %d\tSFeld2 = %d\tSOrder = %d\n" ,
		STab[i].STname,STab[i].SFeld1,STab[i].SFeld2,STab[i].SOrder);
	printf("\tSTmyname = %d\tSFeld3 = %d\tSFeld4 = %d\tSRecVar = %d\n" ,
		STab[i].STmyname,STab[i].SFeld3,STab[i].SFeld4,STab[i].SRecVar);
	printf("\tSFeld5 = %d\n", STab[i].SFeld5);
	switch(STab[i].SKind) {
		case DummyDcl:
			printf("\n");
			break;
		case TypeDcl:
		case PackedTypeDcl:
		case RecordVariantDcl:
			switch(STab[i].SType) {
				case ShortType:
				case UshortType:
				case IntegerType:
				case UnsignedType:
				case BooleanType:
				case CharType:
				case UcharType:
				case EnumerativeType:
				case CharacterType:			/* DK 16.07.88 */
					printf(
					"\tlowrange = %d\thighrange = %d\tfirstenum = %d\n", 	/* DK 16.07.88 */
					STab[i].SLowRange, STab[i].SHighRange, STab[i].SFirstEnum);/* DK 16.07.88 */
					break;
				case FloatType:
				case LongType:
				case UlongType:
				case DoubleType:
					printf("\n");
					break;
				case ArrayType:
					strcpy(index, typestring(STab[i].SIndex));
					printf(
					"\tindex = %-8.8selement = %s\n",
					index, typestring(STab[i].SElement));
					break;
				case BlockType:
				       printf("\tlength = %d\telemalign = %d\n",
					  STab[i].SBlockLen,STab[i].SElemAlign);
					break;
			        case StringType:
				       printf("\tlength = %d\telemalign = %d\n",
					  STab[i].SLength, STab[i].SElem);
					break;
				case RecordType:
				case UnionType:
					printf(
					"\tfirst field = %d\tfirst variant = %d"
					, STab[i].SFirstField,
					STab[i].SFirstVariant);
					printf("\tnext variant = %d\n",
					STab[i].SNextVariant);
					break;
				case FileType:
					printf("\tcomponent = %s\n",
					typestring(STab[i].SComponent));
					break;
				case SetType:
					printf("\tbase = %s\n",
					typestring(STab[i].SBase));
					break;
				case PointerType:
					printf("\tdomain = %s\n",
					typestring(STab[i].SDomain));
					break;
				case FunctionType:
				case ProcedureType:
					printf(
				"\tresult = %sparms = %d\tblock = %d\n",
					typestring(STab[i].SResult),
					STab[i].SParmCnt,
					STab[i].SBlock);
					break;
				case VoidType:
					printf("\n");
					break;
				default:
					if(STab[i].SType < 0)
						printf(
						"Bad Type: %d !\n",
						STab[i].SType);
					else printf(
					"\tlowrange = %d\thighrange = %d\n",
					STab[i].SLowRange, STab[i].SHighRange);
					break;
			}

			if(debug_flag) {
			   printf("\tMachine dependent fields:\n");
			   switch(STab[i].SType) {
			   case FunctionType:
			   case ProcedureType:
			      printf("NONE - all eliminated\n" );
			      break;
			   default:
			      printf("\tSAlign = %d\tSSize = %d\tSOffset = %d\n",
				STab[i].TypeInfo.SAlign,STab[i].TypeInfo.SSize,
				STab[i].TypeInfo.SOffset);
			   }
			}

			break;

		case ModuleDcl:
		case ProcDcl:
		case FuncDcl:
		case ForwardProcDcl:
		case ForwardFuncDcl:
		case CBlockDcl:
		case LabelDcl:
		case CBlockElementDcl:
		case VarDcl:
		case ParmDcl:
	        case FuncParmDcl:
	        case ProcParmDcl:
		case NameListDcl:						/* DK 17.10.88 */
			printf("\tclass = %s\tsubclass = %s\n",
			Class[STab[i].SClass], SubClass[STab[i].SSubClass]);
			if(debug_flag) {
			printf("\tMachine dependent fields:\n\t");
			printf("SRegister = %d\n", STab[i].AddrInfo.SRegister );
			}
			break;
		case RecordElementDcl:
			printf("\tnext field = %d\n",
			STab[i].SNextField);
			if(debug_flag) {
			printf("\tMachine dependent fields:\n\t");
			printf("SAlign = %d\tSSize = %d\tSOffset = %d\n",
			STab[i].ElementInfo.SAlign,STab[i].ElementInfo.SSize,
			STab[i].ElementInfo.SOffset);
			}
			break;
		case ConDcl:
			switch (STab [i].SType) {
				case SubRangeType:
				case ShortType:
				case IntegerType:
				case BooleanType:
				case CharType:
				case UcharType:
				case CharacterType:
				case EnumerativeType:
					printf("\tvalue = %d", /* DK 16.07.88 */
					STab[i].SCon1val);
					break;
				case LongType:
					printf("\tvalue = %ld",
					*(long *) &STab[i].SCon1val);
					break;
				case UlongType:
					printf("\tvalue = %lu",
					*(unsigned long *) &STab[i].SCon1val);
					break;
				case FloatType:
					printf("\tvalue = %f", /* DK 16.07.88 */
					*(double *) &STab[i].SCon1val);
					break;
				case DoubleType:
					printf("\tvalue = %e",			/* DK 16.07.88 */
					*(double *) &STab[i].SCon1val);
					break;
				default:
					printf("invalid type\n");
					break;
			}
			printf("\tnextenum = %d\n",STab[i].SNextEnum);		/* DK 16.07.88 */
			break;
		case SysProcDcl:
			printf("\tsystem specific = %d\n",
			STab[i].SSysSpecific);
			break;
		case TypeNameDcl:
			printf("\n");
			break;
		case BlockDcl:
			printf("\tLast Entry = %d\tFuncBlock = %d\n",
			STab[i].SLastEntry, STab[i].SFuncBlock);
			break;
		default:
			printf("Unknown Kind: %d !\n", STab[i].SKind);
			break;
	}
}

DumpSymbolTable(debug_level)
int debug_level;
{
	int i;
	printf("----------------------------------------------------------------------\n");
	printf("\n\nSymbol Table\n");
	for(i=1 /* 120 */ ; i<=SymPtr; i = STab[i].SNext) {
		DumpSymbol(i, debug_level > 1 ? 1 : 0);
	}
	fflush(stdout);
}

DumpSymbols(first,last)
int first,last;
{
	int i;
	printf("\n\nSymbol Table\n");
	for(i=first; i<=last; i = STab[i].SNext) {
		printf("\n----------------------------------------------------------------------------\n");
		if (!i) break;
		DumpSymbol(i, 1);
	}
	fflush(stdout);
}
