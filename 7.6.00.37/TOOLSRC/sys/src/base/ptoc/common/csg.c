/* @(#)common/csg.c

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

#include "../common/csg.def"
#include "../common/globals.ci"
#include "../common/csg.i"
#include "../common/opcode.i"

#define pascal_nil 0


SetMinor (NodeIndex, opcode)
	HdgRange NodeIndex;
	int opcode;
{
	Node1field = ((Node1field >> HdgOpSize) << HdgOpSize) + opcode;
}


HdgRange
node3 (op, typ, p1, p2)
	int op, typ, p1, p2;
{
	HdgRange NodeIndex;

#ifdef __hp9000s700
	if (op == LitOp) {
		/* make sure that the address of Node1parameter is a multiple */
		/* of 8, because doubles must be aligned this way on HPs */
		NodeIndex = ptrg;
		if ((int) &Node1parameter & 0x7)
			++ptrg;
	}
#endif
	TestOvflw (NodeNodeSize);
	NodeIndex = ptrg;
	Node1field = (node << HdgOpSize) + op;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	Node1parameter = p1;
	Node2parameter = p2;
	ptrg = NodeIndex + NodeNodeSize;
	return NodeIndex;
}


HdgRange
UnaryExpression (op, typ, op1, p1, p2)
	int op, typ, op1, p1, p2;
{
	int NodeIndex;

	TestOvflw (UnaryNodeSize);
	NodeIndex = ptrg;
	Node1field = (UnaryExp << HdgOpSize) + op;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	Unary1operand = op1;
	Unary1parameter = p1;
	Unary2parameter = p2;
	ptrg = NodeIndex + UnaryNodeSize;
#ifndef PYRAMID_DOUBTFUL
	if ( op == LoadadrOp || op == StorepadrOp ) {
	int SavIndex;
	SavIndex = NodeIndex;
	NodeIndex = op1;
		if ( MinorOperator == IdOp 			&&
		     ( STab[Node1parameter].SClass == AUTO    ||
		       STab[Node1parameter].SClass == REGISTER ) )
		   STab[Node1parameter].SClass = AUTONR;
	NodeIndex = SavIndex;
	}
#endif
	return NodeIndex;
}


HdgRange
BinaryExpression (op, typ, op1, op2)
	int op, typ, op1, op2;
{
	int NodeIndex;

	TestOvflw (BinNodeSize);
	NodeIndex = ptrg;
	Node1field = (BinaryExp << HdgOpSize) + op;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	Bin1operand = op1;
	Bin2operand = op2;
	if (op == SubscriptOp) {
		Bin1parameter = 0;
		Bin2parameter = 0;
	}
	else {
		  	Bin1parameter = IsConstant(op1);
		Bin2parameter = IsConstant(op2);
	}
	ptrg = NodeIndex + BinNodeSize;
	return NodeIndex;
}


SetBinaryParameters (NodeIndex, p1, p2)
	HdgRange NodeIndex;
	int p1, p2;
{
		Bin1parameter = p1;
		Bin2parameter = p2;
}


AppendList (major, typ, exp1, exp2)
	int major, typ, exp1, exp2;
{
	HdgRange NodeIndex;

	NodeIndex = exp2;
	if (NextLink != -1) {
		cerror ("internal error - AppendList");
	}
	NodeIndex = exp1;
	TypeField = typ;
	if (ListHeadField == -1) {
		ListHeadField = exp2;
	}
	else {
		NodeIndex = ListTailField;
		NextLink = exp2;
	}
	NodeIndex = exp1;
	ListTailField = exp2;
	ListSizeField = ListSizeField + 1;
}


InsertList (major, typ, exp1, exp2)
	int major, typ, exp1, exp2;
{
	HdgRange NodeIndex;
	HdgRange HeadNode;

	NodeIndex = exp2;
	if (NextLink != -1) {
		cerror ("internal error - AppendList");
	}
	NodeIndex = exp1;
	TypeField = typ;
	if (ListTailField == -1) {
		ListTailField = exp2;
	}
	else {
		HeadNode = ListHeadField;
		NodeIndex = exp2;
		NextLink = HeadNode;
	}
	NodeIndex = exp1;
	ListHeadField = exp2;;
	ListSizeField = ListSizeField + 1;
}


LinkLists (L1, L2)
	int L1, L2;
{
	HdgRange NodeIndex, L2head, L2tail;
	int L2size;

	NodeIndex = L2;
/*  	L2head = ListHeadField;  07-06-88 WB */
	if ((L2head = ListHeadField) == -1) return;
	L2tail = ListTailField;
	L2size = ListSizeField;
	NodeIndex = L1;
	if (ListHeadField == -1) {
		ListHeadField = L2head;
	}
	else {
		NodeIndex = ListTailField;
		NextLink = L2head;
	}
	NodeIndex = L1;
	ListTailField = L2tail;
	ListSizeField = ListSizeField + L2size;
}


HdgRange
MakeList (major, typ, op1, op2, op3)
	int major, typ, op1, op2, op3;
{
	HdgRange NodeIndex;

	TestOvflw (ListNodeSize);
	NodeIndex = ptrg;
	Node1field = ExpList << HdgOpSize;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	ListHeadField = -1;
	ListTailField = -1;
	ListSizeField = 0;
	ptrg = NodeIndex + ListNodeSize;
	if (op1 != -1) {
		AppendList (major, typ, NodeIndex, op1);
		if (op2 != -1) {
			AppendList (major, typ, NodeIndex, op2);
			if (op3 != -1) {
				AppendList (major, typ, NodeIndex, op3);
			}
		}
	}
	return NodeIndex;
}


