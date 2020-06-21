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
#define  __RTEBIO
#include "idl/idl2xml/rte.h" 
#include "idl/idl2xml/pms.h" 

#ifdef MS_DOS
#include "idl/idl2xml/string.h"
#include "idl/idl2xml/stdlib.h"
#endif

#define AtL_ct   5
#define MAX_LINE    256

#define PATFILE "pmser.mat"


bool PmRpnOpen
(
 st_pms_p       pms_p,
 int            argc, 
 char         **argv,
 file_pp        patp,
 file_pp        outp,
 file_pp        lstp
)
{
 unsigned short rc;
 st_ParLdsc     AtL_d;
 st_ParAtL      AtL[AtL_ct];
                  
 memset (AtL, 0, sizeof (AtL));
 
 strcpy (AtL[0].nm, "PATFILE"); strcpy (AtL[0].vc, PATFILE);
 strcpy (AtL[1].nm, "MSGINP");  strcpy (AtL[1].vc, "pmser.msg");
 strcpy (AtL[2].nm, "MSGOUT");  strcpy (AtL[2].vc, "pmsmsg.h"); 
 strcpy (AtL[3].nm, "MSGLIST"); strcpy (AtL[3].vc, "pmser.h"); 
 strcpy (AtL[4].nm, "TRCOUT");  strcpy (AtL[4].vc, ""); 
                  
 AtL_d.ct = AtL_ct;
 AtL_d.pt = AtL;
 
 rc  = RteArg (&AtL_d, argc, argv);
 if ((rc % 2) != 1) return 0;

 PmExecInit (pms_p, AtL[0].vc, AtL[4].vc);

 rc  = RteFilOpen (fil_get, AtL[1].vc, patp);
 if ((rc % 2) != 1) return 0;

 rc  = RteFilOpen (fil_put, AtL[2].vc, outp);
 if ((rc % 2) != 1) return 0;

 rc  = RteFilOpen (fil_put, AtL[3].vc, lstp);
 if ((rc % 2) != 1) return 0;

 return ((rc % 2) == 1);

}


main (int argc, char **argv)  
{
  rc_t                rc;
  unsigned short      rtc;
  file_p              inp;
  file_p              outp;
  file_p              lstp;
  st_d                buf_d;
  unsigned char       buf [MAX_LINE];
  unsigned char       exenam [32];
  st_sdc              exenam_d;
  st_sdc              line_d;
  unsigned char       ty [32];
  st_sdc              ty_d;
  char                nm [32];
  st_sdc              nm_d;
  st_sdc              val_d;
  char                val[32];
  st_pms              pms;
  bool                brc;
  char                out [MAX_LINE];
  char                text [MAX_LINE];
  char                name [MAX_LINE];
  char                param [32];
  char                ct [32];
  st_sdc              ct_d;
  char                tx [32];
  st_sdc              tx_d;
  st_d                out_d;
  unsigned short      msg_ct;
  unsigned short      par_ct;
  unsigned short      tot_ct;


  PmdDInit (&line_d,   buf,    sizeof (buf),    ""); 
  PmdDInit (&ty_d,     ty,     sizeof (ty),     "ty"); 
  PmdDInit (&tx_d,     tx,     sizeof (tx),     "tx"); 
  PmdDInit (&nm_d,     nm,     sizeof (nm),     "nm"); 
  PmdDInit (&ct_d,     ct,     sizeof (ct),     "ct"); 
  
  brc = PmRpnOpen (&pms, argc, argv, &inp, &outp, &lstp);

  buf_d.lg = MAX_LINE;
  buf_d.pt = buf;  
  
  
  out_d.pt = (unsigned char *) out;

  strcpy (out, 
  "typedef enum {s_success, s_info, s_warning, s_error, s_fatal} severity_code;");

  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  out_d.lg = 0;
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"void PmsMsg"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"("); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"  unsigned short        msg_id,"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"  char                  modname[5],"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"  unsigned short        line_no,"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"  severity_code         s_code,"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"  unsigned short        arg_ct,"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"  char                 *msg_line,"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,"  ..."); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  strcpy (out,");"); 
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  out_d.lg = 0;
  rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);

  msg_ct = 0;

  val_d.pt = val;
  val[0] = '5';

  brc = PmExecAssign (&pms, "ty", val_d.pt, 1);

  brc = PmExecGetVar (&pms, &ty_d, &val_d);
