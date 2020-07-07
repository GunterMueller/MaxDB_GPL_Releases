
/* @(#) $Id: //sapdb/V76/c_00/b_32/sys/src/sap/700/nixx.h#1 $ SAP*/
/*####################################################################*/
/*
 *    SAP AG Walldorf
 *     Systeme, Anwendungen und Produkte in der Datenverarbeitung
 *
 *     (C) Copyright SAP AG 1991 - 1997
 */
/*####################################################################*/



/*include-------------------------------------------------------------*/
/*                                                                    */
/* Beschreibung:                                                      */
/* ============                                                       */
/*                                                                    */
/* nixx.h:   ni = Projekt Network-Interface                           */
/*           xx = OS-unabhaengiges Coding                             */
/*                                                                    */
/* nixx.h enthaelt die Deklarationen fuer das Network-Interface.      */
/* Das Network-Interface bietet Dienste fuer die Interprozess-        */
/* Kommunikation ueber Rechnergrenzen hinweg an.                      */
/*                                                                    */
/* Ein einfaches Beispielprogramm fuer Client und Server ist          */
/* niexample (Modul niexampl.c). Hier findet man auch einfuehrende    */
/* Worte in die Programmierung mit NI.                                */
/*                                                                    */
/* Die ni-Bibliothek (nilib.o, Makefile ni.mak) kann mit              */
/* dem Programm 'niping' getestet werden (Modul nixxtst.c, Makefile   */
/* ni.mak)                                                            */
/*                                                                    */
/*-NiQue--------------------------------------------------------------*/
/*                                                                    */
/* Verbergen der Fragmentierung mit interner Queue (NiBuf):           */
/* ========================================================           */
/*                                                                    */
/* Die alte NI-Schicht verbirgt die Fragmentierung der                */
/* Pakete durch die Netzwerkschicht nicht.                            */
/* Dies fuehrt zu immer gleichem Coding in den Anwendungen.           */
/* Dies kann jetzt umgangen werden. Dafuer gibt es                    */
/* verschiedene Moeglichkeiten:                                       */
/*                                                                    */
/* 1) Man ersetzt alle Aufrufe von NiInit, NiConnect und              */
/*    NiListen durch die entsprechenden NiBuf*                        */
/*    Aufrufe. Ansonsten braucht man nichts zu aendern.               */
/* 2) In einer zweiten Stufe kann man die NiRead und                  */
/*    NiWrite-Aufrufe durch andere Calls ersetzen, die                */
/*    verhindern, dass die Daten kopiert werden muessen.              */
/*    Siehe unten "Neue Puffer-API"                                   */
/*                                                                    */
/* Es wird dann sichergestellt, dass entweder das ganze Paket oder    */
/* gar nichts verarbeitet wird.                                       */
/* Durch setzen der Parameter NIBUFP_MAXQUEUE und NIBUFP_MAXHEAP kann */
/* man die interne Queue aktivieren. Hierbei bedeutet NIBUFP_MAXQUEUE */
/* die maximale Queuelaenge pro handle und NIBUFP_MAXHEAP die         */
/* maximale Groesse aller Queues.                                     */
/* Falls die Queue voll ist wird zunaechst NIEQUE_FULL                */
/* zurueckgegeben, das Paket aber verarbeitet. Erst beim naechsten    */
/* Versuch wird mit NIETIMEOUT signalisiert, dass das Paket nicht     */
/* verarbeitet wurde. Nach NIEQUE_FULL sollte man mit NiWriteOK       */
/* sicherstellen, dass die Queue abgearbeitet wurde.                  */
/* Hinweis: Bei Ueberschreiten von NIBUFP_MAXHEAP wird immer nur      */
/* NIEQUE_FULL zurueckgegeben, aber die Queues trotzdem               */
/* erweitert. Dies ist nur ein Softlimit.                             */
/* NIEPING wird nur noch signalisiert, wenn als Antwort auf ein       */
/* NICheck direkt NI_PONG empfangen wird. D.h., wer NiCheck nicht     */
/* aufruft, bekommt NIEPING nie als Fehler zurueck                    */
/*                                                                    */
/* NiBufInit           Initialisierung der nilib und initialisierung  */
/*                     der internen Pufferung. Diese Funktion muss vor*/
/*                     den anderen NiBuf-Aufrufen gerufen werden      */
/* NiBufListen         Vorbereitungen fuer Connect-Requests von       */
/*                     Clients treffen (Server). Die per NiAccept auf */
/*                     diesem Handle erzeugten NI-Handles verbergen   */
/*                     die Fragmentierung                             */
/* NiBufConnect        Verbindung zu Server aufbauen. Der             */
/*                     resultierende NI-Handle verbirgt die           */
/*                     Fragmentierung                                 */
/*                                                                    */
/* Nonblocking Connects:                                              */
/* ====================                                               */
/* Ab Version 31 unterstuetzt NiBuf nonblocking Connects auf          */
/* Plattformen, die das bereitstellen:                                */
/* Nach NiBuf*Connect kann NIECONN_PENDING ignoriert werden und die   */
/* Pakete werden dann in die Queue gestellt. (Achtung: Wer            */
/* NIECONN_PENDING _nicht_ ignoriert, muss den Handle selber          */
/* schliessen!)                                                       */
/* Es ist damit moeglich Connects mit timeout 0 (NI_ASYNC) aufzurufen.*/
/*                                                                    */
/*-NiBuf--------------------------------------------------------NiQue-*/
/*                                                                    */
/* Neue Puffer-API:                                                   */
/* ===============                                                    */
/*                                                                    */
/* Diese API basiert auf der NiBuf-ERweiterung und bietet ein paar    */
/* neue Funktionen, die NiRead/Write ersetzen, bzw. Hilfsfunktionen   */
/* dazu.                                                              */
/* Diese Funktionen arbeiten nicht mit beliebigen Datenbereichen, in  */
/* die und aus denen dann kopiert werden muss, sondern es werden      */
/* Pointer auf Strukturen vom Typ NIBUFFER uebergeben. Dies fuehrt zu */
/* deutlich kuerzeren Laufzeiten als bei NiRead/Write                 */
/*                                                                    */
/* Die Structur NIBUFFER hat zwei oeffentliche Variablen:             */
/* struct NIBUFFER                                                    */
/* {                                                                  */
/*    SAP_RAW *data;                                                  */
/*    SAP_INT  len;                                                   */
/*    ...                                                             */
/* }                                                                  */
/* data ist ein Zeiger auf den Anfang des Datenbereichs und len ist   */
/* die Laenge desselben.                                              */
/*                                                                    */
/* Man erhaelt solche Puffer implizit ueber NiBufReceive und kann sie */
/* explizit ueber NiBufAlloc anfordern                                */
/* Diese Puffer muessen dann aber auch wieder freigegeben werden.     */
/* Dies geschieht wieder implizit ueber NiBufSend oder explizit ueber */
/* NiBufFree.                                                         */
/* Genauso,wie bei NiBufRead/Write wird garantiert, dass ein NiBufSend*/
/* nach einem NiBufReceive nicht mit NIETIMEOUT zurueckkehrt.         */
/* Diese Funktionen koennen auch mit NiRead/Write vermischt werden.   */
/*                                                                    */
/* Funktionen:                                                        */
/*                                                                    */
/* NiBufReceive    Empfange einen NIBUFFER                            */
/* NiBufSend       Sende einen NIBUFFER                               */
/*                                                                    */
/* NiBufFree       Gebe einen NIBUFFER wieder frei                    */
/* NiBufAlloc      Allokiere einen NIBUFFER                           */
/* NiBufWrap       'Verpacke' einen Datenbereich in einem NI-Puffer   */
/* NiBufUnWrap     'Entpacke' einen mit NiBufWrap                     */
/*                            erzeugten NI-Puffer                     */
/* NiBufDup        Dupliziere einen NIBUFFER                          */
/*                                                                    */
/* NiBufPush       Lege einen Puffer auf einen Stack                  */
/* NiBufPop        Hole einen Puffer vom Stack                        */
/*                                                                    */
/* Ueber den Handle-Parameter NIHP_ALLOC_FUNC kann man eine Funktion  */
/* registrieren, die beim Empfangen eines Pakets gerufen wird, um     */
/* einen Puffer zu allokieren.                                        */
/* Dies ermoeglicht zusammen mit NiBufWrap, auch z.B. direkt aus      */
/* shared Memory heraus zu senden, ohne kopieren zu muessen           */
/*                                                                    */
/*--------------------------------------------------------------NiBuf-*/
/*-NiSel--------------------------------------------------------------*/
/*                                                                    */
/* Um die folgenden  Funktionen nutzen zu koennen, muss zusaetzlich   */
/* zur nilib die nisellib und die filib gebunden werden.              */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*                                                                    */
/* Neue Select-API:                                                   */
/* ===============                                                    */
/*                                                                    */
/* Das alte NiSelect ist dadurch sehr langsam, dass es einen          */
/* eigenen Typ fuer die selectmasken definiert. Diese muessen         */
/* bei jedem Aufruf in die socket-masken umgesetzt                    */
/* werden.                                                            */
/*                                                                    */
/* Die neue API verwaltet jetzt nur noch intern                       */
/* socket-Masken. Ausserdem kann der Aufrufer eigene Daten            */
/* hinterlegen, die den Zugriff auf seine Daten beschleunigt.         */
/*                                                                    */
/* Dafuer muessen die Selectsets jetzt angefordert und                */
/* explizit wieder freigegeben werden.                                */
/*                                                                    */
/* Ein typischer Ablauf waere:                                        */
/* {                                                                  */
/*   NISEL_HDL set;                                                   */
/*   SAP_BOOL  read;                                                  */
/*                                                                    */
/*   NiSelNewSet (&set);                                              */
/*                                                                    */
/*   NiSelSet (set, nihdl, NI_CONNECT, NULL);                         */
/*   [...]                                                            */
/*   while (something_to_do)                                          */
/*   {                                                                */
/*     NiSelSelect (set, timeout);                                    */
/*     while (NiSelNext (set, &read, NULL, &conn, &myclient)          */
/*            != NI_INVALID_HDL)                                      */
/*     {                                                              */
/*       if (conn)                                                    */
/*       {                                                            */
/*         MyConnect(); // hier wird irgendwo                         */
/*            // NiSelSet (set, newnihdl, NI_READ, myclient)          */
/*            // gerufen                                              */
/*       }                                                            */
/*       if (read)                                                    */
/*         Myread (myclient);// hier wird evtl.                       */
/*            // NiSelSet (set, myclient->nihdl, NI_WRITE,            */
/*            //           myclient)                                  */
/*            // gerufen                                              */
/*       if (write)                                                   */
/*         MyWrite (myclient);                                        */
/*     }                                                              */
/*   }                                                                */
/*   NiSelDestroySet (&set);                                          */
/* }                                                                  */
/*                                                                    */
/*-NiSnc--------------------------------------------------------NiSel-*/
/*                                                                    */
/* Transparente Integration von SNC (Secure Network Communication):   */
/* ===============================================================    */
/*                                                                    */
/* Es ist moeglich, NI-vebindungen verschluesseln zu lassen. Hierzu   */
/* wurde NI so erweitert, dass die SNC-Library transparent gerufen    */
/* wird.                                                              */
/* Die Loesung basiert auf NiBuf (s.o.). Es muessen NiInit, NiConnect */
/* und NiListen durch NiSncInit, NiSncConnect und NiBufListen ersetzt */
/* werden.                                                            */
/* NiSncInit erhaelt als Parameter den eigenen SNC-Namen.             */
/* NiConnect erhaelt den SNC-Namen des Partners. Falls Dieser leer    */
/* ist, verhaelt sich NiSncConnect wie NiBufConnect                   */
/* Nach dem NiAccept kann man, nachdem der Handle Schreib-oder        */
/* Lesebereit ist, per NiSncGetPeer erfragen, ob die Verbindung       */
/* verschluesselt ist und wie der Partner heisst                      */
/* Um diese Funktionen nutzen zu koennen, muss zusaetzlich zur nilib  */
/* die nisnclib gebunden werden. Diese braucht zusaetzlich die filib, */
/* snclib und sappar0/sapparam.                                       */
/*                                                                    */
/* NiSncInit          Ersetzt NiInit (muss gerufen werden)            */
/* NiSncConnect       Ersetzt NiConnect                               */
/* NiSnc2Connect      Ersetzt Ni2onnect                               */
/* NiSncGetPeer       Gibt den evtuellen SNC-Account des Partners     */
/*                    zurueck                                         */
/* NiSncRequest       Fordert eine SNC-Verschluesselung zu            */
/*                    bestehender Verbindung an                       */
/* NiSncAclStr        Gibt einen lesbaren String fuer                 */
/*                    einen SNC-Account zurueck                       */
/* NiSncNameToAcl     Macht aus einem lesbaren SNC-Namen einen        */
/*                    internen                                        */
/* NiSncAclCmp        Vergleicht zwei interne SNC-Namen               */
/*                                                                    */
/*--------------------------------------------------------------NiSnc-*/
/*-NiSendHandle-------------------------------------------------------*/
/*                                                                    */
/* Mit der Funktion NiSendHandle kann ein Ende einer socket-Verbindung*/
/* ueber eine zweite lokale socket-Verbindung verschickt werden.      */
/* Diese Funktionalitaet ist fuer Unix und Windows NT (mit WinSock2)  */
/* implementiert.                                                     */
/* Die Uebertragung erfolgt in den mehreren Einzelschritten. Der      */
/* client ruft NiSendHandle, um den socket zu uebertragen.            */
/* Die empfangende Seite muss den Socket, ueber den empfangen werden  */
/* soll, in der read-Maske haben. Die Uebertragung wird in der select */
/* loop des servers mit dem status 'connect' angezeigt. Der server    */
/* kann die Verbindung mit NiAccept annehmen und anschliesend ueber   */
/* die empfangene Verbindung kommunizieren.                           */
/*                                                                    */
/*-------------------------------------------------------NiSendHandle-*/
/*--------------------------------------------------------------------*/
/*                                                                    */
/* allgemeine Dienste                                                 */
/* ==================                                                 */
/* NiInit              Initialisierung der nilib                      */
/*                     Sollte immer vor Benutzen von Ni-Funktionen    */
/*                     gerufen werden.                                */
/* NiThrInit           Thread specific initialization                 */
/* NiExit              Aufraeumen der Resourcen                       */
/* NiThrExit           Thread specific cleanup                        */
/* NiCloseHandle       Handle freigeben, evtl. Verbindung abbauen     */
/* NiShutdownHandle    Verbindung einseitig schliessen                */
/* NiCloseAll          alle offenen Handles freigeben                 */
/* NiSelect            Auf Daten warten                               */
/* Ni2Select           Auf Daten und freie Sendeleitungen warten      */
/* NiPeek              Testen, ob Daten fuer Handle angekommen        */
/* NiWriteOK           Testen, ob Write auf Handle blockieren wuerde  */
/* NiRouteToTable      Zerlege einen Routestring                      */
/* NiHostToAddr        Hostnamen in Adresse umwandeln                 */
/* NiAddrToHost        Adresse in Hostnamen umwandeln                 */
/* NiAddrToHost2       Adresse in Hostnamen umwandeln (reentrant)     */
/* NiServToNo          Service-Namen in (Port-)Nummer umwandeln       */
/* NiNoToServ          (Port-)Nummer in Service-Namen umwandeln       */
/* NiNoToServ2         (Port-)Nummer in Service-Namen umw. (reentrant)*/
/* NiMyHostName        eigenen Hostnamen ermitteln                    */
/* NiMyAddrVerify      eigene Host-Adresse verifizieren               */
/* NiReadPending       noch zu lesende Bytes (letztes Paket)          */
/* NiWritePending      noch zu schreibende Bytes (letztes Paket)      */
/* NiWait              n Millisekunden warten                         */
/* NiGetParam          allg. NI-Parameter lesen                       */
/* NiSetParam          allg. NI-Parameter setzen                      */
/* NiGetParamEx        allg. NI-Parameter lesen (beliebige Datentypen)*/
/* NiSetParamEx        allg. NI-Parameter setzen(beliebige Datentypen)*/
/* NiHBufPrintBuf      Dump the Ni-Hostbuffer to a (UC) Char - buffer */
/* NiErrSet            Sap-Error-Info zu NI-Returncode bereitstellen  */
/* NiErrStr            Namens-String  zu Ni-Error-Konstante liefern   */
/* NiSecStr            SapRouter-Passwort aus String ausblenden       */
/* NiSecStr2           SapRouter-Passwort aus String ausb. (reentrant)*/
/* NiHdlToSock         Gebe den Socket zu einem NI-Handle zurueck     */
/* NiSockToHdl         Gebe den NiHandle zu einem Socket zurueck      */
/* NiHdlGetPeer        Gebe den Remote-Peer zu einem Handle zurueck   */
/* NiHdlGetName        Gebe die lokale Adresse zu einem Handle zurueck*/
/* NiHdlSetBlockmode   Hdl blocking oder nonblocking setzen           */
/* NiHdlGetBlockmode   pruefen ob Hdl blockierend oder nicht ist      */
/* NiHdlGetStatus      Gebe den Verbindungsstatus eines Handle zurueck*/
/* NiLocalCheck        Check ob NI_HOST_ADR lokal ist                 */
/* NiGet<level>_<opt>  Get socket option for specified protocol level */
/* NiSet<level>_<opt>  Set socket option for specified protocol level */
/* NiSetTraceAll       NI-Trace fuer alle activen Handles aktivieren  */
/* NiClearTraceAll     NI-Trace fuer alle activen Handles loeschen    */
/* NiHdlDump           Informationen fuer ein Handle ins Trace dumpen */
/* NiDumpNiHdls        Informationen fuer alle aktiven Handles dumpen */
/*                                                                    */
/* Dienste f. verbindungsorientierte Kommunikation (Stream, "Telefon")*/
/* ===================================================================*/
/* NiListen            Vorbereitungen fuer Connect-Requests von       */
/*                     Clients treffen (Server)                       */
/* NiConnect           Verbindung zu Server aufbauen                  */
/* NiAccept            Verbindung zu Client aufbauen                  */
/* NiWrite             Daten an Partner senden                        */
/* NiRead              Daten vom Partner empfangen                    */
/* NiCheck             Verbindung zum Partner ueberpruefen            */
/* Ni2Listen           wie NiListen, aber Service-Nummer als Input    */
/* Ni2Connect          wie NiConnect, Host-Adr + Serv-Nr als Input    */
/*                                                                    */
/* Alle Funktionen fuer Native TCP/IP Verbindungen nutzen direkt      */
/* die Systemfunktionen und benutzen NICHT die Pufferung (NiBuf):     */
/* NiRawConnect        wie NiConnect, Talkmode nach Verbindungsaufbau */
/*                     Native statt NI (fuer SapTelnet ..)            */
/* NiRawWrite          Daten an "Native-TCP-Partner" senden           */
/* NiRawRead           Daten von "Native-TCP-Partner" empfangen       */
/* NiRawPeek           Testen, ob Daten fuer Handle angekommen        */
/* NiRawWriteOK        Testen, ob Write auf Handle blockieren wuerde  */
/* NiHdlGetOutOctets   Anzahl der ueber Handle gesendeten Bytes       */
/* NiHdlGetInOctets    Anzahl der ueber Handle empfangenen Bytes      */
/* NiHdlGetOutMsgs     Anzahl der ueber Handle gesendeten Ni-Messages */
/* NiHdlGetInMsgs      Anzahl der ueber Handle empfangenen Ni-Messages*/
/* NiHdlGetStartTime   Startzeit der Verbindung zurueckgeben          */
/*                                                                    */
/*                                                                    */
/* Dienste fuer verbindungslose Kommunikation (Datagramm, "Brief")    */
/* ===============================================================    */
/* NiDgBind            Vorbereitungen fuer Empfang von Datagrammen    */
/*                     treffen                                        */
/* NiDgSend            Datagramm senden                               */
/* NiDg2Send           wie NiDgSend, aber Host-Adresse als Input      */
/* NiDgReceive         Datagramm empfangen                            */
/* NiDgGetHdl          intern benutzten Handle abfragen               */
/* NiDgFreeResrc       belegte "Datagramm-Resourcen" wieder freigeben */
/*-NiDatagramAPI------------------------------------------------------*/
/*                                                                    */
/* NiDgHdlCreate       Datagramm Handle erzeugen                      */
/* NiDgHdlBind         Datagramm-Handle an IP/Port binden             */
/* NiDgHdlBindName     Datagramm-Handle an IP/Port binden             */
/* NiDgHdlConnect      Datagramm-Handle an IP-Adresse verbinden       */
/* NiDgHdlConnectName  Datagramm-Handle an Hostnamen  verbinden       */
/* NiDgHdlSend         Daten an verbundenen Datagram-Handle schicken  */
/* NiDgHdlSendTo       Datagramm an fernen IP/Port schicken           */
/* NiDgHdlSendToName   Datagramm an fernen IP/Port schicken           */
/* NiDgHdlRecvFrom     Datagramm vom Netz empfangen                   */
/* NiDgHdlClose        Datagramm-Handle schliessen                    */
/*                                                                    */
/* -----------------------------------------------------NiDatagramAPI-*/
/* NiWakeupOpen        Wakeup Server erstellen                        */
/* NiWakeupClose       Wakeup Server schliessen                       */
/* NiWakeupGetNiHdl    Listen Handle des Wakeup Servers lesen         */
/* NiWakeupGetCntNo    Anzahl Clients lesen                           */
/* NiWakeupCollect     Empfangene Wakeup Requests lesen               */
/* NiWakeupAttach      Wakeup Client an Server binden                 */
/* NiWakeupDetach      Wakeup Client von Server trennen               */
/* NiWakeupExec        Wakeup Request an Server schicken              */
/*                                                                    */
/*-NiMulticast--------------------------------------------------------*/
/*                                                                    */
/* You may use NI in order to receive (IP-)multicast streams.         */
/* IP multicast is a mechanism to send data to multiple peers         */
/* simultaneously. Any peer wishing to receive data for a particular  */
/* group has to join that group. With IP groups are identified with   */
/* special IP-Addresses                                               */
/* (Class D-Addresses 224.0.0.1 - 239.255.255.255                     */
/*  high order byte starting with 1110 )                              */
/*                                                                    */
/* In order order to receive Multicast you have to:                   */
/*     Create a  Datagram Handle               (NiDgHdlCreate)        */
/*     Bind that handle to a port              (NiDgHdlBind/-Name)    */
/*     Join the group                          (NiDgHdlJoinGroup)     */
/*     Receive Packets for that group          (NiDgHdlRecvFrom)      */
/*                                                                    */
/* The sender may use the NiDgHdl API to send datagrams to a          */
/* particular Multicast Group:                                        */
/*     Create a  Datagram Handle               (NiDgHdlCreate)        */
/*     Send to  the group                      (NiDgHdlSendTo)        */
/*                                                                    */
/* Example: see nidgtst.c  (mapro krn/ni nidgtst)                     */
/*--------------------------------------------------------NiMulticast-*/
/* Dienste fuer 1-n Kommunikation (Multicast)                         */
/*====================================================================*/
/*                                                                    */
/* NiDgHdlJoinGroup     einer (IP-) Multicastgruppe beitreten         */
/* NiDgHdlLeaveGroup    eine  (IP-) Multicastgruppe verlassen         */
/*                                                                    */
/*end-----------------------------------------------------------------*/

#ifndef NIXX_H
#define NIXX_H

