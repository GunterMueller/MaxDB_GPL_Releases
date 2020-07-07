/*


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




*/

#include "bw/sphsapdbIspc.h"

const t_FuncBufDesc CO_FunctionDesc[] =
{
  3, (char*)"test", 1, &co_IntrospectFunction1,
  4, (char*)"sphn", 8, &co_IntrospectFunction2,
  5, (char*)"sphl", 8, &co_IntrospectFunction3};

extern "C" void __stdcall Co_IntrospectObject (  LPCLSID      *lpClsId,
                                       LPIID          *lpIId,
                                       unsigned short *lpNum,
                                       long           *lpVersion )
{
  *lpClsId        = (LPCLSID)&CLSID_Intro;
  *lpIId          = (LPIID)&IID_Intro;
  *lpNum          = PROCEDURE_NUM;
  *lpVersion      = VERSION_NUMBER;
};

  enum Co_AccessType
{
    get_by_name,
    get_by_index
};

extern "C" void __stdcall Co_IntrospectMethod ( Co_AccessType   accessType,
                                      char            **lplpMethod,
                                      unsigned short  Num,
                                      unsigned short  *lpIdx,
                                      unsigned short  *lpParamCount,
                                      CO_INTROSPECTFUNCTION *lplpIntrospecFunc )
{
  *lplpIntrospecFunc = NULL;
  if (accessType == get_by_name)
  {
	  if ( **lplpMethod == '\0' )
    {
        *lpIdx             = CO_FunctionDesc[0].FuncIdx;
        *lpParamCount      = CO_FunctionDesc[0].ParamCount;
        *lplpIntrospecFunc = CO_FunctionDesc[0].lpFunction;
        *lplpMethod        = CO_FunctionDesc[0].lpMethod;
    }
    else
    for ( int i=0; i < PROCEDURE_NUM; i++ )
      if ( 0 == strcmp(*lplpMethod, CO_FunctionDesc[i].lpMethod ))
      {
        *lpIdx             = CO_FunctionDesc[i].FuncIdx;
        *lpParamCount      = CO_FunctionDesc[i].ParamCount;
        *lplpIntrospecFunc = CO_FunctionDesc[i].lpFunction;
      }
  }
  else
  {
    if (( Num <= PROCEDURE_NUM )) // && ( Num >= 0 ))
      {
        *lpIdx             = CO_FunctionDesc[Num].FuncIdx;
        *lpParamCount      = CO_FunctionDesc[Num].ParamCount;
        *lplpIntrospecFunc = CO_FunctionDesc[Num].lpFunction;
        *lplpMethod        = CO_FunctionDesc[Num].lpMethod;
      }
  }
};

