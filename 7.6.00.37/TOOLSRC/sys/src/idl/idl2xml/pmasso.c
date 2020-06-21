/*


    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#include "idl/idl2xml/pen00.h"
#include "idl/idl2xml/mam.h"
#include "idl/idl2xml/rte.h" 
#include "idl/idl2xml/pms.h" 
#include "idl/idl2xml/pms00.h" 
#include "idl/idl2xml/pmexec.h"
#include "idl/idl2xml/pmmadm.h"
#include "idl/idl2xml/pmfram.h" 
#include "idl/idl2xml/pmdesc.h" 
#include "idl/idl2xml/pmstck.h"
#include "idl/idl2xml/pmsmsg.h"


bool PmAssoPrep
(
 st_pmv_p    pmv,
 st_exelst_p r_exe_p
)
{
 us_int ix;
 st_exelst_p exe_p;

  for (ix = 0;ix <= 6;ix++)
  {
   exe_p = r_exe_p + ix;
  }
	
  return TRUE;
 
}


bool PmAssoExec
(
 st_pmv_p   pmv
)
{ 
  
  
  return FALSE;
}
