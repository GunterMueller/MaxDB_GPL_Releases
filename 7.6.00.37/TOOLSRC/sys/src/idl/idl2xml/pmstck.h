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
void PmfPopAlt
(
 st_pmv_p     pmv,
 ul_int_p     altern
);

void PmfPopRet 
(
 st_pmv_p pmv
);

bool PmfAltSucc 
(
 st_pmv_p   pmv,
 ul_int     altern,
 ul_int     succ  
);

bool PmfPreCursor 
(
 st_pmv_p   pmv
);

bool PmfBreakx 
(
 st_pmv_p   pmv,
 st_sdc_p   str_dp,
 us_int_p   cursor_p,
 st_sdc_p   val_dp
);

bool PmfCursor 
(
 st_sdc_p   dum_dp,
 us_int_p   cursor_p,
 us_int_p   val_p
);

bool PmfCallPat 
(
 st_pmv_p    pmv,
 us_int      newpat
);

bool PmfPshPrec 
(
 st_pmv_p    pmv,
 ul_int      cursor,
 us_int      buf_ix
);

bool PmfPopPrec 
(
 st_pmv_p    pmv
);

bool PmfPopPrecImmed 
(
 st_pmv_p    pmv
);

bool PmfGetPrec 
(
 st_pmv_p    pmv,
 ul_int_p    cursor_p
);

bool PmfPshArb
(
 st_pmv_p    pmv,
 us_int      min_ct,
 us_int      max_ct
);

bool PmfResetArb 
(
 st_pmv_p    pmv,
 us_int      altern
);

bool PmfPopArb 
(
 st_pmv_p    pmv,
 bool        exec
);

bool PmfPshAss 
(
 st_pmv_p   pmv,
 char       oper,
 us_int     subnr,
 st_sdc_p   to_str_dp
);

bool PmfPopAss 
( 	
 st_pmv_p    pmv
);

bool PmfPshPrecStr 
(
 st_pmv_p    pmv,
 ul_int      cursor
);

bool PmfPopPrecStr
(
 st_pmv_p    pmv
);

bool PmfPshStr 
(
 st_pmv_p   pmv,
 char       oper,
 us_int     subnr,
 st_sdc_p   to_str_dp
);

bool PmfPopStr 
( 	
 st_pmv_p    pmv
);

