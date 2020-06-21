* show privileges granted to &1 on * !
declare sel_privs cursor for 
 select *
   from sysdd.granted_privileges
  where schemaname not in ('SUT','DOMAIN','SUPERDBA') 
    and schemaname not like 'SYS%'
    and username = '&1' !
fetch sel_privs into :a, :b, :c, :d, :e, :f !
close sel_privs !