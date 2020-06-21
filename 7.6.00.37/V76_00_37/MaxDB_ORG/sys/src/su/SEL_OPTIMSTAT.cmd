* show optimize stat &1 !
declare sel_opt cursor for 
 select columnname,indexname,distinctvalues,
        pagecount
   from sysdba.optimizerstatistics
  where tablename = '&1' !
fetch sel_opt into :a,:b,:c,:d !
close sel_opt !
