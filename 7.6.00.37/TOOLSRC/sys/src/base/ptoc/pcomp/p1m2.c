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
#include <stdio.h>

typedef
  unsigned char STRING[256];
int sql__strlen();
int ErrorCount;
int StabI();
int EnterString();
int GetString();
unsigned char *_GetStringPtr();
int _GetStringLength();
int _GetStringNo();
unsigned char _GetStringChar();
unsigned char _GetFirstChar();
int CurrentFile;
int OpenInput();
int initialize();
int getline();
int sql__exit();
int P1m2I();
int ErrorMsg();
int DclError();
int werror();
int FatalErrorMsg();
int WarnStrCmp();
unsigned char LongErrorMsg[50][51];
unsigned char ShortErrorMsg[41][21];
int Column;
unsigned char errpfx;

struct _ptoc_FRAME_P1m2I
  {
  char dummy;
  } ;


int P1m2I()
    {
    struct _ptoc_FRAME_P1m2I _ptoc_LOCAL;
    int i;
    int j;
    
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[0], "Invalid constant expression\0                       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[1], "Conflict with ENTRY declare\0                       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[2], "Duplicate declaration\0                             ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[3], "Not a procedure or function\0                       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[4], "Require scalar for case/select\0                    ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[5], "Too many select expressions\0                       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[6], "Illegal INITIAL procedure\0                         ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[7], "Interpreter underflow\0                             ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[8], "Interpreter overflow\0                              ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[9], "String constant too long\0                          ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[10], "String table space exceeded\0                       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[11], "Bad pointer in node\0                               ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[12], "Proc body too large\0                               ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[13], "Bad 1st parm-link chain\0                           ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[14], "Bad 2nd parm-link chain\0                           ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[15], "Identifier list overflow\0                          ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[16], "Expression list overflow\0                          ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[17], "Symbol table overflow\0                             ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[18], "No more temps\0                                     ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[19], "Must abort\0                                        ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[20], "Bad pascal2\0                                       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[21], "Input line too long\0                               ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[22], "External procedures or functions must be level 1\0  ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[23], "\'external\' or \'forward\' expected\0                  ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[24], "Unable to open source file\0                        ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[25], "Include syntax error - expected \' or \"\0            ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[26], "Include nesting limit exceeded\0                    ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[27], "Unable to open include file\0                       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[28], "Unrecognized directive\0                            ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[29], "Failed to assign function result\0                  ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[30], "Backwards range in case label\0                     ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[31], "Duplicate case labels\0                             ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[32], "Unable to open object file\0                        ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[33], "Packed array of char expected\0                     ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[34], "Local variable expected\0                           ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[35], "Case label out of range\0                           ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[36], "Illegal assignment: file component in record\0      ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[37], "File in type or variable not allowed\0              ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[38], "Goto-statement outside this block\0                 ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[39], "Label defined in a structured statement\0           ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[40], "Don\'t use identifier \"main\" here\0                  ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[41], "Can\'t compare records with variant parts\0          ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[42], "Assignment to control variable not allowed\0        ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[43], "PAC lower bound must be one when used as a string\0 ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[44], "Character array is unpacked or lower bound <> 1\0   ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[45], "Control variable could be changed\0                 ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[46], "Expression value out of range\0                     ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[47], "Cannot take succ of last element of a range\0       ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[48], "Cannot take pred of first element of a range\0      ")
    _ptoc_ASSIGN(unsigned char , 51, LongErrorMsg[49], "Structured type for return value not allowed\0      ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[0], "Type conflict\0       ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[1], "Illegal type\0        ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[2], "Not a type\0          ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[3], "Not declared\0        ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[4], "Expression type\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[5], "Too many constants\0  ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[6], "Illegal scope level\0 ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[7], "Procedure nesting\0   ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[8], "Parameter count\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[9], "Inv actual parameter\0")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[10], "Array dimension\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[11], "Index type conflict\0 ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[12], "Include error\0       ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[13], "Illegal number\0      ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[14], "Illegal string\0      ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[15], "Illegal character\0   ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[16], "Too many exports\0    ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[17], "Short error 17\0      ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[18], "Not a variable\0      ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[19], "Not a record\0        ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[20], "Illegal record field\0")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[21], "Not an array\0        ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[22], "Array elem not a rec\0")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[23], "Mul array indices\0   ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[24], "Subscript range\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[25], "Proc name long\0      ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[26], "Not a procedure\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[27], "Not implemented\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[28], "Syntax error\0        ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[29], "Not a pointer\0       ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[30], "Invalid I/O item\0    ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[31], "Text file expected\0  ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[32], "File expected\0       ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[33], "String expected\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[34], "Not a function\0      ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[35], "Illegal label\0       ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[36], "Duplicate label\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[37], "Illegal low bound\0   ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[38], "String too long\0     ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[39], "Illegal assignment\0  ")
    _ptoc_ASSIGN(unsigned char , 21, ShortErrorMsg[40], "xxxxxxxx\0            ")
    }
int error();

struct _ptoc_FRAME_ErrorMsg
  {
  char dummy;
  } ;


