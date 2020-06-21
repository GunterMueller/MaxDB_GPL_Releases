/*!
  @file           hos37.h
  @author         FerdinandF
  @brief          COM
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



\endif
*/





#ifndef HOS37_H
#define HOS37_H

#define HRESULT_ERROR_OS37          1,  "HRESULT: 0x%X, %s:%.64s"
#define TOO_MANY_INTERFACES_OS37	  2,  "only ONE interface allowed per COMObject"
#define METHOD_NOT_FOUND_OS37       3,  "specified method not found within interface"
#define P1_NO_USERDEFINED_TYPE_OS37 4,  "Method: %s, first parameter MUST be of type VT_USERDEFINED; found type %d"
#define P1_IO_TYPE_INPUT_OS37       5,  "Method: %s, first parameter MUST not be of I/O type INPUT"
#define P1_IO_TYPE_OUTPUT_OS37      6,  "Method: %s, first parameter MUST not be of I/O type OUTPUT"
#define P1_WRONG_GUID_OS37          7,  "Method: %s, first parameter has wrong uid"
#define UNSUPPORTED_VT_CARRAY1_OS37 8,  "Method: %s, VT_CARRAY:VT_USERDEFINED: unsupported parameter type: %d or alias not VT_I1 or VT_UI1"
#define UNSUPPORTED_VT_CARRAY2_OS37 9,  "Method: %s, VT_CARRAY: unsupported element type: %d"
#define UNSUPPORTED_VT_CARRAY3_OS37 10, "Method: %s, unsupported parameter type VT_ARRAY with dimension %d"
#define UNSUPPORTED_VT_TYPE_OS37    11, "Method: %s, unsupported type %d"
#define RECORD_EXSPECTED_OS37       12, "Method: %s, TKIND_ALIAS or TKIND_RECORD expected: found: %d"
#define MISSING_GUID_OS37           13, "Method: %s, missing user type guid"
#define LOADTYPELIB_ERROR_OS37      14, "LoadTypeLib error: 0x%X for TypeLib: %s"
#define BUFFER_TOO_SMALL_OS37       15, "buffer too small"
#define TOO_MANY_OUTPARMS_OS37      16, "Method: %s, more than one out parameter specified"
#define UNSUPPORTED_VT_TYPE2_OS37   17, "Method: %s, Parameter: %d - unsupported VT_TYPE %d"
#define UNSUPPORTED_TLIBSTRUCT_OS37 18, "Method: %s, Parameter: %d - unsupported typelib structure"
#define UNSUPPORTED_VT_CARRAY4_OS37 19, "Method: %s, Parameter: %d - VT_ARRAY with dimension %d not allowed"
#define INVALID_PARAMETER_OS37	    20, "Method: %s, Parameter: %d - invalid"
#define CSTRUCT_BYVALUE_OS37        21, "Method: %s, Parameter: %d - C-Struct not allowed to be passed by value"
#define SCALAR_BYVALUE_OS37         22, "Method: %s, Parameter: %d - Only scalar datatypes may be passed by value. VT_TYPE found: %d"
#define WRONG_PARAMETERS_OS37       23, "Call: GenInspectionCode <TLB-Name> <Output-Filename> <Type-of-Object>(UDT|DBPROC)"
#define TERMINATE_MSG_OS37          24, "Error generating Inspection-Code; HRESULT: %X"
#define IN_SCALAR_BYPTR_OS37        25, "Method: %s, Parameter: %d - scalar datatype passed by ptr but parameter is input only. VT_TYPE found: %d"

#endif  HOS37_H


