* show privileges granted to * on &1 !
declare sel_privs cursor for 
 select *
   from sysdd.granted_privileges
  where schemaname = usergroup
    and tablename = '&1' !
fetch sel_privs into :a, :b, :c, :d, :e, :f !
close sel_privs !