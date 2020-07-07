/*!
  @file           ven12.c
  @author         JoergM
  @brief          Client RunTime: sqlarg3/sqlargs
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/

#define MOD__ "ven12.c:"
/* ***** sqlarg[l3] includes ********************************************** */

#include        <ctype.h>
#include "gen00.h"
#include "heo46.h"
#include "gen13.h"
#include "geo001.h"
#include "geo57.h"
#include "hsp77.h"
#include "hsp78_0.h"
#include "hsp81.h"
#include "hsp83.h"
#include "heo12.h"
#include "heo13.h"

/* ***** sqlarg[l3] macros ************************************************ */

#undef  toupper
#define toupper(x)      (x>='a'&&x<='z'?x-'a'+'A':x)

#define PUTBIT(b,i)     { b [ (unsigned) i / 8 ] |=                          \
					  ( 1 << ( (unsigned) i % 8 ) ) ; }
#define CLRBIT(b,i)     { b [ (unsigned) i / 8 ] &=                          \
					~ ( 1 << ( (unsigned) i % 8 ) ) ; }
#define TSTBIT(b,i)     ( b [ (unsigned) i / 8 ] &                           \
			 ( 1 << ( (unsigned) i % 8 ) ) )

/* ***** sqlarg[l3] global consts, types and vars ************************* */

#define unknown_error           "unknown error                           "
#define no_valid_option         "no valid option                         "
#define no_comp_specified       "no component specified                  "
#define wrong_option_sequence   "wrong option sequence                   "
#define missing_quote           "missing quote in your argument string   "
#define illegal_argument        "illegal argument string                 "
#define illegal_dialog_obj      "illegal component object                "
#define double_seperator        "illegal seperator sequence              "

#define maxargcnt               MX_LINE / 2

/* ***** sqlarg[l3] exported functions, consts, types and vars ************** */

global	void			sqlarg3 (
struct tsp4_xuser_record       *user_params ,
tsp00_Pw                        password ,
struct tsp4_args_options       *options ,
tsp4_xuserset                   xusertype ,
tsp00_ErrText                   errtext ,
tsp00_Bool                     *ok
								 );
global	void			sqlargs (tsp00_Line args);
global	void	                sqlargl (tsp00_Line args);

/* ***** sqlarg[l3] imported functions, consts, types and vars ************** */

extern  void                    sql03_split_dbname () ;

extern  char                    * optarg ;
extern  int                     optind ;
extern  int                     opterr ;


/* ***** sqlarg[l3] local functions, consts, types and vars *************** */

static	void			args_init ();
static	void			prec_args_init ();
static	void			pc_rt_args_init ();
static	void			dcomp_args_init ();
static	void			audit_args_init ();
static	void			userx_args_init ();
static	void			prec_opt_analysis ();
static	void			pc_rt_opt_analysis ();
static	void			dcomp_opt_analysis ();
static	void			audit_opt_analysis ();
static	void			userx_opt_analysis ();
static	void			mk_user_passwd ();
static	void			mk_dbid ();
static	void			mk_dbnode ();
static	void			mk_dblang ();
static	void	mk_userkey (struct tsp4_xuser_record       *user_params ,
                            tsp4_xuserset                   xusertype , 
                            tsp00_Bool                     *ok ,
                            tsp00_ErrText                   errtext );
static	void			mk_sqlmode ();
static	void			mk_isolation ();
static	void			mk_timeout ();
static	void			mk_cachelimit ();
static	void			mk_args ( int                      o_char ,
								tsp4_component             component ,
								struct tsp4_xuser_record * user_params ,
								struct tsp4_args_options * options ,
								tsp4_xuserset              xusertype ,
								tsp00_ErrText              errtext ,
								tsp00_Bool               * ok );
static	void	        mk_my_args ( int                o_char ,
							tsp4_component              component ,
							struct tsp4_xuser_record  * user_params ,
							struct tsp4_args_options  * options ,
							tsp4_xuserset               xusertype ,
							tsp00_ErrText               errtext ,
							tsp00_Bool                * ok );
static	void			mk_prec_input_fn ();
static	void			mk_prec_trace_fn ();
static	void			mk_prec_prog_fn ();
static	void			mk_prec_margin ();
static	void			mk_prec_datetime ();
static	void			mk_prec_sys_tpmon ();
static	void			mk_prec_mode ();
static	void			mk_prec_cansi ();
static	void			mk_prec_packetsize ();
static	void			mk_prec_unicode ();
static	void			mk_pc_rt_trace_fn ();
static	void			mk_pc_rt_mfetch ();
static	void			mk_pc_rt_rstmt_cnt ();
static	void			mk_pc_rt_rtime_limit ();
static	void			mk_dcomp_r_fn ();
static	void			mk_dcomp_b_fn ();
static	void			mk_dcomp_r_obj ();
static	void			mk_dcomp_b_obj ();
static	void			mk_dcomp_object ();
static	void			mk_dcomp_r_obj ();
static	void			mk_dcomp_b_obj ();
static	void			mk_dcomp_object ();
static	void			mk_dcomp_code_type ();
static	void			mk_dcomp_export ();
static	void			mk_dcomp_import ();
static	void			mk_audit_cntrl ();
static	void			mk_audit_logfile ();
static	void			mk_audit_log_blocksize ();
static	void			mk_audit_output ();
static	void			mk_userx_b_fn ();
static	void			move_toupper ();
static	void			check_idents ();
static	void			mk_user_pass_l ();
static	void			mk_dbname ();
static	void			mk_cmd ();
static	void			mk_argl ();
static	void			mk_a_line ();
static	void			sql12_build_arg_line_from_struct ();
static	int			str_len ();
static	int			str_chr ();

static	int                     argc = 0;
static	char                    **argv = ((char **)NULL);


/* ***** old sqlargs ****************************************** */

typedef struct 
            {
	    tsp00_KnlIdentifier username       ;
	    tsp00_Name          password        ;
	    tsp00_DbName        dbname          ;
	    tsp00_C12           cmd             ;
	    tsp00_VFilename     filename        ;
	    tsp00_C40           aname           ;
	    tsp00_Line          arguments       ;
            } old_sql_arg_line ;
 
static	old_sql_arg_line            sal ;

static	tsp00_Bool                 username_found = FALSE;
static	tsp00_Bool                 password_found = FALSE;
static	tsp00_Bool                 dbname_found = FALSE;
static	tsp00_Bool                 cmd_found = FALSE;
static	tsp00_Bool                 filename_found = FALSE;
static	tsp00_Bool                 arguments_found = FALSE;

static	tsp00_Bool                 env_dbname_found = FALSE;
static	tsp00_Bool                 env_dblang_found = FALSE;

#define OPT_AND_ARG_CNT         25
static	int                     my_argc = 0;
static	char                    * my_argv [ OPT_AND_ARG_CNT ] = 
{
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL
};

#define         MAX_IDENT_CNT   3
static	struct
{
    char *              ident [ MAX_IDENT_CNT ] ;
    long                i_len [ MAX_IDENT_CNT ] ;
    tsp00_Bool          i_tup [ MAX_IDENT_CNT ] ;
} ctu = {
	{ NULL, NULL, NULL }, { 0L, 0L, 0L }, { FALSE, FALSE, FALSE }
}; /* convert to upper structure */

&if $COMP = PTOC
#  include <ptoc.h>	/* for global _argc and _argv */
&else
   extern	void			sql12a_argas ();
&endif

/*==========================================================================*/


global	void	sqlarg3 ( 
struct tsp4_xuser_record       * user_params ,
tsp00_Pw                         password ,
struct tsp4_args_options       * options ,
tsp4_xuserset                    xusertype ,
tsp00_ErrText                    errtext ,
tsp00_Bool                     * ok )
{
	tsp4_component           opt_rec_comp ;

#ifdef  DUMMY
	char			argums [ MX_LINE ];
	char			*argumv [ 1 ];
#endif


	DBG1 (( MF__,"********** sqlarg3 called ********************\n" ))

	/* ***** initialize some of the return parameter ***** */

	*ok = TRUE ;
	args_init ( user_params , password , xusertype );


	/* ***** initialize extern vars ***** */

	opterr = 0 ;
	optind = 1 ;


	/* ***** look for the componint within the option record ***** */
	/* ***** it must be in the correct range                 ***** */

	opt_rec_comp = options -> opt_component ;

	if ( ( ( int ) opt_rec_comp < sp4co_sql_precompiler  ) ||
	     ( ( int ) opt_rec_comp > sp4co_sql_param        )    )
	{
		* ok = FALSE ;
		SAPDB_memcpy ( errtext , no_comp_specified , sizeof ( tsp00_ErrText ) ) ;
		return ;
	}


	/* ***** return of the addresses of argc and argv ***** */
	/* ***** ( not the system argv and argc !!!! )    ***** */
 
#ifdef	DUMMY
	    {
		argc = 1 ;
		argv = argumv ;
		argv[0] = argums ;
		argv[1] = 0 ;
		(void) strcpy ( argums , "hallo" );
	    }
#else
&  if $COMP = PTOC
	    {
		argc = _argc ;
		argv = (char**) _argv ; /* The PTOC type for _argv is funny */
	    }
&  else
#    if SUN || SOLARIS
	    {
		extern	int	_ARGC ;
		extern	char	**_ARGV ;
		argc = _ARGC ;
		argv = _ARGV ;
	    }
#    else
#      if HP9
	    {
		argc = _argc ;
		argv = (char**) _argv ;
	    }
#      else
	    {
		sql12a_argas ( &argc , &argv );
	    }
#      endif  /*HP9*/
#    endif  /*SUN||SOLARIS*/
&  endif  /*PTOC*/
#endif	/*DUMMY*/

	DBG1 (( MF__,"sqlarg3         : opterr                         = %d\n",
				  opterr ))
	DBG1 (( MF__,"sqlarg3         : optind                         = %d\n",
				  optind ))
	DBG1 (( MF__,"sqlarg3         : * ok                           = %d\n",
				  * ok ))
	DBG1 (( MF__,"sqlarg3         : argc                           = %d\n",
				  argc ))
#       ifdef DEBUG_RTE
	if ( argc > 0 )
	{
	    DBG1 (( MF__,"sqlarg3         : argv [ 0 ]                     = %s\n",
							  argv [ 0 ] ))
	    DBG1 (( MF__,"sqlarg3         : argv [ argc - 1 ]              = %s\n",
							  argv [ argc - 1 ] ))
	}
#       endif


	/* ***** jump to the component's option analysis ***** */

	DBG1 (( MF__,"sqlarg3         : opt_rec_comp                   = %d\n",
				  opt_rec_comp ))

	switch ( opt_rec_comp )
	{
		case sp4co_sql_precompiler  :
			opterr = 1 ;
			prec_args_init ( options ) ;
			prec_opt_analysis ( user_params , password , options ,
					    xusertype , ok , errtext ) ;
			break ;

		case sp4co_sql_pc_runtime  :
			opterr = 1 ;
			pc_rt_args_init ( options ) ;
			pc_rt_opt_analysis ( user_params , password ,
					     options , xusertype ,
					     ok , errtext ) ;
			break ;

		case sp4co_sql_easy    :
		case sp4co_sql_query   :
		case sp4co_sql_dialog  :
		case sp4co_sql_appl    :
		case sp4co_sql_load    :
		case sp4co_sql_util    :
		case sp4co_sql_param   :
			dcomp_args_init ( options ) ;
			dcomp_opt_analysis ( user_params , password ,
					     options , xusertype ,
					     ok , errtext ) ;
			break ;

		case sp4co_sql_audit  :
			audit_args_init ( options ) ;
			audit_opt_analysis ( user_params , password ,
					     options , xusertype ,
					     ok , errtext ) ;
			break ;

		case sp4co_sql_userx  :
			userx_args_init ( options ) ;
			userx_opt_analysis ( user_params , password ,
					     options , xusertype ,
					     ok , errtext ) ;
			break ;

	}
	DBG1 (( MF__,"sqlarg3         : password                       = >>%.18s<<\n",
				  password ))
	DBG1 (( MF__,"sqlarg3 !!!!!!! : user_params -> xu_serverdb     = %.18s\n",
				  user_params -> xu_serverdb ))
	DBG1 (( MF__,"sqlarg3 !!!!!!! : user_params -> xu_dblang       = %.18s\n",
				  user_params -> xu_dblang   ))
}

/*==========================================================================*/

static	void	args_init ( 
struct tsp4_xuser_record        * user_params ,
tsp00_Pw                        password ,
tsp4_xuserset                   xusertype )
{
   struct tsp4_xuser_record     l_user_parms ;
   tsp00_Bool                   l_ok ;
   tsp00_ErrText                l_errtext ;
   char                        *h_ptr ;

   /* ***** defaults of return parameters ***** */
   /* ***** outside the option record     ***** */

   sql13u_init_user_params( user_params );

   env_dbname_found = sql13c_get_serverdb ( &h_ptr );
   env_dblang_found = sql13c_get_dblang ( &h_ptr );

   SAPDB_memset ( password                     , ' '  , sizeof ( tsp00_Pw ) );
   SAPDB_memset ( xusertype                    , '\0' , sizeof ( tsp4_xuserset ) );

   PUTBIT ( xusertype , sp4xu_sql_usermask  ) ;


   /* ***** try to call sqlgetuser, ***** */
   /* ***** xu_key = DEFAULT_USER,  ***** */
   /* ***** check for errors        ***** */

   SAPDB_memcpy ( l_user_parms . xu_key , DEFAULT_USER , 
          sizeof ( l_user_parms . xu_key ) ) ;

   DBG1 (( MF__,"args_init       : l_user_parms . xu_key          = %.18s\n",
			     l_user_parms . xu_key ))

   sqlgetuser ( & l_user_parms , NULL, l_errtext , & l_ok ) ;

   DBG1 (( MF__,"args_init       : l_ok                           = %d\n",
			     l_ok ))
   DBG1 (( MF__,"args_init       : l_errtext                      = %.40s\n",
			     l_errtext ))
   if ( l_ok )
   {
      SAPDB_memcpy ( user_params -> xu_user       , l_user_parms . xu_user ,
	     sizeof ( user_params -> xu_user ) );
      SAPDB_memcpy ( user_params -> xu_password   , l_user_parms . xu_password ,
	     sizeof ( user_params -> xu_password ) ) ;
      SAPDB_memcpy ( user_params -> xu_sqlmode    , l_user_parms . xu_sqlmode ,
	     sizeof ( user_params -> xu_sqlmode ) ) ;
      SAPDB_memcpy ( user_params -> xu_servernode , l_user_parms . xu_servernode ,
	     sizeof ( user_params -> xu_servernode ) ) ;
      SAPDB_memcpy ( user_params -> xu_serverdb   , l_user_parms . xu_serverdb ,
	     sizeof ( user_params -> xu_serverdb  ) ) ;
      SAPDB_memcpy ( user_params -> xu_dblang , l_user_parms . xu_dblang ,
	     sizeof ( l_user_parms . xu_dblang ) ) ;

      user_params -> xu_cachelimit = l_user_parms . xu_cachelimit ;
      user_params -> xu_timeout    = l_user_parms . xu_timeout    ;
      user_params -> xu_isolation  = l_user_parms . xu_isolation  ;

      SAPDB_memcpy ( user_params -> xu_userUCS2       , l_user_parms . xu_userUCS2 ,
	     sizeof ( user_params -> xu_userUCS2 ) );
      SAPDB_memcpy ( user_params -> xu_passwordUCS2   , l_user_parms . xu_passwordUCS2 ,
	     sizeof ( user_params -> xu_passwordUCS2 ) ) ;

      PUTBIT ( xusertype , sp4xu_sql_userdefault ) ;
      CLRBIT ( xusertype , sp4xu_sql_usermask  ) ;

      env_dbname_found = env_dblang_found = FALSE ;
   }

   if ( env_dbname_found )
   {
        PUTBIT ( xusertype , sp4xu_sql_serverdb ) ;
        DBG1 (( MF__,"args_init       : DBNAME in env\n" ))
   }
   if ( env_dblang_found )
   {
        PUTBIT ( xusertype , sp4xu_sql_dblang  ) ;
        DBG1 (( MF__,"args_init       : DBLANG in env\n" ))
   }

   DBG1 (( MF__,"args_init       : user_params -> xu_key          = %.18s\n",
			     user_params -> xu_key ))
   DBG1 (( MF__,"args_init       : user_params -> xu_servernode   = %.64s\n",
			     user_params -> xu_servernode ))
   DBG1 (( MF__,"args_init       : user_params -> xu_serverdb     = %.18s\n",
			     user_params -> xu_serverdb ))
   DBG1 (( MF__,"args_init       : user_params -> xu_user         = %.64s\n",
			     user_params -> xu_user ))
   /*
   DBG1 (( MF__,"args_init       : user_params -> xu_password     = %24x\n",
			     user_params -> xu_password ))
   */
   DBG1 (( MF__,"args_init       : user_params -> xu_sqlmode      = %.8s\n",
			     user_params -> xu_sqlmode ))
   DBG1 (( MF__,"args_init       : user_params -> xu_cachelimit   = %ld\n",
			     user_params -> xu_cachelimit ))
   DBG1 (( MF__,"args_init       : user_params -> xu_timeout      = %d\n",
			     user_params -> xu_timeout ))
   DBG1 (( MF__,"args_init       : user_params -> xu_isolation    = %d\n",
			     user_params -> xu_isolation ))
   DBG1 (( MF__,"args_init       : user_params -> xu_dblang       = %.18s\n",
			     user_params -> xu_dblang ))
   DBG1 (( MF__,"args_init       : env_dbname_found               = %d\n",
			     env_dbname_found ))
   DBG1 (( MF__,"args_init       : env_dblang_found               = %d\n",
			     env_dblang_found ))
}

/*==========================================================================*/

static	void	prec_args_init ( 
struct tsp4_args_options        * options )
{
	/* ***** defaults for the option record ***** */
	/* ***** PRECOMPILER                    ***** */

	SAPDB_memset ( options -> variant.C_sp4co_sql_precompiler.opt_modulefn_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_modulefn_F ) ) ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_precompiler.opt_tracefile_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_tracefile_F ) ) ;

	options -> variant.C_sp4co_sql_precompiler.opt_prognamel_F = 0 ;
	SAPDB_memset ( options -> variant.C_sp4co_sql_precompiler.opt_progname_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_progname_F ) ) ;

	options -> variant.C_sp4co_sql_precompiler.opt_modulenamel_F = 0 ;
	SAPDB_memset ( options -> variant.C_sp4co_sql_precompiler.opt_modulename_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_modulename_F ) ) ;

	options -> variant.C_sp4co_sql_precompiler.opt_tpmonidl_F = 0 ;
	SAPDB_memset ( options -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F ) ) ;

	options -> variant.C_sp4co_sql_precompiler.opt_begmar_F    = 0 ;
	options -> variant.C_sp4co_sql_precompiler.opt_endmar_F    = 0 ;
	options -> variant.C_sp4co_sql_precompiler.opt_prof_F      = 1 ;
	options -> variant.C_sp4co_sql_precompiler.opt_trace_F     = 0 ;
	options -> variant.C_sp4co_sql_precompiler.opt_datetime_F  = 1 ;
	options -> variant.C_sp4co_sql_precompiler.opt_dyn_F       = 0 ;
	options -> variant.C_sp4co_sql_precompiler.opt_tpmon_F     = 0 ;
	options -> variant.C_sp4co_sql_precompiler.opt_mode_F      = 2 ;
	options -> variant.C_sp4co_sql_precompiler.opt_extern_F    = 1 ;
	options -> variant.C_sp4co_sql_precompiler.opt_cansi_F     = 0 ;
	options -> variant.C_sp4co_sql_precompiler.opt_comp_F      = 1 ;
	options -> variant.C_sp4co_sql_precompiler.opt_packetsize_F = 0 ;

	options -> variant.C_sp4co_sql_precompiler.opt_list_F        = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_precom_F      = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_traceswitch_F = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_comment_F     = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_silent_F      = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_nowarn_F      = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_lineno_F      = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_lib_F         = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_quote_F       = TRUE ;
	options -> variant.C_sp4co_sql_precompiler.opt_decpoint_F    = TRUE ;
	options -> variant.C_sp4co_sql_precompiler.opt_tabformat_F   = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_version_F   = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_help_F   = FALSE ;
	options -> variant.C_sp4co_sql_precompiler.opt_unicode_F= FALSE ;

	options -> variant.C_sp4co_sql_precompiler.opt_quote_F = '\'' ;
	options -> variant.C_sp4co_sql_precompiler.opt_decpoint_F = '.' ;
/*
	SAPDB_memset ( options -> variant.C_sp4co_sql_precompiler.opt_ruser_args_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_ruser_args_F ) ) ;
*/

}

/*==========================================================================*/

static	void	pc_rt_args_init ( 
struct tsp4_args_options        * options )
{
	/* ***** defaults for the option record ***** */
	/* ***** PRECOMPILER RUNTIME            ***** */

