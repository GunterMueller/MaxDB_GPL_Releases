* show index &1.&2 !
declare sel_ind cursor for
 select owner,tablename,indexname,type,
        columnname,sort
   from domain.indexcolumns
  where owner = '&1'
    and tablename = '&2' 
   order by owner,tablename,indexname,columnno !
fetch sel_ind into :a,:b,:c,:d,:e,:f !
close sel_ind !
