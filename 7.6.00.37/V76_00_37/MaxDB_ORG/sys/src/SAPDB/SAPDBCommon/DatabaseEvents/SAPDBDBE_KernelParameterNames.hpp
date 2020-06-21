/*!
  @file     SAPDBDBE_KernelParameterNames.hpp
  @author   MarcW
  @ingroup  DatabaseEvents
  @brief    Mapping for configuration parameter names used for database events (parameter class EVENTS).

            The parameter names are defined here, because also the DBM Server needs these names.
  @see            

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


#ifndef SAPDBDBE_KernelParameterNames_hpp
#define SAPDBDBE_KernelParameterNames_hpp

/*! @brief event category DBFILLINGABOVELIMIT */
#define SAPDBDBE_KERNELPARAMETERNAME_DBFILLINGABOVELIMIT (RTEConf_Parameter::Name) \
                "DBFILLINGABOVELIMIT"

/*! @brief event category DBFILLINGBELOWLIMIT */
#define SAPDBDBE_KERNELPARAMETERNAME_DBFILLINGBELOWLIMIT (RTEConf_Parameter::Name) \
                "DBFILLINGBELOWLIMIT"

/*! @brief event category LOGABOVELIMIT */
#define SAPDBDBE_KERNELPARAMETERNAME_LOGABOVELIMIT (RTEConf_Parameter::Name) \
                "LOGABOVELIMIT"

#endif //SAPDBDBE_KernelParameterNames_hpp