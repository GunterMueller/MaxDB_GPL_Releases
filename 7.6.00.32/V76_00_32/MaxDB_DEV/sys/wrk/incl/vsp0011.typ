
      tsp11_ConfParamName  = tsp00_XpKeyType;
      tsp11_ConfParamValue = PACKED ARRAY [1.. 256] OF char;

      tsp11_ConfParamReturnValue = (
            ok_sp11,
            key_not_found_sp11,
            cannot_change_online_sp11,
            no_change_permission_sp11,
            type_mismatch_sp11,
            not_ok_sp11);
 
