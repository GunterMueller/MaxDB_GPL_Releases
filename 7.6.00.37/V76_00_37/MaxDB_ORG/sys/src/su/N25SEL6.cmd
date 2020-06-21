*   *ID* N25SEL6  DBCMD    changed on 1992-09-28-13.25.48 by ADMIN     *
file connect ( kern test !
CREATE TABLE  relship
(  defobjtype     CHAR (8)  KEY,
   defuser        CHAR (8)  KEY,
   defobjn1       CHAR (18) KEY,
   defobjn2       CHAR (18) KEY,
   refobjtype     CHAR (8)  KEY,
   refuser        CHAR (8)  KEY,
   refobjn1       CHAR (18) KEY,
   refobjn2       CHAR (18) KEY,
   reltype        CHAR (18) RANGE IN ( 'CONTAINS', 'CALLS', 'OWNER',
                                       'DERIVED_FROM', 'USES', 'ALIAS',
                                       'PERMISSION_FOR', 'COMMENTED' ),
   createdate     DATE,
   createtime     TIME,
   privilegs      CHAR( 19) ) !
CREATE VIEW entity_contains ( defobjtype, defuser, defobjn1,
                      defobjn2, refobjtype, refuser,
                      refobjn1, refobjn2, reltype )
           AS  SELECT defobjtype, defuser, defobjn1,
                      defobjn2, refobjtype, refuser,
                      refobjn1, refobjn2, reltype
               FROM   relship
               WHERE  (reltype,    defobjtype, refobjtype)
                  in (('CONTAINS', 'ENTITY',   'ATTR'))
           WITH CHECK OPTION !
CREATE TABLE  attribute_type
(  etype_name     CHAR (18) KEY,
   desc_name      CHAR (32),
   authid         CHAR (8),
   createdate     DATE,
   upd_author     CHAR (8),
   last_upd       DATE,
   COMMENTS       CHAR (80) ) !
insert entity_contains values
   ( 'ENTITY','BARBARA','TABLE', ' ',
     'ATTR','BARBARA','AUTHID',' ','CONTAINS' ) !
insert attribute_type values
  ( 'AUTHID', 'Author des Eintrags', 'BARBARA',null,null,null,'aa' ) !
select defobjn1, refobjn1, desc_name
  from attribute_type, entity_contains
  where refobjn1 = etype_name !
fetch !
select defobjn1, refobjn1, desc_name
  from entity_contains, attribute_type
  where refobjn1 = etype_name !
fetch !
commit work release !
