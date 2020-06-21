*   *ID* C00E213  DBCMD    changed on 1992-09-28-13.23.07 by ADMIN     *
* interne Fehlernummer 213 !
* dutch railways !
file cleark !
file connect ( kern test !
create table t_verkocht (
    cd_af  fixed (3)   key,
    nr_lok fixed (2)   key,
    bdg_l  date        key,
    nr_bo  fixed (6)   key          range between 1 and 999999,
    nr_lod fixed (3)       not null,
    srt_v  char  (3) ascii not null range in (
             'ASL', 'BAP', 'BNL', 'BTL', 'NEV', 'RSB', 'RES', 'VGD', 'LKT'),
    bedra  fixed (9,2)     not null,
    ind_v  char  (1) ascii not null range in ('J', 'N'),
    statu  char  (1) ascii not null range in ('R', 'S', 'V', 'A'),
    dt_re  date            not null,
    tijd_  time            not null,
    nr_te  fixed (6)                range between 1 and 999999,
    cd_ho  fixed (1)                range between 1 and 8) !
alter table t_verkocht add (
    dt_se  date          )!
alter table t_verkocht column dt_se not null !
create view v as select cd_af, nr_lok, bdg_l, nr_bo, nr_lod,
    srt_v, bedra, ind_v, statu,
    dt_re, tijd_, nr_te, cd_ho,
    dt_se
    from t_verkocht
    where srt_v not in ('BAP', 'LKT') OR srt_v in ('BAP', 'LKT') and
    ind_v = 'N'
    with check option !
pars_then_ex !
data !
insert v (cd_af, nr_lok, bdg_l, nr_bo, nr_lod, srt_v,
    bedra, ind_v, statu, dt_re, dt_se, tijd_, nr_te, cd_ho)
    values (:a, :b, :c, :d, :e, :f, :g, :h, :i, :j, :k, :l, :m, :n) !
n 3 762 n 2 22 c 10 '19900101' n 6 10001 n 3 39 c 3 'BNL'
n 9,2 17.5 c 1 'N' c 1 'V' c 10 '19910530'
c 10 '19910530'
c 8 '092407'
n 6 25  n 1 1

nodata !
pars_execute !
drop table t_verkocht !
commit release !