	SAPDB_memset ( options -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F ) ) ;

	options -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F     = 0 ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rprof_F      = 0 ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F    = 1 ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F  = 0 ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F= 0 ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rfiller_F    = 0 ;

	options -> variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F = FALSE ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F = FALSE ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F       = FALSE ;
	options -> variant.C_sp4co_sql_pc_runtime.opt_rfiller1_F     = FALSE ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F ) ) ;
}

/*==========================================================================*/

static	void	dcomp_args_init ( 
struct tsp4_args_options        * options )
{
	/* ***** defaults for the option record ***** */
	/* ***** DIALOG COMPONENTS AND UTILITY  ***** */

	SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ) ) ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ) ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ) ) ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_runfile_F   , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_easy.opt_runfile_F   ) ) ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_parameter_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_easy.opt_parameter_F ) ) ;

	options -> variant.C_sp4co_sql_easy.opt_append_F    = FALSE ;
	options -> variant.C_sp4co_sql_easy.opt_prompt_F    = FALSE ;
	options -> variant.C_sp4co_sql_easy.opt_codeonly_F  = FALSE ;
	options -> variant.C_sp4co_sql_easy.opt_utility_session_F = FALSE ;

	options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_none ;
	options -> variant.C_sp4co_sql_easy.opt_code_F = is_codeneutral ;
}

/*==========================================================================*/

static	void	audit_args_init ( 
struct tsp4_args_options        * options )
{
	/* ***** defaults for the option record ***** */
	/* ***** AUDIT                          ***** */

	SAPDB_memset ( options -> variant.C_sp4co_sql_audit.opt_aud_control_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_audit.opt_aud_control_F ) ) ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_audit.opt_aud_logfile_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_audit.opt_aud_logfile_F ) ) ;

	SAPDB_memset ( options -> variant.C_sp4co_sql_audit.opt_aud_output_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_audit.opt_aud_output_F ) ) ;
        options -> variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F = 8 ;
	options -> variant.C_sp4co_sql_audit.opt_aud_comm_mode_F = sp4cm_sql_none ;
}

/*==========================================================================*/

static	void	userx_args_init ( 
struct tsp4_args_options        * options )
{
	/* ***** defaults for the option record ***** */
	/* ***** USERX                          ***** */

	SAPDB_memset ( options -> variant.C_sp4co_sql_userx.opt_ux_runfile_F , ' ' ,
		 sizeof ( options -> variant.C_sp4co_sql_userx.opt_ux_runfile_F ) ) ;

	options -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_none ;
}

/*==========================================================================*/

static	void	prec_opt_analysis ( 
struct tsp4_xuser_record       * user_params ,
tsp00_Pw                         password ,
struct tsp4_args_options       * options ,
tsp4_xuserset                   xusertype ,
tsp00_Bool                     * ok ,
tsp00_ErrText                    errtext )
{
	tsp00_Bool                 more_args ;
	int                     o_char ;


	DBG1 (( MF__,"********** prec_opt_analysis called **********\n" ))

	/* ***** initialize internel vars ***** */

	more_args = TRUE ;
	o_char = ' ' ;

	DBG1 (( MF__,"prec_opt_analys : * ok                           = %d\n",
				 * ok ))
	DBG1 (( MF__,"prec_opt_analys : argc                           = %d\n",
				 argc ))

	/* ***** parameter existing ? ***** */

	if ( argc > 1 )
	{
		/* ***** get options ***** */

		while ((( o_char = sqlgetopt ( argc , argv ,
		"CORTXYlcoVWswhiLqpezu:b:d:n:U:r:y:F:P:I:t:m:S:D:M:H:E:G:x:"))
								  != - 1 ) &&
		       more_args && *ok )
		{
			DBG1 (( MF__,"prec_opt_analys : o_char                         = %c\n",
						 o_char ))
			DBG1 (( MF__,"prec_opt_analys : optind                         = %d\n",
						 optind ))
			switch ( o_char )
			{
				/* ***** general optargs ***** */

				case 'u' : mk_user_passwd ( user_params ,
							    password ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_userparms ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask  ) ;
					   break ;

				case 'd' : mk_dbid ( user_params ,
						     xusertype ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_serverdb ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'n' : mk_dbnode ( user_params ,
						       xusertype ) ;
					   break ;

				case 'U' : mk_userkey ( user_params ,
							xusertype ,
							errtext , ok ) ;
					   break ;

				case 'S' : mk_sqlmode ( user_params ,
						        xusertype ) ;
					   break ;

				case 'I' : mk_isolation ( user_params ,
						          xusertype ) ;
					   break ;

				case 't' : mk_timeout ( user_params ,
						        xusertype ) ;
					   break ;

				case 'y' : mk_cachelimit ( user_params ,
						           xusertype ) ;
					   break ;

				case 'x' : mk_dblang ( user_params ,
						       xusertype ) ;
					   break ;

				/* ***** optargs with arguments ***** */

				case 'r' : mk_prec_input_fn ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'F' : mk_prec_trace_fn ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'P' : mk_prec_prog_fn ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'm' : mk_prec_margin ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'D' : mk_prec_datetime ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'M' : mk_prec_sys_tpmon ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'H' : mk_prec_mode ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'E' : mk_prec_cansi ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'G' : mk_prec_unicode ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'b' : mk_prec_packetsize ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;


				/* ***** optargs with no arguments ***** */

				case 'C' : options -> variant.C_sp4co_sql_precompiler.opt_comp_F = 0;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_comp       = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_comp_F   ))
					   break ;

				case 'R' : options -> variant.C_sp4co_sql_precompiler.opt_prof_F = 0 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_prof       = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_prof_F ))
					   break ;

				case 'O' : options -> variant.C_sp4co_sql_precompiler.opt_trace_F = 4 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_trace      = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_trace_F ))
					   break ;

				case 'T' : options -> variant.C_sp4co_sql_precompiler.opt_trace_F = 2 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_trace      = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_trace_F ))
					   break ;

				case 'X' : options -> variant.C_sp4co_sql_precompiler.opt_trace_F = 3 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_trace      = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_trace_F ))
					   break ;

				case 'Y' : options -> variant.C_sp4co_sql_precompiler.opt_dyn_F = 1 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_dyn        = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_dyn_F ))
					   break ;

				case 'l' : options -> variant.C_sp4co_sql_precompiler.opt_list_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_list       = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_list_F ))
					   break ;

				case 'c' : options -> variant.C_sp4co_sql_precompiler.opt_precom_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_precom     = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_precom_F ))
					   break ;

				case 'o' : options -> variant.C_sp4co_sql_precompiler.opt_comment_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_comment     = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_comment_F ))
					   break ;

				case 'W' : options -> variant.C_sp4co_sql_precompiler.opt_traceswitch_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_traceswitch= %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_traceswitch_F ))
					   break ;

				case 's' : options -> variant.C_sp4co_sql_precompiler.opt_silent_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_silent     = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_silent_F ))
					   break ;

				case 'w' : options -> variant.C_sp4co_sql_precompiler.opt_nowarn_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_nowarn     = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_nowarn_F ))
					   break ;

				case 'i' : options -> variant.C_sp4co_sql_precompiler.opt_lineno_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_lineno     = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_lineno_F ))
					   break ;

				case 'L' : options -> variant.C_sp4co_sql_precompiler.opt_lib_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_lib        = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_lib_F ))
					   break ;

				case 'q' : options -> variant.C_sp4co_sql_precompiler.opt_quote_F = FALSE ;
					   options -> variant.C_sp4co_sql_precompiler.opt_quote_F = '\"' ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_quote      = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_quote_F ))
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_quote        = %c\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_quo_F ))
					   break ;

				case 'p' : options -> variant.C_sp4co_sql_precompiler.opt_decpoint_F = FALSE ;
					   options -> variant.C_sp4co_sql_precompiler.opt_decpoint_F = ',' ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_decpoint   = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_decpoint_F ))
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_decpoint      = %c\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_decpoint_F ))
					   break ;

				case 'z' : options -> variant.C_sp4co_sql_precompiler.opt_tabformat_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_tabformat  = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_tabformat_F ))
					   break ;

				case 'e' : options -> variant.C_sp4co_sql_precompiler.opt_extern_F = 0 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"mk_prec_mode    : op -> comp.prec.opt_extern     = %d\n",
						     options -> variant.C_sp4co_sql_precompiler.opt_extern_F ))
					   break ;


				case 'V' : options -> variant.C_sp4co_sql_precompiler.opt_version_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_version  = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_version_F ))
					   break ;

				case 'h' : options -> variant.C_sp4co_sql_precompiler.opt_help_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"prec_opt_analys : op -> comp.prec.opt_help  = %d\n",
							 options -> variant.C_sp4co_sql_precompiler.opt_help_F ))
					   break ;

				/* ***** all the other arguments ***** */

#ifdef	OLD	/*IST 20.04.94, precompiler don't want to stop at ill option */
				case '?' : mk_args ( '?' , sp4co_sql_precompiler ,
						     user_params ,
						     options , xusertype ,
						     errtext , ok ) ;
					   more_args = FALSE ;
					   break ;
#else	/*OLD*/
				case '?' :
					   *ok = FALSE ;
					   break ;
#endif	/*OLD*/
			}
		}

		/* ***** all the other arguments , o_char = - 1 ***** */

		if ( ! *ok )
		{
		    eo46CtoP ( errtext , "-- illegal precompiler options" ,
							sizeof (tsp00_ErrText) );
		}
		else if ( more_args )
		{
			mk_args ( ' ' , sp4co_sql_precompiler , user_params ,
				  options , xusertype , errtext , ok ) ;
		}
	}

	DBG1 (( MF__,"prec_opt_analys : * ok ( at the end )            = %d\n",
				 * ok            ))
	DBG1 (( MF__,"prec_opt_analys : xusertype [ 0 ]                = %d\n",
				 xusertype [ 0 ] ))
	DBG1 (( MF__,"prec_opt_analys : xusertype [ 1 ]                = %d\n",
				 xusertype [ 1 ] ))
}

/*==========================================================================*/

static	void	pc_rt_opt_analysis ( 
struct tsp4_xuser_record      * user_params ,
tsp00_Pw                        password ,
struct tsp4_args_options      * options ,
tsp4_xuserset                   xusertype ,
tsp00_Bool                    * ok ,
tsp00_ErrText                   errtext )
{
	tsp00_Bool                 more_args ;
	int                     o_char ;
	static	char            envname [] = "SQLOPT" ;
	int                     i , j ;
	int                     len ;
        char                    * sqlopt ; 


	DBG1 (( MF__,"********** pc_rt_opt_analysis called *********\n" ))

	/* ***** initialize internel vars ***** */

	more_args = TRUE ;
	o_char = ' ' ;
	my_argc = 0 ;


	/* ***** get the precompiler runtime options ***** */
	/* ***** and build up an argv-array and a    ***** */
	/* ***** argc-count by reading the env var   ***** */
	/* ***** SQLOPTIONS                          ***** */

	sqlopt = ( char * ) getenv ( envname ) ;

	if ( sqlopt != NULL )
	{
		static	char *opt_string = NULL;
		/* ***** set argv [ 0 ] to something, ***** */
		/* ***** because you need something   ***** */

		len = strlen ( sqlopt ) ;
		if ( opt_string ) FREE_MEM_EO57( opt_string );
                if ( ALLOC_MEM_EO57( (void **)&opt_string, len+1 ) != NO_ERROR_EO001 )
		{
		    *ok = FALSE ;
		    eo46CtoP ( errtext , "insufficient memory" ,
						sizeof (tsp00_ErrText) );
		    return ;
		}
                strcpy ( opt_string, sqlopt ) ;

		my_argv [ 0 ] = "precomp_runtime" ;

		DBG1 (( MF__,"pc_rt_opt_analys: opt_string                     = %s\n",
					 opt_string ))
		DBG1 (( MF__,"pc_rt_opt_analys: my_argv [ 0 ]                  = %s\n",
					 my_argv [ 0 ] ))
		DBG1 (( MF__,"pc_rt_opt_analys: len                            = %d\n",
					 len ))


		/* ***** extract every option and argument ***** */

		for ( i = 0 ; i < len ; i ++ )
		{
			if ( opt_string [ i ] != ' ' )
			{
				my_argc ++ ;

				DBG1 (( MF__,"pc_rt_opt_analys: & opt_string [ * ]             = 0x%08lx\n",
						 (long) &opt_string[i] ))


				/* ***** take over the string ***** */

				my_argv [ my_argc ] = &opt_string[i];


				/* ***** skip string ***** */

				for ( j = i ;
				      ( j < len ) &&
				      ( opt_string [ j ] != ' ' ) ;
				      j ++ )   ;


				/* ***** string end ***** */

				opt_string [ j ] = '\0' ;
				i = j ;

				DBG1 (( MF__,"pc_rt_opt_analys: my_argv [ * ]                  = ->%s<-\n",
							 my_argv [ my_argc ] ))
			}
		}

		my_argc ++ ;

		/* ***** last argv element ***** */

		my_argv [ my_argc ] = "\0" ;

		DBG1 (( MF__,"pc_rt_opt_analys: my_argv [ * ] (last arg elem)  = ->%s<-\n",
					 my_argv [ my_argc ] ))
	}

	DBG1 (( MF__,"pc_rt_opt_analys: my_argc                        = %d\n",
				 my_argc ))

	/* ***** parameter existing ? ***** */

	if ( my_argc > 1 )
	{
		/* ***** get options ***** */
		int sv_optind = optind ;
		int sv_opterr = opterr ;
		optind = 1 ;
		opterr = 0 ;


		/* ***** get options ***** */

		while ( ((o_char = sqlgetopt(my_argc , my_argv ,
			  "OTXNWRZfu:d:n:U:y:S:F:I:t:B:Y:L:x:")) != -1) &&
			more_args && *ok )
		{
			DBG1 (( MF__,"pc_rt_opt_analys: o_char                         = %c\n",
						 o_char ))
			DBG1 (( MF__,"pc_rt_opt_analys: optind                         = %d\n",
						 optind ))
			switch ( o_char )
			{
				/* ***** general optargs ***** */

				case 'u' : mk_user_passwd ( user_params ,
							    password ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_userparms ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'd' : mk_dbid ( user_params ,
						     xusertype ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_serverdb ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'n' : mk_dbnode ( user_params ,
						       xusertype ) ;
					   break ;

				case 'U' : mk_userkey ( user_params ,
							xusertype ,
							errtext , ok ) ;
					   break ;

				case 'S' : mk_sqlmode ( user_params ,
						        xusertype ) ;
					   break ;

				case 'I' : mk_isolation ( user_params ,
						          xusertype ) ;
					   break ;

				case 't' : mk_timeout ( user_params ,
						        xusertype ) ;
					   break ;

				case 'y' : mk_cachelimit ( user_params ,
						           xusertype ) ;
					   break ;

				case 'x' : mk_dblang ( user_params ,
						       xusertype ) ;
					   break ;

				/* ***** optargs with arguments ***** */

				case 'F' : mk_pc_rt_trace_fn ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'B' : mk_pc_rt_mfetch ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'Y' : mk_pc_rt_rstmt_cnt ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'L' : mk_pc_rt_rtime_limit ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;


				/* ***** opargs with no arguments ***** */

				case 'O' : options -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F = 4 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rtrace    = %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F ))
					   break ;

				case 'T' : options -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F = 2 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rtrace    = %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F ))
					   break ;

				case 'X' : options -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F = 3 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rtrace    = %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rtrace_F ))
					   break ;

				case 'R' : options -> variant.C_sp4co_sql_pc_runtime.opt_rprof_F = 1 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rprof     = %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rprof_F ))
					   break ;

				case 'Z' : options -> variant.C_sp4co_sql_pc_runtime.opt_rprof_F = 2 ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rprof     = %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rprof_F ))
					   break ;

				case 'N' : options -> variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rnotraceti= %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F ))
					   break ;

				case 'W' : options -> variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rtraceswit= %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F ))
					   break ;

				case 'f' : options -> variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"pc_rt_opt_analys: op -> comp.pc_rt.opt_rnosdf    = %d\n",
							 options -> variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F ))
					   break ;


				/* ***** all the other arguments ***** */

#ifdef	OLD	/*IST 20.04.94, precompiler don't want to stop at ill option */
				case '?' : mk_args ( '?' , sp4co_sql_pc_runtime ,
						     user_params ,
						     options , xusertype ,
						     errtext , ok ) ;
					   more_args = FALSE ;
					   break ;
#else	/*OLD*/
				case '?' :
					   *ok = FALSE ;
					   break ;
#endif	/*OLD*/
			}
		}

		/* ***** all the other arguments , o_char = - 1 ***** */

		if ( ! *ok )
		{
		    eo46CtoP ( errtext , "-- illegal precompiler options" ,
							sizeof (tsp00_ErrText) );
		}
		else if ( more_args )
		{
			mk_my_args ( ' ' , sp4co_sql_pc_runtime , user_params ,
				     options , xusertype , errtext , ok ) ;
		}
		optind = sv_optind ;
		opterr = sv_opterr ;
	}

	DBG1 (( MF__,"pc_rt_opt_analys: xusertype [ 0 ]                = %d\n",
				 xusertype [ 0 ] ))
	DBG1 (( MF__,"pc_rt_opt_analys: xusertype [ 1 ]                = %d\n",
				 xusertype [ 1 ] ))
}

/*==========================================================================*/

static	void	dcomp_opt_analysis ( 
struct tsp4_xuser_record      * user_params ,
tsp00_Pw                        password ,
struct tsp4_args_options      * options ,
tsp4_xuserset                   xusertype ,
tsp00_Bool                    * ok ,
tsp00_ErrText                   errtext )
{
	tsp00_Bool                 more_args ;
	int                     o_char ;


	DBG1 (( MF__,"********** dcomp_opt_analysis called *********\n" ))

	/* ***** initialize internel vars ***** */

	more_args = TRUE ;
	o_char = ' ' ;

	DBG1 (( MF__,"dcomp_opt_analys: argc                           = %d\n",
				 argc ))

	/* ***** parameter existing ? ***** */

	if ( argc > 1 )
	{
		/* ***** get options ***** */

		while ( ((o_char = sqlgetopt ( argc , argv ,
			  "APVLTcsu:d:n:U:r:b:R:B:O:e:i:S:y:I:t:C:x:")) != -1 ) &&
			more_args && *ok )
		{
			DBG1 (( MF__,"dcomp_opt_analys: o_char                         = %c\n",
						 o_char ))
			DBG1 (( MF__,"dcomp_opt_analys: optind                         = %d\n",
						 optind ))
			switch ( o_char )
			{
				/* ***** general optargs ***** */

				case 'u' : mk_user_passwd ( user_params ,
							    password ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_userparms ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'd' : mk_dbid ( user_params ,
						     xusertype ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_serverdb ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'n' : mk_dbnode ( user_params ,
						       xusertype ) ;
					   break ;

				case 'U' : mk_userkey ( user_params ,
							xusertype ,
							errtext , ok ) ;
					   break ;

				case 'S' : mk_sqlmode ( user_params ,
						        xusertype ) ;
					   break ;

				case 'I' : mk_isolation ( user_params ,
						          xusertype ) ;
					   break ;

				case 't' : mk_timeout ( user_params ,
						        xusertype ) ;
					   break ;

				case 'y' : mk_cachelimit ( user_params ,
						           xusertype ) ;
					   break ;

				case 'x' : mk_dblang ( user_params ,
						       xusertype ) ;
					   break ;

				/* ***** optargs with arguments ***** */

				case 'r' : mk_dcomp_r_fn ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'b' : mk_dcomp_b_fn ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'R' : mk_dcomp_r_obj ( options ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'B' : mk_dcomp_b_obj ( options ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'O' : mk_dcomp_object ( options ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'C' : mk_dcomp_code_type ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'e' : mk_dcomp_export ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'i' : mk_dcomp_import ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;


				/* ***** optargs with no arguments ***** */

				case 'A' : options -> variant.C_sp4co_sql_easy.opt_append_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"dcomp_opt_analys: op -> comp.dcu.opt_append      = %d\n",
							 options -> variant.C_sp4co_sql_easy.opt_append_F ))
					   break ;

				case 'P' : options -> variant.C_sp4co_sql_easy.opt_prompt_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"dcomp_opt_analys: op -> comp.dcu.opt_prompt      = %d\n",
							 options -> variant.C_sp4co_sql_easy.opt_prompt_F ))
					   break ;

				case 'c' : options -> variant.C_sp4co_sql_easy.opt_codeonly_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"dcomp_opt_analys: op -> comp.dcu.opt_codeonly    = %d\n",
							 options -> variant.C_sp4co_sql_easy.opt_codeonly_F ))
					   break ;

				case 'V' : options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_comp_vers;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"dcomp_opt_analys: op -> comp.dcu.opt_comm_mode   = %d\n",
						      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
					   break ;

				case 's' : options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_select ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"dcomp_opt_analys: op -> comp.dcu.opt_comm_mode   = %d\n",
						      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
					   break ;
				case 'T' : options -> variant.C_sp4co_sql_easy.opt_utility_session_F = TRUE ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"dcomp_opt_analys: op -> comp.dcu.opt_utility_session   = %d\n",
						      options -> variant.C_sp4co_sql_easy.opt_utility_session_F ))
					   break ;

				case 'L' : options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_list_command ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"dcomp_opt_analys: op -> comp.dcu.opt_comm_mode   = %d\n",
						      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
					   break ;


				/* ***** all the other arguments ***** */

				case '?' : mk_args ( '?' , sp4co_sql_easy ,
						     user_params ,
						     options , xusertype ,
						     errtext , ok ) ;
					   more_args = FALSE ;
					   break ;
			}
		}

		/* ***** all the other arguments , o_char = - 1 ***** */

		if ( ! *ok )
		{
		    eo46CtoP ( errtext , 
                                 "-- illegal option or missing parameter" ,
							sizeof (tsp00_ErrText) );
		}
		if ( more_args )
		{
			mk_args ( ' ' , sp4co_sql_easy , user_params , options ,
				  xusertype , errtext , ok ) ;
		}
	}

	DBG1 (( MF__,"dcomp_opt_analys: xusertype [ 0 ]                = %d\n",
				 xusertype [ 0 ] ))
	DBG1 (( MF__,"dcomp_opt_analys: xusertype [ 1 ]                = %d\n",
				 xusertype [ 1 ] ))
}

