/*!*********************************************************************

    @file         RTEIO_DirectoryIterator.hpp

    @author       DanielD

    @brief        Directory iterator

    This class allows to iterate through a directory and 

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

#ifndef RTEIO_DIRECTORYITERATOR_HPP
#define RTEIO_DIRECTORYITERATOR_HPP


/*!

  @class RTEIO_DirectoryIterator

 */
class RTEIO_DirectoryIterator
{
public:
    enum {withDots};
    /*!
      @brief ctor
 
      Setup iteration of directory given. Option allows to include files
      beginning with dots (default excluded from iteration)

      @param path [in] path to directory to iterate through
      @param options [in] 0 or RTEIO_DirectoryIterator::withDots
     */
    RTEIO_DirectoryIterator (const char * path, int options = 0);
    /*!
      @brief dtor
     */
    ~RTEIO_DirectoryIterator ();
    /*!
      @brief iterate to next entry
      @return false if no more entries
     */
    bool next ();
    /*!
      @brief get current full path
      @return full path name of current entry
     */
    const char * currentFullpath () const;
    /*!
      @brief get current file name
     */
    const char * currentName () const;
    /*!
      @brief check if current entry is a directory
     */
    bool         currentIsDir ();
    /*!
      @brief check if iterator is valid
      @return true if current entry is valid
     */
    bool         isValid () const;
    /*!
      @brief close iteration
      @return none
     */
    void         close ();

private:
    bool basicNext ();
    bool currentOK () const;

private:
    char         fullbuf [1024];
    char       * namestart;
    int          isdir;
    int          options;
    int          state;
    void       * dirHandle;
};

#endif
