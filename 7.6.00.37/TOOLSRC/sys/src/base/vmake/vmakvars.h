/*


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
#ifndef _VMAKEVARS_H
#define _VMAKEVARS_H

/*      vmakvars.h     */

/* data used by vmake */

#ifdef  WIN32
#include <Windows.h>
global HANDLE hStopRequest;
global HANDLE hStopRequestThread;
global HANDLE hChildProcess;
#endif


/* options */
char			** opt_argv ;
int             opt_interactive ;
int             opt_watch ;
int             opt_background ;
int             opt_collection ;
int             opt_modlist ;
int             opt_print ;
int             opt_noexec ;
int             opt_retrieve ;
int             opt_lint ;
int             opt_sizeof ;
int             opt_local ;
int             opt_global ;
int             opt_nodep ;
int             opt_nameduncond ;
int             opt_uncond ;
int             opt_uncondincl ;
int             opt_keep ;
int             opt_profile ;
int             opt_debug ;
int				opt_man ;
int				opt_noprot ;
int				opt_errorinc ;
int				opt_nolink ;
int             opt_require ;
int             opt_nodebug_col ;
int             opt_distrib;    /* &gar CTS 1000237 */
int             opt_optdebug;   /* CTS 1110663 */

int             opt_nodircache;
int             opt_nodependencies;

/* CTS DEPS */
int             opt_showdeps;
int             opt_countdeps;


int             opt_check_for_running_vmake;


/* environment */

char            * env_VMAKE_VERSION ;
char            * env_VMAKE_DEFAULT ;
char            * env_VMAKE_OPTION ;
char            * env_VMAKE_PATH ;
char            * env_INSTROOT ;
char            * env_SHELL ;
char            * env_TOOLSHELL ;
char            * env_TOOL ;
char            * env_TOOLEXT ;
char            * env_TOOLOPT ;
char            * env_DLL_TYPE ;
char            * env_ARC_TYPE ;
char            * env_BUILDSTRING;
char            * env_INSTLIB;
char            * env_OWN;
char            * env_BUILDPID; /* CTS 1111841 */
char            * env_VMAKE_GLOBALS;
char            * env_VMAKE_DEFAULT_VARIANT;
char			* env_RELVER;
char			* env_CORRECTION_LEVEL ;
char			* env_BUILDPRAEFIX ;
char			* env_OSSPEC;
char			* env_MACH;
char			* env_BIT64;
int             env_CMD_OUTPUT_LENGTH;
int             env_SUPPRESS_VMAKE_PIDFILE;
int             env_VMAKE_SILENT_MODE;
int             env_VMAKE_PRINT_STATISTICS;
int             env_VMAKE_NO_DIRCACHE;
int             env_VMAKE_REPORT_PERCENT;
int             env_VMAKE_REPORT_TIME;
char            * env_VMAKE_TEST_READDIR;


/* data computed from options and environment */

int					vopt_level_i ;
int					vopt_level_s ;
int					vopt_level_p ;
int					vopt_level_c ;
int					vopt_level_o ;
int					vopt_prefer_shm ;
int					vopt_timestamp ;
int					vopt_source_warn ;
int					vopt_source_copy ;
int					vopt_object_warn ;
int					vopt_object_copy ;
int					vopt_mod_only ;
int					vopt_reloc_lib ;
int					vopt_local ;
int					vopt_global ;
int					vopt_dates ;
int					vopt_ignore_shrglob ;
int					vopt_local_objcopy ;
int					vopt_c_includes_p ;
int					vopt_no_auto_hdr ;
int                 vopt_no_ext_map ;
int                 vopt_rw_permission;       /*&gar RW*/
int                 vopt_no_case_sens_error; /* PTS 1102705 */
int					vopt_desc_in_src;		 
int					vopt_current_date;          /* CTS 1110368 */
int					vopt_xml_prot; 		     /* CTS XML */
int					vopt_fulloutput;         /* CTS 1112006 */
int                 vopt_new_variant_handling;
int                 vopt_make_in_objdir;
int                 vopt_break_after_first_error;
int                 opt_check_for_running_vmake;
int                 opt_silent_mode;
int					opt_warning_protocol;


struct path_info    * vmake_path ;
char                vmakeversion ;


/* cache for objects frequently used during collection */

struct dir_info         * dir_lists ;
struct com_info         * com_lists ;
struct extra_info       * xtr_lists ;
struct lang_info	    * lng_lists ;
struct langext_info	    * lngext_lists ;
struct dirmap_info      * map_lists; /* PTS 1105033 */
struct mod_com_info     * global_mci;
struct opt_info         * option_list;         
struct com_info         * spec_com_lists ;



/* the collection */

struct dep_info         * make_list ;
struct trg_info         * macros ;
struct trg_info         * projects ;
struct trg_info         * programs ;
struct trg_info         * dynlinklibs ;
struct trg_info         * relocs ;
struct trg_info         * libraries ;
struct trg_info         * javaprojects ;
struct trg_info         * modules ;
struct trg_info         * includes ;
struct trg_info         * files ;
struct trg_info         * command_list ;
struct trg_info         * dirlist ;
struct trg_info         * extra_list ;
struct dsc_info         * desc_list ;
struct dist_info        * dist_list;   /* &gar CTS 1000237 */
struct dscfile_info     * plus_descriptions;

/* data used during the make phase */

int			yydebug ;
char                    tmp_dir [ FILENAMESIZE ] ;
char                    vmakeversion_env [ FILENAMESIZE ] ;
char                    vmakelayer_env [ FILENAMESIZE+16 ] ;
int                     cmdargc ;
char                    * cmdargv [ MAXCMDARGS ] ;
int                     cmdlgt ;
int						is_variant;    /* for UNIX: copy or link if variant */
char                    cmdname [ LINESIZE ] ;
char                    cmd [ CMDBUFFERSIZE + LINESIZE ] ;
char                    outstampline [LINESIZE];
char                    outstamptitle [FILENAMESIZE];
static int                     current_silent_char = -1;
static char                    silent_char_string_rotate [] = "|/-\\";
static char                    silent_char_string_dot [] = "...............";
char                    *silent_char_string;
int					current_silent_char_count;
int                     silent_rotate_activated;


static char                std_objectext[]=".o"; /* CTS 1104953 */
					   /* ^ security space */
#if ! unix
int			warn_target_link ;
#endif

/* CTS 1110368 */
time_t date_make;

static char default_variant_separator [] = "+";


/* CTS 1111841 */
time_t xtr_date;        /* Date of Extra */
time_t lng_date;        /* Date of LangInfo */
time_t lngext_date;     /* Date of LangExtInfo */
time_t map_date;        /* Date of DirectoryMapping */

/* CTS XML */
/*
struct xml_info         * xml_prot;
struct xml_info         * xml_modprot;
*/

/* CTS 1112006 */
struct output_info		* output_list;

char * PIDFilename;
int fd_PIDFile;

char *ENV_TO_SET;

struct mod_com_info     * empty_com_dummy;

char *main_layer;

int all_target_counter;
int finished_target_counter;
int allerror_target_counter;

int next_report_percent;
int next_report_time;

#endif
