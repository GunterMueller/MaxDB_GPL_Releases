/* Pattern Primitives for 32bit Cursor 

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

bool PmlRem  
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p
);

bool PmlInStr 
(
 ub_int    ultab[256],
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 bool      anchored,
 bool      sensitive,
 bool      infront,
 st_sdc_p  substr_dp
);

bool PmlFindCset 
(
 ub_int    ultab[256],
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 bool      anchored,
 bool      infront,
 bool      sensitive,
 st_sdc_p  set_dp
);

bool PmlFindNotCset 
(
 ub_int    ultab[256],
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 bool      anchored,
 bool      infront,
 bool      sensitive,
 st_sdc_p  set_dp
);

bool PmlPos 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);


bool PmlPosLe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);


bool PmlPosGt
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);


bool PmlPosGe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);


bool PmlPosLt
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlPosNe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlRpos
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlRposLt
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlRposLe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlRposGe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlRposGt
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlRposNe
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    pos
);

bool PmlLen 
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    len
);

bool PmlTab
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    tab
);

bool PmlRtab
(
 ul_int    lg,
 ul_int_p  cursor_p,
 ul_int    tab
);

bool PmlAny 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmlNotany 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmlSpan 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmlBreak 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmlBrake 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmlNotbrake 
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p,
 st_sdc_p  set_dp
);

bool PmlFbal
(
 ub_int_p  pt,
 ul_int    lg,
 ul_int_p  cursor_p
);

bool PmlBreakx 
(
 st_pmv_p   pmv,
 st_sdc_p  set_dp
);
