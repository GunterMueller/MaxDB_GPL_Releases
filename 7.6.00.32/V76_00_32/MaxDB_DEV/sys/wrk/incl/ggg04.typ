
 
      tgg04_FuncParamInfo = RECORD
            fpi_nil_ptr : tsp00_ObjAddr;
            fpi_sel_ptr : tgg00_SelectParamPtr;
      END;
 
 
      tgg04_DbFuncInfo = RECORD
            dbf_codeid              : tgg00_Surrogate;
            dbf_acv_addr            : tsp00_Addr;
            dbf_site                : tgg00_ServerdbNo;
            dbf_new_code            : boolean;
            dbf_com_params          : tsp_dcom_co_create_instance_parms;
            dbf_dispid              : tsp00_Int4;
            dbf_created             : boolean;
      END;
 
      tgg04_FsUseMode = (
            fs_read,
            fs_write,
            fs_readwrite);
      (* *)
 
      tgg04_StringFileDescr = RECORD (*40*)
            str_treeid : tgg00_FileId;
            use_mode   : tgg04_FsUseMode;
            codespec   : tsp00_CodeType;
            dhomesite  : tgg00_ServerdbNo;
      END;
 
      tgg04_Distinct = (
            no_distinct,
            full_distinct,
            distinct_without_warning_byte);
      (* *)
      tgg04_CacheType = (
            cachCatalog_egg04,
            cachConv_egg04,
            cachData_egg04,
            cachFbm_egg04,
            cachProcCode_egg04,
            cachProcData_egg04,
            cachTemp_egg04,
            cachUsm_egg04,
            cachSequence_egg04);
      (* *)
 
      tgg04_ServerdbIdent = RECORD
            svServerdb_gg04   : tsp00_DbName;
            svServernode_gg04 : tsp00_NodeId;
      END;
 
 
      tgg04_ControlUser = RECORD
            gg04cu_password : tsp00_CryptPw;
            gg04cu_user     : tsp00_KnlIdentifier
      END;
 
 
      tgg04_OneShortDay = RECORD
            daynumber : tsp00_Int4;
            dayname   : tsp00_C3;
            filler    : boolean;
      END;
 
 
      tgg04_OneLanguageShortDaynames = RECORD
            language : tsp00_C3;
            filler   : boolean;
            days     : ARRAY [cgg04_monday..cgg04_sunday]
                  OF tgg04_OneShortDay;
      END;
 
      tgg04_ShortDaynames = ARRAY [1..cgg04_languages]
            OF tgg04_OneLanguageShortDaynames;
 
      tgg04_CodeTables = RECORD
            tables      : ARRAY [1..MAX_CODE_TABLES_GG00 ] OF tsp00_Ctable;
      END;
 
 
      tgg04_CodeGlobals = RECORD
            ctype        : tsp00_CodeType;
            char_default : tsp00_CodeType;
            date_time    : tgg00_DateTimeFormat;
            kernel_swap  : tsp00_SwapKind
      END;
 
 
      tgg04_TabIdGlobals = RECORD
      (* 04.09.2003 M.Ki.:                                   *)
      (* sys1_cat and sys2_cat have been removed;            *)
      (* use kb560*Catalog* wrappers or Catalog_Interface to *)
      (* access the catalog files                            *)
      (* PTS 1104393 E.Z. *)
            sys_diag_parse  : tgg00_FileId;
            obj_hash        : tgg00_FileId;
            sys_cmd_analyze : tgg00_FileId;
            sys_data_analyze: tgg00_FileId
      END;
 
      (* *)
      tgg04_ErrorText = (
            errtext_char,
            errtext_stack,
            errtext_surrogate,
            errtext_indexNo);
      (* *)
 
      tgg04_Err = RECORD
            CASE integer OF
                1 :
                    (errt        : tsp00_C256);
                2 :
                    (errtableid  : tgg00_Surrogate;
                    errstack     : tgg00_StackEntry;
                    errtablesite : tgg00_ServerdbNo;
                    errstacktype : tgg00_Tfn;
                    errrest1     : boolean;
                    errrest2     : tsp00_C20);
                3 :
                    (errtableid_ : tgg00_Surrogate;
                    errfiller0   : tsp00_Int4;
                    errfiller1   : tsp00_Int4;
                    errindexno   : tsp00_Uint1;
                    errfiller2   : tsp00_Int4);
                END;
            (*ENDCASE*) 
            (**)
 
      tgg04_subq_context = RECORD
            sc_tree_id      : tgg00_FileId;
            sc_tree_pos     : tgg00_Lkey;
            sc_read_state   : tgg00_BasisError;
      END;

