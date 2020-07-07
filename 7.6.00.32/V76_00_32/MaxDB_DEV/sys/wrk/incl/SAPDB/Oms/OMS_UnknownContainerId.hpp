/*


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



*/
#ifndef __OMS_UNKNOWNCONTAINERID_HPP
#define __OMS_UNKNOWNCONTAINERID_HPP

#include "ggg00.h"
#include "ggg01.h"
#include "Oms/OMS_ContainerHandle.hpp"      // FF 2003-09-23 new FileDir

class OMS_UnknownContainerId 
{
public :
  OMS_UnknownContainerId() {
    m_fileId.fileObjFileType_gg00().becomes(oftUnknownType_egg00);
  }
  
  const OMS_ContainerHandle& GetContainerHandle() { //FF 2003-09-23 new FileDir
    return *reinterpret_cast<OMS_ContainerHandle*>(&m_fileId.fileDirFileId_gg00()); // this is possible due to correct alignment, see below
  }
  
  bool IsVarObjFile() {
    return (oftVarLenObjFile_egg00 == m_fileId.fileObjFileType_gg00());
  }
  
  operator tgg00_FileId&()
  {
    return m_fileId;
  }
  
  tgg00_FileId* GetFileIdPointer()
  {
    return &m_fileId;
  }
  
private:
  union {
    tgg01_ContainerId m_fileId;
    void*             m_align;
  };  
};

#endif  // __OMS_UNKNOWNCONTAINERID_HPP
