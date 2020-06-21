* show statistics index &1.&2 !
declare sel_stat cursor for 
select description,
       value (chr(numeric_value), char_value) "VALUE"
  from sysdba.indexstatistics
  where owner <> 'SUT'
   and tablename = '&1' 
   and indexname = '&2' !
fetch sel_stat into :a,:b !
close sel_stat !
