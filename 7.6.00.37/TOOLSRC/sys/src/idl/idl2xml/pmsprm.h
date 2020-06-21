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



bool PmsRem  
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p
);

bool PmsCmpr 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
);

bool PmsMatch 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
);


bool PmsNotmatch 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
);

bool PmsLike
(
 ub_int    ultab[256],
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
);

bool PmsLook
(
 ub_int    ultab[256],
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
);

bool PmsNotlook
(
 ub_int    ultab[256],
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  substr_dp
);

bool PmsPos 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
);

bool PmsRpos 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    pos
);

bool PmsLen 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    len
);

bool PmsTab 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    tab
);

bool PmsRtab 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 us_int    tab
);

bool PmsIniSet
(
 st_sdc_p  str_dp,
 st_sdc_p  set_dp
);

bool PmsIniStr
(
 st_sdc_p  set_dp,
 char_p    str
);

bool PmsAny 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsNotany 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsSpan 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsNotspan 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsBackSpan 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsBrake 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsNotbrake 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsBreak 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsBackBreak 
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmsFbal
(
 st_sdc_p  str_dp,
 us_int_p  cursor_p
);


