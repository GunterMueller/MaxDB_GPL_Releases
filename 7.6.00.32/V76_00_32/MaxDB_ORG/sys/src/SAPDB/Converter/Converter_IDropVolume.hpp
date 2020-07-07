/*!
  @file           Converter_IDropVolume.hpp
  @author         Henrik
  @ingroup        Converter
  @brief          abstract drop volume interface class to the converter
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#ifndef CONVERTER_IDROPVOLUME_HPP
#define CONVERTER_IDROPVOLUME_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "IOManager/IOMan_BlockAddress.hpp"

class RTETask_ITask;

/*!
  @interface      Converter_IDropVolume
  @brief          abstract drop volume interface class to the converter
 */
class Converter_IDropVolume
{
public:

    /*!
       @brief          returns the drop volume interface of the converter
       @return         (Converter_IDropVolume) instance of the converter

       - This static function returns the interface of the converter which
         allows access to all methods defined in this interface class
     */
    static Converter_IDropVolume & Instance();

    /*!
       @brief          prepares the converter for dropping a volume 

       - Waits until a running savepoint has finished
         marks all permanent converter pages as changed to force a rewrite of all
         converter pages during the next savepoint

       @param   task [in] own task
       @return  none
     */
    virtual void PrepareForDropVolume( const RTETask_ITask &task ) = 0;

};


#endif  /* CONVERTER_IDROPVOLUME_HPP */
