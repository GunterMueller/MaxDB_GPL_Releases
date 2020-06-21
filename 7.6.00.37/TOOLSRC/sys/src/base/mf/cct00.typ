
      (* Targon/35 *)
      c45_int4      = longint ;
 
      c45_token_typ = PACKED ARRAY [ 1..c45_maxtokenlength ] OF char ;
      c45_errortext = PACKED ARRAY [ 1..c45_maxerrtlength ] OF char;


      (* look up in vsp00 *)
      line          = PACKED ARRAY [ 1..maxlinelength ] OF char;
      name          = PACKED ARRAY [ 1..maxnamelength ] OF char;
 