/*==========================================================================*/

static	void	audit_opt_analysis ( 
struct tsp4_xuser_record      * user_params ,
tsp00_Pw                        password ,
struct tsp4_args_options      * options ,
tsp4_xuserset                   xusertype ,
tsp00_Bool                    * ok ,
tsp00_ErrText                   errtext )
{
	tsp00_Bool                 more_args ;
	int                     o_char ;


	DBG1 (( MF__,"********** audit_opt_analysis called *********\n" ))

	/* ***** initialize internel vars ***** */

	more_args = TRUE ;
	o_char = ' ' ;

	DBG1 (( MF__,"audit_opt_analys: argc                           = %d\n",
				 argc ))

	/* ***** parameter existing ? ***** */

	if ( argc > 1 )
	{
		/* ***** get options ***** */

		while ( ((o_char = sqlgetopt( argc , argv ,
			    "Vu:d:n:U:S:I:t:y:c:l:o:x:N:" )) != - 1) &&
			more_args && *ok )
		{
			DBG1 (( MF__,"audit_opt_analys: o_char                         = %c\n",
						 o_char ))
			DBG1 (( MF__,"audit_opt_analys: optind                         = %d\n",
						 optind ))
			switch ( o_char )
			{
				/* ***** general optargs ***** */

				case 'u' : mk_user_passwd ( user_params ,
							    password ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_userparms ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'd' : mk_dbid ( user_params ,
						     xusertype ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_serverdb ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'n' : mk_dbnode ( user_params ,
						       xusertype ) ;
					   break ;

				case 'U' : mk_userkey ( user_params ,
							xusertype ,
							errtext , ok ) ;
					   break ;

				case 'S' : mk_sqlmode ( user_params ,
						        xusertype ) ;
					   break ;

				case 'I' : mk_isolation ( user_params ,
						          xusertype ) ;
					   break ;

				case 't' : mk_timeout ( user_params ,
						        xusertype ) ;
					   break ;

				case 'y' : mk_cachelimit ( user_params ,
						           xusertype ) ;
					   break ;

				case 'x' : mk_dblang ( user_params ,
						       xusertype ) ;
					   break ;

				/* ***** optargs with arguments ***** */

				case 'c' : mk_audit_cntrl ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'l' : mk_audit_logfile ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'N' : mk_audit_log_blocksize ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;

				case 'o' : mk_audit_output ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;


				/* ***** opargs with no arguments ***** */

				case 'V' : options -> variant.C_sp4co_sql_audit.opt_aud_comm_mode_F = sp4cm_sql_comp_vers;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"audit_opt_analys: op -> comp.aud.opt_aud_comm_mod= %d\n",
						      options -> variant.C_sp4co_sql_audit.opt_aud_comm_mode_F ))
					   break ;


				/* ***** all the other arguments ***** */

				case '?' : mk_args ( '?' , sp4co_sql_audit ,
						     user_params ,
						     options , xusertype ,
						     errtext , ok ) ;
					   more_args = FALSE ;
					   break ;
			}
		}

		/* ***** all the other arguments , o_char = - 1 ***** */

		if ( ! *ok )
		{
		    eo46CtoP ( errtext , "-- illegal precompiler options" ,
							sizeof (tsp00_ErrText) );
		}
		if ( more_args )
		{
			mk_args ( ' ' , sp4co_sql_audit , user_params , options ,
				  xusertype , errtext , ok ) ;
		}
	}

	DBG1 (( MF__,"audit_opt_analys: xusertype [ 0 ]                = %d\n",
				 xusertype [ 0 ] ))
	DBG1 (( MF__,"audit_opt_analys: xusertype [ 1 ]                = %d\n",
				 xusertype [ 1 ] ))
}

/*==========================================================================*/

static	void	userx_opt_analysis ( 
struct tsp4_xuser_record        * user_params ,
tsp00_Pw                          password ,
struct tsp4_args_options        * options ,
tsp4_xuserset                     xusertype ,
tsp00_Bool                      * ok ,
tsp00_ErrText                     errtext )
{
	tsp00_Bool                 more_args ;
	int                     o_char ;


	DBG1 (( MF__,"********** userx_opt_analysis called *********\n" ))

	/* ***** initialize internel vars ***** */

	more_args = TRUE ;
	o_char = ' ' ;

	DBG1 (( MF__,"userx_opt_analys: argc                           = %d\n",
				 argc ))

	/* ***** parameter existing ? ***** */

	if ( argc > 1 )
	{
		/* ***** get options ***** */

		while ( ((o_char = sqlgetopt( argc , argv ,
			   "Vu:b:" )) != - 1) &&
			more_args && *ok )
		{
			DBG1 (( MF__,"userx_opt_analys: o_char                         = %c\n",
						 o_char ))
			DBG1 (( MF__,"userx_opt_analys: optind                         = %d\n",
						 optind ))
			switch ( o_char )
			{
				/* ***** general optargs ***** */

				case 'u' : mk_user_passwd ( user_params ,
							    password ,
							    ok , errtext ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_userparms ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				/* wird nicht benoetigt FSt ******************
				/ *"Vu:d:n:U:S:I:t:y:b:" ) ) != - 1 ) &&* /
				case 'd' : mk_dbid ( user_params ,
						     xusertype ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_serverdb ) ;
					   CLRBIT ( xusertype ,
						    sp4xu_sql_usermask ) ;
					   break ;

				case 'n' : mk_dbnode ( user_params ,
						       xusertype ) ;
					   break ;

				case 'U' : mk_userkey ( user_params ,
							xusertype ,
							errtext , ok ) ;
					   break ;

				case 'S' : mk_sqlmode ( user_params ,
						        xusertype ) ;
					   break ;

				case 'I' : mk_isolation ( user_params ,
						          xusertype ) ;
					   break ;

				case 't' : mk_timeout ( user_params ,
						        xusertype ) ;
					   break ;

				case 'y' : mk_cachelimit ( user_params ,
						           xusertype ) ;
					   break ;
				*********************************************/


				/* ***** optargs with arguments ***** */

				case 'b' : mk_userx_b_fn ( options ) ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   break ;


				/* ***** opargs with no arguments ***** */

				case 'V' : options -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_comp_vers ;
					   PUTBIT ( xusertype ,
						    sp4xu_sql_params ) ;
					   DBG1 (( MF__,"userx_opt_analys: op -> comp.ux.opt_ux_comm_mod  = %d\n",
						      options -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F ))
					   break ;


				/* ***** all the other arguments ***** */

				case '?' : mk_args ( '?' , sp4co_sql_userx ,
						     user_params ,
						     options , xusertype ,
						     errtext , ok ) ;
					   more_args = FALSE ;
					   break ;
			}
		}

		/* ***** all the other arguments , o_char = - 1 ***** */

		if ( ! *ok )
		{
		    eo46CtoP ( errtext , "-- illegal precompiler options" ,
							sizeof (tsp00_ErrText) );
		}
		if ( more_args )
		{
			mk_args ( ' ' , sp4co_sql_userx , user_params , options ,
				  xusertype , errtext , ok ) ;
		}
	}

	DBG1 (( MF__,"userx_opt_analys: xusertype [ 0 ]                = %d\n",
				 xusertype [ 0 ] ))
	DBG1 (( MF__,"userx_opt_analys: xusertype [ 1 ]                = %d\n",
				 xusertype [ 1 ] ))
}

/*==========================================================================*/

/*---------------------------------------
  Function: Ascii8Copy
  Description: Copy native UCS2 pascal string into ascii8 copy
  
  Arguments: destAscii8 [out] destination
             srcUCS2 [in] source
             srcLen [in] length of source and destination buffer
  Return value: true if Ascii8 was ok, false if UCS2 is needed (destAscii8 buffer filled with spaces)
 */
static bool Ascii8Copy(unsigned char *destAscii8, const unsigned short *srcUCS2, int srcLen)
{
    int i;

    SAPDB_memset(destAscii8, ' ', srcLen);
    for ( i = 0; (i < srcLen/2) && (srcUCS2[i] != ' ') ; i++ )
    {
        if ( srcUCS2[i] < 256 )
        {
            destAscii8[i] = (unsigned char)srcUCS2[i];
        }
        else
        {
            SAPDB_memset(destAscii8, ' ', srcLen);
            return false;
        }
    }
    return true;
}

/*---------------------------------------------------------------------------*/

static	void	mk_user_passwd ( 
struct tsp4_xuser_record        * user_params ,
tsp00_Pw                          password ,
tsp00_Bool                      * ok ,
tsp00_ErrText                     errtext )
{
    tsp00_Bool                     uid_to_upper ;
    tsp00_Bool                     pw_to_upper ;
    int                         length ;
    char                      * start_1_adr ;
    char                      * start_2_adr ;
    unsigned long               len_1 ;
    unsigned long               len_2 ;
    bool                        bpureAsciiUser = true;
    bool                        bpureAsciiPassw = true;     

    DBG1 (( MF__,"********** mk_user_passwd called *************\n" ))

    /* ***** get userid ( only if it exists ) ***** */

    if ( strlen ( optarg ) )
    {
        tsp00_Uint4 fillLength;
        void *      fillPointer;
	    /* ***** initialize user id, password string ***** */
	    /* ***** and conv_to_upper                   ***** */

	    SAPDB_memset ( user_params -> xu_user       , ' ' ,
					    sizeof ( user_params -> xu_user ) ) ;
	    SAPDB_memset ( password   , ' ' , sizeof ( tsp00_Pw ) ) ;

            fillLength  = sizeof(user_params->xu_userUCS2);
            fillPointer = &user_params->xu_userUCS2[0];
            sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');

	    /* ***** check user id and password ;             ***** */
	    /* ***** maybe you don't need to convert to upper ***** */

	    check_idents ( ',' , 2 , ok , errtext ) ;

	    if ( ! ( * ok ) )
	        return ;

	    /* ***** takeover ident infomations ***** */

	     len_1        = ctu . i_len [ 0 ] ;
	     len_2        = ctu . i_len [ 1 ] ;
	     uid_to_upper = ctu . i_tup [ 0 ] ;
	     pw_to_upper  = ctu . i_tup [ 1 ] ;
	     start_1_adr  = ctu . ident [ 0 ] ;
	     start_2_adr  = ctu . ident [ 1 ] ;



	    /* ***** check length ***** */

	    if ( len_1 > 0 )
	    {
            tsp00_Uint4 bytesWritten;

    	    /* ***** determinate length of user id ***** */

	        length = sizeof(user_params->xu_user) ;

            if ( len_1 > length )
    		    len_1 = length ;

            /* The caller must use sqlSetOptionEncodingUTF8(true) to enable this... */
            /* PTS 1109583 */
            if ( sqlIsOptionEncodingUTF8() )
            {
                tsp00_Uint4 bytesParsed;
                unsigned int byteLength;
                unsigned int charLength;

                if ( sp83UTF8Convert_Success != 
                      sp83UTF8_Bytelen((const tsp00_Byte *)start_1_adr, len_1, &byteLength, &charLength) )
                {
                      eo46CtoP ( errtext,
                                 "username contains non UTF8 character",
                                 sizeof(tsp00_ErrText) );
                      *ok = false;
                      return;
                }

                if ( charLength > length )
                {
                      eo46CtoP ( errtext,
                                 "username too long for UCS2",
                                 sizeof(tsp00_ErrText) );
                      *ok = false;
                      return;
                }

                sp78convertBuffer( sp77encodingUCS2Native,
                                   user_params->xu_userUCS2,
                                   sizeof(user_params->xu_userUCS2),
                                  &bytesWritten,
                                   sp77encodingUTF8,
                                   start_1_adr, 
                                   len_1,
                                  &bytesParsed);

                /* Convert xu_userUCS2 into xu_user */
                bpureAsciiUser = Ascii8Copy((unsigned char *)&user_params->xu_user[0], 
                                            (const unsigned short *)&user_params->xu_userUCS2[0],
                                            bytesWritten );
                if ( !bpureAsciiUser )
                {
                    SAPDB_memset(&user_params->xu_user[0], ' ', length);
                }

                fillLength  = sizeof(tsp00_Pw);
                fillPointer = password;
                sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');

            }
            else
            {
                const unsigned char * srcAscii;
                unsigned short      * dstUCS2;
                tsp00_Int4            sizeInCharacters;
                tsp00_Int4            i;

                bytesWritten = len_1 * 2;
                if ( bytesWritten > length )
                {
                      eo46CtoP ( errtext,
                                 "Username too long for UCS2",
                                 sizeof(tsp00_ErrText) );
                      *ok = false;
                      return;
                }
                dstUCS2 = (unsigned short *)&user_params->xu_userUCS2[0];
                srcAscii = (const unsigned char *)start_1_adr;
                for ( i = 0; i < len_1; i++ )
                {
                  dstUCS2[i] = srcAscii[i];
                }

                SAPDB_memcpy(user_params->xu_user, start_1_adr, len_1);
              }

              /* if necessary convert string to upper */
	          if ( uid_to_upper )
              {
                sp81UCS2StringToupper( &user_params -> xu_userUCS2[0], bytesWritten );
   	            move_toupper ( (char *)&user_params -> xu_user[0] , (int) bytesWritten/2 ) ;
              }

    	      DBG1 (( MF__,"mk_user_passwd  : user_params -> xu_user = %.64s\n",
				        user_params -> xu_user ))

              SAPDB_memset( password, ' ', sizeof(tsp00_Pw) );
	    }


	    /* ***** get password ***** */
	    /* ***** if len_2 > 0 ***** */

	    if ( len_2 > 0 )
        {
            tsp00_Uint4 bytesWritten;
            tsp00_Uint4 bytesParsed;

            unsigned int byteLength;
            unsigned int charLength;

            tsp00_Pw tmpPasswordUCS2;
            tsp00_Pw tmpPasswordAscii;

            /* determinate length of password */
    	    length = sizeof ( tsp00_Pw ) ;

            /* check length */
            if ( len_2 > length )
                len_2 = length;

            /* The caller must use sqlSetOptionEncodingUTF8(true) to enable this... */
            if ( sqlIsOptionEncodingUTF8() )
            {
                if ( sp83UTF8Convert_Success != 
                      sp83UTF8_Bytelen((const tsp00_Byte *)start_2_adr, len_2, &byteLength, &charLength) )
                {
                      eo46CtoP ( errtext,
                                 "Password contains non UTF8 character",
                                 sizeof(tsp00_ErrText) );
                      *ok = false;
                      return;
                }

                if ( charLength > length )
                {
                      eo46CtoP ( errtext,
                                 "password too long for UCS2",
                                 sizeof(tsp00_ErrText) );
                      *ok = false;
                      return;
                }

                sp78convertBuffer( sp77encodingUCS2Native,
                                  &tmpPasswordUCS2[0],
                                   sizeof(tsp00_Pw),
                                  &bytesWritten,
                                   sp77encodingUTF8,
                                   start_2_adr, 
                                   len_2,
                                  &bytesParsed);

                /* Convert xu_userUCS2 into xu_user */
                bpureAsciiPassw = Ascii8Copy((unsigned char *)&tmpPasswordAscii[0], 
                                             (const unsigned short *)&tmpPasswordUCS2[0],
                                             bytesWritten );
                if ( !bpureAsciiPassw || !bpureAsciiUser )
                {
                    fillLength  = sizeof(tsp00_Pw);
                    fillPointer = password;
                    sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');
                    SAPDB_memset(&user_params->xu_user[0], ' ', sizeof(tsp00_KnlIdentifier));
                    SAPDB_memcpy ( password, tmpPasswordUCS2 , bytesWritten );
                    if ( pw_to_upper ) 
                    {
                        sp81UCS2StringToupper( password, bytesWritten );
                    }
                }
                else
                {
                    SAPDB_memset ( password, ' ', sizeof ( tsp00_Pw ) );
                    SAPDB_memcpy ( password, tmpPasswordAscii , bytesWritten/2 );
                    if ( pw_to_upper ) 
                    {
                        move_toupper( password, bytesWritten/2 );
                    }
                }

            }
            else
            {
                SAPDB_memset ( password, ' ', sizeof ( tsp00_Pw ) );
                SAPDB_memcpy ( password, start_2_adr , len_2 );
                if ( pw_to_upper ) 
                {
            		move_toupper ( password , (int) len_2 ) ;
                }
            }
        }

	    DBG1 (( MF__,"mk_user_passwd  : password                       = %.18s\n",
				      password ))
    }
}

/*==========================================================================*/

static	void	mk_dbid ( 
struct tsp4_xuser_record        * user_params ,
tsp4_xuserset                   xusertype )
{
	SQL_NODEIDC     snode ;
	SQL_DBNAMEC     sdb ;
	/* int             len_arg ; */
	/* int             length ; */
	/* int             len ; */
	/* unsigned long   start_adr ; */
	/* unsigned long   comma_adr ; */

	DBG1 (( MF__,"********** mk_dbid called ********************\n" ))

	/* ***** get servernode ( only if it exists ) ***** */

	if ( strlen ( optarg ) )
	{
		/* ***** initialize serverdb string ***** */

		SAPDB_memset ( user_params -> xu_serverdb   , ' ' ,
		       sizeof ( user_params -> xu_serverdb ) ) ;


		/* ***** if nessecary slpit serverdb and servernode ***** */

		SAPDB_memset ( snode , 0 , sizeof(snode) );
		SAPDB_memset ( sdb , 0 , sizeof(sdb) );
		(void) strncpy ( (char *)sdb , (const char *)optarg , sizeof(sdb)-1 );

		DBG1 (( MF__,"mk_dbid         : sdb            ( ori )         = %s\n",
					  sdb ))

		sql03_split_dbname ( sdb , snode ) ;

		DBG1 (( MF__,"mk_dbid         : sdb            ( c )           = %s\n",
					  sdb ))
		DBG1 (( MF__,"mk_dbid         : snode          ( c )           = %s\n",
					  snode ))


		/* ***** take over C-names into pascal vars ***** */

		eo46CtoP ( user_params -> xu_serverdb   , (char *)sdb   ,
			     sizeof(user_params -> xu_serverdb) );

		if ( strlen ( (const char *)snode ) )
		{
			/* ***** initialize servernode string ***** */

			SAPDB_memset ( user_params -> xu_servernode , ' ' ,
			       sizeof ( user_params -> xu_servernode ) ) ;


			/* ***** get servernode ***** */

			eo46CtoP ( user_params -> xu_servernode , (char *)snode ,
				     sizeof(user_params -> xu_servernode) );


			/* ***** get servernode ***** */

			PUTBIT ( xusertype , sp4xu_sql_servernode ) ;
		}

		DBG1 (( MF__,"mk_dbid         : user_params->xu_serverdb (pas) = %.18s\n",
					  user_params -> xu_serverdb ))
		DBG1 (( MF__,"mk_dbid         : user_params->xu_servernode(pas)= %.64s\n",
					  user_params -> xu_servernode ))
	}
}

/*==========================================================================*/

static	void	mk_dbnode ( 
struct tsp4_xuser_record        * user_params ,
tsp4_xuserset                   xusertype )
{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_dbnode called ******************\n" ))

	/* ***** get servernode ***** */

	SAPDB_memset ( user_params -> xu_servernode , ' ' , 
               sizeof ( user_params -> xu_servernode ) ) ;
	len_arg = strlen ( optarg ) ;
	length = sizeof ( user_params -> xu_servernode ) ;

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( user_params -> xu_servernode , optarg , len_arg ) ;

	DBG1 (( MF__,"mk_dbnode       : user_params -> xu_servernode   = %.64s\n",
				  user_params -> xu_servernode ))

        PUTBIT ( xusertype , sp4xu_sql_servernode ) ;
	CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
}


/*==========================================================================*/

static	void	mk_dblang ( 
struct tsp4_xuser_record        * user_params ,
tsp4_xuserset                   xusertype )
{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_dblang called ******************\n" ))

	/* ***** get dblang ***** */

	length = sizeof ( user_params -> xu_dblang ) ;
	SAPDB_memset ( user_params -> xu_dblang , ' ' , length ) ;
	len_arg = strlen ( optarg ) ;

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( user_params -> xu_dblang , optarg , len_arg ) ;

	DBG1 (( MF__,"mk_dblang       : user_params -> xu_dblang   = %.18s\n",
				  user_params -> xu_dblang ))

        PUTBIT ( xusertype , sp4xu_sql_dblang ) ;
	CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
}


/*==========================================================================*/

static	void	mk_userkey ( 
struct tsp4_xuser_record      * user_params ,
tsp4_xuserset                   xusertype ,
tsp00_ErrText                   errtext,   /* PTS 1104907 */
tsp00_Bool                    * ok)        /* PTS 1104907 */
{
	int                     length ;
	int                     len_arg ;

	DBG1 (( MF__,"********** mk_userkey called *****************\n" ))

	/* ***** get userkey and xusertype ***** */

	length  = sizeof ( user_params -> xu_key ) ;
	len_arg = strlen ( optarg ) ;

	if ( len_arg > length )
		len_arg = length ;


	if ( ( strcmp ( optarg , "prompt" ) == 0 )  ||
	     ( strcmp ( optarg , "PROMPT" ) == 0 )    )
	{
		PUTBIT ( xusertype , sp4xu_sql_usermask ) ;
	}
	else
	{
		/* ***** check for error conditions ***** */

		if ( TSTBIT ( xusertype , sp4xu_sql_userparms  )  |
		     TSTBIT ( xusertype , sp4xu_sql_servernode )  |
		     TSTBIT ( xusertype , sp4xu_sql_mode       )  |
		     TSTBIT ( xusertype , sp4xu_sql_isolation  )  |
		     TSTBIT ( xusertype , sp4xu_sql_timeout    )  |
		     TSTBIT ( xusertype , sp4xu_sql_cachelimit )     )
		{
		     * ok = FALSE ;
		     SAPDB_memcpy ( errtext , wrong_option_sequence , sizeof ( tsp00_ErrText ));
		     return ;
		}

		if ( ( ! env_dbname_found )                  &&
		     ( TSTBIT ( xusertype , sp4xu_sql_serverdb ) )   )
		{
			* ok = FALSE ;
			SAPDB_memcpy ( errtext , wrong_option_sequence ,
							     sizeof ( tsp00_ErrText ));
			return ;
		}

		/* ***** copy without '\0' ***** */

		if ( sql13c_only_blanks ( optarg ) )
		{
			SAPDB_memset ( user_params -> xu_key , ' ' ,
			       sizeof ( user_params -> xu_key ) ) ;
			SAPDB_memcpy ( user_params -> xu_key , DEFAULT_USER ,
			       sizeof ( user_params -> xu_key ) ) ;
			PUTBIT ( xusertype , sp4xu_sql_userdefault ) ;
			CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
		}
		else
		{
			SAPDB_memset ( user_params -> xu_key , ' ' ,
			       sizeof ( user_params -> xu_key ) ) ;
			SAPDB_memcpy ( user_params -> xu_key , optarg , len_arg ) ;

			PUTBIT ( xusertype , sp4xu_sql_userkey ) ;
			CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
			CLRBIT ( xusertype , sp4xu_sql_userdefault ) ;
		}


		/* ***** calling sqlgetuser because of a  ***** */
		/* ***** specified user key               ***** */

		sqlgetuser ( user_params , NULL, errtext , ok ) ;

		DBG1 (( MF__,"mk_userkey      : user_params -> xu_key          = %.18s\n",
					  user_params -> xu_key ))
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_servernode   = %.64s\n",
					  user_params -> xu_servernode ))
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_serverdb     = %.18s\n",
					  user_params -> xu_serverdb ))
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_user         = %.64s\n",
					  user_params -> xu_user ))
		/*
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_password     = %24x\n",
					  user_params -> xu_password ))
		*/
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_sqlmode      = %.8s\n",
					  user_params -> xu_sqlmode ))
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_cachelimit   = %ld\n",
					  user_params -> xu_cachelimit ))
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_timeout      = %d\n",
					  user_params -> xu_timeout ))
		DBG1 (( MF__,"mk_userkey      : user_params -> xu_isolation    = %d\n",
					  user_params -> xu_isolation ))
	}

	DBG1 (( MF__,"mk_userkey      : xusertype [ 0 ]                = %d\n",
				 xusertype [ 0 ] ))
	DBG1 (( MF__,"mk_userkey      : xusertype [ 1 ]                = %d\n",
				 xusertype [ 1 ] ))
}

