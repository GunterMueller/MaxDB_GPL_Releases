create table b
 (b1 int key ,
  b2 int key ,
  b3 int key ,
  b4 date default '19901111'
  constraint b4 > '1990-01-01' AND b4 < '19901231')!
create table b
 (b1 int key ,
  b2 int key ,
  b3 int key ,
  b4 date default '19901111'
  constraint b4 > '19900101' AND b4 < '19901231')!
file sel_cols_tab ( B !
file sel_checkdef ( USER B B4 !
set format iso !
file sel_checkdef ( USER B B4 !
insert b values (1, 1, 1, default)!
insert b values (1, 1, 2, default)!
insert b values (1, 1, 3, default)!
insert b values (1, 1, 4, default)!
insert b values (1, 1, 5, default)!
update b set b4 = '19900601' where b3 = 1!
update b set b4 = '1990-06-01' where b3 = 1!
update b set b4 = substr('19900701', 1) where b3 = 2!
update b set b4 = substr('1990-07-01', 1) where b3 = 2!
update b set b4 = date('19900801') where b3 = 3!
update b set b4 = date('1990-08-01') where b3 = 3!
update b set b4 = char(date('19900901'), europe) where b3 = 4!
update b set b4 = char(date('1990-09-01'), europe) where b3 = 4!
update b set b4 = char(date('19900901'), iso) where b3 = 4!
update b set b4 = char(date('1990-09-01'), iso) where b3 = 4!
update b set b4 = char(date('1989-02-28'), iso) where b3 = 5!
update b set b4 = char(date('1989-02-29'), iso) where b3 = 5!
update b set b4 = date(char(date('1991-04-30'), europe)) where b3 = 5!
update b set b4 = date(char(date('1991-04-30'), iso)) where b3 = 5!
update b set b4 = date(char(date('1991-04-31'), iso)) where b3 = 5!
update b set b4 = date(char(date('1990-04-30'), iso)) where b3 = 5!
select * from b !
fetch!
drop table b!
commit!
***********************************************************
***********************************************************
***********************************************************
set format internal!
create table b
 (bd date     default date
              constraint dayofweek (bd) in (1, 2, 3, 4, 5) and
              dayofyear (bd) not in (2, 88, 91, 150, 350),
  bd1 date    not null with default constraint (bd1 = bd) or ((bd1 > bd)
              and (datediff (bd1, bd) > 28)),
  bt1 time    not null with default constraint ((bt1=bt) and (bd1=bd))
              or ((bt1 > bt) and (timediff(bt, bt1) < '40000')),
  bt time     default time
              constraint (bt > '20000' and bt < '130000') or
              (bt > '140000' and bt < '240000' and dayofweek (bd) <> 3),
  b2  fixed    (8, 2) not null with default constraint (b2 + b3) > -b4,
  b3  fixed    (7, 2) default 0 range between -5000 and 5000,
  b4  smallint         default 10000,
  b5  int              default 0,
  b6  varchar  (254)   default 'TEST' constraint b6 > 'a',
                       primary key (b5, bd, bt))!
set format iso !
file sel_cols_tab ( B !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BD1 !
file sel_checkdef ( USER B BT !
file sel_checkdef ( USER B BT1 !
file sel_checkdef ( USER B B2 !
file sel_checkdef ( USER B B3 !
file sel_checkdef ( USER B B4 !
file sel_checkdef ( USER B B5 !
file sel_checkdef ( USER B B6 !
insert b (b5) values (1)!
insert b (b5) values (2)!
select * from b!
fetch!
update b set bd = '1991-10-20', bd1 = substr('1991-11-20', 1),
             bt1 = addtime(bt, '3.00.00') !
update b set bd = '1991-01-02', bd1 = date('1991-11-20'),
             bt1 = addtime(bt, '3.00.00') !
update b set bd1 = adddate(bd, 28), bt1 = addtime(bt, '3.00.00') !
update b set bd1 = adddate(bd, 29), bt1 = addtime(bt, '4.00.00') !
insert b(b5, bt, bt1) values (3, '1.00.00', '1.00.00') !
insert b(b5, bt, bt1) values (3, '13.00.00', '13.00.00') !
insert b(b5, bd, bd1, bt, bt1) values (3, '1991-10-23', '1991-11-23',
             '15.00.00', '18.00.00') !
update b set bd1 = adddate(bd, 29), bt1 = addtime(bt, '3.33.33') !
select * from b!
fetch!
update b set b2 = 6000 where  b5 = 1 !
update b set b2 = -6000 where b5 = 2 !
update b set b3 = -6000 !
update b set b3 = -5000 !
update b set b3 = -4000 !
update b set b3 = -3000 !
select * from b!
fetch!
drop table b!
commit!
