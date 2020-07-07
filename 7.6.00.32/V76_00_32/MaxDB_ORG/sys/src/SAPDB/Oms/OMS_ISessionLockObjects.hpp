/*!***************************************************************************

module      : OMS_ISessionLockObjects.hpp

-------------------------------------------------------------------------

responsible : ThomasA

special area: OMS

description : defines the Interface for the lock objects of a session

last changed: 2000-07-26  18:24

see also    : 

-------------------------------------------------------------------------

copyright:    Copyright (c) 2000-2005 SAP AG



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


*****************************************************************************/

#ifndef __OMS_ISESSIONLOCKOBJECTS_HPP
#define __OMS_ISESSIONLOCKOBJECTS_HPP

class OMS_Session;

class OMS_ISessionLockObjects
{
public :
  static OMS_ISessionLockObjects* Create(OMS_Session&);
  virtual void TransactionEnd() = 0;
  virtual void Destroy() = 0;
};

#endif  // __OMS_ISESSIONLOCKOBJECTS_HPP
