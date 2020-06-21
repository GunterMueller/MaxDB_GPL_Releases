/* @(#)pascl/initialize.c

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

#ifdef WIN32
#pragma warning (disable : 4033) /*RASE err. 1a: warnings.level 2*/
#endif

#include <stdio.h>
#include "../common/globals.ci"
#include "../common/stab.h"
#include "../pascl/sflags.def"
#include "../pascl/symtab.ch"
#include "../common/opcode.i"
#include "../common/csg.def"
#include "../common/csg.i"
#include "../common/hdg.def"
#include "../gram/data.h"
#include "../pascl/globals1.ci"

#define DATACNT 250
#define DATASIZE DATACNT * sizeof(struct datafield)


#define type_size(x) STab[x].TypeInfo.SSize

static int name;
static int last_offset;

CopyVar(symbol)
int symbol;
{
	BumpSymPtr();

	STab[SymPtr].SName = STab[symbol].SName;
	STab[SymPtr].SLink = STab[symbol].SLink;
	STab[SymPtr].SKind = STab[symbol].SKind;
	STab[SymPtr].SType = STab[symbol].SType;
	STab[SymPtr].SLevel = STab[symbol].SLevel;
	STab[SymPtr].SCard = STab[symbol].SCard;
	STab[SymPtr].SFile = STab[symbol].SFile;
	STab[SymPtr].SLowRange = STab[symbol].SLowRange;
	STab[SymPtr].SHighRange = STab[symbol].SHighRange;
	STab[SymPtr].SFlags = STab[symbol].SFlags;
	STab[SymPtr].SParent = STab[symbol].SParent;

	if(STab[SymPtr].SKind == VarDcl || STab[SymPtr].SKind == ParmDcl) {
		STab[SymPtr].SClass = LOCALSTATIC;
		STab[SymPtr].SSubClass = InitializedVar;
	}
		
	TypeSize(SymPtr);
	return(SymPtr);
}

IsScalar(type)
int type;
{
	switch(STab[type].SType) {
		case ArrayType:
		case RecordType:
		case UnionType:
		case SetType:
			return(0);
		default:
			return(1);
	}
}

static int lhs, external, scalar = 0;

process_init_list(id_list, value_list, lineno)
int id_list, value_list, lineno;
{
	int i;

	if(value_list == -1) return;
	for(i=StartCnt[id_list] + 1; i<=StartCnt[id_list] + cnt[id_list]; i++) {
		HdgHead = LinkChain(HdgHead,
		process_initializer(FindSymbol(IdList[i]), value_list, lineno));
	}
}

process_initializer(symbol, NodeIndex, lineno)
int symbol, NodeIndex, lineno;
{
	if(symbol == -1) return (-1);
	StartupInit ();
	if(STab[symbol].SClass != AUTO && STab[symbol].SClass != REGISTER) {
		external = 1;
		/* Let storage allocator know that this item is initialized */
		STab[symbol].SSubClass = InitializedVar;
		if((name = STab[symbol].SLink) &&
		STab[name].SSubClass == InitializedVar &&
		STab[name].SLevel == STab[symbol].SLevel)
			uerror("duplicate initialized definitions for %s",
				GetStringPtr(STab[symbol].SName));
		name = symbol;
		last_offset = 0;
		scalar = IsScalar(symbol = STab[BaseType(symbol)].SType);
		TypeSize(symbol);
		traverse(0, NodeIndex, symbol, lineno);
		return( -1 );
	} else {
		external = 0;
		name = symbol;
		last_offset = 0;
		lhs = node3(IdOp, STab[name].SType, name, 0);
		scalar = IsScalar(symbol = STab[BaseType(symbol)].SType);
		if(!scalar) {
			name = CopyVar(name);
		}
		TypeSize(STab[name].SType);
		traverse(0, NodeIndex, STab[name].SType, lineno);
		if(!scalar) {
			/* Generate an assignment of the dummy name to name */
			lhs = GenStore(lhs, PrimType(symbol), symbol, -1,
				UnaryExpression(LoadOp, symbol,
				node3(IdOp,STab[name].SType,name,0), 0, 0),
				PrimType(symbol), symbol, -1, lineno);
		}
		return( lhs );
	}
}

