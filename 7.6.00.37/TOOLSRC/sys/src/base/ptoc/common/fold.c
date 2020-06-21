/* @(#)common/fold.c

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

/* fold.c */

#include "../common/globals.ci"
#include "../common/csg.def"
#include "../common/hdg.def"
#include "../common/csg.i"
#include "../common/opcode.i"

#define CONSTANT 1

int
NoSideEffects(NodeIndex)
	HdgRange	NodeIndex;
{
    if (NodeIndex != -1) {
	switch(MajorOperator) {
		case node:
			return(1);
		case UnaryExp:
			return(NoSideEffects(Unary1operand));
		case BinaryExp:
			return(NoSideEffects(Bin1operand) &&
				NoSideEffects(Bin2operand));
		case ExpList:
			{
				int item, temp;

				temp = 1;
				while((item = ExtractList(NodeIndex)) != -1)
					temp = NoSideEffects(item) ? temp : 0;
				return(temp);
			}
		case ifthenelse:
			return(NoSideEffects(IfTrueField) &&
			NoSideEffects(IfFalseField));
		case spec:
		case ShortStmnt:
		case LongStmnt:
		case loop:
		case Switch:
		case Case:
		case RepeatUntil:
			return(0);
		default:
			cerror("Unknown major op in NoSideEffects.");
			return(0);
	}
    } else {
	return (1);
    }
}

int
IsConstant (NodeIndex)
	HdgRange	NodeIndex;
{
	return (MajorOperator == node) && (MinorOperator == LitOp);
}

int
IsZero(NodeIndex)
	HdgRange 	NodeIndex;
{
	if (STab[TypeField].SType == FloatType	||
	    STab[TypeField].SType == UlongType  ||
	    STab[TypeField].SType == LongType   ||
	    STab[TypeField].SType == DoubleType	)
		return	(MajorOperator == node) &&
			(MinorOperator == LitOp) &&
			(Node1parameter == 0) &&
			(Node2parameter == 0);
	else
		return	(MajorOperator == node) &&
			(MinorOperator == LitOp) &&
			(Node1parameter == 0);
}

int
IsOne(NodeIndex)
	HdgRange 	NodeIndex;
{
    if (MajorOperator == node && MinorOperator == LitOp)
    {
	switch (STab[TypeField].SType)
	{
	case IntegerType:
	    return (Node1parameter == 1);
	    break;
	case LongType:
	    return (Node1parameter == 1 && Node2parameter == 0);
	    break;
	case FloatType:
	    return ((Node1parameter == 0x3f800000 ||
		Node1parameter == 0x3ff00000) && Node2parameter == 0x0);
	    break;
	case DoubleType:
	    return (Node1parameter == 0x3ff00000 && Node2parameter == 0x0);
	    break;
	}
    }
    return (0);
}

static HdgRange
fold_uminus (typ, operand)
	SymbolTableRange	typ;
	HdgRange		operand;
{
	HdgRange	NodeIndex;
	int		*i;
	long		*l;
	double          *d;

	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
		case PointerType:
			NodeIndex = operand;
			i = (int *) &Node1parameter;
			*i = -*i;
			break;

		case UnsignedType:
		case UcharType:
		case CharacterType:
		case UshortType:
			NodeIndex = operand;
			/* Leave the type unsigned, the result will be
			the same since we emit the same instructions for
			signed and unsigned numbers */
			i = (int *) &Node1parameter;
			*i = -*i;
			break;

		case LongType:
		case UlongType:
			NodeIndex = operand;
			l = (long *) &Node1parameter;
			*l = -*l;
			break;

		case FloatType:
			NodeIndex = operand;
			d = (double *) &Node1parameter;
			*d = (double) (-((float) *d));
			break;

		case DoubleType:
			NodeIndex = operand;
			d = (double *) &Node1parameter;
			*d = -*d;
			break;

		default:
			cerror ("invalid typ - fold_uminus");
			break;
	}
	TypeField = typ;
	return NodeIndex;
}


static HdgRange
fold_not (typ, operand)
	SymbolTableRange	typ;
	HdgRange		operand;
{
	HdgRange	NodeIndex;
	int		*i;
	unsigned	*u;
	long		*l;
	unsigned long	*ul;
	double          *d;

	NodeIndex = operand;
	switch (STab [TypeField].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			i = (int *) &Node1parameter;
			*i = !*i;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			u = (unsigned *) &Node1parameter;
			*u = !*u;
			break;

		case LongType:
			l = (long *) &Node1parameter;
			*l = !*l;
			break;

		case UlongType:
			ul = (unsigned long *) &Node1parameter;
			*ul = !*ul;
			break;

		case FloatType:
			d = (double *) &Node1parameter;
			*d = (double) (!((float) *d));
			break;

		case DoubleType:
			d = (double *) &Node1parameter;
			*d = !*d;
			break;

		default:
			cerror ("invalid typ - fold_not");
			break;
	}
	TypeField = typ;
	return NodeIndex;
}

static HdgRange
fold_convert (
        SymbolTableRange        typ,
        HdgRange                NodeIndex);

static HdgRange
fold_shl (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;

	if (IsZero(operand2)) return(fold_convert(typ, operand1));
	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 << *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 << *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 << *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 << *ul2;
			break;

		default:
			cerror ("invalid typ - fold_shl");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		int const_type;

		NodeIndex = operand1;
		if(IsConstant(operand2)) {
			if(Bin2parameter == CONSTANT) {
				int const1, const2;

				NodeIndex = Bin2operand;
				const1 = Node1parameter;
				NodeIndex = operand2;
				const_type = TypeField;
				const2 = Node1parameter;
				NodeIndex = operand1;
				if(MinorOperator == ShlOp) {
					if(const1 + const2 < 32) {
					operand2 = FoldBinary(AddOp, const_type,
						Bin2operand, operand2);
					if(IsZero(operand2))
					return(fold_convert(typ, Bin1operand));
					return BinaryExpression(ShlOp, typ,
						Bin1operand, operand2);
					}
				} /* else if(MinorOperator == ShrOp) {
					if(const2 >= const1 &&
					const1 + const2 < 32) {
						operand2 =
						FoldBinary(SubtOp, const_type,
							operand2, Bin2operand);
						if(IsZero(operand2))
						return(fold_convert(typ,
							Bin1operand));
						return
						BinaryExpression(ShlOp, typ,
							Bin1operand, operand2);
					}
				}
				*/
			}
		}
		if(MinorOperator == ShlOp && IsConstant(Bin2operand)) {
			int temp;

			temp = Bin2operand;
			Bin2operand = operand2;
			operand2 = temp;
		}
	}
	if(IsZero(operand2)) return(fold_convert(typ, operand1));
	return BinaryExpression (ShlOp, typ, operand1, operand2);
}


