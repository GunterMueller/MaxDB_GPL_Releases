* show statistics index &1.&2 !
declare sel_stat cursor for 
select description,
       value (chr(numeric_value), char_value) "VALUE"
  from sysdba.tablestatistics
 where owner = '&1' 
   and tablename = '&2' !
fetch sel_stat into :a,:b !
close sel_stat !
