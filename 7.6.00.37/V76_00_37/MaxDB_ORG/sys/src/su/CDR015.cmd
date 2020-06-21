*   *ID* CDR015   DBCMD    changed on 1992-11-11-17.01.55 by KERN      *
file connect ( sun sun sqlmode ansi !
INSERT INTO SIZ1_P VALUES
         ('E1','TTT',1,'SSS',10,'RRR','HHH','YYY',20)!
INSERT INTO SIZ1_P VALUES
         ('E1','TTS',1,'SSS',10,'RRR','HHH','YYY',21)!
INSERT INTO SIZ1_P VALUES
         ('E2','TTT',1,'SSS',10,'RRR','HHH','YYY',22)!
INSERT INTO SIZ1_P VALUES
         ('E3','TTT',1,'SSS',11,'RRR','HHH','YYY',23)!
INSERT INTO SIZ1_P VALUES
         ('E4','TTT',2,'SSS',10,'RRR','HHH','YYY',24)!
INSERT INTO SIZ1_P VALUES
         ('E1','TTS',3,'SSS',10,'RRR','HHH','YYY',25)!
INSERT INTO SIZ1_P VALUES
         ('E2','TTT',1,'SSS',10,'TRR','HHH','YYY',26)!
INSERT INTO SIZ1_P VALUES
         ('E3','TTT',4,'SSS',11,'RRR','HHH','YYY',27)!

INSERT INTO SIZ1_F VALUES
         ('E1','TTT',1,'SSS',10,'RRR','YYY',90,20)!
INSERT INTO SIZ1_F VALUES
         ('E1','TTS',1,'SSS',10,'RRR','YYY',91,20)!
INSERT INTO SIZ1_F VALUES
         ('E2','TTT',1,'SSS',10,'RRR','YYY',92,20)!
INSERT INTO SIZ1_F VALUES
         ('E3','TTT',1,'SSS',11,'RRR','YYY',93,20)!
INSERT INTO SIZ1_F VALUES
         ('E4','TTT',2,'SSS',10,'RRR','YYY',94,20)!
INSERT INTO SIZ1_F VALUES
         ('E1','TTS',3,'SSS',10,'RRR','YYY',95,20)!
INSERT INTO SIZ1_F VALUES
         ('E2','TTT',1,'SSS',10,'TRR','YYY',96,20)!
INSERT INTO SIZ1_F VALUES
         ('E2','TTT',1,'SSS',10,'TRR','YYY',97,20)!
pars_then_ex !
DELETE FROM SIZ1_F
        WHERE F1 = 'E1'!
UPDATE SIZ1_P
        SET S1 = 'E9'
        WHERE S1 = 'E1' AND S2 = 'TTT'!
SELECT S1 INTO :EMPNO1
        FROM SIZ1_P
        WHERE S1 = 'E9'!
pars_execute !
rollback work release !
