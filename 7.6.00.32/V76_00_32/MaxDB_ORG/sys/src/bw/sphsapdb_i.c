/* this file contains the actual definitions of 

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
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Mar 17 16:24:13 2000
 */
/* Compiler settings for sphsapdb.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_SPHSAPDBLIB = {0x6E822A6D,0x91E6,0x11D3,{0xA4,0x69,0x00,0x08,0xC7,0x1E,0x43,0x05}};


const IID IID_Isphsapdb = {0x39C0C052,0x91E8,0x11d3,{0xA4,0x69,0x00,0x08,0xC7,0x1E,0x43,0x05}};


const CLSID CLSID_sphsapdb = {0x6E822A6E,0x91E6,0x11D3,{0xA4,0x69,0x00,0x08,0xC7,0x1E,0x43,0x05}};


#ifdef __cplusplus
}
#endif