#ifdef __cplusplus
extern "C"
{
#endif


/*--------------------------------------------------------------------*/
/* Version der NI-Schicht                                             */
/*                                                                    */
/* NI_VERSION wird bei allen Aenderungen der NI-Schicht (inklusive    */
/* SapRouter) inkrementiert und bei NI-internen Messages uebers Netz  */
/* mitgesendet                                                        */
/*                                                                    */
/* 13  02.12.94 hq   Interface Erweiterung NiSetParam(), NiGetParam() */
/* 14  21.12.94 hq   Host/Serv-Buf VALID/UNKNOWN/alloc/flush/dump     */
/*                   NiSetParam() BUF_OPCOD, HSBUF_MAX, LIB_VERS      */
/*                   NiAdrToStr(), NiSrvToStr() speichern 10 Strings  */
/*                   SapRouter Queue Pre-Alloc                        */
/* 15  03.01.95 hq   Interface Erweiterung NiMyAddrVerify ()          */
/* 16  16.03.95 hq   Interface Erweiterung NiRawConnect()             */
/*                   fragmentierte Header jetzt erlaubt               */
/* 17  25.04.95 hq   niping client delay wg. keepalive Tests          */
/*                   SapRouter Def-Werte fuer -B /-Q von 100KB / 10MB */
/*                   auf 500 KB / 20 MB                               */
/*     22.05.95 hq   Def. fuer Routtab-Laenge von 1000 auf 2000       */
/* 18  06.09.95 hq   Maske von 21 auf 32 Int's erweitert              */
/*     18.09.95 hq   NiErrStr () aufgenommen                          */
/*     05.10.95 hq   NiSecStr () aufgenommen                          */
/*     16.10.95 hq   NiGetSapDllRelease () aufgenommen                */
/* 19  17.10.95 hq   NiPBind () und NiBind () erweitert (log-Param)   */
/*                   SapRouter Portrange-Feature                      */
/* 20  30.10.95 hq   SapRouter Bug unter NT gefixt (close in NiRAdmin)*/
/* 21  13.11.95 hq   ErrInfo-Makros ueberall eingebaut                */
/*                   NiPing SNC-Schicht integriert                    */
/*                   SAProuter RT_ROUTTAB_MAX_NO von 2000 auf 3000    */
/*                   SAProuter Permission Field Gross / Klein         */
/*     07.12.95 hq   Patch wg. ErrInfo in NiICloseHandle ...          */
/* 22  11.12.95 hq   Vorbereitung fur Umstellung auf Handshake-Protok.*/
/*                   NiIConnect als Zustandsautomat                   */
/* 23  20.12.95 hq   Bug in Connect-Fktn behoben (INVALID_HDL setzen) */
/* 24  27.02.96 hq   ErrInfo analog errno-Konzept, ni_compname global */
/*                   API's setzen bei Fehlern immer ErrInfo, machen   */
/*                   nie ErrClear, ErrSet ueberschreibt immer         */
/*                   SAProuter bug (Routtab vorm Neueinlesen loeschen)*/
/* 25  01.04.96 hq   SAProuter kommt runter, wenn keine Routtab da    */
/*                   oder wenn kein Log geschrieben werden kann       */
/* 26  06.05.96 cr   NiTcpConnect realisiert                          */
/*                   Passwordverfahren fuer SAPSERVX                  */
/*                   Saprouter von sapparam unabhaengig               */
/*     15.10.96 cr   NiRouteToTable                                   */
/*     29.10.96 cr   Saprouter laesst sich auch bei maximaler Anzahl  */
/*                   von clients administrieren                       */
/*                   Default fuer die maximale Anzahl erhoeht auf 800 */
/*     05.11.96 cr   Mehr Sicherheit bei der maximalen Anzahl         */
/*                   Dynamische Anpassung der Routtab                 */
/*                   NI_MAX_HDLS auf 2048                             */
/*     02.12.96 cr   Saprouter gibt Statusmeldung in Admin-requests   */
/*                   zurueck                                          */
/*     04.02.97 cr   Saprouter ersetzt in der iroute seinen Hostname  */
/*                   durch die IP-Addresse des Vorgaengers            */
/* 27  12.11.96 rw   NiPRead, NiPWrite bekommen jetzt Flag fuer       */
/*                   nonblocking                                      */
/*     02.01.97 cr   Threadsafe: NiRouteAdd, NiRouteConnect gestrichen*/
/*     31.01.97 hq   fragmentierte Header werden sauber behandelt     */
/*                   Fragment-Test-Modus gebaut (siehe NiSetParam (), */
/*                   niping -Q)                                       */
/*     12.02.97 cr   Threadsafe als Compileroption                    */
/* 28  21.03.97 hq   SNC in SAProuter eingebaut (Alpha Version !)     */
/*     25.04.97 cr   SNC in Saprouter erweitert                       */
/*                   NiSel-API realisiert       (Alpha Version !)     */
/*     20.06.97 cr   Administration des Saprouter von Remote im Server*/
/*                   verhindern                                       */
/* 29  11.07.97 cr   NiBuf und NiSnc realisiert und im Saprouter      */
/*                   integriert                                       */
/* 30  23.07.97 cr   Soft Shutdown im Router                          */
/*                   Administration ueber neue Requests anfordern     */
/*                   Workingdirectory und saprouttab im INFO-request  */
/*                   ausgeben                                         */
/*     29.07.97 cr   NiSnc, Raw-Modus und Test von NiBuf in niping    */
/*     13.08.97 cr   NiRawRead gab zu frueh NIECONN_BROKEN zurueck    */
/*     15.08.97 cr   Mehrfache FD_SET's vermeiden fuer NT             */
/*                   NiBuf mit in die nilib                           */
/*     03.09.97 cr   NI_FINDSERV_OS implementiert fuer sapftp         */
/*                   REUSEADDR unter NT abgeschaltet                  */
/*     21.10.97 cr   Parsing der Source-Addresse im Router repariert  */
/* 31  31.10.97 cr   NiQueue implementiert                            */
/*     27.11.97 cr   Nonblocking Connect                              */
/*     03.12.97 cr   Server koennen jetzt auch Verschluesselung       */
/*                   nachfordern.                                     */
/*                   Hostnamenpuffer kann das Retry fuer revers       */
/*                   lookups abschalten                               */
/*                   Local bind ist jetzt moeglich.                   */
/*                   (diese drei Features werden im Saprouter jetzt   */
/*                   genutzt)                                         */
/*     11.12.97 cr   NiBufWrap/NiBufDup                               */
/*                   Nonblocking connect mit extra Funktion           */
/*     12.01.98 cr   EWOULDBLOCK auch pruefen                         */
/* 32  17.09.98 cr   Local port selection fixed                       */
/*                   Saprouter uses NT-Eventlog for fatal errors      */
/* 33  19.02.98 cr   NiDgSend optimiert                               */
/*                   AF_UNIX fuer localhost-Kommunikation             */
/*     11.05.98 cr   NiDgSend war nicht optimal                       */
/*                   Ni{Host,Net}INT in nixx.h                        */
/*     16.07.98 cr   Interne Handles auf Pointer umgestellt           */
/*                   Queuelimit jetzt per Packet und nicht per Byte   */
/*     18.02.99 ol   NiGetHostAddrList eingebaut                      */
/*      7.06.99 ol   NiCheck2 implementiert                           */
/*      8.07.99 ol   Konfigurierbarer Pfad fuer Unix Domain Sockets   */
/* 34  30.08.99 ol   SNC Fehler mit SAProuter (NIVERSION 34)          */
/*      9.08.99 ol   NiRawPeek und NiRawWriteOK implementiert. NiPeek */
/*                   und NiWriteOK fuellen intern die Puffer, die dann*/
/*                   fuer NiRawRead verloren gehen.                   */
/*     11.10.99 ol   NiShutdownHandle implementiert                   */
/*     30.03.00 cps  NiSncFinHS implementiert und NiDgSend            */
/*                   erneut verbessert                                */
/* 35  10.04.00 cps  SAProuter kann jetzt Prozesse nachstarten,       */
/*                   falls keine Slots mehr frei sind (UNIX only)     */
/*              ol   FI fuer die Suche nach freien slots in nitab     */
/*              cps  Poll statt Select unter UNIX                     */
/* 36  12.11.01 cps  queue Test als Parameter                         */
/*              cps  maximale Message Laenge  als Parameter           */
/*                   im saprouter beim Verbindungsaufbau realisiert   */
/*              cps  Versionierung des saprouter                      */
/*              cps  Erweitertes tracing mit NiHandles                */
/* 37  26.02.02 cps  UNIX jetzt ohne select                           */
/*                   nitab dynamisch allokiert, #hdls > 2048 moeglich */
/*              cps  Ermitteln der maximalen Anzahl von sockets, die  */
/*                   man maximal  oeffnen kann implementiert          */
/*              cps  NI_MAX_HDLS auf 16K, maximale Anzahl der Handles */
/*                   jetzt als Parmameter in NiInit2                  */
/*     09.04.02 cps  Datagramm Schnittstelle/ Multicast implementiert */ 
/*     17.05.02 ms   Ni*Raw2Connect eingebaut                         */
/*     22.05.02 ms   Neuer Parameter-Typ fuer HostAdressen            */
/*     26.07.02 ms   Wakeup mechanism                                 */
/*     30.08.02 ms   Functions to get / set socket options            */
/*     11.09.02 ms   Reentrant hostname / address caching             */
/*     12.11.02 ms   Update function specification                    */
/*     13.08.03 ms   New function parameter for NiSncAclStr           */
/*     30.09.03 ms   Update datagram documentation                    */
/* 38  13.11.03 ms   New struct NI_NODEADDR and functions             */
/*     11.05.04 ms   new function NiGetHdlProto                       */
/*                   new function NiGetMacAddrList                    */
/*     03.06.04 ms   move struct NI_HOSTADR to NI_NODEADDR            */
/*     18.10.04 ms   switch to disable error traces NiEnableTrace /   */
/*                   NiDiableTrace                                    */
/*                                                                    */
/*--------------------------------------------------------------------*/

#define NI_COMPNAME             cU("NI (network interface)")
#define NI_VERSION              38
#define NI_LAST_MODIFIED        cU("October 18 2004")

/* ################################################################## */
/* # constants                                                      # */
/* ################################################################## */

/* Fehler-Konstanten--------------------------------------------------*/
#define NIEINTERN            -1    /* NI-interner Fehler              */
#define NIEHOST_UNKNOWN      -2    /* Hostname unbekannt              */
#define NIESERV_UNKNOWN      -3    /* Service unbekannt               */
#define NIESERV_USED         -4    /* Service already used            */
#define NIETIMEOUT           -5    /* Zeitlimit ueberschritten        */
#define NIECONN_BROKEN       -6    /* Verbindung zum Partner abgebr.  */
#define NIETOO_SMALL         -7    /* Daten-bereich zu klein          */
#define NIEINVAL             -8    /* ungueltiger Parameter           */
#define NIEWAKEUP            -9    /* Wake-Up (ohne Daten)            */
#define NIECONN_REFUSED      -10   /* Verbindungsaufbau misslungen    */
#define NIEPING              -11   /* PING/PONG-Signal erhalten       */
#define NIECONN_PENDING      -12   /* Verbindung zum Partner ueber    */
                                   /* NiRouter noch nicht aufgebaut   */
#define NIEVERSION           -13   /* ungueltige Version              */
#define NIEMYHOSTNAME        -14   /* eigener Hostname nicht ermittelb*/
#define NIENOFREEPORT        -15   /* kein freier Port innerhalb Range*/
#define NIEMYHOST_VERIFY     -16   /* eigener Hostname ungueltig      */
#define NIESNC_FAILURE       -17   /* Fehler in der SNC Schicht       */
                                   /* im saprouter ==>                */
#define NIEOPCODE            -18   /* Opcode received                 */
#define NIEQUE_FULL          -19   /* Queue limit erreicht, naechstes */
                                   /* Paket wird nicht akzeptiert     */ 
#define NIETOO_BIG           -20   /* Angefordertes Paket zu gross    */
#define NIEACCEPT            -21   /* Accepted handle pending         */
#define NIEPASSED            -22   /* Handle passed to other process  */
#define NIENO_ANSWER         -23   /* Partner hat nicht geantwortet   */
#define NIECALL              -24   /* Ungueltiger Funktionsaufruf     */

#define NIEROUT_HOST_UNKNOWN -90   /* Hostname unbekannt              */
#define NIEROUT_SERV_UNKNOWN -91   /* Service unbekannt               */
#define NIEROUT_CONN_REFUSED -92   /* Verbindungsaufbau misslungen    */
#define NIEROUT_INTERN       -93   /* NI-interner Fehler              */
#define NIEROUT_PERM_DENIED  -94   /* Connect von Source an           */
                                   /* Destination verboten            */
#define NIEROUT_CONN_BROKEN  -95   /* Verbindung abgebrochen          */
#define NIEROUT_VERSION      -96   /* ungueltige Client-Version       */
#define NIEROUT_CANCELED     -97   /* Verbindung von Administrator    */
                                   /* ge"canceled"                    */
#define NIEROUT_SHUTDOWN     -98   /* saprouter shutdown              */
#define NIEROUT_INFO_DENIED  -99   /* Informations Request abgelehnt  */
#define NIEROUT_OVERFLOW    -100   /* max. Anzahl Clients erreicht    */
#define NIEROUT_MODE_DENIED -101   /* talkmode not allowed            */
#define NIEROUT_NOCLIENT    -102   /* client nicht vorhanden          */
#define NIEROUT_EXTERN      -103   /* error in external library       */
#define NIEROUT_SNC_FAILURE -104   /* Fehler in der SNC Schicht       */

#define NI_ISROUTERR(nirc)   ((nirc) <= NIEROUT_HOST_UNKNOWN)
#define NI_ISLIBERR(nirc)    ((nirc) == NIEROUT_SNC_FAILURE || \
                              (nirc) == NIESNC_FAILURE)
#define NI_ISNONFATAL(nirc)  (NI_ISLIBERR(nirc) ||    \
                              (nirc) == NIEPING ||    \
                              (nirc) == NIEVERSION || \
                              (nirc) == NIEROUT_VERSION)

/* Konstanten fuer timeout-Parameter----------------------------------*/
#define NI_BLOCK            -1
#define NI_ASYNC             0

/* Konstanten fuer servNo-Parameter-----------------------------------*/
#define NI_FINDSERV         (SAP_USHORT) -1 /* Hier suchen wir selber */
#define NI_FINDSERV_OS      (SAP_USHORT)  0 /* Hier sucht das OS      */

/* Konstante fuer handle-Parameter------------------------------------*/
#define NI_INVALID_HDL      (NI_HDL)  -1
#define NI_INVALID_SOCK     (SAP_INT) -1

/* Default fuer SapRouter-Service-------------------------------------*/
#define NI_ROUTER_SERVICE   cU("sapdp99")
#define NI_ROUTER_SERVNO    cU("3299")
#define NI_ROUTER_SERVNO_N  (SAP_USHORT) 3299

/* Separatoren fuer Routen im Hostnamen-------------------------------*/
#define NI_HOSTSEP     cU("/H/")  /* Separator fuer Hostname in Route */
#define NI_SERVSEP     cU("/S/")  /* Separator fuer Service in Route  */
#define NI_PASSSEP     cU("/P/")  /* Separator fuer Passwort in Route */
#define NI_PAS2SEP     cU("/W/")  /* Separator fuer Passwort in Route */
#define NI_KRPTSEP     "/K/"      /* Separator fuer SNC-Name          */
/* reservierte Separatoren fuer zukuenftige Entwicklung, momentan     */
/* 'extern' in commandline parser NT sapgui, JAVAgui implementiert    */
/* -> M.Tewes                                                         */
#define NI_SYSTSEP     "/R/" /* Separator fuer Systemname R/3 ('MLP') */
#define NI_GRUPSEP     "/G/" /*                Gruppenname ('PUBLIC') */
#define NI_ALIASEP     "/A/" /*                Aliasname f. Saprouter */
#define NI_MSSVSEP     "/M/" /*                Messageserver name     */

#define NI_SEPSIZ      3

/* common handle trace format */
#define NI_HANDLE_TRC           cU("hdl %d")
#define NI_HANDLE_SOCKET_TRC    NI_HANDLE_TRC cU(" / ") SI_SOCKET_TRC

/* Parameter fuer NiSel/NiPeek----------------------------------------*/
#define NI_READ             (SAP_RAW) 1
#define NI_WRITE            (SAP_RAW) 2
#define NI_CONNECT          (SAP_RAW) 4

/* Parameter fuer NiShutdownHandle------------------------------------*/
#define NI_SHUT_RD          (SAP_INT) 0
#define NI_SHUT_WR          (SAP_INT) 1
#define NI_SHUT_RDWR        (SAP_INT) 2


/*--------------------------------------------------------------------*/
/* Neue Puffer-API:                                                   */
/*--------------------------------------------------------------------*/

struct NIBUFFER;
typedef SAPRETURN NIBUF_ALLOC_FUNC (struct NIBUFFER **buf,
                                    SAP_INT           len,
                                    const SAP_UC     *text);
typedef void      NIBUF_FREE_FUNC  (struct NIBUFFER **buf);

/* Groessere Packete kann NiBuf nicht verarbeiten                     */
#define NIBUF_MAXALLOC (8*1024*1024)

enum NIBUFFER_TYPE
{
    NIBUF_NORMAL,
    NIBUF_REFERENZ,
    NIBUF_WRAPPED
};


typedef struct NIBUFFER
{
    /* public data */
    void       *data;           /* Zeiger auf Userdaten               */
    SAP_INT     len;            /* Laenge der Userdaten               */

    /* private data */
#if defined(NI_PRIVATE_DATA)
    void       *space;          /* Zeiger auf Datenbereich            */
    SAP_UINT    spacelen;       /* Laenge des Datenbereichs           */
# ifdef LST_NODE
    enum NIBUFFER_TYPE type;
    SAP_USHORT  refcount;
    union {
        struct NIBUFFER *ref;
        NIBUF_FREE_FUNC *freef;
    } u;
    void *uptr;
    LST_NODE (struct NIBUFFER, queue); /* Queueliste                  */
# endif /* LST_NODE */
#endif /* NI_INTERNAL */
} NIBUFFER;


/* ################################################################## */
/* # Protocol families                                              # */
/* ################################################################## */

/*--------------------------------------------------------------------*/
/* protocol-note:                                                     */
/* ==============                                                     */
/*                                                                    */
/* GENERAL:                                                           */
/*   Die Wahl des Protokolls ist, falls nicht explizit angegeben      */
/*   (oder UNSPEC), von folgenden Parametern abhaengig:               */
/*   - useIPv6 Parameter beim Initialisieren der Library (NiInit3)    */
/*   - NIP_CONNLOCAL Library Parameter (NiSetParamEx)                 */
/*   - Unterstuetzte Protokolle der Plattform (Unix Domain Sockets    */
/*     (UDS))                                                         */
/*                                                                    */
/*                                                                    */
/* NI-HANDLE   (NiGetHdlProto):                                       */
/*   Ein NI Handle spricht immer nur ein Protokoll, falls es sich     */
/*   nicht um einen Listen Handle handelt. Der Listen Handle kann     */
/*   mehrere Protokolle sprechen, jedoch das Protokoll LOCAL nur in   */
/*   Verbindung mit einem IP.                                         */
/*                                                                    */
/*                                                                    */
/* SERVER   (NiServerHandleEx):                                       */
/*   Bei UNSPEC wird einerseits abhaengig von dem useIPv6 Parameter   */
/*   ein IPv4 bzw. IPv6 Server-Socket erstellt; zudem ein UD-Socket   */
/*   falls keine spezifische Listen-Adresse angegeben wurde, UDS auf  */
/*   dieser Plattform unterstuetzt werden und auch aktive             */
/*   (NIP_CONNLOCAL).                                                 */
/*                                                                    */
/*   Das Protokoll LOCAL kann nur in Verbindung mit einem IP Socket   */
/*   angegeben werde, da der IP Socket fuer das Binden des ent-       */
/*   sprechenden Port notwenig ist. Zudem ist die Suche nach einem    */
/*   freien Port vom Betriebssystem nicht moeglich (NI_FINDSERV_OS).  */
/*   Der NI Parameter NIP_CONNLOCAL kann jedoch mit LOCAL ueber-      */
/*   steuert werden.                                                  */
/*                                                                    */
/*   Ein IPv6 Server Socket kann auch erzeugt werden, falls useIPv6   */
/*   nicht gesetzt war beim Initiieren. Ein reines horchen auf der    */
/*   IPv6 Any-Adresse wird bisher von den Betriebssystemen nicht      */
/*   unterstuetzt. Ein IPv6 bedeutet somit immer auch ein IPv4, falls */
/*   nicht eine spezifische IPv6 Listen-Adresse angegeben wird.       */
/*                                                                    */
/*   Ist Protokoll IPv4 und IPv6 gesetzt, so wird abhaengig von dem   */
/*   useIPv6 Parameter die entsprechend Socket Familie verwendet.     */
/*   Dies kann in Verbindung mit dem Protokoll LOCAL sinnvoll sein,   */
/*   falls als Adresse NULL uebergeben wird.                          */
/*                                                                    */
/*   Das Binden der IPv4 Any-Adresse 0.0.0.0 auf einem IPv6 Socket    */
/*   wird von den meisten Plattformen nicht unterstuetzt.             */
/*                                                                    */
/*                                                                    */
/* CLIENT   (Ni*Handle*):                                             */
/*   Noch nicht Implementiert. Parameter wird bisher ignorieret.      */
/*                                                                    */
/*                                                                    */
/* HOSTNAME RESOLUTION   (NiHostToAddr2):                             */
/*   Bei UNSPEC wird abhaengig von dem useIPv6 Parameter versucht     */
/*   eine IPv4 bzw. IPv6 Adresse zu dem Hostnamen zurueckzugeben.     */
/*   Sind beide vorhanden, so wird die IPv6 zurueckgegeben, falls     */
/*   IPv6 aktiviert ist.                                              */
/*                                                                    */
/*   Wird das Protokoll IPv6 bzw. IPv4 uebergeben und zu dem Host-    */
/*   namen ist keine entsprechende Adresse verfuegbar, wird           */
/*   NIEHOST_UNKNOWN zurueckgegeben. Im Falle das anstelle von einem  */
/*   Hostnamen ein Adress-String uebergeben wird und der Protokoll    */
/*   Parameter entspricht nicht dem Adress-Protokoll, wird ein        */
/*   NIEINVAL zurueckgegeben.                                         */
/*                                                                    */
/*   LOCAL ist als Protokoll Parameter nicht zugelassen.              */
/*                                                                    */
/*--------------------------------------------------------------------*/

#define NI_PROTO_FAM_UNSPEC     0   /* protocol family unspecified    */
#define NI_PROTO_FAM_LOCAL      1   /* local protocol          (UDS)  */
#define NI_PROTO_FAM_IPv4       2   /* protocol IP version 4   (IPv4) */
#define NI_PROTO_FAM_IPv6       4   /* protocol IP version 6   (IPv6) */


/* ################################################################## */
/* # IPv6 part                                                      # */
/* ################################################################## */

#define NI_NODEADDR_LEN     16

typedef union /* NI_NODEADDR */
{
    SAP_RAW      addr  [NI_NODEADDR_LEN];       /* 16 */
    SAP_USHORT   addr16[NI_NODEADDR_LEN / 2];   /*  8 */
    SAP_UINT     addr32[NI_NODEADDR_LEN / 4];   /*  4 */
    SAP_ULLONG   addr64[NI_NODEADDR_LEN / 8];   /*  2 */
} NI_NODEADDR;

#define NI_ADDR_I4_OFFSET   12
#define NI_ADDR_I4_LEN      (NI_NODEADDR_LEN - NI_ADDR_I4_OFFSET)
#define NI_ADDR_I6_LEN      NI_NODEADDR_LEN

/*--------------------------------------------------------------------*/

#define NI_ADDR_ANY_I4     { { 0,0,0,0,0,0,0,0,0,0,0xff,0xff,  0,0,0,0 } }
#define NI_ADDR_LOCAL_I4   { { 0,0,0,0,0,0,0,0,0,0,0xff,0xff,127,0,0,1 } }

#define NI_ADDR_ANY_I6     { { 0,0,0,0,0,0,0,0,0,0,   0,   0,  0,0,0,0 } }
#define NI_ADDR_LOCAL_I6   { { 0,0,0,0,0,0,0,0,0,0,   0,   0,  0,0,0,1 } }

extern NI_NODEADDR   NI_ADDR_ANY_INIT;
extern NI_NODEADDR   NI_ADDR_LOCAL_INIT;

extern const NI_NODEADDR   NI_ADDR_ANY_I4_INIT;
extern const NI_NODEADDR   NI_ADDR_LOCAL_I4_INIT;

extern const NI_NODEADDR   NI_ADDR_ANY_I6_INIT;
extern const NI_NODEADDR   NI_ADDR_LOCAL_I6_INIT;

/*--------------------------------------------------------------------*/
/* Makros:                                                            */
/*                                                                    */
/* NI_IS_ADDR_ANY       is wildcard address ('::' or '0.0.0.0')       */
/* NI_IS_ADDR_LOCAL     is loopback address ('::1' or '127.*.*.*)     */
/* NI_IS_IPv4_ADDR      is IPv4 address     ('::ffff:*.*.*.*')        */
/* NI_IS_ADDR_MULTICAST is multicast address('ff::/8' or              */
/*                                           '224.0.0.0/4')           */
/* NI_IS_ADDR_LINKLOCAL is IPv6 address of format 'fe80::/64'         */
/* NI_IS_ADDR_SITELOCAL is IPv6 address of format 'fec0::/48'         */
/*                                                                    */
/* NI_ADDR_EQUAL        are the addresses equal                       */
/* NI_ADDR_PTR_EQUAL    are the addresses pointing to equal           */
/*                                                                    */
/* NI_SET_NO_ADDR       set 0-addresse      (IPv6 wildcard addr '::') */
/* NI_IS_VALID_ADDR     is address not 0-address                      */
/*                                                                    */
/*--------------------------------------------------------------------*/

#define NI_NO_ADDR   NI_ADDR_ANY_I6       /* cf. makro NI_SET_NO_ADDR */

/*--------------------------------------------------------------------*/

#define NI_IS_ADDR_ANY( /* NI_NODEADDR */ nodeAddr )                   \
    ((NI_IS_IPv4_ADDR( nodeAddr ) &&                                   \
      (nodeAddr).addr32[3] == 0) ||                                    \
     NI_IS_VALID_ADDR( nodeAddr ) == FALSE)
    
#if defined(SAPwithINT_LITTLEENDIAN)
    
#define NI_IS_ADDR_LOCAL( /* NI_NODEADDR */ nodeAddr )                 \
    ((NI_IS_IPv4_ADDR( nodeAddr ) && (nodeAddr).addr[12] == 0x7f) ||   \
     ((nodeAddr).addr64[0] == 0 && (nodeAddr).addr32[2] == 0 &&        \
      (nodeAddr).addr32[3] == 0x01000000))
         
#define NI_IS_IPv4_ADDR( /* NI_NODEADDR */ nodeAddr )                  \
    ((nodeAddr).addr64[0] == 0 && (nodeAddr).addr32[2] == 0xffff0000)
    
#define NI_IS_ADDR_LINKLOCAL( /* NI_NODEADDR */ nodeAddr )             \
    ((nodeAddr).addr32[0] == 0x000080fe &&                             \
     (nodeAddr).addr32[1] == 0)

