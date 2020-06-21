* show foreign key &1.&2 !
declare sel_ref cursor for 
 select owner,tablename,columnname,
        fkeyname refname,
        refowner,reftablename,refcolumnname,rule
   from domain.foreignkeycolumns
  where owner = '&1' 
    and tablename = '&2' !
fetch sel_ref into :a,:b,:c,:d,:e,:f,:g,:h!
close sel_ref !
