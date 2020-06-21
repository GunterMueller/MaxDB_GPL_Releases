* show monitor !
declare sel_mon cursor for 
 select description, 
        fixed(value,11,0) "VALUE"  
   from sysdba.monitor 
  where NOT (type in ('&1', '&2')) !
fetch sel_mon into :a, :b !
close sel_mon !