IsListNode(NodeIndex)
int NodeIndex;
{
	if(NodeIndex == -1) return( 0 );
	if(MajorOperator == ExpList) return(MinorOperator == ListOp ? 1 : 0);
	return( 0 );
}

GetListHead(NodeIndex)
int NodeIndex;
{
	return( MajorOperator == ExpList && MinorOperator == ListOp ?
		ListHeadField : NodeIndex);
}

GetListSize(NodeIndex)
int NodeIndex;
{
	return( MajorOperator == ExpList && MinorOperator == ListOp ?
		ListSizeField : 1);
}

GetMinor(NodeIndex)
int NodeIndex;
{
	if (NodeIndex == -1) return (-1);
	return( MinorOperator );
}

GetNextLink(NodeIndex)
int NodeIndex;
{
	return( NextLink );
}

GetUnaryOperand(NodeIndex)
int NodeIndex;
{
	return( Unary1operand );
}

int GetCount(NodeIndex)
int NodeIndex;
{
	return(Node1parameter);
}

Is_String(NodeIndex)
int NodeIndex;
{
	/*
	 * if(GetListSize(NodeIndex) != 1) return(0);
	 */
	NodeIndex = GetListHead(NodeIndex);
	if(MinorOperator == LoadstrOp) return(1);
	else return( 0 );
}

/*
 * FindElementType(type)
 * SymbolTableRange        type;
 * {
 *         while (STab [type].SType == ArrayType) {
 *                 type = STab [type].SElement;
 *         }
 *         return type;
 * }
 */

#define ALIGN(x, y) x += x % y ? y - x % y : 0

int GetDimensions(type)
int type;
{
	int fields = 0;

	if(STab[type = BaseType(type)].SType == ArrayType) {
		return( (STab[STab[type].SIndex].SHighRange -
			STab[STab[type].SIndex].SLowRange + 1) *
			GetDimensions(STab[type].SElement) );
	} else if(STab[type = BaseType(type)].SType == RecordType) {
		if((type = BaseType(STab[type].SFirstField)) != -1) {
			fields += GetDimensions(type);
		}
		while((type = BaseType(STab[type].SNextField)) != -1) {
			fields += GetDimensions(type);
		}
		return( fields );
	} else if(STab[type = BaseType(type)].SType == UnionType) {
		return( 1 );
	} else return(1);
}

GetBaseType(type)
int type;
{
	if(STab[type = BaseType(type)].SType != ArrayType) return(type);
	else return(GetBaseType(STab[type].SElement));
}

int GetInitSize(NodeIndex, type)
int NodeIndex, type;
{
	int temp, sum = 0;

	if(MajorOperator == ExpList && MinorOperator == ListOp) {
		temp = GetListSize(NodeIndex);
		NodeIndex = GetListHead(NodeIndex);
		for(; temp-- > 0;) {
			sum += GetInitSize(NodeIndex, BaseType(TypeField));
			NodeIndex = NextLink;
		}
		return( sum );
	} else if(MinorOperator == LoadstrOp) {
		if(type == CharPtr) {
			return(GetStringLength(Node1parameter) + 1);
		} else return(1);
	}
	return( 1 );
}

