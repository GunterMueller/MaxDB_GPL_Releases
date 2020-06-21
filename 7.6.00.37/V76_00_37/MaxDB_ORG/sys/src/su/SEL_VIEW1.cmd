* show view &1.&2 !
declare sel_view cursor for 
 select definition
   from sysdd.showview
  where owner = '&1'
    and owner = usergroup
    and tablename = '&2' !
fetch sel_view into :a !
close sel_view !