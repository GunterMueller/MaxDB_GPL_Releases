file connect ( kern test sqlmode &1 !
 SELECT *
 FROM tc, (SELECT *
       FROM TB, TA)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB, TA), tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), TA, tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM TA, (SELECT *
       FROM TB), TC!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TA, tc), (select * from TB) !
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA, tc)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA), tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA), (select * from tc)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
rollback !
 SELECT *
 FROM (SELECT *
       FROM (select * from TB, TA), (select * from tc))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM (select *
             from TB, (select * from TA)), (select * from tc))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM (select * from TB, (select * from TA, (select * from tc))))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
rollback !
 SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
       FROM (select * from TB, (select * from TA,
       (select * from tc))))))))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 *----------------------------------------------------
 *
 *  create view mit from select ist z. Zt. abgeklemmt.
 *
 *----------------------------------------------------
 * file fromselv !
commit work release !
