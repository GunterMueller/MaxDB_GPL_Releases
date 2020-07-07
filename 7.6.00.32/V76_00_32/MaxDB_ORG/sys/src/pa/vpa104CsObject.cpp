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
// CsObject.cpp: implementation of the CsObject class.
//
//////////////////////////////////////////////////////////////////////

#include "hpa101saptype.h"
#include "hpa106cslzc.h"
#include "hpa107cslzh.h"

#include "hpa104CsObject.h"
#include "hpa105CsObjInt.h"

// #include "cslzc.h"
// #include "cslzh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CsObject::CsObject()
{
	Cs = new CsObjectInt ();
}

CsObject::~CsObject()
{
	delete Cs;
}


int CsObject::CsCompr (SAP_INT    sumlen,
             SAP_BYTE * inbuf,
             SAP_INT    inlen,
             SAP_BYTE * outbuf,
             SAP_INT    outlen,
             SAP_INT    option,
             SAP_INT  * bytes_read,
             SAP_INT  * bytes_written)
{
	return Cs->CsCompr (sumlen,inbuf,inlen,outbuf,outlen,option,bytes_read,bytes_written);
}


int CsObject::CsDecompr (SAP_BYTE * inbuf,     /* ptr input .......*/
               SAP_INT    inlen,         /* len of input ....*/
               SAP_BYTE * outbuf,        /* ptr output ......*/
               SAP_INT    outlen,        /* len output ......*/
               SAP_INT    option,        /* decompr. option  */
               SAP_INT *  bytes_read,    /* bytes read ......*/
               SAP_INT *  bytes_decompressed) /* bytes decompr.  */
{
	return Cs->CsDecompr (inbuf,inlen,outbuf,outlen,option,bytes_read,bytes_decompressed);
}

int CsObject::CsGetAlgorithm (SAP_BYTE * data)
{
	return Cs->CsGetAlgorithm (data);
}

SAP_INT CsObject::CsGetLen (SAP_BYTE * data)
{
	return Cs->CsGetLen (data);
}

int CsObject::CsGetVersion (SAP_BYTE * data)
{
	return Cs->CsGetVersion (data);
}

int CsObject::CsSetHead (SAP_BYTE * outbuf,
   SAP_INT    len,
   SAP_BYTE   veralg,
   SAP_BYTE   special)
{
	return Cs->CsSetHead (outbuf,len,veralg,special);
}

int CsObject::CsInitCompr (SAP_BYTE * outbuf, SAP_INT sumlen, SAP_INT option)
{
	return Cs->CsInitCompr (outbuf,sumlen,option);
}

int CsObject::CsInitDecompr (SAP_BYTE * inbuf)
{
	return Cs->CsInitDecompr (inbuf);
}