static HdgRange
fold_shr (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;

	if (IsZero(operand2)) return(fold_convert(typ, operand1));
	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 >> *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 >> *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 >> *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 >> *ul2;
			break;

		default:
			cerror ("invalid typ - fold_shr");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		int const_type;

		NodeIndex = operand1;
		if(IsConstant(operand2)) {
			if(Bin2parameter == CONSTANT) {
				int const1, const2;

				NodeIndex = Bin2operand;
				const1 = Node1parameter;
				NodeIndex = operand2;
				const_type = TypeField;
				const2 = Node1parameter;
				NodeIndex = operand1;
				if(MinorOperator == ShrOp) {
					if(const1 + const2 < 32) {
					operand2 = FoldBinary(AddOp, const_type,
						Bin2operand, operand2);
					if(IsZero(operand2))
					return(fold_convert(typ, Bin1operand));
					return BinaryExpression(ShrOp, typ,
						Bin1operand, operand2);
					}
				} /* else if(MinorOperator == ShlOp &&
				(STab[TypeField].SType == UnsignedType ||
				STab[TypeField].SType == UlongType ||
				STab[TypeField].SType == UcharType ||
				STab[TypeField].SType == UshortType )) {
					if(const2 >= const1 &&
					const1 + const2 < 32) {
						operand2 =
						FoldBinary(SubtOp, const_type,
							operand2, Bin2operand);
						if(IsZero(operand2))
						return(fold_convert(typ,
							Bin1operand));
						return
						BinaryExpression(ShrOp, typ,
							Bin1operand, operand2);
					}
				}
				*/
			}
		}
		if(MinorOperator == ShrOp && IsConstant(Bin2operand)) {
			int temp;

			temp = Bin2operand;
			Bin2operand = operand2;
			operand2 = temp;
		}
	}
	if(IsZero(operand2)) return(fold_convert(typ, operand1));
	return BinaryExpression (ShrOp, typ, operand1, operand2);
}


static HdgRange
fold_bNot (typ, operand)
	SymbolTableRange	typ;
	HdgRange		operand;
{
	HdgRange	NodeIndex;
	int		*i;
	unsigned	*u;
	long		*l;
	unsigned long	*ul;
	double          *d;

	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand;
			i = (int *) &Node1parameter;
			*i = ~*i;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand;
			u = (unsigned *) &Node1parameter;
			*u = ~*u;
			break;

		case LongType:
			NodeIndex = operand;
			l = (long *) &Node1parameter;
			*l = ~*l;
			break;

		case UlongType:
			NodeIndex = operand;
			ul = (unsigned long *) &Node1parameter;
			*ul = ~*ul;
			break;

		default:
			cerror ("invalid typ - fold_bNot");
			break;
	}
	TypeField = typ;
	return NodeIndex;
}


static HdgRange
fold_convert (typ, NodeIndex)
	SymbolTableRange	typ;
	HdgRange		NodeIndex;
{
    int			*i1;
    unsigned		*u1;
    long		*l1;
    unsigned long	*ul1;
    double		*d1;

    if (typ == TypeField) {
	    return NodeIndex;
    }

    switch (STab [TypeField].SType) {

	case ShortType:
	case IntegerType:
	case BooleanType:
	case CharType:
	case EnumerativeType:
		i1 = (int *) &Node1parameter;
		switch (STab [typ].SType) {
			case CharType:
				*i1 = (char) *i1;
				break;
			case ShortType:
				*i1 = (short) *i1;
				break;
			case IntegerType:
			case BooleanType:
			case EnumerativeType:
				break;
			case UcharType:
			case CharacterType:
				*(unsigned *)i1 = (unsigned char) *i1;
				break;
			case UshortType:
				*(unsigned *)i1 = (unsigned short) *i1;
				break;
			case UnsignedType:
				*(unsigned *)i1 = (unsigned) *i1;
				break;
			case LongType:
				*(long *)i1 = (long) *i1;
				break;
			case UlongType:
				*(unsigned long *)i1 = (unsigned long) *i1;
				break;
			case FloatType:
				*(double *)i1 = (float) *i1;
				break;
			case DoubleType:
				*(double *)i1 = (double) *i1;
				break;
			case PointerType:
				/*
				return UnaryExpression (
					ConvertOp,
					typ,
					NodeIndex,
					0,
					0);
				*/
				TypeField = typ;
				return (NodeIndex);
			default:
				cerror ("invalid typ - fold_convert");
				break;
		}
		break;

	case UshortType:
	case UnsignedType:
	case UcharType:
	case CharacterType:
		u1 = (unsigned *) &Node1parameter;
		switch (STab [typ].SType) {
			case CharType:
				*(int *)u1 = (char) *u1;
				break;
			case ShortType:
				*(int *)u1 = (short) *u1;
				break;
			case IntegerType:
			case BooleanType:
			case EnumerativeType:
				*(int *)u1 = (int) *u1;
				break;
			case UcharType:
			case CharacterType:
				*u1 = (unsigned char) *u1;
				break;
			case UshortType:
				*u1 = (unsigned short) *u1;
				break;
			case UnsignedType:
				break;
			case LongType:
				*(long *)u1 = (long) *u1;
				break;
			case UlongType:
				*(unsigned long *)u1 = (unsigned long) *u1;
				break;
			case FloatType:
				*(double *)u1 = (float) *u1;
				break;
			case DoubleType:
				*(double *)u1 = (double) *u1;
				break;
			case PointerType:
				return UnaryExpression (
					ConvertOp,
					typ,
					NodeIndex,
					0,
					0);
			default:
				cerror ("invalid typ - fold_convert");
				break;
		}
		break;

	case LongType:
		l1 = (long *) &Node1parameter;
		switch (STab [typ].SType) {
			case CharType:
				*(int *)l1 = (char) *l1;
				break;
			case ShortType:
				*(int *)l1 = (short) *l1;
				break;
			case IntegerType:
			case BooleanType:
			case EnumerativeType:
				*(int *)l1 = (int) *l1;
				Node2parameter = 0;
				break;
			case UcharType:
			case CharacterType:
				*(unsigned *)l1 = (unsigned char) *l1;
				break;
			case UshortType:
				*(unsigned *)l1 = (unsigned short) *l1;
				break;
			case UnsignedType:
				*(unsigned *)l1 = (unsigned) *l1;
				break;
			case LongType:
				break;
			case UlongType:
				*(unsigned long *)l1 = (unsigned long) *l1;
				break;
			case FloatType:
				*(double *)l1 = (float) *l1;
				break;
			case DoubleType:
				*(double *)l1 = (double) *l1;
				break;
			case PointerType:
				/*
				return UnaryExpression (
					ConvertOp,
					typ,
					NodeIndex,
					0,
					0);
				*/
				TypeField = typ;
				return (NodeIndex);
			default:
				cerror ("invalid typ - fold_convert");
				break;
		}
		break;
	case UlongType:
		ul1 = (unsigned long *) &Node1parameter;
		switch (STab [typ].SType) {
			case CharType:
				*(int *)ul1 = (char) *ul1;
				break;
			case ShortType:
				*(int *)ul1 = (short) *ul1;
				break;
			case IntegerType:
			case BooleanType:
			case EnumerativeType:
				*(int *)ul1 = (int) *ul1;
				Node2parameter = 0;
				break;
			case UcharType:
			case CharacterType:
				*(unsigned *)ul1 = (unsigned char) *ul1;
				break;
			case UshortType:
				*(unsigned *)ul1 = (unsigned short) *ul1;
				break;
			case UnsignedType:
				*(unsigned *)ul1 = (unsigned) *ul1;
				break;
			case LongType:
				*(long *)ul1 = (long) *ul1;
				break;
			case UlongType:
				break;
			case FloatType:
				*(double *)ul1 = (float) *ul1;
				break;
			case DoubleType:
				*(double *)ul1 = (double) *ul1;
				break;
			case PointerType:
				return UnaryExpression (
					ConvertOp,
					typ,
					NodeIndex,
					0,
					0);
			default:
				cerror ("invalid typ - fold_convert");
				break;
		}
		break;

	case FloatType:
	case DoubleType:
		d1 = (double *) &Node1parameter;
		switch (STab [typ].SType) {
			case CharType:
				*(int *)d1 = (char) *d1;
				break;
			case ShortType:
				*(int *)d1 = (short) *d1;
				break;
			case IntegerType:
			case BooleanType:
			case EnumerativeType:
				*(int *)d1 = (int) *d1;
				Node2parameter = 0;
				break;
			case UcharType:
			case CharacterType:
				*(unsigned *)d1 = (unsigned char) *d1;
				break;
			case UshortType:
				*(unsigned *)d1 = (unsigned short) *d1;
				break;
			case UnsignedType:
				*(unsigned *)d1 = (unsigned) *d1;
				break;
			case LongType:
				*(long *)d1 = (long) *d1;
				break;
			case UlongType:
				*(unsigned long *)d1 = (unsigned long) *d1;
				break;
			case FloatType:
			case DoubleType:
				break;
			case PointerType:
				return UnaryExpression (
					ConvertOp,
					typ,
					NodeIndex,
					0,
					0);
			default:
				cerror ("invalid typ - fold_convert");
				break;
		}
		break;

	case PointerType:
		return UnaryExpression (
			ConvertOp,
			typ,
			NodeIndex,
			0,
			0);

	/* 16.09.87 */
	case ArrayType:
	case RecordType:
		if (STab [typ].SType == ArrayType) {
		   break;
		}
	/* 16.09.87 */
	default:
		/*
		cerror ("invalid typ - fold_convert");
		*/
		break;
    }
    TypeField = typ;
    return NodeIndex;
}


