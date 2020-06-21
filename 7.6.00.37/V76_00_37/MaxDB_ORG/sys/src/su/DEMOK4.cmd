*   *ID* DEMOK4   DBCMD    changed on 1993-03-23-15.00.00 by MAX       *
file connect ( travel00 travel00 !
 * Maerz 1993, Folie 4.020 !
create table temp.calculation (
    service char (40),
    price   fixed (6,2)) ignore rollback !
 * Maerz 1993, Folie 4.030 !
create table companies (cno, name, zip, address)
    as select cno, name, zip, address
       from   customer
       where  title = 'Company' !
select * from companies !
fetch !
 * Maerz 1993, Folie 4.040 -> Siehe EDEMO.inst !
file sel_table1 ( CITY !
 * Maerz 1993, Folie 4.050 !
create table tab1 (
    c1 char(10),
    k2 char(10),
    c3 char(10),
    k1 char(10),
    primary key (k1,k2)) !
file sel_cols_tab ( TAB1 !
file sel_primkey ( TRAVEL00 TAB1 !
 * Maerz 1993, Folie 4.060 !
create table tab2 (
    u2 char(10),
    c2 char(10),
    c3 char(10),
    u1 char(10),
    constraint c1_tab2 unique (u1,u2)) !
file sel_index_tab ( TAB2 !
create table tab3 (
    u1 char(10),
    constraint c1_tab3 unique(u1),
    c2 char(10),
    c3 char(10),
    u2 char(10),
    constraint c2_tab3 unique(u2)) !
file sel_index_tab ( TAB3 !
 * Maerz 1993, Folie 4.070 !
create table tab4 (
    c1 char(10),
    c2 char(10),
    c3 char(10),
    foreign key (c3,c2) references tab2 (u1,u2))!
file sel_foreignkey ( TRAVEL00 TAB4 !
 * Maerz 1993, Folie 4.090 !
subtrans begin !
create table rectangle (
    x1   fixed( 5) constraint x1 > 0,
    y1   fixed( 5) constraint y1 > 0,
    x2   fixed( 5) constraint x2 > 0 and x2 > x1,
    y2   fixed( 5) constraint y2 > 0 and y2 > y1,
    area fixed(10) constraint area = (x2 - x1) * (y2 - y1)) !
subtrans rollback !
create table rectangle (
    x1   fixed( 5) constraint x1 > 0,
    y1   fixed( 5) constraint y1 > 0,
    x2   fixed( 5),
    y2   fixed( 5),
    area fixed(10) constraint (x2 > x1) and
                              (y2 > y1) and
                              (area = (x2 - x1) * (y2 - y1))) !
 * Maerz 1993, Folie 4.100 !
create domain birthday
    date default date
    constraint birthday >  '18800101' and
               birthday <= date !
 * Maerz 1993, Folie 4.110 !
rename synonym hotel to prison !
rename view cust_addr to prisoner_addr !
drop table temp.calculation !
rollback release!
file connect ( travel10 travel10 !
rename table room to jail !
rollback release!
*
file connect ( travel00 travel00 sqlmode ansi !
 * Maerz 1993, Folie 4.120 !
create table ansi (
       c1 character,
       c2 decimal,
       c3 double precision,
       c4 float,
       c5 integer,
       c6 numeric (10,2),
       c7 real,
       c8 smallint) !
commit work release!
file connect ( travel00 travel00 sqlmode db2 !
 * Maerz 1993, Folie 4.130 !
create table db2_1 (
       c1 char,
       c2 date,
       c3 decimal,
       c4 double precision,
       c5 float (10),
       c6 graphic (10),
       c7 integer,
       c8  numeric (10),
       c9  real,
       c10 smallint,
       c11 time,
       c12 timestamp,
       c13 varchar (10),
       c14 vargraphic (10)) !
create table db2_2 (
       c1 long varchar) !
create table db2_3 (
       c1 long vargraphic) !
commit work release!
file connect ( travel00 travel00 sqlmode oracle!
 * Maerz 1993, Folie 4.140 !
create table oracle (
       c1  char,
*      c2  character,
       c2  date,
       c3  dec,
       c4  decimal(*),
       c5  double precision,
       c6  float(*),
       c7  int,
       c8  integer,
       c9  long,
*      c10 long raw,
*      c11 long varchar,
       c12 number,
*      c13 number(*),
       c13 numeric,
       c14 raw,
       c15 real,
*      c16 rowid,
       c16 smallint,
       c17 varchar) !
commit work release!
file connect ( travel00 travel00 !
file sel_cols_tab ( ANSI !
file sel_cols_tab ( DB2_1 !
file sel_cols_tab ( DB2_2 !
file sel_cols_tab ( DB2_3 !
file sel_cols_tab ( ORACLE !
drop table ansi !
drop table db2_1 !
drop table db2_2 !
drop table db2_3 !
drop table oracle !
file sel_cols_tab ( DUAL !
select * from dual !
fetch !
commit work release !
