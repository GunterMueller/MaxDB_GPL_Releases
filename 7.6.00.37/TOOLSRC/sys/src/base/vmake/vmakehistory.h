/*      vmakehistory.h         

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

*/
#ifndef _VMAKEHISTORY_H
#define _VMAKEHISTORY_H 

#define VMAKE_VERSION_NUMBER "10.05.10"
#define VMAKE_VERSION "@(#)vmake "VMAKE_VERSION_NUMBER" 11-05-2006" 

/* toDO:
- project idea
- export LAYER of all - OK
- Dateiliste für alle zu erstellenden Dateien (zum Abgleich mit Distribution - Mapfiles!) 
- novariant(<variant>) zum Abblocken von einzelnen Varianten
- execute-in der COM-Description (anstatt Compile)
- MD5-Summen
- Kein Kopieren der Objektdateien (unter UNIX-Hardlink)
- HTM-Ausgabe
- Globals.com in richtiger Reihenfolge
- Ausgabe von Abhängigkeiten (was ist alles von abc.h abhängig)
- vmake-Aufruf zum Check, ob ein PIDFile zu sperren ist 
*.rc unpackinc=mfinc(-rc) unpacksrc=mf(-rc) obj=*.res compiler=comprc(-rc) in Globals.com
*/

/* 10.??.??       - to do: double or more reading of empty com descriptions */
/* 10.05.10       - tmp. more size for description line (4000->12000)       */
/* 10.05.09       - bugfix for uncond option of lib targets (PTS 1141212)   */
/* 10.05.08       - warning protocol (CTS 1141066)                          */
/*                - no percent info in normal protocol (only console)       */
/*                bugfix                                                    */
/*                - no_static_lib (PTS 1140977)                             */
/*                - crash in file list for default layer                    */
/* 10.05.07       - bugfix for default layer in com descriptions            */
/* 10.05.06       - temp switch off of check options (only write)           */
/* 10.05.05       - support exec and distrib for filelists  (PTS 1140827)   */
/*                - rebuild modules and headers if option were changed      */
/*                  in com files                                            */
/*                - new option: required vmake version                      */
/* 10.05.03       - filter definition for directory collecting              */
/* 10.05.02       - collect a directory without recursion                   */
/* 10.05.01       - eliminate debug output                                  */
/* 10.05.00       - recursive collecting                                    */
/*                - allow to handle files with "-" in filename              */
/*                - protstyle=...                                           */
/*                - option text                                             */
/*                - use tempnam onw windows now                             */
/* 10.04.06       - option comfile=<filepath>                               */
/* 10.04.05       - option comfile=<filepath> started                       */
/*                - bugfix for --snapshot                                   */  
/* 10.04.04       - other handling for recursive copying of directories     */
/*                - first changes for comfile=... option                    */
/* 10.04.03       - silent copy of filelist                                 */
/*                - &if exists("<path>")                                    */ 
/*                - serrno() output in error protocol                       */
/* 10.04.02       - right output for errors of filelists                    */
/* 10.04.00       - recursive copy of source directories                    */
/*                - ? execute                                               */
/* 10.03.13       - bugfix for error output (vcopy)                         */
/* 10.03.12       - allow virtual files for headers too                     */
/* 10.03.11       - more DESCRIPTIONSIZE  (4000)                            */
/* 10.03.10       - on_success option                                       */
/* 10.03.09       - REPORT messages (BEGIN, END, PERCENT)                   */
/* 10.03.08       - VMAKE_NO_DIRCACHE for global handling (remote make)     */
/* 10.03.07       - missing source: read again without directory caching    */
/* 10.03.06       - Globals descriptions in Globals directory               */
/*                - no_dep_remake messages only in extended protocol        */    
/* 10.03.05       - more output in error protocol                           */    
/* 10.03.00       - allow to use current directory for default layer        */    
/*                - special event handling for windows to stop vmake        */
/*                - no_dep_remake as new option                             */
/*                - new handling for dep= (+,-,..)                          */
/*                - new substitution for internal variables                 */
/*                - allow global variant handling for all                   */
/*                - silent make if VMAKE_SILENT_MODE is set (some goodies   */ 
/*                - statistics at the end of makes                          */
/* 10.02.19       bugfix - don't use defdault layer for descriptions        */
/*                - output of modlist: (original sources of "virtual" files)*/ 
/* 10.02.18       bugfix - output: for variants (cut 2 first characters)    */  
/*                - allow variant substitution for "? output:" lines        */
/* 10.02.17       bugfix - for missing ctip for fallback profile generation */
/*                       - right output for com descriptions in src         */
/*                       - demand of modules                                */
/*                - ...:variant(!<variant>,...) -> novariant                */
/*                - use default port if nothing find in etc/services        */ 
/* 10.02.16       - allow variant substitution for <-<file> contructs       */
/* 10.02.15       - bugfix for parentlinkoption                             */
/* 10.02.14       - allow parallel make for genration of modlist (-m)       */
/*                - VMAKE_OPTION 'z' or "-z" for - "stop after first error" */
/* 10.02.13       - allow exec flag in command line without warning         */
/*                bugfix: - test for ctip if use ctip->name (for noversion) */ 
/* 10.02.12       - ? responsible: <resonsible>                             */
/*                - allow overwiting from source renaming (<-<src>)         */
/*                - don't substitute "/" in argv if " " found               */          
/* 10.02.11       - integrate internal vmake make to opensource make        */
/* 10.02.10       bugfix: touch jar files with current timestamp            */
/* 10.02.09       - suppress_uncond (necessary if modules behind inlcudes)  */
/*                - "-dU" force new make of targets only in the first level */
/* 10.02.08       - bugfix: for handling of recursion                       */
/* 10.02.07       - bugfix: recursion for slow includes                     */
/* 10.02.06       - bugfix: handling of output=                             */
/* 10.02.05       - bugfix: variant handling 2                              */
/* 10.02.04       - bugfix: variant handling                                */
/* 10.02.03       - bugfix: output direction                                */
/* 10.02.02       - bugfix: for global variant handling                     */
/* 10.02.01       - bugfix for handling of com descriptions                 */
/* 10.02.00       - allow vmake call without action (-c|-n) while another   */
/*                  vmake is running in the same area                       */
/*                - right handling for Globals.com                          */
/*                - activate new variant handling for sapdb/lc              */   
/*                - "? error: ..." - option                                 */
/*                - comp= -option                                           */
/*                - checkdate                                               */
/*                - "? parentlinkoption:", parentlinkoption                 */
/*                - nocomfile                                               */
/* 10.01.09|10    bugfix: missing version if found in desc directory        */
/* 10.01.08       bugfix: copy exec flag only if not defined before         */
/* 10.01.07       - global char pointer for SUPPRESS_VMAKE_PIDFILE          */
/* 10.01.06       - set SUPPRESS_VMAKE_PIDFILE in set_work_dir (UNIX)       */
/* 10.01.05       - timestamps for descriptions (use checksums later)       */
/* (25.11.02)     - activate PIDFile again                                  */  
/*                   -> set and evaluate SUPPRESS_VMAKE_PIDFILE             */
/* 10.01.04       - create pidfile after creating directories               */
/* 10.01.03       - temp. switch off of lock (xxx.mac call "imf ... ")      */
/* 10.01.02       - allow only one make in one own (lock pidfile)           */
/* (24.11.02)     bugfix:                                                   */
/*                   missing env_OWN in vmakprot                            */
/*                    -> move allocation for pidfile name to createPIDFile  */                                               
/* 10.01.01       - allow variant substitutions for targets                 */
/* (20.11.02)     - search descriptions and sources in defaultlayer         */
/*                - allow relative pathes of targets (with ..)              */
/*                - write PID in pidfile ($WRK/vmake.PID)                   */
/* ------------------------------------------------------------------------ */
/* 10.00.15       bugfix: finish with error line if collecting failed       */
/* 10.00.14       bugfix: right output of description list(-m) in src       */
/* 10.00.13       bugfix: write compilerlines withhout arguments            */
/* 10.00.12       bugfix: right output of description list(-m) in src       */
/* 10.00.11       - allow also full path names for distribution             */
/*                - write RELVER,CORRECTION_LEVEL,BUILD_PRAEFIX,MACH,       */
/*                  OS_SPEC,BIT64 to x-protocol                             */ 
/*                - return also make status from mac's                      */
/*                bugfix: bug in sizeof handling                            */
/* 10.00.10       - uncond is also allowed in com descriptions              */
/*                - read also files with dot at begin (.makedbv)            */
/*                - allow '_' in variant names                              */
/* 10.00.09       - source location with "<-"                               */
/*                - error of collecting if concom error was found            */
/*                - change VMAKEBUILD in vmake.mac                          */
/*                bugfix: global-variant-names in output directory          */
/* 10.00.08       - little bugfix for other_dep_tip                         */
/* 10.00.07       - timestamp in error protocol                             */
/*                - "? vmakeoption : no_static_lib" for shared libs         */
/*                - bugfilx afile in make_dll                               */
/* 10.00.06       - bugfix tip->file.file                                   */
/* 10.00.05       - VMAKE_OPTION u for make in obj directory                */
/* 10.00.04       - bugfix Extra files                                      */ 
/*                - copy Extra files from obj to target dir                 */
/*                - replace "%%" with "%"                                   */  
/*                - bugfix for replacemanet of variants (%)                 */                    
/* 10.00.03       - bugfix for calling a description for more times         */ 
/* 10.00.02       - novariant for ignoring of global variants of caller     */ 
/* 10.00.01       - descriptions in src directory                           */ 
/*                - vmakserv installed as starting automatic                */
/*                - new global variant specification                        */
/*                - recursion between  different files                      */
/*                - allow $(...) for substitution                           */
/*                - Globals.com                                             */
/*                - different output dirs for global variants               */
/*                - variant replacment with %+, %- or %*                    */
/*                - &if variant (<variant>) and variant (<variant>)         */ 
/* ------------------------------------------------------------------------ */
/* 9.9.7.         - make of recursive called modules ( CTS 1112727 )        */
/*                - type=inc                                                */
/* 9.9.6          - + for ignoring return value of commands (CTS 1112287)   */
/*                - define language with option lang= (CTS 1112700)         */
/*                - define output length with environmentvariable           */
/*                   CMD_OUTPUT_LENGTH (CTS 1112496)                        */
/*                - use also &ifvariant and &ifnvariant (CTS 1112287)       */
/*                - change process for making of extra files (CTS 1112702)  */
/*                - insert html header at begin of error protocol           */
/*                bugfix: error protocol                                    */
/* 9.9.5          - create a new error protocol ( CTS 1112006 )             */
/* 9.9.4          - add targetversion in extended protocol (CTS 1111662)    */
/* (18.09.2001)   - new option +c for output collecting informations        */
/*                  and make (CTS 1111663)                                  */
/*                - vmake exit with 1 if there were no errors and anything  */
/*                  was build (for writing of protocol in the database)     */
/*                CTS 1111841:                                              */      
/*                - full path from sys in Modules-protocol                  */
/*                - add description and Extra etc. in Modules-protocol      */
/*                - Modules.m0 => Modules.$BUILDPID.m0                      */
/*                bugfix:                                                   */
/*                - for backslash at end of lines                           */
/*                - correct output of $VMAKE_OPTION and call options        */
/* 9.9.3.5        - bugfix for backslash at end of lines                    */
/* 9.9.3.4        - checkdir check also for dirs before "\" (PTS 1111250)   */
/* (13.08.01)     - vcopy ascci files to tmp dir always (no link)           */
/*                - backslash at the end of lines (CTS 1104984)             */
/* 9.9.3 28.06.01 - change the call option -E and the vmake option E to     */
/*                  debug compiling with optimize (optdebug) (CTS 1110663)  */
/*                bugfix                                                    */
/*                - redirection of object or class files (PTS 1110251)      */
/*                - set exec permissions for copied files (PTS 1110662)     */
/* 9.9.2 22.05.01 - the same timestamp for objects and headers(CTS 1110368) */
/*                  -> touch exdep's also and use it for tip->date          */ 
/*                - nodistrib option for targets and outputs (CTS 1110367)  */
/*                - noversion option in com description (CTS 1110258)       */
/*                - ascii + concom + unpack possible  ( CTS 1110365 )       */ 
/*                - change option separator from ? to //                    */
/*                  (some shells don't like a '?' in arguments )            */ 
/*                - change jp extension for java projects to .jpr           */
/*                bugfix:                                                   */
/*                - different bugs in the dynamic memory allcation          */ 
/*                - no remake of modules and headers in a mac description   */
/*                  that isn't called with -u option directly (PTS 1110170) */
/* 9.9.1 13.03.01 - java project files '.jp'                                */
/*                - beginthread in place of CreateThread                    */
/* 9.9.0 12.03.01 - uniform option separator '?' for qualified and          */
/*                  nonqualified targets on command line                    */
/*                - dynamic memory allocation for target structures         */
/*                - 'sdemand' source demand                                 */
/*                - 'odemand' (object demand) as synonym for 'demand'       */
/*                - treat "$$" as '$' in target names                       */
/*                - bugfix: output redirection with more than one object    */
/*                - remove 'interface' concept                              */
/*                - remove include marker '(i)' in demand/dependency lists  */
/* 9.8.7 27.02.01 bugfix:                                                   */
/*                - for variants under unix also for includes (see 1002016) */
/*                - target directory for several objectfiles (PTS 1109460)  */
/* 9.8.6 05.02.01 - different little changes                                */
/* 9.8.5 28.11.00 - print caller if remdep fails (CTS 1108432)              */
/*                bugfix:                                                   */
/*                - create etc directory if not existent by "imf -F"        */
/*                  (PTS 1108423)                                           */
/*                - modules in distribution (PTS 1108421)                   */
/* 9.8.4 01.11.00 bugfix:                                                   */
/*                - Filelist without commands / copy distrib flag to        */
/*                  a existent target without it (PTS 1108150)              */
/*                - cut last / fom Extra directory if exists (PTS 1108163)  */
/*                - abort vmake if no more space on device or close all     */                
/*                  connections if an other error occurs while fwrite       */
/*                  in fileincopy (PTS 1108168)                             */
/*                - warn if mac description in link description and         */
/*                  ignore it (CTS 1108170)                                 */
/* 9.8.3 16.08.00 - output with * at the end   (CTS 1107534)                */
/*                - "->" with '/' at the end in link descriptions           */
/*                  (PTS 1107469)                                           */
/*                - test inc/dep with full path for directory mapping       */
/*                  (PTS 1107138)                                           */
/*                bugfix:                                                   */
/*                - DESCRIPTIONSIZE 512 -> 3072  (PTS 1107138)              */
/*                - errors of nobind option (PTS 1107143)                   */
/* 9.8.2 12.07.00 - start and finish tag for targets (CTS 1107111)          */
/*                bugfix:                                                   */
/*                - DESCRIPTIONSIZE 512 -> 3072  (PTS 1107138)              */
/*                - errors of nobind option (PTS 1107143)                   */
/* 9.8.1.1        - nodefaultlib + libcmt + oldnames.lib (vmakserv)         */
/* 9.8.1          - target names with the first character _ (PTS 1106608)   */
/*                - same com-definition for different variants of one       */
/*                  module (CTS VARIANT)                                    */
/* 9.8.0.2        bugfix: right name for static libs of dld (PTS 1106573)   */
/* 9.8.0          - obj-extension in langinfo (JAVA - CTS 1104953)          */
/* 9.7.9 08.03.00 - &ifdebug, &ifndebug  (CTS 1105643)                      */
/*                - Option ascii for text-copy (CTS 1105829)                */ 
/*                - different wrk-dirs with WRK_NAME (CTS 1105673)          */
/*                - nobind (CTS 1105891)                                    */
/* 9.7.8          - check $OWN with the 1st entry in vmake_path             */
/*                  (CTS 1105503)                                           */
/*                - replace full path of obj with relative path to current  */
/*                  working directory ( CTS 1105495)                        */
/*                - append at end of descriptions '\n' if necessary         */
/*                  (PTS 1105219)                                           */
/* 9.7.7          -> ascdate modified (Y2K save - localtime )               */
/* 9.7.6          - map filenames with file DirectoryMapping (CTS 1105037)  */
/*                bugfix:												    */
/*                - uninitialized variable in make_dll (PTS 1104836 )       */
/*                - recursive call for active callers ( PTS 1104691/2 )     */
/* 9.7.5          bugfix:                                                   */
/*                - recursive call for active callers ( PTS 1104691 )       */
/* 9.7.4          bugfix:                                                   */
/*                - tip->minpathlvl also copy to other_dep_tip              */
/*                 ( PTS 1104596 / 2 )                                      */
/* 9.7.3          - CMDBUFSIZE = 10240 (PTS 1104090)                        */
/*                - idl-Files not always make (PTS  1104635)                */
/*                - same include at dep= and inc= ( PTS 1104596 )           */
/*                - opt_debug with VMAKE_OPTION=e switch of with imf +e ... */
/*                  ( CTS 1104286 )                                         */
/*               bugfix:                                                    */
/*                - inc=-:layer/module (PTS 1104466)                        */
/*                - imf :layer/module/-<Option> (PTS 1104466)               */
/*                - correct entry in collectingliste for descriptions for   */
/*                  desc/version/<dirctories>/description ( PTS 1103893)    */
/*                - allocate space for $INSTLIB-Environmentvariable because */
/*                  a local variable not copy to environment at some        */
/*                  platforms and so it's destroyed after leave routine     */
/* 9.7.2 23.08.99 bugfix:                                                   */
/*                - max_depmade (return tip->made if < 0 ) (PTS 1103746)    */
/* 9.7.1 17.08.99 - many backslashes before " under NT (CTS 1103620)        */
/*                bugfix:                                                   */
/*                - correct make after remove local dependencies            */
/*                  -> new member max_depmade in tip ( PTS 1103633 )        */
/*                - no free propagate value( PTS 1103623 )                  */ 
/*                - no propagete in com-descriptions ( PTS 1103622 )        */
/* 9.7.0 11.08.99 - comcom with DEFINED ( cba - CTS 1103594 )               */
/* 9.6.9 10.08.99 - environment variable INSTLIB for 2 or more lib          */
/*                  directories (CTS 1103482)                               */
/*                - get_com_info for includes in the same version as caller */
/*                  ( CTS 1103486 )                                         */  
/*                bugfix:                                                   */
/*                - conditional command ( PTS 1103539 )                     */
/* 9.6.8 29.07.99 bugfix:                                                   */
/*                - PTS 1103379 => PTS 1103379-1							*/
/* 9.6.7 20.07.99 - files search case sensitive (CTS 1102705)				*/
/*                - write permissions for sources from CD (CTS 1103400)     */
/*                bugfix:                                                   */
/*                - create directories without last / (PTS 1103276)         */
/*                - demands and depenencies in not only in the fist level   */
/*                  from an include (PTS 1103112)							*/
/*                - change module if dependencies changed as before         */
/*                  (PTS   1103379)                                         */
/* 9.6.6 08.06.99 - "!?"<cmd> for conditional run of os-calls(CTS 1102522)  */
/*                - different demanddirs for demands in com-descriptions    */
/*                  ( PTS 1103000 )                                         */
/*                - newer file in later tree with path (CTS 1000199)        */
/* 9.6.5 26.04.99 - new option for Modules - nodebug (CTS 1102466)          */
/*                - print Buildstring in protocol (CTS 1102333)             */
/*                bugfix:                                                   */
/*                - check target directory for Extra files (PTS 1102465)    */
/*                - copy includes if in local vmake_path existent           */
/*                  (PTS 1102287)                                           */
/* 9.6.4 25.03.99 - if binary for module or include no call tools           */
/*                - "-><directory>/" for more then one files in             */
/*                    com-descriptions (PTS 1102241)                        */
/*                - defaultlayer also for mac files (PTS 1102240)           */
/* 9.6.3 17.03.99 bugfix													*/
/*                - protocol in false directory if default layer set        */
/*                  (PTS 1102210)											*/
/* 9.6.2 16.03.99 bugfix													*/
/*				  - Error: descriptions in <xxx><12> not known (PTS 1102198)*/
/*                - create $OWN/tmp   PTS 1102197                           */
/* 9.6.1 01.03.99 - "xxx distrib " in link descriptions                     */
/*                - propagate also while collecting + bugfix propagate		*/
/*                bugfix:													*/
/*                - initialize local variables rc and remake in fuctions    */
/*                   with 0 (PTS 1002078)                                   */
/*                - substitute layernames in desriptions (PTS 1002075)      */
/*                - cut ':' on protocolname if call imf ::<layername>/...   */
/*                    (PTS 1002076)                                         */ 		
/* 9.6.0 09.02.99 create Filelists with call option -F                      */
/*                - only with new option "? distrib :..." in the descr.     */
/* 9.5.6 28.01.99 bugfix                                                    */
/*                - substitute also for ? linkoption (PTS 1001820)          */
/*                - in com-descriptions +<option> without =1  (PTS 1001838) */
/*                - in UNIX: copy file from local SRC directory to temp     */
/*                  if variant (was link only) (PTS 1002016)                */
/* 9.5.5 26.10.98 bugfix (9.5.3):											*/
/*                - propagate (PTS 1001613)									*/
/*                - :<layername>/<name> -> <output>	(PTS 1001577)			*/
/* 9.5.4 08.10.98 modifying only vmake server (bugfix):                     */
/*                - close filehandles if read_file or seek_file finished    */
/*                - close all opened file-/dirhandles if connection closed  */
/*                ( PTS 1001420 )                                           */
/* 9.5.3 06.08.98 remote mkdir, rename, unlink, optimize diectory caching   */
/*                target option propagate                                   */
/* 9.5.2 06.07.98 allow destination directories for demands                 */
/*                remove trailing spaces while line reading of descriptions */
/*                Langextinfo ( langchar extensions, ... ),                 */
/*                v..(i) mark includes in demand list                       */
/* 9.5.1 02.07.98 use environment variable INSTROOT instead of DBROOT       */
/*                block [s ... s], [q ... q], [f ... f] in target lines     */
/* 9.5.0 01.07.98 print out ext_dependencies, code revision                 */
/*                allow explicit target (.lnk,.mac,...), qualified desc (::)*/
/*                ? output, ? defaultversion, more than one fixed file      */
/*                ? link with, ? link option                                */
/*                descriptions in $DESC and not in $DESC/{fast/quick/slow}  */
/*                &fast, &quick, &slow, &endfast, ...                       */
/* 9.4.5 03.06.98 change date/time output, regular expression in com-lists  */
/* 9.4.4 26.05.98 bugfix: output, mci_defaultlayer, more than one extdep    */
/* 9.4.3 18.05.98 bugfix: uncond, includes with variants, '->' for objects  */
/* 9.4.2 11.05.98 bugfix: extdep, ?require                                  */
/* 9.4.1 01.05.98 target options with '?', require, defaultlayer,           */
/*                qualificated modul names (:)                              */
/* 9.4.0 21.04.98 bugfix                                                    */
/* 9.3.9 20.04.98 bugfix: remake, vopt_local                                */
/* 9.3.8 09.04.98 extdept= , target variants, new concom 'IFNVAR', 'IFVAR'  */
/* 9.3.7 01.04.98 interface, bugfix dept=                                   */
/* 9.3.6 23.03.98 -R 'requirements' for targets                             */
/* 9.3.5 16.03.98 new protokoll (Modules.mX), bugfix: pipe handling \n (NT) */
/* 9.3.4 11.03.98 remake                                                    */
/* 9.3.3 10.03.98 separates dates/obj files for layer (improve performance) */
/* 9.3.2 04.03.98 bugfix: noobjcopy(mod), opt_noexec(extra); lint C++       */
/* 9.3.1 26.02.98 code revision: misc stat in bitfields                     */
/* 9.3.0 27.02.98 noobjcopy for trgts, new default com-lines (h*)           */
/*                all combin. of defaults possible,                         */
/*                bugfix for obj=, new description:  noobjcopy              */
/* 9.2.9 09.02.98 dep file isn't a include dependencies                     */
/* 9.2.8 28.01.98 well_known ext: lex, ycc                                  */
/* 9.2.7 27.01.98 bugfix: demand target if target exists                    */
/* 9.2.6 23.01.98 bugfix: get_layer, is_global_include                      */
/* 9.2.5 19.01.98 more objects for a module: obj=                           */
/* 9.2.4 16.01.98 explicit dependencies: dep=; definition for DLL's         */
/* 9.2.3 13.01.98 gsize-dependencies in make_module removed for WIN32       */
/* 9.2.2 12.01.98 vmakcoll.c ohne /O2 optimiert, da damit Fehler auftreten  */
/* 9.2.1 07.01.98 bugfix(9.2.0): make_other_inc: don't unlink if            */
/*                tip->name equal tool-output                               */
/* 9.2.0 06.01.98 allow extension for Modules, allow longer layers, first   */
/*                look for modules than for extra, .com default line  for   */
/*                different languages, demand modul objects,                */
/*                binary modules ( no unpack, no compile )                  */
/* 9.1.5 01.12.97 is_global_include                                         */
/* 9.1.4 28.11.97 introduce env_ARC_TYPE, allow c includes for cpp modules  */
/* 9.1.3 18.11.97 try to stretch files, if we can't unlink the file         */
/* 9.1.2 17.11.97 look for tools first in $TOOL/bin, then in VMAKE_PATH     */
/* 9.1.1 11.11.97 create_directories extendet with important dbroot dirs    */
/* 9.1.0 08.08.97 TOOLEXT, get_lang_info extended by x                      */
/* 9.0.9 17.06.97 Dates files for libs and pgms; Collect startprot to .p0   */
/* 9.0.8 20.05.97 Valid bits for dates files; -s nolink; Extra: ext=; VOPT=H*/
/* 9.0.7 12.05.97 copy_file replaces system(CMD_CP); unset TZ in WIN32      */
/* 9.0.6 17.04.97 protfiles .p0 - .p9, .x0 - .x9; TOOLSHELL                 */
/* 9.0.5 14.04.97 inc=[+-] disposition; -N no protocol; -E Error on includs */
/* 9.0.4 09.04.97 VOPT=b C modules can include Pascal includes              */
/* 9.0.3 26.03.97 Datesfiles synchronized with objects                      */
/* 9.0.2 06.01.97 Bugfix in vmakserv.c/get_msg: COPY overflw from (char*)pkt*/
/* 9.0.1 09.12.96 Case insensitive get_dir_entry                            */
/* 9.0.0 23.10.96 VOPT=A Allow object copy on local system                  */
/* 8.9.9 18.09.96 enabled binary file transfer; TOLerate utime diffs        */
/* 8.9.8 26.06.96 WIN32: close thread and process handles in cmdexe         */
/* 8.9.7 05.06.96 WIN32: ln :== cp -m                                       */
/* 8.9.6 23.05.96 and once again: -r (opt_retrieve) keep .p file            */
/* 8.9.5 08.05.96 don't collect targets twice (find_target)                 */
/* 8.9.4 21.02.96 -r keep .p file; date|1 for vmake generated files         */
/* 8.9.3 28.12.95 copy options V,W: get first non-local (not newest); -e    */
/* 8.9.2 14.12.95 -m lists Extras+Descs too; don't collect descs twice      */
/* 8.9.1 06.12.95 remove archive in tmp dir before remaking                 */
/* 8.9.0 08.11.95 .def files copied to tmp even if local                    */
/* 8.8.9 03.11.95 opt_retrieve: unpack to .p and .c files                   */
/* 8.8.8 18.09.95 default MAXFNMLGT = 63; cmdexe MAXLINELENGTH 72           */
/* 8.8.7 21.08.95 !unix: target link problems -> WARNING; Extra: binary     */
/* 8.8.6 14.08.95 v??00 -> v??00.h                                          */
/* 8.8.5 05.07.95 .def files always in $DESC/fast; -C option                */
/* 8.8.4 09.06.95 .exe,.dll -> pgm (not unix)                               */
/* 8.8.3 24.05.95 FILENAMESIZE=256                                          */
/* 8.8.2 19.05.95 Support for definition files                              */
/* 8.8.1 12.05.95 Support for binary file transfer MS<->UNIX                */
/* 8.8.0 05.01.95 C: is drive name, not node name                           */
/* 8.7.9 03.01.95 default version NEVER slower than version                 */
/* 8.7.8 19.12.94 MOD_DAT environment variable                              */
/* 8.7.7 01.12.94 bugcheck(8.7.3): ignore comments                          */
/* 8.7.6 18.10.94 maintain minpathlvl for all dependencies of objects       */
/* 8.7.5 14.10.94 options with a slash are not files                        */
/* 8.7.4 06.10.94 local make corrected for includes                         */
/* 8.7.3 18.08.94 don't skip 'insdep' for targets already present           */
/* 8.7.2 12.08.94 print VMAKE_PATH to prot, -n does create prot             */
/* 8.7.1 27.07.94 allow DLLs in LIBs                                        */
/* 8.7.0 02.06.94 TT_OPTION insert_target into file list                    */
/* 8.6.9 17.05.94 search_library corrections (Gloria)                       */
/* 8.6.8 26.04.94 ->$xxx/file1 generated wrong file name in search_library  */
/* 8.6.7 18.04.94 ->$xxx/file1 generated wrong file name                    */
/* 8.6.6 29.03.94 -m without variant identifiers                            */
/* 8.6.5 24.03.94 allow for DSCR=                                           */
/* 8.6.4 09.03.94 inc=module: mfcexp                                        */
/* 8.6.3 03.03.94 !!uncond command; -debug=1 to concom                      */
/* 8.6.2 01.02.94 DLL support; again ->file corrections; !alnum options     */
/*                -%X renamed to %X; options for .lib and .rel              */
/* 8.6.1 06.01.94 -d search object files through VMAKE_PATH                 */
/* 8.6.0 30.11.93 correct ->file (fixed_file)                               */
/* 8.5.9 25.11.93 search cmdscripts in $TOOL/bin after VMAKE_PATH           */
/* 8.5.8 24.11.93 break if target itself in inc= list; opt=SV copies Extra  */
/* 8.5.7 15.10.93 quotes in target token of dscr.; ->file in target lists   */
/* 8.5.6 14.10.93 Extras must be local; dirread rejects !alnum&&!'_'        */
/* 8.5.5 23.09.93 trg_info.status: srch lib by nearest obj; rc != COMMNDerr */
/* 8.5.4 09.09.93 read .com file before analyzing it; -n: no stat on target */
/* 8.5.3 25.08.93 concom correctly counts lines in files                    */
/* 8.5.2 17.08.93 copy dates: src->xtr, .o->.gsz; check size .o > 0         */
/* 8.5.1 03.08.93 macros return maximum rc from dependencies (for commands) */
/* 8.5.0 02.08.93 print_env_opt with -n; don't collect targets twice 2. try */
/* 8.4.9 28.07.93 mkdir when linking and copying files                      */
/* 8.4.8 27.07.93 No set_protocol with -m,-c; -DMAXFNMLGT=...; vmakprot     */
/* 8.4.7 25.06.93 break search for file if errno != ENOENT                  */
/* 8.4.6 10.06.93 debug pgm with -g instead of -lg; vm_path Odsot display   */
/* 8.4.5 01.06.93 copy collection phase protocol by fread/fwrite            */
/* 8.4.4 28.05.93 don't collect target lists recursively                    */
/* 8.4.3 26.04.93 vopt=R: ignore shrglob/noshrglob descriptions             */
/* 8.4.2 22.03.93 don't create 'gsize' with opt_noexec                      */
/* 8.4.1 28.02.93 dirread error: initialize file cache in diropen           */
/* 8.4.0 19.02.93 correct startprot;setbuf:no malloc loop;keepalive sockets */
/* 8.3.9 11.02.93 don't copy generated extra files; no remote obj with -d   */
/* 8.3.8 01.02.93 start prot in tmpfile, no !vmake.xpro                     */
/* 8.3.7 28.01.93 use 'compp' with opt=SO                                   */
/* 8.3.6 18.01.93 allow quoted options (\,',") in descriptions              */
/* 8.3.5 14.01.93 don't collect targets twice                               */
/* 8.3.4 06.01.93 open lint protocol at the correct time                    */
/* 8.3.3 17.12.92 don't copy libs,rels,pgms                                 */
/* 8.3.2 09.12.92 correct filename generation in copy_shrglob               */
/* 8.3.1 03.12.92 observe version of .gsz files when copied with lib        */
/* 8.3.0 01.12.92 print options in start protocol                           */
/* 8.2.9 30.11.92 cache correction (big directories)                        */
/* 8.2.8 25.11.92 incopy: copy to temp file, then move                      */
/* 8.2.7 24.11.92 expect .gsz only after compilation of PASCAL file         */
/* 8.2.6 19.11.92 create directories before opening protfile                */
/* 8.2.5 11.11.92 vmakserv: (DIR*)/(FILE*) mismatch; link/unlink->move      */
/* 8.2.4 10.11.92 -l correction; descriptions copy with vopt V              */
/* 8.2.3 09.11.92 dates files must be enabled by vopt D                     */
/* 8.2.2 04.11.92 son closes 0,1,2; start with protocol from last arg       */
/* 8.2.1 22.10.92 deliver default version to subtargets                     */
/* 8.2.0 20.10.92 dates files for saving the used source's dates            */
/* 8.1.9 20.10.92 use $SHELL, don't set -C: comppc uses VMAKE_VERSION       */
/* 8.1.8 13.10.92 xprot: shows dates of all files                           */
/* 8.1.7 12.10.92 .gsz as dependencies                                      */
/* 8.1.6 29.09.92 linksh[rm] with -Y if shrglob                             */
/* 8.1.5 29.09.92 Langinfo: list of supported languages                     */
/* 8.1.4 29.09.92 !commands export the VMAKE_VERSION of their calling macro */
/* 8.1.3 03.09.92 Extra: list of supplement files handled by mfextra        */
/* 8.1.2 18.08.92 other_inc: source arg after description args              */
/* 8.1.1 17.07.92 vopt V copies includes too                                */
/* 8.1.0 14.07.92 new names: VMAKE_VERSION, VMAKE_DEFAULT, VMAKE_PATH       */
/* 8.0.1 14.07.92 -l,-g, vopt l,g local/global make: own objects/newest obj.*/
/* 8.0.0 14.07.92 source on a level prohibits object from later tree        */

