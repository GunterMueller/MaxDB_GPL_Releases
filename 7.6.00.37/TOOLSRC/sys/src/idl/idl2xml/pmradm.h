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
#define PmrFilID 11111

rc_t PmrInit 
(
 st_mam_p v_mam_p
);

rc_t PmrElmAdd
(
 st_mdc_p       v_d_p,
 st_dsc_p       r_rcd_dp,            /* pointer to record descriptor        */
 us_int_p       v_ct_p
);

rc_t PmrElmAppend
(
 st_lstcb_p     v_lcb_p,
 st_dsc_p       r_rcd_dp,            /* pointer to record descriptor        */
 us_int         r_id
);

rc_t PmrElmReplace
(
 st_lstcb_p     v_lcb_p,
 st_dsc_p       r_rcd_dp,            /* pointer to record descriptor        */
 us_int         r_id
);

rc_t PmrLstDelete
(
 st_lstcb_p     v_lcb_p,
 us_int         r_id
);

rc_t PmrDesc 
(
 st_mam_p v_mam_p,
 s_int    from_ix
);

rc_t PmrDescAll 
(
 st_mam_p v_mam_p
);

rc_t PmrTerm 
(
 st_mam_p v_mam_p
);

rc_t PmrFilOpen 
(              
 st_mam_p v_mam_p,
 char     *fn
);

rc_t PmrPatOpen
(
 st_mam_p v_mam_p
);

rc_t PmrStrAdd
(
 st_mdc_p       v_strstk_p,
 st_mdc_p       r_str_dp            /* pointer to new string        */
);
