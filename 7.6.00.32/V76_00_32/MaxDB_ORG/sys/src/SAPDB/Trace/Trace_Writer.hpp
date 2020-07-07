/*!**************************************************************************

  module      : Trace_Writer.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
  description : Implementation of Kernel VTrace

  last changed: 2000-11-27  18:15
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


#ifndef TRACE__WRITER_HPP
#define TRACE__WRITER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"      // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"      // PASCAL: GG_kernel_constants_and_types

#include "DataAccess/Data_PageFrame.hpp"
#include "IOManager/IOMan_IPage.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

class Kernel_Dump;
class RTETask_ITask;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Trace_Writer
{
public:

    static bool CreateInstance( const RTETask_ITask &task );

    static Trace_Writer& GetInstance()
    {
        return( *m_Instance );
    };

    void UserForcedDump( 
        const RTETask_ITask     &task,
        const tsp00_VFilename   &FileName,
        tgg00_DumpStateSet      &DumpStateSet );

    void Run();

    /*!-------------------------------------------------------------------------
    function:     Suspend
    description:  This method is used to suspend the trace writer task. 
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void Suspend();

    /*!-------------------------------------------------------------------------
    function:     Resume
    description:  This method is used to resume the trace writer task. 
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/

    void Resume();

private:

    Trace_Writer( const RTETask_ITask   &task );

    void Dump( 
        const RTETask_ITask   &task,
        Kernel_Dump           &Dump,
        tgg00_DumpStateSet    &DumpStateSet,
        tsp00_Bool            bEnterRegions );

    void SystemForcedDump();

private:

    /*-------------------------------------------------------------------------
    declaration: m_Instance
    description: Instance pointer
    --------------------------------------------------------------------------*/

    static  Trace_Writer    *m_Instance;
    
    /*-------------------------------------------------------------------------
    declaration: m_Task
    description: own task
    --------------------------------------------------------------------------*/
    
    const RTETask_ITask    &m_Task;

    /*-------------------------------------------------------------------------
    declaration: m_Frame
    description: Page buffer to build up dump entries and write them into
                 the kernel dump file
    --------------------------------------------------------------------------*/
    
    Data_PageFrame m_Frame;
};

typedef Trace_Writer* Trace_WriterPtr;

#endif  /* TRACE__WRITER_HPP */
