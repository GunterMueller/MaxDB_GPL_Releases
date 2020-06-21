*   *ID* N99A12   DBCMD    changed on 1992-09-28-13.27.12 by ADMIN     *
file connect ( sut sut !
create user "domain" password test dba not exclusive !
commit release!
file connect ( "domain" test !
  create table sysc_all (
      c_entrytype char (18) ascii  key,
      c_authid    char (64) ascii  key,
      c_objn1     char (64) ascii  key,
      c_objn2     char (64) ascii  key,
      c_language  char (3)  ascii  key range in ('GER','ENG'),
      c_lineno    fixed (4)        key,
      c_text      char (80) ascii )!
   create view syscomment
      as select *  from sysc_all
      where  c_authid  = USER
      with check option !
grant insert, delete, update, select
              on syscomment to public !
   create view sysc_dba
      as select *  from sysc_all
      where  c_authid  = USER
          or c_authid = 'SUPERDBA'
      with check option !
commit release !
file connect ( kern test !
create table kern.tab ( hotel fixed(5), kunde fixed(5) ) !
insert "domain".syscomment set
    c_entrytype  = 'COLUMN',
    c_authid     = user,
    c_objn1      = 'TAB',
    c_objn2      = 'HOTEL',
    c_language   = 'ENG',
    c_lineno     = 1,
    c_text       = 'KERN.HOTEL ist eine Spalte' !
insert "domain".syscomment set
    c_entrytype  = 'COLUMN',
    c_authid     = user,
    c_objn1      = 'TAB',
    c_objn2      = 'KUNDE',
    c_language   = 'ENG',
    c_lineno     = 1,
    c_text       = 'KERN.KUNDE ist eine Spalte' !
rollback work release !
