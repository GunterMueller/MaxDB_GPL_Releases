* show column &1.&2 !
declare sel_col cursor for 
 select owner,tablename,columnname,
        mode as "MOD",
        datatype,
        codetype as "CODE",
        len,"DEC",columnprivileges,"DEFAULT"
   from domain.columns
  where owner = USER
    and tablename = '&1'
    and columnname = '&2' !
fetch sel_col into :a,:b,:c,:d,:e,:f,:g,:h,:i,:j !
close sel_col !