/*
#define _TSTMSG(p,p0,p1,p2,p3) PmsMsg(1,M__NAME,__LINE__,p0,3,p,p1,p2,p3)
*/
  for (;;)
    {
     rtc = RteFilRecord (fil_get, inp, MAX_LINE, &buf_d);
     if ((rtc % 2) != 1) break;
     
     line_d.lg  = buf_d.lg;
     pms.cursor = 0;
     
     PmdDInit (&exenam_d, exenam, sizeof (exenam), "p_msgall"); 
     brc = PmExecMatch (&pms, &line_d, &exenam_d);
     if (brc)
       {
        brc = PmExecGetVar (&pms, &ty_d, &val_d);
        memcpy (val, val_d.pt, val_d.lg); 
        val[val_d.lg] = 0;
        switch (val[0]) 
          {
           case '1':
             msg_ct++;
             brc = PmExecGetVar (&pms, &nm_d, &val_d);
             memcpy (name, val_d.pt, val_d.lg); 
             name[val_d.lg] = 0;
             par_ct = 0;
             strcpy (out, "#define _");
             strcat (out, name);
             strcat (out, "(p,p0");
             memset (param, 0, sizeof (param));
             break;
           case '2':
             strcat (param, ",p");
             brc = PmExecGetVar (&pms, &ct_d, &val_d);
             memcpy (&param[strlen(param)], val_d.pt, val_d.lg); 
             par_ct++;
             break;
           case '3':
             strcat (out, param);
             sprintf (&out[strlen (out)], 
                      ") PmsMsg(%d,M__NAME,__LINE__,p0,%d,p%s)",
                      msg_ct - 1, par_ct, param);
             out_d.lg = strlen(out);
             rtc = RteFilRecord (fil_put, outp, MAX_LINE, &out_d);
             break;
           case '4': 
             break;
          }
       }
     
    }

/*
#define MSG_NO     2
#define MSG_LEN   83

static char_p msg[MSG_NO] =
{
"MSG1ST first (%d) message (%s)",
"MSG2ND second (%d) message (%s)"
};
*/
  rc  = RteFilClose (&outp);

  rtc = RteFilRewind (inp);

  
  out_d.lg = 0;
  rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);

  sprintf (out, "#define MSG_NO %5d", msg_ct);
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);

  strcpy (out, "#define MSG_LEN   83");
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);

  out_d.lg = 0;
  rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);

  tot_ct = msg_ct;
  msg_ct = 0;
  

  strcpy (out, "static char * MsgLst[MSG_NO] =");
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);

  strcpy (out, "{");
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);

  for (;;)
    {
     rtc = RteFilRecord (fil_get, inp, MAX_LINE, &buf_d);
     if ((rtc % 2) != 1) break;
     
     line_d.lg  = buf_d.lg;
     pms.cursor = 0;
     
     PmdDInit (&exenam_d, exenam, sizeof (exenam), "p_msgall"); 
     brc = PmExecMatch (&pms, &line_d, &exenam_d);
     if (brc)
       {
        brc = PmExecGetVar (&pms, &ty_d, &val_d);
        memcpy (val, val_d.pt, val_d.lg); 
        val[val_d.lg] = 0;
        if (val[0] == '1') 
          {
           msg_ct++;
           brc = PmExecGetVar (&pms, &nm_d, &val_d);
           memcpy (name, val_d.pt, val_d.lg); 
           name[val_d.lg] = 0;
           brc = PmExecGetVar (&pms, &tx_d, &val_d);
           memcpy (text, val_d.pt, val_d.lg); 
           text[val_d.lg] = 0;
           sprintf (out, "\"%s %s\",", name, text);
           out_d.lg = strlen(out);
           if (tot_ct == msg_ct)
             out_d.lg--;
           rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);
          }
       }
     
    }

  strcpy (out, "};");
  out_d.lg = strlen(out);
  rtc = RteFilRecord (fil_put, lstp, MAX_LINE, &out_d);
  
  rc  = RteFilClose (&lstp);


 return 1;
}