#define NI_IS_ADDR_SITELOCAL( /* NI_NODEADDR */ nodeAddr )             \
    ((nodeAddr).addr32[0] == 0x0000c0fe &&                             \
     (nodeAddr).addr16[3] == 0)

#else /* SAPwithINT_BIGENDIAN */
    
#define NI_IS_ADDR_LOCAL( /* NI_NODEADDR */ nodeAddr )                 \
    ((NI_IS_IPv4_ADDR( nodeAddr ) && (nodeAddr).addr[12] == 0x7f) ||   \
     ((nodeAddr).addr64[0] == 0 && (nodeAddr).addr32[2] == 0 &&        \
      (nodeAddr).addr32[3] == 0x00000001))
         
#define NI_IS_IPv4_ADDR( /* NI_NODEADDR */ nodeAddr )                  \
    ((nodeAddr).addr64[0] == 0 && (nodeAddr).addr32[2] == 0x0000ffff)
         
#define NI_IS_ADDR_LINKLOCAL( /* NI_NODEADDR */ nodeAddr )             \
    ((nodeAddr).addr32[0] == 0xfe800000 &&                             \
     (nodeAddr).addr32[1] == 0)

#define NI_IS_ADDR_SITELOCAL( /* NI_NODEADDR */ nodeAddr )             \
    ((nodeAddr).addr32[0] == 0xfec00000 &&                             \
     (nodeAddr).addr16[3] == 0)

#endif  /* SAPwithINT_BIGENDIAN */

#define NI_IS_ADDR_MULTICAST( /* NI_NODEADDR */ nodeAddr )             \
    ((NI_IS_IPv4_ADDR( nodeAddr ) &&                                   \
      ((nodeAddr).addr[12] & 0xf0) == 0xe0) ||                         \
     (nodeAddr).addr[0] == 0xff)

/*--------------------------------------------------------------------*/

#define NI_ADDR_EQUAL( /* NI_NODEADDR */ nodeAddrA,                    \
                       /* NI_NODEADDR */ nodeAddrB )                   \
    ((nodeAddrA).addr64[0] == (nodeAddrB).addr64[0] &&                 \
     (nodeAddrA).addr64[1] == (nodeAddrB).addr64[1])

#define NI_ADDR_PTR_EQUAL( /* NI_NODEADDR * */ pNodeAddrA,             \
                           /* NI_NODEADDR * */ pNodeAddrB )            \
    ((pNodeAddrA)->addr64[0] == (pNodeAddrB)->addr64[0] &&             \
     (pNodeAddrA)->addr64[1] == (pNodeAddrB)->addr64[1])

/*--------------------------------------------------------------------*/

#define NI_SET_NO_ADDR( /* NI_NODEADDR */ nodeAddr )                   \
    (nodeAddr) = NI_ADDR_ANY_I6_INIT

#define NI_IS_VALID_ADDR( /* NI_NODEADDR */ nodeAddr )                 \
    ((nodeAddr).addr64[0] != 0 || (nodeAddr).addr64[1] != 0)


/* ################################################################## */
/* # IPv4 part                                                      # */
/* ################################################################## */

typedef union /* NI_IPv4ADDR */
{
    SAP_RAW      addr  [NI_ADDR_I4_LEN];       /* 4 */
    SAP_UINT     addr32[NI_ADDR_I4_LEN / 4];   /* 1 */
} NI_IPv4ADDR;

#define NI_IPv4ADDR_LEN     NI_ADDR_I4_LEN


/* convert functions (nodeAddr <-> ipv4Addr) */
NI_NODEADDR API_DF NiTransIPv4ToNode (NI_IPv4ADDR   ipv4Addr);
NI_IPv4ADDR API_DF NiTransNodeToIPv4 (NI_NODEADDR   nodeAddr);


/* convert functions (nodeAddr <-> 4-byte buffer) */
#define NI_V4BUFF_TO_ADDR( /* NODEADDR   */ destAddr,                  \
                           /* SAP_RAW  * */ pSrcBuffer )               \
do {                                                                   \
    NI_IPv4ADDR   ipv4Addr;                                            \
    memcpyR(ipv4Addr.addr, pSrcBuffer, NI_IPv4ADDR_LEN);               \
    destAddr = NiTransIPv4ToNode(ipv4Addr);                            \
} while (FALSE) /* force semicolon */

#define NI_ADDR_TO_V4BUFF( /* SAP_RAW  * */ pDestBuffer,               \
                           /* NODEADDR   */ srcAddr )                  \
do {                                                                   \
    NI_IPv4ADDR   ipv4Addr;                                            \
    ipv4Addr = NiTransNodeToIPv4(srcAddr);                             \
    memcpyR(pDestBuffer, ipv4Addr.addr, NI_IPv4ADDR_LEN);              \
} while (FALSE) /* force semicolon */

/*--------------------------------------------------------------------*/
/* Makros:                                                            */
/*                                                                    */
/* NI_IPv4_ADDR_EQUAL        are the addresses equal                  */
/* NI_IPv4_ADDR_PTR_EQUAL    are the addresses pointing to equal      */
/*                                                                    */
/* NI_IPv4_SET_NO_ADDR       set 0-addresse                           */
/*                           (IPv4 wildcard addr '0.0.0.0')           */
/* NI_IPv4_IS_VALID_ADDR     is address not 0-address                 */
/*                                                                    */
/*--------------------------------------------------------------------*/

#define NI_IPv4_NO_ADDR   { { 0,0,0,0 } }  /* cf. NI_IPv4_SET_NO_ADDR */

/*--------------------------------------------------------------------*/

#define NI_IPv4_ADDR_EQUAL( /* NI_IPv4ADDR */ ipv4AddrA,               \
                            /* NI_IPv4ADDR */ ipv4AddrB )              \
    ((ipv4AddrA).addr32[0] == (ipv4AddrB).addr32[0])

#define NI_IPv4_ADDR_PTR_EQUAL( /* NI_IPv4ADDR * */ pIPv4AddrA,        \
                                /* NI_IPv4ADDR * */ pIPv4AddrB )       \
    ((pIPv4AddrA)->addr32[0] == (pIPv4AddrB)->addr32[0])

/*--------------------------------------------------------------------*/

#define NI_IPv4_SET_NO_ADDR( /* NI_IPv4ADDR */ ipv4Addr )              \
    (ipv4Addr).addr32[0] = 0

#define NI_IPv4_IS_VALID_ADDR( /* NI_IPv4ADDR */ ipv4Addr )            \
    ((ipv4Addr).addr32[0] != 0)


/* ################################################################## */
/* # net address structures part                                    # */
/* ################################################################## */

typedef union /* NI_NODEADDR_RAW */
{
    SAP_RAW   addr[NI_NODEADDR_LEN];   /* 16 */
} NI_NODEADDR_RAW;

typedef union /* NI_IPv4ADDR_RAW */
{
    SAP_RAW   addr[NI_ADDR_I4_LEN];    /*  4 */
} NI_IPv4ADDR_RAW;


/* convert functions (nodeAddr <-> nodeAddrRaw) */
NI_NODEADDR     API_DF NiTransRawToNode (NI_NODEADDR_RAW   nodeAddrRaw);
NI_NODEADDR_RAW API_DF NiTransNodeToRaw (NI_NODEADDR       nodeAddr);


/* convert functions (ipv4Addr <-> ipv4AddrRaw) */
NI_IPv4ADDR     API_DF NiTransRawToIPv4 (NI_IPv4ADDR_RAW   ipv4AddrRaw);
NI_IPv4ADDR_RAW API_DF NiTransIPv4ToRaw (NI_IPv4ADDR       ipv4Addr);


/* ################################################################## */
/* # DDIC part                                                      # */
/* ################################################################## */

/* length of data elements in ABAP package SNI */
#define DDIC_NI_IPV4ADDR_LEN        15 /* chars */
#define DDIC_NI_NODEADDR_LEN        45 /* chars */
#define DDIC_NI_NODEADDR_MASK_LEN   43 /* chars */
#define DDIC_NI_HOSTNAME_LEN        63 /* chars */
#define DDIC_NI_PROTO_FAM_LEN        4 /* bytes */
#define DDIC_NI_ADDR_TYPE_LEN        4 /* bytes */
#define DDIC_NI_ADDR_LOCAL_LEN       1 /* bytes */
#define DDIC_NI_SERVNO_LEN           2 /* bytes */
#define DDIC_NI_SERVNAME_LEN        31 /* chars */
#define DDIC_NI_BUFLINE_LEN        512 /* chars */

/* structures and types in ABAP package SNI */
typedef struct {
    SAP_CHAR   nodeAddr[DDIC_NI_NODEADDR_LEN];
    SAP_INT    protocol;
    SAP_INT    addrType;
} DDIC_NI_NODEADDR_LIST;

typedef struct {
    SAP_CHAR   line[DDIC_NI_BUFLINE_LEN];
} DDIC_NI_BUFLINE_LIST;


/* ################################################################## */

/*---------------------------------------------------------------------*/
/* Maximal hostname lenght inclusive terminating NULL                  */
/* (most os: MAXHOSTNAMELEN)                                           */
/*     e.g. Linux, HP-UX    64                                         */
/*          SunOS, AIX     256                                         */ 
/*---------------------------------------------------------------------*/
#ifdef MAXHOSTNAMELEN
    #define NI_MAX_HOSTNAME_LEN   MAXHOSTNAMELEN
#else
    #define NI_MAX_HOSTNAME_LEN   64
#endif

/*---------------------------------------------------------------------*/
/* Maximal servicename lenght inclusive terminating NULL               */
/*---------------------------------------------------------------------*/
#define NI_MAX_SERVNAME_LEN       32

/*---------------------------------------------------------------------*/
/* Optimal string length for host address and service number           */
/* translation (lenght inclusive terminating NULL)                     */
/* cf. NiAddrToStr, NiAddrToStr2, NiSrvToStr2 and NiSrvToStrL2         */
/*---------------------------------------------------------------------*/
#define NI_NODEADDR_STR_LEN       46
#define NI_IPv4ADDR_STR_LEN       16
#define NI_SERVNO_STR_LEN          6
#define NI_SERVNO_STR_L_LEN       12

/*---------------------------------------------------------------------*/
/* maximale Laenge des Pfades fuer Unix Domain Sockets                 */
/*---------------------------------------------------------------------*/
#define NI_UDS_PATH_LEN     108

/*---------------------------------------------------------------------*/
/* Struktur fuer Routenanalyse                                         */
/*---------------------------------------------------------------------*/
#define NI_HOSTLN   60   /* inc. term. '\0' */
#define NI_SERVLN   30   /* inc. term. '\0' */
#define NI_PASSLN   20   /* inc. term. '\0' */

typedef struct /* NI_ROUTE_TABLE */
{
    SAP_UC hostname[NI_HOSTLN];
    SAP_UC service [NI_SERVLN];
} NI_ROUTE_TABLE;


/*--------------------------------------------------------------------*/
/* Struktur fuer Select-Masken                                        */
/*--------------------------------------------------------------------*/
#define NI_IMASK_LN          512     /* number of SAP_INT's in a mask */

#define BITS_PER_INT         (sizeofR (SAP_INT) * 8)
#define NI_FD_SETSIZE        (NI_IMASK_LN * BITS_PER_INT)

#define NI_DEF_HDLS           2048   /*  2k handles */
#define NI_MAX_HDLS          32768   /* 32K handles */


typedef struct /* NI_MASK_T */
{
    SAP_INT       hdl_count;
    SAP_INT       mask[NI_IMASK_LN];
}   NI_MASK_T;

#define NI_MASK_SIZ          sizeofR (NI_MASK_T)


/*--------------------------------------------------------------------*/
/* Struktur fuer interne SNC-Namen                                    */
/*--------------------------------------------------------------------*/

#define NISNC_ACLKEY_MAXLEN 1024

typedef struct NISNC_ACLKEY {
    SAP_INT  len;
    SAP_RAW  key[NISNC_ACLKEY_MAXLEN];
} NISNC_ACLKEY;


/* ################################################################## */
/* # typedefs                                                       # */
/* ################################################################## */

typedef void    * NISEL_HDL;
typedef SAP_INT   NI_HDL;


/* ################################################################## */
/* # macros                                                         # */
/* ################################################################## */

/*--------------------------------------------------------------------*/
/* Macros zum Manipulieren von Select-Masken                          */
/*--------------------------------------------------------------------*/

/* Inhalt einer Maske auf HEX 0 setzen */
#define NI_CLEAR_MASK(M) \
    (memsetR (&(M), 0, sizeofR(M)))

/* Anzahl der gesetzten Bits in Maske liefern */
#define NI_COUNT_MASK(M)      ((M).hdl_count)

/* in Maske 'mask' Bit fuer Handle 'hdl' setzen */
#define NI_ADDTO_MASK(M, hdl) \
    { if (NI_HDL_OK (hdl) && (! NI_ISIN_MASK (M, hdl))) \
      { (M).mask [hdl / BITS_PER_INT] |= (1 << (hdl % BITS_PER_INT)); \
        (M).hdl_count++; } }

/* in Maske 'mask' Bit fuer Handle 'hdl' loeschen */
#define NI_DELFROM_MASK(M, hdl) \
    { if (NI_ISIN_MASK (M, hdl)) \
      { (M).mask [hdl / BITS_PER_INT] &= ~(1 << (hdl % BITS_PER_INT)); \
        (M).hdl_count--; } }

/* Ist in Maske 'mask' Bit fuer Handle 'hdl' gesetzt ? */
#define NI_ISIN_MASK(M, hdl) \
    (NI_HDL_OK (hdl) ? \
       ((M).mask [hdl / BITS_PER_INT] & (1 << (hdl % BITS_PER_INT))) \
       : FALSE )

/*--------------------------------------------------------------------*/
/* Pruefen, ob Handle zu gross fuer Maske                             */
/*--------------------------------------------------------------------*/
extern SAP_INT ni_max_hdls;

#define NI_HDL_OK(hdl) \
    (((hdl >= 0) && (hdl < ni_max_hdls)) ? TRUE :FALSE)


/* ################################################################## */
/* # function declarations                                          # */
/* ################################################################## */


/*--------------------------------------------------------------------*/
/* NiInit  ()                                                         */
/* NiInit2 ()                                                         */
/* NiInit3 ()                                                         */
/*                                                                    */
/* NiHSBufInit ()                                                     */
/*                                                                    */
/* NiSncInit ()                                                       */
/*                                                                    */
/* Initialization of NI library                                       */
/*                                                                    */
/* INPUT   maxHandles      maximum number of handles                  */
/*                         (0 < maxHandles <= NI_MAX_HDLS;            */
/*                         default: NI_DEF_HDLS)                      */
/*         useIPv6         enable IPv6                                */
/*         pLinearImpl     use linear list or cache implementation    */
/*                         (NULL for default)                         */
/*         indexSize       index size of host- and service name list  */
/*                         or cache (default value 100)               */
/*         expTValHost     expire time of valid host entries          */
/*         expTUnkHN       expire time of unknown hostname            */
/*         expTUnkHA       expire time of unknown IP-address          */
/*         expTValServ     expire time of valid service entry         */
/*         expTUnkSN       expire time of unknown service name        */
/*         expTUnkSP       expire time of unknown service number      */
/*         pMySncName      own SNC name or NULL for disabled SNC      */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        invalid parameter; maxHandle probably to   */
/*                         large (errInfo set)                        */
/*         NIESNC_FAILURE  SNC initialization failed (errInfo set)    */
/*         NIECALL         host- / service name buffer already        */
/*                         initialized; NiHSBufInit only (errInfo set)*/
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - must be called by main thread                              */
/*       - expire times in seconds; a value of 0 effects that the     */
/*         entry will not expire, which is the default value for      */
/*         valid entries; unknown values expires per default after    */
/*         600 seconds (10 minutes)                                   */
/*                                                                    */
/* Einige NI Funktionen setzten eine bereits initialisierte Library   */
/* voraus. Diese rufen intern NiInit auf und Initialisieren NI. Wird  */
/* spaeter vom Benutzer NiInit2 oder NiInit3 aufgerufen, koennen die  */
/* uebergebenen Parameter nicht beruecksichtigt werden! Ein Trace-    */
/* Eintrag wird in diesem Falle geschrieben. Es empfiehlt sich daher, */
/* die Library beim Start des Programmes zu initialisieren.           */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiInit      (void);
SAPRETURN API_DF NiInit2     (SAP_INT          maxHandles);

SAPRETURN API_DF NiInit3     (SAP_UINT         maxHandles,
                              SAP_BOOL         useIPv6);

SAPRETURN API_DF NiHSBufInit (const SAP_BOOL * pLinearImpl,
                              SAP_BOOL         useIPv6,
                              SAP_UINT         indexSize,
                              SAP_UINT         expTValHost,
                              SAP_UINT         expTUnkHN,
                              SAP_UINT         expTUnkHA,
                              SAP_UINT         expTValServ,
                              SAP_UINT         expTUnkSN,
                              SAP_UINT         expTUnkSP);

SAPRETURN API_DF NiSncInit   (const SAP_UC   * pMySncName);


/*--------------------------------------------------------------------*/
/* NiThrInit ()                                                       */
/*                                                                    */
/* Thread specific initialization                                     */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIECALL         NiInit* not yet called (errInfo set)       */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* Werden von einer Funktion Thread Spezifische Daten benoetigt, so   */
/* wird intern NiThrInit aufgerufen. Ein Aufruf der Funktion nach     */
/* kreieren des Threads ist somit nicht notwendig.                    */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiThrInit (void);


/*--------------------------------------------------------------------*/
/* NiExit ()                                                          */
/*                                                                    */
/* Release resources of NI library                                    */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiExit (void);


/*--------------------------------------------------------------------*/
/* NiThrExit ()                                                       */
/*                                                                    */
/* Thread specific cleanup                                            */
/*                                                                    */
/* INPUT   pThrGlobData    NULL or pointer to thread global internal  */
/*                         data                                       */
/*                                                                    */
/* Aufruf notwendig, um Thread spezifische Ressourcen freizugeben.    */
/*                                                                    */
/*--------------------------------------------------------------------*/

void API_DF NiThrExit (void * pThrGlobData);


/*--------------------------------------------------------------------*/
/* Ni<Buf>ServerHandle        ()                                      */
/* Ni<Buf>ServerHandleForPort ()                                      */
/* Ni<Buf>ServerHandleForAddr ()                                      */
/*                                                                    */
/* NiServerHandleEx ()                                                */
/*                                                                    */
/* FORMER  Ni*Listen ()                                               */
/*                                                                    */
/* Listen for connections on a handle                                 */
/*                                                                    */
/* INPUT   pServName       service name or number (e.g. "sapdp57",    */
/*                         "3300", etc.)                              */
/*         pLocalAddr      NULL for ANY address ('0.0.0.0' or '::')   */
/*                         or local interface                         */
/*         pServNo         service number in host byte order,         */
/*                         NI_FINDSERV or NI_FINDSERV_OS for          */
/*                         automatic search                           */
/*         protocol        protocol family (cf. INFO part)            */
/*         buffered        accepted handles in buffered mode          */
/*         pHandle         NI_HDL as out parameter                    */
/*                                                                    */
/* OUTPUT  pHandle         listen handle                              */
/*         pServNo         chosen service number                      */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        invalid parameter or too many handles      */
/*                         (errInfo set)                              */
/*         NIESERV_UNKNOWN service name 'pServName' unknown           */
/*                         (errInfo set)                              */
/*         NIESERV_USED    service already in use (errInfo set)       */
/*         NIEMYHOST_VERIFY   invalid local address 'pLocalAddr'      */
/*                         (errInfo set)                              */
/*         NIENOFREEPORT   no more free ports in range (errInfo set)  */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - the protocol family specifies the protocols on which the   */
/*         handle is going to listen. Default is NI_PROTO_FAM_UNSPEC, */
/*         that is IPv4, LOCAL (depending on the platform and the     */
/*         NI parameter NIP_CONNLOCAL) and IPv6 (cf. NiInit3)         */
/*         -> cf. protocol-note                                       */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiServerHandle           (const SAP_UC      * pServName,
                                           NI_HDL            * pHandle);

SAPRETURN API_DF NiBufServerHandle        (const SAP_UC      * pServName,
                                           NI_HDL            * pHandle);

SAPRETURN API_DF NiServerHandleForPort    (SAP_USHORT        * pServNo,
                                           NI_HDL            * pHandle);

SAPRETURN API_DF NiBufServerHandleForPort (SAP_USHORT        * pServNo,
                                           NI_HDL            * pHandle);

SAPRETURN API_DF NiServerHandleForAddr    (const NI_NODEADDR * pLocalAddr,
                                           SAP_USHORT        * pServNo,
                                           NI_HDL            * pHandle);

SAPRETURN API_DF NiBufServerHandleForAddr (const NI_NODEADDR * pLocalAddr,
                                           SAP_USHORT        * pServNo,
                                           NI_HDL            * pHandle);

SAPRETURN API_DF NiServerHandleEx         (const NI_NODEADDR * pLocalAddr,
                                           SAP_USHORT        * pServNo,
                                           SAP_INT             protocol,
                                           SAP_BOOL            buffered,
                                           NI_HDL            * pHandle);


/*--------------------------------------------------------------------*/
/* Ni<Raw><Buf><Snc>Handle         ()                                 */
/* Ni<Raw><Buf><Snc>HandleForAddr  ()                                 */
/* Ni<Raw><Buf><Snc>HandleForProto ()                                 */
/*                                                                    */
/* NiHandleEx        ()                                               */
/* NiHandleForAddrEx ()                                               */
/*                                                                    */
/* FORMER  Ni*Connect ()                                              */
/*                                                                    */
/* Initiate a connection on a handle                                  */
/*                                                                    */
/* INPUT   pHostName       hostname (e.g. "localhost" or "10.0.0.76") */
/*         pServName       service name or number (e.g. "sapdp57",    */
/*                         "3300", etc.)                              */
/*         pSncTargetName  SNC security identifier of the target      */
/*         pNodeAddr       IP-addresse                                */
/*         servNo          service number in host byte order          */
/*         pSncTargetKey   SNC security key of the target             */
/*         protocol        protocol family (cf. protocol-note)        */
/*         rawMode         talking NI_RAW_IO instead of NI_MESG_IO    */
/*         buffered        create buffered handle                     */
/*         timeout         timeout in ms or NI_BLOCK                  */
/*         pHandle         NI_HDL as out parameter                    */
/*                                                                    */
/* OUTPUT  pHandle         connected handle                           */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIETIMEOUT      accept timed out                           */
/*         NIECONN_PENDING connection in progress (only if buffered,  */
/*                         retured handle 'pHandle' is valid!!!       */
/*         NIEINVAL        invalid parameter or too many handles      */
/*                         (errInfo set)                              */
/*         NIEHOST_UNKNOWN unknown hostname (errInfo set)             */
/*         NIESERV_UNKNOWN unknown service name (errInfo set)         */
/*         NIECONN_REFUSED connection failed (errInfo set)            */
/*         NIEMYHOST_VERIFY   invalid local address (errInfo set)     */
/*         NIESERV_USED    service already in use (errInfo set)       */
/*         NIENOFREEPORT   no more free ports in range (errInfo set)  */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*         route connects only:                                       */
/*         NIEINVAL        too many route entries (errInfo set)       */
/*         NIETOO_SMALL    route struct too small (errInfo set)       */
/*         NIECONN_BROKEN  connection to partner broken (err-info set)*/
/*         NIETOO_BIG      required buffer length too big             */
/*                         (errInfo set)                              */
/*                                                                    */
/*--------------------------------------------------------------------*/

