
      tpr_intaddr = tsp00_Int4;
      tpr_longint = tsp00_Int4;
      tpr_cmd_state_Enum = integer;
      tpr_parsid      = PACKED ARRAY [ 1..mxpr_parsid ]  OF char;
      tpr00_ParseId = tpr_parsid;
      tpr00_sp1rFunctionCode = tsp00_Int2;

      tpr00_ParseInfo = RECORD
            ParseId          : tpr00_ParseId;
            sp1rFunctionCode : tpr00_sp1rFunctionCode;
      END;

      tpr00_KnlSessionID = PACKED ARRAY [ 1..4 ]  OF char;

      tpr00_SessionID = RECORD
            KnlSession : tpr00_KnlSessionID; (* Session no from Kernel *)
            ClientSession : tsp00_Int4;      (* unique connect id on client *)
      END;

      tpr_password   = tsp00_Name;
      tpr_sqlline    = PACKED ARRAY [ 1..mxpr_sqlline ]  OF char;
      tpr_macroline  = PACKED ARRAY [ 1..mxpr_macroline] OF char;
      tpr_hostname   = tsp00_C64;
      (**   tpr_buffer     = PACKED ARRAY [ 1..mxpr_buffer] OF char; **)
      tpr_buffer     = tsp00_MoveObj;
      tpr_stm_bufferp= ^tsp00_MoveObj;
      tpr_vvalue  = PACKED ARRAY [ 1..mxpr_value ] OF char;
      tpr_symbol  =    tsp00_Int2;

      tpr_scannertype = RECORD    (* 16 B *)
            syposacc : tsp00_Int4;  (* aktuelle pos ab der analys. wird *)
            sypos    : tsp00_Int4;  (* tpr_symbol position *)
            sylen    : tsp00_Int4;  (* tpr_symbol l?ange    *)
            symb     : tpr_symbol; (* erkanntes tpr_symbol *)
            syfill   : tsp00_Int2;    (* :true analyse ist innerhalb einer *)
            (* nur pascal stringkonstanten *)
      END;

      tpr_runtime_errors = (
            cpr_p_ok,
            cpr_area_overflow ,               (* -761 *)
            cpr_cmd_not_available,            (* -101 *)
            cpr_cmd_too_long,                 (* -706 *)
            cpr_could_not_load_lib,           (* -888 *)
            cpr_connect_syntax_wrong,         (* -707 *)
            cpr_conv_from_unicode_impossible, (* -777 *)
            cpr_conv_to_unicode_impossible,   (* -778 *)
            cpr_no_long_columns_found,        (* -779 *)
            cpr_datafile_close_error,         (* -776 *)
            cpr_datafile_open_error,          (* -773 *)
            cpr_datafile_read_error,          (* -775 *)
            cpr_datafile_write_error,         (* -774 *)
            cpr_db_session_not_allowed,       (* -806 *)
            cpr_differ_sqlmode_not_allowed,   (* -756 *)
            cpr_duplicate_fetch,              (* -703 *)
            cpr_duplicate_fetch_callif,       (* -703 *)
            cpr_errorno_from_sysprofile,      (* -720 *)
            cpr_ffetch_fetch_mix_not_allowed, (* -750 *)
            cpr_ffetch_not_allowed,           (* -749 *)
            cpr_implicit_connect_wrong,       (* -711 *)
            cpr_incompatible_datatype,        (* -817 *)
            cpr_inp_default_not_allowed,      (* -822 *)
            cpr_inp_null_not_allowed,         (* -801 *)
            cpr_inp_number_invalid,           (* -802 *)
            cpr_inp_number_overflow,          (* -751 *)
            cpr_inp_number_truncated,         (* w1   *)
            cpr_inp_string_truncated,         (* -743 *)
            cpr_inp_string_truncated_oracle,  (* w1   *)
            cpr_invalid_commandstate,         (*-9801 *)
            cpr_invalid_declare_command,      (* -820 *)
            cpr_invalid_escape,               (* -768 *)
            cpr_invalid_program,              (* -805 *)
            cpr_macro_not_init,               (* -818 *)
            cpr_memory_allocation_faild,      (* -760 *)
            cpr_missing_previous_fetch,       (*-7026 *)
            cpr_missing_user_passw,           (* -715 *)
            cpr_missing_variable_addr,        (* -702 *)
            cpr_multibyteset_not_allowed,     (* -772 *)
            cpr_no_session_connected,         (* -807 *)
            cpr_not_implemented,              (* -808 *)
            cpr_out_null_not_allowed,         (* -809 *)
            cpr_out_number_invalid,           (*-9806 *)
            cpr_out_number_overflow,          (* -811 *)
            cpr_output_part_too_short,        (*  w3  *)
            cpr_paramlist_not_allowed,        (* -819 *)
            cpr_paramlist_too_long,           (* -804 *)
            cpr_paramlist_too_short,          (* -812 *)
            cpr_paramlist_wrong,              (* -883 *)
            cpr_r_loop_init_wrong,            (* -856 *)
            cpr_r_sql_cancelled,              (* -102 *)
            cpr_r_too_many_statements,        (* -868 *)
            cpr_reflex_connect_notok,         (* -709 *)
            cpr_reflex_crash,                 (* +800 *)
            cpr_reflex_notok,                 (* -807 *)
            cpr_reflex_shutdown,              (* -708 *)
            cpr_reflex_start_required,        (* -813 *)
            cpr_reflex_timeout,               (* +700 *)
            cpr_request_area_overflow,        (* -706 *)
            cpr_rmissing_indicatorvariable,   (* -884 *)
            cpr_rt_no_output_variable,        (* -753 *)
            cpr_select_parsid_missing,        (* -710 *)
            cpr_session_already_connected,    (* -752 *)
            cpr_set_format_error,             (*-9808 *)
            cpr_too_many_fetch_statements,    (* -705 *)
            cpr_too_many_hostvar,             (* -893 *)
            cpr_too_many_hostvar_callif,      (* -712 *)
            cpr_too_many_users,               (*-9807 *)
            cpr_tracefile_close_error,        (* -814 *)
            cpr_tracefile_open_error,         (* -815 *)
            cpr_tracefile_write_error,        (* -816 *)
            cpr_trace_error,                  (* -803 *)
            cpr_unknown_datatype,             (*-9803 *)
            cpr_unknown_statement_name,       (* -880 *)
            cpr_unknown_sysprofile,           (* -719 *)
            cpr_unterminated_c_string,        (* -767 *)
            cpr_use_call_stm,                 (* -744 *)
            cpr_valbuf_too_short,             (* -747 *)
            cpr_xuser_file_open_error,        (* -757 *)
            cpr_abap_desc_not_set,            (* -861 *)
            cpr_abap_exception,               (* -862 *)
            cpr_connection_busy,              (* -810 *)
            cpr_session_not_connected,        (* -821 *)
            cpr_invalid_option,               (* -863 *)
            cpr_unknown_cursor_name           (* -874 *)
            );
      (*******************************************)
      (* Descriptor for dynamic allocated memory *)
      (*   neu 10.5.94  descriptor zur Area-     *)
      (*  Behandlung  (siehe vpr03c,p03dynalloc) *)
      (*******************************************)

      tpr_sqlmdesc  = RECORD        (* 20 B *)
            descMaxelem : tsp00_Int4;      (* number of allocated elements *)
            descNelem   : tsp00_Int4;      (* number of used elements *)
            descElemSize: tsp00_Int4;      (* element size in bytes *)
            descIncr    : tsp00_Int4;      (* no. of elem. alloc. in one step *)
            descElemPtr : tpr_intaddr;   (* pointer to first element *)
      END;

      (******************************************)
      (*        V A L U E R E C                 *)
      (*                                        *)
      (* Variant-Record, der zum Austausch der  *)
      (* Parameterwerte zwischen Hostprogramm   *)
      (* und der Pascal-Subroutine des Precom-  *)
      (* piler-Programms dient, in der SQLDB-    *)
      (* Auftraege erzeugt und abgeschickt      *)
      (* werden.                                *)
      (* Austausch erfolgt ueber Adresse der    *)
      (* Hostvariablen                          *)
      (******************************************)
      tpr_vvvtype     =  cpr_vint2 .. cpr_viaddr;

      tpr_valuerec    = RECORD   (* 8488 B *)
            CASE tpr_vvvtype OF
                cpr_vint2 :
                    (i2 : tsp00_Int2);
                cpr_vint4 :
                    (i4 : tsp00_Int4);
                cpr_vreal4 :
                    (r4 : tsp00_Shortreal);
                cpr_vreal8 :
                    (r8 : tsp00_Longreal);
                cpr_vreal8sun :
                    (r8s : tsp00_Longreal);
                cpr_vdecimal :
                    (dec : tsp00_Decimal);
                cpr_vzoned, cpr_vlzone, cpr_vlszon, cpr_vtszon :
                    (zon : tsp00_Zoned);
                cpr_vchar, cpr_vbchar, cpr_vdbchar, cpr_vcharc :
                    (ch : tpr_buffer);
                cpr_vbuf  :
                    (buf : tpr_buffer);
                cpr_viaddr  :
                    (iaddr : tpr_intaddr);
                END;
            (*ENDCASE*)

      tpr_ptr_to_value = ^tpr_valuerec;

      tpr_vtypepointer = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : tpr_ptr_to_value);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L D A                   *)
            (* Descriptor    Area                 *)
            (* Datentype f?ur describe Kommandos   *)
            (* Variablen ?uber diesen type werden  *)
            (* declariert und dynamisch belegt.   *)
            (**************************************)


      tpr_sqlrow = RECORD     (* 8 B *)
            ireclen : tsp00_Int4;
            oreclen : tsp00_Int4;
      END;


      tpr_sqlcol = RECORD     (* 36 B *)
            coltype  : tsp00_DataType;
            colmode  : tsp1_param_opt_set;
            colio    : tsp1_param_io_type;
            colfrac  : tsp00_Uint1;
            collen   : tsp00_Int4;
            colpos   : tsp00_Int4;
            coliolen : tsp00_Int2;
            colisnum : boolean;
            colisuni : boolean;
            colislong: boolean;
            colvartype  : tsp00_Int2;
            colindtype  : tsp00_Int2;
            coltobuf   : tpr_intaddr;
            colfrombuf : tpr_intaddr;
            coltoindi  : tpr_intaddr;
            colfromindi: tpr_intaddr;
      END;


      sqlvartype  =   RECORD   (* 84 B *)
            colname  : tsp00_KnlIdentifier;          (* selectlist columnnames  *)
            colio    : tsp00_Int2;          (* 0:inp ;  1:outp         *)
            colmode  : tsp1_param_opt_set;
            colfill1 : tsp00_C3;
            coltype  : tsp00_Int2;          (* 0:number; 2: char        *)
            collength: tsp00_Int4;          (* anzahl stellen gesamt    *)
            colfrac  : tsp00_Int2;          (* anzahl stel. nach komma  *)
            colfill2 : tsp00_Int2;          (* anzahl stel. nach komma  *)
            hostvartype   : tsp00_Int2;     (* host variable type       *)
            hostcolsize   : tsp00_Int2;     (* hostvar size for array's *)
            hostindicator : tsp00_Int4;     (* host indicator variable  *)
            hostvaraddr : tpr_vtypepointer;(* hostvariablen address    *)
            hostindaddr : tpr_vtypepointer;(* host indicator addresse  *)
            sqlcoldb   : tpr_sqlcol;  (* interne tpr_sqlcol infos sqldb *)
      END;


      sqldatype =   RECORD
            sqldaid : PACKED ARRAY [ 1..8 ] OF char; (* sqlda_const *)
            sqlmax  : tsp00_Int4; (* max anzahl der SQLDA.sqlvar = 300*)
            sqln    : tsp00_Int2; (* anzahl sqlvar elemente      *)
            sqld    : tsp00_Int2; (* erg. anzahl sqlvar elemente *)
            sqlloop   : tsp00_Int4; (* array statem loop value,default=1 *)
            sqloffset : tsp00_Int4; (* offset of loop index  1,2.. *)
            sqlkano   : tsp00_Int2; (* leztes describe kommando    *)
            sqlprno   : tsp00_Int2; (* letztes describe prkommando *)
            sqlkamode : tsp00_Int2; (* call_interface gakamode     *)
            sqlfiller : tsp00_Int2; (* offset of loop index  1,2.. *)
            sqlrow    : tpr_sqlrow; (* sqlrow 8 B *)
            sqlvar  : ARRAY [ 1..mxpr_sqln ] OF sqlvartype;
      END;

      sqldapointer = ^sqldatype;
      (**************************************)
      (*        S Q L D A  for  D B 2 /S A A*)
      (* Descriptor    Area                 *)
      (* Datentype f?ur describe Kommandos   *)
      (* Variablen ?uber diesen type werden  *)
      (* declariert und dynamisch belegt.   *)
      (**************************************)

      sqlvardb2type  =   RECORD   (* 80 B *)
            sqltype    : tsp00_Int2;        (*   data type db2 or saa   *)
            sqllen     : tsp00_Int2;        (* data external length     *)
            sqldata    : tpr_vtypepointer;(* hostvariablen address    *)
            sqlind     : tpr_vtypepointer;(* indicator     address    *)
            sqlnamel      : tsp00_Int2;     (* sqlname length           *)
            sqlname       : tsp00_C30;      (* selectlist columnnames   *)
            sqlcoldb      : tpr_sqlcol;   (* interne tpr_sqlcol infos *)
      END;


      sqldb2type =   RECORD
            sqldaid : PACKED ARRAY [ 1..8 ] OF char; (* sqlda_const *)
            sqldabc : tsp00_Int4;     (* max length of SQLDA in bytes *)
            sqln    : tsp00_Int2;     (* anzahl sqlvar elemente      *)
            sqld    : tsp00_Int2;     (* erg. anzahl sqlvar elemente *)
            sqlloop   : tsp00_Int4; (* array statem loop value,default=1 *)
            sqloffset : tsp00_Int4; (* offset of loop index  1,2.. *)
            sqlkano   : tsp00_Int2; (* call-int.  mfindex bei ffetch *)
            sqlprno   : tsp00_Int2; (* call-int.fetchspec bei ffetch *)
            sqlkamode : tsp00_Int2; (* call_interface gakamode     *)
            sqlfiller : tsp00_Int2; (* offset of loop index  1,2.. *)
            sqlrow    : tpr_sqlrow; (* sqlrow 8 B *)
            sqlvardb2 : ARRAY [ 1..mxpr_sqln ] OF sqlvardb2type;
      END;

      sqldb2pointer = ^sqldb2type;
      (**************************************)
      (*        S Q L D A                   *)
      (* Descriptor    Area   O R A C L E   *)
      (* Datentype f?ur describe Kommandos   *)
      (* Variablen ?uber diesen type werden  *)
      (* declariert und dynamisch belegt.   *)
      (**************************************)
      tpr_arr_int2 = ARRAY [1..mxpr_sqln] OF tsp00_Int2;
      tpr_arr_int4 = ARRAY [1..mxpr_sqln] OF tsp00_Int4;
      tpr_arr_ptrv = ARRAY [1..mxpr_sqln] OF tpr_vtypepointer;
      tpr_arr_dbcolinfo =  ARRAY [1..1] OF tpr_sqlcol;
      tpr_arr_ptrvp   = ^tpr_arr_ptrv;
      tpr_arr_int2p   = ^tpr_arr_int2;
      tpr_arr_int4p   = ^tpr_arr_int4;
      tpr_arr_sqlcolp = ^tpr_arr_dbcolinfo;

      tpr_sqlcolpointer = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (sqlcolp : tpr_arr_sqlcolp);
                END;
            (*ENDCASE*)


      sqloratype =   RECORD
            sqldaid : PACKED ARRAY [ 1..8 ] OF char; (* sqlda_const *)
            sqldabc : tsp00_Int4;   (* max length of SQLDA in bytes *)
            sqlfill : tsp00_Int4;   (* dummy *)
            sqlloop : tsp00_Int4;   (* sqlda array statement :loop *)
            sqloffset : tsp00_Int4; (* offset of loop index  1,2.. *)
            sqlkano   : tsp00_Int2; (* call-int.  mfindex bei ffetch *)
            sqlprno   : tsp00_Int2; (* call-int.fetchspec bei ffetch *)
            sqlkamode : tsp00_Int2; (* call_interface gakamode     *)
            sqlfiller : tsp00_Int2; (* offset of loop index  1,2.. *)
            sqlrow    : tpr_sqlrow; (* sqlrow 8 B *)
            sqlcoldb  : tpr_sqlcolpointer; (* pointer to array of sqlcol's *)
            ora_n   : tsp00_Int4;   (* anzahl sqlvar elemente      *)
            ora_v   : tpr_arr_ptrvp; (** ptr arr of addr of SLI or BV values *)
            ora_l   : tpr_arr_int4p; (*  ptr arr of length of SLI or BV value*)
            ora_t   : tpr_arr_int2p; (*  ptr arr of datatyp.of SLI or BV val.*)
            ora_i   : tpr_arr_ptrvp; (** ptr arr of addr of indicator values *)
            ora_f   : tsp00_Int4;   (*  actuelle anzahl sqlvar elemente     *)
            ora_s   : tpr_arr_ptrvp; (** ptr arr of addr of SLI or P names   *)
            ora_m   : tpr_arr_int2p; (*  ptr arr of max-leng of SLI or P name*)
            ora_c   : tpr_arr_int2p; (*  ptr arr of cur-leng of SLI or P name*)
            ora_x   : tpr_arr_ptrvp; (** ptr arr of addr of indicator names  *)
            ora_y   : tpr_arr_int2p; (*  ptr arr of max-leng of indica.names *)
            ora_z   : tpr_arr_int2p; (*  ptr arr of cur-leng of indica.names *)
            (* for cobol only *)
            ora_fm  : tpr_arr_ptrvp; (** ptr arr of addr of hostvar decimal  *)
            ora_fml : tpr_arr_int2p; (*  ptr arr of max-leng of hostvar decim*)
            ora_fclp: tpr_arr_int2p; (*  ptr set to null for future use    *)
            ora_fcrc: tpr_arr_int2p; (*  ptr set to null for future use    *)
      END;

      sqlorapointer = ^sqloratype;
      (*********************************************)
      (*        S Q L D A V R E C    and           *)
      (*        S Q L V A R V R E C                *)
      (*  Varient record for sqlda in              *)
      (*    call interface                         *)
      (*********************************************)

      sqlvarvrec  = RECORD
            CASE boolean OF
                true :
                    (sqldbv  : sqlvartype);
                false :
                    (db2v   : sqlvardb2type);
                END;
            (*ENDCASE*)

      tpr_kind      = cpr_kind_empty .. cpr_kind_oracle;

      sqldaptrvrec  = RECORD
            CASE tpr_kind  OF
                cpr_kind_internal :
                    (sqldaptr  : sqldapointer);
                cpr_kind_db2 :
                    (db2daptr  : sqldb2pointer);
                cpr_kind_oracle :
                    (oradaptr  : sqlorapointer);
                cpr_kind_empty  :
                    (intaddr   : tpr_intaddr);
                END;
            (*ENDCASE*)


      sqldavrec  = RECORD
            CASE tpr_kind  OF
                cpr_kind_internal :
                    (sqldb  : sqldatype);
                cpr_kind_db2 :
                    (db2   : sqldb2type);
                cpr_kind_oracle :
                    (orada   : sqloratype);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L T R A C E  TYPE  neu  *)
            (*    trace output infos              *)
            (*    aus alten sqlca ?ubernommen    *)
            (**************************************)


      sqltatype    = RECORD
            tatracefn    : tsp00_VFilename; (* options name oder actueller *)
            tatracepprt  : tsp00_Int4;     (* lzu 3.0 vf  poolprt *)
            tatraceno    : tsp00_Int4;     (* fileno nach vfopen *)
            tatracety    : tsp00_Int2;     (* option type oder actueller  *)
            tamaxsecoutp : tsp00_Int2;     (*is_true komm. wird ausgegeben*)
            tamaxsecfn   : tsp00_VFilename; (* sqltracefn max sec *)
            tamaxsecno   : tsp00_Int4;     (* file number*)
            tamaxsecpptr : tsp00_Int4;     (* pool-pointer*)
            tastartdate  : tsp00_C8;
            taenddate    : tsp00_C8;
            tastarttime  : tsp00_C8;
            taendtime    : tsp00_C8;
            tacnttrstm   : tsp00_Int2;  (* count statem.s in trace zyklisch*)
            tatrout      : tsp00_Int2;  (* trace output typefr?uher xatrace *)
            tafiller     : tsp00_Int2;
            tastr80l     : tsp00_Int2;
            tastr80      : tsp00_C256;   (* output string z.Z parameter *)
      END;

      sqltapointer =  ^sqltatype;
      (**************************************)
      (*    O P T I O N - A R E A   neu     *)
      (*    runtime optionen                *)
      (*    aus alten sqlga ?ubernommen      *)
      (**************************************)

      sqloatype  = RECORD   (* 82 B *)
            oamodtracefn    : tsp00_VFilename;  (* modul options *)
            oamodtrace      : tsp00_Int2;     (* modul option=true*)
            oamodtracety    : tsp00_Int2;     (* modul options *)
            oamodsettrty    : tsp00_Int2;     (* set trace options *)
            oamaxtrstm      : tsp00_Int2;  (* max statem.s in trace zyklisch*)
            oanosdf         : tsp00_Int2;  (* r.op noseldirfast  cpr_is_true*)
            oamaxsec        : tsp00_Int2;  (* trace max sec command option  *)
            oapidflag       : tsp00_Int2;  (* tracefilename :: processid    *)
            oapid           : tsp00_Int4;
      END;

      sqloapointer =  ^sqloatype;
      (*********************************************)
      (*        LONGVARCHAR                        *)
      (*  For sql commands with longvarchars       *)
      (* Structur to send a hostvariable >= 4k     *)
      (* to the  kernel                            *)
      (*********************************************)

      tpr_longvarcharrec   = RECORD       (* 44 B *)
            lvccolno   : tsp00_Int2;  (* last colno           *)
            lvcdescno  : tsp00_Int2;  (* last descno          *)
            lvcfreepos : tsp00_Int4;  (* varpart inp  freepos *)
            lvcaoffset : tsp00_Int2;  (* last hostaddr offset *)
            lvcnext    : boolean;   (* next longvar column  *)
            lvcvalmode : tsp00_ValMode;  (* last desc valmode *)
            lvcoldpos  : tsp00_Int2;     (* dummy     *)
            lvcoldlen  : tsp00_Int2;     (* dummy     *)
            lvcvalpos  : tsp00_Int4;     (* last desc valpos  *)
            lvcvallen  : tsp00_Int4;     (* last desc vallen  *)
            lvchostpos : tsp00_Int4;  (* last hostvar pos     *)
            lvchostlen : tsp00_Int4;  (* last hostvarlen      *)
            lvcindicator:tsp00_Int4;  (* indicator inhalt     *)
            lvcvalmlen : tsp00_Int4;  (* max len an kern gesendet *)
            lvcerror   : tpr_runtime_errors; (*        error    *)
            lvcpiecewise : boolean;
      END;

      (*********************************************)
      (*        LONGVARCHAR DESCRIPTOINS INFOS     *)
      (*  For sql commands with longvarchars       *)
      (*  globale variable in g_init allocated     *)
      (* pointer is set in sqlra-area (dlvcp)      *)
      (*   will be set in procedure p04pinputvalue *)
      (*   will be init in p01adbsexecute          *)
      (*   will be used only for one command       *)
      (*********************************************)

      tpr_ptr_packet = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : tsp1_packet_ptr);
                END;
            (*ENDCASE*)


      tpr_ptr_moveobj = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : tsp00_MoveObjPtr);
                END;
            (*ENDCASE*)


      tpr_restbuflong = RECORD     (* 28 B *)
            rbcolno      : tsp00_Int2;  (* last bearbeitete colno *)
            rbfiller1    : tsp00_Int2;  (* last bearbeitete colno *)
            rbfiller2    : tsp00_Int4;  (* last bearbeitete colno *)
            rbrestlen    : tsp00_Int4;  (* anz bytes im puffer    *)
            rbhostlensum : tsp00_Int4;  (* akumulierte hostlength *)
            rblongcolmax : tsp00_Int4;  (* maximale longcollength *)
            rbbufsize    : tsp00_Int4;  (* allocate leng from rbvarpart *)
            rbbufpart    : tpr_ptr_moveobj;
      END;


      tpr_ptr_restbuflong = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : ^tpr_restbuflong);
                END;
            (*ENDCASE*)


      tpr_colpointer = RECORD
            CASE boolean OF
                true:
                    (addr: ^tpr_sqlcol);
                false:
                    (intaddr: tpr_intaddr);
                END;
            (*ENDCASE*)


      tpr_sqlspld   = RECORD   (* 60 B *)
            CASE boolean OF
                false:
                    (
                          ldkind_o   :  tsp00_Int2; (* cpc_if_false : old *)
                    ldlength_o :  tsp00_Int2; (* sizeof (tsp00_LongDescriptor) *)
                descold :
                    tsp00_LongDescriptor
                          );
                true :
                    (
                          ldkind_n   :  tsp00_Int2; (*  cpr_is_true : new *)
                    ldlength_n :  tsp00_Int2; (* sizeof (tsp00_LongDescBlock) *)
                descnew :
                    tsp00_LongDescBlock
                          );
                END;
            (*ENDCASE*)


      tpr_sqlpcld   = RECORD    (* 44 B *)
            ldhostaddr : tpr_vtypepointer; (* last hostvaraddr  *)
            ldindkaddr : tpr_vtypepointer; (* last indikatoraddr*)
            ldcoladdr  : tpr_colpointer;   (* sqlcol record addr*)
            ldhostlen  : tsp00_Int4;  (* last hostvarlen      *)
            ldhosttype : tsp00_Int2;  (* last hostvartype     *)
            ldindktype : tsp00_Int2;  (* last indikatortype   *)
            ldindklen  : tsp00_Int2;  (* last indikatortypelen*)
            ldindkfrac : tsp00_Int2;  (* last indikatortypefrac*)
            ldrowno    : tsp00_Int2;  (* rowno for m_commands *)
            ldcolno    : tsp00_Int2;  (* colno im record      *)
            ldsfiotype : tsp00_C1;    (* index 1 = sfiotype  *)
            ldsfdtype  : tsp00_DataType; (* index 2 = sfdtype   *)
            ldfill     : tsp00_Int2;
            ldfileno   : tsp00_Int4;  (* fileno if long spalte datei *)
            ldsfbufpos : tsp00_Int4;  (* first inp varpart pos*)
            ldhostlensum : tsp00_Int4;(* hostvarlen aufaddiert  *)
            ldrestdata : tpr_ptr_restbuflong;  (* pointer to restdata in varpart *)
      END;

      tpr_sqlspldarr = ARRAY [ 1..mxpr_descrlvc ] OF tpr_sqlspld;
      tpr_sqlpcldarr = ARRAY [ 1..mxpr_descrlvc ] OF tpr_sqlpcld;
      tpr_sqlspldptr = ^tpr_sqlspldarr;
      tpr_sqlpcldptr = ^tpr_sqlpcldarr;

      sqlorentry_ptr = ^sqlorentry;

      tpr_sqlld   = RECORD          (* 36 B *)
            ldmaxi       : tsp00_Int2; (* maximale lvc-col inp *)
            ldmaxo       : tsp00_Int2; (* maximale lvc-col outp*)
            ldindpc      : tsp00_Int2; (* index to pc_descr    *)
            ldindsp      : tsp00_Int2; (* index to ldsp*)
            ldcolkind    : tsp00_Int2; (* cpr_is_true lvc-col,cpr_is_false*)
            ldcount      : tsp00_Int2; (* anz act. descr in buf*)
            ldputvalsend : boolean;  (* init false, true putval send *)
            ldallsend    : boolean;  (* init false, true all record send*)
            ldretmaxi    : tsp00_Int2; (* maximale lvc-col inp *)
            ldretmaxo    : tsp00_Int2; (* maximale lvc-col outp*)
            ldretindpc   : tsp00_Int2; (* index to pc_descr    *)
            ldretindsp   : tsp00_Int2; (* index to ldsp*)
            ldrestbufp   : tpr_ptr_restbuflong;
            lderrd3      : tsp00_Int4; (* anz fetched rows *)
            ldpc         : tpr_sqlpcldptr;
            ldsp         : tpr_sqlspldptr;
            ldpc_mdesc   : tpr_sqlmdesc; (* PTS 1102339 *)
            ldsp_mdesc   : tpr_sqlmdesc; (* PTS 1102339 *)
            ldpcarr      : tpr_intaddr;
            ldsparr      : tpr_intaddr;
            ldpcarr_mdesc : tpr_sqlmdesc;
            ldsparr_mdesc : tpr_sqlmdesc;
	    ldorelastcmd : sqlorentry_ptr; (* ref. to sqlor area of cmd before putval/getval *)
      END;


      tpr_ptr_sqlld = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : ^tpr_sqlld);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L G A   neu              *)
            (*  Global Area                       *)
            (* Globaldaten, die zusaetzlich       *)
            (* benoetigt werden                   *)
            (**************************************)

      tpr_part_kind_ptr_record = ARRAY [ 1..mxpr_partkind ] OF tsp1_part_ptr;

      sqlgaentry =  RECORD     (* 220 B *)
      (* sqlga *)
            gareference : tsp00_TaskId; (* sapdb pid 4B   *)
            gamesscode  : tsp00_C2;         (*code_type, swap_kind*)
            gastatus    : tsp00_Int2;
            gavaindex   : tsp00_Int2; (*0=tpr_hostname, >0 :hostvar.*)
            ganodeindex : tsp00_Int2;
            gareconnect : tsp00_Int2;  (* reconnect set is_true, *)
            ganolog     : tsp00_Int2;
            gaprofinit  : tsp00_Int2;        (* profile init   *)
            gadbcodeset : tsp00_Int2;        (* connect codeset *)
            gaatdbname  : tsp00_KnlIdentifier;      (* db-name  18B    *)
            gafiller2   : tsp00_Int2;
            gapatchptr  : tpr_intaddr;   (*  patch area *)
            gareqptr    : tsp1_packet_ptr;
            garecptr    : tsp1_packet_ptr;
            gapacketlist: tsp1_packet_list;
            gaxuserrec : tsp4_xuser_record;   (* 180 B *)
            gaKernelVersion : tsp00_Int4;
            gaServerNode : tpr_intaddr;
            gaIsCanceled : boolean;
            gaKnlSessionID : tpr00_SessionID;
      END;

      sqlgaentrypointer   = ^sqlgaentry;

      sqlgatype =  RECORD     (* 232 B *)
      (* sqlga *)
            gaopxuserrec  : tsp4_xuser_record;   (* 180 B *)
            gauseropset   : tsp00_Int2;  (* cpr_is_true, if useroptions *)
            gamodisolation: tsp00_Int2;  (* modul options *)
            gasqlconntyp  : tsp00_Int2;
            gaencodingcnt : tsp00_Int2;     (* from s81uni_fload         *)
            gaencodingptr : tpr_intaddr;  (* pointer from s81uni_fload *)
            gaasciicodeset: tsp00_Int2;     (* src_  from s81uni_fload   *)
            gaunicodeset  : tsp00_Int2;     (* dest_ from s81uni_fload   *)
            gacmpkind     : tsp00_Int2;     (* component, oci, odbc, c, cob *)
            gaCmdState    : tpr_cmd_state_Enum ;
            gaConnection  : tsp00_Addr;
            gaentry       : PACKED ARRAY  [1..8] OF sqlgaentrypointer;
      END;

      sqlgapointer =  ^sqlgatype;
      (**************************************)
      (*        S Q L R A                   *)
      (*    Runtime Area                    *)
      (* SQLDB-Auftragssegment               *)
      (* f?ur  SQL und Dialog Aufrufe       *)
      (* Precompilerprogramm                *)
      (**************************************)

      tpr_error_warnset =  RECORD
            CASE boolean OF
                true :
                    (int2 : tsp00_Int2);
                false :
                    (warn : tsp00_WarningSet);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L R A  neu              *)
            (* Runtime Area SQLDB-Auftragssegment *)
            (* f?ur  SQL und Dialog Aufrufe       *)
            (* Precompilerprogramm                *)
            (**************************************)


      tpr_sqlerror  =  RECORD    (* 104 B *)
            ereturncode   :  tsp00_Int2;
            etextlen      :  tsp00_Int2;
            eerrorpos     :  tsp00_Int4;
            ewarning      :  tsp00_C16;
            esqlstate     :  tsp00_C6;
            elzu          :  tsp1_comm_error;
            eprerr        :  tpr_runtime_errors;
            etext         :  tsp00_C70;
      END;

      sqlempointer = ^tpr_sqlerror;

      sqlratype  = RECORD    (* 68 B *)
      (*  sqlra *)
            ratermid      : tsp00_C8;
            raprocessid   : tsp00_TaskId;  (*4B anwender pid  *)
            racomponent   : tsp00_C3;
            rasqlos       : tsp00_Os;    (* 1 B char betriebssystem *)
            ralang        : tsp00_Int2;  (* language of runtime syst*)
            rasqlinit     : tsp00_Int2;  (* sqlinit = 3 cpr_is_init  *)
            raactsession  : tsp00_Int2;
            radbmode      : tsp00_Int2;  (* programm sqldbmode      *)
            rasqlansi     : tsp00_Int2;  (*z cpr_kind_empty or kind_ansi *)
            rakamode      : tsp00_Int2;  (* actual statem mode      *)
            ramodeswitch  : tsp00_Int2;  (* switch fuer mess_type   *)
            ramesstype    : tsp00_Int2;  (* last request mess_type  *)
            ranumrows     : tsp00_Int4;  (* number of rows errd(3)  *)
            ranotrtimeoutp: tsp00_Int2;  (* no trace time output    *)
            raopprof      : tsp00_Int2;  (* profile option          *)
            raprofsec     : tsp00_Int4;  (* profile sec pro command *)
            raparse_again : boolean ;  (* true, wenn -8 gesetzt   *)
            rafiller      : tsp00_C3  ;
            ra2filler     : tsp00_Int4;
            rasegptr      : tsp1_segment_ptr;
            rasegmpartptr : tpr_part_kind_ptr_record;
            raprofstmtp   : tpr_intaddr;  (* profile stmt-pointer  *)
            rasqlldp      : tpr_ptr_sqlld;    (* dlvc pointer      *)
            rasqltap      : sqltapointer;    (* trace area        *)
      END;

      sqlrapointer = ^sqlratype;
      (*********************************************)
      (*        S Q L S N                          *)
      (* Statementname Area for statementname or   *)
      (* cursorname from hostvar                   *)
      (*********************************************)

      tpr_sqlsnentry =  RECORD           (* 52 B *)
            snname    : tsp00_KnlIdentifier;     (* statement or cursorname *)
            sntype    : tsp00_Int2;     (* Type of name cpr_is_statement or *)
            (* cpr_is_cursor *)
            snindex   : tsp00_Int2;     (* index into st-area or sn-area *)
            (* in case of sntype *)
            sncount   : tsp00_Int2;     (* count of st-area *)
            snfiller1  : tsp00_Int2;
            snstmtp   : tpr_vtypepointer; (* pointer to statement if the *)
            (* comes from hostvar snstind = 0 *)
            snbuflen  : tsp00_Int4;     (* len of allcate buffer  *)
            snstmtlen : tsp00_Int4;     (* len of statement *)
            (** neu 8.7.96 ****)
            snsession : tsp00_Int2;     (* prepare stmname raactsessionr *)
            snfiller2  : tsp00_Int2;
            snParseInfo : tpr00_ParseInfo;  (* parsid von prepare  *)
      END;

      tpr_sqlsnentries = ARRAY [ 1..mxpr_sqlsn ] OF tpr_sqlsnentry;

      tpr_ptr_sqlsne = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : ^tpr_sqlsnentries);
                END;
            (*ENDCASE*)


      tpr_sqlsn = RECORD
            snentcnt : tsp00_Int4;
            snentact : tsp00_Int4;    (* actueller snentry index **)
            snentptr : tpr_ptr_sqlsne;
      END;

      (*********************************************)
      (*        S Q L M A                          *)
      (*  Macro Area                              *)
      (* Liste fester Laenge                       *)
      (* Eintraege enthalten Werte der Macro-      *)
      (* parameter, die in SQL-Kommandos des       *)
      (* Hostprogramms vorkommen                   *)
      (*********************************************)

      tpr_mavalue  =  RECORD       (* 32 B *)
            malen      : tsp00_Int2;    (* macrolength  *)
            maval      : tpr_macroline;
      END;

      sqlmaentries = ARRAY [ 1..mxpr_sqlma ] OF tpr_mavalue;

      sqlmatype = RECORD       (* 4100 B *)
      (* sqlma *)
            maversion : tsp00_Int4;
            maentry   : sqlmaentries;
      END;

      sqlmapointer =  ^sqlmatype;

      tpr05_StringEncoding = tsp00_Addr;

      (**************************************)
      (*        S Q L M F                   *)
      (* Mfetch Area                        *)
      (* wird zur Umschaltung von fetch auf *)
      (* mfetch gegraucht                   *)
      (**************************************)
      (* fetch optimierungs typen  *)

      tpr_mfetchentry  = RECORD      (* 92 B *)
            mfInit       : tsp00_Int2; (* cpr_is_true record belegt *)
            mfSelectno   : tsp00_Int2; (* zugeh. select number  *)
            mfBufinit    : tsp00_Int2; (* 0 = kein new alloc,<>0 buf exist*)
            mfReclen     : tsp00_Int2; (* acctuelle record length im buf *)
            mfBuflen     : tsp00_Int4; (* zugeh. auftrags puffer length*)
            mfRecpos     : tsp00_Int4; (* acctuelle record position im buf*)
            mfPrevfaspec : tsp00_Int2; (* faspec from command before      *)
            mfLastfaspec : tsp00_Int2; (* faspec cpr_fe_last if it was called *)
            mfRownotfound: tsp00_Int2; (* error : cpr_is_true,                *)
            mfFetchget   : tsp00_Int2; (* init = 0, if one mfetch = cpr_is_init*)
            mfRecposbeg  : tsp00_Int4; (* first record position in buf    *)
            mfMaxselrec  : tsp00_Int4; (* max records in result:sqlerrd(3)*)
            mfLastrecno  : tsp00_Int4; (* last fetch rec no to user    *)
            mfNextLoopCnt: tsp00_Int4; (* Number of remaining records in mfBuf *)
            mfAllRecordsRead: tsp00_Int4;
            mfActualPos: tsp00_Int4;   (* Position of the actual record to read *)
            mfBufbegno   : tsp00_Int4; (* record begin no  in buffer   *)
            mfBufendno   : tsp00_Int4; (* record end no  in buffer     *)
            mfLongcolpos : tsp00_Int4; (* first longvarchar pos in buf    *)
            mfBufpointer : tpr_ptr_packet;  (* pointer eines packets.   *)
            mfrecpointer : tpr_ptr_moveobj; (* pointer auf 1.record save*)
            mfsegmpartptr: tpr_part_kind_ptr_record;
            mfrecptrlen  : tsp00_Int4;    (* length of rec pointer buffer *)
            mforindex    : tsp00_Int2;    (* index in or_area von open    *)
            mffetchkind  : tsp00_Int2;    (*init:is_true, fetch:is_false, *)
            (*  mfetch: is_loop             *)
            mfsumanzrec  : tsp00_Int4;    (* anz rec gesammt bei mfetch is_loop*)
            mfopendata   : tsp00_Int2;    (*init:is_false, opendata:is_true*)
            mfPrecomindex: tsp00_Int2;    (* empty    *)
            mfSelparsid  : tpr_parsid;  (* select parsid des mfetch     *)
            mfresultname : tsp00_KnlIdentifier;    (* select resultname            *)
            PacketEncoding : tpr05_StringEncoding;
      END;

      sqlmfentarr = ARRAY [ 1..csp_maxint2 ] OF tpr_mfetchentry;
      sqlmfentptr = ^sqlmfentarr;

      sqlmftype   =  RECORD    (* 1516 B *)
            mfselcnt   :  tsp00_Int2;
            mfmfetch   :  tsp00_Int2; (* change next com to mfetch *)
            mffiller   :  tsp00_Int4; (* 8 alignment                *)
            mfdesc     :  tpr_sqlmdesc;
            mfentry    :  sqlmfentptr;
      END;

      sqlmfpointer =  ^sqlmftype;
      tpr_sqlarea_mode  = 0.. 10 ;

      sqlareapointer = RECORD
            CASE tpr_sqlarea_mode  OF
                0  :
                    (intaddr   : tpr_intaddr);
                1 :
                    (sqlgaptr  : sqlgapointer);
                2 :
                    (sqlraptr  : sqlrapointer);
                3 :
                    (sqloaptr  : sqloapointer);
                4 :
                    (sqlmaptr  : sqlmapointer);
                5 :
                    (sqlmfptr  : sqlmfpointer);
                6 :
                    (sqllaptr  : tpr_ptr_sqlld);
                7 :
                    (sqltaptr  : sqltapointer);
                END;
            (*ENDCASE*)
            (*********************************************)
            (*        S Q L V A 1, 2, 3, EX - A R E A S  *)
            (*  Variable Area                            *)
            (* sqlva1 addressen der hostvariblen + index *)
            (*        to  sqlva2, sqlva3                 *)
            (* sqlva2 contains the datatyps              *)
            (* sqlva3 contains the variable names        *)
            (* sqlnd   contains the declare for expanding*)
            (*         the sqlva1 table  only used       *)
            (*         during precompiling (vpc00)       *)
            (*********************************************)


      sqlva1entry = RECORD    (* 16 B *)
            CASE tsp00_Int2 OF
                sqlvasc:
                    (
                          va1indi_sc : tsp00_Int2;
                    va1indva2_sc : tsp00_Int2;
                    va1indva3_sc : tsp00_Int2;
                    va1fill   : tsp00_Int2;
                    va1addr_sc : tpr_vtypepointer;
                    va1fill1  : tpr_intaddr;
                    );
                sqlvacm:
                    (
                          va1indi_cm  : tsp00_Int2;
                    va1indva2_cm: tsp00_Int2;
                    va1indva3_cm: tsp00_Int2;
                    va1fill_cm  : tsp00_Int2;
                    va1offs_cm    : tsp00_Int4;
                    va1fill2   : tsp00_Int4;
                    );
                    sqlvast:
                          (
                          va1indi_st : tsp00_Int2;
                    va1cmpcnt_st : tsp00_Int2;
                    va1size_st   : tsp00_Int4;
                    va1addr_st : tpr_vtypepointer;
                    va1ix_st     : tsp00_Int4;
                    );
                    sqlvapt:
                          (
                          va1indi_pt : tsp00_Int2;
                    va1fill3  : tsp00_Int2;
                    va1ix_pt   : tsp00_Int4;
                    va1addr_pt : tpr_vtypepointer;
                    va1fill5  : tpr_intaddr;
                    );
                END;
            (*ENDCASE*)


      sqlva2entry = RECORD      (* 12 B *)
            va2typ   : tsp00_Int2;
            va2digit : tsp00_Int2;
            va2size  : tsp00_Int4;
            va2frac  : tsp00_Int2;
            va2const : tsp00_Int2; (* const flag; if va2const <> 0 then va1addr
            can only be used as input parameter *)
      END;


      sqlva3entry = RECORD        (* 36 B *)
            va3name  : tsp00_Lname;
            va3naml  : tsp00_Int2;        (* vaname length *)
            va3fill: tsp00_Int2;
      END;

      sqlva1type = ARRAY [ 1..mxpr_sqlva1 ] OF sqlva1entry;
      sqlva1p    = ^sqlva1type;
      sqlva2type = ARRAY [ 1..mxpr_sqlva2 ] OF sqlva2entry;
      sqlva2p    = ^sqlva2type;
      sqlva3type = ARRAY [ 1..mxpr_sqlva3 ] OF sqlva3entry;
      sqlva3p    = ^sqlva3type;
      (*********************************************)
      (*        S Q L P A R  neu                   *)
      (* Parameter Area                            *)
      (* loopcount-entry, struct-enty und          *)
      (* Liste, soviele Eintraege wie Summe aller  *)
      (* Parameter in allen SQL-Kommandos des      *)
      (* Hostprogramms                             *)
      (* Eintraege enthalten Verweise auf Host-    *)
      (* variable und Indikatorvariable und Be-    *)
      (* schreibung - fuer Parameter-Uebergabe     *)
      (* zwischen Hostprogramm und SQLDB - der      *)
      (* Hostvariablen                             *)
      (*********************************************)

      sqlparentry = RECORD            (* 12 B *)
            CASE tsp00_Int2 OF  (* parType, loop, struc, element, sqlpl, funcpl *)
                sqlparlo :
                    (  pakindlo   : tsp00_Int2;
                    pava1index : tsp00_Int2;      (* hostvar index :loop *)
                    paloopcnt  : tsp00_Int4;      (* const of loop count *)
                    paloopmin  : tsp00_Int4;  (* minimum of var-arrays cnt*)
                    );
                    sqlparst :
                          (  pakindst    : tsp00_Int2;
                    pavarst     : tsp00_Int2;         (* sqlva1 stindex *)
                    paindst     : tsp00_Int2;         (* sqlva1 stindex *)
                    paelcnt     : tsp00_Int2;
                    pafill4     : tpr_intaddr;
                    );
                sqlparel :
                    (  pakindel : tsp00_Int2;
                    pavarno  : tsp00_Int2;         (* sqlva1 index *)
                    paindno  : tsp00_Int2;         (* sqlva1 index *)
                    );
                END;
            (*ENDCASE*)

      sqlpartype = ARRAY [ 1..mxpr_sqlpa ] OF sqlparentry;
      sqlparpointer = ^sqlpartype;
      (*********************************************)
      (*        S Q L F A                          *)
      (*  Fetch Area                               *)
      (* Eintraege enthalten zur mfetch Abarbeitung*)
      (*     interne Werte                         *)
      (*********************************************)

      sqlfaentry =  RECORD        (* 16 B *)
            faselectno : tsp00_Int2;   (* selectno of mfarea *)
            famfindex  : tsp00_Int2;   (* index into mfarea  *)
            fapos      : tsp00_Int4;   (* pos value of fetch *)
            faspec     : tsp00_Int2;   (* specification fetch*)
            fafetch    : tsp00_Int2;   (* cpr_is_true for fetch stm *)
            fareclen   : tsp00_Int4;   (* output record length  *)
      END;

      sqlfatype = ARRAY [ 1..mxpr_sqlfa ] OF sqlfaentry;
      sqlfapointer  = ^sqlfatype;
      (*********************************************)
      (*        S Q L K A                          *)
      (*  Kommando Area                            *)
      (* Struktur aus Initialisierungs-Flag fuer   *)
      (* Hatzel-Check und einer Liste mit sovielen *)
      (* Eintraegen wie SQL-Kommandos im Hostpro-  *)
      (* gramm eingebettet sind.                   *)
      (* Eintraege enthalten Kommandobeschreibung  *)
      (* und Verweis auf Parameterbeschreibungen   *)
      (*********************************************)

      sqlkaentry =  RECORD       (* 56 B *)
            katyp      : tsp00_Int2;
            kastate    : tsp00_Int2;
            kapacount : tsp00_Int2;      (* sqlpar anzahl   *)
            kapaindex : tsp00_Int2;      (* sqlpar 1. index *)
            kaStcount : tsp00_Int2;      (* sqlst anzahl   *)
            kaStindex : tsp00_Int2;      (* sqlst 1. index *)
            kamacro    : tsp00_Int2;          (* true : enth?altmacros *)
            kaprindex  : tsp00_Int2;      (* prarea-index (prepare)*)
            kadiindex  : tsp00_Int2;      (* dialog-index (dialog) *)
            kaparserrc : tsp00_Int2;      (* pars  error_code      *)
            kaversion  : tsp00_Int4;      (* macro version  *)
            kaParseInfo : tpr00_ParseInfo;
            kaopendata : tsp00_Int2;      (* fetch results from open command *)
            kaparamcnt : tsp00_Int2;      (* interner kasa z?ahler *)
            kalineno   : tsp00_Int2;      (* lineno for profiling  *)
            kafaindex  : tsp00_Int2;      (* faarea index (fetch ) *)
            kaatindex  : tsp00_Int2;      (* atarea index (ora-db) *)
            kacuindex  : tsp00_Int2;      (* cuarea index (cursor, statement) *)
            kamode     : tsp00_Int2;      (* sqldbmode of the current command *)
            kasqlrow   : tpr_sqlrow;    (* reclen info's after parsen *)
            kafae      : ^sqlfaentry;
      END;

      sqlkaentry_ptr = ^sqlkaentry;
      sqlkatype = ARRAY [ 1..mxpr_sqlka ] OF sqlkaentry;
      sqlkapointer  = ^sqlkatype;
      (*********************************************)
      (*        S Q L O R                          *)
      (*  Oracle   Area                            *)
      (* Eintraege enthalten zu jedem Kommando     *)
      (* von Prepare, Open, Fetch  Kommandos       *)
      (* Pointer auf die shortfield infos,         *)
      (* ist open neu geparsed, und beim fetch den *)
      (* kaindex zum zugehoerigen open             *)
      (*********************************************)

      sqlorentry =  RECORD      (* 28 B *)
            orcolcnt   : tsp00_Int4;
            orcollen   : tsp00_Int4;
            orsqlrow   : tpr_sqlrow;    (* reclen info's after parsen *)
            orcolptr   : tpr_sqlcolpointer; (* sqlcol-info pointer  *)
            orcolcntacc: tsp00_Int4;(* acctuelle colcnt belegung      *)
            oropennew  : tsp00_Int2;(*nach:open cpr_is_true,fetch cpr_is_false*)
            (**)
            orrescnt   : tsp00_Int4;    (* max anz result columns *)
            (**)
            orreslen   : tsp00_Int4;
            (**)
            orressqlrow: tpr_sqlrow;  (* res-reclen info's after parsen *)
            (**)
            orresptr   : tpr_sqlcolpointer;  (* rescol-info pointer  *)
            (**)
            orrescntacc: tsp00_Int4; (* acctuelle rescolcnt belegung   *)
            (**)
            orresnamlen: tsp00_Int4; (* max  resnamelen alloc <1Blen,name> *)
            (**)
            orresnamptr: tpr_vtypepointer; (* rescol-names pointer  *)
            (**)
            orresnamacc: tsp00_Int4; (* acctuelle resnamelen belegung   *)
            orresnamencoding: tpr_vtypepointer; (* acctuelle resnamelen belegung   *)
      END;

      (* sqlorentry_ptr = ^sqlorentry; *) (* declared above before tpr_sqlld *)
      sqlortype = ARRAY [ 1..mxpr_sqlka ] OF sqlorentry;
      sqlorpointer  = ^sqlortype;
      (*********************************************)
      (*        S Q L P R                          *)
      (*  Prepare  Area                            *)
      (* Eintraege enthalten Kommandobeschreibung  *)
      (* von Prepare Kommandos  und                *)
      (* von Report  Kommandos                     *)
      (* und Verweis auf Parameterbeschreibungen   *)
      (*********************************************)

      sqlprentry =  RECORD
            prStmtNameIndex   : tsp00_Int2;  (* > 0 statementname in va-area *)
            prCursorNameIndex : tsp00_Int2;  (* > 0 cursorname in va-area *)
            prstate           : tsp00_Int2;
            prStmtName        : tsp00_KnlIdentifier; (*statementname*)
            prkaindex         : tsp00_Int2;  (*sqlka index  *)
            prarea            : tsp00_Int2;  (*sqlva od. sqlst*)
            prDescribe        : tsp00_Int2;
            prCursorName      : tsp00_KnlIdentifier;  (* cursorname *)
            prcuindex         : tsp00_Int2;  (* > 0 cursorname from hostvar *)
            prstcount         : tsp00_Int2;  (* anz sqlst entries *)
            prstindex         : tsp00_Int2;  (* st_count = 0 index into va *)
      END;

      sqlprtype = ARRAY [ 1..mxpr_sqlpr ] OF sqlprentry;
      sqlprpointer  = ^sqlprtype;
      (*********************************************)
      (*        S Q L S T                          *)
      (*  Statement  Area                          *)
      (* Eintraege enthalten Stringkonstanten      *)
      (* von Prepare  Kommandos                    *)
      (*********************************************)

      sqlstentry =  RECORD        (* 56 B *)
            stkano     : tsp00_Int4;
            stllen     : tsp00_Int4;
            stline     : tpr_sqlline;
      END;

      sqlsttype = ARRAY [ 1..mxpr_sqlst ] OF sqlstentry;
      sqlstpointer  = ^sqlsttype;
      (*********************************************)
      (*        S Q L A T                          *)
      (*  AT <dbname> for  ORACLE Area             *)
      (* Eintraege enthalten DBnamen-beschreibung  *)
      (* von Oracle  Kommandos  und                *)
      (* und Verweise auf die gaarea               *)
      (*********************************************)

      sqlatentry =  RECORD      (* 88 B *)
            atdbname   : tsp00_KnlIdentifier;      (* db-name  18B    *)
            atdbindex  : tsp00_Int2;      (* index in sqlva1 *)
            atdbstring : tsp00_NodeId;    (* servernode  64B  *)
            atgaindex  : tsp00_Int2;      (* actt. index into ga-area *)
            atdbnamix  : tsp00_Int2;      (* dbname index in sqlpa    *)
      END;

      sqlattype = ARRAY [ 1..mxpr_sqlat ] OF sqlatentry;
      sqlatpointer  = ^sqlattype;
      (*********************************************)
      (*        S Q L C U                          *)
      (*  Cursor and Statementname   Area  Oracle  *)
      (* Eintraege enthalten Ergn u Stmn  for      *)
      (* multi db   Kommandos                      *)
      (*********************************************)

      sqlcuentry =  RECORD        (* 28 B *)
            cuname     : tsp00_KnlIdentifier;     (* erg or stm-name 18B *)
            cufill     : tsp00_Int2;
            cuatindex  : tsp00_Int2;     (* index into at-area  *)
            cuerrd3    : tsp00_Int4;     (* accumulierter erd3 des cursors *)
            custate    : tsp00_Int2;     (* Status des cursors *)
            cukind     : tsp00_Int2;     (* cursor = 0; statement = 1 *)
            cukaopenind : tsp00_Int2;
            culoopin   : tsp00_Int4;
            culoopout  : tsp00_Int4;
            cusqlindap : sqldaptrvrec;
            cusqloutdap: sqldaptrvrec;
      END;

      sqlcuentry_ptr = ^sqlcuentry;
      sqlcutype = ARRAY [ 1..mxpr_sqlcu ] OF sqlcuentry;
      sqlcupointer  = ^sqlcutype;

      (*********************************************)
      (*        S Q L F N                          *)
      (*  Filename include Area                    *)
      (* Eintraege enthalten kanomin bis kanomax   *)
      (* und filename des include files            *)
      (*********************************************)

      sqlfnentry =  RECORD        (* 24 B *)
            fnkanomin  : tsp00_Int2;  (* min kano in inclfile *)
            fnkanomax  : tsp00_Int2;  (* max kano in inclfile *)
            fnfilen    : tsp00_KnlIdentifier;  (* file name   *)
            fnincllno  : tsp00_Int2;  (* incl lineno *)
      END;

      sqlfntype = ARRAY [ 1..mxpr_sqlfn ] OF sqlfnentry;
      sqlfnpointer  = ^sqlfntype;

      tpr00_OldAreas  =  RECORD
            sqlpap     : sqlparpointer;
            sqlkap     : sqlkapointer;
            sqlprp     : sqlprpointer;
            sqlstp     : sqlstpointer;
            sqlfap     : sqlfapointer;
            sqlatp     : sqlatpointer;
            sqlv1p    : sqlva1p;
            sqlv2p    : sqlva2p;
            sqlv3p    : sqlva3p;
            sqlcup    : sqlcupointer;
            sqlorp    : sqlorpointer;
            sqlfnp    : sqlfnpointer;
      END;

      tpr01_DescTypeEnum     = tsp00_Int4;
      tpr01_SQLStateBinary   = tsp00_Int4;
      tpr01_SQLAttribute     = tsp00_Int2;
      tpr00_ComTypeEnum      = tsp00_Int4;
      tpr00_DescribeKindEnum = tsp00_Int4;
      tpr00_TraceParseID_Enum = tsp00_Int4;
      tpr01_KeywordTypeEnum  = tsp00_Int4;
      tpr_gsymboltypes = tsp00_Int4;

      tpr01_ConDesc =  tsp00_Addr;

      tpr01_SQLDesc =  RECORD
            DescType : tpr01_DescTypeEnum;
            SQL       : tsp00_Addr;
            StmtNameDesc : tsp00_Addr;
            CursorDesc : tsp00_Addr;
            ErrorDesc  : tsp00_Addr;
            ConDesc    : tsp00_Addr;
            ComType : tpr00_ComTypeEnum;
            MessType : tsp1_cmd_mess_type;
            PacketEncoding : tpr05_StringEncoding;
            pr : ^sqlprentry;
            ka : ^sqlkaentry;
            ore : ^sqlorentry;
            cu : ^sqlcuentry;
            sqlxap : tsp00_Addr;
            State : tpr01_SQLStateBinary;
            Attribute : tpr01_SQLAttribute;
      END;

      (*********************************************)
      (*        S Q L C X A  - A R E A             *)
      (*  cxa-area   interne werte call-interf.    *)
      (*********************************************)

      sqlcxatype   =  RECORD   (* 92 B *)
      (* sqlcxa-anf *)
            xainit     : tsp00_Int2;    (* Modulinitalisen = true *)
            xacmdinit  : tsp00_Int2;
            xakano     : tsp00_Int2;    (* actueller sqlka index *)
            (* call-interf commandotype *)
            xaprogn    : tsp00_KnlIdentifier;    (* programm-name *)
            xaprogc    : tsp00_Int2;    (* progcheck   *)
            xamodn     : tsp00_KnlIdentifier;    (* modul-name  *)
            xamodc     : tsp00_Int2;    (* mod-check   *)
            xalang     : tsp00_Int2;    (* nach ra language    *)
            sqlModDesc : tsp00_Addr;
            xaSQLDesc : ^tpr01_SQLDesc;
            xacfetposc : tsp00_Int4;    (* fetch pos  const-value *)
            xaprno     : tsp00_Int2;    (* actuelle prno von kano*)
            xalcheck   : tsp00_Sname;   (* local hatzel_check    *)
            xaprname   : tsp00_KnlIdentifier;    (* statementname *)
            (* call-interf. resultname *)
            xafiller1  : tsp00_Int2;
            xasqldap   : sqldaptrvrec;(* sqlda-area all_kind *)
            xacfetspec : tsp00_Int2;    (* search fetch specification  *)
            xacfiller  : tsp00_C6;
      END;

      sqlcxapointer   = ^sqlcxatype;
      sqlstatetype     = PACKED ARRAY [ 1..6 ]  OF char;
      (**************************************)
      (*        S Q L C A  neu              *)
      (* Communication Area                 *)
      (* Fehlermeldungen von SQLDB an das    *)
      (* Hostprogramm                       *)
      (**************************************)

      sqlcatype =   RECORD         (* 448 B *)
            sqlenv  : tsp00_Int4;      (*  cobol init = 0  *)
            sqlcaid : PACKED ARRAY [1..8] OF char; (* bei Dumpanalyse *)
            sqlcabc  : tsp00_Int4;  (* Laenge der SQLCA, konstant ?? *)
            sqlcode  : tsp00_Int4;  (* SQLDB-Returncode        *)
            sqlerrml : tsp00_Int2;  (* Laenge der Fehlermeldung *)
            sqlerrmc : tsp00_C70;   (* Meldungstext             *)
            sqlerrp  : tsp00_C8;    (* connect_lockmode         *)
            sqlerrd  : ARRAY [ 1..6 ]  OF tsp00_Int4; (* ResCnt(3),Cost *)
            sqlwarn0 : char; (* Anzeige, ob Warnings ges., ggf. 'W' *)
            sqlwarn1 : char; (* alphanumerische Werte abgeschnitten *)
            sqlwarn2 : char; (* Bei Aggregierung NULL-Werte ignor.  *)
            sqlwarn3 : char; (* SELECT-Spal.zahl ungleich Paramzahl *)
            sqlwarn4 : char; (* UPDATE o. DELETE ?uber ganze Tabelle *)
            sqlwarn5 : char;
            sqlwarn6 : char;
            sqlwarn7 : char;
            sqlwarn8 : char;
            sqlwarn9 : char;
            sqlwarna : char;  (* row not found bei verteil. node down*)
            sqlwarnb : char;
            sqlwarnc : char;  (* rowno begrenzung *)
            sqlwarnd : char;
            sqlwarne : char;  (*select next,prev sec.key ?anderung *)
            sqlwarnf : char;  (* create table,index mit ibm syntax *)
            sqlext   : PACKED ARRAY [1..12] OF char; (* hatzel_check *)
            sqlresn : tsp00_KnlIdentifier;    (* resultname after select-comm *)
            sqlfill1   : tsp00_Int2; (* empty                        *)
            sqlmfetch  : tsp00_Int2; (* 0:kein, 1:implicit, 18:option *)
            sqltermref : tsp00_Int4; (* Reference to virtual termial  *)
            sqlfill5   : tsp00_Int2; (* empty                        *)
            sqldbmode  : tsp00_Int2; (* SQLDB=1,ansi=3,saa/db2=2,orac=4*)
            sqldatetime: tsp00_Int2; (* SQLDB-datetime format :0 mormal*)
            sqlstate   : sqlstatetype; (*   6 B    *)
            sqlargl    : tsp4_argline;  (* option runtime argline 132 B*)
            sqlgap   : sqlgapointer;    (* global connect  *)
            sqlrap   : sqlrapointer;    (* runtime auftrag,sql or dia*)
            sqloap   : sqloapointer;    (* runtime optionen-area *)
            sqlmap   : sqlmapointer;    (* macro, varlang  *)
            sqlmfp   : sqlmfpointer;    (* mfetch-area     *)
            sqlplp   : tpr_intaddr;     (* empty           *)
            sqlemp   : sqlempointer;    (* tpr_sqlerror pointer *)
            sqlxadesc: tpr_sqlmdesc;
            sqlcxap   : sqlcxapointer;   (* pointer to sqlcxa *)
            sqlEnvDesc : tsp00_Addr;
      END;

      (*********************************************)
      (*        S Q L X A                          *)
      (*  Extent   Area                            *)
      (* Struktur aus Initialisierungs-Flag fuer   *)
      (* Hatzel-Check und modul globale Angaben    *)
      (* Pointer auf alle SQL-Areas                *)
      (*********************************************)

      sqlxatype = RECORD     (* 236 B *)
      (* sqlxa *)
            xainit     : tsp00_Int2;        (* Modulinitalisen = true *)
            xacmdinit  : tsp00_Int2;
            xakano     : tsp00_Int2;        (* actueller sqlka index *)
            (* call-interf commandotype *)
            xaprogn    : tsp00_KnlIdentifier;        (* programm-name *)
            xaprogc    : tsp00_Int2;        (* progcheck   *)
            xamodn     : tsp00_KnlIdentifier;        (* modul-name  *)
            xamodc     : tsp00_Int2;        (* mod-check   *)
            xalang     : tsp00_Int2;        (* nach ra language    *)
            xaModDesc : tsp00_Addr;
            xaSQLDesc : ^tpr01_SQLDesc;
            xacfetposc : tsp00_Int4;    (* fetch pos  const-value *)
            xaprno     : tsp00_Int2;         (* actuelle prno von kano*)
            xalcheck   : tsp00_Sname;        (* local hatzel_check    *)
            xaprname   : tsp00_KnlIdentifier;         (* statementname *)
            (* call-interf. resultn  *)
            xastopcnt  : tsp00_Int2;         (* anz loop bei error -1 *)
            (* anz klammern precomp. *)
            xallen     : tsp00_Int2;         (* line length *)
            xaline     : tpr_sqlline;      (* line  52 B  *)
            xafiller2  : tsp00_Int2;
            xatime     : tsp00_Int4;         (* pc ret_syposacc*)
            xadescpos  : tsp00_Int2;         (* descrip.pos in varpart *)
            xadesclen  : tsp00_Int2;         (* descrip.len in varpart *)
            xaatcount : tsp00_Int2;          (* nur precompiler *)
            xaatmax   : tsp00_Int2;          (* max sqlatentrys, runtime *)
            xafiller3  : tsp00_Int4;
            sqlpap     : sqlparpointer;    (* parameter *)
            sqlpae     : sqlparpointer;    (* parameter *)
            sqlkap     : sqlkapointer;     (* kommando  *)
            sqlkae     : sqlkapointer;     (* kommando  *)
            sqlprp     : sqlprpointer;     (* prepare   *)
            sqlpre     : sqlprpointer;     (* prepare   *)
            sqlstp     : sqlstpointer;     (* statement *)
            sqlste     : sqlstpointer;     (* statement *)
            sqlfap     : sqlfapointer;     (* fetch     *)
            sqlfae     : sqlfapointer;     (* fetch     *)
            sqlatp     : sqlatpointer;     (* sqlat-area for oracle *)
            sqlate     : sqlatpointer;     (* sqlat-area for oracle *)
            sqlv1p    : sqlva1p;           (* pointer to sqlva1-area *)
            sqlv1e    : sqlva1p;           (* pointer to sqlva1-area *)
            sqlv2p    : sqlva2p;           (* pointer to sqlva2-area *)
            sqlv2e    : sqlva2p;           (* pointer to sqlva2-area *)
            sqlv3p    : sqlva3p;           (* pointer to sqlva3-area *)
            sqlv3e    : sqlva3p;           (* pointer to sqlva3-area *)
            sqlfill1  : tpr_intaddr;       (* unused *)
            sqlfill2  : tpr_intaddr;       (* unused *)
            sqlfill3  : tpr_intaddr;       (* unused *)
            sqlfill4  : tpr_intaddr;       (* unused *)
            sqlcup    : sqlcupointer;      (* sqlcuarea for oracle *)
            sqlcue    : sqlcupointer;      (* sqlcuarea for oracle *)
            sqlorp    : sqlorpointer;      (* sqlorarea for oracle *)
            sqlore    : sqlorpointer;      (* sqlorarea for oracle *)
            sqlfnp    : sqlfnpointer;      (* pointer to sqlfn-area *)
            sqlfne    : sqlfnpointer;      (* pointer to sqlfn-area *)
            sqlsnp    : tpr_sqlsn;     (* pointer to sqlsnarea *)
            sqlsndesc : tpr_sqlmdesc;      (* sqlsn-descriptor pointer *)
      END;

      (*********************************************)
      (*        LOOPRECORD                         *)
      (*  For sql array commands                   *)
      (* Structur to send several commands         *)
      (* to then kernel                            *)
      (*********************************************)

      tpr_sqlloop   = RECORD       (* 40 B *)
            lpmax   : tsp00_Int4;  (* maximale array count *)
            lpcnt   : tsp00_Int4;  (* max.actuell arr.count*)
            lperrd3 : tsp00_Int4;  (* max. sqlerrd 3 count *)
            lpnext  : tsp00_Int2;  (* cpr_is_true send com.to kern*)
            lpfiller: tsp00_Int2;  (*   empty              *)
            lperrd3i  : tsp00_Int4;  (* input acumulierte sqlerrd[3] *)
            lprescount: tsp00_Int4; (* only select into array  *)
            lpindi  : tsp00_Int4;  (* actuell input  index *)
            lpindo  : tsp00_Int4;  (* actuell output index *)
            lpfirsti: tsp00_Int2;  (* first input  call    *)
            lpfirsto: tsp00_Int2;  (* first output call    *)
            lprecli : tsp00_Int4;  (* recordlength input   *)
            lpreclo : tsp00_Int4;  (* recordlength output  *)
            lperri  : tpr_runtime_errors; (* error input     *)
            lperro  : tpr_runtime_errors; (* error output    *)
      END;

      (************************************************)
      (*   T Y P E N  des C A L L - I N T E R F A C E *)
      (*     beginnen mit sqc                         *)
      (************************************************)
      tpr_char_array = char;      (* 1.Byte eines char-arrays *)
      tpr_char_arrayp = ^tpr_char_array;
      tpr_string = PACKED ARRAY [ 1..2 ]  OF char;
      tpr05_CharBuffer = char;

      tpr05_String = RECORD
            rawString    : ^tpr05_CharBuffer;  (* contains the SQL-Statement *)
            encodingType : ^tsp00_Addr;          (* encoding type (ASCII, UCS2...) of szSQLStatement*)
            cbLen        : tsp00_Int4;         (* length of szSQLStatement in Byte *)
            cbAllocLen   : tsp00_Int4;         (* allocated length for szSQLStatement in Byte *)
      END;

      tpr05_StringPointer = ^tpr05_String;

      tpr05_StmtAnalysisDesc = RECORD
           StmtType   : tpr01_KeywordTypeEnum; (*first keyword of SQL-Statement (SELECT, UPDATE ...)*)
           StmtOption : tpr01_KeywordTypeEnum; (*option for a SQL-Statement (NEXT, FIRST ...)*)
           CursorPos  : tsp00_Int4;            (*position of hostvariable which contains cursorname*)
           CursorLength : tsp00_Int4;          (*length of hostvariable which contains cursorname*)
           CursorType : tpr_gsymboltypes;      (*TYPE of symbol where cursor come from (paramter, identifier...)*)
           FetchPosType : tpr_gsymboltypes;    (*TYPE of symbol (paramter, identifier...) which contains parameter for fetch [pos|relative|absolute...]*)
           UsingPos : tsp00_Int4;               (*position of using-clause in SQL-Statement*)
      END;