/* version 7.9  13.07.92  search/copy file in make phase not in coll phase  */
/* version 7.8  03.07.92  deliver descriptions + dependencies to mf[ac]inc  */
/* version 7.7  03.07.92  use 'dirent' to access directories (portability)  */
/* version 7.6  02.07.92  create directories needed for creation of files   */
/* version 7.5  02.07.92  vopt L handles libs like relocs: ignore contents  */
/* version 7.4  01.07.92  vopt W && -u: don't copy libs                     */
/* version 7.3  29.06.92  vopt M identifies all mods as standalone          */
/* version 7.2  06.04.92  libraries can contain relocatables                */
/* version 7.1  27.03.92  remote objects with NOT trailing // instead of @  */
/* version 7.0  26.03.92  New Development Environment: OWN GRP ALL REL      */
/* version 6.15 17.02.92  VMAKE_OPTION=SV copies all sources into OWN tree  */
/* version 6.14 17.02.92  Initialization of file_cache after malloc         */
/* version 6.13 22.01.92  shmlink without -Y, outwrite shrglob              */
/* version 6.12 14.01.92  -L creates only one protocol. (protf lint)        */
/* version 6.11 31.12.91  -u with lib does not build members uncond         */
/* version 6.10 21.11.91  -U generates tip->file for libraries              */
/* version 6.9  17.10.91  inc= gets no default; vmakeman searched $TSRCVMAK */
/* version 6.8  09.10.91  -U && opt=O doesn't remove objs,libs,relocs       */
/* version 6.7  08.10.91  timestamp the start of bigger objects             */
/* version 6.6  07.10.91  Support for shared globals                        */
/* version 6.5  02.10.91  Copy objects from remote                          */
/* version 6.4  23.09.91  search options for sources and objects: vVwW      */
/* version 6.3  23.09.91  manual file, -UI option                           */
/* version 6.2  11.09.91  enable remote objects with '@node:'               */
/* version 6.1  11.09.91  prot names .prot .xpro save old to .PROT .XPRO    */
/* version 6.0  12.08.91  conditional compilaton in description files       */
/* version 5.15 07.08.91  VMAKE_OPTION=h gives .shm precedence over .lnk    */
/* version 5.14 07.08.91  changes for IBM RS 6000 ( _IBMR2 ): ar files      */
/* version 5.13 15.07.91  set found_o for modules in a lib with OPT=O       */
/* version 5.12 15.07.91  set level_s for includes of modules with level_s  */
/* version 5.11 21.06.91  correct options in .lnk, combine lint modules     */
/* version 5.10 06.06.91  don't try to use .p files if nonexist. (SpCO)     */
/* version 5.9  24.05.91  rm before incp, -S vxx00: no $INCL, opts in .lnk  */
/* version 5.8  19.03.91  don't delete .c with opt=CO and uncond            */
/* version 5.7  21.02.91  pausable vmake (SIGTRAP)                          */
/* version 5.6  18.02.91  compiler specific options (-%P...,-%C...,-%T...)  */
/* version 5.5  08.02.91  allow longer descriptions (FILENAMESIZE)          */
/* version 5.4  16.01.91  don't delete .c with opt=CO and uncond            */
/* version 5.3  06.11.90  .con/.typ with mfsize, VDNVERSION exported        */
/* version 5.2  17.10.90  opt_nodep not used for macros                     */
/* version 5.1  10.10.90  supply default value for conditional comp. opts   */
/* version 5.0  01.10.90  Support of variants                               */
/* version 4.3  19.09.90  -U removes targets before rebuilding them         */
/* version 4.2  12.09.90  correction: -u with CO options don't use p-file   */
/* version 4.1  18.07.90  lint support, sizeof support                      */
/* version 4.0  12.07.90  descriptions for include files                    */
/* version 3.7  09.07.90  extended protocol                                 */
/* version 3.6  29.06.90  -K option, uppercase/lowercase level letters      */
/* version 3.5  30.05.90  PATH_DELIMITER ','    Environment vars in file    */
/* version 3.4  22.05.90  shared relocatables                               */
/* version 3.3  27.04.90  enable reloc use reloc, unmade mod into lib       */
/* version 3.2  20.03.90  enable unconditional includes                     */
/* version 3.1  27.02.90  lint corrections                                  */
/* version 3.0  19.02.90  C-port support                                    */
/* version 2.4  27.11.89  NODE_SEPARATOR ':', PATH_DELIMITER ';'            */
/* version 2.3  23.11.89  server: outcopy,link,unlink                       */
/* version 2.2  17.11.89  members/objects with -d                           */
/* version 2.1  19.10.89  absolute temp files                               */
/* version 2.0  11.10.89  remote access                                     */

#endif
