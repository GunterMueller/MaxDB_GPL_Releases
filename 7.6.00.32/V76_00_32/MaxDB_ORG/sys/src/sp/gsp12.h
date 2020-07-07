/*!================================================================
 module:        gsp12.h

 -------------------------------------------------------------------

 responsible:   DanielD

 special area:  Abstract Interfaces

 description:   Stream Interfaces

 -------------------------------------------------------------------

 Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




 ===================================================================*/

#ifndef GSP12_H
#define GSP12_H

/*!
  Class: isp12_OutputStream
 */
class isp12_OutputStream
{
public:
    virtual ~isp12_OutputStream () {};
    virtual int write (const void * buf, int len) = 0;
    virtual int writeln (const void * buf, int len = -1) = 0;
    virtual int flush () {return 0;};
    virtual int close () = 0;
};

/*!
  Class: isp12_InputStream
 */
class isp12_InputStream
{
public:
    virtual ~isp12_InputStream () {};
    virtual int read (void * buf, int requestedLen) = 0;
};


#endif