/*==========================================================================*/

static	void	mk_sqlmode ( 
struct tsp4_xuser_record        * user_params ,
tsp4_xuserset                   xusertype )
{
	int                     length ;
	int                     len_arg ;

	DBG1 (( MF__,"********** mk_sqlmode called *****************\n" ))

	/* ***** get sqlmode ***** */

	SAPDB_memset ( user_params -> xu_sqlmode , ' ' , 
               sizeof ( user_params -> xu_sqlmode ) ) ;
	length  = sizeof ( user_params -> xu_sqlmode ) ;
	len_arg = strlen ( optarg ) ;

	if ( len_arg > length )
		len_arg = length ;


	/* ***** copy without '\0' ***** */

	SAPDB_memcpy ( user_params -> xu_sqlmode   , optarg , len_arg ) ;

	DBG1 (( MF__,"mk_sqlmode      : user_params -> xu_sqlmode      = %.8s\n",
				 user_params -> xu_sqlmode ))
        PUTBIT ( xusertype , sp4xu_sql_mode ) ;
        CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
}

/*==========================================================================*/

static	void	mk_isolation ( 
struct tsp4_xuser_record        * user_params ,
tsp4_xuserset                   xusertype )
{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_isolation called ***************\n" ))

	/* ***** get isolation level and test for the digits  ***** */
	/* ***** 0 through 9                                  ***** */

	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
		user_params -> xu_isolation = atoi ( optarg );

	DBG1 (( MF__,"mk_isolation    : user_params -> xu_isolation    = %d\n",
				  user_params -> xu_isolation ))
        PUTBIT ( xusertype , sp4xu_sql_isolation ) ;
        CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
}

/*==========================================================================*/

static	void	mk_timeout ( 
struct tsp4_xuser_record        * user_params ,
tsp4_xuserset                   xusertype )
{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_timeout called *****************\n" ))

	/* ***** get timeout and test for the digits 0 through 9 ***** */

	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
		user_params -> xu_timeout = atoi ( optarg ) ;

	DBG1 (( MF__,"mk_timeout      : user_params -> xu_timeout      = %d\n",
				  user_params -> xu_timeout ))
        PUTBIT ( xusertype , sp4xu_sql_timeout ) ;
        CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
}

/*==========================================================================*/

static	void	mk_cachelimit ( 
struct tsp4_xuser_record        * user_params ,
tsp4_xuserset                   xusertype )
{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_cachelimit called **************\n" ))

	/* ***** get timeout and test for the digits 0 through 9 ***** */

	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
		user_params -> xu_cachelimit = atoi ( optarg ) ;

	DBG1 (( MF__,"mk_cachelimit   : user_params -> xu_cachelimit   = %ld\n",
				  user_params -> xu_cachelimit ))
        PUTBIT ( xusertype , sp4xu_sql_cachelimit ) ;
        CLRBIT ( xusertype , sp4xu_sql_usermask ) ;
}

/*==========================================================================*/

static	void	mk_args ( 
int                             o_char ,
tsp4_component                  component ,
struct tsp4_xuser_record      * user_params ,
struct tsp4_args_options      * options ,
tsp4_xuserset                   xusertype ,
tsp00_ErrText                   errtext ,
tsp00_Bool                    * ok )
{
    int                         test_char_0 ;
    int                         test_char_1 ;
    int                         len ;
    int                         argn ;
    int                         out_ind  ;
    int                         size ;
    char                        * argument_string ;
    tsp00_Bool                     arg_line_exists = FALSE ;

    DBG1 (( MF__,"********** mk_args called ********************\n" ))

    if ( o_char == '?' )
    {
	/* ***** look for option names ***** */
	/* ***** if found take over    ***** */

	DBG1 (( MF__,"mk_args         : optind  ( o_char = ? )         = %d\n",
				  optind ))

	test_char_0 = argv [ optind - 1 ] [ 0 ] ;
	test_char_1 = argv [ optind - 1 ] [ 1 ] ;

	DBG1 (( MF__,"mk_args         : argv [ optind - 1 ]            = %s\n",
				  argv [ optind - 1 ]  ))
	DBG1 (( MF__,"mk_args         : test_char_0                    = %c\n",
				  test_char_0 ))
	DBG1 (( MF__,"mk_args         : test_char_1                    = %c\n",
				  test_char_1 ))

	if ( ( test_char_0 == '-' ) &&
	     ( test_char_1 == 'r' || test_char_1 == 'b' ||
	       test_char_1 == 'R' || test_char_1 == 'B'     ) )
	{
	    /* ***** get command           ***** */
	    /* ***** set runfile to "STDIN ***** */

	    switch ( component )
	    {
		case sp4co_sql_easy    :
		case sp4co_sql_query   :
		case sp4co_sql_dialog  :
		case sp4co_sql_appl    :
		case sp4co_sql_load    :
		case sp4co_sql_param   :
		case sp4co_sql_util    :
		    switch ( test_char_1 )
		    {
			case 'r' :
			    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run ;
			    PUTBIT ( xusertype , sp4xu_sql_params ) ;
			    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F ,
				     "STDIN" , 5 ) ;
			    break ;
			case 'b' :
			    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch ;
			    PUTBIT ( xusertype , sp4xu_sql_params ) ;
			    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F ,
				     "STDIN" , 5 ) ;
			    break ;
			case 'R' :
			    SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F,
				   ' ' ,
				   sizeof (options->variant.C_sp4co_sql_easy.opt_object_owner_F));

			    SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F,
				   ' ' ,
				   sizeof ( options->variant.C_sp4co_sql_easy.opt_object_name1_F));

			    SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F,
				   ' ' ,
				   sizeof ( options->variant.C_sp4co_sql_easy.opt_object_name2_F));
			    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run_object ;
			    PUTBIT ( xusertype , sp4xu_sql_params ) ;
			    break ;
			case 'B' :
			    SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F,
				   ' ' ,
				   sizeof (options->variant.C_sp4co_sql_easy.opt_object_owner_F));

			    SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F,
				   ' ' ,
				   sizeof ( options->variant.C_sp4co_sql_easy.opt_object_name1_F));

			    SAPDB_memset ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F,
				   ' ' ,
				   sizeof ( options->variant.C_sp4co_sql_easy.opt_object_name2_F));
			    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch_object ;
			    PUTBIT ( xusertype , sp4xu_sql_params ) ;
			    break ;
		    }

		    DBG1 (( MF__,"mk_args         : op -> comp.dcu.opt_obj_owner   = %.18s\n",
				  options -> variant.C_sp4co_sql_easy.opt_object_owner_F ))
		    DBG1 (( MF__,"mk_args         : op -> comp.dcu.opt_obj_name1   = %.18s\n",
				  options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
		    DBG1 (( MF__,"mk_args         : op -> comp.dcu.opt_obj_name2   = %.18s\n",
				  options -> variant.C_sp4co_sql_easy.opt_object_name2_F ))
		    DBG1 (( MF__,"mk_args         : options->co.dcut.opt_comm_mode = %d\n",
			 options -> variant.C_sp4co_sql_easy.opt_comm_mode_F )) ;
		    DBG1 (( MF__,"mk_args         : options->co.dcut.opt_runfile   = %.64s\n",
			 options -> variant.C_sp4co_sql_easy.opt_runfile_F )) ;
		    break ;

		case sp4co_sql_userx   :
		    switch ( test_char_1 )
		    {
			case 'b' :
			    options -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_batch ;
			    PUTBIT ( xusertype , sp4xu_sql_params ) ;
			    SAPDB_memcpy ( options -> variant.C_sp4co_sql_userx.opt_ux_runfile_F ,
				     "STDIN" , 5 ) ;
			    break ;
		    }
		    DBG1 (( MF__,"mk_args         : options->co.ux.opt_ux_runfile  = %.64s\n",
			 options -> variant.C_sp4co_sql_userx.opt_ux_runfile_F )) ;
		    break ;
	    }

	    return ;
	}
	else
	{
	    /* ***** may be there was a 'U' ***** */

	    if (( test_char_0 == '-' ) && ( test_char_1 == 'U' ))
	    {
		/* ***** check for error conditions ***** */

		if ( TSTBIT ( xusertype , sp4xu_sql_userparms  ) |
		     TSTBIT ( xusertype , sp4xu_sql_servernode ) |
		     TSTBIT ( xusertype , sp4xu_sql_mode       ) |
		     TSTBIT ( xusertype , sp4xu_sql_isolation  ) |
		     TSTBIT ( xusertype , sp4xu_sql_timeout    ) |
		     TSTBIT ( xusertype , sp4xu_sql_cachelimit )   )
		{
		     * ok = FALSE ;
		     SAPDB_memcpy ( errtext , wrong_option_sequence ,
					     sizeof ( tsp00_ErrText ) );
		     return ;
		}

		if ( ( ! env_dbname_found )                &&
		     ( TSTBIT ( xusertype , sp4xu_sql_serverdb ) ) )
		{
		     * ok = FALSE ;
		     SAPDB_memcpy ( errtext , wrong_option_sequence , sizeof ( tsp00_ErrText ));
		     return ;
		}

		PUTBIT ( xusertype , sp4xu_sql_userdefault ) ;
		CLRBIT ( xusertype , sp4xu_sql_usermask ) ;

		SAPDB_memcpy ( user_params -> xu_key , DEFAULT_USER ,
		       sizeof ( user_params -> xu_key ) ) ;
		sqlgetuser ( user_params , NULL, errtext , ok ) ;

		DBG1 (( MF__,"mk_args         : xusertype [ 0 ]                = %d\n",
					  xusertype [ 0 ] ))
		DBG1 (( MF__,"mk_args         : xusertype [ 1 ]                = %d\n",
					  xusertype [ 1 ] ))
		DBG1 (( MF__,"mk_args         : user_params -> xu_key          = %.18s\n",
					  user_params -> xu_key ))
		DBG1 (( MF__,"mk_args         : user_params -> xu_servernode   = %.64s\n",
					  user_params -> xu_servernode ))
		DBG1 (( MF__,"mk_args         : user_params -> xu_serverdb     = %.18s\n",
					  user_params -> xu_serverdb ))
		DBG1 (( MF__,"mk_args         : user_params -> xu_user         = %.64ss\n",
					  user_params -> xu_user ))
		/*
		DBG1 (( MF__,"mk_args         : user_params -> xu_password     = %24x\n",
					  user_params -> xu_password ))
		*/
		DBG1 (( MF__,"mk_args         : user_params -> xu_sqlmode      = %.8s\n",
					  user_params -> xu_sqlmode ))
		DBG1 (( MF__,"mk_args         : user_params -> xu_cachelimit   = %ld\n",
					  user_params -> xu_cachelimit ))
		DBG1 (( MF__,"mk_args         : user_params -> xu_timeout      = %d\n",
					  user_params -> xu_timeout ))
		DBG1 (( MF__,"mk_args         : user_params -> xu_isolation    = %d\n",
					  user_params -> xu_isolation ))
		return ;
	    }
	    else
	    {
		/* ***** no regular option, reduce   ***** */
		/* ***** optind but be sure to do    ***** */
		/* ***** this only you have found an ***** */
		/* ***** '-?' expression             ***** */
		/* ***** ? : means only one char     ***** */

		if ( ( optind > 1 ) &&
		     ( strlen ( argv [ optind - 1 ] ) == 2 ) &&
		     ( test_char_0 == '-' ) )
			optind -- ;
	    }
	}
    }


    /* ***** take over args into arg line; if an arg line exists ***** */
    /* ***** but be sure you have an argument                    ***** */

    if ( argc > optind )
    {

	DBG1 (( MF__,"mk_args         : component                      = %d\n",
				  component ))
	DBG1 (( MF__,"mk_args         : argc                           = %d\n",
				  argc   ))
	DBG1 (( MF__,"mk_args         : optind                         = %d\n",
				  optind ))

	switch ( component )
	{
	    case sp4co_sql_precompiler  :
		if ( options->variant.C_sp4co_sql_precompiler.opt_modulefn_F[0] == ' ' )
		{
		    optarg = argv[optind++];
		    mk_prec_input_fn ( options );
		    PUTBIT ( xusertype , sp4xu_sql_params ) ;
		}
/*
		size = sizeof ( options -> components . prec .
							    opt_ruser_args ) ;
		argument_string = options -> components . prec .
							      opt_ruser_args ;
		arg_line_exists = TRUE ;
*/
		break ;

		/* *ok = FALSE ; */
		/* SAPDB_memcpy ( errtext , no_valid_option , sizeof(tsp00_ErrText) ); */
		/* return ; */

	    case sp4co_sql_pc_runtime   :
		size = sizeof ( options -> variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F ) ;
		argument_string = (char *)options -> variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F ;
		arg_line_exists = TRUE ;
		break ;

	    case sp4co_sql_easy    :
	    case sp4co_sql_query   :
	    case sp4co_sql_dialog  :
	    case sp4co_sql_appl    :
	    case sp4co_sql_load    :
	    case sp4co_sql_util    :
		size = sizeof ( options -> variant.C_sp4co_sql_easy.opt_parameter_F ) ;
		argument_string = (char *)options -> variant.C_sp4co_sql_easy.opt_parameter_F ;
		arg_line_exists = TRUE ;
		break ;

	    default :
		arg_line_exists = FALSE ;
		break ;
	}

	if ( arg_line_exists )
	{
	    out_ind = 0 ;

	    for ( argn = optind ; argn < argc ; argn ++ )
	    {
		len = strlen ( argv [ argn ] ) ;

		DBG1 (( MF__,"mk_args         : len                            = %d\n",
					  len ))

		if ( len > 0 )
		{
		    PUTBIT ( xusertype , sp4xu_sql_params ) ;
		}

		if ( out_ind + len  <= size )
		{
		    SAPDB_memcpy ( argument_string + out_ind , argv [ argn ] , len ) ;
		    out_ind += len + 1 ;

		}
		else
		    break ;
	    }

	    DBG1 (( MF__,"mk_args         : argument_string                = %.132s\n",
				     argument_string ))
	}
    }
}

/*==========================================================================*/

