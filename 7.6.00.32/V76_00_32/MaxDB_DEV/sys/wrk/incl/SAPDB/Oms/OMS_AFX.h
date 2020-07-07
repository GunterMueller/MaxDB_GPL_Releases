/*


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



*/
#ifndef __OMS_AFX_H
#define __OMS_AFX_H

#ifdef WIN32

#undef  AFX_EXT_CLASS
#define DECLSPEC_EXPORT         __declspec(dllexport)
#define DECLSPEC_IMPORT         __declspec(dllimport)
#ifdef OMS_IMPLEMENTATION
#define AFX_EXT_CLASS           __declspec(dllexport)
#else
#define AFX_EXT_CLASS           __declspec(dllimport)
#endif

#else   // WIN32

#define AFX_EXT_CLASS
#define DECLSPEC_IMPORT
#define DECLSPEC_EXPORT

#endif  // WIN32

#endif  // __OMS_AFX_H
