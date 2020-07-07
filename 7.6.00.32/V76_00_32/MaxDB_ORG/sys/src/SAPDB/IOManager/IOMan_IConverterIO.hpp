/*!
  @file     IOMan_IConverterIO.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for read and write access of converter pages on the data volumes

\if EMIT_LICENCE
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

\endif
*/


#ifndef IOMAN_ICONVERTERIO_HPP
#define IOMAN_ICONVERTERIO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


class Converter_Page;
class IOMan_BlockAddress;


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
   @interface   IOMan_IConverterIO
   @brief       This interface is used for read and write access of converter
                pages on the data devspaces.
 */
/* -----------------------------------------------------------------------*/

class IOMan_IConverterIO
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method returns the access to the IOMan_IConverterIO interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the IOMan_IConverterIO interface
     */
    /* -----------------------------------------------------------------------*/

    static IOMan_IConverterIO& GetInstance();

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to read one converter page synchronous from 
                a data devspace into the given memory. The caller is blocked as 
                long as the read io is not completed. The position of the page 
                is given. The correctness of the read data page will be checked. 
                In case of errors detected by the check method of the 
                page the read io will be repeated. If the error is still present 
                after three read operations, the provided buffer is dumped into 
                the rundirectory of the kernel. Each failed read operation is 
                logged within the knldiag and knldiag error file. In any case of 
                an error an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   block [in] Position of the wanted page at the data devspace
       @param   page [in] Handler for the wanted page.
       @return  none.
     */
    /* -----------------------------------------------------------------------*/

    virtual void ReadConverterPage(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        Converter_Page              &page ) = 0;

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   This method is used to write one converter page on one data 
                devspace. The caller is blocked as long as the write io is not 
                completed. The destination of the converter page will be requested
                from the componenet FBM and the old block address is released 
                similar to the block addresses of data pages. Before the proper 
                write is executed the correctness of the given pages will be 
                checked. In case of an error the corrupted page is dumped into 
                the kernel rundirectory and an emergency shutdown is executed.
                In the other case the converter page is prepared for the io, i.e. 
                the the checksum is calculated.
       @param   taskId [in] Identification of the calling task.
       @param   page [in] Handler of the given page
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    virtual void WriteConverterPage(
        const tsp00_TaskId  taskId,
        Converter_Page      &page ) = 0;
};

#endif  /* IOMAN_ICONVERTERIO_HPP */
