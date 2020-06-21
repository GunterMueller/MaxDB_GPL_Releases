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

static char    *str[10] ={"A","BB","CCC","DDDD","EEEEE","FFFFFF",
                          "GGGGGGG","HHHHHHHH","IIIIIIIII","JJJJJJJJJJ"};
static us_int  strct;
static us_int  extent;     
static char    extents[10];

bool PmCallPrep
(
 st_pmv_p    pmv,
 st_exelst_p r_exe_p
)
{
 us_int ix;
 st_exelst_p exe_p;
 st_conlst_p con_p;
 st_varlst_p var_p;
 bool        calc;
 char        value[8];

  calc   = TRUE;
    
  for (ix = 0;ix <= 4;ix++)
  {
   exe_p = r_exe_p + ix;

   switch(ix)
     {
      case 0:
        break;

      case 1:
        if (exe_p->arg_ty == 1)
          {
           con_p = pmv->con1_p + (exe_p->vcc_id - 1);
           calc  = 0 == memcmp("calc", con_p->val_d.pt, 4);
          }
        else
          {
           calc = FALSE;
          }
        break;

      case 2:
        if (exe_p->arg_ty == 2)
          {
           var_p = pmv->var1_p + (exe_p->vcc_id - 1);
           memset (value, 0, sizeof(value));
           memcpy (value, var_p->val_d.pt, var_p->val_d.lg);
           extent = (us_int) (atoi (value) * 1.76385);
           sprintf (extents, "%d", extent);
          }
        else
          {
           calc = FALSE;
          }
        break;

      case 3:
        if (exe_p->arg_ty == 1)
          {
           con_p = pmv->con1_p + (exe_p->vcc_id - 1);
           calc  = 0 == memcmp("*", con_p->val_d.pt, 1);
          }
        else
          {
           calc = FALSE;
          }
        break;

      case 4:
        if (exe_p->arg_ty == 1)
          {
           con_p = pmv->con1_p + (exe_p->vcc_id - 1);
           calc  = 0 == memcmp("1.7638", con_p->val_d.pt, 5);
          }
        else
          {
           calc = FALSE;
          }
        break;
     }
  }
	
  if (!calc)
    extent = 0;

  return TRUE;
 
}


bool PmCallExec
(
 st_pmv_p   pmv
)
{ 
  
  if (extent == 0)
    {
     if (strct > 9) strct = 0;
     pmv->callstr_d.llg = strlen (str[strct]);
     pmv->callstr_d.pt  = /*PTS 1109360*/ (unsigned char*)str[strct];
     strct++;
    }
  else
    {
     pmv->callstr_d.llg = strlen (extents);
     pmv->callstr_d.pt  = /*PTS 1109360*/ (unsigned char*)extents;
    }
    
  
  return TRUE;
}
