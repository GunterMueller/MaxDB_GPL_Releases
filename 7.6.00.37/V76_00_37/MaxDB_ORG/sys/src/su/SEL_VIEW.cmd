* show view &1 !
declare sel_view cursor for 
 select definition
   from sysdd.showview
  where owner = usergroup
    and tablename = '&1' !
fetch sel_view into :a !
close sel_view !