HdgRange
FoldUnary (op, typ, NodeIndex)
    int              op;
    SymbolTableRange typ;
    HdgRange	     NodeIndex;
{
    if (IsConstant (NodeIndex)) {
	switch (op) {
	    case UminusOp: return fold_uminus (typ, NodeIndex);
	    case NotOp:    return fold_not (typ, NodeIndex);
	    case BNotOp:   return fold_bNot (typ, NodeIndex);
	    case ConvertOp:return fold_convert (typ, NodeIndex);
	    default:       return UnaryExpression (op,typ,NodeIndex,0,0);
	}	
    }
    else {
	/* Added DK 08.12.88 */
	if (op==LoadadrOp && MajorOperator==UnaryExp && MinorOperator==DerefOp ||
	    op==UminusOp && MajorOperator==UnaryExp && MinorOperator==UminusOp ||
	    op==BNotOp && MajorOperator==UnaryExp && MinorOperator==BNotOp)
	    return(Unary1operand);
	/* end change */

	return UnaryExpression (op, typ, NodeIndex, 0, 0);
    }
}


static HdgRange
fold_add (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsZero(operand1)) return(fold_convert(typ, operand2));
	if (IsZero(operand2)) return(fold_convert(typ, operand1));
	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case IntegerType:
		case BooleanType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 + *i2;
			break;

		case PointerType:
			return BinaryExpression (AddOp, typ,
			    operand1, operand2);
			break;

		case UnsignedType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 + *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 + *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 + *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 + (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 + *d2;
			break;

		default:
			cerror ("invalid typ - fold_add");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		int const_type;

		if(IsConstant(operand1)) {
			int temp;

			temp = operand1;
			operand1 = operand2;
			operand2 = temp;
		}
		NodeIndex = operand2;
		const_type = TypeField;
		NodeIndex = operand1;
		if(MinorOperator == AddOp) {
			if(Bin2parameter == CONSTANT) {
				operand2 = FoldBinary(AddOp, const_type,
					Bin2operand, operand2);
				if(IsZero(Bin1operand))
					return(fold_convert(typ, operand2));
				if(IsZero(operand2))
					return(fold_convert(typ, Bin1operand));
				return BinaryExpression(AddOp, typ, Bin1operand,
					operand2);
			}
		} else if(MinorOperator == SubtOp) {
			if(Bin2parameter == CONSTANT) {
				operand2 = FoldBinary(SubtOp, const_type,
					operand2, Bin2operand);
				if(IsZero(Bin1operand))
					return(fold_convert(typ, operand2));
				if(IsZero(operand2))
					return(fold_convert(typ, Bin1operand));
				return BinaryExpression(AddOp, typ, Bin1operand,
					operand2);
			} else if(Bin1parameter == CONSTANT) {
				operand2 = FoldBinary(AddOp, const_type,
					Bin1operand, operand2);
				if(IsZero(Bin2operand))
					return(fold_convert(typ, operand2));
				return BinaryExpression (SubtOp, typ, operand2,
					Bin2operand);
			}
		}
	} else {
		NodeIndex = operand2;
		if(MinorOperator == UminusOp) {
			if(IsZero(Unary1operand))
				return(fold_convert(typ, operand1));
			return BinaryExpression(SubtOp, typ, operand1,
				Unary1operand);
		}
		/* if the right side is complicated then switch operands */
		if(MinorOperator == AddOp || MinorOperator == SubtOp) {
			int temp;

			temp = operand1;
			operand1 = operand2;
			operand2 = temp;
		}
		NodeIndex = operand1;
		if(MinorOperator == AddOp && IsConstant(Bin2operand)) {
			int temp;

			temp = Bin2operand;
			Bin2operand = operand2;
			operand2 = temp;
		} else if(MinorOperator == SubtOp && IsConstant(Bin1operand)) {
			int temp;

			TypeField = typ;
			temp = Bin1operand;
			Bin1operand = operand2;
			operand2 = temp;
		}
	}
	if(IsZero(operand1))
		return(fold_convert(typ, operand2));
	if(IsZero(operand2))
		return(fold_convert(typ, operand1));
	return BinaryExpression (AddOp, typ, operand1, operand2);
}