traverse(offset, NodeIndex, symbol, lineno)
int offset, NodeIndex, symbol, lineno;
{
	int i, list, dim_size, elem_type, base_type, len, max_size;
	int init_size, truncate = 0;
	int field_offset;
	static char *p;

	symbol = BaseType(symbol);
	list = NodeIndex;
	switch(STab[PrimType(symbol)].SType) {
		case ArrayType:
			dim_size = STab[STab[symbol].SIndex].SHighRange -
				STab[STab[symbol].SIndex].SLowRange + 1;
			elem_type = BaseType(STab[symbol].SElement);
			base_type = GetBaseType(elem_type);
			if(IsListNode(NodeIndex)) {
				init_size = GetListSize(NodeIndex);
				NodeIndex = GetListHead(NodeIndex);
			} else if(MinorOperator==LoadstrOp ||
				(MinorOperator==LitOp && TypeField==PacPtr)) {
				p = GetStringPtr(Node1parameter);
				len = GetStringLength(Node1parameter);
				if(elem_type == CharPtr) {
					if(dim_size >= len)
						init_size = len;
					else {
						/* here dim_size is not zero */
						init_size = dim_size;
						truncate = 1;
					}
				}
			} else {
				uerror("unbracketed array initializer list");
			}
			for(i = 0; i < dim_size; i++) {
				int initializer;

				if ((GetMinor(list) == LoadstrOp &&
				    elem_type == CharPtr) ||
				    (MinorOperatorOf(list)==LitOp &&
				    TypeFieldOf(list)==PacPtr)) {
					if(--init_size >= 0)
					   initializer =
						node3(LitOp,elem_type,*p++,0);
					else if(MinorOperatorOf(list)==LitOp &&
					   TypeFieldOf(list)==PacPtr) {
					   initializer =
						node3(LitOp,elem_type,' ',0);
					} else {
					   initializer =
						node3(LitOp,elem_type,0,0);
					}
				} else {
					initializer = NodeIndex;
				}
				NodeIndex = traverse(offset, initializer,
					elem_type, lineno);
			}
			TypeSize(symbol);
			if(truncate) {
				uerror("too many initializers");
			}
			return(GetNextLink(list));
			break;
		case RecordType:
			elem_type = STab[symbol].SFirstField;
			dim_size = 1;
			TypeSize(elem_type);
			field_offset = STab[elem_type].ElementInfo.SOffset;
			if(IsListNode(NodeIndex)) {
				init_size = GetListSize(NodeIndex);
				ALIGN(last_offset,
				STab[BaseType(STab[elem_type].SType)].
				ElementInfo.SAlign);
				NodeIndex = traverse(offset + field_offset,
					GetListHead(NodeIndex),
					BaseType(STab[elem_type].SType),
					lineno);
			} else {
				uerror("unbracketed record initializer list");
			}
			while((elem_type = STab[elem_type].SNextField) != -1) {
				TypeSize(elem_type);
				field_offset = STab[elem_type].ElementInfo.SOffset;
				dim_size++;
				ALIGN(last_offset,
				STab[BaseType(STab[elem_type].SType)].
				ElementInfo.SAlign);
				NodeIndex = traverse(offset + field_offset,
					NodeIndex,
					BaseType(STab[elem_type].SType),
					lineno);
			}
			TypeSize(symbol);
			ALIGN(last_offset, STab[symbol].TypeInfo.SAlign);
			if(dim_size < init_size) {
				uerror("too many initializers");
			}
			return(GetNextLink(list));
			break;
		case UnionType:
			if(NodeIndex != -1)
				uerror("variant records may not be initialized");
			return( -1 ); /* no initializers may follow a union */
			break;
		case SetType:
			if(IsListNode(NodeIndex)) {
			    int l, h, i, size, sbase, low, high, lower;

			    list = GetListHead(NodeIndex);
			    size = STab[symbol].TypeInfo.SSize;
			    sbase = STab[symbol].SBase;
			    lower = STab[sbase].SLowRange;
			    for(l = 0, h = 7; l < size; l += 8, h += 8) {
			    	char pattern = 0;

				NodeIndex = list;
				while(NodeIndex != -1) {
				    if(MajorOperatorOf(NodeIndex) == ExpList) {
					int pair;

			    		pair = GetListHead(NodeIndex);
					if(MinorOperatorOf(pair)==LitOp) {
					    low = Node1parameterOf(pair)-lower;
					} else if (IsConstantExpr(pair)) {
					    low = ConstExp(pair)-lower;
					} else {
					    uerror("Constant expected in set");
					}
					pair = NextLinkOf(pair);
					if(MinorOperatorOf(pair)==LitOp) {
					    high = Node1parameterOf(pair)-lower;
					} else if (IsConstantExpr(pair)) {
					    high = ConstExp(pair)-lower;
					} else {
					    uerror("Constant expected in set");
					}
					NodeIndex = NextLink;
					if(low > h || high < l) continue;
					if(low < l) low = l;
					if(high > h) high = h;
					for(i = low - l; i <= high - l; i++)
					    pattern |= (1 << i);
				    } else {
					if(MinorOperatorOf(NodeIndex)==LitOp) {
					    low = Node1parameter - lower;
					} else if (IsConstantExpr(NodeIndex)) {
					    low = ConstExp(NodeIndex) - lower;
					} else {
					    uerror("Constant expected in set");
					}
					NodeIndex = NextLink;
					if(low > h || low < l) continue;
					pattern |= (1 << (low - l));
				    }
				}
				OutputInit(name,
				    node3(LitOp, CharPtr, pattern, 0));
			    }
			} else {
				uerror("set constructor expected");
			}
			break;
		case PointerType:
			if(NodeIndex != -1 &&
			!(MinorOperator == LitOp &&
			STab[TypeField].SType == PointerType &&
			Node1parameter == 0 &&
			Node2parameter == 0)) {
			    uerror("pointers may only be initialized to nil");
			    return(NextLink);
			}
		case LongType:
		case IntegerType:
		case ShortType:
		case CharType:
		case UshortType:
		case BooleanType:
		case CharacterType:
		case StringType:
		case FloatType:
		case DoubleType:
		case EnumerativeType:
			if(NodeIndex != -1 && MinorOperator == LoadstrOp &&
			STab[symbol].SType != PointerType) {
				uerror("primitive initializer expected");
			} else {
				if(external || !scalar) {
				if(!scalar) {
				/* braces and not a scalar */
				if(IsListNode(NodeIndex)) {
					if(!scalar) uerror(
					    "primitive initializer expected"
					);

					NodeIndex = GetListHead(NodeIndex);
				}
				}
				TypeSize(symbol);
				ALIGN(offset,
				STab[symbol].ElementInfo.SAlign);
				/***************************************
				if(NodeIndex == -1)
					NodeIndex =
					node3(LitOp, symbol, 0, 0);
				***************************************/
				if(offset > last_offset)
					   last_offset += offset - last_offset;
				ALIGN(last_offset,
				STab[symbol].ElementInfo.SAlign);
				if (NodeIndex == -1) {
					 last_offset += type_size(symbol);
					 return(-1);
				} else {
					 OutputInit(name, NodeIndex);
					 last_offset += type_size(symbol);
				}
				} else {   /* scalar and !external */
					/* ignore optional braces */
					if(IsListNode(NodeIndex)) {
					    if(GetListSize(NodeIndex) > 1)
						uerror(
						"primitive initializer expected"
						);
					    NodeIndex = GetListHead(NodeIndex);
					}
					if(NodeIndex == -1)
						NodeIndex =
						node3(LitOp, symbol, 0, 0);
					lhs = GenStore(lhs,
						PrimType(TypeFieldOf(lhs)),
						TypeFieldOf(lhs),
						-1, NodeIndex,
						PrimType(TypeField),
						TypeField, -1, lineno);
				}
			}
			return( NextLink );
			break;
		default:
			uerror("type may not be initialized");
			return( NextLink );
			break;
	}
}


StartupInit ()
{
	extern char	*malloc ();
	static int	first_time = 1;

	if (first_time) {
		datafile = (struct datafield *) malloc (DATASIZE);
		if (datafile == (struct datafield *) NULL) {
			cerror ("out of memory - initialize");
		}
		first_time = 0;
	}
}


OutputInit (symbol, NodeIndex)
int symbol, NodeIndex;
{
	static int n = 1;
	extern char *realloc ();

	if (datacnt >= DATACNT*n - 1 ) {
		datafile = (struct datafield *) realloc (datafile,++n*DATASIZE);
		if (datafile == (struct datafield *) NULL) {
			cerror ("out of memory - initialize");
		}
	}
	(datafile + datacnt)->symfield = symbol;
	(datafile + datacnt)->valuefield = NodeIndex;
	(datafile + ++datacnt)->symfield = -1;
}

version_writing()
{
extern char *PGENC_id[] ;
	printf ( "Version is: %s \n" , PGENC_id [0] );
}
