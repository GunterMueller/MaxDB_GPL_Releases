/*!*********************************************************************

    @file         RTEUDE_Win32ServiceControl.hpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#ifndef RTEUDE_WIN32SERVICECONTROL_HPP
#define RTEUDE_WIN32SERVICECONTROL_HPP


/*!

  @class RTEUDE_Win32ServiceControl

 */
class RTEUDE_Win32ServiceControl
{
public:
    /*!
        @brief start the UDE Relay

        If serviceName is NULL or empty, the default serivce name is used.
        Otherwise, serviceName is appended to the default service name.
        This allows that multiple UDE relays with different permissions
        are running.
     */
    static bool start (const char * serviceName);

    /*!
        @brief stop the UDE Relay

        If serviceName is NULL or empty, the default serivce name is used.
        Otherwise, serviceName is appended to the default service name.
     */
    static bool stop (const char * serviceName);


    static const char * fullServiceName (const char * serviceName, char * buf);
    static const char * fullServiceDisplay (const char * serviceName, char * buf);

private:
    RTEUDE_Win32ServiceControl ();
    ~RTEUDE_Win32ServiceControl ();
};

#endif
