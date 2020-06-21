* show tabledef &1.&2 !
declare sel_col cursor for 
 select columnname,
        mode as "MOD",
        datatype,
        codetype as "CODE",
        len,"DEC",columnprivileges,"DEFAULT"
   from domain.columns
  where owner = '&1'
    and tablename = '&2' 
    and mode = 'KEY'
    and keypos > 0
  order by keypos !
fetch sel_col into :a,:b,:c,:d,:e,:f,:g,:h !
close sel_col !
