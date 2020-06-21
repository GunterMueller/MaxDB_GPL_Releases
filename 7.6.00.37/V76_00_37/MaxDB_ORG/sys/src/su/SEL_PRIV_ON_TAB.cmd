* show priv on &1 !
declare sel_privs cursor for 
 select schemaname,tablename,columnname,privileges,grantor
   from sysdd.db_privileges 
  where grantor not in ('SUT','SUPERDBA')
    and (schemaname = user or username = user or username = 'PUBLIC')
    and tablename = '&1' !
fetch sel_privs into :a,:b,:c,:d,:e !
close sel_privs !