static HdgRange
fold_subt (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsZero(operand2)) return(fold_convert(typ, operand1));
	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case IntegerType:
		case BooleanType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 - *i2;
			break;

		case PointerType:
			return BinaryExpression (SubtOp, typ,
			    operand1, operand2);
			break;

		case UnsignedType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 - *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 - *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 - *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 - (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 - *d2;
			break;

		default:
			cerror ("invalid typ - fold_subt");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		int const_type;

		if(IsConstant(operand1)) {
		NodeIndex = operand1;
		const_type = TypeField;
		NodeIndex = operand2;
		if(MinorOperator == AddOp) {
			if(Bin2parameter == CONSTANT) {
				operand1 = FoldBinary(SubtOp, const_type,
					operand1, Bin2operand);
				if(IsZero(Bin1operand))
					return(fold_convert(typ, operand1));
				return BinaryExpression (SubtOp, typ, operand1,
					Bin1operand);
			}
		} else if(MinorOperator == SubtOp) {
			if(Bin2parameter == CONSTANT) {
				operand1 = FoldBinary(AddOp, const_type,
					Bin2operand, operand1);
				if(IsZero(Bin1operand))
					return(fold_convert(typ, operand1));
				return BinaryExpression (SubtOp, typ, operand1,
					Bin1operand);
			} else if(Bin1parameter == CONSTANT) {
				operand1 = FoldBinary(SubtOp, const_type,
					operand1, Bin1operand);
				if(IsZero(Bin2operand))
					return(fold_convert(typ, operand1));
				if(IsZero(operand1))
					return(fold_convert(typ, Bin2operand));
				return BinaryExpression(AddOp, typ, Bin2operand,
					operand1);
			} else {
				/* changed this to a FoldBinary call */
				return FoldBinary (AddOp, typ,
				     FoldBinary(SubtOp,const_type,Bin2operand,
					Bin1operand), operand1);
			}
		}
		} else { /* operand2 is a constant */
		NodeIndex = operand2;
		const_type = TypeField;
		NodeIndex = operand1;
		if(MinorOperator == AddOp) {
			if(Bin2parameter == CONSTANT) {
				operand2 = FoldBinary(SubtOp, const_type,
					Bin2operand, operand2);
				if(IsZero(Bin1operand))
					return(fold_convert(typ, operand2));
				if(IsZero(operand2))
					return(fold_convert(typ, Bin1operand));
				return BinaryExpression(AddOp, typ, Bin1operand,
					operand2);
			} else {
				int minus;

				NodeIndex = operand2;
				minus =	FoldUnary(UminusOp,TypeField,operand2);
				if(IsZero(operand1))
					return(fold_convert(typ, minus));
				if(IsZero(minus))
					return(fold_convert(typ, operand1));
				return BinaryExpression(AddOp, typ, operand1,
					minus);
			}
		} else if(MinorOperator == SubtOp) {
			if(Bin2parameter == CONSTANT) {
				int minus;

				operand2 = FoldBinary(AddOp, const_type,
					operand2, Bin2operand);
				minus = FoldUnary(UminusOp,const_type,operand2);
				if(IsZero(Bin1operand))
					return(fold_convert(typ, minus));
				if(IsZero(minus))
					return(fold_convert(typ, Bin1operand));
				return BinaryExpression(AddOp,typ,Bin1operand,
				     minus);
			} else if(Bin1parameter == CONSTANT) {
				operand2 = FoldBinary(SubtOp, const_type,
					Bin1operand, operand2);
				if(IsZero(Bin2operand))
					return(fold_convert(typ,operand2));
				return BinaryExpression (SubtOp, typ, operand2,
					Bin2operand);
			}
		}
		{
			int minus;

			minus = FoldUnary(UminusOp,const_type,operand2);
			if(IsZero(operand1))
				return(fold_convert(typ, minus));
			if(IsZero(minus))
				return(fold_convert(typ, operand1));
			return BinaryExpression (AddOp, typ, operand1, minus);
		}
	}
	}
	NodeIndex = operand1;
	if(MinorOperator == AddOp && IsConstant(Bin2operand)) {
		operand2 = BinaryExpression(SubtOp, typ, Bin1operand,
			operand2);
		NodeIndex = Bin2operand;
		if(IsZero(operand2))
			return(fold_convert(typ, NodeIndex));
		if(IsZero(NodeIndex))
			return(fold_convert(typ, operand2));
		return BinaryExpression(AddOp, typ, operand2, NodeIndex);
	} else if(MinorOperator == SubtOp && IsConstant(Bin1operand)) {
		operand2 = BinaryExpression(AddOp, typ, operand2,
			Bin2operand);
		NodeIndex = Bin1operand;
		if(IsZero(operand2))
			return(fold_convert(typ, NodeIndex));
		return BinaryExpression(SubtOp, typ, NodeIndex, operand2);
	}
	NodeIndex = operand2;
	if(MinorOperator == AddOp && IsConstant(Bin2operand)) {
		int minus;

		if(IsZero(Bin1operand))
			operand1 = fold_convert(typ, operand1);
		else
			operand1 = BinaryExpression(SubtOp, typ, operand1,
				Bin1operand);
		NodeIndex = Bin2operand;
		minus = FoldUnary(UminusOp, TypeField, NodeIndex);
		if(IsZero(operand1))
			return(fold_convert(typ, minus));
		if(IsZero(minus))
			return(fold_convert(typ, operand1));
		return BinaryExpression(AddOp, typ, operand1, minus);
	} else if(MinorOperator == SubtOp && IsConstant(Bin1operand)) {
		int minus;

		if(IsZero(operand1))
			operand1 = fold_convert(typ, Bin2operand);
		else if(IsZero(Bin2operand))
			operand1 = fold_convert(typ, operand1);
		else
		operand1 = BinaryExpression(AddOp, typ, operand1,
			Bin2operand);
		NodeIndex = Bin1operand;
		minus = FoldUnary(UminusOp, TypeField, NodeIndex);
		if(IsZero(operand1))
			return(fold_convert(typ, minus));
		if(IsZero(minus))
			return(fold_convert(typ, operand1));
		return BinaryExpression(AddOp, typ, operand1, minus);
	}
	if(MinorOperator == UminusOp) {
		if(IsZero(operand1))
			return(fold_convert(typ, Unary1operand));
		if(IsZero(Unary1operand))
			return(fold_convert(typ, operand1));
		return BinaryExpression (AddOp, typ, operand1, Unary1operand);
	}
	if(IsZero(operand2))
		return(fold_convert(typ, operand1));
	return BinaryExpression (SubtOp, typ, operand1, operand2);
}


static HdgRange
fold_mult (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsZero(operand1) && NoSideEffects(operand2))
		return(fold_convert(typ, operand1));
	if (IsZero(operand2) && NoSideEffects(operand1))
		return(fold_convert(typ, operand2));
	if (IsOne(operand1)) return(fold_convert(typ, operand2));
	if (IsOne(operand2)) return(fold_convert(typ, operand1));
	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case IntegerType:
		case BooleanType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 * *i2;
			break;

		case UnsignedType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 * *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 * *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 * *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 * (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 * *d2;
			break;

		default:
			cerror ("invalid typ - fold_mult");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		int const_type;

		if(IsConstant(operand1)) {
			int temp;

			temp = operand1;
			operand1 = operand2;
			operand2 = temp;
		}
		NodeIndex = operand2;
		const_type = TypeField;
		NodeIndex = operand1;
		if(MinorOperator == MultOp) {
			if(Bin2parameter == CONSTANT) {
				operand2 = FoldBinary(MultOp, const_type,
					Bin2operand, operand2);
				if(IsZero(Bin1operand))
					return(fold_convert(typ, Bin1operand));
				if(IsOne(Bin1operand))
					return(fold_convert(typ, operand2));
				if(IsOne(operand2))
					return(fold_convert(typ, Bin1operand));
				return BinaryExpression(MultOp,typ,Bin1operand,
					operand2);
			}
		} else if(MinorOperator == AddOp) {
			if(Bin2parameter == CONSTANT) {
				if(IsZero(Bin1operand))
					operand1 =
					   fold_convert(TypeField, Bin1operand);
				else if(IsOne(Bin1operand))
					operand1 =
					   fold_convert(TypeField, operand2);
				else if(IsOne(operand2))
					operand1 =
					   fold_convert(TypeField, Bin1operand);
				else
				operand1 = BinaryExpression(MultOp, TypeField,
					Bin1operand, CopyTree (operand2));
				operand2 = FoldBinary(MultOp, const_type,
					operand2, Bin2operand);
				if(IsZero(operand1))
					return(fold_convert(typ, operand2));
				if(IsZero(operand2))
					return(fold_convert(typ, operand1));
				return BinaryExpression(AddOp,typ,operand1,
					operand2);
			}
		} else if(MinorOperator == SubtOp) {
			if(Bin2parameter == CONSTANT) {
				if(IsZero(Bin1operand))
					operand1 =
					   fold_convert(TypeField, Bin1operand);
				else if(IsOne(Bin1operand))
					operand1 =
					   fold_convert(TypeField, operand2);
				else if(IsOne(operand2))
					operand1 =
					   fold_convert(TypeField, Bin1operand);
				else
				operand1 = BinaryExpression(MultOp, TypeField,
						     /* 27.10.88 -WB */
					Bin1operand, CopyTree(operand2));
				operand2 = FoldBinary(MultOp, const_type,
					operand2, Bin2operand);
				if(IsZero(operand2))
					return(fold_convert(typ, operand1));
				return BinaryExpression(SubtOp,typ,operand1,
					operand2);
			} else if(Bin1parameter == CONSTANT) {
				operand1 = FoldBinary(MultOp, const_type,
					CopyTree(operand2), Bin1operand);
				if(IsZero(Bin2operand))
					operand2 =
					   fold_convert(TypeField, Bin2operand);
				else if(IsOne(Bin2operand))
					operand2 =
					   fold_convert(TypeField, operand2);
				else if(IsOne(operand2))
					operand2 =
					   fold_convert(TypeField, Bin2operand);
				else
				operand2 = BinaryExpression(MultOp, TypeField,
					Bin2operand, operand2);
				if(IsZero(operand2))
					return(fold_convert(typ, operand1));
				return BinaryExpression(SubtOp,typ,operand1,
					operand2);
			}
		}
	} else {
		NodeIndex = operand1;
		if(MinorOperator == MultOp && IsConstant(Bin2operand)) {
			int temp;

			temp = Bin2operand;
			Bin2operand = operand2;
			operand2 = temp;
		}
	}
	if(IsZero(operand1) && NoSideEffects(operand2))
		return(fold_convert(typ, operand1));
	if(IsZero(operand2) && NoSideEffects(operand1))
		return(fold_convert(typ, operand2));
	if(IsOne(operand1))
		return(fold_convert(typ, operand2));
	if(IsOne(operand2))
		return(fold_convert(typ, operand1));
	return BinaryExpression (MultOp, typ, operand1, operand2);
}


