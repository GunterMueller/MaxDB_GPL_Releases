* show table &1.* !
declare sel_tab cursor for 
 select owner,tablename,privileges,type
   from domain.tables
  where owner = '&1' 
    and tablename <> 'SEL_TAB' !
fetch sel_tab into :a, :b, :c, :d !
close sel_tab !