static	void	mk_my_args ( 
int                             o_char ,
tsp4_component                  component ,
struct tsp4_xuser_record      * user_params ,
struct tsp4_args_options      * options ,
tsp4_xuserset                   xusertype ,
tsp00_ErrText                   errtext ,
tsp00_Bool                    * ok )
{
	int                     test_char_0 ;
	int                     test_char_1 ;
	/* FSt
	int                     len ;
	int                     argn ;
	int                     out_ind  ;
	int                     size ;
	char                    * argument_string ;
	tsp00_Bool                 arg_line_exists = FALSE ;
	*/

	DBG1 (( MF__,"********** mk_my_args called **************************\n" ))
	DBG1 (( MF__,"mk_my_args      : o_char                         = '%c'\n",
				  o_char ))
	DBG1 (( MF__,"mk_my_args      : optind                         = %d\n",
				  optind ))
	DBG1 (( MF__,"mk_my_args      : my_argc                        = %d\n",
				  my_argc ))

	if ( o_char == '?' )
	{
		/* ***** look for option names ***** */
		/* ***** if found take over    ***** */

		DBG1 (( MF__,"mk_my_args      : optind  ( o_char = ? )         = %d\n",
					  optind ))

		test_char_0 = my_argv [ optind - 1 ] [ 0 ] ;
		test_char_1 = my_argv [ optind - 1 ] [ 1 ] ;

		DBG1 (( MF__,"mk_my_args      : my_argv [ optind - 1 ]         = %s\n",
					  my_argv [ optind - 1 ]  ))
		DBG1 (( MF__,"mk_my_args      : test_char_0                    = %c\n",
					  test_char_0 ))
		DBG1 (( MF__,"mk_my_args      : test_char_1                    = %c\n",
					  test_char_1 ))

		if ( ( test_char_0 == '-' ) &&
		     ( test_char_1 == 'r' || test_char_1 == 'b' ) )
		{
			/* ***** get command           ***** */
			/* ***** set runfile to "STDIN ***** */

			switch ( test_char_1 )
			{
				case 'r' :
					options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run ;
					PUTBIT ( xusertype , sp4xu_sql_params ) ;
					SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F ,
						 "STDIN" , 5 ) ;
					break ;
				case 'b' :
					options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch ;
					PUTBIT ( xusertype , sp4xu_sql_params ) ;
					SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F ,
						 "STDIN" , 5 ) ;
					break ;
			}

			DBG1 (( MF__,"mk_my_args      : options->co.dcut.opt_comm_mode = %d\n",
			     options -> variant.C_sp4co_sql_easy.opt_comm_mode_F )) ;
			DBG1 (( MF__,"mk_my_args      : options->co.dcut.opt_runfile   = %.64s\n",
			     options -> variant.C_sp4co_sql_easy.opt_runfile_F )) ;

			return ;
		}
		else
		{
			/* ***** may be there was a 'U' ***** */

			if (( test_char_0 == '-' ) && ( test_char_1 == 'U' ))
			{
				/* ***** check for error conditions ***** */

			if ( TSTBIT ( xusertype , sp4xu_sql_userparms  ) |
			     TSTBIT ( xusertype , sp4xu_sql_servernode ) |
			     TSTBIT ( xusertype , sp4xu_sql_mode       ) |
			     TSTBIT ( xusertype , sp4xu_sql_isolation  ) |
			     TSTBIT ( xusertype , sp4xu_sql_timeout    ) |
			     TSTBIT ( xusertype , sp4xu_sql_cachelimit )   )
				{
				     * ok = FALSE ;
				     SAPDB_memcpy ( errtext , wrong_option_sequence ,
							      sizeof ( tsp00_ErrText ));
				     return ;
				}

			if ( ( ! env_dbname_found )                &&
			     ( TSTBIT ( xusertype , sp4xu_sql_serverdb ) ) )
				{
				     * ok = FALSE ;
				     SAPDB_memcpy ( errtext , wrong_option_sequence ,
							      sizeof ( tsp00_ErrText ));
				     return ;
				}

				PUTBIT ( xusertype , sp4xu_sql_userdefault ) ;
				CLRBIT ( xusertype , sp4xu_sql_usermask ) ;

				SAPDB_memcpy ( user_params -> xu_key , DEFAULT_USER ,
				       sizeof ( user_params -> xu_key ) ) ;
				sqlgetuser ( user_params , NULL, errtext , ok ) ;

				DBG1 (( MF__,"mk_my_args      : xusertype [ 0 ]                = %d\n",
							  xusertype [ 0 ] ))
				DBG1 (( MF__,"mk_my_args      : xusertype [ 1 ]                = %d\n",
							  xusertype [ 1 ] ))
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_key          = %.18s\n",
							  user_params -> xu_key ))
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_servernode   = %.64s\n",
							  user_params -> xu_servernode ))
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_serverdb     = %.18s\n",
							  user_params -> xu_serverdb ))
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_user         = %.64s\n",
							  user_params -> xu_user ))
				/*
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_password     = %24x\n",
							  user_params -> xu_password ))
				*/
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_sqlmode      = %.8s\n",
							  user_params -> xu_sqlmode ))
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_cachelimit   = %ld\n",
							  user_params -> xu_cachelimit ))
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_timeout      = %d\n",
							  user_params -> xu_timeout ))
				DBG1 (( MF__,"mk_my_args      : user_params -> xu_isolation    = %d\n",
							  user_params -> xu_isolation ))
				return ;
			}
			else
			{
				/* ***** no regular option, reduce   ***** */
				/* ***** optind but be sure to do    ***** */
				/* ***** this only you have found an ***** */
				/* ***** '-?' expression             ***** */
				/* ***** ? : means only one char     ***** */

				if ( ( optind > 1 ) &&
				     ( strlen ( my_argv [ optind - 1 ] ) == 2 ) &&
				     ( test_char_0 == '-' ) )
					optind -- ;
			}
		}
	}


	/* ***** take over args into arg line; if an arg line exists ***** */
	/* ***** but be sure you have an argument                    ***** */

	if ( my_argc > optind )
	{
		DBG1 (( MF__,"mk_my_args      : component                      = %d\n",
					  component ))
		DBG1 (( MF__,"mk_my_args      : optind                         = %d\n",
					  optind ))

		switch ( component )
		{
			case sp4co_sql_pc_runtime :
				* ok = FALSE ;
				SAPDB_memcpy ( errtext , no_valid_option , sizeof ( tsp00_ErrText ) ) ;
				return ;
			default :
				/* FSt arg_line_exists = FALSE ; */
				break ;
		}

		/* FSt
		if ( arg_line_exists )
		{
			out_ind = 0 ;

			for ( argn = optind ; argn < my_argc ; argn ++ )
			{
				len = strlen ( my_argv [ argn ] ) ;

				if ( out_ind + len  <= size )
				{
					SAPDB_memcpy ( argument_string + out_ind ,
						 my_argv [ argn ] , len ) ;
					out_ind += len + 1 ;

				}
				else
					break ;
			}

			DBG1 (( MF__,"mk_my_args      : argument_string                = %.132s\n",
						 argument_string ))
		}
		*/
	}
}

/*==========================================================================*/

static	void	mk_prec_input_fn ( 
struct tsp4_args_options        * options )
{
	int                     len_arg ;
	int                     length ;
	int                     cnt ;
	char                    *beg ;
	char                    *end ;

	DBG1 (( MF__,"********** mk_prec_input_fn called ***********\n" ))

	/* ***** get file name ***** */