static HdgRange
fold_div (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsOne(operand2)) return (operand1);
	if (IsZero(operand2)) {
		uerror("division by zero");
		return( node3(LitOp, typ, 0, 0) );
	}
	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case IntegerType:
		case BooleanType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 / *i2;
			break;

		case UnsignedType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 / *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 / *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 / *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 / (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 / *d2;
			break;

		default:
			cerror ("invalid typ - fold_div");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (DivOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_or (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
    HdgRange		NodeIndex;
    int			*i1;
    unsigned		*u1;
    long		*l1;
    unsigned long	*ul1;
    double      	*d1;
    void		*p2;

    if (IsConstant (operand1) && !IsZero(operand1)) {
	return( node3(LitOp, typ, 1, 0) );
    }
    if (IsConstant (operand1) && IsConstant (operand2)) {
    NodeIndex = operand1;
    switch (STab [TypeField].SType) {

	case ShortType:
	case IntegerType:
	case BooleanType:
	case CharType:
	case EnumerativeType:
	    i1 = (int *) &Node1parameter;
	    NodeIndex = operand2;
	    p2 = (void *) &Node1parameter;
	    switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			    *(int *)p2 = *i1 || *(int *)p2;
			    break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			    *(int *)p2 = *i1 || *(unsigned *)p2;
			    break;
		    case LongType:
			    *(long *)p2 = *i1 || *(long *)p2;
			    break;
		    case UlongType:
			    *(long *)p2 = *i1 || *(unsigned long *)p2;
			    break;
		    case FloatType:
			    *(long *)p2 = *i1 || (float) *(double *)p2;
			    break;
		    case DoubleType:
			    *(long *)p2 = *i1 || *(double *)p2;
			    break;
		    default:
			    cerror ("invalid typ - fold_or");
			    break;
	    }
	    break;

	case UshortType:
	case UnsignedType:
	case UcharType:
	case CharacterType:
	    u1 = (unsigned *) &Node1parameter;
	    NodeIndex = operand2;
	    p2 = (void *) &Node1parameter;
	    switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			    *(int *)p2 = *u1 || *(int *)p2;
			    break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			    *(int *)p2 = *u1 || *(unsigned *)p2;
			    break;
		    case LongType:
			    *(long *)p2 = *u1 || *(long *)p2;
			    break;
		    case UlongType:
			    *(long *)p2 = *u1 || *(unsigned long *)p2;
			    break;
		    case FloatType:
			    *(long *)p2 = *u1 || (float) *(double *)p2;
			    break;
		    case DoubleType:
			    *(long *)p2 = *u1 || *(double *)p2;
			    break;
		    default:
			    cerror ("invalid typ - fold_or");
			    break;
	    }
	    break;

	case LongType:
	    l1 = (long *) &Node1parameter;
	    NodeIndex = operand2;
	    p2 = (void *) &Node1parameter;
	    switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			    *(int *)p2 = *l1 || *(int *)p2;
			    break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			    *(int *)p2 = *l1 || *(unsigned *)p2;
			    break;
		    case LongType:
			    *(long *)p2 = *l1 || *(long *)p2;
			    break;
		    case UlongType:
			    *(long *)p2 = *l1 || *(unsigned long *)p2;
			    break;
		    case FloatType:
			    *(long *)p2 = *l1 || (float) *(double *)p2;
			    break;
		    case DoubleType:
			    *(long *)p2 = *l1 || *(double *)p2;
			    break;
		    default:
			    cerror ("invalid typ - fold_or");
			    break;
	    }
	    break;

	case UlongType:
	    ul1 = (unsigned long *) &Node1parameter;
	    NodeIndex = operand2;
	    p2 = (void *) &Node1parameter;
	    switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			    *(int *)p2 = *ul1 || *(int *)p2;
			    break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			    *(int *)p2 = *ul1 || *(unsigned *)p2;
			    break;
		    case LongType:
			    *(long *)p2 = *ul1 || *(long *)p2;
			    break;
		    case UlongType:
			    *(long *)p2 = *ul1 || *(unsigned long *)p2;
			    break;
		    case FloatType:
			    *(long *)p2 = *ul1 || (float) *(double *)p2;
			    break;
		    case DoubleType:
			    *(long *)p2 = *ul1 || *(double *)p2;
			    break;
		    default:
			    cerror ("invalid typ - fold_or");
			    break;
	    }
	    break;

	case FloatType:
	    d1 = (double *) &Node1parameter;
	    NodeIndex = operand2;
	    p2 = (void *) &Node1parameter;
	    switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			    *(int *)p2 = (float) *d1 || *(int *)p2;
			    break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			    *(int *)p2 = (float) *d1 || *(unsigned *)p2;
			    break;
		    case LongType:
			    *(long *)p2 = (float) *d1 || *(long *)p2;
			    break;
		    case UlongType:
			    *(long *)p2 = (float) *d1 || *(unsigned long *)p2;
			    break;
		    case FloatType:
			    *(long *)p2 = (float) *d1 || (float) *(double *)p2;
			    break;
		    case DoubleType:
			    *(long *)p2 = (float) *d1 || *(double *)p2;
			    break;
		    default:
			    cerror ("invalid typ - fold_or");
			    break;
	    }
	    break;

	case DoubleType:
	    d1 = (double *) &Node1parameter;
	    NodeIndex = operand2;
	    p2 = (void *) &Node1parameter;
	    switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			    *(int *)p2 = *d1 || *(int *)p2;
			    break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			    *(int *)p2 = *d1 || *(unsigned *)p2;
			    break;
		    case LongType:
			    *(long *)p2 = *d1 || *(long *)p2;
			    break;
		    case UlongType:
			    *(long *)p2 = *d1 || *(unsigned long *)p2;
			    break;
		    case FloatType:
			    *(long *)p2 = *d1 || (float) *(double *)p2;
			    break;
		    case DoubleType:
			    *(long *)p2 = *d1 || *(double *)p2;
			    break;
		    default:
			    cerror ("invalid typ - fold_or");
			    break;
	    }
	    break;

	default:
	    cerror ("invalid typ - fold_or");
	    break;
    }
    TypeField = typ;
    return NodeIndex;
    }
    else {
	return BinaryExpression (OrOp, typ, operand1, operand2);
    }
}


