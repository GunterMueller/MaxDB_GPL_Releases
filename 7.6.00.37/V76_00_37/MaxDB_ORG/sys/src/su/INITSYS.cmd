*   *ID* INITSYS  DBCMD    changed on 1992-09-28-13.23.55 by ADMIN     *
file connect (sut sut !
set current_schema = domain!
create table syscomment (dummy int) !
grant all on syscomment to public !
  create table sysusage   (
      defobjtype     char (8)   ascii  key,
      defauthid      char (18)  ascii  key,
      defobjn1    char (18)  ascii  key,
      defobjn2    char (18)  ascii  key,
      refobjtype     char (8)   ascii  key,
      refauthid      char (18)  ascii  key,
      refobjn1    char (18)  ascii  key,
      refobjn2    char (18)  ascii  key,
      createdate     date,
      createtime     time,
      text           char (80)  ascii ) !
grant all on sysusage to public !
(*************************)
(***  sysusage  user   ***)
(***                   ***)!
   create view sysdefusage
      as select *  from sysusage
         where  defauthid  = USERGROUP
         with check option !
  grant all on sysdefusage to public !
  create view sysrefusage
      as select *  from sysusage
         where  refauthid  = USERGROUP
            or  refauthid  = '        '
         with check option !
  grant select on sysrefusage to public !
(***********************)
(***  sysusage_dba   ***)
(***                 ***)!
   create view sysdu_dba
      as select *  from sysusage
         where  defauthid  = USER or defauthid = 'DOMAIN'
         with check option !
   create view sysru_dba
      as select *  from sysusage
         where  refauthid  = USER  or  refauthid = 'DOMAIN'
         with check option !
commit work !
(************************)
(****   syscomment    ***)
(***                  ***)!
  create table sysc_all (
      c_entrytype char (18) ascii  key,
      c_authid    char (18) ascii  key,
      c_objn1  char (18) ascii  key,
      c_objn2  char (18) ascii  key,
      c_language  char (3)  ascii  key range in ('GER','ENG'),
      c_lineno    fixed (4)        key,
      c_text      char (80) ascii )!
   create view sysc_priv
      as select *  from sysc_all
      where  c_authid  = USER
      with check option !
grant insert, delete, update, select
              on syscomment to public !
   create view sysc_dba
      as select *  from sysc_all
      where  c_authid  = USER
          or c_authid = 'DOMAIN'
      with check option !
commit work !
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',-1,'In die DOMAIN.syscomment Tabelle koennen alle Benutzer' )!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',0, 'Daten einfuegen, loeschen, aendern und selectieren nur ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',1, 'mit der c_authid = current_authid.  ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER', 2,'Alle Keyangaben muessen in Grossbuchstaben angegeben werden')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',3, 'Zum Ansehen der Informationen koenen alle Benutzer ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',4, '"show comment ..." absetzen ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',5, 'Beim "show comment .." werden die Privilegien des ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',6, 'des Benutzers geprueft. ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',7, 'Ist c_entrytyp ungleich TABLE oder COLUMN, so kann der')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',8, 'Benutzer nur seine eigenen c_eingaben sehen, d.h. ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',9,'c_authid=current_auth_id. Moechte er die Informationen ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',10, 'anderen zugaenglich machen, muss er sie als c_authid=')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'GER',11, ' "PUBLIC" angeben ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',12, ' Definition der sysc_all table :: ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',13, 'create table syscomment (')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',14, '   c_entrytype char (18)  ascii  key,')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',15, '   c_authid    char (8)   ascii  key,')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',16, '   c_objn1  char (18)  ascii  key, ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',17, '   c_objn2  char (18)  ascii  key, ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',18,
'   c_language  char (3)  ascii  key range in (''GER'',''ENG''),')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',19, '   c_lineno    fixed (4)    key,')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',20, '   c_text      char (80)  ascii)!  ')!
insert sysc_all values ('TABLE','DOMAIN','SYSCOMMENT','  ',
'ENG',21, 'grant insert,delete,update,select on syscomment to public !')!
commit release !
