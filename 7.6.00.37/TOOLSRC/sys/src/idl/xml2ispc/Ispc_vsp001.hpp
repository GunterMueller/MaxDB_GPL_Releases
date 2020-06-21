/*!*****************************************************************************

  module      : Ispc_vsp001.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : VT types

  ----------------------------------------------------------------------------

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#ifndef ISPC_VSP001_HPP
#define ISPC_VSP001_HPP


#define csp1_vt_empty                           0
#define csp1_vt_null                            1
#define csp1_vt_i2                              2
#define csp1_vt_i4                              3
#define csp1_vt_r4                              4
#define csp1_vt_r8                              5
#define csp1_vt_cy                              6
#define csp1_vt_date                            7
#define csp1_vt_bstr                            8
#define csp1_vt_dispatch                        9
#define csp1_vt_error                          10
#define csp1_vt_bool                           11
#define csp1_vt_variant                        12
#define csp1_vt_unknown                        13
#define csp1_vt_decimal                        14
#define csp1_vt_i1                             16
#define csp1_vt_ui1                            17
#define csp1_vt_ui2                            18
#define csp1_vt_ui4                            19
#define csp1_vt_i8                             20
#define csp1_vt_ui8                            21
#define csp1_vt_int                            22
#define csp1_vt_uint                           23
#define csp1_vt_void                           24
#define csp1_vt_hresult                        25
#define csp1_vt_ptr                            26
#define csp1_vt_safearrary                     27
#define csp1_vt_carray                         28
#define csp1_vt_userdefined                    29
#define csp1_vt_lpstr                          30
#define csp1_vt_lpwstr                         31
#define csp1_vt_filetime                       64
#define csp1_vt_blob                           65
#define csp1_vt_stream                         66
#define csp1_vt_storage                        67
#define csp1_vt_streamed_object                68
#define csp1_vt_stored_object                  69
#define csp1_vt_blob_object                    70
#define csp1_vt_cf                             71
#define csp1_vt_clsid                          72
#define csp1_vt_lc_char                       201
#define csp1_vt_lc_wchar                      202
#define csp1_vt_lc_byte                       203
#define csp1_vt_lc_bool                       204
#define csp1_vt_lc_abap_tab_handle            205
#define csp1_vt_lc_stream_handle              206
#define csp1_vt_lc_int1                       207
#define csp1_vt_lc_kb71_param_desc            208
#define csp1_vt_lc_uint1                      209
#define csp1_vt_lc_ascii7                     210
#define csp1_vt_lc_wyde                       211
#define csp1_vt_lc_numc                       212
#define csp1_vt_lc_utf8                       213
#define csp1_vt_lc_userdefined_array          214
#define csp1_vt_lc_bitfields                  215
#define csp1_vt_lc_oid                        216
#define csp1_vt_lc_extoid                     217
#define csp1_vt_lc_varoid                     218
#define csp1_vt_vector                       4096
#define csp1_vt_array                        8192
#define csp1_vt_byref                       16384


#endif /* ISPC_VSP001_HPP */

