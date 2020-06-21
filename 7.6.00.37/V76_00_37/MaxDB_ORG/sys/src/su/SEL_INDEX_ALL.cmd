* show index !
declare sel_ind cursor for
 select owner,tablename,indexname,type,
        columnname,sort
   from domain.INDEXCOLUMNS
  where owner <> 'SUT'
   order by owner,tablename,indexname,columnno !
fetch sel_ind into :a,:b,:c,:d,:e,:f !
close sel_ind !