/* hostname and route connect */
SAPRETURN API_DF NiHandle               (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiRawHandle            (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiBufHandle            (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiBufRawHandle         (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiSncHandle            (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         const SAP_UC       * pSncTargetName,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

/* IP-address connect */
SAPRETURN API_DF NiHandleForAddr        (const NI_NODEADDR  * pNodeAddr,
                                         SAP_USHORT           servNo,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiRawHandleForAddr     (const NI_NODEADDR  * pNodeAddr,
                                         SAP_USHORT           servNo,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiBufHandleForAddr     (const NI_NODEADDR  * pNodeAddr,
                                         SAP_USHORT           servNo,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiBufRawHandleForAddr  (const NI_NODEADDR  * pNodeAddr,
                                         SAP_USHORT           servNo,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiSncHandleForAddr     (const NI_NODEADDR  * pNodeAddr,
                                         SAP_USHORT           servNo,
                                         const NISNC_ACLKEY * pSncTargetKey,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

/* hostname and route connect with protocol information */
SAPRETURN API_DF NiHandleForProto       (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              protocol,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiRawHandleForProto    (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              protocol,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiBufHandleForProto    (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              protocol,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiBufRawHandleForProto (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              protocol,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiSncHandleForProto    (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              protocol,
                                         const SAP_UC       * pSncTargetName,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

/* universal connect */
SAPRETURN API_DF NiHandleEx             (const SAP_UC       * pHostName,
                                         const SAP_UC       * pServName,
                                         SAP_INT              protocol,
                                         SAP_BOOL             rawMode,
                                         SAP_BOOL             buffered,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);

SAPRETURN API_DF NiHandleForAddrEx      (const NI_NODEADDR  * pNodeAddr,
                                         SAP_USHORT           servNo,
                                         SAP_INT              protocol,
                                         SAP_BOOL             rawMode,
                                         SAP_BOOL             buffered,
                                         SAP_INT              timeout,
                                         NI_HDL             * pHandle);


/*--------------------------------------------------------------------*/
/* NiAcceptEx ()                                                      */
/*                                                                    */
/* FORMER  NiAccept ()                                                */
/*                                                                    */
/* Accept a connection on a server handle                             */
/*                                                                    */
/* INPUT   inHandle        server handle                              */
/*         timeout         timeout in ms or NI_BLOCK                  */
/*         pOutHandle      NI_HDL as out parameter                    */
/*         pPeerAddr       NI_NODEADDR as out parameter or NULL       */
/*         pPeerServNo     SAP_USHORT as out parameter or NULL        */
/*                                                                    */
/* OUTPUT  pOutHandle      accepted handle                            */
/*         pPeerAddr       peer IP-address                            */
/*         pPeerServNo     peer service number (host byte order)      */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIETIMEOUT      accept timed out                           */
/*         NIEINVAL        invalid parameter or too many handles      */
/*                         (errInfo set)                              */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiAcceptEx (NI_HDL        inHandle,
                             SAP_INT       timeout,
                             NI_HDL      * pOutHandle,
                             NI_NODEADDR * pPeerAddr,
                             SAP_USHORT  * pPeerServNo);


/*--------------------------------------------------------------------*/
/* NiWrite    ()                                                      */
/* NiRawWrite ()                                                      */
/*                                                                    */
/* Send data to partner / 'native TCP partner'                        */
/*                                                                    */
/* INPUT   handle          connected handle                           */
/*         pData           data to send                               */
/*         dataLen         length of data to send                     */
/*         timeout         timeout in ms or NI_BLOCK                  */
/*         pWritten        SAP_INT as out parameter or NULL (NULL     */
/*                         only allowed for buffered handle; but even */
/*                         for this handles, the data may sent        */
/*                         incomplete (return code NIETOO_BIG))       */
/*                                                                    */
/* OUTPUT  pWritten        length of sent data                        */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIETIMEOUT      send timed out (check pWritten, some data  */
/*                         may sent)                                  */
/*         NIEINVAL        invalid parameter (errInfo set)            */
/*         NIECONN_BROKEN  connection to partner broken (err-info set)*/
/*         NIECONN_REFUSED connection failed (errInfo set)            */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*         buffered handles only:                                     */
/*         NIETIMEOUT      timed out (no data written !!!)            */
/*         NIETOO_BIG      required buffer length too big             */
/*                         (errInfo set)                              */
/*         NIESNC_FAILURE  error in SNC processing (errInfo set)      */
/*         NIEQUE_FULL     queue quota reched or heap memory limit    */
/*         'all'           all error set by NiBufISetError            */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiWrite    (NI_HDL       handle,
                             const void * pData,
                             SAP_INT      dataLen,
                             SAP_INT      timeout,
                             SAP_INT    * pWritten);

SAPRETURN API_DF NiRawWrite (NI_HDL       handle,
                             const void * pData,
                             SAP_INT      dataLen,
                             SAP_INT      timeout,
                             SAP_INT    * pWritten);


/*--------------------------------------------------------------------*/
/* NiRead    ()                                                       */
/* NiRawRead ()                                                       */
/*                                                                    */
/* Receive data from partner                                          */
/*                                                                    */
/* INPUT   handle          connected handle                           */
/*         pBuffer         data buffer (or NULL if bufferLen is 0 as  */
/*                         well)                                      */
/*         bufferLen       length of data buffer                      */
/*         timeout         timeout in ms or NI_BLOCK                  */
/*         pRead           SAP_INT as out parameter                   */
/*                                                                    */
/* OUTPUT  pRead           length of read data                        */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIETIMEOUT      received timed out                         */
/*         NIETOO_SMALL    buffer too small; if bufferLen is 0 and    */
/*                         mode not NI_RAW_IO, no err-info is set;    */
/*                         otherwise err-info is set, bufferLen data  */
/*                         are read, but buffer is too small for hole */
/*                         packet                                     */
/*         NIECONN_BROKEN  connection to partner broken; if pRead is  */
/*                         0, connection closed by peer (err-info set)*/
/*         NIECONN_REFUSED connection failed (errInfo set)            */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*         talkmode NI_MESG_IO only:                                  */
/*         NIEPING         NI_PONG received after NiCheck (standard)  */
/*                         or opCode processed                        */
/*                                                                    */
/*         buffered handles only:                                     */
/*         NIETOO_BIG      required buffer length too big             */
/*                         (errInfo set)                              */
/*         NIESNC_FAILURE  error in SNC processing (errInfo set)      */
/*                                                                    */
/*         routed connections only:                                   */
/*         NIEROUT_HOST_UNKNOWN                                       */
/*         NIEROUT_SERV_UNKNOWN                                       */
/*         NIEROUT_CONN_REFUSED                                       */
/*         NIEROUT_INTERN                                             */
/*         NIEROUT_PERM_DENIED                                        */
/*         NIEROUT_CONN_BROKEN                                        */
/*         NIEROUT_VERSION                                            */
/*         NIEROUT_CANCELED                                           */
/*         NIEROUT_SHUTDOWN                                           */
/*         NIEROUT_INFO_DENIED                                        */
/*         NIEROUT_OVERFLOW                                           */
/*         NIEROUT_MODE_DENIED                                        */
/*         NIEROUT_NOCLIENT                                           */
/*         NIEROUT_EXTERN                                             */
/*         NIEROUT_SNC_FAILURE                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiRead    (NI_HDL    handle,
                            void    * pBuffer,
                            SAP_INT   bufferLen,
                            SAP_INT   timeout,
                            SAP_INT * pRead);

SAPRETURN API_DF NiRawRead (NI_HDL    handle,
                            void    * pBuffer,
                            SAP_INT   bufferLen,
                            SAP_INT   timeout,
                            SAP_INT * pRead);


/*--------------------------------------------------------------------*/
/* NiGetHdlProto ()                                                   */
/*                                                                    */
/* Get protocol type of handle                                        */
/*                                                                    */
/* INPUT   handle          NI handle                                  */
/*                                                                    */
/* RETURN  protocol        protocol for non-listen handle; bitmask    */
/*                         for listen handle                          */
/*                         NI_PROTO_FAM_UNSPEC   family unspecified   */
/*                                               or invalid handle    */
/*                         NI_PROTO_FAM_LOCAL    local protocol (UDS) */
/*                         NI_PROTO_FAM_IPv4     IP version 4 (IPv4)  */
/*                         NI_PROTO_FAM_IPv6     IP version 6 (IPv6)  */
/*                                                                    */
/* INFO  - cf. protocol-note                                          */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAP_INT API_DF NiGetHdlProto (NI_HDL   handle);























/*--------------------------------------------------------*/
/* NiSelect ()                                            */
/*                                                        */
/* Auf ankommende Daten warten                            */
/*                                                        */
/* INPUT   timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* IN/OUT  readmask      Maske der zu ueberwachenden      */
/*                       Handles (Masken-Macros verwenden)*/
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

#define NiSelect(mask, timeout) Ni2Select (mask, NULL, timeout)

/*--------------------------------------------------------*/
/* Ni2Select ()                                           */
/*                                                        */
/* Auf ankommende Daten warten und freie Sendeleitungen   */
/* warten                                                 */
/*                                                        */
/* INPUT   timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* IN/OUT  readmask      Maske der zu ueberwachenden      */
/*                       Read-Handles                     */
/*         writemask     Maske der zu ueberwachenden      */
/*                       Write-Handles                    */
/*                       (Masken-Macros verwenden)        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/


SAPRETURN API_DF  Ni2Select              (NI_MASK_T *  readmask,
                                          NI_MASK_T *  writemask,
                                          SAP_INT      timeout);

/*--------------------------------------------------------*/
/* NiPeek ()                                              */
/*                                                        */
/* Testen, ob Daten auf einer best. Leitung angekommen    */
/*                                                        */
/* INPUT   handle        zu ueberpruefender Handle        */
/*         timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar (Daten da)            */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiPeek                 (NI_HDL   handle,
                                          SAP_INT  timeout);


/*--------------------------------------------------------*/
/* NiWriteOK ()                                           */
/*                                                        */
/* Testen, ob Write auf Handle blockieren wuerde          */
/*                                                        */
/* INPUT   handle        zu ueberpruefender Handle        */
/*         timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar (Daten da)            */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiWriteOK              (NI_HDL   handle,
                                          SAP_INT  timeout);




/*--------------------------------------------------------*/
/* NiCheck ()                                             */
/*                                                        */
/* Verbindung zum Partner ueberpruefen.                   */
/* NiCheck sendet ein "Ping"-Signal zum Partner. Ist die  */
/* Verbindung o.k., bekommen beide Partner bei einem der  */
/* naechsten NiRead-Calls den Returncode NIEPING (ohne    */
/* Daten).                                                */
/* (NiRead sendet automatisch Antwort auf Ping)           */
/*                                                        */
/* INPUT   handle        Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*         timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIECONN_BROKEN        Verbindung zum Partner abgebr.   */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiCheck                (NI_HDL      handle,
                                          SAP_INT     timeout);

/*--------------------------------------------------------*/
/* NiCheck1 ()                                            */
/*                                                        */
/* Wie Nicheck, nur, dass keine Antwort zurueckkommt      */
/*                                                        */
/* INPUT   handle        Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*         timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIECONN_BROKEN        Verbindung zum Partner abgebr.   */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiCheck1               (NI_HDL      handle,
                                          SAP_INT     timeout);

/*--------------------------------------------------------------------*/
/* NiCheckEx ()                                                       */
/*                                                                    */
/* Check connection with ping messages                                */
/*                                                                    */
/* INPUT   handle          NI handle                                  */
/*         timeout         timeout in milliseconds (ms) or negative   */
/*                         (NI_BLOCK) for blocking indefinitely       */
/*         recvResp        should the partner send an response; if    */
/*                         value is TRUE, a ping message is sent and  */
/*                         the partner will respond with a pong; if   */
/*                         value is FALSE, a pong message is sent and */
/*                         the partner won't reply.                   */
/*         pWritten        number of bytes already written; parameter */
/*                         is important if a fragmentation of the     */
/*                         message is possible (non-buffered handles);*/
/*                         integer value must be 0 for the first call,*/
/*                         following calls passes the returned value  */
/*                         of previous call; NULL if not interested   */
/*                         in fragmentation.                          */
/*                                                                    */
/* OUTPUT  pWritten        number of bytes written; value is just set */
/*                         if the write operation timed out           */
/*                         (NIETIMEOUT returned)                      */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        handle invalid                             */
/*         NIETIMEOUT      write operation timed out, call function   */
/*                         function again (cf. parameter pWritten)    */
/*         NIECONN_BROKEN  connection to partner broken               */
/*         NIEINTERN       internal error (check err-trace)           */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF  NiCheckEx              (NI_HDL      handle,
                                          SAP_INT     timeout,
                                          SAP_BOOL    recvResp,
                                          SAP_UINT  * pWritten);


/*--------------------------------------------------------*/
/* NiCheck2 ()                                            */
/*                                                        */
/* Wie Nicheck, nur wird intern der Status gespeichert    */
/* und nicht NIEPING zurueckgegeben. Beim naechsten Aufruf*/
/* wird der Status zurueckgeliefert. Funktioniert nur     */
/* mit Verwendung der gepufferten Schnittstelle!          */
/*                                                        */
/* INPUT   handle        Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*         timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* RETURN                                                 */
/*         count         Anzahl Tests ohne Antwort        */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIENO_ANSWER          bisher keine Antwort vom Partner */
/* NIECONN_BROKEN        Verbindung zum Partner abgebr.   */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINVAL              illegale Parameter               */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiCheck2               (NI_HDL      handle,
                                          SAP_INT     timeout,
                                          SAP_INT     *count);


/*--------------------------------------------------------*/
/* NiCheck2Cancel ()                                      */
/*                                                        */
/* Diese Funktion setzt den Status von NiCheck2 zurueck.  */
/* Damit werden bereits eingegangene Antworten vom        */
/* Partner verworfen. Diese Funktion kann verwendet       */
/* werden, wenn nach einem NiCheck2 richtige Daten ueber  */
/* das Netz kommen.                                       */
/*                                                        */
/* INPUT   handle        Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiCheck2Cancel         (NI_HDL      handle);


/*--------------------------------------------------------*/
/* NiCloseHandle ()                                       */
/*                                                        */
/* Mit NiListen oder NiConnect erzeugten Handle freigeben */
/* und evtl. bestehende Verbindung abbauen                */
/*                                                        */
/* INPUT   handle                                         */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              ungueltiger Handle               */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiCloseHandle          (NI_HDL  handle);

/*--------------------------------------------------------*/
/* NiCloseHandleKP ()                                     */
/*                                                        */
/* Mit NiListen oder NiConnect erzeugten Handle freigeben */
/* und evtl. bestehende Verbindung abbauen. Evtl.         */
/* vorhandener UDS Pfad wird nicht geloescht.             */
/*                                                        */
/* INPUT   handle                                         */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              ungueltiger Handle               */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiCloseHandleKP        (NI_HDL  handle);


/*--------------------------------------------------------*/
/* NiShutdownHandle ()                                    */
/*                                                        */
/* Schliessen von Lese- und/oder Schreibseite einer Ver-  */
/* bindung. Der Handle muss mit NiCloseHandle noch        */
/* ansckliessend geschlossen werden.                      */
/*                                                        */
/* INPUT   handle        NI handle                        */
/*         howto         Verbindungsseite, die geschlossen*/
/*                       werden soll: NI_SHUT_RD,         */
/*                       NI_SHUT_WR oder NI_SHUTRDWR      */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              ungueltiger Handle               */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiShutdownHandle       (NI_HDL  handle,
                                          SAP_INT howto);


/*--------------------------------------------------------*/
/* NiCloseAll ()                                          */
/*                                                        */
/* Alle offenen Handles freigeben                         */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiCloseAll            (void);


/*--------------------------------------------------------*/
/* NiRouteToTable ()                                      */
/*                                                        */
/* Routingstring in eine Tabelle von NI_ROUTE-Eintraegen  */
/* konvertieren                                           */
/*                                                        */
/* INPUT   route        Routestring (null-terminiert)     */
/*         size         Groesse des NI_ROUTE-Arrays       */
/*                                                        */
/* IN/OUT  table        Tabelle mit Routingeintraegen     */
/*                                                        */
/* OUTPUT  cnt          Anzahl der gefundenen Eintaege    */
/*                                                        */
/* RETURN  SAP_O_K      alles klar                        */
/*         NIEINTERN    Interner Fehler                   */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiRouteToTable (int            * cnt,
                                 const SAP_UC   * routestring,
                                 NI_ROUTE_TABLE * table,
                                 int              size);
/*--------------------------------------------------------*/
/* NiHBufPrintBuf ()                                      */
/*                                                        */
/* Dump the NI-Hostbuffer into an UC-Buffer               */
/*                                                        */
/*                                                        */
/* INPUT   buf           UC-Buffer to dump to             */
/*         buflen        initial size (must be >= 1)      */
/*                                                        */
/* OUTPUT  buf           pretty-printed nihostbuf         */
/*         buflen        length of the UC-buffer          */
/*                       including trailing cU('\0')      */
/*         done          all entries printed ?            */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               ok                               */
/* NIEINVAL              buffer too small etc.            */
/*                                                        */
/*--------------------------------------------------------*/


SAPRETURN API_DF NiHBufPrintBuf (SAP_UC   * pBuffer,
                                 SAP_INT  * pBufferLen,
                                 SAP_BOOL * pDone);


/*--------------------------------------------------------------------*/
/* NiHSBufLock   ()                                                   */
/* NiHSBufUnlock ()                                                   */
/*                                                                    */
/* NiHSBufGetHost ()                                                  */
/* NiHSBufGetServ ()                                                  */
/*                                                                    */
/* Dump internal host- resp. service-name buffer entry formatted into */
/* own string buffer                                                  */
/*                                                                    */
/* INPUT   pBuffer         data buffer                                */
/*         bufferLen       length of data buffer                      */
/*         pDone           SAP_BOOL as out parameter                  */
/*                                                                    */
/* OUTPUT  pDone           FALSE as long as a line could be read;     */
/*                         TRUE if the end of buffer is reched        */
/*                         (return value SAP_O_K, pBuffer not valid)  */
/*                                                                    */
/* RETURN  SAP_O_K         successful (check *pDone)                  */
/*         NIEINVAL        invalid parameter (errInfo set)            */
/*         NIECALL         buffer not jet initialized                 */
/*         NIETOO_SMALL    data buffer too small to save hole buffer  */
/*                         entry (pBuffer invalid)                    */
/*                                                                    */
/* INFO  - use function NiHSBufLock and NiHSBufUnlock to lock the     */
/*         buffer table while looping over entries                    */
/*       - the fields per line are seperated by tabulator ('\t')      */
/*       - the last field is teminated by '\n'                        */
/*       - a hostname line has following entries:                     */
/*           - table index (integer)                                  */
/*           - entry status ("VALID" or "UNKNOWN")                    */
/*           - entry type (character)                                 */
/*               - ' ': hostname is full-qualified as well            */
/*               - 'H': hostname but not full-qualified               */
/*               - 'A': alias hostname                                */
/*               - 'F': alias which is full-qualified                 */
/*           - IPv4 address                                           */
/*           - IPv6 address                                           */
/*           - hostname                                               */
/*       - a servname line has following entries:                     */
/*           - table index (integer)                                  */
/*           - entry status ("VALID" or "UNKNOWN")                    */
/*           - service number (port) of format "<hex-value>/<port>"   */
/*             (e.g. sapgw53 -> "0D.19/3353")                         */
/*           - service name                                           */
/*       - unknown addresses, ports oder names are denoted by '-'     */
/*       - unused fields are denoted by '~' (only IPv6 address field  */
/*         in a none IPv6 environment so far)                         */
/*       - if a address field is not jet evaluated, it is marked as   */
/*         open ('o')                                                 */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiHSBufLock    (void);
SAPRETURN API_DF NiHSBufUnlock  (void);
SAPRETURN API_DF NiHSBufGetHost (SAP_UC   * pBuffer,
                                 SAP_UINT   bufferLen,
                                 SAP_BOOL * pDone);
SAPRETURN API_DF NiHSBufGetServ (SAP_UC   * pBuffer,
                                 SAP_UINT   bufferLen,
                                 SAP_BOOL * pDone);


/*--------------------------------------------------------------------*/
/* NiHSBufGetAll ()                                                   */
/*                                                                    */
/* Dump all internal host- and service-name buffer entry formatted    */
/* into allocated buffer                                              */
/*                                                                    */
/* INPUT   ppData          SAP_UC pointer as out parameter (pointer   */
/*                         must be NULL)                              */
/*         pDataLen        SAP_UINT as out parameter                  */
/*                                                                    */
/* OUTPUT  ppData          pointer to data (must be freed !!!)        */
/*         pDataLen        length of data                             */
/*                                                                    */
/* RETURN  SAP_O_K         successful (check *pDone)                  */
/*         NIEINVAL        invalid parameter (errInfo set)            */
/*         NIECALL         buffer not jet initialized                 */
/*         NIETOO_SMALL    internal data buffer too small (should not */
/*                         happen)                                    */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiHSBufGetAll  (SAP_UC  ** ppData,
                                 SAP_UINT * pDataLen);


/*--------------------------------------------------------*/
/* NiGetNodeAddrList ()                                   */
/*                                                        */
/* gibt die aktiven Interfaces der Maschine als IP-Addresse*/
/* zur"uck.                                               */
/*                                                        */
/* INPUT   nodeaddr      array von bufsiz NI_NODEADDR     */
/*         protocol      not yet implemented in this      */
/*                       function (cf. protocol-note)     */
/*         bufsiz        maximale Anzahl an Eintraegen    */
/*                                                        */
/* OUTPUT  if_found      Anzahl der wirklich gefundenen   */
/*                       Interfaces                       */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/*                       sonst interner Fehler            */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiGetNodeAddrList   (NI_NODEADDR * pNodeAddrList,
                                      SAP_INT       protocol,       
                                      SAP_UINT      listSize,
                                      SAP_UINT    * pFound);

/*--------------------------------------------------------------------*/
/* NiGetMacAddrList ()                                                */
/*                                                                    */
/* Get list of local medium access control (MAC) addresses            */
/*                                                                    */
/* INPUT   pMacAddrList    array of NI_MACADDR structures as out      */
/*                         parameter                                  */
/*         listSize        size of pMacAddrList array                 */
/*         pFound          SAP_UINT as out parameter                  */
/*                                                                    */
/* OUTPUT  pMacAddrList    filled array of local MAC addresses        */
/*         pFound          number of MAC addresses found              */
/*                                                                    */
/* RETURN  SAP_O_K         successful (*pFound <= listSize)           */
/*         NIETOO_SMALL    array too small, addresses filled up to    */
/*                         listSize (*pFound > listSize !!!)          */
/*         NIEINVAL        invalid parameter (pMacAddrList or pFound  */
/*                         is NULL) (errInfo set)                     */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

typedef struct /* NI_MACADDR */
{
  SAP_RAW   addr[6];
} NI_MACADDR;

#define NI_MACADDR_LEN   6

SAPRETURN API_DF NiGetMacAddrList   (NI_MACADDR * pMacAddrList,
                                     SAP_UINT     listSize,
                                     SAP_UINT   * pFound);

/*--------------------------------------------------------*/
/* NiHostToAddr ()                                        */
/*                                                        */
/* log. Hostname in interne Addresse umwandeln             */
/* Achtung: Hostnamen und IP-Addr. werden beim 1. Zugriff */
/*          gepuffert. Dump/Flush siehe NiSetParam ()     */
/*                                                        */
/* INPUT   hostName      Hostname (null-terminiert),      */
/*                       Address-String, z.B. "155.56.2.3" */
/*                       oder NULL fuer localhost         */
/*         protocol      cf. protocol-note                */
/*                                                        */
/* OUTPUT  nodeAddr      interne Host-Addresse             */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEHOST_UNKNOWN       log. Hostname unbekannt          */
/* NIEINVAL              nodeAddr == NULL oder hostName   */
/*                       zu lange                         */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiHostToAddr           (const SAP_UC * pHostName,
                                          NI_NODEADDR  * pNodeAddr);

SAPRETURN API_DF  NiHostToAddr2          (const SAP_UC * pHostName,
                                          SAP_INT        protocol,       
                                          NI_NODEADDR  * pNodeAddr);


/*--------------------------------------------------------------------*/
/* NiGetLoopbackAddr ()                                               */
/*                                                                    */
/* Get loopbach address of machine                                    */
/*                                                                    */
/* INPUT   pNodeAddr       NI_NODEADDR as out parameter               */
/*                                                                    */
/* OUTPUT  pNodeAddr       loopback address                           */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        invalid parameter (errInfo set)            */
/*                                                                    */
/* INFO  - the 'localhost' address of the system is retured           */
/*       - the address is usually '127.0.0.1'; on IPv6 systems it may */
/*         be '::1'. An IPv6 address is only retured, if IPv6 is      */
/*         enabled by the NiInit3 call.                               */
/*       - This call should be used that client and server using the  */
/*         same loopback address                                      */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiGetLoopbackAddr (NI_NODEADDR * pNodeAddr);


/*--------------------------------------------------------*/
/* NiAddrToHost  ()                                       */
/* NiAddrToHost2 ()   (reentrant)                         */
/*                                                        */
/* interne Addresse in log. Hostnamen umwandeln            */
/* Achtung: Hostnamen und IP-Addr. werden beim 1. Zugriff */
/*          gepuffert. Dump/Flush siehe NiSetParam ()     */
/*                                                        */
/* INPUT   nodeAddr      interne Host-Addresse oder NULL   */
/*                       fuer localhost                   */
/*         hostName      Hostnamen Puffer                 */
/*         bufLen        Groesse des Puffers in SAP_UC    */
/*                       (optimal NI_MAX_HOSTNAME_LEN)    */
/*                                                        */
/* RETURN                Hostname (null-terminiert)       */
/*                       Host-Addr-String, falls unbekannt */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEHOST_UNKNOWN       Host Addresse unbekannt           */
/* NIEINVAL              hostName == NULL                 */
/* NIETOO_SMALL          Puffer fuer Hostnamen zu klein   */
/*                                                        */
/*--------------------------------------------------------*/

      SAP_UC * API_DF NiAddrToHost  (const NI_NODEADDR * pNodeAddr);
SAPRETURN      API_DF NiAddrToHost2 (const NI_NODEADDR * pNodeAddr,
                                     SAP_UC            * pHostName,
                                     SAP_UINT            bufferLen);

/*--------------------------------------------------------*/
/* NiAddrToHostCanon ()                                   */
/*                                                        */
/* -> Funktion mit NiAddrToHost identisch                 */
/* (                                                      */
/* interne Addresse in log. Hostnamen umwandeln            */
/* Achtung: Es wird immer der kanonische Name des Rechners*/
/*          zurueckgegeben. Ist dieser nicht voll         */
/*          qualifiziert, wird auch nur der Rechnernamen  */
/*          zurueckgegeben. Die Eintraege werden nicht    */
/*          gepuffert.                                    */
/* )                                                      */
/*--------------------------------------------------------*/

      SAP_UC * API_DF NiAddrToHostCanon  (const NI_NODEADDR * pNodeAddr);
SAPRETURN      API_DF NiAddrToHostCanon2 (const NI_NODEADDR * pNodeAddr,
                                          SAP_UC            * pHostName,
                                          SAP_UINT            bufferLen);


/*--------------------------------------------------------*/
/* NiServToNo ()                                          */
/*                                                        */
/* Service-Namen in (Port-)Nummer umwandeln               */
/* Achtung: Servicenamen und Ports werden beim 1. Zugriff */
/*          gepuffert. Dump/Flush siehe NiSetParam ()     */
/*                                                        */
/* INPUT   servName      Servicename (null-terminiert)    */
/*                       oder Service-Nummer-String, z.B. */
/*                       "3200"                           */
/*                                                        */
/* OUTPUT  servNo        Service-Nummer (Host-Byte-Order) */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIESERV_UNKNOWN       log. Servicename unbekannt       */
/* NIEINVAL              servName == NULL, servName zu    */
/*                       lange oder servNo == NULL        */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiServToNo             (const SAP_UC * servName,
                                          SAP_USHORT   * servNo);

/*--------------------------------------------------------*/
/* NiNoToServ  ()                                         */
/* NiNoToServ2 ()   (reentrant)                           */
/*                                                        */
/* (Port-)Nummer in Service-Namen umwandeln               */
/* Achtung: Servicenamen und Ports werden beim 1. Zugriff */
/*          gepuffert. Dump/Flush siehe NiSetParam ()     */
/*                                                        */
/* INPUT   servNo        Service-Nummer (Host-Byte-Order) */
/*         servName      Servicenamen Puffer              */
/*         bufLen        Groesse des Puffers in SAP_UC    */
/*                       (optimal NI_MAX_SERVNAME_LEN)    */
/*                                                        */
/* RETURN                Service-Name (null-terminiert)   */
/*                       Service-Nummer-String, falls     */
/*                       unbekannt                        */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIESERV_UNKNOWN       (Port-)Nummer unbekannt          */
/* NIEINVAL              servName == NULL                 */
/* NIETOO_SMALL          Puffer fuer Servicenamen zu klein*/
/*                                                        */
/*--------------------------------------------------------*/

      SAP_UC * API_DF NiNoToServ  (SAP_USHORT   servNo);
SAPRETURN      API_DF NiNoToServ2 (SAP_USHORT   servNo,
                                   SAP_UC     * pServName,
                                   SAP_UINT     bufferLen);

/*--------------------------------------------------------*/
/* NiMyHostName ()                                        */
/*                                                        */
/* eigenen Hostnamen ermitteln                            */
/*                                                        */
/* OUTPUT  pHostName     eigener Hostname (null-termin.)  */
/*                                                        */
/* INPUT   bufferLen     maximale Laenge fuer Hostnamen   */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIETOO_SMALL          Hostname wurde abgeschnitten     */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiMyHostName           (SAP_UC  * pHostName,
                                          SAP_INT   bufferLen);


/*--------------------------------------------------------*/
/* NiMyAddrVerify ()                                      */
/* NiMyAddrCheck ()                                       */
/*                                                        */
/* eigene Host-Addresse verifizieren (mittels NiIlisten,   */
/* NiConnect () und NiAccept ())                          */
/* NiMyAddrVerify laesst 127.0.0.1 nicht zu               */
/*                                                        */
/* INPUT   myhostname    eigener Hostname (null-termin.)  */
/*                       oder Address-String, z.B.         */
/*                       "155.56.2.3"                     */
/*                       oder (SAP_UC   *) 0 (Funktion    */
/*                       ermittelt eig. Nodeadddr. selbst)*/
/*                                                        */
/* OUTPUT  mynodeaddr    eigene Host-Addresse              */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEMYHOST_VERIFY      Fehler beim Verifizieren der     */
/*                       eigenen Host-Addresse             */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiMyAddrVerify  (const SAP_UC * myhostname,
                                  NI_NODEADDR  * mynodeaddr);

SAPRETURN API_DF NiMyAddrCheck   (const SAP_UC * myhostname,
                                  NI_NODEADDR  * mynodeaddr);


/*--------------------------------------------------------*/
/* NiLocalCheck ()                                        */
/*                                                        */
/* pruefen ob eine NI_HOST_ADDR lokal ist                  */
/*                                                        */
/* OUTPUT    nodeaddr    Host-Addresse                     */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* TRUE                  nodeaddr ist lokal                */
/* FALSE                 nodeaddr ist nicht lokal          */
/*                                                        */
/*--------------------------------------------------------*/

SAP_BOOL NiLocalCheck(const NI_NODEADDR *ha);

/*---------------------------------------------------------*/
/* NiHdlToSock ()                                          */
/*                                                         */
/* Gebe den Socket zu einem NI-Handle zurueck              */
/*                                                         */
/* INPUT   handle       NI-Handle                          */
/*                                                         */
/* OUTPUT  sock         korrespondierender Socket          */
/*                                                         */
/* RETURN                                                  */
/*                                                         */
/* SAP_O_K              alles klar                         */
/* NIEINVAL             handle ist kein NI-Handle          */
/*                                                         */
/*---------------------------------------------------------*/

SAPRETURN API_DF NiHdlToSock     (NI_HDL    handle,
                                  int    *  sock);


/*---------------------------------------------------------*/
/* NiSockToHdl ()                                          */
/*                                                         */
/* Gebe den NiHandle zu einem Socket zurueck               */
/*                                                         */
/* INPUT   sock         Socket                             */
/* OUTPUT  handle       korrespondierender NI-Handle       */
/*                                                         */
/* RETURN                                                  */
/*                                                         */
/* SAP_O_K              alles klar                         */
/* NIEINVAL             sock hat keinen korrespondierenden */
/*                      NI-Handle                          */
/*                                                         */
/*---------------------------------------------------------*/

SAPRETURN API_DF NiSockToHdl     (int       sock,
                                  NI_HDL *  handle);

/*---------------------------------------------------------*/
/* NiHdlGetPeer ()                                         */
/*                                                         */
/* Gebe die Addresse des Kommunikationspartners eines       */
/* NI_HDL zurueck                                          */
/*                                                         */
/* INPUT   handle         Handle                           */
/* OUTPUT  niha           Host-Addresse                     */
/* OUTPUT  port           Port-Nummer (net-byte-order)     */
/*                        (== 0 fuer UDS Sockets)          */
/* OUTPUT  unix_domain_pathname   sic                      */
/*                        (Speicher dafuer muss            */
/*                         reserviert sein,                */
/*                         NI_UDS_PATH_LEN Bytes)          */
/*                                                         */
/* RETURN                                                  */
/*                                                         */
/* SAP_O_K              alles klar                         */
/* NIEINTERN            Socket-Fehler oder unbekannte      */
/*                      Protokol Domain                    */
/* NIEINVAL             Hdl ist an Unix Domain Namen       */
/*                      gebunden, aber der entsprechende   */
/*                      Parameter war NULL                 */ 
/*                                                         */
/*---------------------------------------------------------*/


SAPRETURN API_DF NiHdlGetPeer    (NI_HDL handle,
                                  NI_NODEADDR *niha,
                                  SAP_USHORT *port,
                                  SAP_UC *unix_domain_pathname);

/*---------------------------------------------------------*/
/* NiHdlGetName ()                                         */
/*                                                         */
/* Gebe die Addresse des Komunikationspartners eines NI_HDL */
/* zurueck                                                 */
/*                                                         */
/* INPUT   handle         Handle                           */
/* OUTPUT  niha           Host-Addresse                     */
/* OUTPUT  port           Port-Nummer (net-byte-order)     */
/*                        (== 0 fuer UDS Sockets)          */
/* OUTPUT  unix_domain_pathname   sic                      */
/*                        (Speicher dafuer muss            */
/*                         reserviert sein,                */
/*                         NI_UDS_PATH_LEN Bytes)          */
/*                                                         */
/*                                                         */
/* RETURN                                                  */
/*                                                         */
/* SAP_O_K              alles klar                         */
/* NIEINTERN            Socket-Fehler oder unbekannte      */
/*                      Protokol Domain                    */
/* NIEINVAL             Hdl ist an Unix Domain Namen       */
/*                      gebunden, aber der entsprechende   */
/*                      Parameter war NULL                 */ 
/*                                                         */
/*---------------------------------------------------------*/

SAPRETURN API_DF NiHdlGetName    (NI_HDL handle,
                                  NI_NODEADDR *ha, 
                                  SAP_USHORT *port, 
                                  SAP_UC *unix_domain_pathname);

/*---------------------------------------------------------*/
/* NiHdlSetBlockmode ()                                    */
/*                                                         */
/* Setze Blockmode fuer einen Handle (und seine sockets)   */
/* an oder aus.                                            */
/* !!!!! Vorsicht: Dieser Call ist nur fuer diejenigen     */
/*       Benutzer der nilib gedacht, die sockets an der    */
/*       Library vorbei benutzen (muessen) und daher den   */
/*       Blockmode zwischen handle und socket konsistent   */
/*       halten muessen.                                   */
/* !!!!!                                                   */
/*                                                         */
/* INPUT   handle         Handle                           */
/*         mode           TRUE/FALSE (block/nonblock)      */
/*                                                         */
/* RETURN                                                  */
/*                                                         */
/* SAP_O_K              Alles OK                           */
/* NIEINTERN            Fehler                             */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/

SAPRETURN API_DF   NiHdlSetBlockmode(NI_HDL handle, SAP_BOOL blocking);


/*---------------------------------------------------------*/
/* NiHdlGetBlockmode ()                                    */
/*                                                         */
/* INPUT   handle         Handle                           */
/*                                                         */
/* RETURN                                                  */
/*                                                         */
/* TRUE                 Blockierende handle                */
/* FALSE                sonst                              */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/
SAP_BOOL           NiHdlGetBlockmode ( NI_HDL hdl );

/*---------------------------------------------------------*/
/* NiHdlGetStatus ()                                       */
/*                                                         */
/* Gebe den Verbindungsstatus  eines NI_HDL                */
/*                                                         */
/* INPUT   handle         Handle                           */
/*                                                         */
/* RETURN                                                  */
/*                                                         */
/* SAP_O_K              Es liegen Daten vor                */
/* NIETIMEOUT           Hdl ok, es liegen keine Daten vor  */
/* NIEINTERN            keine Info ueber hdl Status        */
/* NIEINVAL             Es liegt ein Socketfehler vor      */
/* NIECONN_BROKEN       Der Socket zum Hdl wurde           */
/*                      geschlossen                        */
/*                                                         */
/*---------------------------------------------------------*/

SAPRETURN API_DF NiHdlGetStatus          (NI_HDL handle);

/*---------------------------------------------------------*/
/* NiHdlDump  ()                                           */
/* NiHdlDump2 ()                                           */
/*                                                         */
/* Dumpt Informationen ueber einen Handle ins Trace        */
/* der Dump erfolgt bei Tracelevel >= 1                    */
/*                                                         */
/* INPUT   handle         Handle                           */
/*         buffer         Puffer fuer Ausgabe              */
/*         bufLen         Groesse des Puffers              */
/*                                                         */
/* RETURN                 nichts                           */
/*                                                         */
/* SAP_O_K                alles klar                       */
/* NIEINVAL               handle ungueltig oder            */
/*                        pBuffer = NULL                   */
/* NIETOO_SMALL           Puffer zu klein                  */
/*                                                         */
/*---------------------------------------------------------*/

void          API_DF NiHdlDump (NI_HDL     hdl);
SAPRETURN     API_DF NiHdlDump2(NI_HDL     hdl,
                                SAP_UC   * pBuffer,
                                SAP_UINT   bufLen);

/*---------------------------------------------------------*/
/* NiDumpNiHdls ()                                         */
/*                                                         */
/* Dumpt Informationen ueber alle aktiven Handles ins Trace*/
/* der Dump erfolgt bei Tracelevel >= 1                    */
/*                                                         */
/* INPUT                  nichts                           */
/*                                                         */
/* RETURN                 nichts                           */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/

void          API_DF NiDumpNiHdls (void);

/*---------------------------------------------------------*/
/* NiHdlGetOutOctets ()                                    */
/*                                                         */
/* liefere die von einem Handle gesendeten Bytes           */
/*                                                         */
/* INPUT   handle         Handle                           */
/*                                                         */
/* RETURN  gesendete Bytes                                 */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/

unsigned long API_DF NiHdlGetOutOctets(NI_HDL handle);

/*---------------------------------------------------------*/
/* NiHdlGetInOctets ()                                     */
/*                                                         */
/* liefere die von einem Handle empfangenen Bytes          */
/*                                                         */
/* INPUT   handle         Handle                           */
/*                                                         */
/* RETURN  empfangene Bytes                                */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/

unsigned long API_DF NiHdlGetInOctets(NI_HDL handle);

/*---------------------------------------------------------*/
/* NiHdlGetOutMsgs   ()                                    */
/*                                                         */
/* liefere die von einem Handle gesendeten Ni-Messages     */
/*                                                         */
/* INPUT   handle         Handle                           */
/*                                                         */
/* RETURN  gesendete Messages                              */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/

unsigned long API_DF NiHdlGetOutMsgs  (NI_HDL handle);

/*---------------------------------------------------------*/
/* NiHdlGetInMsgs   ()                                     */
/*                                                         */
/* liefere die von einem Handle empfangenen Ni-Messages    */
/*                                                         */
/* INPUT   handle         Handle                           */
/*                                                         */
/* RETURN  empfangene Messages                             */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/

unsigned long API_DF NiHdlGetInMsgs  (NI_HDL handle);

/*---------------------------------------------------------*/
/* NiHdlGetStartTime   ()                                  */
/*                                                         */
/* liefere die Sekunden die zwischen dem 1.1. 1970 und der */
/* Zeit als der Handle geoeffnet wurde verstrichen sind    */
/*                                                         */
/* INPUT   handle         Handle                           */
/*                                                         */
/* RETURN  Startzeit                                       */
/*                                                         */
/*                                                         */
/*---------------------------------------------------------*/

long API_DF NiHdlGetStartTime  (NI_HDL handle);

/* ################################################################## */
/* # datagram communication (handle based)                          # */
/* ################################################################## */

/*--------------------------------------------------------------------*/
/* NiDgHdlCreate ()                                                   */
/*                                                                    */
/* Create datagram handle (always IP family)                          */
/*                                                                    */
/* INPUT   pHandle         NI_HDL as out parameter                    */
/*                                                                    */
/* OUTPUT  pHandle         created datagram handle                    */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        pHandle = NULL or maximum number of        */
/*                         handles reached (errInfo set)              */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - handle is always blocking and from IP family               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiDgHdlCreate      (NI_HDL * pHandle);

/*--------------------------------------------------------------------*/
/* NiDgHdlBind     ()                                                 */
/* NiDgHdlBindName ()                                                 */
/*                                                                    */
/* Bind handle to local interface and service                         */
/*                                                                    */
/* INPUT   handle          datagram handle                            */
/*         nodeAddr        local interface address                    */
/*         pHostName       local hostname; NULL or "" for address     */
/*                         any (0.0.0.0)                              */
/*         netServNo       service number; NI_FINDSERV or             */
/*                         NI_FINDSERV_OS (network byte order)        */
/*         pServName       service name                               */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        not a datagram handle, pServName = NULL,   */
/*                         pServName too long, pHostName too long,    */
/*                         hostname unknown or service name unknown   */
/*                         (errInfo set)                              */
/*         NIESERV_USED    service already in use (errInfo set)       */
/*         NIEMYHOST_VERIFY   non-local interface address (errInfo    */
/*                         set)                                       */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - use function NiHdlGetName to verify the bound address and  */
/*         service                                                    */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiDgHdlBind        (NI_HDL       handle,
                                     NI_NODEADDR   nodeAddr,
                                     SAP_USHORT   netServNo);

SAPRETURN API_DF NiDgHdlBindName    (NI_HDL         handle,
                                     const SAP_UC * pHostName,
                                     const SAP_UC * pServName) ;

/*--------------------------------------------------------------------*/
/* NiDgHdlConnect     ()                                              */
/* NiDgHdlConnectName ()                                              */
/*                                                                    */
/* Connect datagram handle to remote peer                             */
/*                                                                    */
/* INPUT   handle          datagram handle                            */
/*         nodeAddr        peer address                               */
/*         pHostName       peer hostname; NULL or "" for loopback     */
/*                         address (127.0.0.1)                        */
/*         netServNo       service number (network byte order)        */
/*         pServName       service name                               */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        not a datagram handle, pServName = NULL,   */
/*                         pServName too long, pHostName too long,    */
/*                         hostname unknown or service name unknown   */
/*                         (errInfo set)                              */
/*         NIECONN_REFUSED could not connect datagram handle (errInfo */
/*                         set)                                       */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiDgHdlConnect     (NI_HDL       handle,
                                     NI_NODEADDR  nodeAddr,
                                     SAP_USHORT   netServNo);

SAPRETURN API_DF NiDgHdlConnectName (NI_HDL       handle,
                                     const SAP_UC     * pHostName,
                                     const SAP_UC     * pServName);

/*--------------------------------------------------------------------*/
/* NiDgHdlSend       ()   (send over connected handle,                */
/*                         cf. NiDgHdlConnect)                        */
/* NiDgHdlSendTo     ()                                               */
/* NiDgHdlSendToName ()                                               */
/*                                                                    */
/* Send datagram over datagram handle                                 */
/*                                                                    */
/* INPUT   handle          datagram handle                            */
/*         pData           data to send                               */
/*         dataLen         data length                                */
/*         nodeAddr        peer host address                          */
/*         pHostName       peer hostname or NULL for localhost        */
/*         netServNo       service number (network byte order)        */
/*         pServName       service name                               */
/*         pWritten        SAP_INT as out parameter; or NULL if not   */
/*                         interested in value                        */
/*                                                                    */
/* OUTPUT  pWritten        number of bytes sent                       */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        not a datagram handle, pData = NULL,       */
/*                         dataLen < 0, wrong handle state, pHostName */
/*                         too long, pServName = NULL or pServName    */
/*                         too long, hostname or service name unknown */
/*                         (errInfo set)                              */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiDgHdlSend        (NI_HDL       handle,
                                     const SAP_RAW    * pData,
                                     SAP_INT      dataLen,
                                     SAP_INT    * pWritten);

SAPRETURN API_DF NiDgHdlSendTo      (NI_HDL       handle,
                                     NI_NODEADDR  nodeAddr,
                                     SAP_USHORT   netServNo,
                                     const SAP_RAW    * pData,
                                     SAP_INT      dataLen,
                                     SAP_INT    * pWritten);

SAPRETURN API_DF NiDgHdlSendToName  (NI_HDL       handle,
                                     const SAP_UC     * pHostName,
                                     const SAP_UC * pServName,
                                     const SAP_RAW    * pData,
                                     SAP_INT      dataLen,
                                     SAP_INT    * pWritten);

/*--------------------------------------------------------------------*/
/* NiDgHdlRecvFrom ()                                                 */
/*                                                                    */
/* Receive datagram                                                   */
/*                                                                    */
/* INPUT   handle          datagram handle                            */
/*         pNodeAddr       NI_NODEADDR struct as out parameter; or    */
/*                         NULL if not interested in value            */
/*         pNetServNo      SAP_USHORT as out parameter; or NULL if    */
/*                         not interested in value                    */
/*         pBuffer         pointer to buffer                          */
/*         bufferLen       buffer length                              */
/*         pRead           SAP_INT as out parameter; or NULL if not   */
/*                         interested in value                        */
/*                                                                    */
/* OUTPUT  pNodeAddr       sender address                             */
/*         pNetServNo      sender service number (network byte order) */
/*         pRead           number of bytes received                   */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        not a datagram handle, pBuffer = NULL or   */
/*                         bufferLen < 0                              */
/*                         handle not bound yet (NiDgBind not called) */
/*                         (errInfo set)                              */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiDgHdlRecvFrom    (NI_HDL       handle,
                                     NI_NODEADDR * pNodeAddr,
                                     SAP_USHORT * pNetServNo,
                                     SAP_RAW    * pBuffer,
                                     SAP_INT      bufferLen,
                                     SAP_INT    * pRead);

/*--------------------------------------------------------------------*/
/* NiDgHdlClose ()                                                    */
/*                                                                    */
/* Close datagram handle                                              */
/*                                                                    */
/* INPUT   handle          datagram handle                            */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        not a datagram handle (errInfo set)        */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiDgHdlClose       (NI_HDL   handle);

/*--------------------------------------------------------------------*/
/* NiDgHdlJoinGroup  ()                                               */
/* NiDgHdlLeaveGroup ()                                               */
/*                                                                    */
/* Join or leave a multicast group                                    */
/*                                                                    */
/* INPUT   handle          datagram handle                            */
/*         mcNodeAddr      multicast host address                     */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        not a datagram handle or mcNodeAddr not a  */
/*                         multicast address (errInfo set)            */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiDgHdlJoinGroup   (NI_HDL       handle,
                                     NI_NODEADDR  mcNodeAddr);
SAPRETURN API_DF NiDgHdlLeaveGroup  (NI_HDL       handle,
                                     NI_NODEADDR  mcNodeAddr);


/* ################################################################## */
/* # datagram communication (thread global handles)                 # */
/* # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -# */
/* # These functions should not be used any more. There are handle  # */
/* # based functions declared above.                                # */
/* # For each thread, only one client and server handle exists. If  # */
/* # a data packet will be sent to another server, the handle needs # */
/* # to be reconnected (expencive).                                 # */
/* ################################################################## */

/*--------------------------------------------------------------------*/
/* NiDgBind ()                                                        */
/*                                                                    */
/* Prepare thread server handle to receive datagram from peer         */
/*                                                                    */
/* INPUT   pServName       service name; NULL if free service should  */
/*                         be choose by library                       */
/*         pServNo         SAP_USHORT as out parameter                */
/*                                                                    */
/* OUTPUT  pServNo         chosen service number (host byte order)    */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        pServNo = NULL, pServName too long or      */
/*                         maximum number of handles reached (errInfo */
/*                         set)                                       */
/*         NIESERV_USED    service already in use (errInfo set)       */
/*         NIESERV_UNKNOWN service name unknown (errInfo set)         */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - handle will be rebound, if it is already bould to a port   */
/*       - if the return value is not SAP_O_K, the socket is closed   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF  NiDgBind               (const SAP_UC * pServName,
                                          SAP_USHORT   * pServNo);

/*--------------------------------------------------------------------*/
/* NiDgSend  ()                                                       */
/* NiDg2Send ()                                                       */
/*                                                                    */
/* Send datagram over thread client handle                            */
/*                                                                    */
/* INPUT   pData           data to send                               */
/*         dataLen         data length                                */
/*         pHostName       peer hostname or NULL for localhost        */
/*         pNodeAddr       peer host address                          */
/*         pServName       peer service name (or NULL if parameter    */
/*                         servNo should be used)                     */
/*         servNo          service number, used if pServName = NULL   */
/*                         (host byte order)                          */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        pData = NULL, dataLen < 0, pHostName too   */
/*                         long, pNodeAddr = NULL pServName too long  */
/*                         or maximum number of handles reached       */
/*                         (errInfo set)                              */
/*         NIESERV_UNKNOWN service name unknown (errInfo set)         */
/*         NIECONN_REFUSED could not connect datagram handle (errInfo */
/*                         set)                                       */
/*         NIECONN_BROKEN  connection broke (errInfo set)             */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - the thread client handle is created by the first call of   */
/*         one of this functions. If the peer address is different    */
/*         from the current connected peer, the socket is reconnected */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF  NiDgSend               (const void   * pData,
                                          SAP_INT        dataLen,
                                          const SAP_UC * pHostName,
                                          const SAP_UC * pServName,
                                          SAP_USHORT     servNo);

SAPRETURN API_DF  NiDg2Send              (const void       * pData,
                                          SAP_INT            dataLen,
                                          const NI_NODEADDR * pNodeAddr,
                                          const SAP_UC     * pServName,
                                          SAP_USHORT         servNo);

/*--------------------------------------------------------------------*/
/* NiDgReceive ()                                                     */
/*                                                                    */
/* Receive datagram on thread server handle                           */
/*                                                                    */
/* INPUT   ppBuffer        void pointer as out parameter              */
/*         pBufferLen      SAP_INT as out parameter                   */
/*         timeout         timeot in milliseconds; or NI_BLOCK for    */
/*                         blocking                                   */
/*                                                                    */
/* OUTPUT  ppBuffer        pointer to thread global buffer (buffer    */
/*                         limit is 1024)                             */
/*         pBufferLen      datagram length                            */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        ppBuffer = NULL, pBufferLen = NULL or      */
/*                         handle not bound yet (NiDgBind not called) */
/*                         (errInfo set)                              */
/*         NIETIMEOUT      receive timed out                          */
/*         NIEWAKEUP       wakeup datagram received                   */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - datagram length less or equal 1 byte are handled as wakeup */
/*         packets. If multiple wakeup packets are detected, they     */
/*         are handled as one single wakeup                           */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF  NiDgReceive            (void    ** ppBuffer,
                                          SAP_INT  * pBufferLen,
                                          SAP_INT    timeout);

/*--------------------------------------------------------------------*/
/* NiDgGetHdl ()                                                      */
/*                                                                    */
/* Get internal thread server handle                                  */
/*                                                                    */
/* INPUT   pHandle         NI_HDL as out parameter                    */
/*                                                                    */
/* OUTPUT  pHandle         thread global server handle                */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        pHandle = NULL or handle not bound yet     */
/*                         (NiDgBind not called) (errInfo set)        */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF  NiDgGetHdl             (NI_HDL * pHandle);

/*--------------------------------------------------------------------*/
/* NiDgFreeResrc ()                                                   */
/*                                                                    */
/* Release internal thread global server and client handle            */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF  NiDgFreeResrc         (void);


/* ################################################################## */




/*--------------------------------------------------------*/
/* NiRawPeek ()                                           */
/*                                                        */
/* Testen, ob Daten auf einer best. Leitung angekommen    */
/* sind. Dabei wird nicht die Pufferung benutzt. Diese    */
/* wuerde interne Puffer fuellen, die fuer NiRawRead      */
/* nicht nutzbar sind.                                    */
/*                                                        */
/* INPUT   handle        zu ueberpruefender Handle        */
/*         timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar (Daten da)            */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiRawPeek              (NI_HDL   handle,
                                          SAP_INT  timeout);

/*--------------------------------------------------------*/
/* NiRawWriteOK ()                                        */
/*                                                        */
/* Testen, ob Write auf Handle blockieren wuerde.         */
/* Dabei wird nicht die Pufferung benutzt. Diese          */
/* wuerde interne Puffer fuellen, die fuer NiRawRead      */
/* nicht nutzbar sind.                                    */
/*                                                        */
/* INPUT   handle        zu ueberpruefender Handle        */
/*         timeout       in MilliSekunden (oder NI_BLOCK) */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar (Daten da)            */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF  NiRawWriteOK           (NI_HDL   handle,
                                          SAP_INT  timeout);


/*--------------------------------------------------------*/
/* NiReadPending ()                                       */
/* NiWritePending ()                                      */
/*                                                        */
/* Info ueber noch zu lesende / schreibende Bytes liefern */
/* (fuer das letzte NI-Paket)                             */
/*                                                        */
/* INPUT   handle        Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* < 0                   handle ungueltig                 */
/* sonst                 noch zu lesende / schreibende    */
/*                       Bytes                            */
/*                                                        */
/*--------------------------------------------------------*/

SAP_INT API_DF    NiReadPending          (NI_HDL  handle);
SAP_INT API_DF    NiWritePending         (NI_HDL  handle);


/*--------------------------------------------------------*/
/* NiWait ()                                              */
/*                                                        */
/* n Millisekunden warten                                 */
/*                                                        */
/* INPUT   millisec      Zeitspanne in Millisekunden      */
/*                                                        */
/*--------------------------------------------------------*/

void API_DF       NiWait                 (SAP_INT  millisec);


/* ################################################################## */
/* # get and set NI parameters / NI handle parameters               # */
/* ################################################################## */

/*--------------------------------------------------------*/
/* NiSetParam ()                                          */
/* NiSetParamEx ()                                        */
/*                                                        */
/* Allgemeinen NI-Parameter setzen                        */
/* NiSetParamEx mit beliebigen Datentypen                 */
/* Fuer komplexe Datentypen muss NiSetParamEx benutzt     */
/* werden, da sonst beim cast Daten verloren gehen        */
/* koennen.                                               */
/*                                                        */
/* INPUT   param         NI-Parameter (s.u.)              */
/*                                                        */
/* IN/OUT  value         Wert                             */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              unzulaessiger Parameter / Wert   */
/*                                                        */
/*--------------------------------------------------------*/

/*--------------------------------------------------------*/
/* NiGetParam ()                                          */
/*                                                        */
/* Allgemeinen NI-Parameter lesen (s.u.)                  */
/*                                                        */
/* INPUT   param         NI-Parameter                     */
/*                                                        */
/* RETURN                Wert des Parameters              */
/*                                                        */
/*--------------------------------------------------------*/
/*--------------------------------------------------------*/
/* NiGetParamEx ()                                        */
/*                                                        */
/* Allgemeinen NI-Parameter lesen (s.u.)                  */
/* NiGetParamEx kann mit beliebigen Datentypen augerufen  */
/* werden. Der Speicher fuer die Daten muss vom Aufrufer  */
/* zur Verfuegung gestellt werden (bei Zeigerwerten).     */
/* Vor dem Aufruf muss der Datentyp in der Struktur auf   */
/* den richtigen Wert gesetzt werden (NI_DT_???).         */
/* Der Wert des Parameters wird im Erfolgsfall in der     */
/* in der Struktur zurueckgeliefert.                      */
/*                                                        */
/* INPUT/OUTPUT                                           */
/*         pdata         Zeiger auf NI-Parameterstruktur  */
/*                                                        */
/* RETURN                Wert                             */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              unzulaessiger Parameter / Wert   */
/*                                                        */
/*--------------------------------------------------------*/


/*--------------------------------------------------------*/
/* NiSetHdlParam ()                                       */
/* NiGetHdlParam ()                                       */
/*                                                        */
/* Handlespezifischen NI-Parameter setzen/lesen           */
/*                                                        */
/* INPUT   hdl           NI-Handle                        */
/*         param         NI-Parameter (s.u.)              */
/*                                                        */
/* IN/OUT  value         Wert                             */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              unzulaessiger Parameter / Wert   */
/*                                                        */
/*--------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* allgemeine NI-Parameter NIP_...                                    */
/*                                                                    */
/* PORTRANGE   Bereich fuer lokale Connect-Port Auswahl einschraenken */
/*             0 = nein, sonst ja, Default 0                          */
/*                                                                    */
/* MAXPORT     Obergrenze Port-Range (s. NIP_PORTRANGE, Default 1023) */
/*                                                                    */
/* MINPORT     Untergrenze Port-Range (s. NIP_PORTRANGE, Default 0)   */
/*                                                                    */
/* INC_SERV_NO increment or decrement service number (port) in range  */
/*             if port is already in use (SAP_BOOL; default FALSE)    */
/*                                                                    */
/* CONT_IN_RANGE   continue by the last successful bound service      */
/*             number plus / minus one or start each time at the      */
/*             upper or lower port limit again                        */
/*             (SAP_BOOL; default TRUE)                               */
/*                                                                    */
/* BUF_OPCOD   NI-Buffer Opcodes (nur NiSetParam)                     */
/*             NI-Buffer ins Trace-File dumpen (0), flushen (1)       */
/*                                                                    */
/* HSBUF_MAX   Anzahl der max. pufferbaren Host/Serv-Eintraege setzen */
/*             (Default 100)                                          */
/*                                                                    */
/* LIB_VERS    NI_VERSION der NI-Library (nur NiGetParam)             */
/*                                                                    */
/* HSBUF_RETRY Anzahl in Sekunden, nach denen ein "Invalid"-Eintrag   */
/*             im Host/Serv-Buffer verfaellt (0 = nie)                */
/*                                                                    */
/* HSBUF_REV_RETRY Schalter, ob "Invalid"-Eintraege fuer              */
/*             IP->host-Aufloesungen verfallen sollen                 */
/*                                                                    */
/* FRAGTEST    Schaltet Fragment-Test-Modus ein (es wird jeweils nur  */
/*             1 Byte gelesen / geschrieben und dann ein Timeout      */
/*             zurueckgegeben (falls nicht NI_BLOCK))                 */
/*             (0 = aus, sonst ein, Default 0)                        */
/*                                                                    */
/* CONNLOCAL   Steuert, ob Connect ueber lokale Domain (AF_UNIX)      */
/*             erfolgen soll                                          */
/*                                                                    */
/* UDS_PATH    Pfadname fuer UNIX DOMAIN SOCKETS. Der Pfadname muss   */
/*             immer absolut angegeben werden!!!                      */
/*             Der Pfad muss ein %d enthalten. Dieser Platzhalter     */
/*             wird durch die socketnummer ersetzt.                   */
/*                                                                    */
/* QUEUE_LEN   Laenge der backlog queue beim listen call.             */
/*             Default Wert: 20                                       */
/*                                                                    */
/* QTEST       Test fuer das NI-Queuing. Ist der Parameter > 0,       */
/*             so errechnet sich die Wahrscheinlichkeit mit der       */
/*             beim Write ein timeout zurueckgegeben wird aus         */
/*             (NIP_QTEST & 0xff) (muss zwischen 0 und 100 sein).     */
/*             Es werden dann immer ((NIP_QTEST) >> 8 & 0xffff),      */
/*             Pakete hintereinander blockiert. Zum erzeugen des      */
/*             Wertes NIP_QTEST aus den einzelparametern benutzt      */
/*             man das Makro NI_QTEST_VAL.                            */
#define NI_QTEST_VAL(_count, _prop) (((_count) & 0xffff) << 8) | ((_prop) & 0xff)
/*                                                                    */
/* LOCAL_ADDR  IP-Addresse fuer ausgehende Verbindungen. Bei 0.0.0.0   */
/*             wird default Addresse verwendet.                        */
/*                                                                    */
/* DNS_TRC_LIMIT Trace Eintrag schrieben, falls die Hostnamen- oder   */
/*             Servicenamen-Aufloesung laenger als <value> ms dauert. */
/*             Trace ausschalten mit <value> = 0.                     */
/*             Daten-Typ: NI_DT_UINT; Defaut-Wert: 0                  */
/*                                                                    */
/* NIP_DNS_TIME_FUNC Time Funktion setzen, mit welcher die Zeit fuer  */
/*             DNS_TRC_LIMIT berechnet wird. Ist die Funktion NULL,   */
/*             wird eine interne verwendet, welche jedoch ungenau     */
/*             sein kann (Sekundenbereich).                           */
/*             Format: SAP_UINT (function *)(void)                    */
/*             Rueckgabe in Mikrosekunden (wie pfclock*())            */
/*             Daten-Typ: NI_DT_VOIDPTR; Defaut-Wert: NULL            */
/* NIP_TRACE_ALL  bestimmt ob alle neuen Handles den Trace            */
/*             eingeschaltet haben                                    */
/*                                                                    */
/* POLL_TRC_LIMIT   Trace Eintrag schrieben, falls der Poll oder      */
/*             Select Aufruf <value> Sekunden laenger als der timeout */
/*             dauert.                                                */
/*             Trace ausschalten mit <value> = 0.                     */
/*             Daten-Typ: NI_DT_UINT; Defaut-Wert: 0                  */
/*                                                                    */
/*--------------------------------------------------------------------*/
/* Pufferspezifische NI-Parameter NIHP_...                            */
/*                                                                    */
/* MODE        Sprachmodus / I/O-Modus der Verbindung: MESG, ROUT, RAW*/
/*                                                                    */
/* Parameter fuer NIHP_MODE in Ni?etHdlParam                          */
#define NI_MESG_IO          (SAP_RAW) 0  /* message-orientierte Komm. */
#define NI_RAW_IO           (SAP_RAW) 1  /* stream-orient. Kommunik.  */
#define NI_ROUT_IO          (SAP_RAW) 2  /* spez. Mode fuer Saprouter */
/* SINGLE_READ NT only: Einzelschritt                                 */
/*                                                                    */
/* CHECK_WAIT  PING/PONG Synchronisation                              */
/*                                                                    */
/* CHECK_QUE   Statusabfrage/Setzen einer Verbindung bei der          */
/*             Pufferverwaltung.                                      */
/*             Mit SetParam wird das Check-flag gesetzt               */
/*             Mit GetParam wird der Status abgefragt                 */
/*             (0: Outqueue ist leer, 1: queue voll und check flag 0  */
/*              2: outqueue voll und check flag 1)                    */
/*                                                                    */
/* ALLOC_FUNC  Funktion zur Erfuellung von Speicheranfragen           */
/*                                                                    */
/* MAXQUEUE    Maximale Anzahl von Paketen in der outqueue. Der       */
/*             Sender wird abgebremst, bis wieder Pakete in die       */
/*             queue passen.                                          */
/*                                                                    */
/* WARNQUEUE   Schwellenwert fuer die Anzahl an Paketen, die noch     */
/*             in die Queue passen sollen. Wird dieser Wert unter-    */
/*             schritten, wird der Fehler NIEQUEFULL zurueckgegeben.  */
/*                                                                    */
/* BUFSTAT     Bufferstatus: ON / OFF                                 */
/* Parameter fuer NIHP_BUFSTAT in Ni?etHdlParam                       */
#define NI_BUFSTAT_OFF      (SAP_RAW) 0  /* Pufferung explizit aus    */
#define NI_BUFSTAT_ON       (SAP_RAW) 1  /* Pufferung an              */
/*                                                                    */
/* MAXMLEN     maximal NI-Message length                              */
/*                                                                    */
/* MAXHEAP     Maximale Speichernutzung durch die Queues. Wird dieser */
/*             Schwellenwert ueberschritten, wird der Fehler          */
/*             NIEQUEFULL zurueckgegeben.                             */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/

/* function parameter structure */
typedef struct /* NI_PARAM_DATA */
{
    SAP_RAW                param;         /* parameter                */
    SAP_RAW                datatype;      /* type of attached data    */

    union {                               /* attached value           */
        SAP_BOOL           bool_val;      /* field for NI_DT_BOOL     */
        SAP_RAW            raw_val;       /* field for NI_DT_RAW      */
        SAP_USHORT         ushort_val;    /* field for NI_DT_USHORT   */
        SAP_UINT           uint_val;      /* field for NI_DT_UINT     */
        NI_NODEADDR        nodeaddr_val;  /* field for NI_DT_NODEADDR */
        void             * voidptr_val;   /* field for NI_DT_VOIDPTR  */
        SAP_UC           * ucptr_val;     /* field for NI_DT_UCPTR    */
        NIBUF_ALLOC_FUNC * allocfunc_val; /* field for NI_DT_ALLOCFUNC*/

        SAP_INT            int_val;
    } value;
} NI_PARAM_DATA;


/* NI parameter */
#define NIP_PORTRANGE       (SAP_RAW)  1  /* type NI_DT_BOOL          */
#define NIP_MAXPORT         (SAP_RAW)  2  /* type NI_DT_USHORT        */
#define NIP_MINPORT         (SAP_RAW)  3  /* type NI_DT_USHORT        */
#define NIP_BUF_OPCOD       (SAP_RAW)  4  /* type NI_DT_RAW           */
#define NIP_HSBUF_MAX       (SAP_RAW)  5  /* type NI_DT_UINT          */
#define NIP_LIB_VERS        (SAP_RAW)  6  /* type NI_DT_UINT          */
#define NIP_HSBUF_RETRY     (SAP_RAW)  7  /* type NI_DT_UINT          */
#define NIP_HSBUF_REV_RETRY (SAP_RAW)  8  /* type NI_DT_UINT          */
#define NIP_FRAGTEST        (SAP_RAW)  9  /* type NI_DT_UINT          */
#define NIP_CONNLOCAL       (SAP_RAW) 10  /* type NI_DT_BOOL          */
#define NIP_UDS_PATH        (SAP_RAW) 11  /* type NI_DT_UCPTR         */
#define NIP_QUEUE_LEN       (SAP_RAW) 12  /* type NI_DT_UINT          */
#define NIP_QTEST           (SAP_RAW) 13  /* type NI_DT_UINT          */
#define NIP_LOCAL_ADDR      (SAP_RAW) 14  /* type NI_DT_NODEADDR      */
#define NIP_DNS_TRC_LIMIT   (SAP_RAW) 15  /* type NI_DT_UINT          */
#define NIP_DNS_TIME_FUNC   (SAP_RAW) 16  /* type NI_DT_VOIDPTR       */
#define NIP_TRACE_ALL       (SAP_RAW) 17  /* type NI_DT_BOOL          */
#define NIP_INC_SERV_NO     (SAP_RAW) 18  /* type NI_DT_BOOL          */
#define NIP_CONT_IN_RANGE   (SAP_RAW) 19  /* type NI_DT_BOOL          */
#define NIP_POLL_TRC_LIMIT  (SAP_RAW) 20  /* type NI_DT_UINT          */

/* NI buffer parameter */
#define NIBUFP_MAXHEAP      (SAP_RAW) 30  /* type NI_DT_UINT          */

/* handle parameter */
#define NIHP_SINGLE_READ    (SAP_RAW) 50  /* type NI_DT_BOOL          */
#define NIHP_CHECK_WAIT     (SAP_RAW) 51  /* without value            */

/* buffer handle parameter */
#define NIBUFHP_MODE        (SAP_RAW) 60  /* type NI_DT_RAW           */
#define NIBUFHP_BUFSTAT     (SAP_RAW) 61  /* type NI_DT_RAW           */
#define NIBUFHP_CHECK_QUE   (SAP_RAW) 62  /* type NI_DT_RAW           */
#define NIBUFHP_ALLOC_FUNC  (SAP_RAW) 63  /* type NI_DT_ALLOCFUNC     */
#define NIBUFHP_MAXQUEUE    (SAP_RAW) 64  /* type NI_DT_UINT          */
#define NIBUFHP_WARNQUEUE   (SAP_RAW) 65  /* type NI_DT_UINT          */
#define NIBUFHP_MAXMLEN     (SAP_RAW) 66  /* type NI_DT_UINT          */


/* type of attacehed data */
#define NI_DT_UNKNOWN       (SAP_RAW)  0
#define NI_DT_BOOL          (SAP_RAW)  1
#define NI_DT_RAW           (SAP_RAW)  2
#define NI_DT_USHORT        (SAP_RAW)  3
#define NI_DT_UINT          (SAP_RAW)  4
#define NI_DT_NODEADDR      (SAP_RAW)  5
#define NI_DT_VOIDPTR       (SAP_RAW)  6
#define NI_DT_UCPTR         (SAP_RAW)  7
#define NI_DT_ALLOCFUNC     (SAP_RAW)  8


/* functions */
SAPRETURN API_DF NiGetParamEx    (NI_PARAM_DATA       * pParamData);

SAPRETURN API_DF NiSetParamEx    (const NI_PARAM_DATA * pParamData);

SAPRETURN API_DF NiGetHdlParamEx (NI_HDL                handle,
                                  NI_PARAM_DATA       * pParamData);

SAPRETURN API_DF NiSetHdlParamEx (NI_HDL                handle,
                                  const NI_PARAM_DATA * pParamData);


/* old functions and names; do not use any more! */
SAP_INT   API_DF NiGetParam      (SAP_RAW         param);

SAPRETURN API_DF NiSetParam      (SAP_RAW         param,
                                  SAP_INT         value);

SAPRETURN API_DF NiGetHdlParam   (NI_HDL          handle,
                                  SAP_RAW         param,
                                  void          * pValue);

SAPRETURN API_DF NiSetHdlParam   (NI_HDL          handle,
                                  SAP_RAW         param,
                                  const void    * pValue);

/* ################################################################## */

SAP_BOOL         NiGetIPv6Support ( void );

/*--------------------------------------------------------*/
/* NiErrSet ()                                            */
/*                                                        */
/* Sap-Error-Info ueber Ni-Returncode bereitstellen.      */
/* Die Sap-Error-Info kann anschliessend ueber die        */
/* Funktionen der Sap-Error-Schnittstelle abgeholt werden */
/* (siehe err.h)                                          */
/*                                                        */
/* INPUT   nirc          Ni-Returncode                    */
/*                                                        */
/*--------------------------------------------------------*/

void API_DF       NiErrSet               (SAPRETURN  nirc);


/*--------------------------------------------------------*/
/* NiErrStr ()                                            */
/*                                                        */
/* Namens-String  zu Ni-Error-Konstante liefern           */
/*                                                        */
/* INPUT   nirc          Ni-Returncode                    */
/*                                                        */
/* RETURN                Namensstring                     */
/*                                                        */
/*--------------------------------------------------------*/

const SAP_UC * API_DF NiErrStr (SAPRETURN   nirc);


/*--------------------------------------------------------------------*/
/* NiSecStr  ()                                                       */
/* NiSecStr2 ()   (reentrant)                                         */
/*                                                                    */
/* SapRouter-Passwort aus String (Route) ausblenden                   */
/*                                                                    */
/* INIT    bei der reentrant library (nirlib) wird beim Aufruf der    */
/*         nicht reentrant Funktion NI initialisiert.                 */
/*                                                                    */
/* INPUT   inStr         Inputstring                                  */
/*         secStr        String Puffer fuer String ohne Passwort      */
/*         bufLen        Groesse des Puffers in SAP_UC                */
/*                       (optimal: strlenU(inStr) + 1)                */
/*                                                                    */
/* RETURN                String mit ausgeblendetem Passwort (falls    */
/*                       String groesser ist als interner Puffer,     */
/*                       wird der String unveraendert zurueckgegeben, */
/*                       bei internem Fehler wird NULL zurueckgegeben)*/
/*                                                                    */
/* SAP_O_K               alles klar                                   */
/* NIEINVAL              inStr = NULL bzw. secStr = NULL              */
/* NIETOO_SMALL          Puffer zu klein                              */
/*                                                                    */
/* Bei der nicht reentrant Funktionen wird threadeigener statischer   */
/* Speicher zurueckgegeben. Die Nutzung der reentrant Funktionen wird */
/* empfohlen!                                                         */
/*                                                                    */
/*--------------------------------------------------------------------*/

const SAP_UC * API_DF NiSecStr  (const SAP_UC * inStr);
SAPRETURN      API_DF NiSecStr2 (const SAP_UC * inStr,
                                 SAP_UC       * secStr,
                                 SAP_UINT       bufLen);


/*--------------------------------------------------------------------*/
/* Ni???ToStr  ()                                                     */
/* Ni???ToStr2 ()   (reentrant)                                       */
/* Ni???ToStr3 ()   (reentrant, if parameters are valid)              */
/*                                                                    */
/* Konvertiert Nodeaddresse bzw. Service Nummer in String              */
/*                                                                    */
/* INIT    bei der reentrant library (nirlib) wird beim Aufruf einer  */
/*         nicht reentrant Funktion NI initialisiert.                 */
/*                                                                    */
/* INPUT   nodeAddr      Nodeaddresse (NULL -> "0.0.0.0")              */
/*         nodeAddrStr   String Puffer fuer Nodeaddresse               */
/*         netServNo     Service-Nummer (Net-Byte-Order)              */
/*         servNoStr     String Puffer fuer Service-Nummer            */
/*         bufLen        Groesse des Puffers in SAP_UC (optimal fuer  */
/*                       nodeAddr: 16 bzw. ServNo: 6; ServNo L: 12;   */
/*                       vgl. NI_NODEADDR_STR_LEN etc.)               */
/*                                                                    */
/* RETURN                formatierter String                          */
/*                                                                    */
/* SAP_O_K               alles klar                                   */
/* NIEINVAL              nodeAddrStr = NULL bzw. servNoStr = NULL     */
/* NIETOO_SMALL          Puffer zu klein                              */
/*                                                                    */
/* Bei den nicht reentrant Funktionen wird threadeigener statischer   */
/* Speicher zurueckgegeben. Bei den Funktionen NiSrvToStr und         */
/* NiSrvToStrL handelt es sich um den selben Speicher, NiAddrToStr hat */
/* eigenen. Die Nutzung der reentrant Funktionen wird empfohlen!      */
/*                                                                    */
/* INFO  - the functions Ni???ToStr3 are a mixture of the other both. */
/*         The returned pointer is identical with the passed buffer   */
/*         if the parameters are valid and the buffer is long enough  */
/*         (cf. NIEINVAL and NIETOO_SMALL). Otherwise the static      */
/*         internal buffer is retured.                                */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN      API_DF NiAddrToStr       (const NI_NODEADDR * pNodeAddr,
                                         SAP_UC            * pNodeAddrStr,
                                         SAP_UINT            bufferLen,
                                         SAP_BOOL            useIPv4Format);

SAP_UC *       API_DF NiAddrToStr2      (const NI_NODEADDR * pNodeAddr,
                                         SAP_UC            * pNodeAddrStr,
                                         SAP_UINT            bufferLen,
                                         SAP_BOOL            useIPv4Format);

const SAP_UC * API_DF NiAddrToStrStatic (const NI_NODEADDR * pNodeAddr,
                                         SAP_BOOL            useIPv4Format);

const SAP_UC * API_DF NiSrvToStr        (SAP_USHORT          netServNo);
SAPRETURN      API_DF NiSrvToStr2       (SAP_USHORT          netServNo,
                                         SAP_UC            * servNoStr,
                                         SAP_UINT            bufLen);
SAP_UC *       API_DF NiSrvToStr3       (SAP_USHORT          netServNo,
                                         SAP_UC            * servNoStr,
                                         SAP_UINT            bufLen);

const SAP_UC * API_DF NiSrvToStrL       (SAP_USHORT          netServNo);
SAPRETURN      API_DF NiSrvToStrL2      (SAP_USHORT          netServNo,
                                         SAP_UC            * servNoStr,
                                         SAP_UINT            bufLen);
SAP_UC *       API_DF NiSrvToStrL3      (SAP_USHORT          netServNo,
                                         SAP_UC            * servNoStr,
                                         SAP_UINT            bufLen);


/*--------------------------------------------------------------------*/
/* NiStrToAddr     ()                                                 */
/* NiStrToAddrMask ()                                                 */
/*                                                                    */
/* NiAddrMatch    ()                                                  */
/* NiAddrStrMatch ()                                                  */
/*                                                                    */
/* Convert a address-string to a address / address-mask               */
/*                                                                    */
/* INPUT   pNodeAddrStr    address string (cf. INFO part)             */
/*         pNodeAddr       NI_NODEADDR as out parameter               */
/*         pNodeAddrMask   NI_NODEADDR as out parameter               */
/*                         (NiStrToAddrMask)                          */
/*         pNodeAddrA      address A                                  */
/*         pNodeAddrB      address B                                  */
/*         pNodeAddrMask   subnet-mask (NiAddrMatch)                  */
/*                                                                    */
/* OUTPUT  pNodeAddr       binary address of address-string           */
/*         pNodeAddrMask   binary subnet-mask of address-string       */
/*                         (set bits are relevant bits)               */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        invalid address-string format              */
/*                                                                    */
/*         TRUE            pNodeAddrA and -B belong to the same subnet*/
/*         FALSE           invalid parameter or pNodeAddrA and -B do  */
/*                         not belong to the same subnet              */
/*                                                                    */
/* INFO  - supported address formats:                                 */
/*         "10.17.69.0", "0.0.0.0", "1080:0:0:0:8:800:200C:417a",     */
/*         "FF01::101", "::", "::1", "0:0:0:0:0:0:13.1.68.3",         */
/*         "::fffF:129.144.52.38", etc.                               */
/*                                                                    */
/*       - supported address-mask formats (with '/'):                 */
/*         "10.17.69.0/24", "10.17.69/24", "1080::8:800:200C:417a/64",*/
/*         "FF01::101/64", "::fffF:129.144.52.38/24", etc.            */
/*                                                                    */
/*       - supported address-mask formats (with '*', IPv4 only):      */
/*         "10.17.*.*", "10.17.69.*", etc.                            */
/*                                                                    */
/*       - supported address-mask formats (with 'x', IPv4 only):      */
/*         "10.17.0100xxxx.*", "10.17.69.0xxxxxx", etc.               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiStrToAddr     (NI_NODEADDR  * pNodeAddr,
                                  const SAP_UC * pNodeAddrStr);

SAPRETURN API_DF NiStrToAddrMask (const SAP_UC * pNodeAddrStr,
                                  NI_NODEADDR  * pNodeAddr,
                                  NI_NODEADDR  * pNodeAddrMask);

SAP_BOOL  API_DF NiAddrMatch     (NI_NODEADDR  * pNodeAddrA,
                                  NI_NODEADDR  * pNodeAddrB,
                                  NI_NODEADDR  * pNodeAddrMask);

SAP_BOOL  API_DF NiAddrStrMatch  (const SAP_UC * pNodeAddrStrA,
                                  const SAP_UC * pNodeAddrStrB);


/*--------------------------------------------------------------------*/
/* NiNetINT  ()                                                       */
/* NiHostINT ()                                                       */
/*                                                                    */
/* Konvertiere SAP_INTs von, bzw. nach Netzbyteorder                  */
/*                                                                    */
/* INIT    NI muss nicht initialisirt sein.                           */
/*                                                                    */
/* INPUT   hostInt       Integer in Host-Byte-Order                   */
/*         netInt        Integer in Net-Byte-Order                    */
/*                                                                    */
/* RETURN                konvertierter Interegr                       */
/*                                                                    */
/*--------------------------------------------------------------------*/

/* SAPUNICODEOK_MIXEDLEN */
SAP_INT API_DF    NiNetINT       (SAP_INT  hostInt);

/* SAPUNICODEOK_MIXEDLEN */
SAP_INT API_DF    NiHostINT      (SAP_INT  netInt);

/*--------------------------------------------------------------------*/
/* NiNetServno  ()                                                    */
/* NiHostServno ()                                                    */
/*                                                                    */
/* Konvertiere Servicenummern von, bzw. nach Netzbyteorder            */
/*                                                                    */
/* INIT    NI muss nicht initialisirt sein.                           */
/*                                                                    */
/* INPUT   hostServNo    Servicenummer in Host-Byte-Order             */
/*         netServNo     Servicenummer in Net-Byte-Order              */
/*                                                                    */
/* RETURN                konvertierte Servicenummer                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAP_USHORT API_DF NiNetServno    (SAP_USHORT  hostServNo);

SAP_USHORT API_DF NiHostServno   (SAP_USHORT  netServNo);


/*--------------------------------------------------------*/
/* NiSetSingleRead ()                                     */
/*                                                        */
/* Setze Socket auf 'Single Read'                         */
/* `Single Read' heisst, dass in jedem NiRead nur einmal  */
/* versucht wird zu lesen. Normalerweise wird nach        */
/* erfolgreichem Lesen, nocheinmal versucht zu lesen,     */
/* falls das NI-Paket nicht komplett ist. Dies wird       */
/* hiermit verhindert. (Ist unter Windows notwendig.)     */
/*                                                        */
/* INPUT  handle    NI-Handle                             */
/*        mode      TRUE/FALSE                            */
/*                                                        */
/* RETURN SAP_O_K   alles klar                            */
/*        NIEINVAL  kein gueltiger NI-Handle              */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiSetSingleRead (NI_HDL  handle,
                                  SAP_BOOL  mode);

/*--------------------------------------------------------*/
/* NiGetSingleRead ()                                     */
/*                                                        */
/* Abfrage auf 'Single Read'                              */
/*                                                        */
/* INPUT  handle    NI-Handle                             */
/*                                                        */
/* RETURN modus                                           */
/*                                                        */
/*--------------------------------------------------------*/

SAP_BOOL  API_DF NiGetSingleRead (NI_HDL  handle);

/*--------------------------------------------------------*/
/* NiGetRawSocket                                         */
/*                                                        */
/* Gebe den socket an den Aufrufer zurueck. Nur RAW       */
/* sockets koennen (sinnvoll) genutzt werden. Socket      */
/* darf nicht von extern veraendert (z.B. geschlossen)    */
/* werden!                                                */
/*                                                        */
/* IN                                                     */
/*      handle           NI-Handle                        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINTERN             interner Fehler aufgetreten      */
/* NIEINVAL              kein valider Parameter           */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiGetRawSocket    (NI_HDL      handle,
                                    int         *socket);

/*--------------------------------------------------------*/
/* NiSelNewSet ()                                         */
/*                                                        */
/* Fordert eine neues SelectSet an                        */
/*                                                        */
/* IN/OUT  set           selectset                        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINTERN             interner Fehler aufgetreten      */
/*                                                        */
/*--------------------------------------------------------*/

/*-NiSel--------------------------------------------------*/

SAPRETURN NiSelNewSet     (NISEL_HDL *sel_hdl_ptr);

/*--------------------------------------------------------*/
/* NiSelNewSet2 ()                                        */
/*                                                        */
/* Fordert eine neues SelectSet an                        */
/*                                                        */
/* IN/OUT  set           selectset                        */
/*         max_sockets   maximum number of sockets        */
/*                       in the select set                */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINTERN             interner Fehler aufgetreten      */
/*                                                        */
/*--------------------------------------------------------*/

/*-NiSel--------------------------------------------------*/

SAPRETURN NiSelNewSet2    (NISEL_HDL *sel_hdl_ptr, int max_sockets);
SAPRETURN NiSelNewSet3    (NISEL_HDL * pNewSelSet,
                           SAP_INT     maxSockets,
                           SAP_RAW   * pImpl);

/*--------------------------------------------------------*/
/* NiSelDestroySet ()                                     */
/*                                                        */
/* Gibt eine altes SelectSet wieder frei                  */
/*                                                        */
/* IN/OUT  set           selectset                        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              kein valider selectset           */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN NiSelDestroySet (NISEL_HDL *sel_hdl_ptr);

/*--------------------------------------------------------*/
/* NiSelClearSet ()                                       */
/*                                                        */
/* Loescht alle Eintaege in einem SelectSet               */
/*                                                        */
/* INPUT   set           selectset                        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              kein valider selectset           */
/* NIEINTERN             interner Fehler                  */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN NiSelClearSet   (NISEL_HDL sel_hdl);

/*--------------------------------------------------------*/
/* NiSelSet/Clear ()                                      */
/*                                                        */
/* Setzt/Loescht die flags fuer diesen NI-Handle flags    */
/* kann hier aus einer Bit-Kombination von NI_WRITE,      */
/* NI_READ und NI_CONNECT bestehen. NiSelSet setzt        */
/* ausserdem die data zu diesem Handle.                   */
/*                                                        */
/* INPUT   set           selectset                        */
/*         hdl           NI-Handle                        */
/*         flags         Bitkombination aus NI_CONNECT,   */
/*                       NI_READ und NI_WRITE             */
/*         void          beliebige Applikationsdaten      */
/*                                                        */
/* IN/OUT  value         Wert                             */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINTERN             interner Fehler                  */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN NiSelSet        (NISEL_HDL sel_hdl,
                           NI_HDL hdl,
                           SAP_RAW flags,
                           const void *data);

SAPRETURN NiSelClear      (NISEL_HDL sel_hdl,
                           NI_HDL hdl,
                           SAP_RAW flags);

/*--------------------------------------------------------*/
/* NiSelSelect ()                                         */
/*                                                        */
/* Ersatz fuer NiSelect                                   */
/* Setzt den Iterator fuer NiSelNext implizit zurueck     */
/*                                                        */
/* INPUT   set           selectset                        */
/*         timeout       timeout fuers select             */
/*                                                        */
/* RETURN                                                 */
/*  siehe NiSelect                                        */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN NiSelSelect     (NISEL_HDL sel_hdl, SAP_INT timeout);

/*--------------------------------------------------------*/
/* NiSelNext  ()                                          */
/*                                                        */
/* Geben ersten/naechsten NI-Handle, der laut mode bereit */
/* ist, zurueck. Direkt nach NiSelect mit diesem Set sind */
/* beide aequivalent                                      */
/*                                                        */
/* INPUT   set           selectset                        */
/* IN/OUT  read/                                          */
/*         write/                                         */
/*         connect       Wenn NULL, modus interessiert    */
/*                                  nicht                 */
/*                       sonst Pointer auf Variable       */
/*                                                        */
/* OUT     data          Userdata                         */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* Ni_INVALID_HDL        kein Handle mehr verfuegbar      */
/*                                                        */
/*--------------------------------------------------------*/

NI_HDL    NiSelNext       (NISEL_HDL sel_hdl,
                           SAP_BOOL *read,
                           SAP_BOOL *write,
                           SAP_BOOL *connect,
                           void **data);

/*--------------------------------------------------------*/
/* NiSelStartLoop ()                                      */
/*                                                        */
/* Setzt den Iterator fuer NiSelNext wieder auf den ersten*/
/* Eintrag                                                */
/*                                                        */
/* INPUT   set           selectset                        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              kein valider selectset           */
/* NIEINTERN             interner Fehler                  */
/*                                                        */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN NiSelStartLoop  (NISEL_HDL sel_hdl);

/*--------------------------------------------------------*/
/* NiSelMode ()                                           */
/* NiSelStat ()                                           */
/*                                                        */
/* Gib den Status/Modus zu hdl zurueck                    */
/* Status ist dabei der Status, den das letzte Select     */
/* zurueckgeliefert hat, waehrend modus der in diesem     */
/* Set angeforderte Modus fuer das naechste Select ist.   */
/*                                                        */
/* INPUT   set           selectset                        */
/*         hdl           NI-Handle                        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* modus des Handles in der maske                         */
/*                                                        */
/*--------------------------------------------------------*/

SAP_RAW API_DF NiSelMode     (NISEL_HDL sel_hdl, NI_HDL hdl);
SAP_RAW API_DF NiSelStat     (NISEL_HDL sel_hdl, NI_HDL hdl);

/*-NiBuf--------------------------------------------NiSel-*/

/*--------------------------------------------------------*/
/* NiBufReceive                                           */
/*                                                        */
/* Empfange einen NI-Puffer                               */
/*                                                        */
/* INPUT   hdl           Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*         timeout       Zeitlimit                        */
/*                                                        */
/* OUTPUT  buffer        Pointer auf Pufferpointer        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIECONN_BROKEN        Verbindung zum Partner abgebr.   */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiBufReceive  (NI_HDL      hdl,
                                NIBUFFER  **buffer,
                                SAP_INT     timeout);

/*--------------------------------------------------------*/
/* NiBufSend                                              */
/*                                                        */
/* Sende einen NI-Puffer                                  */
/*                                                        */
/* INPUT   hdl           Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*         timeout       Zeitlimit                        */
/* IN/OUT  buffer        NI-Puffer zum senden. Wird bei   */
/*                       Erfolg freigegeben.              */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIECONN_BROKEN        Verbindung zum Partner abgebr.   */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiBufSend     (NI_HDL      hdl,
                                NIBUFFER  **buffer,
                                SAP_INT     timeout);

/*--------------------------------------------------------*/
/* NiBufSendHandle                                        */
/*                                                        */
/* Sende einen NI-Handle ueber einen anderen              */
/*                                                        */
/* INPUT   handle        Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*         sendHandle    Handle, der gesendet werden soll */
/*         timeout       Zeitlimit                        */
/*         saveModes     handle Eigenschaften uebernehmen */
/*                       - buffered   (def. TRUE )        */
/*                       - talkmode   (def. NI   )        */
/*                       - blocking   (def. FALSE)        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIECONN_BROKEN        Verbindung zum Partner abgebr.   */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiBufSendHandle (NI_HDL     handle,
                                  NI_HDL     sendHandle,
                                  SAP_INT    timeout,
                                  SAP_BOOL   saveModes);

SAPRETURN API_DF NiBufGetSentHdl (NI_HDL     inHandle,
                                  SAP_INT    timeout,
                                  NI_HDL   * pOutHandle);


/*--------------------------------------------------------*/
/* NiBufFree                                              */
/*                                                        */
/* Dies ist die Funktion, ueber die beliebige NI-Puffer   */
/* freigegeben werden. Sie wird intern dazu aufgerufen    */
/* und kann von der Applikation gerufen werden.           */
/*                                                        */
/* IN/OUT  buffer        Alter Puffer                     */
/*                                                        */
/*--------------------------------------------------------*/

void     API_DF NiBufFree     (NIBUFFER  **buffer);

/*--------------------------------------------------------*/
/* NiBufAlloc                                             */
/*                                                        */
/* Allokiere einen neuen NI-Puffer                        */
/*                                                        */
/* INPUT   len           Laenge des Datenbereichs         */
/*         txt           Erkennungstext fuer den Puffer   */
/*                                                        */
/* OUTPUT  buffer        Neuer Puffer                     */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiBufAlloc    (NIBUFFER  **buffer,
                                SAP_INT     len,
                                const SAP_UC     *txt);

/*--------------------------------------------------------*/
/* NiBufAlloc2                                            */
/*                                                        */
/* Allokiere einen neuen NI-Puffer, bei dem eine externe  */
/* Free-Funktion vorhanden ist, die von NiBufFree         */
/* aufgerufen wird, anstatt den Puffer sofort wieder      */
/* freizugeben. Diese Freefunktion kann dann NiBufFree2   */
/* aufrufen, um den Puffer endgueltig freizugeben         */
/*                                                        */
/* INPUT  len           Laenge des Datenbereichs          */
/*        txt           Erkennungstext fuer den Puffer    */
/* OUTPUT buffer        Neuer Puffer                      */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/
/* NiBufFree2                                             */
/*                                                        */
/* Gibt einen NI-Puffer frei, ohne evtuelle externe       */
/* freefunktion zu rufen (siehe NiBufAlloc2)              */
/*                                                        */
/* IN/OUT  buffer        Alter Puffer                     */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiBufAlloc2   (NIBUFFER  **buffer,
                                SAP_INT     len,
                                const SAP_UC     *text,
                                NIBUF_FREE_FUNC *func);

void      API_DF NiBufFree2    (NIBUFFER  **buffer);

/*--------------------------------------------------------*/
/* NiBufDup                                               */
/*                                                        */
/* Dupliziere einen Puffer, um ihn an mehrere Clients zu  */
/* senden. Nach dem Senden einer Referenz darf der Inhalt */
/* des Puffers nicht mehr veraendert werden.              */
/*                                                        */
/* INPUT   from          Bestehender Puffer               */
/*                                                        */
/* OUTPUT  to            Neue Referenz auf Puffer from    */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiBufDup      (NIBUFFER  **to,
                                NIBUFFER   *from);

/*--------------------------------------------------------*/
/* NiBufWrap                                              */
/*                                                        */
/* Erstelle einen Puffer zu einem bestehenden             */
/* Datenbereich:                                          */
/* Wenn der Puffer NiBufFree ruft dann die den Parameter  */
/* 'freef', um den Datenbereich freizugeben. Diese        */
/* Funktion muss dann NiBufUnwrap aufrufen.               */
/*                                                        */
/* INPUT   data          Anfang auf den Datenbereich      */
/*         len           Laenge des Datenbereichs         */
/*         spacelen      Laenge des gesammten Puffers     */
/*         freef         Freigabefunktion                 */
/*                                                        */
/* OUTPUT  to            Neue Referenz auf Puffer from    */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/
/* NiBufUnwrap                                            */
/*                                                        */
/* Entpacke einen mit NiBufWrap erzeugten NI-Puffer.      */
/* Diese Funktion sollte nur aus der in NiBufWrap         */
/* angegebenen Freefunktion aufgerufen werden             */
/*                                                        */
/* IN/OUT  buffer        NI-Puffer                        */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* NULL                  Puffer war nicht mit NiBufWrap   */
/*                       erzeugt                          */
/* sonst                 Datenbereich                     */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiBufWrap     (NIBUFFER  **to,
                                SAP_RAW    *data,
                                SAP_INT     len,
                                SAP_INT     spacelen,
                                NIBUF_FREE_FUNC *freef,
                                void       *uptr);

void *    API_DF NiBufUnWrap   (NIBUFFER  **buffer);

/*--------------------------------------------------------*/
/* NiBufGetPtr                                            */
/*                                                        */
/* Gebe den Userptr zurueck                               */
/*                                                        */
/* IN      buf           Puffer fuer den stack            */
/*                                                        */
/* RETURN  userpointer                                    */
/*                                                        */
/*--------------------------------------------------------*/

void * API_DF NiBufGetPtr      (const NIBUFFER  *buf);


/*--------------------------------------------------------*/
/* NiBufPush                                              */
/*                                                        */
/* Baue einen Stack aus NI-Puffern                        */
/*                                                        */
/* IN/OUT  stack         stack-pointer                    */
/*                                                        */
/* IN      buf           Puffer fuer den stack            */
/*                                                        */
/*--------------------------------------------------------*/
/*                                                        */
/* NiBufPop                                               */
/*                                                        */
/* Hole einen Puffer vom stack                            */
/*                                                        */
/* IN/OUT  stack         stack-pointer                    */
/*                                                        */
/* RETURN  NULL          Stack leer                       */
/*         sonst         Puffer                           */
/*                                                        */
/*--------------------------------------------------------*/

void      API_DF NiBufPush     (NIBUFFER **stack,
                                NIBUFFER *buf);
NIBUFFER *API_DF NiBufPop      (NIBUFFER **stack);

/*-NiSnc--------------------------------------------NiBuf-*/

/*--------------------------------------------------------------------*/
/* NiSncAclStr ()                                                     */
/*                                                                    */
/* Converts a binary ACL key into a printable name                    */
/*                                                                    */
/* INPUT   pAcl            ACL key                                    */
/*         pBuffer         buffer to save printable name              */
/*         bufLen          length of buffer in characters             */
/*                                                                    */
/* OUTPUT  pBuffer         printable name of ACL key (NULL terminated)*/
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        pAcl or pBuffer is NULL                    */
/*         NIETOO_SMALL    buffer to small to save printable name     */
/*                         (smaller than SNC_NAME_MAXLEN)             */
/*         NIESNC_FAILURE  failure in convertion; error info is set   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiSncAclStr  (const NISNC_ACLKEY * pAcl,
                               SAP_UC             * pBuffer,
                               SAP_UINT             bufLen);

/*--------------------------------------------------------*/
/* NiSncGetPeer                                           */
/*                                                        */
/*                                                        */
/*                                                        */
/* INPUT   hdl           Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*                                                        */
/* OUTPUT  peer          SNC-Account                      */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIETIMEOUT            Der Handle ist noch nicht lese-  */
/*                       oder schreibbereit               */
/* NIESNC_FAILURE        Verbindung ist nicht             */
/*                       SNC-gesichert                    */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiSncGetPeer  (NI_HDL        hdl,
                                NISNC_ACLKEY *peer);

/*--------------------------------------------------------*/
/* NiSncRequest                                           */
/*                                                        */
/* Fordere eine SNC-Verschluesselung zum Client mit       */
/* SNC-Name peer:                                         */
/* Hiermit kann ein Server Verschluesselung nach dem      */
/* Accept einer normalen Verbindung anfordern.            */
/* SNC-Fehler werden aber erst waehrend der normalen      */
/* Communication gemeldet                                 */
/*                                                        */
/* INPUT   hdl          NI_HDL                            */
/*         timeout      timeout                           */
/*         peer         Client-SNC-Name                   */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiSncRequest (NI_HDL        hdl,
                               SAP_INT       timeout,
                               const NISNC_ACLKEY *peer);

/*--------------------------------------------------------*/
/* NiSncNameToAcl                                         */
/*                                                        */
/*                                                        */
/*                                                        */
/* INPUT   handle        Handle fuer Verbindungskanal     */
/*                       zum Partner                      */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               alles klar                       */
/* NIECONN_BROKEN        Verbindung zum Partner abgebr.   */
/* NIETIMEOUT            Zeitlimit ueberschritten         */
/* NIEINVAL              Puffer oder handle unguelt.      */
/* NIEINTERN             interner Fehler (Err-Trace)      */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiSncNameToAcl (const SAP_UC *name,
                                 NISNC_ACLKEY *acl);

/*--------------------------------------------------------*/
/* NiSncAclCmp                                            */
/*                                                        */
/*                                                        */
/*                                                        */
/* INPUT   acl1, acl2    SNC-Accounts                     */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* TRUE                  SNC-Namen sind gleich            */
/* FALSE                 sonst                            */
/*                                                        */
/*--------------------------------------------------------*/

SAP_BOOL  API_DF NiSncAclCmp   (const NISNC_ACLKEY *acl1,
                                const NISNC_ACLKEY *acl2);


/*--------------------------------------------------------*/
/* NiGetMaxSockets ( void )                               */
/*                                                        */
/* return the maximal number of sockets                   */
/*                                                        */
/*                                                        */
/*                                                        */
/*--------------------------------------------------------*/

int NiGetMaxSockets ( void );

/*--------------------------------------------------------*/
/* Client and server handle for wakeup mechanism.         */
/*--------------------------------------------------------*/

struct NIWU_SERVER;
struct NIWU_CLIENT;

typedef struct NIWU_SERVER * NIWU_S_HDL;
typedef struct NIWU_CLIENT * NIWU_C_HDL;

/*--------------------------------------------------------*/
/* NiWakeupOpen ()                                        */
/*                                                        */
/* Create wakeup server. The handle sould be finally      */
/* closed by the NiWakeupClose function to release the    */
/* recources.                                             */
/*                                                        */
/* INPUT   handle        must be initialized by NULL      */
/*                                                        */
/* OUTPUT  handle        server handle                    */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               success                          */
/* NIENOFREEPORT         no free port within range        */
/* NIEINVAL              parameter invalid                */
/* NIEINTERN             internal error (out of virt. mem)*/
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiWakeupOpen    (NIWU_S_HDL * handle);

/*--------------------------------------------------------*/
/* NiWakeupClose ()                                       */
/*                                                        */
/* Close wakeup server                                    */
/*                                                        */
/* INPUT   handle        server handle                    */
/*                                                        */
/* OUTPUT  handle        NULL handle                      */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               success                          */
/* NIEINVAL              parameter invalid                */
/* NIEINTERN             internal error (NiCloseHandle)   */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiWakeupClose   (NIWU_S_HDL * handle);

/*--------------------------------------------------------*/
/* NiWakeupGetNiHdl ()                                    */
/*                                                        */
/* Get ni listen handle                                   */
/*                                                        */
/* INPUT   handle        server handle                    */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* NiHandle              success                          */
/* NI_INVALID_HDL        parameter invalid                */
/*                                                        */
/*--------------------------------------------------------*/

NI_HDL    API_DF NiWakeupGetNiHdl(NIWU_S_HDL   handle);

/*--------------------------------------------------------*/
/* NiWakeupGetCntNo ()                                    */
/*                                                        */
/* Get number of attached clients                         */
/*                                                        */
/* INPUT   handle        server handle                    */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* referenc count        success                          */
/* -1                    parameter invalid                */
/*                                                        */
/*--------------------------------------------------------*/

SAP_INT   API_DF NiWakeupGetCntNo(NIWU_S_HDL   handle);

/*--------------------------------------------------------*/
/* NiWakeupCollect ()                                     */
/*                                                        */
/* Receive wakeup request to clean server handle buffer   */
/*                                                        */
/* INPUT   handle        server handle                    */
/*                                                        */
/* OUTPUT  wuReqNo       number of received requests      */
/*                       (or NULL)                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               success                          */
/* NIETOO_BIG            received datagram isn't a wakeup */
/*                       request                          */
/* NIETIMEOUT            no datagram received             */
/* NIEINVAL              parameter invalid                */
/* NIEINTERN             internal error                   */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiWakeupCollect (NIWU_S_HDL   handle,
                                  SAP_UINT   * wuReqNo);
SAPRETURN API_DF NiWakeupCollect2 (NIWU_S_HDL   handle,
                                   SAP_RAW    * pBuffer,
                                   SAP_UINT     bufferLen,
                                   SAP_UINT   * pRead);

/*--------------------------------------------------------*/
/* NiWakeupAttach ()                                      */
/*                                                        */
/* Attach client handle to server. The handle sould be    */
/* finally detached by the NiWakeupDetach function to     */
/* release the resources.                                 */
/*                                                        */
/* INPUT   serverHandle  server handle                    */
/* INPUT   clientHandle  must be initialized by NULL      */
/*                                                        */
/* OUTPUT  handle        client handle                    */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               success                          */
/* NIECONN_REFUSED       build up connection failed       */
/* NIETIMEOUT            timeout occurred                 */
/* NIEINVAL              parameter invalid                */
/* NIEINTERN             internal error (out of virt. mem)*/
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiWakeupAttach  (NIWU_S_HDL   serverHandle,
                                  NIWU_C_HDL * clientHandle);

/*--------------------------------------------------------*/
/* NiWakeupDetach ()                                      */
/*                                                        */
/* Detach client handle from server and release resources */
/*                                                        */
/* INPUT   handle        client handle                    */
/*                                                        */
/* OUTPUT  handle        NULL                             */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               success                          */
/* NIEINVAL              parameter invalid                */
/* NIEINTERN             internal error (NiCloseHandle)   */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiWakeupDetach  (NIWU_C_HDL * handle);

/*--------------------------------------------------------*/
/* NiWakeupExec ()                                        */
/*                                                        */
/* Send wakeup request to attached server                 */
/*                                                        */
/* INPUT   handle        client handle                    */
/*                                                        */
/* RETURN                                                 */
/*                                                        */
/* SAP_O_K               success                          */
/* NIECONN_BROKEN        server closed                    */
/* NIECONN_REFUSED       build up connection failed       */
/* NIEINVAL              hdl invalid                      */
/* NIEINTERN             internal error                   */
/*                                                        */
/*--------------------------------------------------------*/

SAPRETURN API_DF NiWakeupExec    (NIWU_C_HDL   handle);
SAPRETURN API_DF NiWakeupExec2   (NIWU_C_HDL      handle,
                                  const SAP_RAW * pData,
                                  SAP_UINT        dataLen);


/*--------------------------------------------------------------------*/
/* NiGet<level>_<option> ()                                           */
/*                                                                    */
/* Get socket option                                                  */
/*                                                                    */
/* INIT    NI should be already initialized by previous calls         */
/*                                                                    */
/* INPUT   handle        handle                                       */
/*                                                                    */
/* OUTPUT  value         option value (see below)                     */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               success                                      */
/* NIEINVAL              handle invalid or value = NULL               */
/* NIEINTERN             internal error (system call)                 */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*                                                                    */
/* SOCKET LEVEL OPTIONS:                                              */
/*   ReuseAddr:                                                       */
/*     Allow the socket to be bound to an address that is already in  */
/*     use                                                            */
/*         value         allow reuse (TRUE / FALSE)                   */
/*                                                                    */
/*   KeepAlive: (option only supported by stream sockets)             */
/*     Send keepalives                                                */
/*         value         send keepalive probe (TRUE / FALSE)          */
/*                                                                    */
/*   Linger: (option only supported by stream sockets)                */
/*     Linger on close if unsent data is present.                     */
/*         onOff         option on / off (TRUE / FALSE)               */
/*                       if FALSE: close immediately                  */
/*         timout        expire time (in clock ticks / seconds;       */
/*                       platform dependent)                          */
/*                                                                    */
/*   SndBuf: (Linux will double the buffer size)                      */
/*   RcvBuf:                                                          */
/*     Specifies the total per-socket buffer space reserved for       */
/*     sends / receives                                               */
/*         value         size in bytes                                */
/*                                                                    */
/* IP LEVEL: (options only supported by datagram sockets)             */
/*   MulticastIF:                                                     */
/*     Address of the outgoing multicast-capable interface            */
/*         addr          address                                      */
/*                                                                    */
/*   MulticastTTL:                                                    */
/*     Time-to-live for a multicast packet                            */
/*         hops          number of surviving hops                     */
/*                                                                    */
/* TCP LEVEL: (options only supported by TCP sockets)                 */
/*   NoDelay:                                                         */
/*     Disables the Nagle algorithm for send coalescing               */
/*         value         disable algorithm (TRUE / FALSE)             */
/*                                                                    */
/*                                                                    */
/* SOCKET DEFAULT VALUES:                                             */
/*   SO_REUSEADDR        1 (TRUE)                                     */
/*   SO_KEEPALIVE        0 (FALSE)                                    */
/*   SO_LINGER           { 0 (FALSE), 0 }                             */
/*   SO_SNDBUF           depending on platform and type               */
/*                       e.g. Linux TCP 16384                         */
/*                            Linux UDP 65535                         */
/*                            AIX   UDP  9216                         */
/*   SO_RCVBUF           0                                            */
/*   IP_MULTICAST_IF     0.0.0.0                                      */
/*   IP_MULTICAST_TTL    1                                            */
/*   TCP_NODELAY         1 (TRUE)                                     */
/*                                                                    */
/*--------------------------------------------------------------------*/

/* SO */
SAPRETURN API_DF NiGetSO_ReuseAddr   (NI_HDL       handle,
                                      SAP_BOOL   * value);
SAPRETURN API_DF NiGetSO_KeepAlive   (NI_HDL       handle,
                                      SAP_BOOL   * value);
SAPRETURN API_DF NiGetSO_Linger      (NI_HDL       handle,
                                      SAP_BOOL   * onOff,
                                      SAP_INT    * timeout);
SAPRETURN API_DF NiGetSO_SndBuf      (NI_HDL       handle,
                                      SAP_INT    * value);
SAPRETURN API_DF NiGetSO_RcvBuf      (NI_HDL       handle,
                                      SAP_INT    * value);
/* IP */
SAPRETURN API_DF NiGetIP_MulticastIF (NI_HDL       handle,
                                      NI_NODEADDR * addr);
SAPRETURN API_DF NiGetIP_MulticastTTL(NI_HDL       handle,
                                      SAP_INT    * hops);
/* TCP */
SAPRETURN API_DF NiGetTCP_NoDelay    (NI_HDL       handle,
                                      SAP_BOOL   * value);


/*--------------------------------------------------------------------*/
/* NiSet<level>_<option> ()                                           */
/*                                                                    */
/* Set socket option                                                  */
/*                                                                    */
/* INIT    NI should be already initialized by previous calls         */
/*                                                                    */
/* INPUT   handle        handle                                       */
/*         value         option value (cf. get function)              */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               success                                      */
/* NIEINVAL              hdl invalid                                  */
/* NIEINTERN             internal error (system call)                 */
/*                                                                    */
/*--------------------------------------------------------------------*/

/* SO */
SAPRETURN API_DF NiSetSO_ReuseAddr   (NI_HDL       handle,
                                      SAP_BOOL     value);
SAPRETURN API_DF NiSetSO_KeepAlive   (NI_HDL       handle,
                                      SAP_BOOL     value);
SAPRETURN API_DF NiSetSO_Linger      (NI_HDL       handle,
                                      SAP_BOOL     onOff,
                                      SAP_INT      timeout);
SAPRETURN API_DF NiSetSO_SndBuf      (NI_HDL       handle,
                                      SAP_INT      value);
SAPRETURN API_DF NiSetSO_RcvBuf      (NI_HDL       handle,
                                      SAP_INT      value);
/* IP */
SAPRETURN API_DF NiSetIP_MulticastIF (NI_HDL       handle,
                                      NI_NODEADDR  addr);
SAPRETURN API_DF NiSetIP_MulticastTTL(NI_HDL       handle,
                                      SAP_INT      hops);
/* TCP */
SAPRETURN API_DF NiSetTCP_NoDelay    (NI_HDL       handle,
                                      SAP_BOOL     value);


/* ################################################################## */
/* # enable / disable trace for basic NI calls and NI handles       # */
/* ################################################################## */

/*--------------------------------------------------------------------*/
/* NiEnableTrace  ()                                                  */
/* NiDisableTrace ()                                                  */
/*                                                                    */
/* Enable or disable a specific NI trace                              */
/*                                                                    */
/* INPUT   traceMask       bit mask of traces to enable / disable     */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*                                                                    */
/* INFO  - trace mask bits:                                           */
/*                                                                    */
/* HOST_LOOKUP             hostname lookup                            */
/* HOST_REV_LOOKUP         hostname reverse lookup (address lookup)   */
/* SERV_LOOKUP             service name lookup                        */
/* SERV_REV_LOOKUP         service name reverse lookup (port lookup)  */
/*                                                                    */
/* CONN_RST_WRITE          connection abort, reset or timeout in send */
/* CONN_RST_READ           connection abort, reset or timeout in      */
/*                         receive                                    */
/* CONN_INV_SHUTDOWN       socket not connected (invalid connection)  */
/*                                                                    */
/*--------------------------------------------------------------------*/

#define NI_LOG_HOST_LOOKUP         (((SAP_ULLONG)1) << 00)     /* on  */
#define NI_LOG_HOST_REV_LOOKUP     (((SAP_ULLONG)1) << 10)     /* off */
#define NI_LOG_SERV_LOOKUP         (((SAP_ULLONG)1) << 20)     /* on  */
#define NI_LOG_SERV_REV_LOOKUP     (((SAP_ULLONG)1) << 30)     /* off */

#define NI_TRC_CONN_RST_WRITE      (((SAP_ULLONG)1) << 40)     /* on  */
#define NI_TRC_CONN_RST_READ       (((SAP_ULLONG)1) << 50)     /* on  */
#define NI_TRC_CONN_INV_SHUTDOWN   (((SAP_ULLONG)1) << 60)     /* on  */


SAPRETURN API_DF NiEnableTrace  (SAP_ULLONG   traceMask);
SAPRETURN API_DF NiDisableTrace (SAP_ULLONG   traceMask);


/*--------------------------------------------------------------------*/
/* NiHdlSetTrace ()                                                   */
/* NiHdlGetTrace ()                                                   */
/*                                                                    */
/* Enable or disable NI handle trace; ascertain NI handle trace       */
/*                                                                    */
/* INPUT   handle          handle                                     */
/*         mode            enable (TRUE) or disable (FASLE) NI handle */
/*                         trace                                      */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        invalid handle (errInfo set)               */
/*                                                                    */
/*         TRUE            trace enabled                              */
/*         FALSE           trace disbaled or handle invalid           */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiHdlSetTrace (NI_HDL     handle,
                                SAP_BOOL   mode);
SAP_BOOL  API_DF NiHdlGetTrace (NI_HDL     handle);


/*--------------------------------------------------------------------*/
/* NiSetTraceAll ()                                                   */
/* NiClearTraceAll ()                                                 */
/*                                                                    */
/* Enable or disable NI handle trace for all handles                  */
/*                                                                    */
/*--------------------------------------------------------------------*/

void API_DF NiSetTraceAll   (void);
void API_DF NiClearTraceAll (void);


/*--------------------------------------------------------------------*/
/* NiHdlSetTraceForPeer   ()                                          */
/* NiHdlClearTraceForPeer ()                                          */
/*                                                                    */
/* Enable / disable trace for incoming connects from a specified      */
/* address or address mask (cf. NiStrToAddrMask)                      */
/*                                                                    */
/* INPUT   pPeerAddr       peer address of incoming connect           */
/*         pPeerAddrMask   address mask in addition to pPeerAddr;     */
/*                         NULL for non mask                          */
/*         pPeerAddrList   array of NI_NODEADDR structures as out     */
/*                         parameter                                  */
/*         pPeerAddrMaskList   array of NI_NODEADDR structures as out */
/*                         parameter                                  */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        invalid parameter (errInfo set)            */
/*         NIETOO_SMALL    no more slots avaliable to save the        */
/*                         address pair (errInfo set);                */
/*                         array too small, addresses filled up to    */
/*                         listSize (*pFound > listSize !!!)          */
/*         NIEINTERN       internal error (errInfo set)               */
/*                                                                    */
/* INFO  - to clear the hole list, use NiHdlClearTraceForPeer with    */
/*         pPeerAddrMask = '::' / NI_ADDR_ANY_I6_INIT                 */
/*                                                                    */
/*--------------------------------------------------------------------*/

#define NI_TRC_FOR_PEER_SLOTS   5   /* max entries in list */

SAPRETURN API_DF NiSetTraceForPeer     (const NI_NODEADDR * pPeerAddr,
                                        const NI_NODEADDR * pPeerAddrMask);
SAPRETURN API_DF NiClearTraceForPeer   (const NI_NODEADDR * pPeerAddr,
                                        const NI_NODEADDR * pPeerAddrMask);
SAPRETURN API_DF NiGetTraceForPeerList (NI_NODEADDR       * pPeerAddrList,
                                        NI_NODEADDR       * pPeerAddrMaskList,
                                        SAP_UINT            listSize,
                                        SAP_UINT          * pFound);


/* ################################################################## */

/*--------------------------------------------------------------------*/
/* NiGetPeerVersion ()                                                */
/*                                                                    */
/* Get NI layer version of peer (not interacted by SAProuters)        */
/*                                                                    */
/* INPUT   handle          connected handle                           */
/*         timeout         timeout in ms or NI_BLOCK                  */
/*         pPeerVersion    SAP_UINT as out parameter                  */
/*                                                                    */
/* OUTPUT  pPeerVersion    peer NI layer version                      */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIETIMEOUT      timed out in read (get peer version info   */
/*                         by a own read call)                        */
/*         NIEINVAL        invalid parameter passed, e.g. handle;     */
/*                         dataLen is not equal with the length of    */
/*                         the pending data to write; unexpected      */
/*                         opCode processed (err-info set)            */
/*         NIECONN_BROKEN  connection to partner broken (err-info set)*/
/*         NIECONN_REFUSED connection failed (errInfo set)            */
/*         NIEINTERN       internal error; failed to alloc memory;    */
/*                         buffer too small; ... (errInfo set)        */
/*                                                                    */
/* INFO  - if peer version could not be read due to a small timeout   */
/*         parameter (NIETIMEOUT returned), use e.g. NiRead to get    */
/*         peer version data block (1 byte packet with peer version   */
/*         as data (SAP_RAW value))                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiGetPeerVersion (NI_HDL     handle,
                                   SAP_INT    timeout,
                                   SAP_UINT * pPeerVersion);

/*--------------------------------------------------------------------*/

/* ################################################################## */
/* # SI functions                                                   # */
/* ################################################################## */

#if defined(SIXX_H)

/*--------------------------------------------------------------------*/
/* NiGetSiSocket ()                                                   */
/*                                                                    */
/* Return SI socket structure of NI handle                            */
/*                                                                    */
/* INPUT   handle          NI handle                                  */
/*         pSocket         SI_SOCK structure as out parameter         */
/*                                                                    */
/* OUTPUT  pSocket         SI socket structure                        */
/*                                                                    */
/* RETURN  SAP_O_K         successful                                 */
/*         NIEINVAL        invalid handle or not connection handle,   */
/*                         e.g. listen handle (errInfo set)           */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF NiGetSiSocket (NI_HDL    handle,
                                SI_SOCK * pSocket);

#endif

/*--------------------------------------------------------------------*/

/* ################################################################## */
/* # dated interface functions and defines                          # */
/* ################################################################## */

#ifndef NI_WITHOUT_OLD_FUNC_DECLARATION

#define NI_IADR_LN                  NI_NODEADDR_LEN
#define NI_HOSTADR                  NI_NODEADDR
#define NI_HOSTADR_SIZ              NI_NODEADDR_LEN

#define ni_hostadr_any_init         NI_ADDR_ANY_INIT
#define ni_hostadr_localhost_init   NI_ADDR_LOCAL_INIT

#define NI_HOSTADR_ANY              NI_ADDR_ANY_I4
#define NI_HOSTADR_LOCALHOST        NI_ADDR_LOCAL_I4

#define NI_HOSTADDR_STR_LEN         NI_NODEADDR_STR_LEN

/*--------------------------------------------------------------------*/

#define NI_IS_ANY_ADR( /* NI_NODEADDR */ nodeAddr )                    \
    NI_IS_ADDR_ANY( nodeAddr )

#define NI_IS_LOCALHOST_ADR( /* NI_NODEADDR */ nodeAddr )              \
    NI_IS_ADDR_LOCAL( nodeAddr )

#define NI_IS_IPv4ADDR( /* NI_NODEADDR */ nodeAddr )                   \
    NI_IS_IPv4_ADDR( nodeAddr )

#define NI_ADR_EQUAL( /* NI_NODEADDR */ nodeAddrA,                     \
                      /* NI_NODEADDR */ nodeAddrB )                    \
    NI_ADDR_EQUAL( nodeAddrA, nodeAddrB )

#define NI_ADRP_EQUAL( /* NI_NODEADDR * */ pNodeAddrA,                 \
                       /* NI_NODEADDR * */ pNodeAddrB )                \
    NI_ADDR_PTR_EQUAL( pNodeAddrA, pNodeAddrB )

#define NI_SET_NO_ADR( /* NI_NODEADDR */ nodeAddr )                    \
    NI_SET_NO_ADDR( nodeAddr )

#define NI_IS_NO_ADR( /* NI_NODEADDR */ nodeAddr )                     \
    (NI_IS_VALID_ADDR( nodeAddr ) == FALSE)

#define NI_IS_VALID_ADR( /* NI_NODEADDR */ nodeAddr )                  \
    NI_IS_VALID_ADDR( nodeAddr )

/*--------------------------------------------------------------------*/

#define                                                                \
/* SAPRETURN */ NiBufInit( )                                           \
    NiInit()

#define                                                                \
/* SAPRETURN */ NiBufReInit( )                                         \
    SAP_O_K

#define                                                                \
/* SAPRETURN */ NiBufInit2( /* SAP_INT */ maxHandles )                 \
    NiInit2(maxHandles)

/*--------------------------------------------------------------------*/

#define                                                                \
/* SAPRETURN */ NiListen( /* const SAP_UC * */ pServName,              \
                          /* NI_HDL       * */ pHandle )               \
    NiServerHandle(pServName, pHandle)

#define                                                                \
/* SAPRETURN */ Ni2Listen( /* SAP_USHORT * */ pServNo,                 \
                           /* NI_HDL     * */ pHandle )                \
    NiServerHandleForPort(pServNo, pHandle)

#define                                                                \
/* SAPRETURN */ Ni3Listen( /* SAP_USHORT        * */ pServNo,          \
                           /* const NI_NODEADDR * */ pLocalAddr,       \
                           /* NI_HDL            * */ pHandle )         \
    NiServerHandleForAddr(pLocalAddr, pServNo, pHandle)

#define                                                                \
/* SAPRETURN */ NiBufListen( /* const SAP_UC * */ pServName,           \
                             /* NI_HDL       * */ pHandle )            \
    NiBufServerHandle(pServName, pHandle)

#define                                                                \
/* SAPRETURN */ NiBuf2Listen( /* SAP_USHORT * */ pServNo,              \
                              /* NI_HDL     * */ pHandle )             \
    NiBufServerHandleForPort(pServNo, pHandle)

#define                                                                \
/* SAPRETURN */ NiBuf3Listen( /* SAP_USHORT        * */ pServNo,       \
                              /* const NI_NODEADDR * */ pLocalAddr,    \
                              /* NI_HDL            * */ pHandle )      \
    NiBufServerHandleForAddr(pLocalAddr, pServNo, pHandle)

/*--------------------------------------------------------------------*/

#define                                                                \
/* SAPRETURN */ NiConnect( /* const SAP_UC * */ pHostName,             \
                           /* const SAP_UC * */ pServName,             \
                           /* SAP_INT        */ timeout,               \
                           /* NI_HDL       * */ pHandle )              \
    NiHandle(pHostName, pServName, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ Ni2Connect( /* const NI_NODEADDR * */ pNodeAddr,       \
                            /* SAP_USHORT          */ servNo,          \
                            /* SAP_INT             */ timeout,         \
                            /* NI_HDL            * */ pHandle )        \
    NiHandleForAddr(pNodeAddr, servNo, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiRawConnect( /* const SAP_UC * */ pHostName,          \
                              /* const SAP_UC * */ pServName,          \
                              /* SAP_INT        */ timeout,            \
                              /* NI_HDL       * */ pHandle )           \
    NiRawHandle(pHostName, pServName, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiRaw2Connect( /* const NI_NODEADDR * */ pNodeAddr,    \
                               /* SAP_USHORT          */ servNo,       \
                               /* SAP_INT             */ timeout,      \
                               /* NI_HDL            * */ pHandle )     \
    NiRawHandleForAddr(pNodeAddr, servNo, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiBufConnect( /* const SAP_UC * */ pHostName,          \
                              /* const SAP_UC * */ pServName,          \
                              /* SAP_INT        */ timeout,            \
                              /* NI_HDL       * */ pHandle )           \
    NiBufHandle(pHostName, pServName, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiBuf2Connect( /* const NI_NDOEADDR * */ pNodeAddr,    \
                               /* SAP_USHORT          */ servNo,       \
                               /* SAP_INT             */ timeout,      \
                               /* NI_HDL            * */ pHandle )     \
    NiBufHandleForAddr(pNodeAddr, servNo, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiBufRawConnect( /* const SAP_UC * */ pHostName,       \
                                 /* const SAP_UC * */ pServName,       \
                                 /* SAP_INT        */ timeout,         \
                                 /* NI_HDL       * */ pHandle )        \
    NiBufRawHandle(pHostName, pServName, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiBufRaw2Connect( /* const NI_NODEADDR * */ pNodeAddr, \
                                  /* SAP_USHORT          */ servNo,    \
                                  /* SAP_INT             */ timeout,   \
                                  /* NI_HDL            * */ pHandle )  \
    NiBufRawHandleForAddr(pNodeAddr, servNo, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiSncConnect( /* const SAP_UC * */ pHostName,          \
                              /* const SAP_UC * */ pServName,          \
                              /* const SAP_UC * */ pSncTargetName,     \
                              /* SAP_INT        */ timeout,            \
                              /* NI_HDL       * */ pHandle )           \
    NiSncHandle(pHostName, pServName, pSncTargetName, timeout, pHandle)

#define                                                                \
/* SAPRETURN */ NiSnc2Connect( /* const NI_NODEADDR  * */ pNodeAddr,   \
                               /* SAP_USHORT           */ servNo,      \
                               /* const NISNC_ACLKEY * */ pSncTargetKey,\
                               /* SAP_INT              */ timeout,     \
                               /* NI_HDL             * */ pHandle )    \
    NiSncHandleForAddr(pNodeAddr, servNo, pSncTargetKey, timeout, pHandle)

/*--------------------------------------------------------------------*/

#define                                                                \
/* SAPRETURN */ NiAccept( /* NI_HDL        */ inHandle,                \
                          /* SAP_INT       */ timeout,                 \
                          /* NI_HDL      * */ pOutHandle,              \
                          /* NI_NODEADDR * */ pPeerAddr )              \
    NiAcceptEx(inHandle, timeout, pOutHandle, pPeerAddr, NULL)

/*--------------------------------------------------------------------*/

#define                                                                \
/* SAPRETURN */ NiGetHostAddrList( /* NI_NDOEADDR * */ pNodeAddrList,  \
                                   /* SAP_INT       */ listSize,       \
                                   /* SAP_INT     * */ pFound )        \
    NiGetNodeAddrList(pNodeAddrList, NI_PROTO_FAM_UNSPEC,              \
                      (SAP_UINT)listSize, (SAP_UINT *)pFound)

/*--------------------------------------------------------------------*/

#define                                                                \
/* SAP_UC  * */ NiAdrToStr( /* const NI_NODEADDR * */ pNodeAddr )      \
    (SAP_UC *)NiAddrToStrStatic(pNodeAddr, TRUE)

#define                                                                \
/* SAPRETURN */ NiAdrToStr2( /* const NI_NODEADDR * */ pNodeAddr,      \
                             /* SAP_UC            * */ pNodeAddrStr,   \
                             /* SAP_UINT            */ bufferLen )     \
    NiAddrToStr(pNodeAddr, pNodeAddrStr, bufferLen, TRUE)

#define                                                                \
/* SAP_UC  * */ NiAdrToStr3( /* const NI_NODEADDR * */ pNodeAddr,      \
                             /* SAP_UC            * */ pNodeAddrStr,   \
                             /* SAP_UINT            */ bufferLen )     \
    NiAddrToStr2(pNodeAddr, pNodeAddrStr, bufferLen, TRUE)

/*--------------------------------------------------------------------*/

#define NI_DT_INT                   NI_DT_UINT

#define NIHP_MODE                   NIBUFHP_MODE
#define NIHP_BUFSTAT                NIBUFHP_BUFSTAT
#define NIHP_CHECK_QUE              NIBUFHP_CHECK_QUE
#define NIHP_ALLOC_FUNC             NIBUFHP_ALLOC_FUNC
#define NIHP_MAXQUEUE               NIBUFHP_MAXQUEUE
#define NIHP_WARNQUEUE              NIBUFHP_WARNQUEUE
#define NIHP_MAXMLEN                NIBUFHP_MAXMLEN

/*--------------------------------------------------------------------*/

#define                                                                \
/* SAPRETURN */ NiSendHandle( /* NI_HDL  */ handle,                    \
                              /* NI_HDL  */ sendHandle,                \
                              /* SAP_INT */ timeout )                  \
    NiBufSendHandle(handle, sendHandle, timeout, FALSE)

#define                                                                \
/* SAPRETURN */ NiSendHandleEx( /* NI_HDL   */ handle,                 \
                                /* NI_HDL   */ sendHandle,             \
                                /* SAP_INT  */ timeout,                \
                                /* SAP_BOOL */ saveModes )             \
    NiBufSendHandle(handle, sendHandle, timeout, saveModes)

#define                                                                \
/* SAP_BOOL  */ NiHdlIsUDSock( /* NI_HDL */ handle )                   \
    ((NiGetHdlProto(handle) & NI_PROTO_FAM_LOCAL) != 0)

#endif /* NI_WITHOUT_OLD_FUNC_DECLARATION */

/*--------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* NIXX_H */

/* ################################################################## */

/*
 * Local variables:
 * c-basic-offset: 4
 * compile-command: "mapro krn/ni lib"
 * End:
 */

/* ################################################################## */