	len_arg = strlen ( optarg ) ;
	length  = sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_modulefn_F );

	if ( len_arg > length )
		len_arg = length ;

	options -> variant.C_sp4co_sql_precompiler.opt_fnind_F = (tsp00_Int2) (optind - 1);
	SAPDB_memcpy ( options -> variant.C_sp4co_sql_precompiler.opt_modulefn_F ,
						 optarg , len_arg ) ;


	/* ***** extract file name                 ***** */
	/* ***** that means: no path and no suffix ***** */
	/* ***** ( path/modulename.suffix )        ***** */

	beg = strrchr ( optarg , '/' ) ;
	end = strrchr ( optarg , '.' ) ;

	if ( ! beg )
		beg = optarg ;
	else
		beg ++ ;

			/*IST 25.04.94, path may be ../name */
	if ( ! end || (end < beg) )
		end = optarg + strlen ( optarg ) ;

	length =
	      sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_modulename_F );
	cnt = end - beg ;

	if ( cnt > length )
		cnt = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_precompiler.opt_modulename_F , beg , cnt ) ;

	options -> variant.C_sp4co_sql_precompiler.opt_modulenamel_F = (tsp00_Int2) cnt ;

	DBG1 (( MF__,"mk_prec_input_fn: op -> comp.prec.opt_modulefn   = %.64s\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_modulefn_F ))
	DBG1 (( MF__,"mk_prec_input_fn: op -> comp.prec.opt_modulename = %.18s\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_modulename_F ))
	DBG1 (( MF__,"mk_prec_input_fn: op -> comp.prec.opt_modulenamel= %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_modulenamel_F ))
}

/*==========================================================================*/

static	void	mk_prec_trace_fn ( 
struct tsp4_args_options        * options )
{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_prec_trace_fn called ***********\n" ))

	/* ***** get trace name ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_tracefile_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_precompiler.opt_tracefile_F , optarg ,
		 len_arg ) ;

	DBG1 (( MF__,"mk_prec_trace_fn: op -> comp.prec.opt_tracefile  = %.64s\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_tracefile_F ))
}

/*==========================================================================*/

static	void	mk_prec_prog_fn ( 
struct tsp4_args_options        * options )
{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_prec_prog_fn called ************\n" ))

	/* ***** get prog name and length of prog name ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_progname_F );

	if ( len_arg > length )
		len_arg = length ;

	options -> variant.C_sp4co_sql_precompiler.opt_prognamel_F = (tsp00_Int2) len_arg ;
	SAPDB_memcpy ( options -> variant.C_sp4co_sql_precompiler.opt_progname_F , optarg ,
		 len_arg ) ;

	DBG1 (( MF__,"mk_prec_progname: op -> comp.prec.opt_prognamel  = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_prognamel_F ))
	DBG1 (( MF__,"mk_prec_progname: op -> comp.prec.opt_progname   = %.18s\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_progname_F ))
}

/*==========================================================================*/

static	void	mk_prec_margin  ( 
struct tsp4_args_options        * options )
{
	tsp00_Bool                 endmar_found ;
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     len ;
	int                     len_first ;
	int                     i ;
	char *                  start_adr ;
	char *                  comma_adr ;
	tsp00_C4                  tmp_margin ;

	DBG1 (( MF__,"********** mk_prec_margin called *************\n" ))

	/* ***** get begmar                         ***** */
	/* ***** determinate comma position, the    ****** */
	/* ***** start pos of optarg and the begmar ****** */
	/* ***** string length                      ****** */

	endmar_found = TRUE ;
	len_arg = strlen ( optarg ) ;
	comma_adr = strchr ( optarg , ',' ) ;
	start_adr = optarg ;
	len = comma_adr - start_adr ;


	/* ***** check length ***** */

	if ( comma_adr == ( unsigned long ) NULL )
	{
		endmar_found = FALSE ;
		len = len_arg ;
	}


	/* ***** convert string to int and take over it ***** */

	len_first = len + 1 ;
	no_alpha_found = TRUE ;
	SAPDB_memset ( tmp_margin , ' ' , sizeof ( tmp_margin ) ) ;

	for ( i = 0 ; ( i < len ) && ( i < sizeof ( tmp_margin ) ) ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}
		else
			tmp_margin [ i ] = optarg [ i ] ;

	if ( no_alpha_found )
		options -> variant.C_sp4co_sql_precompiler.opt_begmar_F =
							 atoi ( (char *)tmp_margin ) ;


	/* ***** get endmar ',' ***** */
	/* ***** was found      ***** */

	if ( endmar_found )
	{
		/* ***** get the beginning of endmar and ***** */
		/* ***** determinate length of endmar    ***** */

		comma_adr ++ ;
		len = start_adr + len_arg - comma_adr ;


		/* ***** convert string to int and take over it ***** */

		no_alpha_found = TRUE ;
		SAPDB_memset ( tmp_margin , ' ' , sizeof ( tmp_margin ) ) ;

		for ( i = 0 ; ( i < len ) && ( i < sizeof ( tmp_margin ) ) ;
									i ++ )
			if ( isalpha ( optarg [ i + len_first ] ) )
			{
				no_alpha_found = FALSE ;
				break ;
			}
			else
				tmp_margin [ i ] = optarg [ i + len_first ] ;

		if ( no_alpha_found )
			options -> variant.C_sp4co_sql_precompiler.opt_endmar_F =
							 atoi ( (char *)tmp_margin ) ;

	}

	DBG1 (( MF__,"mk_prec_margin  : op -> comp.prec.opt_begmar     = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_begmar_F    ))

	DBG1 (( MF__,"mk_prec_margin  : op -> comp.prec.opt_endmar     = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_endmar_F    ))
}

/*==========================================================================*/

static	void	mk_prec_datetime ( 
struct tsp4_args_options        * options )
{
	DBG1 (( MF__,"********** mk_prec_datetime called ***********\n" ))

	/* ***** get date type ***** */

	if ( strcmp ( optarg , "iso" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 2 ;

	if ( strcmp ( optarg , "usa" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 3 ;

	if ( strcmp ( optarg , "eur" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 4 ;

	if ( strcmp ( optarg , "jis" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_datetime_F = 5 ;

	DBG1 (( MF__,"mk_prec_datetime: op -> comp.prec.opt_datetime   = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_datetime_F ))
}

/*==========================================================================*/

static	void	mk_prec_sys_tpmon ( 
struct tsp4_args_options        * options )
{
	tsp00_Bool                 tpm_found ;
	int                     len_arg ;
	int                     length ;
	int                     len ;
	char *                  start_adr ;
	char *                  comma_adr ;
	char                    tmp_sys [ 9 ] ;

	DBG1 (( MF__,"********** mk_prec_sys_tpmon called **********\n" ))

	/* ***** get sys                      ***** */
	/* ***** determinate comma position,  ****** */
	/* ***** the start position of optarg ****** */
	/* ***** and the sys string length    ****** */

	tpm_found = TRUE ;
	len_arg = strlen ( optarg ) ;
	comma_adr = strchr ( optarg , ',' ) ;
	start_adr = optarg ;
	len = comma_adr - start_adr ;


	/* ***** check length ***** */

	if ( comma_adr == ( unsigned long ) NULL )
	{
		tpm_found = FALSE ;
		len = len_arg ;
	}


	/* ***** copy with '\0' ***** */

	if ( len < sizeof ( tmp_sys ) )
	{
		(void) strncpy ( tmp_sys , optarg , ( size_t ) len ) ;
		tmp_sys [ len ] = '\0' ;
	}
	else
	{
		(void) strncpy ( tmp_sys , optarg , sizeof ( tmp_sys ) - 1 ) ;
		tmp_sys [ sizeof ( tmp_sys ) - 1 ] = '\0' ;
	}

	DBG1 (( MF__,"mk_prec_sys_tpm : tmp_sys                        = %s\n",
				  tmp_sys ))


	/* ***** determinate tpmon ***** */

	if ( strcmp ( tmp_sys , "cics" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 1 ;

	if ( strcmp ( tmp_sys , "pccics" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 2 ;

	if ( strcmp ( tmp_sys , "aim" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 3 ;

	if ( strcmp ( tmp_sys , "utm" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_tpmon_F = 4 ;

	DBG1 (( MF__,"mk_prec_sys_tpm : op -> comp.prec.opt_tpmon      = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_tpmon_F ))


	/* ***** get tpmonid if ***** */
	/* ***** ',' was found  ***** */

	if ( tpm_found )
	{
		/* ***** get the beginning of tpmonid     ***** */
		/* ***** and the length of tpmonid field, ***** */
		/* ***** determinate length of tpmonid    ***** */

		comma_adr ++ ;
		length = sizeof ( options -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F ) ;
		len = start_adr + len_arg - comma_adr ;


		/* ***** check length ***** */

		if ( len > length )
			len = length ;


		/* ***** copy without '\0' ***** */

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F ,
						( char * ) comma_adr , len ) ;

		DBG1 (( MF__,"mk_prec_sys_tpm : op -> comp.prec.opt_tpmonid    = %.18s\n",
			      options -> variant.C_sp4co_sql_precompiler.opt_tpmonid_F ))
	}
}

/*==========================================================================*/

static	void	mk_prec_mode ( 
struct tsp4_args_options        * options )
{
	DBG1 (( MF__,"********** mk_prec_mode called ***************\n" ))

	/* ***** get sql mode ***** */

	if ( strcmp ( optarg , "syntax" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_mode_F = 1 ;

	if ( strcmp ( optarg , "nocheck" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_mode_F = 0 ;

	DBG1 (( MF__,"mk_prec_mode    : op -> comp.prec.opt_mode       = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_mode_F ))
}

/*==========================================================================*/

static	void	mk_prec_cansi ( 
struct tsp4_args_options        * options )
{
	DBG1 (( MF__,"********** mk_prec_cansi called **************\n" ))

	/* ***** get sql cansi ***** */

	if ( strcmp ( optarg , "cansi" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_cansi_F = 1 ;
	else if ( strcmp ( optarg , "cplus" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_cansi_F = 2 ;
	else if ( strcmp ( optarg , "ansi85" ) == 0 )
		options -> variant.C_sp4co_sql_precompiler.opt_cansi_F = 3 ;

	DBG1 (( MF__,"mk_prec_mode    : op -> comp.prec.opt_cansi      = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_cansi_F ))
}

/*==========================================================================*/

static	void	mk_prec_unicode ( 
struct tsp4_args_options        * options )
{
	DBG1 (( MF__,"********** mk_prec_unicode called **************\n" ))

	/* ***** get unicode ***** */
        if ( strcmp ( optarg , "unicode" ) == 0 ) 
	  options -> variant.C_sp4co_sql_precompiler.opt_unicode_F = TRUE ;
	DBG1 (( MF__,"mk_prec_unicode  : op -> comp.prec.opt_unicode      = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_unicode_F ))
}


/*==========================================================================*/

static	void	mk_prec_packetsize ( 
struct tsp4_args_options        * options )
{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_prec_packetsize called **************\n" ))


	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
          options -> variant.C_sp4co_sql_precompiler.opt_packetsize_F = atoi ( optarg ) ;

	DBG1 (( MF__,"mk_prec_packetsize    : op -> comp.prec.opt_packetsize = %d\n",
		      options -> variant.C_sp4co_sql_precompiler.opt_packetsize_F ))
}

/*==========================================================================*/

static	void	mk_pc_rt_trace_fn ( 
struct tsp4_args_options        * options )
{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_pc_rt_trace_fn called **********\n" ))

	/* ***** get trace name ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F , optarg ,
		 len_arg ) ;

	DBG1 (( MF__,"mk_pc_rt_trace_f: op -> comp.pc_rt.opt_rtracefile= %.64s\n",
		      options -> variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F ))
}

/*==========================================================================*/

static	void	mk_pc_rt_mfetch ( 
struct tsp4_args_options        * options )
{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_pc_rt_mfetch called ************\n" ))

	/* ***** get mfetch count and test for the digits 0 through 9 ***** */

	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
		options -> variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F =
							     atoi ( optarg ) ;

	DBG1 (( MF__,"mk_pc_rt_mfetch : op -> comp.pc_rt.opt_rmfetch   = %d\n",
		      options -> variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F ))
}

/*==========================================================================*/

static	void	mk_pc_rt_rstmt_cnt ( 
struct tsp4_args_options        * options )
{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_pc_rt_rstmt_cnt called ************\n" ))


	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
		options -> variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F =
							     atoi ( optarg ) ;

	DBG1 (( MF__,"mk_pc_rt_mfetch : op -> comp.pc_rt.opt_rstmt_cnt   = %d\n",
		      options -> variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F ))
}

/*==========================================================================*/

static	void	mk_pc_rt_rtime_limit ( 
struct tsp4_args_options        * options )
{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_pc_rt_rtime_limit called ************\n" ))


	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
		options -> variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F =
							     atoi ( optarg ) ;

	DBG1 (( MF__,"mk_pc_rt_mfetch : op -> comp.pc_rt.opt_rtime_limit   = %d\n",
		      options -> variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F ))
}

/*==========================================================================*/

static	void	mk_dcomp_r_fn ( 
struct tsp4_args_options        * options )
{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_dcomp_r_fn called **************\n" ))

	/* ***** get runfile ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_easy.opt_runfile_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F , optarg ,
		 len_arg ) ;


	/* ***** get command mode ***** */

	options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run ;

	DBG1 (( MF__,"mk_dcomp_r_fn   : op -> comp.dcu.opt_runfile     = %.64s\n",
		      options -> variant.C_sp4co_sql_easy.opt_runfile_F ))
	DBG1 (( MF__,"mk_dcomp_r_fn   : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

/*==========================================================================*/

static	void	mk_dcomp_b_fn ( 
struct tsp4_args_options        * options )
{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_dcomp_b_fn called **************\n" ))

	/* ***** get runfile ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_easy.opt_runfile_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F , optarg ,
		 len_arg ) ;


	/* ***** get command mode ***** */

	options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch ;

	DBG1 (( MF__,"mk_dcomp_b_fn   : op -> comp.dcu.opt_runfile     = %.64s\n",
		      options -> variant.C_sp4co_sql_easy.opt_runfile_F ))
	DBG1 (( MF__,"mk_dcomp_b_fn   : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

/*==========================================================================*/

static	void	mk_dcomp_r_obj ( 
struct tsp4_args_options       * options ,
tsp00_Bool                     * ok ,
tsp00_ErrText                    errtext )
{
    int                         lengtho ; /* o = owner */
    int                         length1 ; /* 1 = name1 */
    int                         length2 ; /* 2 = name2 */
    int                         leno ;
    int                         len1 ;
    int                         len2 ;

    DBG1 (( MF__,"********** mk_dcomp_r_obj called *************\n" ))

    /* ***** initialize ***** */

    lengtho = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ) ;
    length1 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ;
    length2 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ) ;


    /* ***** determinate object owner, name1 and name2 for ***** */
    /* ***** dialog                                        ***** */

    if ( ( options -> opt_component == sp4co_sql_dialog ) ||
	 ( options -> opt_component == sp4co_sql_appl   )    )
    {

	/* ***** get your args ***** */

	check_idents ( '.' , 3 , ok , errtext ) ;

	if ( ! ( * ok ) )
	    return ;

	if ( ( ctu . i_len [ 0 ] > 0 ) &&
	     ( ctu . i_len [ 1 ] > 0 ) &&
	     ( ctu . i_len [ 2 ] > 0 )     )
	{
	    leno = ctu . i_len [ 0 ] ;
	    if ( leno > lengtho )
		leno = lengtho ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		     (char *)ctu . ident [ 0 ] , leno ) ;

	    if ( ctu . i_tup [ 0 ] )
	    {
		move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		       leno ) ;
	    }

	    len1 = ctu . i_len [ 1 ] ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		     (char *)ctu . ident [ 1 ] , len1 ) ;

	    if ( ctu . i_tup [ 1 ] )
	    {
		move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		       len1 ) ;
	    }

	    len2 = ctu . i_len [ 2 ] ;
	    if ( len2 > length2 )
		len2 = length2 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
		     ctu . ident [ 2 ] , len2 ) ;

	    if ( ctu . i_tup [ 2 ] )
	    {
		move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
		       len2 ) ;
	    }
	}
	else
	{
	    if ( ( ctu . i_len [ 0 ] > 0  ) &&
		 ( ctu . i_len [ 1 ] > 0  ) &&
		 ( ctu . i_len [ 2 ] == 0 )     )
	    {
		len1 = ctu . i_len [ 0 ] ;
		if ( len1 > length1 )
		    len1 = length1 ;

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
			 ctu . ident [ 0 ] , len1 ) ;

		if ( ctu . i_tup [ 0 ] )
		{
		    move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
			   len1 ) ;
		}

		len2 = ctu . i_len [ 1 ] ;
		if ( len2 > length2 )
		    len2 = length2 ;

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
			 ctu . ident [ 1 ] , len2 ) ;

		if ( ctu . i_tup [ 1 ] )
		{
		    move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
			   len2 ) ;
		}
	    }
	    else
	    {
		if ( ( ctu . i_len [ 0 ] >  0 ) &&
		     ( ctu . i_len [ 1 ] == 0 ) &&
		     ( ctu . i_len [ 2 ] == 0 )     )
		{
		    len1 = ctu . i_len [ 0 ] ;
		    if ( len1 > length1 )
			len1 = length1 ;

		    SAPDB_memcpy ( options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			     ctu . ident [ 0 ] , len1 ) ;

		    if ( ctu . i_tup [ 0 ] )
		    {
			move_toupper (
			   options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			       len1 ) ;
		    }
		}
		else
		{
		    * ok = FALSE ;
		    SAPDB_memcpy ( errtext , illegal_dialog_obj , sizeof ( tsp00_ErrText ) ) ;
		    return ;
		}
	    }
	}
    }


    /* ***** determinate object owner and name1 for ***** */
    /* ***** query and easy                         ***** */

    if ( ( options -> opt_component == sp4co_sql_query ) ||
	 ( options -> opt_component == sp4co_sql_easy  )    )
    {
	/* ***** get your args ***** */

	check_idents ( '.' , 2 , ok , errtext ) ;

	if ( ! ( * ok ) )
	    return ;

	if ( ( ctu . i_len [ 0 ] > 0  ) &&
	     ( ctu . i_len [ 1 ] > 0  ) &&
	     ( ctu . i_len [ 2 ] == 0 )     )
	{
	    leno = ctu . i_len [ 0 ] ;
	    if ( leno > lengtho )
		leno = lengtho ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		     ctu . ident [ 0 ] , leno ) ;

	    if ( ctu . i_tup [ 0 ] )
	    {
		move_toupper (
		   options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		       leno ) ;
	    }

	    len1 = ctu . i_len [ 1 ] ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		     ctu . ident [ 1 ] , len1 ) ;

	    if ( ctu . i_tup [ 1 ] )
	    {
		move_toupper (
		   options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		       len1 ) ;
	    }
	}
	else
	{
	    if ( ( ctu . i_len [ 0 ] >  0 ) &&
		 ( ctu . i_len [ 1 ] == 0 ) &&
		 ( ctu . i_len [ 2 ] == 0 )     )
	    {
		len1 = ctu . i_len [ 0 ] ;
		if ( len1 > length1 )
		    len1 = length1 ;

		SAPDB_memcpy ( options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			 ctu . ident [ 0 ] , len1 ) ;

		if ( ctu . i_tup [ 0 ] )
		{
		    move_toupper (
		       options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			   len1 ) ;
		}
	    }
	    else
	    {
		* ok = FALSE ;
		SAPDB_memcpy ( errtext , illegal_dialog_obj , sizeof ( tsp00_ErrText ) ) ;
		return ;
	    }
	}
    }


    /* ***** get command mode ***** */

    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run_object ;

    DBG1 (( MF__,"mk_dcomp_r_obj  : op -> comp.dcu.opt_obj_owner   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_owner_F ))
    DBG1 (( MF__,"mk_dcomp_r_obj  : op -> comp.dcu.opt_obj_name1   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
    DBG1 (( MF__,"mk_dcomp_r_obj  : op -> comp.dcu.opt_obj_name2   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_name2_F ))
    DBG1 (( MF__,"mk_dcomp_b_fobj : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}
/*==========================================================================*/

static	void	mk_dcomp_b_obj ( 
struct tsp4_args_options        * options ,
tsp00_Bool                      * ok ,
tsp00_ErrText                     errtext )
{
    int                         lengtho ; /* o = owner */
    int                         length1 ; /* 1 = name1 */
    int                         length2 ; /* 2 = name2 */
    int                         leno ;
    int                         len1 ;
    int                         len2 ;

    DBG1 (( MF__,"********** mk_dcomp_b_obj called *************\n" ))

    /* ***** initialize ***** */

    lengtho = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ) ;
    length1 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ;
    length2 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ) ;


    /* ***** determinate object owner, name1 and name2 for ***** */
    /* ***** dialog                                        ***** */

    if ( ( options -> opt_component == sp4co_sql_dialog ) ||
	 ( options -> opt_component == sp4co_sql_appl   )    )
    {

	/* ***** get your args ***** */

	check_idents ( '.' , 3 , ok , errtext ) ;

	if ( ! ( * ok ) )
	    return ;

	if ( ( ctu . i_len [ 0 ] > 0 ) &&
	     ( ctu . i_len [ 1 ] > 0 ) &&
	     ( ctu . i_len [ 2 ] > 0 )     )
	{
	    leno = ctu . i_len [ 0 ] ;
	    if ( leno > lengtho )
		leno = lengtho ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		     ctu . ident [ 0 ] , leno ) ;

	    if ( ctu . i_tup [ 0 ] )
	    {
		move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		       leno ) ;
	    }

	    len1 = ctu . i_len [ 1 ] ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		     ctu . ident [ 1 ] , len1 ) ;

	    if ( ctu . i_tup [ 1 ] )
	    {
		move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		       len1 ) ;
	    }

	    len2 = ctu . i_len [ 2 ] ;
	    if ( len2 > length2 )
		len2 = length2 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
		     ctu . ident [ 2 ] , len2 ) ;

	    if ( ctu . i_tup [ 2 ] )
	    {
		move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
		       len2 ) ;
	    }
	}
	else
	{
	    if ( ( ctu . i_len [ 0 ] > 0  ) &&
		 ( ctu . i_len [ 1 ] > 0  ) &&
		 ( ctu . i_len [ 2 ] == 0 )     )
	    {
		len1 = ctu . i_len [ 0 ] ;
		if ( len1 > length1 )
		    len1 = length1 ;

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
			 ctu . ident [ 0 ] , len1 ) ;

		if ( ctu . i_tup [ 0 ] )
		{
		    move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
			   len1 ) ;
		}

		len2 = ctu . i_len [ 1 ] ;
		if ( len2 > length2 )
		    len2 = length2 ;

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
			 ctu . ident [ 1 ] , len2 ) ;

		if ( ctu . i_tup [ 1 ] )
		{
		    move_toupper (
		       options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
			   len2 ) ;
		}
	    }
	    else
	    {
		if ( ( ctu . i_len [ 0 ] >  0 ) &&
		     ( ctu . i_len [ 1 ] == 0 ) &&
		     ( ctu . i_len [ 2 ] == 0 )     )
		{
		    len1 = ctu . i_len [ 0 ] ;
		    if ( len1 > length1 )
			len1 = length1 ;

		    SAPDB_memcpy ( options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			     ctu . ident [ 0 ] , len1 ) ;

		    if ( ctu . i_tup [ 0 ] )
		    {
			move_toupper (
			   options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			       len1 ) ;
		    }
		}
		else
		{
		    * ok = FALSE ;
		    SAPDB_memcpy ( errtext , illegal_dialog_obj , sizeof ( tsp00_ErrText ) ) ;
		    return ;
		}
	    }
	}
    }


    /* ***** determinate object owner and name1 for ***** */
    /* ***** query and easy                         ***** */

    if ( ( options -> opt_component == sp4co_sql_query ) ||
	 ( options -> opt_component == sp4co_sql_easy  )    )
    {
	/* ***** get your args ***** */

	check_idents ( '.' , 2 , ok , errtext ) ;

	if ( ! ( * ok ) )
	    return ;

	if ( ( ctu . i_len [ 0 ] > 0  ) &&
	     ( ctu . i_len [ 1 ] > 0  ) &&
	     ( ctu . i_len [ 2 ] == 0 )     )
	{
	    leno = ctu . i_len [ 0 ] ;
	    if ( leno > lengtho )
		leno = lengtho ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		     ctu . ident [ 0 ] , leno ) ;

	    if ( ctu . i_tup [ 0 ] )
	    {
		move_toupper (
		   options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		       leno ) ;
	    }

	    len1 = ctu . i_len [ 1 ] ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		     ctu . ident [ 1 ] , len1 ) ;

	    if ( ctu . i_tup [ 1 ] )
	    {
		move_toupper (
		   options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		       len1 ) ;
	    }
	}
	else
	{
	    if ( ( ctu . i_len [ 0 ] >  0 ) &&
		 ( ctu . i_len [ 1 ] == 0 ) &&
		 ( ctu . i_len [ 2 ] == 0 )     )
	    {
		len1 = ctu . i_len [ 0 ] ;
		if ( len1 > length1 )
		    len1 = length1 ;

		SAPDB_memcpy ( options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			 ctu . ident [ 0 ] , len1 ) ;

		if ( ctu . i_tup [ 0 ] )
		{
		    move_toupper (
		       options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
			   len1 ) ;
		}
	    }
	    else
	    {
		* ok = FALSE ;
		SAPDB_memcpy ( errtext , illegal_dialog_obj , sizeof ( tsp00_ErrText ) ) ;
		return ;
	    }
	}
    }


    /* ***** get command mode ***** */

    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch_object ;

    DBG1 (( MF__,"mk_dcomp_b_obj  : op -> comp.dcu.opt_obj_owner   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_owner_F ))
    DBG1 (( MF__,"mk_dcomp_b_obj  : op -> comp.dcu.opt_obj_name1   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
    DBG1 (( MF__,"mk_dcomp_b_obj  : op -> comp.dcu.opt_obj_name2   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_name2_F ))
    DBG1 (( MF__,"mk_dcomp_b_fobj : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

/*==========================================================================*/

static	void	mk_dcomp_object ( 
struct tsp4_args_options        * options ,
tsp00_Bool                      * ok ,
tsp00_ErrText                     errtext )
{
    int                     lengtho ;
    int                     length1 ;
    int                     leno ;
    int                     len1 ;

    DBG1 (( MF__,"********** mk_dcomp_object called ************\n" ))

    /* ***** initialize ***** */

    lengtho = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ) ;
    length1 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ;


    /* ***** get your args ***** */

    check_idents ( '.' , 2 , ok , errtext ) ;

    if ( ! ( * ok ) )
	return ;

    if ( ( ctu . i_len [ 0 ] > 0  ) &&
	 ( ctu . i_len [ 1 ] > 0  ) &&
	 ( ctu . i_len [ 2 ] == 0 )     )
    {
	leno = ctu . i_len [ 0 ] ;
	if ( leno > lengtho )
	    leno = lengtho ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		 ctu . ident [ 0 ] , leno ) ;

	if ( ctu . i_tup [ 0 ] )
	{
	    move_toupper (
	       options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
		   leno ) ;
	}

	len1 = ctu . i_len [ 1 ] ;
	if ( len1 > length1 )
	    len1 = length1 ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		 ctu . ident [ 1 ] , len1 ) ;

	if ( ctu . i_tup [ 1 ] )
	{
	    move_toupper (
	       options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		   len1 ) ;
	}
    }
    else
    {
	if ( ( ctu . i_len [ 0 ] >  0 ) &&
	     ( ctu . i_len [ 1 ] == 0 ) &&
	     ( ctu . i_len [ 2 ] == 0 )     )
	{
	    len1 = ctu . i_len [ 0 ] ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
		     ctu . ident [ 0 ] , len1 ) ;

	    if ( ctu . i_tup [ 0 ] )
	    {
		move_toupper (
		   options->variant.C_sp4co_sql_easy.opt_object_name1_F ,
		       len1 ) ;
	    }
	}
	else
	{
	    * ok = FALSE ;
	    SAPDB_memcpy ( errtext , illegal_dialog_obj , sizeof ( tsp00_ErrText ) ) ;
	    return ;
	}
    }

    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_list ;

    DBG1 (( MF__,"mk_dcomp_object : op -> comp.dcu.opt_obj_owner   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_owner_F ))
    DBG1 (( MF__,"mk_dcomp_object : op -> comp.dcu.opt_obj_name1   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
    DBG1 (( MF__,"mk_dcomp_object : op -> comp.dcu.opt_comm_mode   = %d\n",
		  options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

#if NOT_SAVE
/*==========================================================================*/

static	void	mk_dcomp_r_obj ( options )
 
struct tsp4_args_options        * options ;

{
	int                     len_arg ;
	int                     lengtho ;
	int                     length1 ;
	int                     length2 ;
	int                     leno ;
	int                     len1 ;
	int                     len2 ;
	char *                  start_adr ;
	char *                  first_point_adr ;
	char *                  second_point_adr ;

	DBG1 (( MF__,"********** mk_dcomp_r_obj called *************\n" ))

	/* ***** initialize ***** */

	len_arg = strlen ( optarg ) ;
	lengtho = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ) ;
	length1 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ;
	length2 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ) ;
	first_point_adr  = strchr  ( optarg , '.' ) ;
	second_point_adr = strrchr ( optarg , '.' ) ;
	start_adr        = optarg ;


	/* ***** determinate object owner, name1 and name2 for ***** */
	/* ***** dialog or appl                                ***** */

	if ( ( options -> opt_component == sql_dialog ) ||
	     ( options -> opt_component == sql_appl   )    )
	{
		if ( first_point_adr == ( unsigned long ) NULL )
		{
			/* ***** no point ***** */

			len1 = len_arg ;
			if ( len1 > length1 )
				len1 = length1 ;

			SAPDB_memcpy ( options -> components . dcomp_util .
					  opt_object_name1 , optarg , len1 ) ;
		}
		else
		{
			if ( first_point_adr == second_point_adr )
			{
				/* ***** one point ***** */

				len1 = first_point_adr - start_adr ;
				if ( len1 > length1 )
					len1 = length1 ;

				len2 = start_adr + len_arg -
							first_point_adr - 1 ;
				if ( len2 > length2 )
					len2 = length2 ;

				SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , optarg , len1 ) ;
				SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
					 first_point_adr + 1 , len2 ) ;
			}
			else
			{
				if (   ( first_point_adr != second_point_adr )
				    && ( second_point_adr != NULL ) )
				{
					/* ***** two points ***** */

					leno = first_point_adr - start_adr ;
					if ( leno > lengtho )
						leno = lengtho ;

					len1 = second_point_adr -
							 first_point_adr - 1 ;
					if ( len1 > length1 )
						len1 = length1 ;

					len2 = start_adr + len_arg -
							second_point_adr - 1 ;
					if ( len2 > length2 )
						len2 = length2 ;

					SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
						 optarg , leno ) ;
					SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
						 first_point_adr + 1 , len1 ) ;
					SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
						 second_point_adr + 1 , len2 ) ;
				}
			}
		}
	}


	/* ***** determinate object owner and name1 for ***** */
	/* ***** query and easy                         ***** */

	if ( ( options -> opt_component == sql_query ) ||
	     ( options -> opt_component == sql_easy  )    )
	{
		if ( first_point_adr == ( unsigned long ) NULL )
		{
			/* ***** no point ***** */

			len1 = len_arg ;
			if ( len1 > length1 )
				len1 = length1 ;

			SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , optarg , len1 ) ;
		}
		else
		{
			/* ***** one point ***** */

			leno = first_point_adr - start_adr ;
			if ( leno > lengtho )
				leno = lengtho ;

			len1 = start_adr + len_arg -
						first_point_adr - 1 ;
			if ( len1 > length1 )
				len1 = length1 ;

			SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F , optarg , leno ) ;
			SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
				 first_point_adr + 1 , len1 ) ;
		}
	}


	/* ***** get command mode ***** */

	options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_run_object ;

	DBG1 (( MF__,"mk_dcomp_r_obj  : op -> comp.dcu.opt_obj_owner   = %.18s\n",
		      options -> variant.C_sp4co_sql_easy.opt_object_owner_F ))
	DBG1 (( MF__,"mk_dcomp_r_obj  : op -> comp.dcu.opt_obj_name1   = %.18s\n",
		      options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
	DBG1 (( MF__,"mk_dcomp_r_obj  : op -> comp.dcu.opt_obj_name2   = %.18s\n",
		      options -> variant.C_sp4co_sql_easy.opt_object_name2_F ))
	DBG1 (( MF__,"mk_dcomp_r_fobj : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

/*==========================================================================*/

static	void	mk_dcomp_b_obj ( options )
 
struct tsp4_args_options        * options ;

{
    int                         len_arg ;
    int                         lengtho ;
    int                         length1 ;
    int                         length2 ;
    int                         leno ;
    int                         len1 ;
    int                         len2 ;
    char *                      start_adr ;
    char *                      first_point_adr ;
    char *                      second_point_adr ;

    DBG1 (( MF__,"********** mk_dcomp_b_obj called *************\n" ))

    /* ***** initialize ***** */

    len_arg = strlen ( optarg ) ;
    lengtho = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ) ;
    length1 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ;
    length2 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ) ;
    first_point_adr  = strchr  ( optarg , '.' ) ;
    second_point_adr = strrchr ( optarg , '.' ) ;
    start_adr        = optarg ;


    /* ***** determinate object owner, name1 and name2 for ***** */
    /* ***** dialog                                        ***** */

    if ( ( options -> opt_component == sql_dialog ) ||
	 ( options -> opt_component == sql_appl   )    )
    {
	if ( first_point_adr == ( unsigned long ) NULL )
	{
	    /* ***** no point ***** */

	    len1 = len_arg ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , optarg , len1 ) ;
	}
	else
	{
	    if ( first_point_adr == second_point_adr )
	    {
		/* ***** one point ***** */

		len1 = first_point_adr - start_adr ;
		if ( len1 > length1 )
			len1 = length1 ;

		len2 = start_adr + len_arg -
					first_point_adr - 1 ;
		if ( len2 > length2 )
		    len2 = length2 ;

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , optarg , len1 ) ;
		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
			 first_point_adr + 1 , len2 ) ;
	    }
	    else
	    {
		if (   ( first_point_adr != second_point_adr )
		    && ( second_point_adr != ( unsigned long )
						      NULL ) )
		{
		    /* ***** two points ***** */

		    leno = first_point_adr - start_adr ;
		    if ( leno > lengtho )
			leno = lengtho ;

		    len1 = second_point_adr - first_point_adr - 1 ;
		    if ( len1 > length1 )
			len1 = length1 ;

		    len2 = start_adr + len_arg - second_point_adr - 1 ;
		    if ( len2 > length2 )
			len2 = length2 ;

		    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ,
			     optarg , leno ) ;
		    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
			     first_point_adr + 1 , len1 ) ;
		    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name2_F ,
			     second_point_adr + 1 , len2 ) ;
		}
	    }
	}
    }


    /* ***** determinate object owner and name1 for ***** */
    /* ***** query and easy                         ***** */

    if ( ( options -> opt_component == sql_query ) ||
	 ( options -> opt_component == sql_easy  )    )
    {
	if ( first_point_adr == ( unsigned long ) NULL )
	{
	    /* ***** no point ***** */

	    len1 = len_arg ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , optarg , len1 ) ;
	}
	else
	{
	    /* ***** one point ***** */

	    leno = first_point_adr - start_adr ;
	    if ( leno > lengtho )
		leno = lengtho ;

	    len1 = start_adr + len_arg - first_point_adr - 1 ;
	    if ( len1 > length1 )
		len1 = length1 ;

	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F , optarg , leno ) ;
	    SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
		     first_point_adr + 1 , len1 ) ;
	}
    }


    /* ***** get command mode ***** */

    options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_batch_object ;

    DBG1 (( MF__,"mk_dcomp_b_obj  : op -> comp.dcu.opt_obj_owner   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_owner_F ))
    DBG1 (( MF__,"mk_dcomp_b_obj  : op -> comp.dcu.opt_obj_name1   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
    DBG1 (( MF__,"mk_dcomp_b_obj  : op -> comp.dcu.opt_obj_name2   = %.18s\n",
		  options -> variant.C_sp4co_sql_easy.opt_object_name2_F ))
    DBG1 (( MF__,"mk_dcomp_b_fobj : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

/*==========================================================================*/

static	void	mk_dcomp_object ( options )
 
struct tsp4_args_options        * options ;

{
	int                     len_arg ;
	int                     lengtho ;
	int                     length1 ;
	int                     leno ;
	int                     len1 ;
	char *                  start_adr ;
	char *                  point_adr ;

	DBG1 (( MF__,"********** mk_dcomp_object called ************\n" ))

	/* ***** initialize ***** */

	len_arg = strlen ( optarg ) ;
	lengtho = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F ) ;
	length1 = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ;
	point_adr = strchr  ( optarg , '.' ) ;
	start_adr = optarg ;


	/* ***** determinate object owner, name1 ***** */

	if ( point_adr == NULL )
	{
		/* ***** no point ***** */

		len1 = len_arg ;
		if ( len1 > length1 )
			len1 = length1 ;

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , optarg , len1 ) ;
	}
	else
	{
		/* ***** one point ***** */

		leno = point_adr - start_adr ;
		if ( leno > lengtho )
			leno = lengtho ;

		len1 = start_adr + len_arg - point_adr - 1 ;
		if ( len1 > length1 )
			len1 = length1 ;

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_owner_F , optarg , leno ) ;
		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F , point_adr + 1 , len1 ) ;
	}

	options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_list ;

	DBG1 (( MF__,"mk_dcomp_object : op -> comp.dcu.opt_obj_owner   = %.18s\n",
		      options -> variant.C_sp4co_sql_easy.opt_object_owner_F ))
	DBG1 (( MF__,"mk_dcomp_object : op -> comp.dcu.opt_obj_name1   = %.18s\n",
		      options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
	DBG1 (( MF__,"mk_dcomp_object : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

#endif
/*==========================================================================*/

static	void	mk_dcomp_code_type ( options )
 
struct tsp4_args_options        * options ;

{
	DBG1 (( MF__,"********** mk_dcomp_code_type called *********\n" ))

	/* ***** get code type ***** */

	if ( strcmp ( optarg , "asc" ) == 0 )
		options -> variant.C_sp4co_sql_easy.opt_code_F = is_ascii ;

	if ( strcmp ( optarg , "ebc" ) == 0 )
		options -> variant.C_sp4co_sql_easy.opt_code_F = is_ebcdic ;

	DBG1 (( MF__,"mk_dcomp_code_ty: op -> comp.dcu.opt_code        = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_code_F ))
}

/*==========================================================================* /
/ * FSt * /
/ * FSt * /static	void	mk_dcomp_parameter ( options )
/ * FSt * /
/ * FSt * /struct tsp4_args_options        * options ;
/ * FSt * /
/ * FSt * /{
/ * FSt * /        int                     len_arg ;
/ * FSt * /        int                     length ;
/ * FSt * /
/ * FSt * /        DBG1 (( MF__,"********** mk_dcomp_parameter called *********\n" ))
/ * FSt * /
/ * FSt * /        DBG1 (( MF__,"mk_dcomp_param  : optarg                         = %.132s\n",
/ * FSt * /                                 optarg ))
/ * FSt * /
/ * FSt * /        len_arg = strlen ( optarg ) ;
/ * FSt * /        length = sizeof ( options -> components . dcomp_util .
/ * FSt * /                                                           opt_parameter );
/ * FSt * /
/ * FSt * /        if ( len_arg > length )
/ * FSt * /                len_arg = length ;
/ * FSt * /
/ * FSt * /        SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_parameter_F ,
/ * FSt * /                                                          optarg , len_arg ) ;
/ * FSt * /
/ * FSt * /
/ * FSt * /        DBG1 (( MF__,"mk_dcomp_param  : op -> comp.dcu.opt_parameter   = %.132s\n",
/ * FSt * /                      options -> variant.C_sp4co_sql_easy.opt_parameter_F ))
/ * FSt * /}
/ * FSt * /
/ *==========================================================================*/

static	void	mk_dcomp_export ( options )
 
struct tsp4_args_options        * options ;

{
	tsp00_Bool                 fn_found ;
	int                     len_arg ;
	int                     length ;
	int                     len ;
	char *                  start_adr ;
	char *                  comma_adr ;

	DBG1 (( MF__,"********** mk_dcomp_export called ************\n" ))

	/* ***** get object name1 :           ***** */
	/* ***** determinate comma position,  ****** */
	/* ***** the start position of optarg ****** */
	/* ***** and the object name1 string  ****** */
	/* ***** length                       ****** */

	fn_found = TRUE ;
	len_arg = strlen ( optarg ) ;
	comma_adr = strchr ( optarg , ',' ) ;
	start_adr = optarg ;
	len = comma_adr - start_adr ;
	length = sizeof ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ) ;


	/* ***** check length ***** */

	if ( comma_adr == ( unsigned long ) NULL )
	{
		fn_found = FALSE ;
		len = len_arg ;
	}

	if ( len > length )
		len = length ;


	/* ***** copy without '\0' ***** */

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_object_name1_F ,
							      optarg , len ) ;


	/* ***** get runfile if ***** */
	/* ***** ',' was found  ***** */

	if ( fn_found )
	{
		/* ***** get the beginning of runfile     ***** */
		/* ***** and the length of runfile field, ***** */
		/* ***** determinate length of runfile    ***** */

		comma_adr ++ ;
		length = sizeof ( options -> variant.C_sp4co_sql_easy.opt_runfile_F ) ;
		len = start_adr + len_arg - comma_adr ;


		/* ***** check length ***** */

		if ( len > length )
			len = length ;


		/* ***** copy without '\0' ***** */

		SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F ,
						( char * ) comma_adr , len ) ;
	}


	/* ***** get command mode ***** */

	options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_export ;

	DBG1 (( MF__,"mk_dcomp_export : op -> comp.dcu.opt_obj_name1   = %.18s\n",
		      options -> variant.C_sp4co_sql_easy.opt_object_name1_F ))
	DBG1 (( MF__,"mk_dcomp_export : op -> comp.dcu.opt_runfile     = %.64s\n",
			   options -> variant.C_sp4co_sql_easy.opt_runfile_F ))
	DBG1 (( MF__,"mk_dcomp_export : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
}

/*==========================================================================*/

static	void	mk_dcomp_import ( options )
 
struct tsp4_args_options        * options ;

{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_dcomp_import called ************\n" ))

	/* ***** get runfile ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_easy.opt_runfile_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_easy.opt_runfile_F ,
							  optarg , len_arg ) ;


	/* ***** get command mode ***** */

	options -> variant.C_sp4co_sql_easy.opt_comm_mode_F = sp4cm_sql_import ;

	DBG1 (( MF__,"mk_dcomp_import : op -> comp.dcu.opt_comm_mode   = %d\n",
		      options -> variant.C_sp4co_sql_easy.opt_comm_mode_F ))
	DBG1 (( MF__,"mk_dcomp_import : op -> comp.dcu.opt_runfile     = %.64s\n",
		      options -> variant.C_sp4co_sql_easy.opt_parameter_F ))
}

/*==========================================================================*/

static	void	mk_audit_cntrl ( options )
 
struct tsp4_args_options        * options ;

{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_audit_cntrl called *************\n" ))

	/* ***** get audit control ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_audit.opt_aud_control_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_audit.opt_aud_control_F , optarg ,
		 len_arg ) ;

	DBG1 (( MF__,"mk_audit_cntrl  : op -> comp.aud.opt_aud_control= %.64s\n",
		      options -> variant.C_sp4co_sql_audit.opt_aud_control_F ))
}

/*==========================================================================*/

static	void	mk_audit_logfile ( options )
 
struct tsp4_args_options        * options ;

{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_audit_logfile called ***********\n" ))

	/* ***** get logfile ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_audit.opt_aud_logfile_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_audit.opt_aud_logfile_F , optarg ,
		 len_arg ) ;

	DBG1 (( MF__,"mk_audit_logfile: op -> comp.aud.opt_aud_logfile     = %.64s\n",
		      options -> variant.C_sp4co_sql_audit.opt_aud_logfile_F ))
}


/*==========================================================================*/

static	void	mk_audit_log_blocksize ( options )
 
struct tsp4_args_options        * options ;

{
	tsp00_Bool                 no_alpha_found ;
	int                     len_arg ;
	int                     i ;

	DBG1 (( MF__,"********** mk_audit_log_blocksize called ***************\n" ))

	/* ***** get isolation level and test for the digits  ***** */
	/* ***** 0 through 9                                  ***** */

	len_arg = strlen ( optarg ) ;
	no_alpha_found = TRUE ;

	for ( i = 0 ; i < len_arg ; i ++ )
		if ( isalpha ( optarg [ i ] ) )
		{
			no_alpha_found = FALSE ;
			break ;
		}

	if ( no_alpha_found )
   	  options -> variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F = atoi ( optarg );


	DBG1 (( MF__,"mk_audit_log_blocksize: comp.aud.opt_aud_log_blocksize %d\n",
		      options -> variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F ))
}

/*==========================================================================*/

static	void	mk_audit_output ( options )
 
struct tsp4_args_options        * options ;

{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_audit_output called ************\n" ))

	/* ***** get audit output ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_audit.opt_aud_output_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_audit.opt_aud_output_F , optarg ,
		 len_arg ) ;

	DBG1 (( MF__,"mk_audit_output : op -> comp.aud.opt_aud_output = %.64s\n",
		      options -> variant.C_sp4co_sql_audit.opt_aud_output_F ))
}

/*==========================================================================*/

static	void	mk_userx_b_fn ( options )
 
struct tsp4_args_options        * options ;

{
	int                     len_arg ;
	int                     length ;

	DBG1 (( MF__,"********** mk_userx_b_fn called **************\n" ))

	/* ***** get runfile ***** */

	len_arg = strlen ( optarg ) ;
	length = sizeof ( options -> variant.C_sp4co_sql_userx.opt_ux_runfile_F );

	if ( len_arg > length )
		len_arg = length ;

	SAPDB_memcpy ( options -> variant.C_sp4co_sql_userx.opt_ux_runfile_F , optarg ,
		 len_arg ) ;


	/* ***** get command mode ***** */

	options -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F = sp4cm_sql_batch ;

	DBG1 (( MF__,"mk_userx_b_fn   : op -> comp.ux.opt_ux_runfile   = %.64s\n",
		      options -> variant.C_sp4co_sql_userx.opt_ux_runfile_F ))
	DBG1 (( MF__,"mk_userx_b_fn   : op -> comp.ux.opt_ux_comm_mode = %d\n",
		      options -> variant.C_sp4co_sql_userx.opt_ux_comm_mode_F ))
}

/*==========================================================================*/

static	void	move_toupper ( str , string_size )
 
char                            * str ;
int                             string_size ;

{
	int                     i ;

	DBG1 (( MF__,"********** move_toupper called ***************\n" ))
	DBG1 (( MF__,"move_toupper    : incoming string                = %.18s\n",
				 str                               ))
	DBG1 (( MF__,"move_toupper    : string_size                    = %d\n",
				 string_size                          ))

	/* ***** convert to upper ***** */

	for ( i = 0 ; i < string_size ; i ++ )
		str [ i ] = toupper ( str [ i ] ) ;


	DBG1 (( MF__,"move_toupper    : outging string                 = %.18s\n",
				 str                               ))
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"check_idents"
static	void	check_idents ( sep , icnt , ok , errtext )
 
int                             sep ;
int                             icnt ;
tsp00_Bool                    * ok ;
tsp00_ErrText                   errtext ;

{
    /* tsp00_Bool        conv_to_upper ; */
    tsp00_Bool           sep_found ;
    int                  cnt ;
    int                  quote_counter ;
    unsigned long        len_arg ;
    char *               sep_adr ;
    char *               start ;
    char *               end ;
    char *               first_quote ;
    char *               second_quote ;

    DBG1 (( MF__,"********** check_idents called ***************\n" ))

    /* ***** initialize global ident parameters ****** */

    ctu . ident [ 0 ] = 0 ;
    ctu . ident [ 1 ] = 0 ;
    ctu . ident [ 2 ] = 0 ;
    ctu . i_len [ 0 ] = 0 ;
    ctu . i_len [ 1 ] = 0 ;
    ctu . i_len [ 2 ] = 0 ;
    ctu . i_tup [ 0 ] = FALSE ;
    ctu . i_tup [ 1 ] = FALSE ;
    ctu . i_tup [ 2 ] = FALSE ;


    /* ***** initialize outgoing parameters ****** */

    * ok = TRUE ;


    /* ***** initialize internel parameters ****** */

    first_quote  = NULL;
    second_quote = NULL;
    

    /* ***** determinate argument length and start position ***** */

    len_arg = ( unsigned long ) strlen ( optarg ) ;
    start   = optarg ;
    end     = start + len_arg  ; /* including "\0" */


    DBG1 (( MF__,"start                    ( 1 ) = %lu\n", (long)start   ));
    DBG1 (( MF__,"end                            = %lu\n", (long)end     ));
    DBG1 (( MF__,"len_arg                        = %lu\n", len_arg ));


    /* ***** point to all occurences of '"' ****** */

    quote_counter = 0 ;

    first_quote  = strchr ( start , '"' ) ;
    if ( first_quote )
	quote_counter ++ ;

    for ( cnt = 0 ; cnt < icnt ; cnt ++ )
    {
	if ( first_quote )
	{
	    second_quote = strchr ( first_quote + 1 , '"' ) ;
	    if ( second_quote )
		quote_counter ++ ;
	}

	if ( second_quote )
	{
	    first_quote = strchr ( second_quote + 1 , '"' ) ;
	    if ( first_quote )
		quote_counter ++ ;
	}
    }

    /* ***** check quotes occurence ****** */

    if ( quote_counter % 2 )
    {
	* ok = FALSE ;
	SAPDB_memcpy ( errtext , missing_quote , sizeof ( tsp00_ErrText ) ) ;
	DBG1 (( MF__,"quote_counter                  = %d\n", quote_counter ))
	return ;
    }


    /* ***** initialize internel parameters again ***** */

    first_quote  = NULL;
    second_quote = NULL;
    sep_found    = FALSE ;


    /* ***** check every part of the argument string ***** */

    for ( cnt = 0 ; ( cnt < icnt ) && ( start != end ) ; cnt ++ )
    {

	DBG1 (( MF__,"start                    ( 2 ) = %lu\n", (long)start ))
	DBG1 (( MF__,"cnt                            = %d\n",  cnt   ))

	/* ***** first sign of the argumnet part == seperator ***** */
	/* ***** skip to the next part                        ***** */

	if ( strchr ( start , (char) sep ) == start )
	{
	    start ++ ;
	    if ( sep_found )
	    {
		* ok = FALSE ;
		SAPDB_memcpy ( errtext , double_seperator , sizeof ( tsp00_ErrText ) ) ;
		return ;
	    }
	    sep_found = TRUE ;
	    DBG1 (( MF__,"start ( seperator found )      = %lu\n", (long)start ))
	    continue ;
	}

	/* ***** reset sep_found flag ***** */

	sep_found = FALSE ;


	/* ***** point to '"' ****** */

	first_quote  = strchr ( start , '"');

	DBG1 (( MF__,"first_quote              ( 1 ) = %lu\n", (long)first_quote   ))


	/* ***** be sure you are at the beginning of the part ***** */

	if ( first_quote != start )
	    first_quote = 0 ;

	if ( first_quote )
	{
	    second_quote = strchr ( first_quote + 1 , '"');

	    DBG1 (( MF__,"second_quote                   = %lu\n", (long)second_quote  ))
	}


	/* ***** correct quote count ?   ***** */

	if ( ( first_quote ) && ( ! second_quote ) )
	{
	    * ok = FALSE ;
	    SAPDB_memcpy ( errtext , missing_quote , sizeof ( tsp00_ErrText ) ) ;
	    return ;
	}


	/* ***** get string  ***** */

	if ( first_quote )
	{
	    /* *****   ------>"string"<------     ***** */
	    /* ***** start address                ***** */
	    /* ***** string length                ***** */
	    /* ***** don't convert to upper signs ***** */

	    ctu . ident [ cnt ] = first_quote + 1 ;
	    ctu . i_len [ cnt ] = second_quote - first_quote - 1 ;
	    ctu . i_tup [ cnt ] = FALSE ;

	    DBG1 (( MF__,"ctu . ident [ cnt ] ( quoted ) = %lu\n",
				     (long)ctu . ident [ cnt ] ))
	    DBG1 (( MF__,"ctu . i_len [ cnt ] ( quoted ) = %ld\n",
				     ctu . i_len [ cnt ] ))
	    DBG1 (( MF__,"ctu . i_tup [ cnt ] ( quoted ) = %d\n",
				     ctu . i_tup [ cnt ] ))


	    /* ***** find seperator ***** */

	    if ( ( sep_adr = strchr ( second_quote , (char) sep ) ) != NULL )
	    {
		/* ***** seperator found ***** */

		sep_found = TRUE ;
		DBG1 (( MF__,"sep_adr ( found , quoted str ) = %lu\n",
					 (long)sep_adr ))
	    }
	    else
	    {
		/* ***** seperator not found ***** */

		sep_found = FALSE ;
		sep_adr   = end ;
		DBG1 (( MF__,"sep_adr (not found, quoted str)= %lu\n",
					 (long)sep_adr ))


		/* ***** check error conditions ***** */

		if ( end > ( second_quote + 1 ) )
		{
		    * ok = FALSE ;
		    SAPDB_memcpy ( errtext , illegal_argument , sizeof ( tsp00_ErrText ) ) ;
		    return ;
		}
	    }
	}
	else
	{
	    /* ***** find seperator ***** */

	    if ( ( sep_adr = strchr ( start , (char) sep ) ) != NULL )
	    {
		/* ***** seperator found ***** */

		sep_found = TRUE ;
		DBG1 (( MF__,"sep_adr ( found, not quoted )  = %lu\n",
					 (long)sep_adr ))
	    }
	    else
	    {
		/* ***** seperator not found ***** */

		sep_found = FALSE ;
		sep_adr   = end ;
		DBG1 (( MF__,"sep_adr (not found, not quoted)= %lu\n",
					 (long)sep_adr ))
	    }


	    /* ***** ------> string <------ ***** */
	    /* ***** start address          ***** */
	    /* ***** string length          ***** */
	    /* ***** convert to upper signs ***** */

	    ctu . ident [ cnt ] = start ;
	    ctu . i_len [ cnt ] = sep_adr - start ;
	    ctu . i_tup [ cnt ] = TRUE ;

	    DBG1 (( MF__,"check_idents    : ctu . ident [ cnt ](not quoted)= %lu\n",
				     (long)ctu . ident [ cnt ] ))
	    DBG1 (( MF__,"check_idents    : ctu . i_len [ cnt ](not quoted)= %ld\n",
				     ctu . i_len [ cnt ] ))
	    DBG1 (( MF__,"check_idents    : ctu . i_tup [ cnt ](not quoted)= %d\n",
				     ctu . i_tup [ cnt ] ))
	}


	/* ***** new start point     ***** */
	/* ***** if seperator found, ***** */
	/* ***** icrease pointer to  ***** */
	/* ***** point after the     ***** */
	/* ***** seperator           ***** */

	start = sep_adr ;
	if ( start != end )
	    start ++ ;
    }
}
/*==========================================================================*/
/*======  old sqlargs  =  sqlargl  =========================================*/
/*==========================================================================*/

global	void	sqlargs ( args )
 
tsp00_Line                args ;

{
	sqlargl ( args ) ;
}

/*==========================================================================*/

global	void	sqlargl ( args )
 
tsp00_Line                args ;

{
	int             o_char ;
	tsp00_Bool         more_args ;
#ifdef  DEBUG_RTE
	int             i ;
#endif
#ifdef  DUMMY
	char			argums [ MX_LINE ];
	char			*argumv [ 2 ];
#endif


	DBG1 (( MF__,"********** sqlargl called ********************\n" ))


	/* ***** initialize return parameter ***** */

	SAPDB_memset ( (char *) args , ' ' , MX_LINE ) ;


	/* ***** initialize internel vars ***** */

	more_args = TRUE ;
	o_char = ' ' ;


	/* ***** initialize global and extern vars ***** */

	username_found = FALSE ;
	password_found = FALSE ;
	dbname_found = FALSE ;
	cmd_found = FALSE ;
	filename_found = FALSE ;
	arguments_found = FALSE ;

	optind = 1 ;
	opterr = 0 ;


	/* ***** return of the addresses of argc and argv ***** */
	/* ***** ( not the system argv and argc !!!! )    ***** */
 
#ifdef	DUMMY
	    {
		argc = 1 ;
		argv = argumv ;
		argv[0] = argums ;
		argv[1] = 0 ;
		(void) strcpy ( argums , "hallo" );
	    }
#else
&  if $COMP = PTOC
	    {
		argc = _argc ;
		argv = (char**) _argv ; /* The PTOC type for _argv is funny */
	    }
&  else
#    if SUN || SOLARIS
	    {
		extern	int	_ARGC ;
		extern	char	**_ARGV ;
		argc = _ARGC ;
		argv = _ARGV ;
	    }
#    else
#      if HP9
	    {
		argc = _argc ;
		argv = (char**) _argv ;
	    }
#      else
	    {
		sql12a_argas ( &argc , &argv );
	    }
#      endif  /*HP9*/
#    endif  /*SUN||SOLARIS*/
&  endif  /*PTOC*/
#endif	/*DUMMY*/

#ifdef DEBUG_RTE
	DBG1 (( MF__,"sqlargl: argc                    = %d\n", argc ))
	for ( i = 0 ; i < argc ; i ++ )
	{
		DBG1 (( MF__,"sqlargl: argv [ i ]              = %s\n", argv[i] ))
	}
	DBG1 (( MF__,"sqlargl: more_args (TRUE,FALSE)  = %d\n", more_args ))
#endif

	/* ***** parameter existing ? ***** */

	if ( argc > 1 )
	{
		/* ***** get options ***** */

		while ( ( ( o_char = sqlgetopt ( argc , argv ,
					      "u:d:r:b:" ) ) != - 1 ) &&
			more_args )
		{
			DBG1 (( MF__,"sqlargl: o_char                  = %c\n",
					 o_char ))
			DBG1 (( MF__,"sqlargl: optind                  = %d\n",
					 optind ))
			switch ( o_char )
			{
				case 'u' : mk_user_pass_l ( ) ;
					   break ;

				case 'd' : mk_dbname ( ) ;
					   break ;

				case 'r' : mk_cmd ("run") ;
					   break ;

				case 'b' : mk_cmd ("batch") ;
					   break ;

				case '?' : mk_argl ( '?' ) ;
					   more_args = FALSE ;
					   break ;
			}
		}


		/* ***** get other arguments ***** */

		if ( more_args )
			mk_argl ( ' ' ) ;


		/* ***** build up the arg line using the    ***** */
		/* ***** elements from sal structure        ***** */

		sql12_build_arg_line_from_struct ( args ) ;
	}
}

/*==========================================================================*/

static	void	mk_user_pass_l ( )
 
{
	int             sep_ind ;
	int             null ;

	DBG1 (( MF__,"********** mk_user_pass_l called *************\n" ))

	/* ***** initialize internel vars ***** */

	username_found = TRUE ;
	password_found = TRUE ;
	SAPDB_memset ( (char *) sal.username  , ' ' , sizeof ( sal.username  ) ) ;
	SAPDB_memset ( (char *) sal.password  , ' ' , sizeof ( sal.password )) ;
	sep_ind = 0 ;
	null = 0 ;


	/* ***** get user name ***** */

	mk_a_line ( optarg , & sep_ind , sizeof ( sal . username ), ',' ,
		    & null , sal . username ) ;
	DBG1 (( MF__,"mk_u_p : sal . username          = %.64s\n",
			 sal . username ))


	/* ***** get password  ***** */
	/* ***** ',' was found ***** */

	if ( optarg [ sep_ind ] == ',' )
	{
		null = 0 ;
		sep_ind ++ ;
		mk_a_line ( optarg , & sep_ind ,
			    sep_ind + ( int ) sizeof ( sal . password ) ,
			    ' ' , & null , sal . password ) ;
		DBG1 (( MF__,"mk_u_p : sal . password          = %.18s\n",
				 sal . password ))
	}
	else
	{
		/* ***** get password           ***** */
		/* ***** but first look for ',' ***** */

		sep_ind = str_chr  ( optarg , ',' , sep_ind ,
				     MX_LINE ) ;
		DBG1 (( MF__,"mk_u_p : sep_ind                 = %d\n", sep_ind ))
		if ( sep_ind != 0 )
		{
			null = 0 ;
			sep_ind ++ ;
			mk_a_line ( optarg , & sep_ind ,
				    ( int ) sizeof ( sal . password ) +
								     sep_ind ,
				    ' ' , & null , sal . password ) ;
			DBG1 (( MF__,"mk_u_p : sal . password          = %.18s\n",
					 sal . password ))
		}
	}
}

/*==========================================================================*/

static	void	mk_dbname ( )
 
{
	int             null ;
	char            * beg ;
	char            dbn_string [ MX_DBNAME + 1 ] ;

	DBG1 (( MF__,"********** mk_dbname called ******************\n" ))

	/* ***** initialize internel vars ***** */

	dbname_found = TRUE ;
	SAPDB_memset ( (char *) sal.dbname    , ' ' , MX_DBNAME       ) ;
	SAPDB_memset ( (char *) dbn_string    , ' ' , MX_DBNAME + 1   ) ;
	null = 0 ;


	/* ***** get the program name because only utility needs ***** */
	/* ***** the dbname quoted in "'" ( 'dbname' )           ***** */

	beg = strrchr ( argv [ 0 ] , '/' ) ;

	if ( beg == NULL )
		beg = argv [ 0 ] ;
	else
		beg ++ ;

	DBG1 (( MF__,"mk_dbn : beg                     = 0x%08lx\n",
			 (long) beg ))
	DBG1 (( MF__,"mk_dbn : beg                     = %.7s\n",
			 beg ))

	if ( ! strcmp ( beg , "utility" ) )
	{
		(void) strcpy ( dbn_string , "'" ) ;
		(void) strncat ( dbn_string , optarg , MX_DBNAME - 2 ) ;
		(void) strcat ( dbn_string , "'" ) ;
		DBG1 (( MF__,"mk_dbn : utility was called\n" ))
	}
	else
	{
		(void) strncpy ( dbn_string , optarg , MX_DBNAME ) ;
		DBG1 (( MF__,"mk_dbn : utility was not called\n" ))
	}

	DBG1 (( MF__,"mk_dbn : dbn_string              = %.65s\n",
			 dbn_string ))


	/* ***** get data base name ***** */

	mk_a_line ( dbn_string , & null , sizeof ( sal . dbname ), ' ' ,
		    & null , sal . dbname ) ;
	DBG1 (( MF__,"mk_dbn : sal . dbname            = %.64s\n",
			 sal . dbname ))
}

/*==========================================================================*/

static	void	mk_cmd ( cmd_type )
 
char                    * cmd_type ;

{
	int             null ;

	DBG1 (( MF__,"********** mk_cmd called *********************\n" ))
	DBG1 (( MF__,"mk_cmd : * cmd_type              = %s\n", cmd_type ))


	/* ***** initialize internel vars ***** */

	SAPDB_memset ( (char *) sal.cmd       , ' ' , sizeof(sal.cmd)  ) ;
	SAPDB_memset ( (char *) sal.filename  , ' ' , MAXVFNLENGTH     ) ;

	cmd_found = TRUE ;
	filename_found = TRUE ;
	null = 0 ;


	/* ***** get command ***** */

	mk_a_line ( cmd_type , & null , sizeof ( sal . cmd ),
		    ' ' , & null , sal . cmd ) ;
	DBG1 (( MF__,"mk_cmd : sal . cmd               = %.12s\n",
			 sal . cmd ))


	/* ***** get file name ***** */

	null = 0 ;
	mk_a_line ( optarg , & null , sizeof ( sal . filename ),
		    ' ' , & null , sal . filename ) ;
	DBG1 (( MF__,"mk_cmd : sal . filename          = %.64s\n",
			 sal . filename ))
}

/*==========================================================================*/

static	void	mk_argl ( o_char )
 
int                     o_char ;

{
	int             test_char_0 ;
	int             test_char_1 ;
	char            * cmd_type ;
	int             argn ;
	int             out_ind  ;
	int             size ;
	int             tmp_size ;
	int             null ;

	DBG1 (( MF__,"********** mk_argl called ********************\n" ))

	if ( o_char == '?' )
	{
		/* ***** look for option names ***** */
		/* ***** if found take over    ***** */

		DBG1 (( MF__,"mk_argl: optind  ( o_char = ? )  = %d\n", optind ))

		test_char_0 = argv [ optind - 1 ] [ 0 ] ;
		test_char_1 = argv [ optind - 1 ] [ 1 ] ;

		DBG1 (( MF__,"mk_argl: argv [ optind - 1 ]     = %s\n",
				 argv [ optind - 1 ]  ))
		DBG1 (( MF__,"mk_argl: test_char_0             = %c\n",
				 test_char_0 ))
		DBG1 (( MF__,"mk_argl: test_char_1             = %c\n",
				 test_char_1 ))

		if ( ( test_char_0 == '-' ) &&
		     ( test_char_1 == 'r' || test_char_1 == 'b' ) )
		{
			cmd_found = TRUE ;
			SAPDB_memset ( (char *) sal.cmd       , ' ' , sizeof(sal.cmd) ) ;
			switch ( test_char_1 )
			{
				case 'r' : cmd_type = "run" ;
					   break ;
				case 'b' : cmd_type = "batch" ;
					   break ;
			}


			/* ***** get command ***** */

			null = 0 ;
			mk_a_line ( cmd_type , & null , sizeof ( sal . cmd ),
				    ' ' , & null , sal . cmd ) ;
			DBG1 (( MF__,"mk_argl: sal . cmd               = %.12s\n",
					sal . cmd ))


			/* ***** 'STDIN' becomes filename ***** */

			filename_found = TRUE ;
			SAPDB_memset ( (char *) sal.filename  , ' ' ,
							      MAXVFNLENGTH ) ;
			null = 0 ;
			mk_a_line ( "STDIN" , & null , sizeof ( sal . filename ),
				    ' ' , & null , sal . filename ) ;
			DBG1 (( MF__,"mk_argl: sal . filename          = %.64s\n",
					sal . filename ))
		}
		else
		{
			/* ***** no regular option , reduce optind ***** */
			/* ***** but be sure to do this only you   ***** */
			/* ***** have found an '-?' expression     ***** */
			/* ***** ? : means only one character      ***** */

			if ( ( optind > 1 ) &&
			     ( strlen ( argv [ optind - 1 ] ) == 2 ) &&
			     ( test_char_0 == '-' ) )
				optind -- ;
		}
	}


	/* ***** take over args into arg line ***** */

	DBG1 (( MF__,"mk_argl: optind                  = %d\n", optind ))

	out_ind = - 1 ;
	size = sizeof ( sal . arguments ) ;
	tmp_size = sizeof ( sal . arguments ) ;
	arguments_found = TRUE ;
	SAPDB_memset ( (char *) sal.arguments , ' ' , MX_LINE ) ;

	for ( argn = optind ; argn < argc ; argn ++ )
	{
		null = 0 ;
		out_ind ++ ;
		mk_a_line ( argv [ argn ]  , & null , tmp_size , ' ' ,
			    & out_ind , sal . arguments ) ;
		tmp_size = size - out_ind + 1 ;
	}

	DBG1 (( MF__,"mk_argl: sal . arguments         = %.132s\n",
			sal . arguments ))
}

/*==========================================================================*/

static	void	mk_a_line ( arg_in , arg_in_ind , line_length , separator ,
						   arg_out_ind , arg_out )
 
char                    * arg_in ;
int                     * arg_in_ind ;
int                     line_length ;
int                     separator ;
int                     * arg_out_ind ;
char                    * arg_out ;

{
	int             arg_len ;
	int             in_ind ;
	int             out_ind ;

	DBG1 (( MF__,"********** mk_a_line called ******************\n" ))
	DBG1 (( MF__,"mk_line: arg_in                  = %s\n", arg_in ))
	DBG1 (( MF__,"mk_line: * arg_in_ind            = %d\n", * arg_in_ind ))
	DBG1 (( MF__,"mk_line: line_length             = %d\n", line_length ))
	DBG1 (( MF__,"mk_line: separator    in ' '     = '%c'\n", separator  ))
	DBG1 (( MF__,"mk_line: * arg_out_ind           = %d\n", * arg_out_ind ))


	/* ***** take all the elements you have got and make a line ***** */
	/* ***** how long is the next element ? ***** */

	arg_len = strlen ( arg_in ) ;

	DBG1 (( MF__,"mk_line: arg_len                 = %d\n",
			arg_len ))


	/* ***** put one element into the line ***** */

	for ( in_ind = * arg_in_ind , out_ind = * arg_out_ind ;
	      ( in_ind < arg_len )       &&
	      ( in_ind < line_length )   &&
	      ( arg_in [ in_ind ] != separator ) ;
	      out_ind ++ , in_ind ++                  )
	{
		arg_out [ out_ind ] = arg_in [ in_ind ] ;
	}

	* arg_out_ind = out_ind ;
	* arg_in_ind = in_ind ;

	DBG1 (( MF__,"mk_line: * arg_in_ind   end      = %d\n", * arg_in_ind ))
	DBG1 (( MF__,"mk_line: * arg_out_ind  end      = %d\n", * arg_out_ind ))
	DBG1 (( MF__,"mk_line: arg_out        end      = %.132s\n", arg_out ))
}

/*==========================================================================*/

static	void	sql12_build_arg_line_from_struct ( args )
 
char                    * args ;

{
	int             arg_ind ;
	int             arg_cnt1 ;
	int             arg_cnt2 ;
	int             arg_cnt3 ;
	int             i ;


	DBG1 (( MF__,"********** build_arg_line_from_struct *********\n" ))
	DBG1 (( MF__,"build_a: sal.username            = %.64s\n", sal.username  ))
	DBG1 (( MF__,"build_a: sal.password            = %.18s\n", sal.password  ))
	DBG1 (( MF__,"build_a: sal.dbname              = %.64s\n", sal.dbname    ))
	DBG1 (( MF__,"build_a: sal.cmd                 = %.12s\n", sal.cmd       ))
	DBG1 (( MF__,"build_a: sal.filename            = %.64s\n", sal.filename  ))
	DBG1 (( MF__,"build_a: sal.arguments           = %.132s\n", sal.arguments ))


	/* ***** initialize local vars ***** */

	arg_ind = 0 ;
	arg_cnt1 = 0 ;
	arg_cnt2 = 0 ;
	arg_cnt3 = 0 ;


	/* ***** build up user-passwd-dbname string ***** */

	arg_cnt1 = str_len ( sal . username , sizeof ( sal . username ) ) ;
	arg_cnt2 = str_len ( sal . password , MAXNAMELENGTH ) ;
	arg_cnt3 = str_len ( sal . dbname , MX_DBNAME ) ;

	if ( ( ( username_found ) && ( arg_cnt1 > 0 ) ) ||
	     ( ( password_found ) && ( arg_cnt2 > 0 ) ) ||
	     ( ( dbname_found   ) && ( arg_cnt3 > 0 ) )    )
	{
		args [ arg_ind ] = '(' ;
		arg_ind ++ ;

		for ( i = 0 ;
		      ( i < arg_cnt1 ) && ( arg_ind < MX_LINE ) ;
							  i ++ , arg_ind ++ )
			args [ arg_ind ] = sal . username [ i ] ;

		args [ arg_ind ] = ',' ;
		arg_ind ++ ;

		DBG1 (( MF__,"build_a: arg_ind behind user     = %d\n", arg_ind ))
		DBG1 (( MF__,"build_a: args                    = %.132s\n", args ))

		for ( i = 0 ;
		      ( i < arg_cnt2 ) && ( arg_ind < MX_LINE ) ;
							  i ++ , arg_ind ++ )
			args [ arg_ind ] = sal . password [ i ] ;

		args [ arg_ind ] = ',' ;
		arg_ind ++ ;

		DBG1 (( MF__,"build_a: arg_ind behind passwd   = %d\n", arg_ind ))
		DBG1 (( MF__,"build_a: args                    = %.132s\n", args ))

		for ( i = 0 ;
		      ( i < arg_cnt3 ) && ( arg_ind < MX_LINE ) ;
							   i ++ , arg_ind ++ )
			args [ arg_ind ] = sal . dbname [ i ] ;

		args [ arg_ind ] = ')' ;
		arg_ind ++ ;

		DBG1 (( MF__,"build_a: arg_ind behind dbname   = %d\n", arg_ind ))
		DBG1 (( MF__,"build_a: args                    = %.132s\n", args ))
	}


	/* ***** build up command string ***** */

	arg_cnt1 = str_len ( sal . cmd , sizeof(sal.cmd) ) ;

	if ( cmd_found && ( arg_cnt1 > 0 ) )
	{
		if ( arg_ind > 0 )
		{
			DBG1 (( MF__,"build_a: arg_ind before cmd      = %d\n",
					arg_ind ))
			args [ arg_ind ] = ' ' ;
			arg_ind ++ ;
		}

		for ( i = 0 ;
		      ( i < arg_cnt1 ) && ( arg_ind < MX_LINE ) ;
							   i ++ , arg_ind ++ )
			args [ arg_ind ] = sal . cmd [ i ] ;

		DBG1 (( MF__,"build_a: arg_ind behind cmd      = %d\n", arg_ind ))
		DBG1 (( MF__,"build_a: args                    = %.132s\n", args ))
	}


	/* ***** build up filename string ***** */

	arg_cnt1 = str_len ( sal . filename , MAXVFNLENGTH ) ;

	if ( filename_found && ( arg_cnt1 > 0 ) )
	{
		if ( arg_ind > 0 )
		{
			DBG1 (( MF__,"build_a: arg_ind before filename = %d\n",
					arg_ind ))
			args [ arg_ind ] = ' ' ;
			arg_ind ++ ;
		}

		args [ arg_ind ] = '\'' ;
		arg_ind ++ ;

		for ( i = 0 ;
		      ( i < arg_cnt1 ) && ( arg_ind < MX_LINE ) ;
							   i ++ , arg_ind ++ )
			args [ arg_ind ] = sal . filename [ i ] ;

		args [ arg_ind ] = '\'' ;
		arg_ind ++ ;

		DBG1 (( MF__,"build_a: arg_ind behind filename = %d\n", arg_ind ))
		DBG1 (( MF__,"build_a: args                    = %.132s\n", args ))
	}


	/* ***** build up argument string ***** */

	arg_cnt1 = str_len ( sal . arguments , MX_LINE ) ;

	if ( arguments_found && ( arg_cnt1 > 0 ) )
	{
		if ( arg_ind > 0 )
		{
			DBG1 (( MF__,"build_a: arg_ind before arguments= %d\n",
					arg_ind ))
			args [ arg_ind ] = ' ' ;
			arg_ind ++ ;
		}

		for ( i = 0 ;
		      ( i < arg_cnt1 ) && ( arg_ind < MX_LINE ) ;
							   i ++ , arg_ind ++ )
			args [ arg_ind ] = sal . arguments [ i ] ;

		DBG1 (( MF__,"build_a: arg_ind behind arguments= %d\n", arg_ind ))
		DBG1 (( MF__,"build_a: args                    = %.132s\n", args ))
	}
}

/*==========================================================================*/

static	int	str_len ( str , len )
 
char                    * str ;
int                     len ;

{
	int             i ;

	DBG1 (( MF__,"********** str_len called ********************\n" ))
	DBG1 (( MF__,"str_len: string                  = %.132s\n" , str ))
	DBG1 (( MF__,"str_len: len                     = %d\n", len ))

	/* ***** this function looks for the end of strings ***** */
	/* ***** which do not ends with \0                  ***** */

	for ( i = len - 1 ; i >= 0 ; i -- )
		if ( ( str [ i ] != ' ' ) && ( str [ i ] != '\0' ) )
			break ;

	DBG1 (( MF__,"str_len: i + 1                   = %d\n", i + 1 ))

	return ( i + 1 ) ;
}

/*==========================================================================*/

static	int	str_chr ( s1 , c , beg , end )
 
char                    * s1 ;
int                     c ;
int                     beg ;
int                     end ;

{
	int             i ;

	DBG1 (( MF__,"********** str_chr called ********************\n" ))
	DBG1 (( MF__,"str_chr: s1                      = %s\n" , s1 ))
	DBG1 (( MF__,"str_chr: c                       = %c\n", c   ))
	DBG1 (( MF__,"str_chr: beg                     = %d\n", beg ))
	DBG1 (( MF__,"str_chr: end                     = %d\n", end ))

	/* ***** str_chr returns the position of the first ***** */
	/* ***** occurrence in string s1 of character c     ***** */

	for ( i = beg ; ( i < end ) && ( s1 [ i ] != c ) ; i ++ ) ;

	if ( s1 [ i ] != c )
		i = 0 ;

	DBG1 (( MF__,"str_chr: i                       = %d\n", i ))

	return ( i ) ;
}

/*==========================================================================*/
#ifdef	INGO_TEST
#define	pri	(void) printf
extern	int	sql22_timestamp ;
global	int	_argc ;
global	int	*_argv ; /* The PTOC type for _argv is funny */

global	int	main ( argcount , argvector )
int			argcount ;
char			**argvector ;
{
    struct tsp4_xuser_record        user_params ;
    tsp00_Pw                        password ;
    struct tsp4_args_options        options ;
    tsp4_xuserset                   xusertype ;
    tsp00_ErrText                   errtext ;
    tsp00_Bool                      ok ;

    _argc = argcount ;
    _argv = (int*) argvector ; /* The PTOC type for _argv is funny */
    sql22_timestamp = FALSE ;
    sqlarg3 ( &user_params , password , &options , xusertype , errtext , &ok );
    pri ( "INGO: sqlarg3 ok: %d \n", ok );
    pri ( "INGO: sqlarg3 errtext: '%.40s'\n", errtext );
    pri ( "INGO: user   '%.64s'\n", user_params.xu_user );
    pri ( "INGO: passwd '%.18s'\n", password );
    pri ( "INGO: filenm '%.64s'\n", options.variant.C_sp4co_sql_precompiler.opt_modulefn_F );
/*
    pri ( "INGO: arglin '%.132s'\n", options.variant.C_sp4co_sql_precompiler.opt_ruser_args_F );
*/
    return ( 0 );
}
#endif