static HdgRange
fold_xor (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
    HdgRange		NodeIndex;
    int			*i1;
    unsigned		*u1;
    long		*l1;
    unsigned long	*ul1;
    double          	*d1;
    void		*p2;

    if (IsConstant (operand1) && IsConstant (operand2)) {
	NodeIndex = operand1;
	switch (STab [TypeField].SType) {

	    case ShortType:
	    case IntegerType:
	    case BooleanType:
	    case CharType:
	    case EnumerativeType:
		i1 = (int *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = (*i1 != 0) ^ (*(int *)p2 != 0);
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = (*i1 != 0) ^ (*(unsigned *)p2 != 0);
			break;
		    case LongType:
			*(long *)p2 = (*i1 != 0) ^ (*(long *)p2 != 0);
			break;
		    case UlongType:
			*(long *)p2 = (*i1 != 0) ^ (*(unsigned long *)p2 != 0);
			break;
		    case FloatType:
			*(long *)p2 = (*i1 != 0) ^ ((float) *(double *)p2 != 0);
			break;
		    case DoubleType:
			*(long *)p2 = (*i1 != 0) ^ (*(double *)p2 != 0);
			break;
		    default:
			cerror ("invalid typ - fold_xor");
			break;
		}
		break;

	    case UshortType:
	    case UnsignedType:
	    case UcharType:
	    case CharacterType:
		u1 = (unsigned *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = (*u1 != 0) ^ (*(int *)p2 != 0);
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = (*u1 != 0) ^ (*(unsigned *)p2 != 0);
			break;
		    case LongType:
			*(long *)p2 = (*u1 != 0) ^ (*(long *)p2 != 0);
			break;
		    case UlongType:
			*(long *)p2 = (*u1 != 0) ^ (*(unsigned long *)p2 != 0);
			break;
		    case FloatType:
			*(long *)p2 = (*u1 != 0) ^ ((float) *(double *)p2 != 0);
			break;
		    case DoubleType:
			*(long *)p2 = (*u1 != 0) ^ (*(double *)p2 != 0);
			break;
		    default:
			cerror ("invalid typ - fold_xor");
			break;
	    }
	    break;

	    case LongType:
		l1 = (long *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = (*l1 != 0) ^ (*(int *)p2 != 0);
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = (*l1 != 0) ^ (*(unsigned *)p2 != 0);
			break;
		    case LongType:
			*(long *)p2 = (*l1 != 0) ^ (*(long *)p2 != 0);
			break;
		    case UlongType:
			*(long *)p2 = (*l1 != 0) ^ (*(unsigned long *)p2 != 0);
			break;
		    case FloatType:
			*(long *)p2 = (*l1 != 0) ^ ((float) *(double *)p2 != 0);
			break;
		    case DoubleType:
			*(long *)p2 = (*l1 != 0) ^ (*(double *)p2 != 0);
			break;
		    default:
			cerror ("invalid typ - fold_xor");
			break;
	    }
	    break;

	    case UlongType:
		ul1 = (unsigned long *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = (*ul1 != 0) ^ (*(int *)p2 != 0);
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = (*ul1 != 0) ^ (*(unsigned *)p2 != 0);
			break;
		    case LongType:
			*(long *)p2 = (*ul1 != 0) ^ (*(long *)p2 != 0);
			break;
		    case UlongType:
			*(long *)p2 = (*ul1 != 0) ^ (*(unsigned long *)p2 != 0);
			break;
		    case FloatType:
			*(long *)p2 = (*ul1 != 0) ^ ((float) *(double *)p2 != 0);
			break;
		    case DoubleType:
			*(long *)p2 = (*ul1 != 0) ^ (*(double *)p2 != 0);
			break;
		    default:
			cerror ("invalid typ - fold_xor");
			break;
	    }
	    break;

	    case FloatType:
		d1 = (double *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = ((float) *d1 != 0) ^ (*(int *)p2 != 0);
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = ((float) *d1 != 0) ^ (*(unsigned *)p2 != 0);
			break;
		    case LongType:
			*(long *)p2 = ((float) *d1 != 0) ^ (*(long *)p2 != 0);
			break;
		    case UlongType:
			*(long *)p2 = ((float) *d1 != 0) ^ (*(unsigned long *)p2 != 0);
			break;
		    case FloatType:
			*(long *)p2 = ((float) *d1 != 0) ^ ((float) *(double *)p2 != 0);
			break;
		    case DoubleType:
			*(long *)p2 = ((float) *d1 != 0) ^ (*(double *)p2 != 0);
			break;
		    default:
			cerror ("invalid typ - fold_xor");
			break;
	    }
	    break;

	    case DoubleType:
		d1 = (double *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = (*d1 != 0) ^ (*(int *)p2 != 0);
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = (*d1 != 0) ^ (*(unsigned *)p2 != 0);
			break;
		    case LongType:
			*(long *)p2 = (*d1 != 0) ^ (*(long *)p2 != 0);
			break;
		    case UlongType:
			*(long *)p2 = (*d1 != 0) ^ (*(unsigned long *)p2 != 0);
			break;
		    case FloatType:
			*(long *)p2 = (*d1 != 0) ^ ((float) *(double *)p2 != 0);
			break;
		    case DoubleType:
			*(long *)p2 = (*d1 != 0) ^ (*(double *)p2 != 0);
			break;
		    default:
			cerror ("invalid typ - fold_xor");
			break;
	    }
	    break;

	default:
	    cerror ("invalid typ - fold_xor");
	    break;
	}
	TypeField = typ;
	return NodeIndex;
    }
    else {
	return BinaryExpression (XorOp, typ, operand1, operand2);
    }
}


