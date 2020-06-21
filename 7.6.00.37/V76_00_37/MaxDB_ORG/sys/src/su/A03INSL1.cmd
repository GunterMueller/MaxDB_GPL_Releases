*   *ID* A03INSL1 DBCMD    changed on 1992-09-28-13.22.20 by ADMIN     *
file connect ( kern test !
create table t ( a char (8), b char (7))!
insert t set a = 'aa', b = 'bb'!
create index i1 on t ( a, b) !
create table t1 like t !
create view v as select * from t where a > 'v' with check option !
create table t2 ( a2 char (8) key, b2 fixed(8))!
insert t2 set a2 = 'f', b2 = 1 !
alter table t1 foreign key l1 (a) references t2 !
insert t1 select * from v where a = 'aa' and b = 'bb' !
select * from t1 !
fetch !
insert t1 select * from t where a = 'aa' and b = 'bb' !
insert t2 set a2 = 'aa', b2 = 3 !
insert t1 select * from t where a = 'aa' and b = 'bb' !
insert t1 select t.a, t.b from t, t2 !
select * from t1 !
fetch !
drop table t !
drop table t1 !
drop table t2 !
create table tab1 ( spalte1 fixed(4) key, spalte2 char(10)) !
create table tab2 ( spalte1 fixed(4) key, spalte2 char(10)) !
create index i1 on tab1 ( spalte2 ) !
create index i1 on tab2 ( spalte2 ) !
drop index i1 on tab1 !
drop index i1 on tab2 !
drop table tab1 !
drop table tab2 !
commit release !
file connect ( kern test !
* insert select with inv only strategy !
create table te1 (s1 int, s2 int) !
create index i1 on te1 (s1,s2) !
insert te1 values (1,1) !
insert te1 values (1,1) !
create table te2 (s1 int key) !
insert te2 select distinct s1 from te1 !
select * from te2 !
fetch!
rollback release !
* PTS 1133181-3 !
file connect ( sut sut!
create sequence seq_inventory!
CREATE TABLE "W_INVENTORY"
(
    "INVENTORY_ID"   Integer    NOT NULL,
    "CONTACTS_ID"   Integer    NOT NULL,
    "PRODUCT_ID"   Integer    NOT NULL,
    "QTY_IN_STOCK"   Fixed (5,0)    DEFAULT  0,
    "UNIT_ID"   Integer,
    "QTY_IN_STOCK_DELIVERED"   Fixed (5,0)    DEFAULT  0,
    PRIMARY KEY ("INVENTORY_ID")
)!
insert into
w_inventory(inventory_id,contacts_id,product_id,unit_id,qty_in_stock,
qty_in_stock_delivered)
select seq_inventory.nextval,7597,68,1,1.0,0  from dual!
select seq_inventory.nextval,7597,68,1,1.0,0  
from dual where not exists
( select 1 from w_inventory i where contacts_id=7597 
 and product_id=68
and unit_id=1)!
insert into
w_inventory(inventory_id,contacts_id,product_id,unit_id,qty_in_stock,
qty_in_stock_delivered)
select seq_inventory.nextval,7597,68,1,1.0,0  from dual 
where not exists
( select 1 from w_inventory i where contacts_id=7597 and product_id=68
and unit_id=1)!
* cored with older versions !
insert into
w_inventory(inventory_id,contacts_id,product_id,unit_id,qty_in_stock,
qty_in_stock_delivered)
select seq_inventory.nextval,7597,68,1,1.0,0  from dual 
where not exists
( select 1 from w_inventory i where contacts_id=7597 and product_id=41
and unit_id=1)!
select * from w_inventory!
fetch!
rollback release !
