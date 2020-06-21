

CONST

      (* look up in vsp00 *)
      blank              = ' ';
      blankname          = '                  ';
      maxnamelength      = 18 ;
 

      (* only used in cct45 *)
      c45_maxtokenlength  = 8 ;
      c45_maxerrtlength   = 60;
      maxlinelength       = 132;
 
      (* Predefined symbols *)
      c45_c_machine       = 'MACH    ' ;
      c45_c_kind          = 'KIND    ' ;
      c45_c_trace         = 'TRACE   ' ;
      c45_c_compiler      = 'COMP    ' ;
      c45_c_osystem       = 'OS      ' ;
 

      c_semy             = ';' ;
      c_dollar           = '$' ;
      c_directive        = '&' ;
      error_line_count   = '* LNO : ' ;
      blank_char3        = '   ';
      blank_token        = '        ' ;
      blankerror         =
              '                                                            ' ;
      semy               = 59;
      max_cond           = 15 ;
      max_syms           = 15 ;
 
      f_ill_nest           = 'Illegal nested : &if-&else-&endif       ' ;
      f_ill_if_num         = 'Too many "&if" statements               ' ;
      f_ill_endif_num      = 'Too many "&endif" statements            ' ;
      f_ill_opera          = 'Illegal operator in "&" statement       ' ;
      f_ill_beg_dilim      = 'Illegal start dilimiter                 ' ;
      f_ill_end_dilim      = 'Illegal end dilimiter                   ' ;
      f_ill_and_opera      = 'Unknown AND operator                    ' ;
      f_ill_set_opera      = 'Unknown SET operator                    ' ;
      f_miss_opera         = 'Missing operator in "&if" expression    ' ;
      f_miss_val           = 'Missing value in "&if" expression       ' ;
      f_inv_token          = 'Undefined symbol in "&" expression      ' ;
      f_undef_sym          = 'Undefined symbol in "$" expression      ' ;
      f_ill_undef_inst     = 'Illegal "&undef" parameter              ' ;
      f_ill_def_inst       = 'Illegal "&def" parameter                ' ;
      f_ill_def_undef_inst = 'Illegal "&def" or "&undef" parameter    ' ;
      f_value_not_found    = 'Value not found                         ' ;
      f_too_many_symb      = 'Symbol tabele overflow !!               ' ;
      f_too_few_symb       = 'Symbol tabele underflow !!              ' ;
      f_no_symb_declared   = 'No symbol name defined                  ' ;
      f_internal_error_1   = 'INTERNAL ERROR  1  !!!!!!!!!!!!!        ' ;
 
TYPE

longint=integer;
shortint=-32768..32767;
double=real;

      (* Targon/35 *)
      c45_int4      = longint ;
 
      c45_token_typ = PACKED ARRAY [ 1..c45_maxtokenlength ] OF char ;
      c45_errortext = PACKED ARRAY [ 1..c45_maxerrtlength ] OF char;


      (* look up in vsp00 *)
      line          = PACKED ARRAY [ 1..maxlinelength ] OF char;
      name          = PACKED ARRAY [ 1..maxnamelength ] OF char;
 

      char2       = PACKED ARRAY [ 1..2 ] OF char;
      char3       = PACKED ARRAY [ 1..3 ] OF char;
      shortstring = PACKED ARRAY [ 1..40 ] OF char;
 
      dilim       = SET OF char;
 
      d_type      = ( d_none , d_define , d_undef , d_ifdef , d_ifndef ,
                      d_if , d_else , d_endif , d_err ) ;
 
      one_cond    = RECORD
                      co_visible , co_suppress , co_else : boolean ;
                    END;
 
      cond_list   = ARRAY [ 1 .. max_cond ] OF one_cond ;
 
      line_desc   = RECORD
		      llen : c45_int4 ;
		      iol  : line ;
                    END;
 
      one_sym     = RECORD
                      s_name , s_value : c45_token_typ ;
                    END;
 
      sym_list    = ARRAY [ 1 .. max_syms ] OF one_sym ;
 
VAR

      c45_dilimiter     : dilim ;
      c45_in_dilimit    : dilim ;
      c45_cond_stack    : cond_list ;
      c45_num_cond      : integer ;
      c45_num_syms      : integer ;
      c45_all_syms      : sym_list ;
      c45_line_suppress : boolean ;
      c45_line_count    : c45_int4 ;
      c45_error_text    : c45_errortext ;


 
(*------------------------------*)
 
PROCEDURE
      c45_init_cond ;
        FORWARD;

PROCEDURE
      c45_store_symbols ( VAR symbol_name  : c45_token_typ ;
                          VAR symbol_value : c45_token_typ ;
                          VAR symbol_count : integer ;
                          VAR error        : c45_errortext );
        FORWARD;

PROCEDURE
      c45_delete_symbols ( VAR symbol_name  : c45_token_typ ;
                           VAR symbol_count : integer ;
                           VAR error        : c45_errortext );
        FORWARD;

PROCEDURE
      c45_do_cond_directive ( VAR dir_line : line ;
                              VAR length   : c45_int4  ;
                              VAR error    : c45_errortext );
        FORWARD;

PROCEDURE
      cond_if (     input_line : line_desc ;
                VAR dir_pos    : c45_int4 ;
                VAR suppress   : boolean ;
                    directive  : d_type ;
                VAR err        : boolean );
        FORWARD;

PROCEDURE
      cond_else (     input_line : line_desc ;
                  VAR dir_pos    : c45_int4 ;
                  VAR suppress   : boolean ;
                  VAR err        : boolean );
        FORWARD;

PROCEDURE
      cond_endif (     input_line : line_desc ;
                   VAR dir_pos    : c45_int4 ;
                   VAR suppress   : boolean ;
                   VAR err        : boolean );
        FORWARD;

