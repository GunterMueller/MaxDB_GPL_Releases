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
#include "idl/idl2xml/rte.h"

#include <string.h>

#define LineLength 120

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section of External Routines                                    */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/



/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Definition Section of Internal Routines                                    */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* Routine Section                                                            */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

rc_t RteArg
(
 st_ParLdscp ArL_dp,
 int         argc,
 char        **argv
)
{
rc_t           rc;
char           name[64];         
unsigned short ix;
unsigned short ix2;
unsigned short lg;
char           * pt;
file_p         fid;
st_d           rec_d;
char           rec[LineLength + 2];
st_ParAtLp     AtLp;

  if ((argc >= 2) && (argv[1][0] != '-'))
    {
     strcpy ( name, (char *) argv[1] );
    }
  else
    {
     strcpy ( name, (char *) argv[0] );
     ix = strlen (name) - 1;
     while ((ix > 0) && (name[ix] != '.')) {ix--;}
     
     if (ix > 0) strcpy (&name[ix + 1], "PAR");
    }

  if ((argc < 2) || (argv[1][0] != '-'))
    {
     rc = RteFilOpen (fil_get, name, &fid);
     
     if ((rc % 2) == 0)
       {
        return rc;
       }
     
     rec_d.pt = (unsigned char *) rec;
     
     while (1)
       {
        rc = RteFilRecord (fil_get, fid, LineLength, &rec_d);
        if ((rc % 2) == 0) break;
        
        rec[rec_d.lg] = 0;
        ix = 0;
        while ((ix < rec_d.lg) && ((rec[ix] != ' ') && (rec[ix] != 9))) {ix++;}
       
        rec[ix] = 0;
        
        if (ix < rec_d.lg)
          {
           ix = rec_d.lg;
           while ((rec[ix - 1] == ' ') || (rec[ix - 1] == 9)) ix--;
           if (rec[ix - 1] == '"')
             {
              ix--;
              rec[ix] = 0;
              while ((rec[ix - 1] != '"') && (rec[ix - 1] != 0))
                {ix--;}
             }
           else
             {
              rec[ix] = 0;
              while ((rec[ix - 1] != ' ') && (rec[ix - 1] != 9) && 
                     (rec[ix - 1] != 0)) 
                {ix--;}
             }
          }
        
        AtLp = ArL_dp->pt;
        
        for (ix2 = 0; ix2 < ArL_dp->ct; ix2++, AtLp++)
          {
           if (0 == strcmp (rec, AtLp->nm)) 
             {
              strcpy (AtLp->vc, &rec[ix]);
              break;
             }
          }
       }
     
     rc = RteFilClose (&fid);
    }  

  if (argc > 2)
    {
     for (ix = 2; ix < (unsigned short) argc; ix++)
       {
        pt = strchr (argv[ix], '=');
        if (0 == pt) return 0;
        lg = (unsigned short)(pt - argv[ix]);

        for (ix2 = 0; ix2 < lg; ix2++) 
          rec[ix2] = toupper (argv[ix][ix2]);
        rec[ix2] = 0;

        AtLp = ArL_dp->pt;

        for (ix2 = 0; ix2 < ArL_dp->ct; ix2++, AtLp++)
          {
           /* if (lg > strlen (AtLp->nm)) return 0; */

           if (0 == memcmp (rec, AtLp->nm, lg)) 
             {
              strcpy (AtLp->vc, &argv[ix][lg + 1]);
              break;
             }
          }
        if (ix2 == ArL_dp->ct) return 0; 
       }
    }

  return 1;
}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/* End of Module                                                              */
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

