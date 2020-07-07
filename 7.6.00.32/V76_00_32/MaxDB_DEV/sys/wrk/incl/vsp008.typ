
      (* UNI_TRANS return-codes *)
      (* *)
      tsp8_uni_error = (
            uni_ok,
            uni_no_such_encoding,
            uni_not_translatable,
            uni_dest_too_short,
            uni_src_too_short,
            uni_is_singlebyte,
            uni_is_multibyte,
            uni_is_incomplete,
            uni_translate_warning);
      tsp8_uni_load_error = (
            uni_load_ok,
            uni_encoding_not_found,
            uni_no_dblang_found,
            uni_cannot_open_infofile,
            uni_cannot_open_localefile,
            uni_cannot_open_longcolfile,
            uni_file_error_localefile,
            uni_file_error_longcolfile,
            uni_memory_alloc_failed,
            uni_encoding_already_loaded,
            uni_bad_localefile,
            uni_bad_infofile);
      (* *)
      tsp8_encoding_type = (
            uni_fixed,
            uni_modal);
      tsp8_value_typ = (
            uni_value,
            uni_index);
      (* *)
      tsp8_uni_flag = (
            uni_upper);
      tsp8_flags = SET OF tsp8_uni_flag;
      (* *)
      (* UNI_TRANS options                                                *)
      (*   uni_change_to_upper:     map character-strings enclosed in     *)
      (*                            single-quotes to uppercase.           *)
      (*                                                                  *)
      (*   uni_fillup_field:        fill the result-string of the trans-  *)
      (*                            lation up to the length specified in_ *)
      (*                            "uni_trans"-parameter "dest_len". Use *)
      (*                            fillchar (see "encoding-record" below *)
      (*                                                                  *)
      (*   uni_del_trailing_blanks: delete trailing blanks of the result  *)
      (*                                                                  *)
      (* *)
      tsp8_uni_trans_option = (
            uni_change_to_upper,
            uni_fillup_field,
            uni_del_trailing_blanks );
      tsp8_uni_opt_set = SET OF tsp8_uni_trans_option;
      (* *)
      (* UNI_LOAD options                                             *)
      (*    uni_read_dblang:    tells "uni_fload" to load the charset *)
      (*                        specified in environm. variable DBLANG*)
      (*                        and to ignore "iso_locale_str"        *)
      (* *)
      tsp8_uni_load_option = (
            uni_read_dblang
            );
      tsp8_uni_load_opt_set = SET OF tsp8_uni_load_option;
      (* *)
      tsp8_descr_string    = PACKED ARRAY [ 1..mxsp8_descr_str  ] OF char;
      tsp8_vendor_string   = PACKED ARRAY [ 1..mxsp8_vendor_str ] OF char;
      (* *)
      tsp8_mapc1values= PACKED ARRAY [ 0..csp8_mapvalues ] OF char;
      tsp8_mapvalues  = PACKED ARRAY [ 0..csp8_mapvalues ] OF tsp00_C2;
      tsp8_mapindexes = PACKED ARRAY [ 0..csp8_mapvalues ] OF tsp00_Int2;
      (* *)
      tsp8_c1_values_table_ptr = ^tsp8_mapc1values;
      tsp8_values_table_ptr    = ^tsp8_mapvalues;
      tsp8_index_table_ptr     = ^tsp8_mapindexes;
 
      tsp8_mapping_table = RECORD
            CASE integer OF
                1 :
                    (values      : tsp8_values_table_ptr;
                    value_typ    : tsp8_value_typ;
                    value_len    : tsp00_Uint1;
                    mt_fill2     : tsp00_Int2;
                    mt_fill3     : tsp00_Int4);
                2 :
                    (indexes     : tsp8_index_table_ptr;
                    imt_fill0    : tsp8_value_typ;
                    imt_fill1    : tsp00_Uint1;
                    imt_fill2    : tsp00_Int2;
                    imt_fill3    : tsp00_Int4);
                3 :
                    (c1_values   : tsp8_c1_values_table_ptr;
                    ic1_fill0    : tsp8_value_typ;
                    ic1_fill1    : tsp00_Uint1;
                    ic1_fill2    : tsp00_Int2;
                    ic1_fill3    : tsp00_Int4);
                END;
            (*ENDCASE*) 
 
      tsp8_mapping_table_ptr = ^tsp8_mapping_table;
 
      tsp8_byterange = PACKED RECORD
            low       : char;
            high      : char;
            delta     : tsp00_Int2;
            br_fill2  : tsp00_Int4
      END;
 
 
      tsp8_subspace = RECORD
            start_ix     : tsp00_Int4; (* should be 0..65535 *)
            end_ix       : tsp00_Int4; (* should be 0..65535 *)
            dimension    : tsp00_Int2;
            ss_filler1   : tsp00_Int2;
            ss_filler2   : tsp00_Int4;
            byteranges   : ARRAY [ 1..csp8_ranges ] OF tsp8_byterange;
      END;
 
 
      tsp8_subcode = RECORD
            descr         : tsp00_KnlIdentifier;
            subspace_cnt  : tsp00_Int2;
            flags         : tsp8_flags;
            sub_c_filler  : boolean;
            subspace      : ARRAY [ 1..csp8_subspaces ] OF tsp8_subspace;
      END;
 
      tsp8_subcode_ptr = ^tsp8_subcode;
      tsp8_subcodes    = ARRAY [ 1..csp8_subcodes ] OF tsp8_subcode_ptr;
      tsp8_subcodes_ptr = ^tsp8_subcodes;
      (* ENCODING-Record                                                    *)
      (*                                                                    *)
      (* iso_locale_str = national characterset, specified as a string      *)
      (*                  like 'JA_JPN.SJIS' or 'ISO8859'                   *)
      (*                                                                    *)
      (* descr          = free-style description-text, describes the        *)
      (*                  characterset (eg. 'Shift-JIS Japan')              *)
      (*                                                                    *)
      (* subcode_cnt    = number of subcodes of a national character-       *)
      (*                  set (eg. ISO8859 has only one subcode (ASCII),    *)
      (*                  but Shift-JIS has 5 subcodes (ASCII, Kana, Kanji  *)
      (*                  etc.). This number is generated.                  *)
      (*                                                                    *)
      (* vendor         = short vendor description, free-style              *)
      (*                                                                    *)
      (* typ            = Classification of characterset, values are:       *)
      (*                  MODAL (Characterset using Shift-In, Shift-Out     *)
      (*                  FIXED (Shift-JIS-type of Characterset             *)
      (*                  Since MODAL-codesets are not implemented, this    *)
      (*                  field is currently not used by s80uni_trans       *)
      (*                                                                    *)
      (* enc_filler     = fill-character used by s80uni_trans to fill       *)
      (*                  fields when requested by option "uni_fill_char"   *)
 
      tsp8_enc_record = RECORD
            mapping1        : tsp8_mapping_table_ptr;
            mapping2        : tsp8_mapping_table_ptr;
            subcode1        : tsp8_subcodes;
            subcode2        : tsp8_subcodes;
            vendor          : tsp8_vendor_string;
            descr           : tsp8_descr_string;
            iso_locale_str  : tsp00_KnlIdentifier;
            subcode_cnt1    : tsp00_Int2;
            subcode_cnt2    : tsp00_Int2;
            enc_filler1     : tsp00_Int2;
            fillcharacter   : tsp00_C4;
            fillchar_len    : tsp00_Uint1;
            max_codewidth   : tsp00_Uint1; (* max nr of bytes per character *)
            typ             : tsp8_encoding_type;
            enc_filler2     : tsp00_Uint1;
      END;
 
      tsp8_encodings = ARRAY [ 1..csp8_encodings ] OF tsp8_enc_record;
      tsp8_encodings_ptr = ^tsp8_encodings;
 
