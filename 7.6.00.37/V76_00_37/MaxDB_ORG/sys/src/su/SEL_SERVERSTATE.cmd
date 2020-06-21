* show stat state !
declare sel_stat cursor for 
select description,
       value (chr(numeric_value), "VALUE") "VALUE"
  from sysdba.db_state !
fetch sel_stat into :a,:b !
close sel_stat !

