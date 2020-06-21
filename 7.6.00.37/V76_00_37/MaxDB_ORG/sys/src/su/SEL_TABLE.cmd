* show table !
declare sel_tab cursor for 
 select owner,tablename,privileges,type
   from domain.tables
  where owner <> 'SUT'
    and tablename <> 'SEL_TAB' !
fetch sel_tab into :a, :b, :c, :d !
close sel_tab !