static HdgRange
fold_and (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
    HdgRange		NodeIndex;
    int			*i1;
    unsigned		*u1;
    long		*l1;
    unsigned long	*ul1;
    double      	*d1;
    void		*p2;

    if (IsZero (operand1))
	return( operand1 );
    if (IsConstant (operand1) && IsConstant (operand2)) {
	NodeIndex = operand1;
	switch (STab [TypeField].SType) {

	    case ShortType:
	    case IntegerType:
	    case BooleanType:
	    case CharType:
	    case EnumerativeType:
		i1 = (int *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = *i1 && *(int *)p2;
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = *i1 && *(unsigned *)p2;
			break;
		    case LongType:
			*(long *)p2 = *i1 && *(long *)p2;
			break;
		    case UlongType:
			*(long *)p2 = *i1 && *(unsigned long *)p2;
			break;
		    case FloatType:
			*(long *)p2 = *i1 && (float) *(double *)p2;
			break;
		    case DoubleType:
			*(long *)p2 = *i1 && *(double *)p2;
			break;
		    default:
			cerror ("invalid typ - fold_and");
			break;
	    }
	    break;

	    case UshortType:
	    case UnsignedType:
	    case UcharType:
	    case CharacterType:
		u1 = (unsigned *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = *u1 && *(int *)p2;
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = *u1 && *(unsigned *)p2;
			break;
		    case LongType:
			*(long *)p2 = *u1 && *(long *)p2;
			break;
		    case UlongType:
			*(long *)p2 = *u1 && *(unsigned long *)p2;
			break;
		    case FloatType:
			*(long *)p2 = *u1 && (float) *(double *)p2;
			break;
		    case DoubleType:
			*(long *)p2 = *u1 && *(double *)p2;
			break;
		    default:
			cerror ("invalid typ - fold_and");
			break;
	    }
	    break;

	    case LongType:
		l1 = (long *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = *l1 && *(int *)p2;
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = *l1 && *(unsigned *)p2;
			break;
		    case LongType:
			*(long *)p2 = *l1 && *(long *)p2;
			break;
		    case UlongType:
			*(long *)p2 = *l1 && *(unsigned long *)p2;
			break;
		    case FloatType:
			*(long *)p2 = *l1 && (float) *(double *)p2;
			break;
		    case DoubleType:
			*(long *)p2 = *l1 && *(double *)p2;
			break;
		    default:
			cerror ("invalid typ - fold_and");
			break;
	    }
	    break;

	    case UlongType:
		ul1 = (unsigned long *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = *ul1 && *(int *)p2;
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = *ul1 && *(unsigned *)p2;
			break;
		    case LongType:
			*(long *)p2 = *ul1 && *(long *)p2;
			break;
		    case UlongType:
			*(long *)p2 = *ul1 && *(unsigned long *)p2;
			break;
		    case FloatType:
			*(long *)p2 = *ul1 && (float) *(double *)p2;
			break;
		    case DoubleType:
			*(long *)p2 = *ul1 && *(double *)p2;
			break;
		    default:
			cerror ("invalid typ - fold_and");
			break;
	    }
	    break;

	    case FloatType:
		d1 = (double *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = (float) *d1 && *(int *)p2;
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = (float) *d1 && *(unsigned *)p2;
			break;
		    case LongType:
			*(long *)p2 = (float) *d1 && *(long *)p2;
			break;
		    case UlongType:
			*(long *)p2 = (float) *d1 && *(unsigned long *)p2;
			break;
		    case FloatType:
			*(long *)p2 = (float) *d1 && (float) *(double *)p2;
			break;
		    case DoubleType:
			*(long *)p2 = (float) *d1 && *(double *)p2;
			break;
		    default:
			cerror ("invalid typ - fold_and");
			break;
	    }
	    break;

	    case DoubleType:
		d1 = (double *) &Node1parameter;
		NodeIndex = operand2;
		p2 = (void *) &Node1parameter;
		switch (STab [TypeField].SType) {

		    case ShortType:
		    case IntegerType:
		    case BooleanType:
		    case CharType:
		    case EnumerativeType:
			*(int *)p2 = *d1 && *(int *)p2;
			break;
		    case UshortType:
		    case UnsignedType:
		    case UcharType:
		    case CharacterType:
			*(int *)p2 = *d1 && *(unsigned *)p2;
			break;
		    case LongType:
			*(long *)p2 = *d1 && *(long *)p2;
			break;
		    case UlongType:
			*(long *)p2 = *d1 && *(unsigned long *)p2;
			break;
		    case FloatType:
			*(long *)p2 = *d1 && (float) *(double *)p2;
			break;
		    case DoubleType:
			*(long *)p2 = *d1 && *(double *)p2;
			break;
		    default:
			cerror ("invalid typ - fold_and");
			break;
		    }
		    break;

	    default:
		cerror ("invalid typ - fold_and");
		break;
	}
	TypeField = typ;
	return NodeIndex;
    }
    else {
	return BinaryExpression (AndOp, typ, operand1, operand2);
    }
}


