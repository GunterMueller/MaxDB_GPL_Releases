* show tabledef &1.&2 !
declare sel_col cursor for 
 select columnname,
        mode as "MOD",
        datatype,
        codetype as "CODE",
        len,"DEC",columnprivileges,"DEFAULT"
   from domain.columns
  where schemaname = '&1'
    and tablename = '&2' 
  order by pos !
fetch sel_col into :a,:b,:c,:d,:e,:f,:g,:h !
close sel_col !