PROCEDURE
      con_if_eval (     input_line : line_desc ;
                    VAR dir_pos    : c45_int4 ;
                    VAR suppress   : boolean ;
                    VAR err        : boolean );
        FORWARD;

PROCEDURE
      part_con_if_eval (     input_line : line_desc ;
                         VAR dir_pos    : c45_int4 ;
                         VAR suppress   : boolean ;
                         VAR err        : boolean );
        FORWARD;

PROCEDURE
      con_def_eval (     input_line : line_desc ;
                     VAR dir_pos    : c45_int4 ;
                     VAR suppress   : boolean ;
                     VAR err        : boolean );
        FORWARD;

PROCEDURE
      con_ndef_eval (     input_line : line_desc ;
                      VAR dir_pos : c45_int4 ;
                      VAR suppress : boolean ;
                      VAR err      : boolean );
        FORWARD;

PROCEDURE
      cond_define (     input_line : line_desc ;
                    VAR dir_pos    : c45_int4 ;
                    VAR suppress   : boolean ;
                        directive  : d_type ;
                    VAR err        : boolean  );
        FORWARD;

PROCEDURE
      cond_err ( VAR suppress : boolean ;
                 VAR err      : boolean );
        FORWARD;

PROCEDURE
      name_to_token (     tmp_nam : name ;
                      VAR tmp_sym : c45_token_typ ;
                      VAR err     : boolean       );
        FORWARD;

PROCEDURE
      name_to_dir (tmp_nam : name ; VAR directive : d_type ;
                   inln : line_desc ; VAR dir_pos : c45_int4 );
        FORWARD;

PROCEDURE
      fetch_val (     input_line : line_desc ;
                  VAR dir_pos    : c45_int4 ;
                  VAR suppress   : boolean ;
                  VAR val        : c45_token_typ ;
                  VAR found      : boolean ;
                  VAR err        : boolean );
        FORWARD;

PROCEDURE
      fetch_in_val (     input_line  : line_desc ;
                     VAR dir_pos     : c45_int4 ;
                         suppress    : boolean ;
                     VAR val         : c45_token_typ ;
                     VAR value_found : boolean ;
                     VAR dilimit     : char2 ;
                     VAR dili_found  : boolean ;
                     VAR err         : boolean );
        FORWARD;

PROCEDURE
      get_first_name (inln : line_desc ; VAR pos : c45_int4 ;
		  VAR nam : name );
        FORWARD;

PROCEDURE
      get_name (inln : line_desc ; VAR pos : c45_int4 ;
		  VAR nam : name );
        FORWARD;

PROCEDURE
      get_beg_end_dilimiter (    inln       : line_desc ;
			     VAR pos        : c45_int4 ;
                             VAR begin_dili : char2 ;
                             VAR end_dili   : char2 ;
                             VAR dilimit_ok : boolean );
        FORWARD;

PROCEDURE
      wr_error ( contents : shortstring ;
                 print_nr : boolean );
        FORWARD;

PROCEDURE
      store_sym (     sym_name : c45_token_typ ;
                      sym_val  : c45_token_typ ;
                  VAR err : boolean );
        FORWARD;

PROCEDURE
      delete_sym (     sym_name : c45_token_typ ;
                   VAR err      : boolean );
        FORWARD;

PROCEDURE
      fetch_sym ( sym_name : c45_token_typ ; VAR found : boolean ;
                  VAR sym_val : c45_token_typ );
        FORWARD;

PROCEDURE
      find_sym ( sym_name : c45_token_typ ; VAR index : integer );
        FORWARD;

FUNCTION
      uppercase (c : char): char;
        FORWARD;

FUNCTION
      c45_suppress_on : boolean;
        FORWARD;

PROCEDURE
        c45_init_cond ;


 
VAR
      empty_sym : one_sym ;
      i         : integer ;
 
BEGIN


c45_line_suppress := false ;
c45_line_count := 0 ;
WITH empty_sym DO
     BEGIN
     s_name := blank_token ;
     s_value := blank_token ;
     END ;
(*ENDWITH*)
FOR i := 1 TO max_syms DO
    c45_all_syms [ i ] := empty_sym ;
(*ENDFOR*)
c45_num_syms := 0 ;
c45_dilimiter :=  [ '(', ')', ',', '.', ':', '[',
                    ']', '$', '*', ' ', '=', chr (semy )] ;
c45_in_dilimit := [ '[' , '(' , '.' , ')' , ']' ] ;
c45_num_cond := 0 ;
END ;
 
(*------------------------------*)
 
PROCEDURE
        c45_store_symbols ;


 
VAR
      i           : integer ;
      err         : boolean ;
      upp_s_name  : c45_token_typ ;
      upp_s_value : c45_token_typ ;
 
BEGIN


error := blankerror ;
IF   ( symbol_name <> blank_token )
THEN
     BEGIN
     IF   c45_num_syms < max_syms
     THEN
          BEGIN
          upp_s_name := blank_token ;
          upp_s_value := blank_token ;
          FOR i := 1 TO c45_maxtokenlength DO
              BEGIN
              upp_s_name [ i ] := uppercase ( symbol_name [ i ] ) ;
              upp_s_value [ i ] := uppercase ( symbol_value [ i ] ) ;
              END ;
          (*ENDFOR*)


          store_sym ( upp_s_name , upp_s_value , err ) ;
          symbol_count := c45_num_syms ;
          END
     ELSE
          BEGIN
          wr_error ( f_too_many_symb , false ) ;
          error := c45_error_text ;
          symbol_count := - 1 ;
          END ;
     (*ENDIF*)
     END
ELSE
     BEGIN
     wr_error ( f_no_symb_declared , false ) ;
     symbol_count := - 1 ;
     error := c45_error_text ;
     END ;