int ErrorMsg(ErrorNumber, ColumnNumber)
int ErrorNumber;
int ColumnNumber;
    {
    struct _ptoc_FRAME_ErrorMsg _ptoc_LOCAL;
    STRING s;
    
    ErrorCount += 1;
    errpfx = (unsigned char)'E';
    _ptoc_ASSIGN(unsigned char , 256, s, "%s                                                                                                                                                                                                                                                              ")
    s[2] = (unsigned char)'\0';
    Column = ColumnNumber;
    if (ErrorNumber < 100)
        {
        error(&s[0] , &ShortErrorMsg[ErrorNumber][0] , 0 , 0 , 0);
        }
    else
        {
        error(&s[0] , &LongErrorMsg[ErrorNumber + -100][0] , 0 , 0 , 0);
        }
    }

struct _ptoc_FRAME_DclError
  {
  char dummy;
  } ;


int DclError(Name, Card, File, ErrorNumber)
int Name;
int Card;
int File;
int ErrorNumber;
    {
    struct _ptoc_FRAME_DclError _ptoc_LOCAL;
    int i;
    STRING s;
    _iorechd *ptoc_Var0;
    int ptoc_Var1;
    
    ErrorCount += 1;
    GetString(Name , &s[0]);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "");
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    ptoc_Var1 = _GetStringLength(Name);
    fprintf(ptoc_Var0->_fbuf , "%*d%c%*.*s" , 1 , Card , 9 , ptoc_Var1 , ptoc_Var1 < 256 ? ptoc_Var1 : 256 , s);
    sql__writeln(ptoc_Var0);
    ptoc_Var0 = sql__ut(output);
    fprintf(ptoc_Var0->_fbuf , "%*d%s" , 1 , Card , "\t^");
    sql__writeln(ptoc_Var0);
    GetString(File , &s[0]);
    ptoc_Var0 = sql__ut(output);
    ptoc_Var1 = _GetStringLength(File);
    fprintf(ptoc_Var0->_fbuf , "%*.*s%s%*d%s" , ptoc_Var1 , ptoc_Var1 < 256 ? ptoc_Var1 : 256 , s , " : " , 1 , Card , " E: ");
    if (ErrorNumber < 100)
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%*.*s" , 21 , 21 , ShortErrorMsg[ErrorNumber]);
        sql__writeln(ptoc_Var0);
        }
    else
        {
        ptoc_Var0 = sql__ut(output);
        fprintf(ptoc_Var0->_fbuf , "%*.*s" , 51 , 51 , LongErrorMsg[ErrorNumber + -100]);
        sql__writeln(ptoc_Var0);
        }
    sql__fflush(output);
    }

struct _ptoc_FRAME_werror
  {
  char dummy;
  } ;


int werror(ptoc_ptr_message)
STRING ptoc_ptr_message;
    {
    struct _ptoc_FRAME_werror _ptoc_LOCAL;
    int len;
    _iorechd *ptoc_Var2;
    int ptoc_Var3;
    STRING message;
    
    _ptoc_MOVE(char, sizeof(STRING ), message, ptoc_ptr_message);
    len = 0;
    while ((int ) message[len] != 0)
        len += 1;
    ptoc_Var2 = sql__ut(output);
    fprintf(ptoc_Var2->_fbuf , "");
    sql__writeln(ptoc_Var2);
    ptoc_Var2 = sql__ut(output);
    ptoc_Var3 = len;
    fprintf(ptoc_Var2->_fbuf , "%s%*.*s" , "warning: " , ptoc_Var3 , ptoc_Var3 < 256 ? ptoc_Var3 : 256 , message);
    sql__writeln(ptoc_Var2);
    sql__fflush(output);
    }

struct _ptoc_FRAME_FatalErrorMsg
  {
  char dummy;
  } ;


int FatalErrorMsg(ErrorNumber, ColumnNumber)
int ErrorNumber;
int ColumnNumber;
    {
    struct _ptoc_FRAME_FatalErrorMsg _ptoc_LOCAL;
    
    ErrorMsg(ErrorNumber , ColumnNumber);
    sql__exit(1);
    }

struct _ptoc_FRAME_WarnStrCmp
  {
  char dummy;
  } ;


int WarnStrCmp(lno)
int lno;
    {
    struct _ptoc_FRAME_WarnStrCmp _ptoc_LOCAL;
    STRING s;
    _iorechd *ptoc_Var4;
    int ptoc_Var5;
    
    GetString(CurrentFile , &s[0]);
    ptoc_Var4 = sql__ut(output);
    ptoc_Var5 = _GetStringLength(CurrentFile);
    fprintf(ptoc_Var4->_fbuf , "%*.*s%s%*d%s" , ptoc_Var5 , ptoc_Var5 < 256 ? ptoc_Var5 : 256 , s , ": " , 1 , lno , ": warning: comparison of strings with different lengths");
    sql__writeln(ptoc_Var4);
    }
