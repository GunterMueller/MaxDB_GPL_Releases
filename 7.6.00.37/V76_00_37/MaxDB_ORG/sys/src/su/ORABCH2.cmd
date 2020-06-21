 * PAGE 30 !
 * folgende Abfrage aus dem Buch geht nicht, weil die
 * weder zu Tabelle skill noch zu workerskill paszt.
 * Auszerdem stimmt die Grosz/Kleinschreibung nicht.
 * vergessen wir sie !
select ability, description
  from skill
 where name = 'John Pearson' and skill = 'Woodcutter' !
 * PAGE 37 !
select name, ability, skill
  from workerskill !
fetch into :a,:b,:c !
commit work !
