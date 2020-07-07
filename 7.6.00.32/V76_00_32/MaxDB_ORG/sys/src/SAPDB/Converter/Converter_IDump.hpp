/*!
  @file           Converter_IDump.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          abstract dump interface class to the converter
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/



#ifndef CONVERTER_IDUMP_HPP
#define CONVERTER_IDUMP_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

class RTETask_ITask;
class Kernel_Dump;

 /*===========================================================================*
 *  DEFINES                                                                  *
 **/


/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

/*!
  @interface      Converter_IDump
  @brief          abstract dump interface class to the converter
 */
class Converter_IDump
{
public:

    /*!
    @brief        This method returns the access to the Converter_IDump interface
                  of component Converter. It is assumed that the Converter singleton
                  is available! No CreateInstance call will be done.
    @return       Access to the Converter_IDump interface
    */
    static Converter_IDump& Instance();

    /*!
       @brief          This method is used to dump the main memory structures of the
                       component converter.
       @param          task         [in] own task
       @param          Dump     [in|out] kernel dump file
       @param          bEnterRegion [in] use the converter regions
       @return         none
     */
    virtual void Dump(
        const RTETask_ITask &task,
        Kernel_Dump         &Dump,
        const bool          bEnterRegion ) = 0;
};

#endif  /* CONVERTER_IDUMP_HPP */