static HdgRange
fold_eq (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	NodeIndex = operand1;
	switch (STab [TypeField].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 == *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 == *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 == *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 == *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 == (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 == *d2;
			break;

		default:
			cerror ("invalid typ - fold_eq");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (EqOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_lt (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	NodeIndex = operand1;
	if ((STab[TypeField].SType == UnsignedType ||
	STab[TypeField].SType == UshortType ||
	STab[TypeField].SType == UlongType ||
	STab[TypeField].SType == UcharType ||
	STab[TypeField].SType == CharacterType) &&
	IsZero (operand2) && NoSideEffects(operand1))
		return(fold_convert(typ, operand2));
	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [TypeField].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 < *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 < *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 < *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 < *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 < (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 < *d2;
			break;

		default:
			cerror ("invalid typ - fold_lt");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (LtOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_gt (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	NodeIndex = operand1;
	switch (STab [TypeField].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 > *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 > *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 > *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 > *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 > (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 > *d2;
			break;

		default:
			cerror ("invalid typ - fold_gt");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (GtOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_le (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	NodeIndex = operand1;
	switch (STab [TypeField].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 <= *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 <= *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 <= *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 <= *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 <= (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 <= *d2;
			break;

		default:
			cerror ("invalid typ - fold_le");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (LeOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_ge (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	NodeIndex = operand1;
	switch (STab [TypeField].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 >= *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 >= *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 >= *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 >= *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 >= (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 >= *d2;
			break;

		default:
			cerror ("invalid typ - fold_ge");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (GeOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_ne (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;
	double          *d1, *d2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	NodeIndex = operand1;
	switch (STab [TypeField].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 != *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 != *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 != *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 != *ul2;
			break;

		case FloatType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = (float) *d1 != (float) *d2;
			break;

		case DoubleType:
			NodeIndex = operand1;
			d1 = (double *) &Node1parameter;
			NodeIndex = operand2;
			d2 = (double *) &Node1parameter;
			*d2 = *d1 != *d2;
			break;

		default:
			cerror ("invalid typ - fold_ne");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (NeOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_mod (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 % *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 % *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 % *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 % *ul2;
			break;

		default:
			cerror ("invalid typ - fold_mod");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	}
	else {
		return BinaryExpression (ModOp, typ, operand1, operand2);
	}
}


static HdgRange
fold_bAnd (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 & *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 & *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 & *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 & *ul2;
			break;

		default:
			cerror ("invalid typ - fold_bAnd");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		if(IsConstant(operand1)) {
			int temp;

			temp = operand1;
			operand1 = operand2;
			operand2 = temp;
		}
		NodeIndex = operand1;
		if(MinorOperator == BAndOp) {
			if(Bin2parameter == CONSTANT) {
				operand2 = FoldBinary(BAndOp, TypeField,
					Bin2operand, operand2);
				return BinaryExpression(BAndOp,typ,Bin1operand,
					operand2);
			}
		}
	} else {
		NodeIndex = operand1;
		if(MinorOperator == BAndOp && IsConstant(Bin2operand)) {
			int temp;

			temp = Bin2operand;
			Bin2operand = operand2;
			operand2 = temp;
		}
	}
	return BinaryExpression (BAndOp, typ, operand1, operand2);
}


static HdgRange
fold_bOr (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 | *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 | *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 | *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 | *ul2;
			break;

		default:
			cerror ("invalid typ - fold_bOr");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		if(IsConstant(operand1)) {
			int temp;

			temp = operand1;
			operand1 = operand2;
			operand2 = temp;
		}
		NodeIndex = operand1;
		if(MinorOperator == BOrOp) {
			if(Bin2parameter == CONSTANT) {
				operand2 = FoldBinary(BOrOp, TypeField,
					Bin2operand, operand2);
				return BinaryExpression(BOrOp,typ,Bin1operand,
					operand2);
			}
		}
	} else {
		NodeIndex = operand1;
		if(MinorOperator == BOrOp && IsConstant(Bin2operand)) {
			int temp;

			temp = Bin2operand;
			Bin2operand = operand2;
			operand2 = temp;
		}
	}
	return BinaryExpression (BOrOp, typ, operand1, operand2);
}


static HdgRange
fold_bXor (typ, operand1, operand2)
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
	HdgRange	NodeIndex;
	int		*i1, *i2;
	unsigned	*u1, *u2;
	long		*l1, *l2;
	unsigned long	*ul1, *ul2;

	if (IsConstant (operand1) && IsConstant (operand2)) {
	switch (STab [typ].SType) {

		case ShortType:
		case IntegerType:
		case BooleanType:
		case CharType:
		case EnumerativeType:
			NodeIndex = operand1;
			i1 = (int *) &Node1parameter;
			NodeIndex = operand2;
			i2 = (int *) &Node1parameter;
			*i2 = *i1 ^ *i2;
			break;

		case UshortType:
		case UnsignedType:
		case UcharType:
		case CharacterType:
			NodeIndex = operand1;
			u1 = (unsigned *) &Node1parameter;
			NodeIndex = operand2;
			u2 = (unsigned *) &Node1parameter;
			*u2 = *u1 ^ *u2;
			break;

		case LongType:
			NodeIndex = operand1;
			l1 = (long *) &Node1parameter;
			NodeIndex = operand2;
			l2 = (long *) &Node1parameter;
			*l2 = *l1 ^ *l2;
			break;

		case UlongType:
			NodeIndex = operand1;
			ul1 = (unsigned long *) &Node1parameter;
			NodeIndex = operand2;
			ul2 = (unsigned long *) &Node1parameter;
			*ul2 = *ul1 ^ *ul2;
			break;

		default:
			cerror ("invalid typ - fold_bXor");
			break;
	}
	TypeField = typ;
	return NodeIndex;
	} else if(IsConstant(operand1) || IsConstant(operand2)) {
		if(IsConstant(operand1)) {
			int temp;

			temp = operand1;
			operand1 = operand2;
			operand2 = temp;
		}
		NodeIndex = operand1;
		if(MinorOperator == BXorOp) {
			if(Bin2parameter == CONSTANT) {
				operand2 = FoldBinary(BXorOp, TypeField,
					Bin2operand, operand2);
				return BinaryExpression(BXorOp,typ,Bin1operand,
					operand2);
			}
		}
	} else {
		NodeIndex = operand1;
		if(MinorOperator == BXorOp && IsConstant(Bin2operand)) {
			int temp;

			temp = Bin2operand;
			Bin2operand = operand2;
			operand2 = temp;
		}
	}
	return BinaryExpression (BXorOp, typ, operand1, operand2);
}


HdgRange
FoldBinary (op, typ, operand1, operand2)
	int	op;
	SymbolTableRange	typ;
	HdgRange		operand1, operand2;
{
		switch (op) {

			case AddOp:
				return fold_add (typ, operand1, operand2);
				break;

			case SubtOp:
				return fold_subt (typ, operand1, operand2);
				break;

			case MultOp:
				return fold_mult (typ, operand1, operand2);
				break;

			case DivOp:
				return fold_div (typ, operand1, operand2);
				break;

			case OrOp:
				return fold_or (typ, operand1, operand2);
				break;

			case AndOp:
				return fold_and (typ, operand1, operand2);
				break;

			case XorOp:
				return fold_xor (typ, operand1, operand2);
				break;

			case ShlOp:
				return fold_shl (typ, operand1, operand2);
				break;

			case ShrOp:
				return fold_shr (typ, operand1, operand2);
				break;

			case EqOp:
				return fold_eq (typ, operand1, operand2);
				break;

			case LtOp:
				return fold_lt (typ, operand1, operand2);
				break;

			case GtOp:
				return fold_gt (typ, operand1, operand2);
				break;

			case LeOp:
				return fold_le (typ, operand1, operand2);
				break;

			case GeOp:
				return fold_ge (typ, operand1, operand2);
				break;

			case NeOp:
				return fold_ne (typ, operand1, operand2);
				break;

			case ModOp:
				return fold_mod (typ, operand1, operand2);
				break;

			case BAndOp:
				return fold_bAnd (typ, operand1, operand2);
				break;

			case BOrOp:
				return fold_bOr (typ, operand1, operand2);
				break;

			case BXorOp:
				return fold_bXor (typ, operand1, operand2);
				break;

			default:
				return BinaryExpression (
					op,
					typ,
					operand1,
					operand2);
				break;
		}
}


FoldSubscript (subscript)
	HdgRange	subscript;
{
	HdgRange	NodeIndex;
	HdgRange	new_index;
	int		constant;

	NodeIndex = subscript;
	NodeIndex = Bin2operand;
	if(NodeIndex == -1) return(subscript);
	switch (MinorOperator) {
		case AddOp:
			if (IsConstant (Bin1operand)) {
				new_index = Bin2operand;
				NodeIndex = Bin1operand;
				constant = Node1parameter;
			}
			else if (IsConstant (Bin2operand)) {
				new_index = Bin1operand;
				NodeIndex = Bin2operand;
				constant = Node1parameter;
			}
			else {
				break;
			}
			NodeIndex = subscript;
			Bin2parameter += constant;
			Bin2operand = new_index;
			break;
		case SubtOp:
			if (IsConstant (Bin1operand)) {
				new_index = FoldUnary (
                                   UminusOp,
                                   TypeField,
                                   Bin2operand);
				NodeIndex = Bin1operand;
				constant = Node1parameter;
			}
			else if (IsConstant (Bin2operand)) {
				new_index = Bin1operand;
				NodeIndex = Bin2operand;
				constant = -Node1parameter;
			}
			else {
				break;
			}
			NodeIndex = subscript;
			Bin2parameter += constant;
			Bin2operand = new_index;
			break;
		case LitOp:
			constant = Node1parameter;
			NodeIndex = subscript;
			Bin2parameter += constant;
			Bin2operand = -1;
			break;
	}
	return (subscript);
}


HdgRange
FoldTree (NodeIndex)
	HdgRange	NodeIndex;
{
	if ( NodeIndex == -1 ) return NodeIndex;  /* 03.12.87 */
	switch (MajorOperator) {

		case UnaryExp:
			Unary1operand = FoldTree (Unary1operand);
			if (IsConstant (Unary1operand)) {
				return FoldUnary (
				   MinorOperator,
				   TypeField,
				   Unary1operand);
			}
			else {
				return NodeIndex;
			}
			break;

		case BinaryExp:
			Bin1operand = FoldTree (Bin1operand);
			Bin2operand = FoldTree (Bin2operand);
			if ( Bin2operand == -1 ) return NodeIndex;
				/* 03.12.87 */
			if (
			   IsConstant (Bin1operand)
			&&
			   IsConstant (Bin2operand)
			) {
				return FoldBinary (
				   MinorOperator,
				   TypeField,
				   Bin1operand,
				   Bin2operand);
			}
			else {
				return NodeIndex;
			}
			break;

		case ifthenelse:
			IfCondField = FoldTree (IfCondField);
			IfTrueField = FoldTree (IfTrueField);
			IfFalseField = FoldTree (IfFalseField);
			if (
			   IsConstant (IfCondField)
			/*
			&&
		     *     IsConstant (IfTrueField)
			&&
		     *     IsConstant (IfFalseField)
			*/
			) {
				if(IsZero(IfCondField))
					return IfFalseField;
				else		
					return IfTrueField;
			} else {
				return NodeIndex;
			}
			break;

		default:
			return NodeIndex;
			break;
	}
}
