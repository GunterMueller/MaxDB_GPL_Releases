* show monitor &1 !
declare sel_mon cursor for 
 select description, 
        fixed(value,11,0) "VALUE"  
   from sysdba.monitor_&1 !
fetch sel_mon into :a, :b !
close sel_mon !