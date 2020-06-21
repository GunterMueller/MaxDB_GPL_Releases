* show tabledef &1 !
declare sel_col cursor for 
 select columnname,
        mode as "MOD",
        datatype,
        codetype as "CODE",
        len,"DEC",columnprivileges,"DEFAULT"
   from domain.columns
  where owner <> 'SUT'
    and tablename = '&1' 
  order by pos !
fetch sel_col into :a,:b,:c,:d,:e,:f,:g,:h !
close sel_col !
