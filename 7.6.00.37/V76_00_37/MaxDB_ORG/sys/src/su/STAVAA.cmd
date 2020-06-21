*** STAVAA.cmd ***
* Aufruf in STAVA.cmd
*
* Anlegen der untersten VIEWs
CREATE VIEW VE1 AS SELECT K,N FROM &1
                   WHERE I = 1!
CREATE VIEW VW1 AS SELECT K,N FROM &1
                   WHERE I = 1!
CREATE VIEW VS1 AS SELECT K,N FROM &1
                   WHERE I = 1!
*
* Anlegen der MONSTER-VIEWs
CREATE VIEW VX1 AS SELECT T1.K,T1.N FROM VE1 T1,VW1 T2,VS1 T3 
WHERE T1.K = T2.K AND T1.N = T2.N 
  AND T3.N = T2.N AND T3.K = T2.K!
*
CREATE VIEW VY1 AS SELECT N,K FROM VE1
                    UNION ALL
                    SELECT N,K FROM VW1
                    UNION ALL
                    SELECT N,K FROM VS1!
*
CREATE VIEW VZ1 AS SELECT T1.K,T1.N FROM VE1 T1,VW1 T2,VS1 T3 
                    WHERE T1.K = T2.K OR T1.N = T2.N 
                    OR T3.N = T2.N OR T3.K = T2.K
                    OR T2.K = T3.N OR T1.N = T2.K!
*
CREATE VIEW VA1 AS SELECT K,N 
                    FROM VE1 
                    WHERE K IN 
                          (SELECT N FROM VW1
                           WHERE K IN 
                                (SELECT N FROM VS1
                                 WHERE (K = 2 * N) OR (K = 1)))!
*
CREATE VIEW VB1 AS SELECT K,N 
                    FROM VE1 
                    WHERE (K IN 
                             (SELECT N FROM VW1
                              WHERE K = 1))
                      AND (N IN    
                             (SELECT K FROM VS1
                              WHERE K = 1))!
*
CREATE VIEW VC1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N)
  AND T0.N IN
              (SELECT K FROM VS1 T2
               WHERE N = T0.K)!
*
CREATE VIEW VD1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T0.N IN
                                    (SELECT N FROM VS1 T2
                                     WHERE K = T0.K))!
*
CREATE VIEW VF1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T0.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VG1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T0.K))!
*
CREATE VIEW VH1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VI1 AS SELECT N FROM VE1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K AND K IS NULL))!
*
CREATE VIEW VJ1 AS SELECT N FROM VE1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K AND T0.K IS NULL))!
*
CREATE VIEW VK1 AS SELECT T0.N FROM VE1 T0
WHERE T0.N IN
              (SELECT T1.N FROM VW1 T1, VS1 T2
               WHERE T1.N = T0.N
                 AND T2.N = T0.K)!
*
FILE STAVAS( 1!
* 
COMMIT!
