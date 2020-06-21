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

int sql__strlen();
typedef
  int HdgRange;
int field[1];
int SetMinor();
int node3();
int UnaryExpression();
int BinaryExpression();
int SetBinaryParameters();
int AppendList();
int InsertList();
int LinkLists();
int MakeList();
int ExtractList();
int ComplexStmnt();
int CaseStmnt();
int special();
int ShortStatement();
int LongStatement();
int ProcNode();
int LinkChain();
int shl();
int shr();
typedef
  int OpcodeRange;
int LongStmntOpt;
typedef
  unsigned TransformType;
int identical();
int ConditionsSatisfied();
int OptLongStmnt();

struct _ptoc_FRAME_constant
  {
  char dummy;
  } ;


int constant(NodeIndex, literal)
HdgRange NodeIndex;
int *literal;
    {
    struct _ptoc_FRAME_constant _ptoc_LOCAL;
    int _constant;
    
    if (shr(field[NodeIndex] , 16) == 1 && (field[NodeIndex] & 65535) == 12)
        {
        *literal = field[NodeIndex + 5];
        _constant = 1;
        }
    else
        {
        _constant = 0;
        }
    return(_constant);
    }

struct _ptoc_FRAME_SetExpression
  {
  char dummy;
  } ;


int SetExpression(NodeIndex, SubExp)
HdgRange NodeIndex;
HdgRange SubExp;
    {
    struct _ptoc_FRAME_SetExpression _ptoc_LOCAL;
    
    field[NodeIndex + 5] = SubExp;
    }

struct _ptoc_FRAME_variables
  {
  char dummy;
  } ;


int variables(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_variables _ptoc_LOCAL;
    int _variables;
    
    if (shr(field[NodeIndex] , 16) == 1 && (field[NodeIndex] & 65535) == 94)
        {
        _variables = field[NodeIndex + 5];
        }
    else
        {
        _variables = -1;
        }
    return(_variables);
    }

struct _ptoc_FRAME_SkipConvert
  {
  char dummy;
  } ;


int SkipConvert(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_SkipConvert _ptoc_LOCAL;
    int _SkipConvert;
    
    _SkipConvert = NodeIndex;
    return(_SkipConvert);
    }

struct _ptoc_FRAME_SpecialBinary
  {
  char dummy;
  } ;


int SpecialBinary(lhs, rhs, new_op, new_rhs)
HdgRange lhs;
HdgRange rhs;
OpcodeRange *new_op;
HdgRange *new_rhs;
    {
    struct _ptoc_FRAME_SpecialBinary _ptoc_LOCAL;
    int _SpecialBinary;
    HdgRange NodeIndex;
    OpcodeRange op;
    
    NodeIndex = SkipConvert(rhs);
    if (shr(field[NodeIndex] , 16) == 5)
        {
        op = field[NodeIndex] & 65535;
        switch (op)
            {
            case 19 :
            case 20 :
            case 21 :
            case 22 :
            case 88 :
            case 104 :
            case 29 :
            case 30 :
            case 89 :
            case 91 :
            case 90 :
                NodeIndex = SkipConvert(field[NodeIndex + 5]);
                if ((field[NodeIndex] & 65535) == 4)
                    {
                    if (identical(lhs , field[NodeIndex + 5]))
                        {
                        NodeIndex = SkipConvert(rhs);
                        *new_op = op;
                        *new_rhs = field[NodeIndex + 6];
                        _SpecialBinary = 1;
                        }
                    else
                        {
                        _SpecialBinary = 0;
                        }
                    }
                else
                    {
                    _SpecialBinary = 0;
                    }
                break;
            default :
                _SpecialBinary = 0;
                break;
            }
        }
    else
        {
        _SpecialBinary = 0;
        }
    return(_SpecialBinary);
    }

struct _ptoc_FRAME_OptLongStmnt
  {
  char dummy;
  } ;


int OptLongStmnt(NodeIndex)
HdgRange NodeIndex;
    {
    struct _ptoc_FRAME_OptLongStmnt _ptoc_LOCAL;
    OpcodeRange new_op;
    HdgRange new_rsh;
    
    if ((field[NodeIndex] & 65535) == 6)
        {
        if (SpecialBinary(field[NodeIndex + 5] , field[NodeIndex + 6] , &new_op , &new_rsh))
            {
            SetMinor(NodeIndex , 102);
            field[NodeIndex + 8] = new_op;
            field[NodeIndex + 6] = new_rsh;
            LongStmntOpt += 1;
            }
        }
    }
