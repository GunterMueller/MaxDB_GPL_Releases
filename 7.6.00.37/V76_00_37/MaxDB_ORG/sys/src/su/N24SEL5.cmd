*   *ID* N24SEL5  DBCMD    changed on 1992-09-28-13.25.44 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT <select list> INTO <parameter list>
 *** FROM tab WHERE ...
 *** Es wird getestet, ob in der <select list> die
 *** <table columns> und <column expression> gemischt
 *** gemischt auftreten koennen.
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
select tab.* , spalte9
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,:a11
   from tab where spalte1 = 'ILSE' !
select spalte9, tab.*, tab.spalte9 , spalte9,
     tab.* , tab.spalte9, tab.* , tab.spalte9,
     spalte9 neu91, tab.* , tab.spalte9 neu92
   into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 ,:a10,
        :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20,
        :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29,:a30,
        :a31,:a32,:a33,:a34,:a35,:a36,:a37,:a38,:a39,:a40,
        :a41,:a42,:a43,:a44,:a45,:a46,:a47
   from tab
   where spalte1 = 'ILSE' !
commit work release !
