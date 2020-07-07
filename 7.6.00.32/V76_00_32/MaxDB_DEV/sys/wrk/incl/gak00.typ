
      (*---------------- basic types ---------------------*)
      tak00_ooption = SET OF (is_order_asc, is_opt, is_possible_oflw_field,
            is_val_expression, is_nls_column, is_not_definite,
            is_col_only_specified);
 
      tak00_ofield  =  RECORD
            ofname   : tsp00_KnlIdentifier;
            ofno     : tsp00_Int2;
            ofstno   : tsp00_Int2;
            ofasc    : tak00_ooption;
            offill   : tsp00_Uint1;
            offno    : tsp00_Int2;
            ofapos   : tsp00_Int4;
            ofnode   : tsp00_Int2;
            oftabno  : tsp00_Int2;
      END;
 
 
      tak00_ordfield_arr = ARRAY [ 1..cak00_maxordercolumns ] OF tak00_ofield;
      
      tak00_ord_fields  =  RECORD
            ofield  : tak00_ordfield_arr;
            ocntord : tsp00_Int2;
            ofill1  : tsp00_Int2;
            ofill2  : tsp00_Int4;
      END;
 
      tak00_ord_fields_ptr = ^tak00_ord_fields;
      (* column catalog representation *)
      tak00_colprop  =  (ctkey,    ctopt,    ctdefault,   ctcomment,
            ctunused, ctmulti,     ctlink,    ctdropped,
            ctexpression, ctinvisible, ctcatalog, ctviewname,
            ctjoinviewkey, ctdomain, ctaltered, ctserial);
      tak00_colpropset  =  SET OF tak00_colprop;
 
      tak00_columninfo  =  PACKED RECORD   (* colinfo_l = 44 B *)
            CASE integer OF
                1 :
                    (ccolstack    : tgg00_StackEntry;
                    ccolpropset   : tak00_colpropset;   (* column prop set *)
                    cextcolno     : tsp00_Int2;
                    creccolno     : tsp00_Int2;
                    ctabno        : tsp00_Uint1;
                    cdatatyp      : tsp00_DataType;
                    cdatalen      : tsp00_Int2;
                    cinoutlen     : tsp00_Int2;
                    cdatafrac     : tsp00_Uint1;
                    cshiftoutchar : char;
                    cnextind      : tsp00_Int2;
                    cudtdatatype  : tsp00_C2;
                    cfiller02     : char;
                    ccolumnn_len  : char;
                    ccolumnn      : tsp00_KnlIdentifier);
                2 :
                    (cfiller1     : tgg00_StackEntry;
                    cfiller2      : tak00_colpropset;
                    cfiller3      : tsp00_Int2;
                    cfiller4      : tsp00_Int2;
                    cfiller5      : tsp00_Uint1;
                    cfiller6      : tsp00_DataType;
                    cfiller7      : tsp00_Int2;
                    cfiller8      : tsp00_Int2;
                    cfiller9      : tsp00_Uint1;
                    cbinary       : boolean);
                3:
                    (cfiller_1     : tgg00_StackEntry;
                    cfiller_2      : tak00_colpropset;
                    cfiller_3      : tsp00_Int2;
                    cfiller_4      : tsp00_Int2;
                    cfiller_5      : tsp00_Uint1;
                    cfiller_6      : tsp00_DataType;
                    cfiller_7      : tsp00_Int2;
                    cfiller_8      : tsp00_Int2;
                    cfiller_9      : tsp00_Uint1;
                    cvarchar       : boolean);
                END;
            (*ENDCASE*) 
 
      tak00_colinfo_ptr  =  ^tak00_columninfo;
      tak00_duration_typ  =  ( year, month, day,
            hour, minute, second, microsecond );
 
      tak00_scolinf  =  RECORD
            sci_len       : tsp00_Int2;
            sci_iolen     : tsp00_Int2;
            sci_frac      : tsp00_Int2;
            sci_typ       : tsp00_DataType;
            sci_dttyp     : tak00_duration_typ;
            sci_cprops    : tak00_colpropset;   (* column prop set *)
            sci_com_type  : tsp00_Int2;
            sci_udt_id    : tsp00_C2;
      END;
 
      (* switches for one table selects *)
      tak00_access_config_switch = (
            cs_L1_strategy,
            cs_L2_strategy,
            cs_L2_range_strategy,
            cs_L2_more_strategies,
            cs_L2_strat_no_optim,
            cs_IN_strategy,
            cs_SUBQ_strategy,
            cs_build_result,
            cs_fetch_result,
            cs_disable_inv_only,
            cs_count_records,
            cs_enable_merge,
            cs_keyaccess,
            cs_keyscan,
            cs_keyrange,
            cs_indexaccess,
            cs_indexscan,
            cs_indexrange,
            cs_trace,
            cs_ordered_join,
            cs_coordinator_join,
            cs_operator_join,
            cs_pretend_parameter,
            cs_hashjoin_ratio,
            cs_unused30,
            cs_unused31,
            cs_unused32,
            cs_unused33,
            cs_unused34,
            cs_unused35,
            cs_unused36,
            cs_unused37);
      (* switches for join selects *)
      tak00_join_config_switch = (
            cj_keyaccess,
            cj_indexaccess,
            cj_noaccesspath,
            cj_predefined_buffer,
            cj_unused2,
            cj_unused3,
            cj_unused4,
            cj_unused5,
            cj_unused6);
      tak00_access_config_switches = SET OF tak00_access_config_switch;
 
      tak00_access_configuration = RECORD
            cfg_indexno   : tsp00_Int2;
            cfg_switches  : tak00_access_config_switches;
            cfg_hashratio : tsp00_Int2;
      END;
 
      tak00_join_config_switches = SET OF tak00_join_config_switch;
 
      tak00_join_configuration = RECORD
            cfg_indexno         : tsp00_Int2;
            cfg_parallel_server : tsp00_Int2;
            cfg_bufsize         : tsp00_Int4;
            cfg_filler2         : tsp00_Int2;
            cfg_join_switches   : tak00_join_config_switches;
            cfg_access_switches : tak00_access_config_switches;
      END;
 
      tak00_join_configuration_arr  = ARRAY [ 1..cak00_maxsources ]
            OF tak00_join_configuration;
 
      tak00_PrivR = (
            r_sel,
            r_upd,
            r_ins,
            r_del,
            r_owner,
            r_alter,
            r_index,
            r_link);
      (* *)
      tak00_PrivilegeSet  = SET OF tak00_PrivR;
