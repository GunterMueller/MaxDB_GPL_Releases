/*!*********************************************************************

    @file         Proc_FactoryDirectory.hpp

    @author       DanielD

    @brief

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
***********************************************************************/

#ifndef PROC_FACTORYDIRECTORY_HPP
#define PROC_FACTORYDIRECTORY_HPP

#include "SQLManager/DBProc/DBProc_DBProcDescriptor.hpp"

class Proc_IFactory;
class SAPDBTrace_Stream;
class SAPDBTrace_Topic;
class Proc_ControllerVector;

/*!

  @class Proc_FactoryDirectory

 */
class Proc_FactoryDirectory
{
public:
    static Proc_IFactory * findFactory (DBProc_DBProcDescriptor::Language   langEnum,
                                        const char * langString);
    static bool addFactory (DBProc_DBProcDescriptor::Language   langEnum,
                            const char     * langString,
                            Proc_IFactory  * newFactory);
    static void clear ();
    static void dump (SAPDBTrace_Topic & topic, int level);
    static void dump (int level);
    static void collectAllServerControls (Proc_ControllerVector & allControls);

private:
    Proc_FactoryDirectory () {};
    ~Proc_FactoryDirectory () {};
};


#endif
