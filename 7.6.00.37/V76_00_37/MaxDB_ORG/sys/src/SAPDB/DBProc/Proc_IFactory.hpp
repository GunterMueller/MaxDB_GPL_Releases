/*!
  @file           Proc_IFactory.hpp
  @author         DanielD
  @brief

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


#ifndef PROC_IFACTORY_HPP
#define PROC_IFACTORY_HPP

class Proc_ISession;
class SAPDBErr_MessageList;
class SAPDBTrace_Stream;
class Proc_ControllerVector;

/*!
  @interface      Proc_IFactory

 */

class Proc_IFactory
{
public:
    virtual ~Proc_IFactory () {};
    virtual Proc_ISession * createSession (SAPDBErr_MessageList * messageList) = 0;
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const = 0;
    virtual void collectAllServerControls (Proc_ControllerVector & allControls) const = 0;
};


#endif
