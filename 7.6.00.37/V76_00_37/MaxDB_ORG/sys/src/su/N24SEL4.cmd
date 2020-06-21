*   *ID* N24SEL4  DBCMD    changed on 1992-09-28-13.25.43 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT /<distinct spec>/ <table columns>
 *** INTO <parameter list> FROM  tab WHERE ...
 *** Getestet wird <table columns>.
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
select tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10
  from tab
  where spalte1 = 'GERHARD' !
select tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10
  from tab
  where spalte1 = 'GERHARD' !
select tab.* ,tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab
  where spalte1 = 'GERHARD' !
select tab.* ,  tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab
  where spalte1 = 'GERHARD' !
select  tab.*,tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab
  where spalte1 = 'GERHARD' !
select  tab.*  ,  tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab
  where spalte1 = 'GERHARD' !
select all tab.*, tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab
  where spalte1 = 'GERHARD' !
select all tab.*, tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab
  where spalte1 = 'GERHARD' !
select distinct tab.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10
  from tab
  where (spalte1, spalte4) in (('KERN', spalte9)) !
select ref.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10
  from tab  ref
  where spalte9 is null !
select all ref.* , ref.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab  ref
  where spalte9 is null !
select distinct ref.*,ref.*
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
       :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20
  from tab  ref
  where (spalte1, spalte4) in (('KERN', spalte9)) !
commit work release !
