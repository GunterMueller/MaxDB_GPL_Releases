* show index &1 !
declare sel_ind cursor for
 select owner,tablename,indexname,type,
        columnname,sort
   from domain.indexcolumns
  where owner <> 'SUT'
    and tablename = '&1' 
   order by owner,tablename,indexname,columnno !
fetch sel_ind into :a,:b,:c,:d,:e,:f !
close sel_ind !

