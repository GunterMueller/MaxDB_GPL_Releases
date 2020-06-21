/*


    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

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
#include "idl/idl2xml/pen00.h" 
#include "idl/idl2xml/rte.h"
#include "idl/idl2xml/interfacetree.h"

	/*
	InterfaceRoot =                                    InterfaceNewElement("LIBRARY");
	InterfaceRoot->Child =                               InterfaceNewElement("INTERFACE");
	InterfaceRoot->Child->Child =                          InterfaceNewElement("IDLS");
	InterfaceRoot->Child->Next =                         InterfaceNewElement("COCLASS");
	InterfaceRoot->Child->Next->Next =                   InterfaceNewElement("CONSTS");
	InterfaceRoot->Child->Next->Next->Next =             InterfaceNewElement("ENUMS");
	InterfaceRoot->Child->Next->Next->Next->Next =       InterfaceNewElement("TYPES");
	InterfaceRoot->Child->Next->Next->Next->Next->Next = InterfaceNewElement("STRUCTS");
	*/


InterfaceElementp InterfaceNewElement
(
  char*  TagName
)
{
  InterfaceElementp iep;

  iep = (void*) RteStdMalloc (sizeof(InterfaceElement));
  
  memset (iep, 0, sizeof(InterfaceElement));

  iep->TagName = InterfaceAlloc(TagName);
  
  return iep;
}

InterfaceAttributep InterfaceNewAttribute
(
)
{
  void* iep;

  iep = RteStdMalloc (sizeof(InterfaceAttribute));
  
  memset (iep, 0, sizeof(InterfaceAttribute));
  
  return (InterfaceAttributep)iep;
}

char* InterfaceAlloc
(
  char*    string
)
{
  void* iep;

  iep = RteStdMalloc (strlen (string) + 1);
  
  strcpy (iep, string);
  
  return (char*)iep;
}
