*   *ID* DEMOK7   DBCMD    changed on 1993-03-23-15.00.00 by MAX       *
file connect ( travel10 travel10 !
 * Maerz 1993, Folie 7.030 !
file sel_checkdef ( TRAVEL00 CUSTOMER TITLE !
 * Maerz 1993, Folie 7.040 !
file sel_constraint !
rollback release !
file connect ( travel00 travel00 !
 * Maerz 1993, Folie 7.060 !
file sel_domain !
 * Maerz 1993, Folie 7.070 !
file sel_domaindef ( CNO_DOM !
 * Maerz 1993, Folie 7.080 !
declare sel_map cursor for 
 select distinct mapcharsetname, code 
   from domain.mapcharsets !
fetch sel_map into :a,:b !
close sel_map !
 * Maerz 1993, Folie 7.090 !
create table t1 (c1 char(10), k2 char(10), c3 char(10), k1 char(10),
primary key (k1,k2)) !
file sel_primkey ( TRAVEL00 T1 !
drop table t1 !
commit release!
file connect ( travel00 travel00 !
 * Maerz 1993, Folie 7.100 !
file sel_priv_grant_on ( CUSTOMER !
 * Maerz 1993, Folie 7.110 !
declare sel_privs cursor for 
 select distinct schemaname,tablename,columnname,privileges,grantor
   from sysdd.db_privileges 
  where grantor not in ('SUT','DOMAIN','SYS','SUPERDBA')
    and schemaname = 'TRAVEL10' !
fetch sel_privs into :a,:b,:c,:d,:e !
close sel_privs !
commit release !
file connect ( travel00 travel00 !
 * Maerz 1993, Folie 7.130 !
 * Maerz 1993, Folie 7.140 !
select sysdba username from dual!
fetch!
commit release !
 * Maerz 1993, Folie 7.160 !
file connect ( travel10 travel10 !
 * Maerz 1993, Folie 7.170 !
 * Maerz 1993, Folie 7.180 !
select * from domain.versions !
fetch !
commit release!
*


