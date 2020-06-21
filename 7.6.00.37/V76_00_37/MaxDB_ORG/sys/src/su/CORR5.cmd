*   *ID* CORR5    DBCMD    changed on 1992-09-28-13.23.02 by ADMIN     *
*
*  join liefert kein ergebnis, die correlation wird daher uebersprungen
*  absturz in bd, gemeldet von COLLOGIA (Frau Krieger) 27.11.89*
*  fehlerprotokoll # 77
*
file connect ( kern test !
*
create domain dm_blgr   char(2) range in ('0', 'A', 'B', 'AB')!
create domain dm_c20    char(20) !
create domain dm_c7     char(7) !
create domain dm_c9     char(9) !
create domain dm_c9def  char(9) default '999999999'!
create domain dm_dat    date range between '19600401'
                                       and '20101231' !
create domain dm_dtb    date range between '18500101'
                                       and '20101231' !
create domain dm_d2     decimal(2) !
create domain dm_ins    char(3) range in ('NO', 'YES') !
create domain dm_inva_2 decimal(8) range between 24000 and 99999999 !
create domain dm_nob    decimal(2) range between 0 and 15 !
create domain dm_pnr    decimal(6) range between 100000 and 999999 !
create domain dm_rhf    char(1) range in ('+', '-') !
create domain dm_sex    char(1) range in ('M', 'F') !
create domain dm_stat   char(4) range in ('CURR', 'PLAN', 'FORM') !
create domain dm_torm   char(4) range in ('TRM', 'NRS', 'RCPT',
                                          'STR', 'VAR') !
*
create table pat (
  pnr    dm_pnr key,
  nm     dm_c20 not null,
  adr    dm_c20,
  zp     dm_c7,
  cty    dm_c20 not null,
  plb    dm_c20,
  dtb    dm_c20 not null,
  regd   dm_dat not null,
  blgr   dm_blgr,
  rhf    dm_rhf,
  sex    dm_sex) !
*
create table rm (
  rnr    dm_c9 key,
  torm   dm_torm not null,
  flsp   dm_d2 not null,
  status dm_stat not null,
  nob    dm_nob)!
*
create table trf (
  pnr    dm_pnr not null,
  tdat   dm_dat not null,
  rnr    dm_c9def,
  com    dm_c20)!
*
create table adm (
  pnr    dm_pnr not null,
  admd   dm_dat not null,
  rnr    dm_c9def,
  disd   dm_dat,
  dsch   dm_ins not null,
  dcsd   dm_ins not null,
  inva   dm_inva_2,
  aftc   dm_c20)!
*
commit work!
*
insert pat set pnr = 100000,     nm   = 'Werner',    cty = 'Kiel',
               dtb = '19800401', regd = date !
insert pat set pnr = 100001,     nm   = 'Holgi',     cty = 'Kiel',
               dtb = '19800401', regd = date !
insert pat set pnr = 100002,     nm   = 'Oelfuss',   cty = 'Kiel',
               dtb = '19800401', regd = date !
*
insert rm  set rnr = '100000000', torm='TRM', flsp = 0, status = 'CURR'!
insert rm  set rnr = '100000001', torm='TRM', flsp = 1, status = 'CURR'!
insert rm  set rnr = '100000002', torm='TRM', flsp = 2, status = 'CURR'!
*
insert trf set pnr = 100000, tdat = date !
insert trf set pnr = 100002, tdat = date !
*
insert adm set pnr  = 100000, admd = date,
               dsch = 'NO',   dcsd = 'NO' !
*
pars_then_ex!
data!
declare pseudo_cursor cursor for
select pat.nm, rm.rnr
  from adm a, pat, rm, trf
 where pat.pnr = a.pnr
   and a.dsch = :B
   and a.admd > ALL (
                select tdat
                  from trf
                 where trf.pnr = a.pnr)
   and rm.rnr = a.rnr !
c 3 'NO'

nodata!
pars_execute!
fetch pseudo_cursor!
drop table pat!
drop table rm !
drop table trf!
drop table adm!
drop domain dm_blgr !
drop domain dm_c20 !
drop domain dm_c7 !
drop domain dm_c9 !
drop domain dm_c9def !
drop domain dm_dat !
drop domain dm_dtb !
drop domain dm_d2 !
drop domain dm_ins !
drop domain dm_inva_2 !
drop domain dm_nob !
drop domain dm_pnr !
drop domain dm_rhf !
drop domain dm_sex !
drop domain dm_stat !
drop domain dm_torm !
commit work release !
