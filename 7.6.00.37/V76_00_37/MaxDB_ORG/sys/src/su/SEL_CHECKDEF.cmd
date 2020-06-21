* show check &1.&2.&3 !
declare sel_check cursor for 
 select c_definition
   from sysdd.constraints
  where owner = '&1' 
    and tablename = '&2'
    and constraintname = '&3' !
fetch sel_check into :a !
close sel_check !
