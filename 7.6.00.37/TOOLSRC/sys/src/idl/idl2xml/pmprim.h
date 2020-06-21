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



typedef struct
{
us_int       ind_lg;
stt_sshort   ind;
us_int       wert_lg;
stt_snorm    wert;

} stt_arr, *stt_arr_p;

typedef struct
{
 unsigned long   ct;
 stt_arr         arr[stc_mxnteps]; 

} stt_tab, *stt_tab_p;

void PmpUpper 
( 
 st_sdc_p str_dp
);

void PmpLower 
( 
 st_sdc_p str_dp
);

bool PmpLexeq 
( 
 st_sdc_p str1_dp,
 st_sdc_p str2_dp
);
bool PmpFind
(   
 stt_tab_p tab_p,
 st_sdc_p  ind_dp,
 st_sdc_p  wert_dp
);


bool PmpIniSet
(
 st_sdc_p  str_dp,
 st_sdc_p  set_dp
);

bool PmpIniStr
(
 st_sdc_p  set_dp,
 char_p    str
);

bool PmpInStr 
(
 st_pmv_p  pmv,
 bool      anchored,
 bool      sensitive,
 bool      infront,
 st_sdc_p  substr_dp
);

bool PmpSpan 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
);

bool PmpAny 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
);

bool PmpNotany 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
);

bool PmpBreak 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
);

bool PmpBreakx 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
);

bool PmpBrake 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
);

bool PmpNotbrake 
(
 st_pmv_p  pmv,
 st_sdc_p  set_dp
);

bool PmpFindCset 
(
 st_pmv_p  pmv,
 bool      anchored,
 bool      infront,
 bool      sensitive,
 st_sdc_p  set_dp
);

bool PmpRem  
(
 st_pmv_p  pmv
);

bool PmpPos 
(
 st_pmv_p  pmv,
 ul_int    pos
);

bool PmpRpos 
(
 st_pmv_p  pmv,
 ul_int    pos
);

bool PmpLen 
(
 st_pmv_p  pmv,
 ul_int    len
);

bool PmpTab 
(
 st_pmv_p  pmv,
 ul_int    tab
);

bool PmpRtab 
(
 st_pmv_p  pmv,
 ul_int    tab
);

bool PmpFbal
(
 st_pmv_p  pmv
);