(*ENDIF*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        c45_delete_symbols ;


 
VAR
      i          : integer ;
      err        : boolean ;
      upp_s_name : c45_token_typ ;
 
BEGIN


error := blankerror ;
IF   ( symbol_name <> blank_token )
THEN
     BEGIN
     IF   c45_num_syms <= 0
     THEN
          BEGIN
          wr_error ( f_too_few_symb , false ) ;
          error := c45_error_text ;
          symbol_count := - 1 ;
          END
     ELSE
          BEGIN
          upp_s_name := blank_token ;
          FOR i := 1 TO c45_maxtokenlength DO
              BEGIN
              upp_s_name [ i ] := uppercase ( symbol_name [ i ] ) ;
              END ;
          (*ENDFOR*)


          delete_sym ( upp_s_name , err ) ;
          symbol_count := c45_num_syms ;
          END ;
     (*ENDIF*)
     END
ELSE
     BEGIN
     wr_error ( f_no_symb_declared , false ) ;
     symbol_count := - 1 ;
     error := c45_error_text ;
     END ;
(*ENDIF*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        c45_do_cond_directive ;


 
VAR
      tmp_len   : c45_int4 ;
      dir_pos   : c45_int4 ;
      dir_name  : name ;
      directive : d_type ;
      inln      : line_desc ;
      err       : boolean ;
      suppress  : boolean ;


 
BEGIN


err := false ;
error := blankerror ;
c45_line_count := succ ( c45_line_count ) ;
dir_pos := 1 ;
inln.iol := dir_line ;
inln.llen := length ;
tmp_len := length ;
get_first_name ( inln , dir_pos , dir_name );
name_to_dir ( dir_name , directive , inln , dir_pos );
CASE directive OF
     d_none :
          BEGIN


          IF   c45_line_suppress
          THEN
               length := 0
          ELSE
               length := tmp_len ;
          (*ENDIF*)
          END ;
     d_err :
          BEGIN
          cond_err ( suppress , err );
          c45_line_suppress := suppress ;
          error := c45_error_text ;
          length := - tmp_len ;
          END ;
     d_define , d_undef :
          BEGIN
          cond_define ( inln , dir_pos , suppress , directive , err );
          c45_line_suppress := suppress ;
          length := 0 ;


          IF   err
          THEN
               BEGIN
               length := - tmp_len ;
               error := c45_error_text ;
               END ;
          (*ENDIF*)
          END ;
     d_ifdef , d_ifndef , d_if :
          BEGIN
          cond_if ( inln , dir_pos , suppress , directive , err );
          c45_line_suppress := suppress ;
          length := 0 ;


          IF   err
          THEN
               BEGIN
               length := - tmp_len ;
               error := c45_error_text ;
               END ;
          (*ENDIF*)
          END ;
     d_else :
          BEGIN
          cond_else ( inln , dir_pos , suppress , err );
          c45_line_suppress := suppress ;
          length := 0 ;


          IF   err
          THEN
               BEGIN
               length := - tmp_len ;
               error := c45_error_text ;
               END ;
          (*ENDIF*)
          END ;
     d_endif :
          BEGIN
          cond_endif ( inln , dir_pos , suppress , err );
          c45_line_suppress := suppress ;
          length := 0 ;


          IF   err
          THEN
               BEGIN
               length := - tmp_len ;
               error := c45_error_text ;
               END ;
          (*ENDIF*)
          END ;
     END ;
(*ENDCASE*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        cond_if ;


 
VAR
      visible : boolean ;
      tmp_res : boolean ;


 
BEGIN
err := false ;
IF   c45_num_cond = 0
THEN
     visible := true
ELSE
     visible := NOT c45_cond_stack [ c45_num_cond ] . co_suppress ;
(*ENDIF*)


IF   c45_num_cond < max_cond
THEN
     BEGIN
     c45_num_cond := succ ( c45_num_cond );
     WITH c45_cond_stack [ c45_num_cond ] DO
          BEGIN
          co_visible := visible ;
          co_suppress := NOT visible ;
          tmp_res := NOT visible ; (* entspricht dem "suppress" *)
               CASE directive OF
                    d_if :
                         con_if_eval (input_line , dir_pos , tmp_res , err );
                    d_ifdef :
                         con_def_eval (input_line , dir_pos , tmp_res , err );
                    d_ifndef :
                         con_ndef_eval (input_line , dir_pos , tmp_res , err );
                    OTHERWISE :
                         BEGIN
                         wr_error ( f_internal_error_1 , true ) ;
                         err := true ;
                         END ;
                    END ;
               (*ENDCASE*)
          IF   visible
          THEN
               co_suppress := tmp_res ;
          (*ENDIF*)
          co_else := false ;
          suppress := co_suppress ;
          END
     (*ENDWITH*)
     END
ELSE
     BEGIN
     err := true ;
     wr_error ( f_ill_if_num , true );
     suppress := true ;
     END ;
(*ENDIF*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        cond_else ;


 


 
BEGIN
err := false ;
IF   c45_num_cond > 0
THEN
     WITH c45_cond_stack [ c45_num_cond ] DO
          IF   co_else
          THEN
               BEGIN
               err := true ;
               wr_error ( f_ill_nest ,true ) ;
               suppress := true ;
               END
          ELSE
               BEGIN
               co_else := true ;
               IF   co_visible
               THEN
                    co_suppress := NOT co_suppress ;
               (*ENDIF*)
               suppress := co_suppress ;
               END
          (*ENDIF*)
     (*ENDWITH*)
ELSE
     BEGIN
     err := true ;
     wr_error ( f_ill_nest ,true );
     suppress := true ;
     END ;
(*ENDIF*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        cond_endif ;


 


 
BEGIN
err := false ;
suppress := false ;
IF   c45_num_cond > 0
THEN
     BEGIN
     c45_num_cond := pred ( c45_num_cond );
     IF   c45_num_cond > 0
     THEN
          suppress := c45_cond_stack [ c45_num_cond ] . co_suppress ;
     (*ENDIF*)
     END
ELSE
     BEGIN
     err := true ;
     wr_error ( f_ill_endif_num ,true );
     suppress := true ;
     END ;
(*ENDIF*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        con_if_eval ;


 
VAR
      operator     : char ;
      and_operator : char3 ;
      and_ok       : boolean;
 
BEGIN
err := false ;
and_ok := true ;
WHILE ( NOT suppress ) AND ( and_ok ) DO
      BEGIN
      part_con_if_eval (input_line , dir_pos , suppress , err );


      IF   (( NOT suppress ) AND ( dir_pos < input_line.llen - 3 ))
      THEN
           BEGIN
           REPEAT
		  operator := input_line . iol [dir_pos ];
                  dir_pos := succ (dir_pos );
           UNTIL
		 (operator <> blank ) OR (dir_pos > input_line . llen );
           (*ENDREPEAT*)
           IF   ( operator = 'A' ) OR ( operator = 'a' )
           THEN
                BEGIN
                and_operator := blank_char3 ;
                and_operator [ 1 ] := operator ;
		and_operator [ 2 ] := input_line . iol [ dir_pos ] ;
		and_operator [ 3 ] := input_line . iol [ dir_pos + 1 ] ;
                dir_pos := dir_pos + 2 ;
                IF   ( and_operator = 'AND' ) OR ( and_operator = 'and' )
                THEN
                     and_ok := true
                ELSE
                     BEGIN
                     and_ok := false ;
                     err := true ;
                     wr_error ( f_ill_and_opera , true );
                     suppress := true ;
                     END ;
                (*ENDIF*)
                END
           ELSE
                BEGIN
                and_ok := false ;
                END ;
           (*ENDIF*)
           END
      ELSE
           BEGIN
           and_ok := false ;
           END ;
      (*ENDIF*)


      END ;
(*ENDWHILE*)
END ;
 
(*------------------------------*)
 
PROCEDURE
        part_con_if_eval ;


 
VAR
      operator     : char ;
      in_operator  : char2 ;
      beg_dilim    : char2 ;
      end_dilim    : char2 ;
      dili         : char2 ;
      val_found    : boolean ;
      is_eval      : boolean ;
      in_val_ok    : boolean ;
      dilim_found  : boolean;
      sym_l_val    : c45_token_typ ;
      sym_r_val    : c45_token_typ ;
 
BEGIN
err := false ;
fetch_val ( input_line , dir_pos , suppress , sym_l_val , val_found , err );


IF   val_found
THEN
     BEGIN
     REPEAT
	    operator := input_line . iol [dir_pos ];
            dir_pos := succ (dir_pos );
     UNTIL
	   (operator <> blank ) OR (dir_pos > input_line . llen );
     (*ENDREPEAT*)
     IF   operator <> blank
     THEN
          BEGIN
          IF   operator = '='
          THEN
               BEGIN
               fetch_val ( input_line , dir_pos , suppress ,
                        sym_r_val , val_found , err );


               IF   val_found
               THEN
                    suppress := NOT (sym_l_val = sym_r_val );
               (*ENDIF*)


               END
          ELSE
               BEGIN
               IF   ( operator = 'I' ) OR ( operator = 'i' )
               THEN
                    BEGIN
                    in_operator [ 1 ] := operator ;
		    in_operator [ 2 ] := input_line . iol [ dir_pos ] ;
                    dir_pos := succ ( dir_pos ) ;


                    IF   ( in_operator = 'IN' ) OR ( in_operator = 'in' )
                    THEN
                         BEGIN
                         get_beg_end_dilimiter ( input_line , dir_pos ,
                                        beg_dilim , end_dilim , dilim_found );


                         IF   dilim_found
                         THEN
                              BEGIN
                              IF end_dilim <> '**'
                              THEN
                                   BEGIN
                                   in_val_ok := false ;
                                   is_eval := false ;
                                   REPEAT
                                          fetch_in_val ( input_line , dir_pos ,
                                                         suppress , sym_r_val , val_found ,
                                                         dili , dilim_found ,
                                                         err ) ;


                                          IF   ( val_found ) AND ( NOT is_eval )
                                          THEN
                                               BEGIN
                                               in_val_ok := true ;
                                               IF sym_l_val = sym_r_val
                                               THEN
                                                    is_eval := true ;
                                               (*ENDIF*)
                                               END ;
                                          (*ENDIF*)
                                   UNTIL
                                         ( dili = end_dilim ) OR
					 ( dir_pos > input_line.llen ) ;
                                   (*ENDREPEAT*)
                                   suppress := NOT is_eval ;
                                   IF   ( dili <> end_dilim )
                                   THEN
                                        BEGIN
                                        err := true ;
                                        wr_error ( f_ill_end_dilim , true );
                                        suppress := true ;
                                        END ;
                                   (*ENDIF*)
                                   IF   ( NOT in_val_ok )
                                   THEN
                                        BEGIN
                                        err := true ;
                                        wr_error ( f_miss_val , true );
                                        suppress := true ;
                                        END ;
                                   END
                              ELSE
                                   BEGIN
                                   err := true ;
                                   wr_error ( f_ill_beg_dilim , true ) ;
                                   suppress := true ;
                                   END ;
                              (*ENDIF*)
                              END
                         ELSE
                              BEGIN
                              err := true ;
                              wr_error ( f_ill_beg_dilim , true );
                              suppress := true ;
                              END ;
                         (*ENDIF*)
                         END
                    ELSE
                         BEGIN
                         err := true ;
                         wr_error ( f_ill_set_opera , true );
                         suppress := true ;
                         END ;
                    (*ENDIF*)
                    END
               ELSE
                    BEGIN
                    err := true ;
                    wr_error ( f_ill_opera , true );
                    suppress := true ;
                    END ;
               (*ENDIF*)
               END ;
          (*ENDIF*)
          END
     ELSE
          BEGIN
          err := true ;
          wr_error ( f_miss_opera , true );
          suppress := true ;
          END ;
     (*ENDIF*)
     END ;
(*ENDIF*)
END ;
 
(*------------------------------*)
 
PROCEDURE
        con_def_eval ;


 
VAR
      i           : integer ;
      sym_defined : boolean ;
      tmp_name    : name ;
      sym_name    : c45_token_typ ;
      sym_val     : c45_token_typ ;
 
BEGIN
get_name (input_line , dir_pos , tmp_name );
name_to_token (tmp_name , sym_name , err );
fetch_sym (sym_name , sym_defined , sym_val );
suppress := NOT sym_defined ;
END ;
 
(*------------------------------*)
 
PROCEDURE
        con_ndef_eval ;


 
VAR
      i : integer ;
      sym_defined : boolean ;
      tmp_name : name ;
      sym_name , sym_val : c45_token_typ ;
 
BEGIN
get_name (input_line , dir_pos , tmp_name );
name_to_token (tmp_name , sym_name , err );
fetch_sym (sym_name , sym_defined , sym_val );
suppress := sym_defined ;
END ;
 
(*------------------------------*)
 
PROCEDURE
        cond_define ;


 
VAR
      i           : integer ;
      tmp_char    : char ;
      sym_defined : boolean ;
      val_ok      : boolean ;
      tmp_name    : name ;
      sym_l_name  : c45_token_typ ;
      sym_tmp     : c45_token_typ ;
      sym_r_val   : c45_token_typ ;
 
BEGIN
err := false ;
val_ok := true ;
IF   c45_num_cond = 0
THEN
     suppress := false
ELSE
     suppress := c45_cond_stack [ c45_num_cond ] . co_suppress ;
(*ENDIF*)


IF   NOT suppress
THEN
     BEGIN
     get_name (input_line , dir_pos , tmp_name );
     name_to_token (tmp_name , sym_l_name , err );


     IF   ( sym_l_name <> blank_token )    AND
          ( sym_l_name <> c45_c_machine )  AND
          ( sym_l_name <> c45_c_compiler ) AND
          ( sym_l_name <> c45_c_osystem )
     THEN
          BEGIN
          REPEAT
		 tmp_char := input_line . iol [dir_pos ];
                 dir_pos := succ (dir_pos );
          UNTIL
		(tmp_char <> blank ) OR (dir_pos > input_line . llen );
          (*ENDREPEAT*)
          IF   directive = d_define
          THEN
               BEGIN
               IF   tmp_char = c_dollar
               THEN
                    BEGIN
                    get_name (input_line , dir_pos , tmp_name );
                    name_to_token (tmp_name , sym_tmp , err );
                    fetch_sym (sym_tmp , sym_defined , sym_r_val );
                    IF   (NOT sym_defined ) AND (NOT suppress )
                    THEN
                         BEGIN
                         err := true ;
                         wr_error ( f_ill_def_inst , true );
                         val_ok := false ;
                         END ;
                    (*ENDIF*)
                    END
               ELSE
                    BEGIN
                    dir_pos := pred (dir_pos );
                    get_name (input_line , dir_pos , tmp_name );
                    name_to_token (tmp_name , sym_r_val , err );
                    IF   ( sym_r_val = blank_token )
                    THEN
                         BEGIN
                         err := true ;
                         wr_error ( f_ill_def_inst , true );
                         val_ok := false ;
                         END ;
                    (*ENDIF*)
                    END ;
               (*ENDIF*)


               IF   val_ok
               THEN
                    store_sym ( sym_l_name , sym_r_val , err );
               (*ENDIF*)
               END
          ELSE
               delete_sym ( sym_l_name , err );
          (*ENDIF*)
          END
     ELSE
          BEGIN
          err := true ;
          wr_error ( f_ill_def_undef_inst , true );
          END ;
     (*ENDIF*)
     END ;
(*ENDIF*)
END ;
 
(*------------------------------*)
 
PROCEDURE
        cond_err ;


 
BEGIN
IF   c45_num_cond = 0
THEN
     suppress := false
ELSE
     suppress := c45_cond_stack [ c45_num_cond ] . co_suppress ;
(*ENDIF*)


err := true ;
wr_error ( f_ill_opera , true );
END ;
 
(*------------------------------*)
 
PROCEDURE
        name_to_token ;


 
VAR
      i : integer ;
 
BEGIN
err := false ;
FOR i := 1 TO c45_maxtokenlength DO
    tmp_sym [i ] := tmp_nam [i ];
(*ENDFOR*)
IF   tmp_nam [succ (c45_maxtokenlength )] <> blank
THEN
     BEGIN
     err := true ;
     wr_error ( f_inv_token , true );
     END ;
(*ENDIF*)
END ;
 
(*------------------------------*)
 
PROCEDURE
        name_to_dir ;


 
VAR
      i         : integer ;
      dir_name  : name ;
 
BEGIN


IF   tmp_nam [ 1 ] = '&'
THEN
     IF   tmp_nam = '&                 '
     THEN
          BEGIN
          get_name ( inln , dir_pos , dir_name );


	  IF   dir_name = 'ENDIF             '
          THEN
               directive := d_endif
          ELSE
	       IF   dir_name = 'IF                '
               THEN
                    directive := d_if
               ELSE
		    IF   dir_name = 'IFDEF             '
                    THEN
                         directive := d_ifdef
                    ELSE
			 IF   dir_name = 'IFNDEF            '
                         THEN
                              directive := d_ifndef
                         ELSE
			      IF   dir_name = 'ELSE              '
                              THEN
                                   directive := d_else
                              ELSE
				   IF   dir_name = 'DEFINE            '
                                   THEN
                                        directive := d_define
                                   ELSE
					IF   dir_name = 'UNDEF             '
                                        THEN
                                             directive := d_undef
                                        ELSE
                                             directive := d_err ;
                                        (*ENDIF*)
                                   (*ENDIF*)
                              (*ENDIF*)
                         (*ENDIF*)
                    (*ENDIF*)
               (*ENDIF*)
          (*ENDIF*)
          END
     ELSE
	  IF   tmp_nam = '&ENDIF            '
          THEN
               directive := d_endif
          ELSE
	       IF   tmp_nam = '&IF               '
               THEN
                    directive := d_if
               ELSE
		    IF   tmp_nam = '&IFDEF            '
                    THEN
                         directive := d_ifdef
                    ELSE
			 IF   tmp_nam = '&IFNDEF           '
                         THEN
                              directive := d_ifndef
                         ELSE
			      IF   tmp_nam = '&ELSE             '
                              THEN
                                   directive := d_else
                              ELSE
				   IF   tmp_nam = '&DEFINE           '
                                   THEN
                                        directive := d_define
                                   ELSE
					IF   tmp_nam = '&UNDEF            '
                                        THEN
                                             directive := d_undef
                                        ELSE
                                             directive := d_err
                                        (*ENDIF*)
                                   (*ENDIF*)
                              (*ENDIF*)
                         (*ENDIF*)
                    (*ENDIF*)
               (*ENDIF*)
          (*ENDIF*)
     (*ENDIF*)
ELSE
     directive := d_none ;
(*ENDIF*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        fetch_val ;


 
VAR
      tmp_char : char ;
      tmp_name : name ;
      sym_tmp  : c45_token_typ ;
 
BEGIN
err := false ;
REPEAT
       tmp_char := input_line . iol [dir_pos ];
       dir_pos := succ (dir_pos );
UNTIL
      (tmp_char <> blank ) OR (dir_pos > input_line . llen );
(*ENDREPEAT*)


IF   tmp_char = c_dollar
THEN
     BEGIN
     get_name (input_line , dir_pos , tmp_name );
     name_to_token (tmp_name , sym_tmp , err );
     fetch_sym (sym_tmp , found , val );


     IF   ( ( sym_tmp = c45_c_trace ) AND ( NOT ( found ) ) )
     THEN
          BEGIN
          suppress := true ;
          found := true ;
          END
     ELSE
          BEGIN
          IF   ( ( NOT ( found ) ) AND ( NOT ( suppress ) ) )
          THEN
               BEGIN
               wr_error ( f_undef_sym , true );
               err := true ;
               suppress := true ;
               END ;
          (*ENDIF*)
          END ;
     (*ENDIF*)
     END
ELSE
     BEGIN
     IF   tmp_char <> blank
     THEN
          BEGIN
          IF   tmp_char IN c45_dilimiter
          THEN
               REPEAT
		      tmp_char := input_line . iol [dir_pos ];
                      dir_pos := succ (dir_pos );
               UNTIL
                     ( NOT ( tmp_char IN c45_dilimiter )) OR
		     ( dir_pos > input_line . llen )        OR
                     ( tmp_char = blank ) ;
               (*ENDREPEAT*)
          (*ENDIF*)
          dir_pos := pred (dir_pos );


	  IF   ( dir_pos <= maxlinelength )  AND
	       ( NOT ( input_line . iol [dir_pos ] IN c45_dilimiter ) )
          THEN
               BEGIN
               get_name (input_line , dir_pos , tmp_name );
               name_to_token (tmp_name , val , err );
               found := true ;
               END
          ELSE
               BEGIN
               wr_error ( f_miss_val ,true );
               err := true ;
               suppress := true ;
               found := false ;
               val := blank_token ;
               END ;
          (*ENDIF*)
          END
     ELSE
          BEGIN
          wr_error ( f_miss_val ,true );
          err := true ;
          suppress := true ;
          found := false ;
          END ;
     (*ENDIF*)
     END ;
(*ENDIF*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        fetch_in_val ;


 
VAR
      tmp_char : char ;
      tmp_name : name ;
      sym_tmp  : c45_token_typ ;
      i        : integer ;
      a        : integer ;
      e        : integer ;
 
BEGIN
value_found := false ;
dili_found := false ;
dilimit [ 1 ] := ' ';
dilimit [ 2 ] := ' ';
val := blank_token ;
tmp_name := blankname ;
WITH input_line DO
     BEGIN
     REPEAT
	    tmp_char := iol [dir_pos ];
            dir_pos  := succ (dir_pos );
     UNTIL
            ( NOT ( tmp_char IN c45_dilimiter )) OR
            ( tmp_char IN c45_in_dilimit )       OR
	    ( dir_pos >  llen ) ;
     (*ENDREPEAT*)


     dir_pos := pred (dir_pos );
     e := 0 ;
     i := dir_pos ;
     a := i ;
     IF   ( tmp_char IN c45_in_dilimit )
     THEN
          BEGIN
	  IF   ( iol [ i ] = ')' )
          THEN
               i := i - 1 ;
          (*ENDIF*)
          REPEAT
                 e := succ ( e ) ;
		 IF   iol [ i ]  IN c45_in_dilimit
                 THEN
		      dilimit [ e ] := iol [ i ] ;
                 (*ENDIF*)
                 i := i + 1
          UNTIL
		( e = 2 ) OR ( i > llen ) OR
                ( dilimit [ 1 ] = '[' ) OR ( dilimit [ 1 ] = ']' );
          (*ENDREPEAT*)


          IF   ( dilimit = '[ ' ) OR ( dilimit = '] ' ) OR
               ( dilimit = '(.' ) OR ( dilimit = '.)' )
          THEN
               BEGIN
               dili_found := true ;
               dir_pos := i ;
               END
          ELSE
               IF   ( dilimit [ 2 ] = '.' )
               THEN
                    BEGIN
		    IF   ( iol [ i ] = ')' )
                    THEN
                         BEGIN
                         dilimit [ 1 ] := dilimit [ 2 ] ;
			 dilimit [ 2 ] := iol [ i ] ;
                         dili_found := true ;
                         dir_pos := i + 1 ;
                         END
                    ELSE
			 IF   ( iol [ i ] <> ')' )
                         THEN
                              BEGIN
                              dilimit [ 1 ] := ' ' ;
                              dilimit [ 2 ] := ' ' ;
                              dili_found := false ;
                              dir_pos := i ;
                              END ;
                         (*ENDIF*)
                    (*ENDIF*)
                    END
               ELSE
                    IF   ( dilimit [ 2 ] = ']' )
                    THEN
                         BEGIN
                         dilimit [ 1 ] := dilimit [ 2 ] ;
                         dilimit [ 2 ] := ' ' ;
                         dili_found := true ;
                         dir_pos := i ;
                         END
                    ELSE
                         IF   ( dilimit [ 2 ] = ')' )
                         THEN
                              BEGIN
                              dili_found := false ;
                              dir_pos := i ;
                              END
                         ELSE
                              BEGIN
                              dili_found := false ;
                              dir_pos := i - 1 ;
                              END ;
                         (*ENDIF*)
                   (*ENDIF*)
               (*ENDIF*)
          (*ENDIF*)
          END
      ELSE
          BEGIN
          REPEAT
		 IF   iol [i ]  IN c45_dilimiter
                 THEN
                      e := i - 1 ;
                 (*ENDIF*)
                 i := i + 1
          UNTIL
		(e <> 0 ) OR (i > llen );
          (*ENDREPEAT*)


          IF   e = 0
          THEN
	       e := llen ;
          (*ENDIF*)
          dir_pos := e + 1 ;
	  IF   e - a >= maxnamelength
          THEN
	       e := a + maxnamelength - 1 ;
          (*ENDIF*)


          FOR i := a TO e  DO
	      tmp_name [ i - a + 1 ]  := uppercase ( iol [ i ] ) ;
          (*ENDFOR*)


          name_to_token (tmp_name , val , err );
          value_found := true ;
          END ;
      (*ENDIF*)
     END;
(*ENDWITH*)


END ;
 
(*------------------------------*)
 
PROCEDURE
        get_first_name ;


 
VAR
      i : integer ;
      a : integer ;
      e : integer ;
 
BEGIN
WITH inln DO
     BEGIN
     nam := blankname ;
     a := pos ;
     e := 0 ;
     i := 1 ;


     IF   ( iol [i ] = c_directive )
     THEN
          BEGIN
          REPEAT
		 IF   iol [i ]  IN c45_dilimiter
                 THEN
                      e := i - 1 ;
                 (*ENDIF*)
                 i := i + 1
          UNTIL
		(e <> 0 ) OR (i > llen );
          (*ENDREPEAT*)


          IF   e = 0
          THEN
	       e := llen ;
          (*ENDIF*)
          pos := e + 1 ;
	  IF   e - a >= maxnamelength
          THEN
	       e := a + maxnamelength - 1 ;
          (*ENDIF*)


          FOR i := a TO e  DO
	      nam [ i ]  := uppercase ( iol [ i ] ) ;
          (*ENDFOR*)
          END ;
     (*ENDIF*)
     END ;
(*ENDWITH*)


END;
 
(*------------------------------*)
 
PROCEDURE
        get_name ;


 
VAR
      i : integer ;
      a : integer ;
      e : integer ;
 
BEGIN
WITH inln DO
     BEGIN
     nam := blankname ;
     a := 0 ;
     e := 0 ;
     i := pos ;


     REPEAT
	    IF   ( iol [i ]  <> blank )
            THEN
                 a := i ;
            (*ENDIF*)
            i := i + 1
     UNTIL
	   (a <> 0 ) OR (i > llen );
     (*ENDREPEAT*)


     IF   a <> 0
     THEN
          BEGIN
	  IF   i <= maxlinelength
          THEN
               BEGIN
               REPEAT
		      IF   iol [i ]  IN c45_dilimiter
                      THEN
                           e := i - 1 ;
                      (*ENDIF*)
                      i := i + 1
               UNTIL
		     (e <> 0 ) OR (i > llen );
               (*ENDREPEAT*)
                END ;
          (*ENDIF*)


          IF   e = 0
          THEN
	       e := llen ;
          (*ENDIF*)
          pos := e + 1 ;
	  IF   e - a >= maxnamelength
          THEN
	       e := a + maxnamelength - 1 ;
          (*ENDIF*)


          FOR i := a TO e  DO
	      nam [ i - a + 1 ]  := uppercase ( iol [ i ] ) ;
          (*ENDFOR*)
          END ;
     (*ENDIF*)
     END ;
(*ENDWITH*)


END;
 
(*------------------------------*)
 
PROCEDURE
        get_beg_end_dilimiter ;


 
VAR
      i : integer ;
      a : integer ;
      e : integer ;
 
BEGIN
dilimit_ok := false ;
end_dili := '**' ;
WITH inln DO
     BEGIN
     begin_dili [ 1 ] := ' ';
     begin_dili [ 2 ] := ' ';
     a := 0 ;
     e := 0 ;
     i := pos ;
     REPEAT
	    IF   iol [i ]  <> blank
            THEN
                 a := i ;
            (*ENDIF*)
            i := i + 1
     UNTIL
	   (a <> 0 ) OR (i > llen );
     (*ENDREPEAT*)
     IF   a <> 0
     THEN
          BEGIN
          i := i - 1 ;
          REPEAT
                 e := succ ( e ) ;
		 IF   iol [ i ]  IN c45_in_dilimit
                 THEN
		      begin_dili [ e ] := iol [ i ] ;
                 (*ENDIF*)
                 i := i + 1
          UNTIL
		( e = 2 ) OR ( i > llen ) OR
                ( begin_dili [ 1 ] = '[' ) OR ( begin_dili [ 1 ] = ']' );
          (*ENDREPEAT*)


          IF   ( begin_dili = '[ ' ) OR ( begin_dili = '(.' )
          THEN
               BEGIN
               dilimit_ok := true ;
               pos := i ;
               IF   begin_dili = '[ '
               THEN
                    end_dili := '] '
               ELSE
                    IF   begin_dili = '(.'
                    THEN
                         end_dili := '.)' ;
                    (*ENDIF*)
               (*ENDIF*)
               END ;
          (*ENDIF*)
          END;
     (*ENDIF*)
     END;
(*ENDWITH*)


END;
 
(*------------------------------*)
 
PROCEDURE
        wr_error ;


 
VAR
      i , j : integer ;
      znr   : integer ;
      m , n : integer ;
      err_l : c45_token_typ ;
 
BEGIN
err_l := error_line_count ;
c45_error_text := blankerror ;
FOR  i := 1 TO 40 DO
    c45_error_text [ i ] := contents [ i ] ;
(*ENDFOR*)
IF print_nr
THEN
     BEGIN
     j := 40 ;
     FOR  i := 1 TO c45_maxtokenlength DO
         c45_error_text [ j + i ] := err_l [ i ]  ;
     (*ENDFOR*)
     j := j + c45_maxtokenlength ;
     i := c45_maxerrtlength ;
     n := c45_line_count ;
     WHILE ( i > j ) AND  ( n  > 0 ) DO
           BEGIN
           c45_error_text [ i ] := chr ( ( n MOD 10 ) + ord ( '0' ) ) ;
           n := n DIV 10 ;
           i := pred ( i ) ;
           END ;
     (*ENDWHILE*)
     END ;
(*ENDIF*)
END;
 
(*------------------------------*)
 
PROCEDURE
        store_sym ;


 
VAR
      i : integer ;
      j : integer ;
 
BEGIN


err := false ;
find_sym ( sym_name , i );
IF   i <> 0
THEN
     BEGIN
     WITH c45_all_syms [ i ] DO
          BEGIN
          s_value := blank_token ;
          j := 1 ;
          WHILE ( j <= c45_maxtokenlength )    AND
                ( sym_val [ j ] <> blank )   AND
                ( sym_val [ j ] <> chr (0) ) DO
                BEGIN
                s_value [ j ] := sym_val [j] ;
                j := succ ( j ) ;
                END ;
          (*ENDWHILE*)
          END ;
     (*ENDWITH*)
     END
ELSE
     IF   c45_num_syms >= max_syms
     THEN
          BEGIN
          wr_error ( f_too_many_symb , false ) ;
          err := true ;
          END
     ELSE
          BEGIN
          c45_num_syms := succ ( c45_num_syms );
          WITH c45_all_syms [ c45_num_syms ] DO
               BEGIN
               s_name := blank_token ;
               j := 1 ;
               WHILE ( j <= c45_maxtokenlength )     AND
                     ( sym_name [ j ] <> blank )   AND
                     ( sym_name [ j ] <> chr (0) ) DO
                     BEGIN
                     s_name [ j ] := sym_name [j] ;
                     j := succ ( j ) ;
                     END ;
               (*ENDWHILE*)
               s_value := blank_token ;
               j := 1 ;
               WHILE ( j <= c45_maxtokenlength )    AND
                     ( sym_val [ j ] <> blank )   AND
                     ( sym_val [ j ] <> chr (0) ) DO
                     BEGIN
                     s_value [ j ] := sym_val [j] ;
                     j := succ ( j ) ;
                     END ;
               (*ENDWHILE*)
               END ;
          (*ENDWITH*)
          END ;
     (*ENDIF*)
(*ENDIF*)
END ;
 
(*------------------------------*)
 
PROCEDURE
        delete_sym ;


 
VAR
      i : integer ;
 
BEGIN


err := false ;
find_sym ( sym_name , i );
IF   i <> 0
THEN
     BEGIN
     c45_all_syms [ i ] . s_value := blank_token ;
     c45_all_syms [ i ] . s_name  := blank_token ;
     c45_num_syms := pred ( c45_num_syms );
     END;
(*ENDIF*)
IF   i = 0
THEN
     BEGIN
     err := true ;
     wr_error ( f_ill_undef_inst , true ) ;
     END;
(*ENDIF*)
END ;
 
(*------------------------------*)
 
PROCEDURE
        fetch_sym ;


 
VAR
      i : integer ;
 
BEGIN
found := false ;
sym_val := blank_token ;
find_sym ( sym_name , i );
IF   i <> 0
THEN
     BEGIN
     found := true ;
     sym_val := c45_all_syms [i ] . s_value ;
     END ;
(*ENDIF*)
END ;
 
(*------------------------------*)
 
PROCEDURE
        find_sym ;


 
VAR
      i : integer ;
 
BEGIN
i := 0 ;
index := 0 ;
WHILE ( index = 0 ) AND ( i < c45_num_syms ) DO
      BEGIN
      i := succ (i );
      IF   c45_all_syms [i ] . s_name = sym_name
      THEN
           index := i
      (*ENDIF*)
      END ;
(*ENDWHILE*)
END ;
 
(*------------------------------*)
 
FUNCTION
        uppercase ;


 
BEGIN
IF   c in [ 'a'..'i','j'..'r','s'..'z' ]
THEN
     uppercase := chr (ord(c) + ord('A') - ord('a'))
ELSE
     uppercase := c
(*ENDIF*)
END;
 
(*------------------------------*)

FUNCTION
        c45_suppress_on ;



BEGIN
c45_suppress_on := c45_line_suppress;
END;

 