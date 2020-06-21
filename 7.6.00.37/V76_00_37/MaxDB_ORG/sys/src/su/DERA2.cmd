*   *ID* DERA2    DBCMD    changed on 1994-03-24-19.30.00 by heinz     *
 *** Bereichs-DELETE wird getestet.
 ***********************************************************!
drop   table RanDelTst!
create table RanDelTst LIKE RanDelBas
!
 ***       with replication!
grant all on RanDelTst to public!
insert RanDelTst select * from RanDelBas!
monitor init!
