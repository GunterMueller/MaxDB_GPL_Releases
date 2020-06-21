* show constraint &1 !
declare sel_cons cursor for 
 select owner,tablename,constraintname
   from domain.constraints !
fetch sel_cons into :a,:b,:c !
close sel_cons !