HdgRange
ExtractList (list)
	HdgRange list;
{
	HdgRange NodeIndex, item, next;

	NodeIndex = list;
	item = ListHeadField;
	if (item != -1) {
		NodeIndex = item;
		next = NextLink;
		NextLink = -1;
		NodeIndex = list;
		ListHeadField = next;
		ListSizeField = ListSizeField - 1;
		if (next == -1) {
			ListTailField = -1;
		}
	}
	return item;
}


HdgRange
ComplexStmnt (major, typ, op1, op2, op3, LineNo)
	int major, typ, op1, op2, op3, LineNo;
{
	HdgRange NodeIndex;

	switch (major) {

	case ifthenelse:
		TestOvflw (IfNodeSize);
		NodeIndex = ptrg;
		Node1field = ifthenelse << HdgOpSize;
		TypeField = typ;
		DscField = pascal_nil;
		NextLink = -1;
		TailLink = ptrg;
		IfCondField = op1;
		IfTrueField = op2;
		IfFalseField = op3;
		IfLineNo = LineNo;
		ptrg = NodeIndex + IfNodeSize;
	break;

	case loop:
		TestOvflw (WhileNodeSize);
		NodeIndex = ptrg;
		Node1field = loop << HdgOpSize;
		TypeField = typ;
		DscField = pascal_nil;
		NextLink = -1;
		TailLink = ptrg;
		WhileTestField = op1;
		WhileBodyField = op2;
		WhileLabelField = -1;
		WhileLineNo = LineNo;
		ptrg = NodeIndex + WhileNodeSize;
	break;

	case RepeatUntil:
		TestOvflw (RepeatNodeSize);
		NodeIndex = ptrg;
		Node1field = RepeatUntil << HdgOpSize;
		TypeField = typ;
		DscField = pascal_nil;
		NextLink = -1;
		TailLink = ptrg;
		RepeatTest = op1;
		RepeatBody = op2;
		RepeatLabel = -1;
		RepeatLineNo = LineNo;
		ptrg = NodeIndex + RepeatNodeSize;
	break;

	case Switch:
		TestOvflw (SwitchNodeSize);
		NodeIndex = ptrg;
		Node1field = Switch << HdgOpSize;
		TypeField = typ;
		DscField = pascal_nil;
		NextLink = -1;
		TailLink = ptrg;
		SwitchTest = op1;
		SwitchBody = op2;
		SwitchFirst = op3;
		RepeatLineNo = LineNo;
		ptrg = NodeIndex + SwitchNodeSize;
	break;

	}
	return NodeIndex;
}


HdgRange
CaseStmnt (major, minor, typ, low, high, LineNo)
	int major, minor, typ, low, high, LineNo;
{
	HdgRange NodeIndex;

	TestOvflw (CaseNodeSize);
	NodeIndex = ptrg;
	Node1field = (Case << HdgOpSize) + minor;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	CaseLowVal = low;
	CaseHighVal = high;
	CaseLeftLink = -1;
	CaseRightLink = -1;
	CaseLineNo = LineNo;
	CaseBalance = 0;
	ptrg = NodeIndex + CaseNodeSize;
	return NodeIndex;
}


HdgRange
special (op, typ, p1, LineNo)
	int op, typ, p1, LineNo;
{
	HdgRange NodeIndex;

	TestOvflw (SpecNodeSize);
	NodeIndex = ptrg;
	Node1field = (spec << HdgOpSize) + op;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	Spec1parameter = p1;
	SpecLineNo = LineNo;
	ptrg = NodeIndex + SpecNodeSize;
#ifndef PYRAMID_DOUBTFUL
	if ( op == GotoOp && STab[p1].SKind == VarDcl )
		control['E'] = 0;
#endif
	return NodeIndex;
}


HdgRange
ShortStatement (op, typ, op1, LineNo)
	int op, typ, op1, LineNo;
{
	HdgRange NodeIndex;

	TestOvflw (SsNodeSize);
	NodeIndex = ptrg;
	Node1field = (ShortStmnt << HdgOpSize) + op;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	Ss1operand = op1;
	SsLineNo = LineNo;
	ptrg = NodeIndex + SsNodeSize;
	return NodeIndex;
}


HdgRange
LongStatement (op, typ, op1, op2, p1, p2, LineNo)
	int op, typ, op1, op2, p1, p2, LineNo;
{
	HdgRange NodeIndex;

	TestOvflw (LsNodeSize);
	NodeIndex = ptrg;
	Node1field = (LongStmnt << HdgOpSize) + op;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	Ls1operand = op1;
	Ls2operand = op2;
	Ls1parameter = p1;
	Ls2parameter = p2;
	LsLineNo = LineNo;
	ptrg = NodeIndex + LsNodeSize;
	return NodeIndex;
}


HdgRange
ProcNode (major, typ, op1, op2, Label, LineNo)
	int major, typ, op1, op2, Label, LineNo;
{
	HdgRange NodeIndex;

	TestOvflw (ProcNodeSize);
	NodeIndex = ptrg;
	Node1field = Proc << HdgOpSize;
	TypeField = typ;
	DscField = pascal_nil;
	NextLink = -1;
	TailLink = ptrg;
	ProcBody = op1;
	ProcProlog = op2;
	ProcLabel = Label;
	ProcLineNo = LineNo;
	ptrg = NodeIndex + ProcNodeSize;
	return NodeIndex;
}


HdgRange
LinkChain (l1, l2)
	int l1, l2;
{
	HdgRange NodeIndex, NewTail;

	if (l1 == -1) {
		return l2;
	}
	else if (l2 == -1) {
		return l1;
	}
	else {
		NodeIndex = l2;
		NewTail = TailLink;
		NodeIndex = l1;
		NodeIndex = TailLink;
		NextLink = l2;
		NodeIndex = l1;
		TailLink = NewTail;
		return l1;
	}
}
