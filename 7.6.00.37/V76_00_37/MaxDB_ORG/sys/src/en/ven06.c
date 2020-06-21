/*!
  @file           ven06.c
  @author         JoergM
  @brief          Client RunTime: Virtual File for Applications (old version for Pascal Tools)
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

#define MOD__ "ven06.c:"

#include "gen00.h"
#include "heo46.h"
#include "geo57.h"
#include "geo001.h"
#include "hen40.h"
#include "geo007_1.h"

#define         MAXBUFFERLENGTH         sizeof(tsp00_Page)
typedef tsp00_Page  *BUFFERADDRESS; /* XXX i know this depends on tsp00_Page, but this is the old virtual file code... */

/* =================== DEVENV ==============================================*/
#ifdef DEVENV
#include        <ctype.h>

		/*
		    All directory names are terminated with '/'.
		    This simplifies the build of a file name.
		*/
#define         CURR                    ""
#define		SRC                     "%s/sys/src/%s/"
#define		DSCR                    "%s/sys/desc/%s/"
#define		TBIN                    "%s/tool/bin/"
#define		TINP                    "%s/tool/inp/"
#define         TSRCVMAK                "%s/tool/src/vmake/"
#define		INC                     "%s/sys/wrk/incl/"
#define		SIZE                    "%s/sys/wrk/size/"
#define		TMP                     "%s/sys/wrk/%s/tmp/"
#define		OBJ                     "%s/sys/wrk/%s/obj/"
#define		PROT                    "%s/sys/wrk/%s/prot/"
#define		GSIZ                    "%s/sys/wrk/%s/gsize/"
#define		PSRC                    "%s/sys/psrc/%s/%s/"
#define		CSRC                    "%s/sys/csrc/%s/%s/"
#define		UPGM                    "%s/usr/pgm/" 

#define		CMD_SHELL               "sh"
#define		CMD_LINK                "ln"
#define		CMD_COPY                "cp"
#define		CMD_MOVE                "mv"
#define		CMD_REMOVE              "rm"
#define		CMD_CHDIR               "cd"

#define		COM_LST                 ".com"
#define		LIB_LST                 ".lib"
#define		REL_LST                 ".rel"
#define		SHR_LST                 ".shr"
#define		LNK_LST                 ".lnk"
#define		SHM_LST                 ".shm"
#define		MAC_LST                 ".mac"

#define		DSC_DEFAULT             "*"
#define		DSC_INC                 "inc"
#define		DSC_OPT                 "opt"

#define		EMPTY                   ""

#ifdef          SOCKET_SERVICE
#undef          SOCKET_SERVICE
#endif
#define         SOCKET_SERVICE          "vmake"
#define		SOCKET_PROTOCOL         "tcp"


/* special characters */

#define         NODE_SEPARATOR          ':'
#define         PATH_DELIMITER          ','
#define         DSC_DELIMITER           '='
#define         OPTION_SEPARATOR        '/'
#define         CMD_PREFIX              '!'

/*
    Vmake levels are used to limit the search for target types
    to those types, which apply to the respective target.
    For instance, a library can be built from modules only, or
    a program can be built from relocs, libraries and modules only.
    Thus a target never can include targets from lower levels.
    With this knowledge, the search for the respective description
    files can be accelerated by avoiding to search for impossible
    combinations.
*/

#define         LEVEL_MACRO             1
#define         LEVEL_PROGRAM           2
#define         LEVEL_RELOC             3
#define         LEVEL_LIBRARY           4
#define         LEVEL_MODULE            5

/* target types */

#define         TT_FILE                 1       /* file */
#define         TT_INC                  2       /* include module */
#define         TT_MEM                  3       /* module in library */
#define         TT_MOD                  4       /* independent module */
#define         TT_LIB                  5       /* library */
#define         TT_REL                  6       /* relocatable */
#define         TT_SHR                  7       /* reloc. with shared mem. */
#define         TT_PGM                  8       /* program */
#define         TT_SHM                  9       /* program with shared mem. */
#define         TT_MAC                  10      /* macro */
#define         TT_CMD                  11      /* shell command */

/* make options */

#define         MK_NONE                 0
#define         MK_NAME                 1
#define         MK_FULL                 2

/* remote server requests */

#define         REM_TIME                1
#define         REM_STAT                2

#define         REM_OPEN                11
#define         REM_CLOSE               12
#define         REM_READ                13
#define         REM_WRITE               14
#define         REM_SEEK                15

#define         REM_DIROPEN             41
#define         REM_DIRCLOSE            42
#define         REM_DIRREAD             43

#define         REM_INCOPY              51
#define         REM_OUTCOPY             52
#define         REM_LINK                53
#define         REM_UNLINK              54

#define         HYSTERESIS              300     /* max. time difference
						   allowed between machines */

/* file kind */

#define         FK_SOURCE		0
#define         FK_OBJECT		1
#define         FK_TOOL			2
/* FSt */
#define         FK_CURR_SOURCE          3
/* FSt */

/* host permission bits for search of file kinds */

#define		HOST_OBJECTS_DENIED	1
#define		HOST_TOOLS_DENIED	2

/* path status bits */

#define		PATH_OWN		0
#define		PATH_LOCAL		1
#define		PATH_REMOTE		2

/* virtual file types */

#define		FT_LOCAL		0
#define		FT_REMOTE		1

/* sizes */

#define         LAYERSIZE               4
#define         NAMESIZE                16
#define         FILENAMESIZE            256   /* FSt 80  */
#define         LINESIZE                256   /* FSt 256 */
#define         CMDBUFFERSIZE           5120
#define         MAXCMDARGS              640
#define         MAXREMOTEFILES          20
#define         MESSAGESIZE             (8192-128) /* allow for headers */
						   /* within 8K frame   */
#ifndef NODIR
# if            DIRSIZ < (NAMESIZE-1)
#  define       MAXFNMLGT               DIRSIZ
# else
#  define       MAXFNMLGT               (NAMESIZE-1)
# endif
#else
# define        MAXFNMLGT               (NAMESIZE-1)
#endif


/* types */


/* file system directory information */

struct dir_info
{
	struct dir_info         * next ;
	char                    dirpath [ FILENAMESIZE ] ;
	struct dir_entry        * entries ;
} ;

/* file system directory entry */

struct dir_entry
{
	struct dir_entry        * next ;
	char                    name [ NAMESIZE ] ;
} ;

/* layer compile information */

struct com_info
{
	struct com_info         * next ;
	char                    layer [ LAYERSIZE ] ;
	char                    version ;
	struct mod_com_info     * mod_com ;
} ;

/* module compile information */

struct mod_com_info
{
	struct mod_com_info     * next ;
	char                    name [ NAMESIZE ] ;
	char			special_target_flag ;
	char			version ;
	char			level_i ;
	char			level_s ;
	char			level_p ;
	char			level_c ;
	char			level_o ;
	char			shrglob ;
	char                    debug ;
	char                    profile ;
	struct dsc_info         * descriptions ;
	struct inc_info         * includes ;
} ;

/* library member information */

struct mem_info
{
	struct mem_info         * next ;
	char                    name [ NAMESIZE ] ;
	tsp00_Int4                date ;
} ;

/* target information */

struct trg_info
{
	struct trg_info         * next ;
	char                    name [ NAMESIZE ] ;
	int			basenamelen ;
	int                     type ;
	char                    version ;
	char                    language ;
	char                    layer [ LAYERSIZE ] ;
	char			level_i ;
	char			level_s ;
	char			level_p ;
	char			level_c ;
	char			level_o ;
	char                    shrglob ;
	char                    debug ;
	char                    profile ;
	char                    uncond ;
	int                     made ;
	char                    file [ FILENAMESIZE ] ;
	tsp00_Int4                date ;
	struct dsc_info         * descriptions ;
	struct dep_info         * dependencies ;
	struct dep_info         * callers ;
	struct mem_info         * members ;
} ;

/* optional target description information */

struct dsc_info
{
	struct dsc_info         * next ;
	char                    descriptor [ FILENAMESIZE ] ;
	char                    value [ FILENAMESIZE ] ;
} ;

/* dependency information */

struct dep_info
{
	struct dep_info         * next ;
	struct trg_info         * target ;
} ;

/* path information is used for VDN_PATH */

struct path_info
{
	struct path_info        * next ;
	int			host_permission ;
	int			path_status ;
	char                    path [ FILENAMESIZE ] ;
} ;

/* include information (00 modules) */

struct inc_info
{
	struct inc_info         * next ;
	char                    name [ NAMESIZE ] ;
	int			basenamelen ;
	char                    language ;
} ;

/* a remote VDN_PATH uses imakeserv: connection information */

struct imake_connection_info
{
	struct imake_connection_info    * next ;
	char                            node [ NAMESIZE ] ;
	int                             sd ;
	int                             cache_lgt ;
	char                            cache [ MESSAGESIZE ] ;
} ;

/* remote message struct, HEADER_SIZE is the part preceeding the data */

struct message
{
	char                    type ;
	char                    reference ;
	unsigned char           length [ 2 ] ;
#define         HEADER_SIZE     4
	char                    data [ MESSAGESIZE ] ;
} ;

/* file data from remote node is cached */

struct file_cache
{
	int                     remain ;
	int                     current ;
	int                     eof ;
	char                    data [ MESSAGESIZE ] ;
} ;

/* remote file struct */

typedef	struct
	{
		int                           in_use ;
		int                           ref ;
		struct imake_connection_info  * cip ;
		struct file_cache             * fcp ;
	}			RFILE ;

/* virtual (local or remote) file struct */

typedef	struct
	{
		int			type ;
		union
		{
			FILE			* l_fp ;
			RFILE			* r_fp ;
		}			v_fp ;
	}			VFILE ;



/* macros */


#define         SIG_RTRN        void
#ifdef global
#undef  global
#endif
#define		global		/* empty in C but clarifies declarations */

#if     defined(pyr) || defined(SCO)
#   ifdef CINT
#   undef  CINT
#   endif
#   define         CINT                    unsigned
#else
#   if     defined(OSF1)
#   define         CINT                    size_t
#   else
#   define         CINT                    int
#   endif
#endif

#if     defined(SCO)
#ifdef CPTR
#undef  CPTR
#endif
#define         CPTR                    void
#else
#define         CPTR                    char
#endif

#define	SPRI	(void) sprintf

#undef	FALSE
#define	FALSE		0
#undef	TRUE
#define	TRUE		1
#if !defined(OSF1)
#undef	NULL
#define	NULL		0L
#endif

/* function declarations */
char                    ** palloc ( );
char                    * ascdate ( );
char                    * versname ( );
char                    * getenv ( );
VFILE                   * fileopen ( );
char                    * filegets ( );
VFILE                   * diropen ( );
long                    atol ( );
#if ! ( defined(MI_ULT) || defined(OSF1) || defined(HPUX) )
long                    time ( );
#endif
static int              get_imake_path ();
static int              find_layer ();
static int              append_path ();


/* data used by vmake */

/* options */

char                    opt_VDNVERSION ;
int                     opt_interactive ;
int                     opt_watch ;
int                     opt_background ;
int                     opt_collection ;
int                     opt_modlist ;
int                     opt_print ;
int                     opt_noexec ;
int                     opt_retrieve ;
int                     opt_lint ;
int                     opt_sizeof ;
int                     opt_nodep ;
int                     opt_nameduncond ;
int                     opt_uncond ;
int                     opt_uncondincl ;
int                     opt_keep ;
int                     opt_profile ;
int			opt_man ;


/* data computed from options and environment */

int                     vopt_level_i ;
int                     vopt_level_s ;
int                     vopt_level_p ;
int                     vopt_level_c ;
int                     vopt_level_o ;
int			vopt_prefer_shm ;
int			vopt_timestamp ;
int                     vopt_source_warn = 0 ;
int                     vopt_source_copy = 0 ;
int                     vopt_object_warn = 0 ;
int                     vopt_object_copy = 0 ;
struct path_info        * vdn ;
char                    vdnversion ;
char                    vdndefault ;

/* cache for objects frequently used during collection */

struct dir_info         * dir_lists ;
struct com_info         * com_lists ;

/* the collection */

struct dep_info         * make_list ;
struct trg_info         * macros ;
struct trg_info         * programs ;
struct trg_info         * relocs ;
struct trg_info         * libraries ;
struct trg_info         * modules ;
struct trg_info         * includes ;
struct trg_info         * files ;
struct trg_info         * command_list ;

/* data used during the make phase */

pid_t                   vmakepid ;
char                    tmp_dir [ FILENAMESIZE ] ;
char                    vdnversion_env [ FILENAMESIZE ] ;
FILE                    * protfp ;
FILE                    * xprotfp ;
char                    protname [ FILENAMESIZE ] ;
char                    protvers ;
int                     cmdargc ;
char                    * cmdargv [ MAXCMDARGS ] ;
int                     cmdlgt ;
char                    cmdname [ LINESIZE ] ;
char                    cmd [ CMDBUFFERSIZE + LINESIZE ] ;
					   /* ^ security space */

#ifdef	SOCKETS
struct imake_connection_info  * connections ;
struct message          remote_msg ;
RFILE                   remote_file [ MAXREMOTEFILES ] ;
			/*
			    remote_file is an array instead of a chained list 
			    because the reference number for the vmakeserver
			    is the index of the remote file server.
			*/
#endif
#endif
/* =================== DEVENV END ==============================================*/

#define EOF_TRACKING

/*
 * Macros for system calls -- all of the macros return 
 * zero if the call is successfull, non-zero otherwise.
 */

#define VEN06_OPENREAD( name, hf )		((*(hf) = open( (name),      \
								O_RDONLY,    \
								0 ))  \
						 == UNDEF)

#define VEN06_OPENWRITE( name, hf )             ((*(hf) = open( (name),      \
								O_WRONLY +   \
								O_CREAT ,    \
								0666 ))  \
						 == UNDEF)

#define VEN06_OPENRDWR( name, hf )              ((*(hf) = open( (name),      \
								O_RDWR,      \
								0666 ))  \
						 == UNDEF)

#define VEN06_OPENCREAT( name, hf )		((*(hf) = open( (name),      \
								O_RDWR	     \
								 +O_CREAT,  \
								0666 ))  \
						 == UNDEF)

#define VEN06_CREATFILE( name, hf )		((*(hf) = creat( (name),     \
								 0666 )) \
						 == UNDEF)

#define VEN06_READFILE( fd, buf, cnt, actual, eof ) ((*(actual) =            \
							sql06_pread1 ( (fd), \
								 (buf),      \
								 (cnt),      \
								 (eof) ))    \
						     < 0 )

#define VEN06_WRITEFILE( fd, buf, cnt, actual )   ((long)( * (actual) =      \
							write( (fd),         \
							       (buf),        \
							       (cnt) ) )     \
						     < 0 )

#define VEN06_CLOSEFILE( fd )			    (close( fd ) == UNDEF)

#define VEN06_SEEKABS( fd, pos, newpos )            ((*(newpos) =            \
						       lseek( (int)(fd),     \
							      (long)(pos),   \
							      0 ))           \
						     == UNDEF)

#define VEN06_SEEKEND( fd, pos, newpos )            ((*(newpos) =            \
						       lseek( (int)(fd),     \
							      (long)(pos),   \
							       2 ))          \
						     == UNDEF)

#define VEN06_FILEDELETE( name )		    (unlink( name ) == UNDEF)


#define VEN06_LOCKFILE( hf , len )                  0

#if ( defined(SUN) && defined(SUNOS) ) || defined(OSF1)
#define OLD_VEN06_LOCKFILE( hf , len )                  0
#else
#define OLD_VEN06_LOCKFILE( hf , len )                  (sql06_fcntl ( hf , len ))
#endif

#ifdef DEVENV
#define VEN06_STAT( path, buf )             (filestat( (path), (buf) ) < 0)
#else
#define VEN06_STAT( path, buf )             (stat( (path), (buf) ) < 0)
#endif

#define VEN06_ISREGFILE(buf)  (((buf). st_mode & S_IFMT) == S_IFREG)
#define VEN06_ISPIPEFIFO(buf) (((buf). st_mode & S_IFMT) == S_IFIFO)
#define VEN06_ISSPECIAL(buf)  ((((buf).st_mode & S_IFMT) == S_IFCHR)\
			   || (((buf). st_mode & S_IFMT) == S_IFBLK))

#define VEN06_FILESIZE(buf)    (buf).st_size


#define 	   HOSTFILE_MIN        1
#define 	   HOSTFILE_MAX        8

/*
 * characters for new-line handling in Ascii files.  Note that the NEWLINE_LEN
 * symbol gives the amount of space required for line termination (2 under
 * OS/2 for \r\n, 1 under Unix for just \n)
 */

#define 	   CR		       '\r'
#define 	   NL		       '\n'

#define 	   NEWLINE_LEN	       1	/* only \n */

#define            MAXSEP               32      /* max index in sql06_sepset[] */
#define            MAXIGN               32      /* max index in sql06_ignset[] */

/*
 * other characters and assorted junk
 */

#define 	   NUL		       '\0'
#define 	   HT		       '\t'
#define            NL                  '\n'
#define 	   TABW 	       8
#define 	   MB2		       2000000000
#define 	   MAGIC_REC_LEN       8

#define 	   SL_TIME	       1

#define 	   KB08   	       8192
#define 	   KB32	               32768
#define            KB64                65536

#define 	   NO_BSS_BUFFERS      2 /* number of buffers init. in .bss */

#define            IN_USE              1
#define            NOT_IN_USE          0
#define            MAX_BLOCK_COUNT     16

#define 	   REGFILE	       1
#define 	   SPECFILE	       0
#define 	   FIFOFILE	       2
#define        SQL_DB_DEVSPACE     "SQL-DB DEVSPACE"

#define            std_in_fn           "STDIN.cmd"

#define 	   std_in	       "STDIN"
#define 	   std_out	       "STDOUT"
#define 	   std_err	       "STDERR"

#define 	   stdin_int	       0
#define 	   stdout_int	       1
#define 	   stderr_int	       2

#define 	   HFS_UNUSED	       1
#define            HFS_USED            2
#define            HFS_END             3
#define            HFS_ERROR           4

#ifdef  DEBUG_RTE
#define ERRTEXT(v,t)  { DBG1((MF__,t));\
		      eo46CtoP(v,t,MAXERRORTEXT); }
#else
#define ERRTEXT(v,t)  { eo46CtoP(v,t,MAXERRORTEXT);}
#endif

/*
 *  The "buf" paramter to VEN06_STAT should be the address
 *  of a variable of type VEN06_FILEDATA.  After the VEN06_STAT call
 *  is made, the buffer can be passed to VEN06_ISREGFILE, etc.
 */

typedef struct stat	 VEN06_FILEDATA;

/*
 * host file management structure.
 */

typedef UINT1                          HOSTFILE_STATE;
typedef struct	   HOSTFILE_ALL
    {
    VF_FILENAMEC     hf_name           /* null-terminated */;
    char             *hf_label          ;
#   ifdef DEVENV
    VFILE            * hf_rmfildes     /* remote vmake FILE DESCRIPTOR */;
#   endif
    int              hf_fildes         /* unix FILE DESCRIPTOR */;
    UINT1            hf_resource       /* stack, buffer pool */;
    tsp00_Int2         hf_bufidx         /* index of file's buffer in buf table */;
    UCHAR            *hf_bufptr        /* pointer to this file's buffer */;
    tsp00_Int4         hf_bufend         /* length of this file's buffer */;
    UINT1            *hf_block_beg_ptr /* start ptr in the buffer pool */;
    tsp00_Int4         hf_block_idx      /* index in free table */;
    tsp00_Int4         hf_block_len      /* number of bytes in the buffer pool */;
    tsp00_Int2         hf_block_cnt      /* number of blocks in the buffer pool */;
    tsp00_Int4         hf_reclen         /* fixed record length, or 0 == variable */;
    tsp00_Int4         hf_HeaderLength   /* number of bytes of the first page  */;
    tsp00_Int4         hf_pos            /* last pos read or to write, 0 - x */;
    tsp00_Int4         hf_maxpos         /* highest usable position in buffer */;
    tsp00_Int4         hf_bufbase        /* abs. file pos of data in hf_buf */;
    long             hf_byte_num       /* highest byte number in file */;
    tsp00_Int4         hf_readcnt        /* seek: count of bytes to read */ ;
    tsp00_Int4         hf_file_type      /* reg.file=1, fifo=2, char-,blockspec=0*/;
    UINT1            hf_format         /* stream, record, plaintext, unknown */;
    UINT1            hf_eff_format     /* only for reading, stream overlays */;
    tsp00_Bool          hf_blocked        /* only for ASCII files: blocked? */;
    HOSTFILE_STATE   hf_state          /* SEE ABOVE */;
    tsp00_VFileOpCodes      hf_direction      /* opcode from sqlfopen(), see above */;
    tsp00_Bool          hf_read_ora       /* reading oracle data? */;
    tsp00_Bool          hf_wr_prot        /* write protection for files? */;
    tsp00_Bool          hf_rd_eof         /* eof found, reading from tapes */;
    tsp00_Bool          hf_second_asc_rd  /* sqlfread sets this to true */;
    } HOSTFILE_ALL_TYPE;

typedef struct HOSTFILE_ALL * HF_PTR;

/*
 * Buffer array.  Here we allocate a small number of buffers (controlled
 * by NO_BSS_BUFFERS) statically, so that the dynamic buffer allocation
 * will not be used until necessary.  This allows programs which only
 * open one or two files to run without allocating dynamic memory.
 *
 * The t35extra field provides extra room to allow the buffers to be
 * aligned on a 32-byte boundary on Targon machines.
 */

typedef struct
    {
    UINT1                              t35extra[32];
    tsp00_Buf			       buf[NO_BSS_BUFFERS];                      
    }            VEN06_BUFFER;

/*
 * buffer control table.  This table provides, for each buffer, the
 * address of the buffers owner (== UNDEF if not in use), and the address
 * of the buffer itself (== UNDEF if not yet allocated).
 */

typedef struct
   {
   struct HOSTFILE_ALL                 *owner;
   char                                *address;
   }             VEN06_BUFTAB;


/*
 * Free storage management for buffer pool
 */

typedef struct
	{
		UINT1                   block_status ;
		UINT1                   * start_adr  ;
	}       VEN06_FREE_TABLE ;

/*
 * Controling number of host files
 */

typedef struct
   {
   int                             open_files ;
   int                             open_files_allowed ;
   }             VEN06_FILE_CNT ;


#ifdef NOT_USED
static char sql06_sepset [ MAXSEP ] =
{
    0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1
};
#endif

#ifdef NOT_USED
static char sql06_ignset [ MAXIGN ] =
{
    1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0
};
#endif


/*
 * signal handling
 */
static SIGNALFUNCTYPE     ( * oldsigpipe ) ( ) ;
static SIGNALFUNCTYPE     sql06c_catch_sigpipe () ;
static int      sql06c_set_signal () ;
static int      sql06c_reset_signal () ;
int             sql06c_last_fifo ;


/*
 * for LOCKFILE under Unix, we use a local file_lock structure.
 * This struct _MUST_ be initialized during the VFINIT call.
 */

static struct	   flock	       file_lock;

/*
 * file control table, static buffers, and buffer control table
 */

static struct      HOSTFILE_ALL        real_files [ HOSTFILE_MAX ];
static		   VEN06_BUFFER        buffers;
static             VEN06_BUFTAB        buftable [ HOSTFILE_MAX ];

static UINT1                           * buf_pool_first_block ;
static tsp00_Int2                        buf_pool_block_cnt ;
#ifdef NOT_USED
static tsp00_Bool                         larger_36kb_in_use ;
static tsp00_Int2                        static_ven06_buf_no ;
#endif
static VEN06_FREE_TABLE                free_table [ MAX_BLOCK_COUNT ] ;
static VEN06_FILE_CNT                  file_cnt ;

/*
 * flags set after first call to sqlfinit()
 */

static int                             hf_second = FALSE;

/*
 * error text
 */

static char BLANK_ERRTEXT []             = " ";
#ifdef soll_wieder_rein
static char CANNOT_CHECK_DEVSPACE []     = "Cannot check file for devspace";
#endif
#ifdef NOT_USED
static char CANNOT_READ_DEVSPACE []      =
				    "Cannot check for devspace, readerror";
#endif
#ifdef soll_wieder_rein
static char FILE_IS_DEVSPACE []          = "YOU ARE WRITING ON YOUR DEVSPACE";
static char CLOSE_DEVSPACE []            =
				    "Close ERROR on devspace test file";
#endif
static char NO_MORE_SPACE_IN_BUF_POOL [] = "No more space within buf pool";
static char BUF_NOT_ALLOC []             = "No memory for buffer";
static char CLOSE_ERRTEXT []             = "Close ERROR ";
static char CREATE_ERRTEXT []            = "Create ERROR";
static char FILE_EXIST []                = "File already exists";
static char FILE_NOT_FOUND_ERRTEXT []    = "File not found";
static char NULL_BYTE_FILE_ERRTEXT []    = "File not found ( 0 bytes )";
static char ILL_APPEND_DEVICE []         =
				    "Illegal device for direction append";
static char ILL_DIREC_ERRTEXT []         = "Illegal direction";
static char ILL_FILE_DESC_ERRTEXT []     = "Illegal UNIX-file description";
static char ILL_HOST_FILE_NAME []        = "Illegal host file name";
static char ILL_FILE_SPEC_ERRTEXT []     = "Illegal file specification";
/* static char ILL_READ_FORMAT_COMB []      = "Illegal read/formt combination"; */
static char ILL_WRITE_FORMAT_COMB []     = "Illegal write/format combination";
static char ILL_REC_LEN_ERRTEXT []       = "Illegal record length";
static char ILL_REC_NO_ERRTEXT []        = "Illegal record number";
static char MAX_REC_ERRTEXT []           =
				    "Record too big   ( > max record length )";
static char MIN_REC_ERRTEXT []           =
				    "Record too small ( < min record length )";
static char NOSEEK_ERRTEXT []            =
				    "Seeking not supported on this file";
static char NOSEEK_EOF_ERRTEXT []        = "No seeking beyond file end";
static char NO_FREE_FILE_DESCRIPTOR []   = "No free file descriptor found";
static char FILE_ALREADY_IN_USE []       = "This file is already in use";
static char NO_MORE_FILES_ALLOWED []     = "No more file descriptor allowed";
static char NO_WRITE_PROTEC_ERRTEXT []   = "Cannot get write protection";
static char OPEN_ERRTEXT []              = "Open ERROR";
static char READ_PERM_DEN_ERRTEXT []     = "Reading ERROR";
static char SEEK_ERRTEXT []              = "Seeking ERROR";
static char STDIO_IN_ERRTEXT []          =
				    "Illegal combination: write/app and stdin";
static char STDIO_OUT_ERRTEXT []         =
				    "Illegal combination: read and stdout/err";
static char UNLINK_ERRTEXT []            = "Unlink ERROR";
static char WRITE_PERM_DEN_ERRTEXT []    = "Writing ERROR";
#ifdef NOT_USED
static char EOFP_WRITE_PERM_DEN_ETXT []  = "Writing EOF page ERROR";
#endif
static char BAD_FILENO_ERRTEXT []        = "Invalid file number";
static char FN_EXPAND_ERRTEXT []         = "Error in expanding file name";
static char NO_REG_FILE []               = "No regular file";
static char AUTOLOADER_NOT_SUPPORTED []  = "Autoloader is not supported";

#ifdef DEBUG_RTE
static char    *dbg_opcodes[]  = {"vread","vwrite","vappend",
				  "voverwrite","vread_ora"} ;
static char    *dbg_resource[] = {"vf_stack","vf_bufferpool"} ;
static char    *dbg_format[]   = {"vf_plaintext","vf_record","vf_stream",
				  "vf_unknown"  ,"vf_4k_block"};
/*
static char    *dbg_return[]   = {"vf_ok","vf_notok","vf_eof","vf_noseek"};
*/
#endif

global	void			sqlfinit  ();
global	void			sqlfopen  ();
global	void			sqlfclose ();
global	void			sqlfread  ();
global	void			sqlfwrite ();
global	void			sqlfseek  ();
global	void			sqlferase ();

extern char *         sqlerrs();
extern void           sql48_change_log ();  /* changes a file name with logical
                                             * part(s) to an expanded file name */
extern unsigned       sleep ();
extern void           exit ();

extern char           * sql48_strloc ();
extern void           sql48_change_log ();


static int         e06_valid_fileno     ( );
static void        sql06_init_one       ( );
static int         sql06_blocked_file   ( );
static void        vfsearch             ( );
static void        sql06_open_buffer_pool_control ( );
static void        sql06_close_buffer_pool_control ( );
static void        sql06_open_stdio     ( );
static void        sql06_write_open     ( );
static void        sql06_overwrite_open ( );
static void        sql06_read_open      ( );
static void        sql06_append_open    ( );
#ifdef soll_wieder_rein
static void        sql06_devspace_and_type_check ( ) ;
#endif
static void        vfstatus             ( );
static void        vf_get_desc          ( );
static void        vf_put_desc          ( );
static void        e06_read_asc_var     ( );
static void        e06_read_bin_var     ( );
static void        e06_read_bin_fix     ( );
static void        e06_write_bin_var     ( );
static void        e06_write_asc_var     ( );
static void        e06_write_bin_fix     ( );
static void        e06_getbuffer         ( );
static void        e06_get_xk_buffer     ( );
static void        e06_putbuffer         ( );
static void        e06_pbuf_and_fill_up  ( );
static void        e06_put_xk_buffer     ( );
static void        e06_posbuffer         ( );
static void        clear_hf_buf          ( );
static int         sql06_pread1          ( );


/*==========================================================================*/

/*
 * Although the virtual file uses the value UNDEF to indicate an invalid
 * file handle, some of the tool components (precompiler) also assume that
 * 0 will also be recognized as invalid.
 */

static int                             e06_valid_fileno ( hf )
struct HOSTFILE_ALL                    * hf ;

{
    DBG1 (( MF__,"e06_valid_fileno: hf                          = 0x%x\n",
			      hf  ));

    if ( (hf == (HF_PTR) UNDEF || hf == 0) )
	return( FALSE );

    if ( hf->hf_state == HFS_UNUSED )
	return( FALSE );

    return( ( hf >= & real_files [ 0 ] &&
	      hf <= & real_files [ HOSTFILE_MAX - 1 ] ) );
}

/*==========================================================================*/

static void     sql06_init_one( hf, first )

struct HOSTFILE_ALL                    * hf ;
int                                    first ;

{
	tsp00_Int2                                   bufidx ;

	bufidx              = hf->hf_bufidx ;
					  /* save to reset buftable[].owner */

	hf -> hf_name [0]      = NUL ;
	hf -> hf_resource      = vf_stack ;
#       ifdef DEVENV
	hf -> hf_rmfildes      = ( VFILE * ) - 1 ;
#       endif
	hf -> hf_fildes        = - 1 ;
	hf -> hf_reclen        =   0 ;
	hf -> hf_pos           = - 1;
	hf -> hf_maxpos        = - 1 ;
	hf -> hf_bufbase       = - 1 ;
	hf -> hf_readcnt       =   0 ;
	hf -> hf_byte_num      = - 1 ;
	hf -> hf_format        = vf_unknown ;
	hf -> hf_eff_format    = vf_unknown ;
	hf -> hf_file_type     = UNDEF ;
	hf -> hf_blocked       = FALSE ;
	hf -> hf_state         = HFS_UNUSED ;
	hf -> hf_wr_prot       = FALSE ;
	hf -> hf_read_ora      = FALSE ;
	hf -> hf_rd_eof        = FALSE ;
	hf -> hf_second_asc_rd = FALSE ;
	hf -> hf_bufptr        = ( UCHAR * ) UNDEF ;
	hf -> hf_bufidx        = UNDEF ;
	hf -> hf_bufend        = 0 ;
	hf -> hf_block_beg_ptr = ( UINT1 * ) UNDEF ;
	hf -> hf_block_len     = 0 ;
	hf -> hf_block_cnt     = 0 ;

	if ( first == FALSE )
	    { /* not called from sqlfinit */
	    if ( bufidx != UNDEF )
		buftable [ bufidx ] . owner = (HF_PTR) UNDEF ;

	    return;
	    }


	/*
	 * This code initializes the Unix file lock structure for
	 * the system call macros.
	 */

	file_lock.l_type = F_WRLCK;
	file_lock.l_whence = 0 ;
	file_lock.l_start = 0 ;
	file_lock.l_len = 0 ;
	file_lock.l_pid = 1 ;

} /* sql06_init_one */


/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sqlfinit"
global	void	sqlfinit ( buf_pool_size , poolptr , ok )

tsp00_Int2                buf_pool_size ;
tsp00_Int4                * poolptr ;	/* despite name, not a ptr */
tsp00_Bool                 * ok ;

{ /* VFinit */

    int                 idx;


    DBG1 (( MF__,"called,    hf_second  %d \n", hf_second  ));
    DBG1 (( MF__,"sqlfinit        : buf_pool_size      %d \n", buf_pool_size ));

    if ( ! hf_second )
    {

	* ok = TRUE ;
	sql06c_last_fifo = 0 ;
	DBG1 (( MF__,"sqlfinit        : sql06c_last_fifo               = %d\n",
				  sql06c_last_fifo ))
	DBG1 (( MF__,"sqlfinit        : calls sql06_init_one \n" ));

	for ( idx = 0; idx < HOSTFILE_MAX ; idx ++ )
	{
	    sql06_init_one ( & real_files [ idx ] , TRUE );
	    buftable [ idx ] . owner   = (HF_PTR) UNDEF;
	    buftable [ idx ] . address = (char *) UNDEF;
	}

	for ( idx = 0; idx < NO_BSS_BUFFERS ; idx ++ )
	{
	    buftable [ idx ] . address = (char *) buffers . buf [ idx ];
	}

#ifdef NOT_USED
	static_ven06_buf_no = 0 ;
#endif


	/* **** allocate memory within data segment for buffer pool ***** */

	if ( buf_pool_size )
	{
	    if ( ( buf_pool_size < 0               ) ||
		 ( buf_pool_size > MAX_BLOCK_COUNT )    )
	    {
		* ok = FALSE ;
		DBG1 (( MF__,"sqlfinit        : illegal buf_pool_size          = %d\n",
					  buf_pool_size ))
		return ;
	    }


	    if ( ALLOC_MEM_EO57( (void **)&buf_pool_first_block, buf_pool_size * KB08 )
            != NO_ERROR_EO001 )
	    {
		* ok = FALSE ;
		* poolptr = UNDEF ;
		DBG1 (( MF__,"buf_pool_first_block not allocatable, %s\n", sqlerrs() ));
		return;
	    }
	    else
	    {
		* poolptr = idx ;	/* Is this ever used? 1995-03-08 glo */
	    }
	}
	else
	{
	    * poolptr = 0 ;
	}

	DBG1 (( MF__,"sqlfinit        : buf_pool_first_block           = 0x%x\n",
				  buf_pool_first_block ));
	DBG1 (( MF__,"sqlfinit        : * poolptr                      = %d\n",
				  * poolptr ));

	buf_pool_block_cnt   = buf_pool_size ;
#ifdef NOT_USED
	larger_36kb_in_use   = FALSE ;
#endif

	DBG1 (( MF__,"sqlfinit        : buf_pool_first_block           = 0x%x\n",
				  buf_pool_first_block ));
	DBG1 (( MF__,"sqlfinit        : buf_pool_block_cnt             = %d\n",
				  buf_pool_block_cnt ));


	/* **** free table initialization : only the requested number ***** */
	/* ****      of 4KB blocks get the "block status" NO_IN USE   ***** */
	/* ****      all the other ones get UNDEF                     ***** */

	for ( idx = 0 ; idx < buf_pool_size ; idx ++ )
	{
	    free_table [ idx ] . block_status = NOT_IN_USE ;
	    free_table [ idx ] . start_adr    =
				     ( buf_pool_first_block + idx * KB08 );

	    DBG1 (( MF__,"(1) freetab[i]. block_status = %d\n",
				      free_table [ idx ] . block_status )) ;
	    DBG1 (( MF__,"(1) freetab[i]. start_adr    = 0x%x\n",
				      free_table [ idx ] . start_adr )) ;
	}

	for ( idx = buf_pool_size ; idx < MAX_BLOCK_COUNT ; idx ++ )
	{
	    free_table [ idx ] . block_status = UNDEF ;
	    free_table [ idx ] . start_adr    = ( UINT1 * ) 0 ;

	    DBG1 (( MF__,"sqlfinit        : (2) freetab[i]. block_status = %d\n",
				      free_table [ idx ] . block_status )) ;
	    DBG1 (( MF__,"sqlfinit        : (2) freetab[i]. start_adr    = 0x%x\n",
				      free_table [ idx ] . start_adr )) ;
	}


	/* **** file control : if block size within buffer pool   ***** */
	/* ****                is larger than 36 kB the number of ***** */
	/* ****                simultanious open files decreases  ***** */

	file_cnt . open_files = 0 ;
	file_cnt . open_files_allowed = HOSTFILE_MAX ;

	hf_second = TRUE;
    }
    else
    {
	* ok = FALSE ;
    }


    DBG1 (( MF__,"returning 0 \n" ));

} /* sqlfinit */


/*==========================================================================*/

static int                             sql06_blocked_file (temp_name, format,
							   rec_len)

/*
 * returns  FALSE  if filename ends with .prot or .pct
 */

VF_FILENAMEC                           temp_name;
UINT1                                  format;
tsp00_Int4                               rec_len;

{ /* sql06_blocked_file */

    if ( ( format == vf_record ) || ( format == vf_stream ) || rec_len )
    {
	return ( TRUE );
    }

    if ( sql48_strloc ( temp_name , ".prt" ) )
    {
	return ( FALSE );
    }

    if ( sql48_strloc ( temp_name , ".PRT" ) )
    {
	return ( FALSE );
    }


    /*
     * old version of the Unix UTILITY produced debug files
     * with suffix .PROT
     */

    if ( sql48_strloc ( temp_name , ".prot" ) )
    {
	return ( FALSE );
    }

    /* if ( sql48_strloc ( temp_name , ".PROT" ) )
     * {
     *     return ( FALSE );
     * }
     */

    if ( sql48_strloc ( temp_name , ".pct" ) )
    {
	return ( FALSE );
    }

    if ( sql48_strloc ( temp_name , ".pcl" ) )
    {
	return ( FALSE );
    }

    if ( sql48_strloc ( temp_name , ".log" ) )
    {
	return ( FALSE );
    }

    if ( strrchr ( (const char *)temp_name, '.' ) )
    {
	return ( TRUE );
    }

    return ( TRUE );

} /* sql06_blocked_file */

/*==========================================================================*/

global	void	sqlfopen ( p1, direction, resource, hf_num, format, rec_len,
			   poolptr, buf_cnt, block, error, errtext )

tsp00_VFilename                   p1 ;
tsp00_VFileOpCodes                     direction ;
UINT1                           resource ;
/* !!!!   NEW PARAMETER !!! tsp_vf_label          f_label ;  */
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
UINT1                           * format ;
tsp00_Int4                        * rec_len ;
tsp00_Int4                        poolptr ;	/* despite name, not a ptr */
tsp00_Int2                        buf_cnt ;
tsp_vf_bufaddr                  * block ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{
   VF_FILENAMEC                 hostfilename ;
   VF_FILENAMEC                 host_file_name ;
   tsp00_Bool                      not_ok ;
   int                          file_allow_cnt ;
   CINT                         rc ;
   struct HOSTFILE_ALL          * hf;
#ifdef  DEVENV
   VF_FILENAMEC                  get_file_name ;
   VF_FILENAMEC                  path ;
   int                           lgt ;
   int                           remote_src ;
   tsp00_Int4                      date_src ;
   char                          layer [ 3 ] ;
   char                          * ptr ;
   struct path_info              * pip ;
#endif

   DBG1 (( MF__,"sqlfopen: called ========================================\n" ));

   /* ***** change file name string from EBCDIC to ASCII ***** */
   eo46PtoC ( (char *)host_file_name ,  p1 , sizeof ( tsp00_VFilename ) );

   DBG1 (( MF__,"sqlfopen        : host_file_name     (0)       = '%s'\n",
			     host_file_name ));
   DBG1 (( MF__,"sqlfopen        : direction                    = %s\n",
			     dbg_opcodes [ direction ]));
   DBG1 (( MF__,"sqlfopen        : resource                     = %s\n",
			     dbg_resource [ resource] ));
   DBG1 (( MF__,"sqlfopen        : * hf_num                     = %d\n",
			     * hf_num ));
   DBG1 (( MF__,"sqlfopen        : * format                     = %s\n",
			     dbg_format [ * format] ));
   DBG1 (( MF__,"sqlfopen        : * rec_len                    = %d\n",
			     * rec_len ));
   DBG1 (( MF__,"sqlfopen        : poolptr                      = 0x%x\n",
			     poolptr)) ;
   DBG1 (( MF__,"sqlfopen        : buf_cnt                      = %d\n",
			     buf_cnt )) ;
   DBG1 (( MF__,"sqlfopen        : block adr                    = 0x%x\n",
			     block )) ;
   DBG1 (( MF__,"sqlfopen        : errtext adr                  = 0x%x\n",
			     errtext )) ;


   /* ***** initializing ***** */

   * error = VF_OK ;
   SAPDB_memset ( errtext , ' ' , sizeof ( tsp00_ErrText ) ) ;


   /* ***** check host file name  ****** */

   rc = ( CINT ) strlen ( (const char *)host_file_name ) ;
   DBG1 (( MF__,"sqlfopen        : strlen ( (const char *)host_file_name )    = %d \n",
			     rc )) ;
   if ( ! rc )
   {
      * error = VF_NOTOK ;
      ERRTEXT( errtext , ILL_HOST_FILE_NAME );
      return ;
   }
   
   /* ***** check format and direction parameter ****** */

   /* noch wird die RTE-Spez. in diesem Punkt nicht so richtig
      befolgt !!!!!!
   if ( ( ( direction == VREAD_EN00 )    || ( direction == VREAD_ORA_EN00 )    &&
	( ( * format == vf_record ) || ( * format == vf_plaintext ) )  )
   {
      * error = VF_NOTOK ;
      ERRTEXT( errtext , ILL_READ_FORMAT_COMB );
      return ;
   }
   */

   if ( ( * format == vf_unknown )          &&
	( ( direction == VWRITE_EN00     ) ||
	  ( direction == VOVERWRITE_EN00 ) ||
	  ( direction == VAPPEND_EN00    )    )    )
   {
      * error = VF_NOTOK ;
      ERRTEXT( errtext , ILL_WRITE_FORMAT_COMB );
      return ;
   }


   if ( strchr ( (const char *)host_file_name , '$' ) )
   {
      /* ***** substitute logical parts ***** */

      sql48_change_log ( host_file_name, hostfilename , & not_ok );

      DBG1 (( MF__,"sqlfopen        : host_file_name     (1)       '%s' \n",
				host_file_name ));
      DBG1 (( MF__,"sqlfopen        : hostfilename                 '%s' \n",
				hostfilename ));

      (void) strcpy ( (char *)host_file_name, (const char *)hostfilename );
   }

   DBG1 (( MF__,"sqlfopen        : host_file_name     (2)       '%s' \n",
			     host_file_name ));

   if ( strcmp ( (const char *)host_file_name , std_in_fn ) == 0 )
   {
      (void) strcpy ( (char *)host_file_name , "STDIN\0");
      DBG1 (( MF__,"sqlfopen        : host_file_name     (2)      '%s' \n",
				host_file_name   ));
   }

   if ( * error )
   {
      * error = VF_NOTOK;
      ERRTEXT( errtext, FILE_NOT_FOUND_ERRTEXT );
      return;
   }

   /*
    * find entries in the file and buffer tables
    */

   if ( ( resource == vf_bufferpool ) && ( 9 < buf_cnt ) )
   {
      file_allow_cnt = 17 - buf_cnt ;
      if ( file_allow_cnt < file_cnt . open_files_allowed )
      {
	 file_cnt . open_files_allowed = file_allow_cnt ;
      }
   }

#ifdef  DEVENV
   /* ***** you must not read oracle data !! ***** */

   if ( direction == VREAD_EN00 )
   {

      /* ***** test local file read ***** */
      /* ***** current directory    ***** */

      if ( access ( host_file_name , 00 /* F_OK */ ) )
      {

	 /* ***** remote reading ***** */

	 get_imake_path ( );

	 strcpy ( get_file_name , host_file_name ) ;
	 find_layer ( host_file_name , layer ) ;

	 rc = ( CINT ) get_file ( get_file_name , SRC , layer , EMPTY ,
			 FK_SOURCE , host_file_name ,
			 & date_src , & remote_src ) ;


	 /* ***** check host file name  ****** */

	 if ( ! strlen ( host_file_name ) )
	 {
	    * error = VF_NOTOK ;
	    ERRTEXT( errtext , ILL_HOST_FILE_NAME );
	    return ;
	 }
      }
   }
#endif

   vfsearch ( host_file_name ,  hf_num , resource , buf_cnt , block ,
	      error , errtext  );

   if ( * error )
      return;

   hf = & real_files [ * hf_num - 1 ] ;

   if ( buf_cnt > 1        && *format  == vf_record && 
        resource == vf_bufferpool )
     { 
       /* 6.1.1 CL 14 ==> 03/07/95 
          F.H. in this case, "typeblock" is running and we have to
          handle the big HeaderPage ( > KB08 ) in vf_get_desc() and
          e06_getbuffer() !
          No buffering is used, so we can use the '*block" space of
          the bufferpool to read the HeaderPage !
       */ 
       hf->hf_HeaderLength = buf_cnt * MAXBUFFERLENGTH  ;
       hf->hf_bufptr  = (UCHAR *) * block ;
     }
   else
       hf->hf_HeaderLength = MAXBUFFERLENGTH ;

   if ( * error )
      return;

   (void) strncpy ( (char *)hf->hf_name, 
                    (const char *)host_file_name, MAXVFNLENGTH );

   hf->hf_label = NULL /* = NEW_PARAMETER f_label */ ; 
   hf->hf_name [ MAXVFNLENGTH ] = NUL ;
   hf->hf_resource = resource /* vf_stack */ ;

   if ( direction == VREAD_ORA_EN00 )
   {
      hf -> hf_direction  = VREAD_EN00 ;
      hf -> hf_read_ora   = TRUE ;
   }
   else
   {
      hf -> hf_direction  = direction;
      hf -> hf_read_ora   = FALSE ;
   }

   hf->hf_blocked = ( tsp00_Bool ) sql06_blocked_file ( host_file_name,
						   * format, * rec_len );

   DBG1 (( MF__,"sqlfopen        : hf -> hf_resource    (2)     = %s\n",
			     dbg_resource [ resource] ));
   DBG1 (( MF__,"sqlfopen        : hf -> hf_direction   (2)     = %s\n",
			     dbg_opcodes [ direction ]));
   DBG1 (( MF__,"sqlfopen        : hf -> hf_read_ora    (2)     = %d\n",
			     hf -> hf_read_ora ));

   if ( direction == VREAD_EN00 )
      hf -> hf_maxpos = 0;
   else
      hf -> hf_maxpos = hf->hf_bufend;

   if ( ( strcmp ( (const char *)host_file_name, std_in )  == 0  ) ||
	( strcmp ( (const char *)host_file_name, std_out ) == 0  ) ||
	( strcmp ( (const char *)host_file_name, std_err ) == 0  )    )
   {
      /* ***** stdio , STDIO ***** */

      sql06_open_stdio ( host_file_name , hf , format , rec_len ,
			 error ,  errtext );
   }
   else
   {
#     ifdef soll_wieder_rein
      /* ***** no stdio :                                ***** */
      /* ***** check for DEVSPACES,                      ***** */
      /* ***** get file type ( fifo, raw, regular file ) ***** */

      sql06_devspace_and_type_check ( hf , error , errtext ) ;

      if ( * error != VF_OK )
      {
	 sql06_init_one ( hf, FALSE );
	 DBG1 (( MF__,"sqlfopen        : ERROR in sql06_devspace_and_typ_check\n",
		                   0 ))
	 DBG1 (( MF__,"sqlfopen        : errtext                     = %.40s\n",
				   errtext ))
	 return ;
      }
#     endif

      switch ( direction)
      {
	 case VWRITE_EN00:
	    sql06_write_open ( host_file_name, hf, * format, * rec_len,
			       error, errtext );
	    break;

	 case VOVERWRITE_EN00:
	    sql06_overwrite_open ( host_file_name, hf, * format, * rec_len,
				   error, errtext );
	    break;

	 case VREAD_EN00: 
	 case VREAD_ORA_EN00:
	    sql06_read_open ( host_file_name, hf, * format, error, errtext );
	    break;

	 case  VAPPEND_EN00:
	    sql06_append_open ( host_file_name, hf, * format,
				* rec_len, error, errtext );
	    break;

	 default:
	    * error = VF_NOTOK;
	    hf->hf_state = HFS_ERROR;
	    ERRTEXT(  errtext, ILL_DIREC_ERRTEXT  );
	    break;
      }
   }

   if ( * error != VF_OK)
   {
      /* ***** close and place back                          ***** */
      /* ***** in case of DEVENV hf->hf_fildes is be set too ***** */

      if ( hf->hf_fildes != UNDEF )
      {
#        ifdef DEVENV
	 fileclose ( hf -> hf_rmfildes );
#        else
	 VEN06_CLOSEFILE ( hf -> hf_fildes );
#        endif

      }

      if ( resource == vf_bufferpool )
        sql06_close_buffer_pool_control ( hf , buf_cnt,
	  				 (tsp_vf_bufaddr) (*block),
					 error , errtext );

      /* FSt requirement from Rudi 1992-11-19
      if ( hf->hf_wr_prot == TRUE )
      {
	 DBG1 (( MF__,"sqlfopen        : wr protected, sleep SL_TIME = %d\n",
						       SL_TIME ));
	 (void) sleep ( SL_TIME );
      }
      */

      sql06_init_one ( hf, FALSE );

      if ( file_cnt . open_files > 0 )
      {
	 file_cnt . open_files -- ;
      }

      DBG1 (( MF__,"sqlfopen        : file_cnt . open_files        = %d\n",
				file_cnt . open_files )) ;

      * hf_num = ( UINT4 ) UNDEF;

   } /* close and place back */

   DBG1 (( MF__,"sqlfopen        : * error                      = %d\n",
			     * error ));
   DBG1 (( MF__,"sqlfopen: returning -------------------------------------\n" ));
}

/*==========================================================================*/

/*
 * PROCEDURE			       vfsearch
 *
 * First, tests the input file name for a suffix specifying a large
 * buffer, and removes the suffix if present.  Searches the real_files[]
 * table for an open entry.  If found, sets the hf_bufptr and hf_bufend
 * values for the corresponding buffer in the buffers[] table.
 */

static void     vfsearch ( hostfilename , hf_num , resource , buf_cnt , block ,
			   error , errtext )

VF_FILENAMEC                    hostfilename ;
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
UINT1                           resource ;
tsp00_Int2                        buf_cnt ;
tsp_vf_bufaddr                  * block ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{ /* vfsearch */

    int                             idx;
    char                            *newbuf;
    struct HOSTFILE_ALL             * hf;

    DBG1 (( MF__,"vfsearch: called \n" ));
    DBG1 (( MF__,"vfsearch        : hostfilename                 '%s'\n",
			      hostfilename ));
    DBG1 (( MF__,"vfsearch        : resource                     = %s\n",
			      dbg_resource [ resource] ));
    DBG1 (( MF__,"vfsearch        : block ( adr )                = 0x%x\n",
			      block ));

    * hf_num = ( UINT4 ) UNDEF ;
    * error  = VF_OK ;
    * block  = ( tsp_vf_bufaddr ) 0 ;

    for ( idx = 0; idx < HOSTFILE_MAX ; idx++ )
    {
	if ( strcmp ( (const char *)real_files [ idx ] . hf_name, 
                  (const char *)hostfilename ) == 0)
	{
	    /* ***** file already in use ***** */

	    DBG1 (( MF__,"vfsearch        : file in use \n" ));
	    * error = VF_NOTOK;
	    ERRTEXT ( errtext , FILE_ALREADY_IN_USE );
	    return;
	}
    }

    DBG1 (( MF__,"vfsearch        : idx :                        = %d\n",
			      idx ));
    DBG1 (( MF__,"vfsearch        : file_cnt . open_files        = %d\n",
			      file_cnt . open_files )) ;
    DBG1 (( MF__,"vfsearch        : file_cnt . open_files_allowed= %d\n",
			      file_cnt . open_files_allowed )) ;

    if ( file_cnt . open_files < file_cnt . open_files_allowed )
    {
	for ( idx = 0; idx < HOSTFILE_MAX ; idx++ )
	{
	    if ( real_files [ idx ] . hf_state == HFS_UNUSED )
	    {
		/* ***** file available ***** */

		hf = & real_files [ idx ] ;
		* hf_num = idx + 1;
		file_cnt . open_files ++ ;
		break;
	    }
	}

	DBG1 (( MF__,"vfsearch        : idx : file available         = %d\n",
				  idx ));
	DBG1 (( MF__,"vfsearch        : * hf                         = 0x%x\n",
				  * hf ));
	DBG1 (( MF__,"vfsearch        : * hf_num                     = %d\n",
				  * hf_num ));

	if ( * hf_num == (UINT4) UNDEF )
	{
	    *error = VF_NOTOK;
	    ERRTEXT(  errtext, NO_FREE_FILE_DESCRIPTOR );
	}
	else
	{
	     for ( idx = 0; idx < HOSTFILE_MAX ; idx++ )
		 if ( buftable[ idx ].owner == (HF_PTR) UNDEF )
		     break;

	    /*
	     * if necessary, allocate a buffer of the appropriate size
	     */
	    if ( buftable [ idx ] . address == (char *) UNDEF )
	    {
		if ( ALLOC_MEM_EO57( (void **)&newbuf, KB08) != NO_ERROR_EO001 )
		{
		    *error = VF_NOTOK;
		    ERRTEXT( errtext, BUF_NOT_ALLOC );
		    return;
		}
		buftable [ idx ] . address = newbuf;
	    }

	    /*
	     * install the buffer into the HOSTFILE_ALL structure
	     */
	    hf->hf_bufidx  = idx ;
	    hf->hf_bufptr  = ( UCHAR * ) buftable [ idx ] . address ;
	    hf->hf_bufend  = KB08 ;
	    buftable [ idx ] . owner   = hf ;


	    /* ***** buffer pool control ***** */

	    if ( resource == vf_bufferpool )
	    {
		DBG1 (( MF__,"vfsearch        : resouce == vf_bufferpool\n"  )) ;
		sql06_open_buffer_pool_control ( hf, buf_cnt ,
						 error , errtext );
		if ( * error )
		    return;
		else
		    * block = ( tsp_vf_bufaddr ) ( hf->hf_block_beg_ptr ) ;

		DBG1 (( MF__,"vfsearch        : hf->hf_block_beg_ptr         = 0x%x\n",
					  hf->hf_block_beg_ptr ));
		DBG1 (( MF__,"vfsearch        : * block                      = 0x%x\n",
					  * block ));

	    }
	}
    }
    else
    {
	*error = VF_NOTOK;
	ERRTEXT(  errtext, NO_MORE_FILES_ALLOWED );
    }

} /* VFsearch*/

/*==========================================================================*/

static void     sql06_open_buffer_pool_control ( hf , block_cnt ,
						 error , errtext )

struct HOSTFILE_ALL                    * hf;
tsp00_Int2                               block_cnt ;
tsp00_VfReturn                          * error;
tsp00_ErrText                            errtext;

{
    int                     idx ;
    int                     save_idx ;
    int                     buf_region_found ;

    DBG1 (( MF__,"sql06_open_buffer_pool_control: called \n" )) ;
    DBG1 (( MF__,"sql06_open_bpc  : block_cnt                  = %d\n",
			      block_cnt )) ;


    /* ***** initialize parameters ***** */

    * error = VF_OK ;


    /* ***** initialize internal vars ***** */

    idx = 0 ;
    buf_region_found = FALSE ;
    save_idx = 0 ;


    /* ***** if no buffer pool is allocated, you can leave here ***** */

    if ( buf_pool_first_block == 0 )
	return ;


    /* ***** reserve "block_cnt" * 4KB buffers out of the ***** */
    /* ***** buffer pool if it is possible                ***** */

    while ( ! buf_region_found )
    {
	/* ***** find first free block in the buffer pool ***** */

	DBG1 (( MF__,"sql06_open_bpc  : (1) save_idx                 = %d\n",
				      save_idx )) ;
	for ( idx = save_idx ; idx < MAX_BLOCK_COUNT ; idx ++ )
	{
	    if ( free_table [ idx ] . block_status == NOT_IN_USE )
	    {
		save_idx = idx ;
		break ;
	    }
	}

	DBG1 (( MF__,"sql06_open_bpc  : (1) idx                      = %d\n",
				      idx )) ;


	/* ***** if there is no more space in the buffer pool ***** */
	/* ***** the index becomes MAX_BLOCK_COUNT            ***** */

	if ( idx == MAX_BLOCK_COUNT )
	{
	    * error = VF_NOTOK ;
	    ERRTEXT(  errtext, NO_MORE_SPACE_IN_BUF_POOL );
	    return ;
	}

	/* ***** search for enough space in the buffer pool ***** */

	for ( idx = 0; idx < block_cnt && idx + save_idx < MAX_BLOCK_COUNT;
	      idx ++ )
	{
	    if ( free_table [ idx + save_idx ] . block_status != NOT_IN_USE )
	    {
		/* ***** currently you have not found ***** */
		/* ***** enough space                 ***** */

		save_idx += ( idx + 1 ) ;
		idx = 0 ;
		DBG1 (( MF__,"sql06_open_bpc  : (2) save_idx                 = %d\n",
					      save_idx )) ;
		DBG1 (( MF__,"sql06_open_bpc  : (3) idx                      = %d\n",
					      idx )) ;
		break ;
	    }
	}


	/* ***** do you have found enough space ? ***** */

	if ( idx == block_cnt )
	{
	    /* ***** you have found enough space ***** */
	    /* ***** in the buffer pool, so take ***** */
	    /* ***** over the start address,     ***** */
	    /* ***** block count and length      ***** */

	    buf_region_found = TRUE ;
	    hf -> hf_block_beg_ptr = free_table [ save_idx ] . start_adr ;
	    hf -> hf_block_idx     = save_idx ;
	    hf -> hf_block_cnt     = block_cnt ;
	    hf -> hf_block_len     = block_cnt * KB08 ;

	    DBG1 (( MF__,"sql06_open_bpc  : free_table[savidx].start_adr = 0x%x\n",
				      free_table [ save_idx ] . start_adr )) ;
	    DBG1 (( MF__,"sql06_open_bpc  : hf -> hf_block_len           = %d\n",
				      hf -> hf_block_len )) ;


	    /* ***** mark free table entries as  ***** */
	    /* ***** IN_USE                      ***** */

	    for ( idx = 0 ; idx < block_cnt ; idx ++ )
	    {
		free_table [ idx + save_idx ] . block_status = IN_USE ;

		DBG1 (( MF__,"sql06_open_bpc  : free_tab[savidx+idx].blk_stat= %d\n",
					  free_table [ idx + save_idx ] .
					  block_status )) ;
	    }
	}


	/* ***** are you at the end of the buffer pool ? ***** */

	if ( ( idx + save_idx ) >= MAX_BLOCK_COUNT )
	{
	    save_idx = MAX_BLOCK_COUNT ;
	}
    }
}  /* sql06_open_buffer_pool_control */

/*==========================================================================*/

/*ARGSUSED*/ /* errtext unused */
static void     sql06_close_buffer_pool_control ( hf , block_cnt ,
						  block_beg_ptr ,
						  error , errtext )

struct HOSTFILE_ALL                     * hf;
tsp00_Int2                                block_cnt ;
tsp_vf_bufaddr                          block_beg_ptr ;
tsp00_VfReturn                           * error ;
tsp00_ErrText                             errtext ;

{
    int                                 idx ;
    int                                 i ;


    DBG1 (( MF__,"sql06_close_buffer_pool_control: called \n" )) ;
    DBG1 (( MF__,"sql06_close_bpc : block_cnt                (1) = %d\n",
			      block_cnt )) ;
    DBG1 (( MF__,"sql06_close_bpc : block_beg_ptr            (1) = 0x%x\n",
			      block_beg_ptr )) ;
    DBG1 (( MF__,"sql06_close_bpc : hf -> hf_block_idx       (1) = %d\n",
			      hf -> hf_block_idx )) ;
    DBG1 (( MF__,"sql06_close_bpc : hf -> hf_block_cnt       (1) = %d\n",
			      hf -> hf_block_cnt )) ;
    DBG1 (( MF__,"sql06_close_bpc : buf_pool_block_cnt       (1) = %d\n",
			      buf_pool_block_cnt )) ;
    DBG1 (( MF__,"sql06_close_bpc : hf -> hf_block_beg_ptr   (1) = 0x%x\n",
			      hf -> hf_block_beg_ptr )) ;


    /* ***** initialize parameters ***** */

    /*IST 1995-10-30
     *  don't initialize 'error'.
     *  It may already carry an error indication and must not
     *  be cleared when close_buffer_pool_control succeeds.
     *  * error = VF_OK ;
     */


    /* ***** if no buffer pool is allocated, you can leave ***** */

    if ( buf_pool_first_block == 0 )
	return ;


    /* ***** ERROR situations occured ?? ***** */

    if ( hf -> hf_block_idx + hf -> hf_block_cnt > buf_pool_block_cnt )
    {
	/* ***** you would free blocks beyond the end ***** */
	/* ***** of the buffer pool                   ***** */

	/*   !!!!!!!!!!!!!!!!!!!!!! FSt
	* error = VF_NOTOK ;
	ERRTEXT(  errtext, NO_MORE_SPACE_IN_BUF_POOL );
	*/
	DBG1 (( MF__,"sql06_close_bpc : hf -> hf_block_idx           = %d\n",
				  hf -> hf_block_idx )) ;
	DBG1 (( MF__,"sql06_close_bpc : hf -> hf_block_cnt           = %d\n",
				  hf -> hf_block_cnt )) ;
	DBG1 (( MF__,"sql06_close_bpc : buf_pool_block_cnt           = %d\n",
				  buf_pool_block_cnt )) ;
	return ;
    }

    if ( block_cnt != hf -> hf_block_cnt )
    {
	/* ***** you would free a number of blocks ***** */
	/* ***** which is not equal to the number  ***** */
	/* ***** of blocks which are allocated     ***** */

	/*   !!!!!!!!!!!!!!!!!!!!!! FSt
	* error = VF_NOTOK ;
	ERRTEXT(  errtext, NO_MORE_SPACE_IN_BUF_POOL );
	*/
	DBG1 (( MF__,"sql06_close_bpc : hf -> hf_block_cnt           = %d\n",
				  hf -> hf_block_cnt )) ;
	DBG1 (( MF__,"sql06_close_bpc : block_cnt                    = %d\n",
				  block_cnt )) ;
	return ;
    }

    if ( block_beg_ptr != ( tsp_vf_bufaddr ) hf -> hf_block_beg_ptr )
    {
	/* ***** you would free blocks starting at ***** */
	/* ***** an address which is not equat to  ***** */
	/* ***** starting address in "sqlfopen"    ***** */

	/*   !!!!!!!!!!!!!!!!!!!!!! FSt
	* error = VF_NOTOK ;
	ERRTEXT(  errtext, NO_MORE_SPACE_IN_BUF_POOL );
	*/
	DBG1 (( MF__,"sql06_close_bpc : hf -> hf_block_beg_ptr       = 0x%x\n",
				  hf -> hf_block_beg_ptr )) ;
	DBG1 (( MF__,"sql06_close_bpc : block_beg_ptr                = 0x%x\n",
				  block_beg_ptr )) ;
	return ;
    }


    /* ***** now you can free the reserved blocks ***** */
    /* ***** from the buffer pool                 ***** */

    for ( idx = hf -> hf_block_idx , i = 0 ; i < hf -> hf_block_cnt ; i ++ )
    {
	free_table [ idx + i ] . block_status = NOT_IN_USE ;
	DBG1 (( MF__,"sql06_close_bpc : idx + i                      = %d\n",
				  idx + i )) ;
	DBG1 (( MF__,"sql06_close_bpc : freetable[idx+i].block_status= %d\n",
				  free_table [ idx + i ] . block_status )) ;
    }

}  /* sql06_close_buffer_pool_control */

/*==========================================================================*/

static void     sql06_open_stdio   ( temp_name , hf , format , rec_len ,
				     error , errtext )
 
VF_FILENAMEC			       temp_name ;
struct HOSTFILE_ALL                    * hf ;
UINT1                                  * format ;
tsp00_Int4                               * rec_len ;
tsp00_VfReturn                          * error ;
tsp00_ErrText                            errtext ;

{
	DBG1 (( MF__,"sql06_open_stdio: called \n" ));
	*error = VF_OK;
	if   ( strcmp( (const char *)temp_name, std_in )== 0 )
	{ /* stdin */
	    if   ( ( hf->hf_direction == VWRITE_EN00 )     ||
		( hf->hf_direction == VOVERWRITE_EN00 ) ||
		( hf->hf_direction == VAPPEND_EN00 )       )
	    {
		*error = VF_NOTOK ;
		ERRTEXT( errtext, STDIO_IN_ERRTEXT );
		hf->hf_state = HFS_ERROR;
	    }
	    else
	    { /* read */
		hf->hf_fildes =  stdin_int ;    /* __iob in *.s file */
		hf->hf_state = HFS_USED ;
		vf_get_desc ( hf , * format , error , errtext ) ;
		if ( *error == VF_OK )
		{
		    if ( * format == vf_stream )
		    {
			 hf->hf_eff_format = * format ;
		    }
		    else /* * format != vf_stream */
		    {
			 *format  = hf->hf_format ;
		    }
		    *rec_len = hf->hf_reclen ;
		}
	    } /* read */
	}   /* stdin */
	else
	{ /* no stdin */
	    if   ( hf->hf_direction == VREAD_EN00 )
	    {
		*error = VF_NOTOK;
		ERRTEXT( errtext, STDIO_OUT_ERRTEXT );
		hf->hf_state = HFS_ERROR ;
	    }
	    else
	    { /* no read */
		if   ( strcmp ( (const char *)temp_name, std_out ) == 0   )
		{
		    hf->hf_fildes =  stdout_int ; /* __iob+16 in *.s file */
		}
		else
		{
		    hf->hf_fildes =  stderr_int ; /* __iob+32 in *.s file */
		}

		hf->hf_format = *format ;
		hf->hf_reclen = *rec_len;
		hf->hf_byte_num = 0 ;
		hf->hf_state = HFS_USED ;
		vf_put_desc ( hf , error , errtext );
	    } /* no read */
	} /* no stdin */
}

/*==========================================================================*/

static void     sql06_write_open ( temp_name , hf , format , rec_len ,
				   error , errtext )

VF_FILENAMEC                    temp_name ;
struct HOSTFILE_ALL             * hf;
UINT1                           format ;
tsp00_Int4                        rec_len ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{
   int                          openstatus;
   long                         seekpos;

   openstatus = VEN06_OPENWRITE ( (const char *)temp_name , & hf -> hf_fildes );

   hf -> hf_reclen   = rec_len ;
   hf -> hf_format   = format ;
   hf -> hf_byte_num = 0 ;

   if ( openstatus == 0 )
   {
      /* ***** file descriptor ok ***** */
      vfstatus ( hf, error, errtext );

      if ( hf -> hf_file_type == FIFOFILE )
        {
          sql06c_set_signal () ;
          sql06c_last_fifo ++  ;
          DBG1 (( MF__,"sql06_write_open: sql06c_last_fifo             = %d\n",
	   			    sql06c_last_fifo ))
       }

      if ( VEN06_LOCKFILE ( hf , hf -> hf_reclen ) == 0 )
      {
	 /* ***** fcntl ok ***** */

	 hf -> hf_wr_prot = TRUE;

	 DBG1 (( MF__,"sql06_wri : hf->hf_wr_prot  %d \n", hf->hf_wr_prot  ));

	 if ( *error == VF_OK)
	 {
	    if ( hf -> hf_file_type == REGFILE )
	    {
	       /* ***** regfile ***** */

	       if ( hf -> hf_byte_num == 0 )
	       {
		  hf -> hf_state = HFS_USED;
		  vf_put_desc ( hf, error, errtext );
	       }
	       else
	       {
		  hf->hf_state  = HFS_ERROR;
		  *error       = VF_NOTOK;
		  ERRTEXT( errtext, FILE_EXIST);
		  VEN06_CLOSEFILE( hf->hf_fildes );
		  hf->hf_fildes = UNDEF;
	       }
	    }
	    else
	    {
	       /* ***** no reg file ***** */

	       if ( hf -> hf_file_type != FIFOFILE )
	       {
		  /* ***** goto the beginning only if the files is ***** */
		  /* ***** not a pipe or a fifo  ;                 ***** */
		  /* ***** the file can be a tape or an other      ***** */
		  /* ***** special device .                        ***** */

		  if ( VEN06_SEEKABS ( hf->hf_fildes, 0, &seekpos ) != 0 )
		  {
		     *error       = VF_NOTOK;
		     ERRTEXT( errtext, SEEK_ERRTEXT);
		     hf->hf_state  = HFS_ERROR;
		     DBG1 (( MF__,"sql06_write_open: special file, SOF ERROR\n" ))
		  }
		  else
		  {
		     hf->hf_state = HFS_USED;
		     vf_put_desc ( hf, error, errtext );
		  }
	       }
	       else
	       {
		  hf->hf_state  = HFS_USED;
		  vf_put_desc ( hf , error , errtext );
	       }
	    }
	 }
	 else
	 {
	    hf->hf_state = HFS_ERROR;
	 }
      }
      else
      {
	 /* ***** fcntl not ok ***** */

	 *error       = VF_NOTOK;
	 ERRTEXT( errtext, NO_WRITE_PROTEC_ERRTEXT );
	 hf->hf_state  = HFS_ERROR;
	 VEN06_CLOSEFILE( hf->hf_fildes );
	 hf->hf_fildes = UNDEF;
      }
   }
   else
   {
      hf->hf_fildes = UNDEF;
      hf->hf_state  = HFS_ERROR;
      *error        = VF_NOTOK;
      ERRTEXT( errtext, OPEN_ERRTEXT);
   }

   DBG1 (( MF__,"sql06_wri : hf_reclen            %d \n", hf->hf_reclen));
   DBG1 (( MF__,"sql06_wri : hf_state             %d \n", hf->hf_state ));
   DBG1 (( MF__,"sql06_wri : error                %d \n", *error       ));
   DBG1 (( MF__,"sql06_wri : returning -----------------------------------\n" ));
}

/*==========================================================================*/

static void     sql06_overwrite_open ( temp_name , hf , format , rec_len ,
				       error , errtext )

VF_FILENAMEC                    temp_name ;
struct HOSTFILE_ALL             * hf ;
UINT1                           format ;
tsp00_Int4                        rec_len ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{
   int                          openstatus ;

   DBG1 (( MF__,"sql06_ovrwr_open: called ================================\n" ));


   DBG1 (( MF__,"sql06_ovrwr_open: temp_name                    = '%s'\n" ,
			     temp_name )) ;
   DBG1 (( MF__,"sql06_ovrwr_open: rec_len                      = %d\n" ,
			     rec_len )) ;
   DBG1 (( MF__,"sql06_ovrwr_open: format                       = %s\n" ,
			     dbg_format [ format ] ));

   openstatus = VEN06_OPENRDWR ( (const char *)temp_name , & ( hf -> hf_fildes ) );

   DBG1 (( MF__,"sql06_ovrwr_open: openstatus                   = %d\n" ,
			     openstatus )) ;
   DBG1 (( MF__,"sql06_ovrwr_open: hf -> hf_fildes              = %d\n" ,
			     hf -> hf_fildes )) ;

   if ( openstatus == 0 )
   {
      /* ***** fildes ok ***** */

      DBG1 (( MF__,"sql06_ovrwr_open: file exists, temp_name       '%s'\n" ,
					     temp_name )) ;
      DBG1 (( MF__,"sql06_ovrwr_open: hf -> hf_reclen              = %d\n" ,
				hf -> hf_reclen )) ;

      vfstatus ( hf , error , errtext ) ;

      if ( hf -> hf_file_type == FIFOFILE )
      {
         sql06c_set_signal () ;
         sql06c_last_fifo ++  ;
         DBG1 (( MF__,"sql06_ovrwr_open: sql06c_last_fifo             = %d\n" ,
   				sql06c_last_fifo ))
      }

      /* ***** lock file with fcntl system call ***** */

      if ( VEN06_LOCKFILE( hf, rec_len ) == 0 )
      {
	 /* ***** fcntl ok ***** */

	 DBG1 (( MF__,"sql06_ovrwr_open: fcntl ok, file locked\n" ));

	 if ( hf->hf_file_type == REGFILE )
	 {
	    /* ***** regfile ***** */

	    DBG1 (( MF__,"sql06_ovrwr_open: unlink temp_name            '%s'\n" ,
					     temp_name ));

	    if ( VEN06_FILEDELETE( (char *) temp_name ) != 0 )
	    {
	       /* ***** unlink ***** */

	       hf->hf_state = HFS_ERROR;
	       *error       = VF_NOTOK;
	       ERRTEXT( errtext, UNLINK_ERRTEXT );
	    }
	 }

         if ( hf -> hf_file_type != FIFOFILE &&
              VEN06_CLOSEFILE( hf->hf_fildes ) == UNDEF )
	 {
	    hf->hf_state = HFS_ERROR;
	    *error       = VF_NOTOK;
	    ERRTEXT( errtext, CLOSE_ERRTEXT );
	 }

	 if ( hf->hf_state != HFS_ERROR )
	 {
	    /* ***** close ok ***** */

           if ( hf -> hf_file_type != FIFOFILE )
	     { DBG1 (( MF__,"sql06_ovrwr_open: create temp_name            '%s'\n" ,
	  				        temp_name ));
	       openstatus = VEN06_CREATFILE ( (const char *)temp_name , & hf->hf_fildes );
              }
            else
	       openstatus = 0 ;

	    hf->hf_reclen = rec_len ;
	    hf->hf_format = format ;
	    hf->hf_byte_num = 0 ;

	    if ( openstatus == 0 )
	    {
	       /* ***** Schreibsperre setzen ***** */

	       vfstatus ( hf , error , errtext ) ;

	       if ( VEN06_LOCKFILE ( hf , hf -> hf_reclen ) == 0)
	       {
		  /* ***** lock ok ***** */

		  hf->hf_wr_prot = TRUE ;

		  DBG1 (( MF__,"sql06_ovrwr_open: hf -> hf_wr_prot            = %d\n",
					    hf -> hf_wr_prot  ));

		   if ( *error == VF_OK )
		   {
		      hf->hf_state = HFS_USED ;
		      vf_put_desc ( hf , error , errtext );
		   }
		   else
		   {
		      hf->hf_state = HFS_ERROR ;
		   }
	       }
	       else
	       {
		  /* ***** lock not ok ***** */

		  hf->hf_state = HFS_ERROR;
		  *error       = VF_NOTOK;
		  ERRTEXT( errtext, NO_WRITE_PROTEC_ERRTEXT );

		  if ( VEN06_CLOSEFILE( hf->hf_fildes ) != 0 )
		  {
		     hf->hf_state = HFS_ERROR;
		     *error       = VF_NOTOK;
		     ERRTEXT( errtext, CLOSE_ERRTEXT );
		  }
		  else
		  {
		     hf->hf_fildes = UNDEF ;
		  }
	       }
	    }
	    else
	    {
	       /* ***** fildes not ok ***** */

	       hf->hf_state = HFS_ERROR;
	       *error       = VF_NOTOK;
	       ERRTEXT( errtext, CREATE_ERRTEXT );
	    }
	 }
      }
      else
      {
	 /* ***** fcntl not ok ***** */

	 hf->hf_state = HFS_ERROR;
	 *error       = VF_NOTOK;
	 ERRTEXT( errtext, NO_WRITE_PROTEC_ERRTEXT );

	 if ( VEN06_CLOSEFILE( hf->hf_fildes ) != 0 )
	 {
	    /* ***** close error ***** */
	    hf->hf_state = HFS_ERROR;
	    *error       = VF_NOTOK;
	    ERRTEXT( errtext, CLOSE_ERRTEXT );
	 }
	 else
	 {
	    hf->hf_fildes = UNDEF ;
	 }
      }
   }
   else
   {
      /* ***** fildes not ok                      ***** */
      /* ***** zu oeffnendes File existierte noch ***** */
      /* ***** nicht => kann kreiert              ***** */
      /* ***** werden                             ***** */

      DBG1 (( MF__,"sql06_ovrwr_open: file does not exist\n"  )) ;

      openstatus = VEN06_CREATFILE( (const char *)temp_name, &hf->hf_fildes );

      hf -> hf_reclen = rec_len ;
      hf -> hf_format = format ;
      hf -> hf_byte_num = 0 ;

      if ( openstatus == 0 )
      {
	  vfstatus ( hf , error , errtext ) ;

	  /* ***** Schreibsperre setzen ***** */

	  if ( VEN06_LOCKFILE ( hf, hf->hf_reclen ) == 0 )
	  {
	     /* ***** fcntl ok ***** */

	     hf -> hf_wr_prot = TRUE ;

	     DBG1 (( MF__,"sql06_ovrwr_open: hf -> hf_wr_prot            = %d\n",
				       hf -> hf_wr_prot  ));

	     if ( * error == VF_OK )
	     {
		/* ***** file status ok ***** */

		hf -> hf_state = HFS_USED ;
		vf_put_desc ( hf , error , errtext );
	     }
	     else
	     {
		/* ***** file status not ok ***** */

		hf -> hf_state = HFS_ERROR ;
	     }
	  }
	  else
	  {
	     /* ***** fcntl not ok ***** */

	     *error = VF_NOTOK ;
	     hf->hf_state = HFS_ERROR;
	     ERRTEXT( errtext, NO_WRITE_PROTEC_ERRTEXT );

	     if ( VEN06_CLOSEFILE( hf->hf_fildes ) != 0 )
	     {
		*error = VF_NOTOK ;
		hf->hf_state = HFS_ERROR;
		ERRTEXT( errtext, CLOSE_ERRTEXT );
	     }
	     else
	     {
		hf->hf_fildes = UNDEF;
	     }
	  }
      }
      else
      {
	 /* ***** fildes <0  ***** */

	 *error        = VF_NOTOK;
	 hf->hf_fildes = UNDEF;
	 hf->hf_state  = HFS_ERROR;
	 ERRTEXT( errtext, CREATE_ERRTEXT );
      }
   }

   DBG1 (( MF__,"sql06_ovrwr_open: hf -> hf_reclen              = %d\n",
			     hf -> hf_reclen));
   DBG1 (( MF__,"sql06_ovrwr_open: hf -> hf_format              = %s\n",
			     dbg_format [ hf -> hf_format ]));
   DBG1 (( MF__,"sql06_ovrwr_open: hf -> hf_state               = %d\n",
			     hf -> hf_state ));
   DBG1 (( MF__,"sql06_ovrwr_open: * error                      = %d\n",
			     * error ));
   DBG1 (( MF__,"sql06_ovrwr_open: returning -----------------------------\n" ));
}

/*==========================================================================*/

static void  sql06_read_open ( temp_name , hf , format , error , errtext )

VF_FILENAMEC                    temp_name ;
struct HOSTFILE_ALL             * hf ;
UINT1                           format ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{
   int                         openstatus ;

   DBG1 (( MF__,"sql06_read_open : called ===========================\n" ));
   DBG1 (( MF__,"sql06_read_open : temp_name                    '%s'\n" ,
			     temp_name )) ;
   DBG1 (( MF__,"sql06_read_open : format (plain,rec,stream,unk)= %d\n",
			     format ));

#  ifdef DEVENV
   openstatus = (( hf -> hf_rmfildes = fileopen ( (const char *)temp_name ,"r") ) == NULL );

   DBG1 (( MF__,"sql06_read_open : opened  hf -> hf_rmfildes    = %d\n",
				     hf -> hf_rmfildes ));
#  else
   openstatus = VEN06_OPENREAD ( (const char *)temp_name , & hf -> hf_fildes );

   DBG1 (( MF__,"sql06_read_open : opened  hf -> hf_fildes      = %d\n",
				     hf -> hf_fildes ));
#  endif

   DBG1 (( MF__,"sql06_read_open : openstatus                   = %d\n" ,
			     openstatus )) ;

   if ( openstatus == 0 )
   {
      /* ***** file descriptor ok ***** */

#     ifdef DEVENV
      /* ***** set hf -> hf_fildes to ok   ***** */
      /* ***** because for UNDEF questions ***** */

      hf -> hf_fildes = 999 ;
#     endif

      vfstatus ( hf,  error, errtext );

      if  ( *error == VF_OK )
      {
	 /* ***** VFstatus ok ***** */

	 hf->hf_state = HFS_USED;
	 vf_get_desc ( hf ,  format , error, errtext );

      }
      else
      {
	 hf->hf_state = HFS_ERROR;
	 DBG1 (( MF__,"sql06_read_open : HFS  error1 \n" ));
      }
   }
   else
   {
      DBG1 (( MF__,"sql06_read_open : error   HFS 2, %s\n", sqlerrs() ));
      hf->hf_fildes = UNDEF;
      hf->hf_state  = HFS_ERROR;
      *error        = VF_NOTOK;
      ERRTEXT ( errtext, OPEN_ERRTEXT );
   }

   DBG1 (( MF__,"sql06_read_open : hf -> hf_reclen              = %d\n",
			     hf -> hf_reclen));
   DBG1 (( MF__,"sql06_read_open : hf -> hf_format              = %s\n",
			     dbg_format [ hf -> hf_format ]));
   DBG1 (( MF__,"sql06_read_open : hf -> hf_eff_format          = %s\n",
			     dbg_format [ hf -> hf_eff_format ]));
   DBG1 (( MF__,"sql06_read_open : hf -> hf_state               = %d\n",
			     hf -> hf_state ));
   DBG1 (( MF__,"sql06_read_open : * error                      = %d\n",
			     * error ));
   DBG1 (( MF__,"sql06_read_open : errtext                      = %.40s\n",
			     errtext ));
   DBG1 (( MF__,"sql06_read_open : returning -----------------------------\n" ));
}

/*==========================================================================*/

static void     sql06_append_open ( temp_name , hf , format , rec_len ,
				    error , errtext )

VF_FILENAMEC                    temp_name ;
struct HOSTFILE_ALL             * hf ;
UINT1                           format ;
tsp00_Int4                        rec_len ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{
   int                          openstatus ;
   long                         end_of_file_position ;


   DBG1 (( MF__,"sql06_app_open: called ==================================\n" ));


   /* ***** check existence of the file : ***** */
   /* ***** if exists open for read/write ***** */
   /* ***** else create new file          ***** */

   openstatus = VEN06_OPENRDWR ( (const char *)temp_name , & hf -> hf_fildes );

   DBG1 (( MF__,"sql06_app_open  : hf -> hf_fildes (open rdwr)  = %d\n",
			    hf -> hf_fildes ));

   if ( openstatus == 0 )
   {
      /* ***** file descriptor ok ***** */

      vfstatus ( hf , error , errtext ) ;

      if ( VEN06_LOCKFILE ( hf , rec_len ) == 0 )
      {
	 /* ***** fcntl ok ***** */

	 hf -> hf_wr_prot = TRUE ;
	 DBG1 (( MF__,"sql06_app_open  : hf -> hf_wr_prot             = %d\n",
				   hf -> hf_wr_prot  ));

	 if ( ( * error == VF_OK ) && ( hf->hf_file_type == REGFILE) )
	 {
	    /* ***** ok and regfile ***** */

	    hf->hf_state = HFS_USED ;
	    vf_get_desc ( hf , format , error , errtext ) ;


	    /* ***** append to empty text file is OK ***** */

	    if ( * error == VF_EOF && format == vf_plaintext )
	    {
	       hf->hf_format = vf_plaintext ;
	       hf->hf_reclen = 0;
	       hf->hf_state  = HFS_USED;
	       *error = VF_OK;
	    }
	    else
	    {
	       if ( * error != VF_OK )
	       {
		  /* ****** error is VF_EOF ***** */ ;
	       }
	       else
	       {
		  if ( ( format == hf -> hf_format  &&
			 rec_len == hf -> hf_reclen     )   ||
		       ( format != vf_record            )        )
		  {
		     /* ***** e06_posbuffer ***** */

		     e06_posbuffer ( hf , (long) NO_OFFSET , SIZE_OFFSET ,
				     & end_of_file_position ,
				     error , errtext );
		  }
		  else
		  {
		     /* ***** noseek ***** */

		     hf->hf_state = HFS_ERROR;
		     *error       = VF_NOSEEK;
		     ERRTEXT( errtext,ILL_FILE_DESC_ERRTEXT  );
		  }
	       }
	    }
	 }
	 else
	 {
	    /* ***** not ok or not regfile ***** */

	    hf -> hf_state = HFS_ERROR;
	    * error        = VF_NOTOK ;
	    ERRTEXT( errtext, ILL_APPEND_DEVICE );
	 }
      }
      else
      {
	 /* ***** fcntl not ok ***** */

	 hf -> hf_state = HFS_ERROR;
	 * error        = VF_NOTOK ;
	 ERRTEXT( errtext, NO_WRITE_PROTEC_ERRTEXT );

	 if   ( VEN06_CLOSEFILE( hf->hf_fildes ) != 0 )
	 {
	    hf->hf_state = HFS_ERROR;
	    *error       = VF_NOTOK ;
	    ERRTEXT( errtext, CLOSE_ERRTEXT );
	 }
	 else
	 {
	    hf->hf_fildes = UNDEF ;
	 }
      }
   }
   else
   {
      /* ***** file descriptor not ok ***** */
      /* ***** file does not exists   ***** */

      openstatus = VEN06_CREATFILE ( (const char *)temp_name , & hf -> hf_fildes );

      hf->hf_reclen = rec_len ;
      hf->hf_format = format ;
      if ( openstatus == 0 )
      {
	 /* ***** fildes >=0 ***** */

	 vfstatus ( hf , error , errtext ) ;

	 if ( VEN06_LOCKFILE( hf, hf->hf_reclen ) == 0 )
	 {
	    /* ***** fcntl ok ***** */

	    hf -> hf_wr_prot = TRUE ;
	    DBG1 (( MF__,"sql06_app_open  : hf -> hf_wr_prot             = %d\n",
				      hf -> hf_wr_prot  ));

	    if ( ( * error == VF_OK )             &&
		 ( hf -> hf_file_type == REGFILE )    )
	    {
	       hf->hf_state = HFS_USED;
	       vf_put_desc ( hf , error , errtext );
	    }
	    else
	    {
	       hf->hf_state = HFS_ERROR;
	       * error       = VF_NOTOK ;
	       ERRTEXT( errtext, ILL_APPEND_DEVICE );
	    }
	 }
	 else
	 {
	    /* ***** fcntl not ok ***** */

	    hf -> hf_state = HFS_ERROR;
	    * error        = VF_NOTOK ;
	    ERRTEXT ( errtext , NO_WRITE_PROTEC_ERRTEXT );

	    if ( VEN06_CLOSEFILE ( hf->hf_fildes ) != 0 )
	    {
	       hf -> hf_state = HFS_ERROR;
	       * error        = VF_NOTOK ;
	       ERRTEXT( errtext, CLOSE_ERRTEXT );
	    }
	    else
	    {
	       hf->hf_fildes = UNDEF ;
	    }
	 }
      }
      else
      {
	 /* ***** openstatus not okay ***** */

	 hf -> hf_fildes = UNDEF;
	 hf -> hf_state  = HFS_ERROR;
	 * error         = VF_NOTOK ;
	 ERRTEXT( errtext, CREATE_ERRTEXT );
      }
   }
   DBG1 (( MF__,"sql06_app_open: returning \n" ));
}

#ifdef soll_wieder_rein
/*==========================================================================*/

/* VORSICHT! Page0 Struktur hat sich geändert... Besser zur Migration ins
   neue virtuelle File überreden....
 */
static void     sql06_devspace_and_type_check ( hf , error , errtext )

struct HOSTFILE_ALL             * hf ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{
   int                          fd ;
   int                          ret ;
   int                          openstatus ;
   int                          cnt ;
   char                         buffer [ 512 ] ;
   char                         dev_space_string [ 16 ] ;

   DBG1 (( MF__,"sql06_devspace_ : called ================================\n" ));

   /* ***** initialize externel parameter ***** */

   * error = VF_OK ;


   /* ***** check for access permission ***** */

   ret = access ( hf -> hf_name , F_OK ) ;

   if ( ret < 0  )
   {
      /* ***** do nothing ***** */ ;
   }
   else
   {
      /* ***** get file type ***** */

      vfstatus ( hf , error , errtext ) ;

      if ( * error != VF_OK )
      {
	 return ;
      }
      else
      {
	 /* ***** check for dev space ***** */

	 /* FSt 14.04.1993
	 if ( ( hf -> hf_file_type != FIFOFILE ) &&
	      ( hf -> hf_direction != VREAD_EN00    )     )
	 */
	 if ( hf -> hf_file_type != FIFOFILE )
	 {
	    fd = open ( hf -> hf_name , O_RDONLY );

	    DBG1 (( MF__,"sql06_devspace_ : fd                           = %d\n" ,
				      fd ))

	    if ( fd < 0 )
	    {
	       /* ***** file descriptor not ok ***** */

	       * error = VF_NOTOK ;
	       ERRTEXT ( errtext, CANNOT_CHECK_DEVSPACE );
	       return ;
	    }
	    else
	    {
	       /* ***** file descriptor ok, read part of header ***** */

	       cnt = read ( fd , buffer , sizeof ( buffer ) ) ;

	       if ( ( cnt < 0 ) || ( hf -> hf_direction == VREAD_EN00 ) )
	       {
		  /* ***** cannot read file;                         ***** */
		  /* ***** ignore the errors, because the beginning  ***** */
		  /* ***** of new tapes is not readable; under these ***** */
		  /* ***** conditions, other devices should not be   ***** */
		  /* ***** writable either;                          ***** */
		  /* ***** if direction VREAD_EN00 is choosen don't check ***** */
		  /* ***** the test header                           ***** */

		  close ( fd ) ;
		  return ;
	       }
	       else
	       {
		  /* ***** DEV SPACE ? ***** */

		  SAPDB_memcpy ( dev_space_string , buffer + MAGIC_REC_LEN ,
			   sizeof ( dev_space_string ) ) ;
		  dev_space_string [ sizeof ( dev_space_string ) -1 ] ='\0' ;

		  DBG1 (( MF__,"sql06_devspace_ : dev_space_string             = >%s<\n" ,
					    dev_space_string ))

		  if ( ! ( strcmp ( dev_space_string , SQL_DB_DEVSPACE ) ) )
		  {
		     * error = VF_NOTOK ;
		     ERRTEXT ( errtext, FILE_IS_DEVSPACE );
		     close ( fd ) ;
		     return ;
		  }
		  else
		  {
		     /* ***** ok, close file, init hf structure ***** */

		     ret = close ( fd ) ;

		     if ( ret < 0 )
		     {
			* error = VF_NOTOK ;
			ERRTEXT ( errtext, CLOSE_DEVSPACE );
			return ;
		     }
		     /*
		     sql06_init_one ( hf , FALSE ) ;
		       */
		  }
	       }
	    }
	 }
      }
   }
}
#endif

/*==========================================================================*/

static void     vfstatus ( hf, error , errtext )

struct HOSTFILE_ALL             * hf ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;


{
   int                          status;
   VEN06_FILEDATA               info_buffer;

   DBG1 (( MF__,"vfstatus: called ========================================\n" ));

   status =  VEN06_STAT ( (const char *)hf -> hf_name , & info_buffer );

   DBG1 (( MF__,"vfstatus        : hf -> hf_name                = %s\n",
			     hf -> hf_name ));
   DBG1 (( MF__,"vfstatus        : status                       = %d\n",
			     status        ));

   if ( status == 0 )
   {
      /* ***** file status ok ***** */

      if ( VEN06_ISSPECIAL ( info_buffer ) )
      {
	 /* ***** raw device ***** */

	 hf->hf_file_type = SPECFILE;
	 hf->hf_byte_num = VEN06_FILESIZE( info_buffer );

	 DBG1 (( MF__,"vfstatus        : hf -> hf_file_type           = %d \n",
				   hf -> hf_file_type ));
	 return;
      }

      if ( VEN06_ISPIPEFIFO ( info_buffer ) )
      {
	 /* ***** fifo spec ***** */

	 hf->hf_file_type = FIFOFILE;
	 hf->hf_byte_num = VEN06_FILESIZE( info_buffer );

	 DBG1 (( MF__,"vfstatus        : hf -> hf_file_type           = %d \n",
				   hf -> hf_file_type ));
	 return;
      }

      if ( VEN06_ISREGFILE ( info_buffer ) )
      {
	 /* ***** ordinary file ***** */

	 hf -> hf_file_type = REGFILE;
	 hf -> hf_byte_num  = VEN06_FILESIZE ( info_buffer );

	 DBG1 (( MF__,"vfstatus        : hf -> hf_file_type           = %d \n",
				   hf -> hf_file_type ));
	 return;
      }

      hf -> hf_file_type = UNDEF;
      hf -> hf_byte_num  = UNDEF;
      * error            = VF_NOTOK;
      ERRTEXT(errtext, "Open not allowed: Unknown file mode ");

      DBG1 (( MF__,"vfstatus        : hf -> hf_file_type           = %d \n",
				hf -> hf_file_type ));
   }
   else
   {
      /* ***** file status not ok ***** */

      *error           = VF_NOTOK;
      ERRTEXT(errtext, "error by get file status");
      DBG1 (( MF__,"vfstatus        : file or ptr to buffer are inv \n" ));
   }

   DBG1 (( MF__,"vfstatus        : * error                      = %d \n",
			      * error       ));
   DBG1 (( MF__,"vfstatus: returning ------------------------------------\n" ));
}

/*==========================================================================*/

static void             vf_get_desc ( hf , format , error , errtext)

/*
 * The first block is read and the file format got (from byte 3 and 4)
 */

struct HOSTFILE_ALL                    * hf ;
UINT1                                  format ;
tsp00_VfReturn                          * error ;
tsp00_ErrText                            errtext ;

{ /* vf_get_desc */

	DBG1 (( MF__,"vf_get_desc: called \n" ));
	DBG1 (( MF__,"vf_get_desc     : format                      = %s\n",
				 dbg_format [ format ] ));

        if ( format == vf_4k_block )
          {
	    hf -> hf_pos    = - 1 ;
	    hf -> hf_format = vf_4k_block ;
            hf -> hf_reclen = KB08 ;
            *error = VF_OK ;     
            return ;
          }

	/* ***** initialize hf_readcnt for  ***** */
	/* ***** reading virt. file header  ***** */
	/* ***** and read virt. file header ***** */

	hf->hf_readcnt = hf->hf_HeaderLength;
	/* hf->hf_readcnt = MAXBUFFERLENGTH; */

	e06_getbuffer ( hf,  error, errtext );     /* read the first block */

	/* ***** check the readability of the file ***** */

	if ( * error == VF_OK )
	{
	    /* ***** check the virt. file type ***** */
	    /* ***** ( binary or plaintext )   ***** */

	    if ( hf -> hf_bufptr [0] == NUL  &&  hf -> hf_bufptr [1] == NUL )
	    {
		/* ***** check the virt. file type ***** */
		/* ***** ( binary fixed or var or  ***** */
		/* ***** ( byte stream )           ***** */

		if ( hf -> hf_bufptr [ 2 ] == (UCHAR) 0xff &&
		     hf -> hf_bufptr [ 3 ] == (UCHAR) 0xff )
		{
		    /* ***** vf_stream ***** */

		    hf -> hf_reclen = - 1 ;
		    hf -> hf_pos    = KB08 - 1 ;
		    hf -> hf_format = vf_stream ;
		}
		else
		{
		    hf -> hf_reclen = 16777216 * hf -> hf_bufptr [ 4 ] +
					 65536 * hf -> hf_bufptr [ 5 ] +
					   256 * hf -> hf_bufptr [ 2 ] +
						 hf -> hf_bufptr [ 3 ] ;
		    if ( hf->hf_reclen == 0 )
		    {
			/* ***** variable ***** */

			hf -> hf_pos = KB08 - 1 ;
			hf -> hf_format = vf_record ;
		    }
		    else
		    {
			/* ***** fixed ***** */

			if (  hf->hf_reclen > KB64 || 
                              hf->hf_reclen < MAGIC_REC_LEN   ||
/* F.H. CL14 new Header */   ( ( hf->hf_HeaderLength > KB08 || 
                                 hf->hf_reclen       > KB08 )
                               && hf->hf_HeaderLength != hf->hf_reclen )
                           )
			{
			    /* ***** length not correct ***** */
	                    MSGD (( -11000 , ERR_TYPE, "VIRTFILE","vf_get_desc: len wanted: %d len given: %d \n",hf->hf_reclen, hf->hf_HeaderLength ));

			    hf->hf_state = HFS_ERROR;
			    *error       = VF_NOTOK;
			    ERRTEXT( errtext,  ILL_FILE_SPEC_ERRTEXT );
			}
			else
			{
			    /* ***** length ok ***** */

                            hf -> hf_pos = hf->hf_HeaderLength - 1 ;
			    hf -> hf_format = vf_record ;
			}
		    }
		}
              if ( hf->hf_label && hf -> hf_pos == KB08 - 1 && 
                   hf->hf_bufptr [ MAGIC_REC_LEN ] != NUL )
                { SAPDB_memcpy ( hf->hf_label, hf->hf_bufptr+MAGIC_REC_LEN,
                           sizeof ( tsp_vf_label ) ) ; 
#                 ifdef DEBUG_RTE           
                  { char buf[20] ;
                    SAPDB_memcpy ( buf, hf->hf_label, 19 ) ; buf[19]='\0' ;
	            DBG1 (( MF__,"vf_get_desc     : hf -> hf_label          = %s\n",
                          buf )) ;
                  }
#                 endif
                }
              else
	        DBG1 (( MF__,"vf_get_desc     : hf -> hf_label : empty         \n"));
	    }
	    else
	    {
		/* ***** plain text file ***** */

		hf -> hf_reclen        = 0;
		hf -> hf_pos           = -1;
		hf -> hf_format        = vf_plaintext ;
		hf -> hf_second_asc_rd = FALSE ;
		DBG1 (( MF__,"vf_get_desc     : hf -> hf_second_asc_rd       = %d\n",
					  hf -> hf_second_asc_rd ));
	    }

	    /* ***** check the virt. file format and the user ***** */
	    /* ***** wanted file format for equivalence       ***** */

	    if ( format == vf_stream )
		hf -> hf_eff_format = vf_stream ;
	    else
		hf -> hf_eff_format = hf -> hf_format ;

	}
	else
	{
	    if ( * error == VF_EOF )
	    {
		ERRTEXT( errtext, NULL_BYTE_FILE_ERRTEXT );
	    }
	}

	DBG1 (( MF__,"vf_get_desc     : hf -> hf_pos                 = %d\n",
				  hf->hf_pos   ));
	DBG1 (( MF__,"vf_get_desc     : hf -> hf_reclen              = %d\n",
				  hf->hf_reclen));
	DBG1 (( MF__,"vf_get_desc     : hf -> hf_format              = %s\n",
				  dbg_format [ hf -> hf_format ] ));
	DBG1 (( MF__,"vf_get_desc     : hf -> hf_eff_format          = %s\n",
				  dbg_format [ hf -> hf_eff_format ] ));
	DBG1 (( MF__,"vf_get_desc     : errtext                      = %.40s\n",
				  errtext ));
	DBG1 (( MF__,"vf_get_desc: returning \n" ));

} /* vf_get_desc */

/*==========================================================================*/

static void             vf_put_desc ( hf, error, errtext )

/*
 * The description is put (3. and 4.byte);
 *
 * If record length is 4kB the first block is written;
 */

struct HOSTFILE_ALL                    *hf;
tsp00_VfReturn                          *error;
tsp00_ErrText                            errtext;

{ /* vf_put_desc */

   tsp00_Int4                               desc_len ;

   DBG1 (( MF__,"vf_put_desc: called =================================\n" ));

   if ( ( hf -> hf_format == vf_record ) ||
	( hf -> hf_format == vf_stream )    )
   {  /* binary */
      if ( ( hf->hf_reclen == 0 )                    ||
	   ( hf->hf_reclen == -1 )                   ||
	   ( ( hf->hf_reclen >= MAGIC_REC_LEN ) &&
	     ( hf->hf_reclen <= KB64 )             )     )
      {    /* length ok */
          desc_len = hf->hf_HeaderLength ;
      }
      else
      {
	 /* ***** length not ok ***** */

	 *error = VF_NOTOK;
	 ERRTEXT( errtext, ILL_FILE_SPEC_ERRTEXT );
	 return;
      }

      DBG1 (( MF__,"vf_put_desc     : SAPDB_memset follows \n" ));
      SAPDB_memset ( & hf -> hf_bufptr [ 0 ] , '\0' , desc_len );

      /* put description */

      if ( hf -> hf_format == vf_stream )
      {
	 hf -> hf_reclen = - 1 ;
	 hf->hf_bufptr[2] = (UCHAR) 0xff ;
	 hf->hf_bufptr[3] = (UCHAR) 0xff ;
	 hf->hf_bufptr[4] = (UCHAR) 0xff ;
	 hf->hf_bufptr[5] = (UCHAR) 0xff ;
      }
      else
      {
	 hf -> hf_bufptr [2] = (UCHAR) (  hf -> hf_reclen / 256);
	 hf -> hf_bufptr [3] = (UCHAR) (  hf -> hf_reclen % 256);
	 hf -> hf_bufptr [4] = (UCHAR) (( hf -> hf_reclen / KB64 ) / 256 );
	 hf -> hf_bufptr [5] = (UCHAR) (( hf -> hf_reclen / KB64 ) % 256 );
      }
      if ( hf->hf_label && hf->hf_label[0] != NUL ) 
        SAPDB_memcpy ( & ( hf -> hf_bufptr [ MAGIC_REC_LEN ] ), hf->hf_label ,
                 sizeof ( tsp_vf_label ) );

      /* position hf->hf_buf */

      hf -> hf_pos = desc_len - 1 ;

      DBG1 (( MF__,"vf_put_desc     : hf -> hf_pos     (1)         = %d\n",
				hf -> hf_pos   ));
      DBG1 (( MF__,"vf_put_desc     : hf -> hf_byte_num (1)        = %d\n",
				hf -> hf_byte_num ));
      DBG1 (( MF__,"vf_put_desc     : hf -> hf_bufbase             = %d\n",
				hf -> hf_bufbase ));
      DBG1 (( MF__,"vf_put_desc     : hf -> hf_reclen              = %d\n",
				hf -> hf_reclen ));
      DBG1 (( MF__,"vf_put_desc     : hf -> hf_bufptr [ 2 ]        = %d\n",
				hf -> hf_bufptr [ 2 ]  ));
      DBG1 (( MF__,"vf_put_desc     : hf -> hf_bufptr [ 3 ]        = %d\n",
				hf -> hf_bufptr [ 3 ]  ));
      DBG1 (( MF__,"vf_put_desc     : hf -> hf_bufptr [ 4 ]        = %d\n",
				hf -> hf_bufptr [ 4 ]  ));
      DBG1 (( MF__,"vf_put_desc     : hf -> hf_bufptr [ 5 ]        = %d\n",
				hf -> hf_bufptr [ 5 ]  ));

      if ( hf->hf_bufbase + hf->hf_pos + 1 > hf->hf_byte_num )
      {
	 hf->hf_byte_num = hf->hf_bufbase + hf->hf_pos ;

	 DBG1 (( MF__,"vf_put_desc     : hf -> hf_byte_num (2)        = %d\n",
				   hf -> hf_byte_num ));
      }

      /* ***** gleich in 4KB Puffer lesen !! ***** */

      e06_putbuffer ( hf, error , errtext );

   }   /* binary */
   else
     if ( hf -> hf_format == vf_4k_block )
       {
          hf -> hf_pos    = - 1 ;
          hf -> hf_reclen = KB08 ;
          *error = VF_OK ;     
       }

   DBG1 (( MF__,"vf_put_desc     : hf -> hf_pos         (2)     = %d\n",
			     hf -> hf_pos   ));
   DBG1 (( MF__,"vf_put_desc     : * error                      = %d\n",
			     * error        ));

   DBG1 (( MF__,"vf_put_desc: returning \n" ));

} /* vf_put_desc */

/*==========================================================================*/

global	void	sqlfclose_next_tape ( hf_num , erase , poolptr , buf_cnt ,
			    block , error , nxt_tape_error, errtext )

UINT4                           * hf_num;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp00_Bool                         erase ;
tsp00_Int4                        poolptr ;	/* despite name, not a ptr */
tsp00_Int2                        buf_cnt ;
BUFFERADDRESS                   block ;
tsp00_VfReturn                   * error;
tsp00_VfReturn                   * nxt_tape_error;
tsp00_ErrText                     errtext;

{
  sqlfclose ( hf_num , erase , poolptr , buf_cnt , block , error , errtext );
  *nxt_tape_error = VF_NOTOK ;
  if ( *error == VF_OK ) 
    ERRTEXT( errtext , AUTOLOADER_NOT_SUPPORTED );
}

/*==========================================================================*/

global	void	sqlfclose ( hf_num , erase , poolptr , buf_cnt ,
			    block , error , errtext )

UINT4                           * hf_num;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp00_Bool                         erase ;
tsp00_Int4                        poolptr ;	/* despite name, not a ptr */
tsp00_Int2                        buf_cnt ;
BUFFERADDRESS                   block ;
tsp00_VfReturn                   * error;
tsp00_ErrText                     errtext;

{
    tsp00_Int2                        ret_cod;
    struct HOSTFILE_ALL             * hf;

    DBG1 (( MF__,"sqlfclose: called \n" ));
    DBG1 (( MF__,"sqlfclose       : * hf_num                    = %d\n", * hf_num ));

    hf = & real_files [ * hf_num - 1 ] ;

    if ( ! e06_valid_fileno ( hf ) )
    {
	*error = VF_NOTOK;
	ERRTEXT ( errtext, BAD_FILENO_ERRTEXT );
	return;
    }

    DBG1 (( MF__,"sqlfclose       : hf -> hf_state              = %d\n",
			      hf -> hf_state ));
    DBG1 (( MF__,"sqlfclose       : hf -> hf_pos                = %d\n",
			      hf -> hf_pos ));
    DBG1 (( MF__,"sqlfclose       : hf -> hf_name               '%s'\n",
			      hf -> hf_name ));
    DBG1 (( MF__,"sqlfclose       : erase                       = %d\n",
			      erase ));
    DBG1 (( MF__,"sqlfclose       : poolptr                     = 0x%x\n",
			      poolptr ));
    DBG1 (( MF__,"sqlfclose       : buf_cnt                     = %d\n",
			      buf_cnt ));
    DBG1 (( MF__,"sqlfclose       : block                       = 0x%x\n",
			      block ));

    * error = VF_OK;
    SAPDB_memset ( errtext , ' ' , sizeof ( tsp00_ErrText ) ) ;

    if ( hf->hf_state != HFS_UNUSED )
    {
	/* ***** may write and close ***** */
	/* ***** buffer pool control ***** */

	sql06_close_buffer_pool_control ( hf , buf_cnt,
					  (tsp_vf_bufaddr) (block),
					  error , errtext );


	/* ***** be sure you have written out the complete data ***** */

	if ( ( hf->hf_direction == VWRITE_EN00      ||
	       hf->hf_direction == VOVERWRITE_EN00  ||
	       hf->hf_direction == VAPPEND_EN00 )   &&
	       hf->hf_state != HFS_ERROR )
	{
	    if ( hf->hf_pos > UNDEF )
	    {
		/* ***** write last block ***** */

		e06_pbuf_and_fill_up ( hf , error , errtext );
	    }

	}


	/* ***** if fd > stderr you can close the file ***** */
	/* ***** in the other case you can't do this   ***** */
	/* ***** if you have to erase the file do this ***** */
	/* ***** before closing the file               ***** */

	if ( hf->hf_fildes >= stderr_int )
	{
	    /* ***** no stdio ***** */

	    if ( erase )
	    {
		ret_cod = unlink ( (const char *)hf->hf_name ) ;
		if ( ret_cod == UNDEF )
		{
		    * error = VF_NOTOK;
		    hf->hf_state = HFS_ERROR ;
		    DBG1 (( MF__,"sqlfclose       : unlink hf -> hf_name         '%s'\n",
					     hf->hf_name ));
		    DBG1 (( MF__,"sqlfclose: unlink error, %s\n",
					     sqlerrs() ));
		    ERRTEXT( errtext , UNLINK_ERRTEXT );
		}
	    }

#           ifdef DEVENV
	    /* ***** you must not read oracle data ***** */

	    if ( hf->hf_direction == VREAD_EN00 )
	    {
		ret_cod = fileclose ( hf->hf_rmfildes );
	    }
	    else
#           endif
	    ret_cod = VEN06_CLOSEFILE ( hf->hf_fildes );
	    if ( ret_cod != 0 )
	    {
		* error = VF_NOTOK;
		hf->hf_state = HFS_ERROR ;
		DBG1 (( MF__,"sqlfclose       : ret_cod close                = %d\n",
					 ret_cod )) ;
		DBG1 (( MF__,"sqlfclose: ret_cod close  error, %s\n",
					 sqlerrs() )) ;
		ERRTEXT( errtext, CLOSE_ERRTEXT);
	    }

	    DBG1 (( MF__,"sqlfclose       : after close \n" ));

	    hf->hf_fildes = UNDEF;

	    /* FSt requirement from Rudi 1992-11-19
	    if ( hf->hf_wr_prot == TRUE )
	    {
		DBG1 (( MF__,"sqlfclose       : write protected , sleep      = %d\n",
						     SL_TIME ));
		ret_cod = sleep ( SL_TIME );
	    }
	    */
	}
	else
	{
	    /* ***** may be stdio ***** */

	    if ( !( hf->hf_fildes >= stdin_int
		&&
		    hf->hf_fildes <= stderr_int
		&&
		    ( strcmp ( (const char *)hf->hf_name, std_in  ) == 0
			||
		      strcmp ( (const char *)hf->hf_name, std_out ) == 0
			||
		      strcmp ( (const char *)hf->hf_name, std_err ) == 0
		    ) ) )
	    {
		hf->hf_state = HFS_ERROR;
		*error = VF_NOTOK ;
		ERRTEXT( errtext, ILL_FILE_DESC_ERRTEXT );
	    }
	}


	/* ***** resret signal handling ***** */

	if ( hf->hf_file_type == FIFOFILE )
	{
	    /* ***** decrement of last open fifo counter ***** */

	    sql06c_last_fifo -- ;
	    DBG1 (( MF__,"sqlfclose       : sql06c_last_fifo             = %d\n",
				      sql06c_last_fifo ))


	    /* ***** last open fifo ? ***** */

	    if ( sql06c_last_fifo == 0 )
	       sql06c_reset_signal () ;
	}


	/* ***** initialize host file structure ***** */

	sql06_init_one ( hf , FALSE );

	if ( file_cnt . open_files > 0 )
	    file_cnt . open_files -- ;

	if ( buf_cnt > 9 )
	    file_cnt . open_files_allowed = HOSTFILE_MAX ;

	DBG1 (( MF__,"sqlfclose       : file_cnt . open_files_allowed= %d\n",
				  file_cnt . open_files_allowed )) ;
	DBG1 (( MF__,"sqlfclose       : file_cnt . open_files        = %d\n",
				  file_cnt . open_files )) ;
    }
    else
    {
	*error = VF_NOTOK;
	ERRTEXT(errtext, BAD_FILENO_ERRTEXT );
	DBG1 (( MF__,"sqlfclose       : hf_state unused %d \n",
				  hf->hf_state ));
    }

    DBG1 (( MF__,"sqlfclose: returning \n" ));

} /* VFclose */

/*==========================================================================*/

global	void	sqlfread ( hf_num , block , length , error , errtext )
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp_vf_bufaddr                  block ;
tsp00_Int4                        * length ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{ /* sqlfread */

    struct HOSTFILE_ALL             * hf;

    DBG1 (( MF__,"sqlfread: called =========================================\n" ));

    hf = & real_files [ * hf_num - 1 ] ;

    DBG1 (( MF__,"sqlfread        : hf -> hf_name                = %s\n",
			      hf -> hf_name ));
    DBG1 (( MF__,"sqlfread        : hf -> hf_reclen              = %d\n",
			      hf -> hf_reclen ));
    DBG1 (( MF__,"sqlfread        : hf -> hf_eff_format          = %s\n",
			      dbg_format [ hf->hf_eff_format ] ));
    DBG1 (( MF__,"sqlfread        : * length                     = %d\n",
			      * length )) ;


    /* ***** initialize ***** */

    * error  = VF_OK;
    SAPDB_memset ( errtext , ' ' , sizeof ( tsp00_ErrText ) ) ;


    /* ***** check hostfileno for validity ***** */

    if ( e06_valid_fileno ( hf ) )
    {
	/* ***** hostfileno ok               ***** */
	/* ***** is direction really vread ? ***** */

	if ( hf->hf_direction == VREAD_EN00 && hf->hf_state == HFS_USED )
	{
	   /* ***** read ***** */

	   if ( hf->hf_eff_format == vf_plaintext )
	   {
	       /* ***** read ascii ***** */

	       e06_read_asc_var ( hf, block, length, error, errtext );
	   }
	   else
	   {
	       /* ***** binary ***** */

	       if ( hf->hf_reclen == 0 )
	       {
		   /* ***** read binary variable ***** */

		   e06_read_bin_var ( hf, block, length, error , errtext );

		   /*JH* / *length = read ( hf->hf_fildes,
		    *JH* /         block, *length );
		    *JH* / if ( *length == -1 )
		    *JH* / {
		    *JH* /     *error = VF_NOTOK ;
		    *JH* /     ERRTEXT(errtext, sqlerrs() ) ;
		    *JH* / }
		    *JH* / else if ( *length == 0 )
		    *JH* /     *error = VF_EOF ;
		    *JH* / DBG1 (( MF__,"sqlfread  : bytes read = %d\n",
		    *JH* /     *length )) ;
		   */

	       }
	       else
	       {
		   /* ***** read binary fixed ***** */

		   e06_read_bin_fix ( hf, block, length, error , errtext );
	       }
	   }
	}
	else
	{
	    /* ***** dir. != VREAD_EN00 or file used ***** */

	    if ( hf->hf_direction == VREAD_EN00 && hf->hf_state == HFS_END )
	    {
	       /* ***** end of file ***** */

		*length = 0;
		*error  = VF_EOF;
	    }
	    else
	    {
		/* ***** error ***** */

		*error = VF_NOTOK;
		ERRTEXT(errtext, ILL_FILE_DESC_ERRTEXT );
	    }
	}
    }
    else
    {
	*error = VF_NOTOK;
	ERRTEXT( errtext, BAD_FILENO_ERRTEXT );
    }

#   ifdef DEBUG_RTE
    if ( *error != VF_OK )
    {
	MSGD (( -11000 ,  ERR_TYPE, "VIRTFILE","sqlfread: returning, with error %d \n", *error ));
    }
#   endif
    DBG1 (( MF__,"sqlfread        : * length                     = %d\n",
			      * length ));
    /*
    DBG1 (( MF__,"sqlfread        : block                        = %.4096s\n",
			      block  ));
    */
    DBG1 (( MF__,"sqlfread        : block [ 0 ]                  = %c\n",
			      ((char *)block) [ 0 ] ));
    DBG1 (( MF__,"sqlfread        : block  adr                   = 0x%x\n",
			      block  ));
    DBG1 (( MF__,"sqlfread: returning ---------------------------------------\n" ))

} /*VFread*/

/*==========================================================================*/

static void     e06_read_asc_var ( hf , block , length , error , errtext )

struct HOSTFILE_ALL             * hf ;
tsp_vf_bufaddr                  block ;
tsp00_Int4                        * length ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;

{
   UINT4                        dst_pos ;  /* block position */

   long                         local_len ;
   char                         * nl_ptr ;

   DBG1 (( MF__,"read_asc: called =========================================\n" ));
   DBG1 (( MF__,"e06_read_asc_var: hf -> hf_name                = %s\n",
			     hf -> hf_name ));
   DBG1 (( MF__,"e06_read_asc_var: hf -> hf_second_asc_rd       = %d\n",
			     hf -> hf_second_asc_rd ));

   dst_pos  = 0 ;
   * length = 0 ;

   /* ***** file end reached ***** */

   if ( ( hf -> hf_rd_eof ) && ( hf -> hf_second_asc_rd ) )
   {
      * length = 0 ;
      * error  = VF_EOF ;
      return ;
   }


   /* ***** endless reading; if record found stop ***** */

   for (;;)
   {
      /* ***** be sure: hf -> hf_pos >= 0             ***** */
      /* ***** ( initial value : hf -> hf_pos = -1    ***** */
      /* *****                   e06_getbuffer call ) ***** */

      if ( hf -> hf_pos == UNDEF )
      {
	 hf -> hf_pos = 0 ;
	 DBG1 (( MF__,"e06_read_asc_var: hf -> hf_pos  ( UNDEF )      = %d\n",
				   hf -> hf_pos ));
      }

      /* ***** look for "\nl" ***** */

      nl_ptr = memchr ( & hf -> hf_bufptr [ hf -> hf_pos ] , '\n' ,
					    hf -> hf_maxpos - hf -> hf_pos ) ;

      if ( nl_ptr != NULL )
      {
	 /* ***** "nl" found, determine record length ***** */

	 local_len = nl_ptr - (char *)( & hf -> hf_bufptr [ hf -> hf_pos ] );

	 DBG1 (( MF__,"e06_read_asc_var: local_len                    = %d\n",
				   local_len ));
	 DBG1 (( MF__,"e06_read_asc_var: dst_pos                      = %d\n",
				   dst_pos ));
	 DBG1 (( MF__,"e06_read_asc_var: hf -> hf_pos                 = %d\n",
				   hf -> hf_pos ));
	 DBG1 (( MF__,"e06_read_asc_var: hf -> hf_maxpos              = %d\n",
				   hf -> hf_maxpos ));


	 /* ***** copy record into buffer,  ***** */
	 /* ***** update internel parameter ***** */

	 SAPDB_memcpy ( & ((char *)block) [ dst_pos ] , & hf -> hf_bufptr [ hf -> hf_pos ] ,
		  local_len ) ;

	 hf -> hf_pos += ( local_len + 1 ) ;
	 * length += local_len ;

	 DBG1 (( MF__,"e06_read_asc_var: * length   ( return )        = %d\n",
				   * length ));

	 /* ***** if this was the last record, remember this ***** */

	 if ( ( hf -> hf_rd_eof )
		     &&
	      ( ( hf -> hf_pos + 1 ) == hf -> hf_maxpos )
		     &&
		( ! memcmp ( & hf -> hf_bufptr [ hf -> hf_pos + 1 ] ,
			     "\n" , 1 ) )
	    )
	 {
	    hf -> hf_second_asc_rd = TRUE ;
	    DBG1 (( MF__,"e06_read_asc_var: hf -> hf_second_asc_rd       = %d\n",
				      hf -> hf_second_asc_rd ));
	    DBG1 (( MF__,"e06_read_asc_var: hf -> hf_pos + 1  (******)   = %d\n",
				      hf -> hf_pos + 1 ));
	    DBG1 (( MF__,"e06_read_asc_var: hf -> hf_maxpos   (******)   = %d\n",
				      hf -> hf_maxpos ));
	    DBG1 (( MF__,"e06_read_asc_var: &hf->hf_bufptr[hf->hf_pos+1] = %c\n",
				      & hf -> hf_bufptr [ hf -> hf_pos + 1 ] ));
	 }

	 return ;
      }
      else
      {
	 /* ***** read next buffer ( if possible ) ***** */

	 local_len = & hf -> hf_bufptr [ hf -> hf_maxpos - 1 ] -
		     & hf -> hf_bufptr [ hf -> hf_pos ] + 1 ;

	 * length += local_len ;

	 DBG1 (( MF__,"e06_read_asc_var: local_len                    = %d\n",
				   local_len ));
	 DBG1 (( MF__,"e06_read_asc_var: * length                     = %d\n",
				   * length ));


	 /* ***** copy record into buffer,  ***** */
	 /* ***** update internel parameter ***** */

	 SAPDB_memcpy ( & ((char *)block) [ dst_pos ] , & hf -> hf_bufptr [ hf -> hf_pos ] ,
		  local_len ) ;

	 dst_pos += local_len ;

	 DBG1 (( MF__,"e06_read_asc_var: dst_pos                      = %d\n",
				   dst_pos ));


	 /* ***** if this was the last record, return them ***** */

	 if ( hf -> hf_rd_eof )
	 {
	    hf -> hf_second_asc_rd = TRUE ;
	    DBG1 (( MF__,"e06_read_asc_var: hf -> hf_second_asc_rd       = %d\n",
				      hf -> hf_second_asc_rd ));
	    DBG1 (( MF__,"e06_read_asc_var: * length   ( return , no nl )= %d\n",
				      * length ));


	    /* ***** if length == 0 then EOF ***** */

	    if ( * length == 0 )
	    {
		* error = VF_EOF ;
		DBG1 (( MF__,"e06_read_asc_var: * length == 0 => EOF\n" )) ;;
	    }

	    return ;
	  }


	 /* ***** change first read count: second read becomes 'TRUE' ***** */
	 /* ***** initialize "hf -> hf_buf" with '\n'                 ***** */
	 /* ***** read next block  into hf -> hf_buf                  ***** */
	 /* ***** check length: the very last char in                 ***** */
	 /* ***** file must not be a separator;                       ***** */

	 DBG1 (( MF__,"e06_read_asc_var: hf -> hf_readcnt             = %d\n",
				   hf -> hf_readcnt ));

	 SAPDB_memset ( hf -> hf_bufptr , '\n' , hf -> hf_readcnt ) ;

	 e06_getbuffer ( hf,  error, errtext );

	 if ( ( hf -> hf_maxpos < hf -> hf_readcnt ) &&
	      ( hf -> hf_maxpos < MAXBUFFERLENGTH )     )
	 {
	    hf -> hf_maxpos ++ ;
	    DBG1 (( MF__,"e06_read_asc_var: hf -> hf_maxpos ++           = %d\n",
				      hf -> hf_maxpos ));
	 }

	 if ( * error != VF_OK )
	 {
	    return;
	 }
      }
   }
}

/*==========================================================================*/

static void     e06_read_bin_var ( hf , block , length , error , errtext )

/*
 * Read binary variable length records
 */

struct HOSTFILE_ALL                    * hf ;
tsp_vf_bufaddr                         block ;
tsp00_Int4                               * length ;
tsp00_VfReturn                          * error ;
tsp00_ErrText                            errtext ;

{ /* e06_read_bin_var */

	tsp00_Int4                           this_len;
	long                               cur_file_pos, next_4k_pos;

	DBG1 (( MF__,"e06_read_bin_var: called \n" ));
	DBG1 (( MF__,"e06_read_bin_var: block                        = 0x%x\n",
				  block ));

	this_len = 0;

	if ( hf->hf_pos < hf->hf_maxpos - 2 )
	     { /* get description of next record */
	     this_len  = 256 * hf->hf_bufptr[++hf->hf_pos];
	     this_len +=       hf->hf_bufptr[++hf->hf_pos];
	     }

	if ( this_len == 0 )
	    { /* read next block to hf->hf_buf */

	    cur_file_pos = hf->hf_bufbase + hf->hf_pos + 1;
	    next_4k_pos = cur_file_pos - (cur_file_pos % MAXBUFFERLENGTH);
	    if ( cur_file_pos % MAXBUFFERLENGTH != 0 )
		next_4k_pos += MAXBUFFERLENGTH;

	    hf->hf_pos += ( next_4k_pos - cur_file_pos );
	    if ( hf->hf_pos + 1 >= hf->hf_maxpos )
		e06_getbuffer ( hf, error, errtext );

	    if ( *error == VF_OK )
		 { /* get description of next record */
		 this_len   = 256 * hf->hf_bufptr[hf->hf_pos+1]
				     + hf->hf_bufptr[hf->hf_pos+2] ;
		 hf->hf_pos += 2;
		 }
	    else
		{ /* return of getting next buffer was not ok */
		*length = 0;
		return;
		}

	    } /* read next block to hf->hf_buf */

	if ( (this_len == 0) ||
	     (this_len >= ( hf -> hf_maxpos - hf -> hf_pos ) ) )
	    { /* intern error */
	    DBG1 (( MF__,"e06_read_bin_var: this_len                 = %d\n",
				     this_len));
	    DBG1 (( MF__,"e06_read_bin_var: hf -> hf_pos             = %d\n",
				     hf -> hf_pos));
	    DBG1 (( MF__,"e06_read_bin_var: hf -> hf_maxpos          = %d\n",
				     hf -> hf_maxpos));
	    *error       = VF_NOTOK;
	    hf->hf_state = HFS_ERROR;
	    * length      = 0;
	    ERRTEXT( errtext, "length of next record is out of range");
	    return;
	    } /* intern error */

	SAPDB_memcpy ( ((char *)block), &hf->hf_bufptr[hf->hf_pos+1], this_len );

	hf->hf_pos += this_len;

	if ( hf->hf_pos < (hf->hf_maxpos - 1) && (! odd ( hf->hf_pos )) )
	    {
	    hf->hf_pos++;
	    }

	* length = this_len;

	DBG1 (( MF__,"e06_read_bin_var: * length                     = %d\n",
				 * length ));
	DBG1 (( MF__,"e06_read_bin_var: * error                      = %d\n",
				 * error ));

	DBG1 (( MF__,"e06_read_bin_var: returning \n" ));

} /* e06_read_bin_var */

/*==========================================================================*/

static void     e06_read_bin_fix ( hf , block , length , error , errtext )

/*
 * Read binary fixed length records
 */

struct HOSTFILE_ALL                    * hf ;
tsp_vf_bufaddr                         block ;
tsp00_Int4                               * length;
tsp00_VfReturn                          * error ;
tsp00_ErrText                            errtext ;

{
   UINT4                                byt_num; /* place in buffer */
   UINT4                                byt_rem; /* remainder   */
   UINT4                                dst_pos; /* block pos. */

   DBG1 (( MF__,"e06_read_bin_fix: called \n" ));
   DBG1 (( MF__,"e06_read_bin_fix: hf -> hf_reclen              = %d\n",
			     hf -> hf_reclen ));
   DBG1 (( MF__,"e06_read_bin_fix: hf -> hf_eff_format          = %s\n",
			     dbg_format [ hf -> hf_eff_format ] ));

   if ( ( hf -> hf_reclen >= MAXBUFFERLENGTH )
			 ||
	( hf -> hf_eff_format == vf_stream )   )
	/* FSt
	( ( hf -> hf_eff_format == vf_stream ) &&
	  ( * length >= MAXBUFFERLENGTH )         ) )
	*/
   {
      /* ***** not buffered ***** */

      if ( hf -> hf_eff_format == vf_stream )
      {
	 hf -> hf_bufend = * length ;
      }
      else
      {
	 hf -> hf_bufend = hf -> hf_reclen ;
	 * length        = hf -> hf_reclen ;
      }

      DBG1 (( MF__,"e06_read_bin_fix: * length                     = %d\n",
			       * length ));
      DBG1 (( MF__,"e06_read_bin_fix: hf -> hf_reclen              = %d\n",
			       hf -> hf_reclen ));

      /* ***** if the buffer pool is used check the ***** */
      /* ***** beginning address within the pool    ***** */

      /*
      if ( hf -> hf_block_beg_ptr != ( UINT1 * ) UNDEF )
      {
	 if ( block != ( tsp_vf_bufaddr ) hf -> hf_block_beg_ptr )
	 {
	    * error = VF_NOTOK ;
	    ERRTEXT(  errtext , DIFF_POOL_ADRS );
	    return ;
	 }
      }
      */

      DBG1 (( MF__,"e06_read_bin_fix: block                        = 0x%x\n",
				block ));


      /* ***** read the file content ***** */

      e06_get_xk_buffer ( hf , block , length , error , errtext );

      if ( ! hf -> hf_read_ora && hf -> hf_format != vf_4k_block )
      {
	 /* ***** EOF page : actual only rel 2.4  ***** */
	 /* *****            ( 1993-04-26 FSt ) : ***** */
	 /* *****            but also possible in ***** */
	 /* *****            migration 2.4 -> 3.0 ***** */

	 if ( hf -> hf_reclen == MAXBUFFERLENGTH )
	 {
	    DBG1 (( MF__,"e06_read_bin_fix: EOF-page test <-----------\n" )) ;

	    if ( ( ((char *)block) [ 0 ] == (char) -1 ) &&
		 ( ((char *)block) [ 1 ] == (char) -1 ) &&
		 ( ((char *)block) [ 2 ] == (char) -1 ) &&
		 ( ((char *)block) [ 3 ] == (char) -1 ) &&
		 ( ((char *)block) [ 4 ] == (char) -1 ) &&
		 ( ((char *)block) [ 5 ] == (char) -1 ) &&
		 ( ((char *)block) [ 6 ] == (char) -1 ) &&
		 ( ((char *)block) [ 7 ] == (char) -1 )     )
	    {
	       * error  = VF_EOF ;
	       * length = 0 ;

	       DBG1 (( MF__,"e06_read_bin_fix: * length   ( EOF-page )      = %d\n",
					 * length ));
	       return ;
	    }
	 }
      }

      hf -> hf_pos = hf -> hf_maxpos - 1;
      * length     = hf -> hf_maxpos ;

      DBG1 (( MF__,"e06_read_bin_fix: * length                     = %d\n",
				* length ));
      DBG1 (( MF__,"e06_read_bin_fix: hf -> hf_pos                 = %d\n",
				hf -> hf_pos ));

   }
   else
   {
      /* ***** record length < 4kB ***** */

      byt_num = (UINT4) (hf->hf_maxpos - hf->hf_pos - 1);
      if ( byt_num > hf->hf_reclen )
      {
	 /* ***** enough place for whole record ***** */

	 byt_num = hf->hf_reclen;
	 byt_rem = 0;
      }
      else
      {
	 /* ***** remainder ***** */

	 byt_rem = hf->hf_reclen - byt_num;
      }

      dst_pos = 0;
      if ( byt_num > ( UINT4 ) 0 )
      {
	 /* ***** use current buffer ***** */

	 SAPDB_memcpy ( ((char *)block), &hf->hf_bufptr[hf->hf_pos+1], byt_num );
	 hf->hf_pos += byt_num;
	 dst_pos     = byt_num;
      }

      if ( byt_rem > ( UINT4 ) 0 )
      {
	 /* ***** read next block to "hf -> hf_buf" ***** */

	 e06_getbuffer ( hf, error, errtext );
	 if ( *error == VF_OK)
	 {
	    SAPDB_memcpy ( &((char *)block)[dst_pos], hf->hf_bufptr, byt_rem);
	    hf->hf_pos = byt_rem - 1;
	 }
      }

      if ( * error == VF_OK )
      {
	 * length = hf -> hf_reclen ;
      }
      else
      {
	 * length = 0;
      }
   }

   DBG1 (( MF__,"re_bin_fix: length               %d \n", *length      ));
   DBG1 (( MF__,"re_bin_fix: error                %d \n", *error       ));
   DBG1 (( MF__,"re_bin_fix: returning \n" ));
}

/*==========================================================================*/

global	void	sqlfwrite ( hf_num , block , length , error , errtext )
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp_vf_bufaddr                  block ;
tsp00_Int4                        length ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext;

{ /* sqlfwrite */

    struct HOSTFILE_ALL             * hf;

    DBG1 (( MF__,"sqlfwrite: called , hf_num = %d \n", *hf_num));

    hf = & real_files [ * hf_num - 1 ] ;

    DBG1 (( MF__,"sqlfwrite       : length                      = %d\n", length));
    DBG1 (( MF__,"sqlfwrite       : block                       = %d\n", block));
    DBG1 (( MF__,"sqlfwrite       : hf -> hf_format             = %s\n",
			      dbg_format [ hf->hf_format ] )) ;
    DBG1 (( MF__,"sqlfwrite       : hf -> hf_reclen             = %d\n",
			      hf->hf_reclen )) ;


    * error = VF_OK;
    SAPDB_memset ( errtext , ' ' , sizeof ( tsp00_ErrText ) ) ;

    if ( e06_valid_fileno ( hf ) )
    {
	/* ***** hostfileno ok ***** */

	if ( hf->hf_state != HFS_ERROR &&
	    ( hf->hf_direction == VWRITE_EN00 ||
	      hf->hf_direction == VAPPEND_EN00 ||
	      hf->hf_direction == VOVERWRITE_EN00 ) )
	{
	    /* ***** write data record ***** */

	    if ( hf->hf_format == vf_plaintext )
	    {
		e06_write_asc_var ( hf, block, length, error,  errtext );
	    }
	    else
	    {
		/* ***** binary ***** */

		if ( ( hf->hf_reclen == 0 /*JH*/ ||
		       hf->hf_reclen == -1 ) &&
		     hf->hf_format != vf_stream )
		{
		    /* ***** binary variable ***** */

		    e06_write_bin_var ( hf, block, length, error,  errtext );
		}
		else
		{
		    /* ***** check length of binary fixed ***** */

		    if ( length == hf->hf_reclen   ||
			hf->hf_format==vf_4k_block || 
			hf->hf_format==vf_stream)
		    {
			e06_write_bin_fix ( hf, block, length,
							error , errtext );
		    }
		    else
		    {
			* error = VF_NOTOK  ;
			ERRTEXT ( errtext ,"non-constant record length");
		    }
		}
	    }

	    if ( hf->hf_bufbase + hf->hf_pos + 1 > hf->hf_byte_num )
	    {
		hf->hf_byte_num = hf->hf_bufbase + hf->hf_pos + 1;
	    }

	}
    }
    else
    {
	* error = VF_NOTOK;
	ERRTEXT(  errtext, BAD_FILENO_ERRTEXT );
    }

    DBG1 (( MF__,"sqlfwrite: returning \n" ));

} /* sqlfwrite */


/*==========================================================================*/


static void     e06_write_bin_var ( hf , block , length , error , errtext )


/*
 * Writes binary variable length records
 */

struct HOSTFILE_ALL                    *hf;
tsp_vf_bufaddr                         block;
tsp00_Int4                               length;
tsp00_VfReturn                          *error;
tsp00_ErrText                            errtext;

{ /* e06_write_bin_var */

    tsp00_Int4                               cur_file_pos, next_4k_pos;

    DBG1 (( MF__,"e06_wr_bin_var  : called \n" ));
    DBG1 (( MF__,"e06_wr_bin_var  : length                      = %d\n",
			      length ));

    * error = VF_OK ;

    if ( length > 0 )
    {
	/* ****** length of record > 0  ***** */

	if ( length < MAXBUFFERLENGTH - 1 )
	{
	    /* ***** TRUE data record :                         ***** */
	    /* ***** The following ensures that variable length ***** */
	    /* ***** records are not split across 4K record     ***** */
	    /* ***** boundaries.  This allows any variable      ***** */
	    /* ***** file to read with only a 4K buffer, even   ***** */
	    /* ***** if it was created with a larger buffer.    ***** */

	    cur_file_pos = hf->hf_bufbase + hf->hf_pos + 1 ;
	    next_4k_pos = ( cur_file_pos + 1 ) -
			  ( ( cur_file_pos + 1 ) % MAXBUFFERLENGTH );

	    DBG1 (( MF__,"e06_wr_bin_var  : hf -> hf_bufbase            = %d\n",
				      hf -> hf_bufbase ));
	    DBG1 (( MF__,"e06_wr_bin_var  : hf -> hf_pos           (1)  = %d\n",
				      hf -> hf_pos ));
	    DBG1 (( MF__,"e06_wr_bin_var  : cur_file_pos                = %d\n",
				      cur_file_pos ));
	    DBG1 (( MF__,"e06_wr_bin_var  : next_4k_pos            (1)  = %d\n",
				      next_4k_pos ));

	    if ( ( cur_file_pos + 1 ) % MAXBUFFERLENGTH != 0 )
		next_4k_pos += MAXBUFFERLENGTH;

	    DBG1 (( MF__,"e06_wr_bin_var  : next_4k_pos            (2)  = %d\n",
				      next_4k_pos ));
	    DBG1 (( MF__,"e06_wr_bin_var  : next_4k_pos-(cur_file_pos+1)= %d\n",
				      next_4k_pos - ( cur_file_pos + 1 ) ));

	    if ( next_4k_pos - ( cur_file_pos + 1 ) <= length + 2 )
	    {
		/* ***** space in hf->hf_buf less than ***** */
		/* ***** record to write               ***** */

		SAPDB_memset ( & hf -> hf_bufptr [ hf -> hf_pos + 1 ], '\0',
			 (next_4k_pos - ( cur_file_pos + 1 ) ) );

		hf->hf_pos += ( next_4k_pos - cur_file_pos - 1 ) ;

		DBG1 (( MF__,"e06_wr_bin_var  : hf -> hf_pos           (2)  = %d\n",
					  hf -> hf_pos ));

		if ( hf->hf_pos + 1 >= hf->hf_maxpos )
		    e06_putbuffer ( hf, error, errtext );

	    }

	    if ( *error == VF_OK)
	    {
		hf->hf_bufptr[ ++hf->hf_pos ] = (UCHAR) (length / 256);
		hf->hf_bufptr[ ++hf->hf_pos ] = (UCHAR) (length % 256);

		DBG1 (( MF__,"e06_wr_bin_var  : hf -> hf_bufptr             = %d\n",
					     hf -> hf_bufptr ));
		DBG1 (( MF__,"e06_wr_bin_var  : hf -> hf_pos       (3)      = %d\n",
					     hf -> hf_pos ));
		DBG1 (( MF__,"e06_wr_bin_var  : Cpy to address bufptr[pos+1]= %d\n",
					     &hf->hf_bufptr[ hf->hf_pos+1 ] ));

		SAPDB_memcpy ( & ( hf -> hf_bufptr [ hf -> hf_pos + 1 ] ) ,
			 ((char *)block) , length );
		hf -> hf_pos = hf -> hf_pos + length ;
		DBG1 (( MF__,"e06_wr_bin_var  : hf -> hf_pos       (4)      = %d\n",
					     hf -> hf_pos ));
	    }

	    if ( hf->hf_pos < (hf->hf_maxpos - 1) && ( ! odd(hf->hf_pos) ) )
	    { /* odd */
		hf->hf_bufptr[++hf->hf_pos] = NUL;
	    } /* odd */
	}  /* TRUE data record */
	else
	{ /* length >= 4094 */
	    *error = VF_NOTOK;
	    ERRTEXT( errtext, ILL_REC_LEN_ERRTEXT );
	}
    } /* record length > 0 */

    DBG1 (( MF__,"e06_wr_bin_var  : hf -> hf_pos       (5)      = %d\n",
			      hf -> hf_pos ));
    DBG1 (( MF__,"e06_wr_bin_var: returning \n" ));

}  /* e06_write_bin_var */

/*==========================================================================*/

static void     e06_write_asc_var ( hf , block , length , error , errtext )

/*
 * Write ASCII record
 */

struct HOSTFILE_ALL                    *hf;
tsp_vf_bufaddr                         block;
tsp00_Int4                               length;
tsp00_VfReturn                          *error;
tsp00_ErrText                            errtext;

{ /* e06_write_asc_var*/

	UINT4                              byt_num;
	UINT4                              byt_rem;
	long                               src_pos;
	UINT4                              byt_written;
	int                                write_status;

	struct                 /* buffer with room for appended newline */
	{
	    tsp00_Buf                             buf ;
	    UCHAR                              newline [ NEWLINE_LEN ];
	}  lblock ;

	DBG1 (( MF__,"wr_asc_var: called \n" ));

	if ( length < 0 )
	{
	    *error = VF_NOTOK;
	    ERRTEXT( errtext, MIN_REC_ERRTEXT );
	}
	else
	{
	    if ( length >= MAXBUFFERLENGTH )
	    {
		* error = VF_NOTOK;
		ERRTEXT( errtext, MAX_REC_ERRTEXT );
	    }
	    else
	    {
		/* ***** TRUE data record ***** */

		if ( hf->hf_blocked == TRUE )
		{
		    /* ***** blocked ***** */

		    byt_num = (UINT4) (hf->hf_maxpos - hf->hf_pos - 1);

		    if ( byt_num >= length + 1 )
		    {
			/* ***** enough space ***** */

			byt_num = length;
			byt_rem = 0;
		    }
		    else
		    {
			/* ***** reminder ***** */

			byt_rem = length - byt_num;
		    }

		    /* block position */
		    src_pos = 0;

		    if ( byt_num > ( UINT4 ) 0 )
		    {
			/* ***** use current buffer ***** */

			(void)SAPDB_memcpy( & hf -> hf_bufptr [ hf -> hf_pos + 1 ],
				          block ,
						  (size_t)byt_num );

			hf->hf_pos   = hf->hf_pos + byt_num;
			src_pos      = byt_num;
		    }

		    if ( hf->hf_pos == hf->hf_maxpos - 1 )
		    {
			/* ***** write hf->hf_buf ***** */

			e06_putbuffer ( hf, error, errtext );
		    }

		    if ( *error == VF_OK)
		    {
			if ( byt_rem > ( UINT4 ) 0 )
			{
			    /* ***** new buffer ***** */

			    (void)SAPDB_memcpy( &hf->hf_bufptr[hf->hf_pos+1],
					          &((char *)block)[src_pos],
					          (size_t)byt_rem );
			    hf->hf_pos = byt_rem - 1;
			}

			hf->hf_bufptr[++hf->hf_pos] = NL;
		    }

		} /* variable size record, blocked */
		else
		{
		    /* ***** unblocked, use current buffer ***** */

		    DBG1 (( MF__,"wr_asc_var: unblocked            %d \n", length  ));

		    hf->hf_pos = 0;
		    (void)SAPDB_memcpy( lblock.buf, 
						  block, 
						  (size_t)length );

		    lblock.buf[length] = NL;

		    write_status = VEN06_WRITEFILE ( hf -> hf_fildes ,
						(char *) & lblock ,
						(unsigned)(length + NEWLINE_LEN ),
						& byt_written );
		    DBG1 (( MF__,"wr_asc_var: written %d \n", byt_written ));

		    if ( write_status != 0      ||
			 byt_written != length+NEWLINE_LEN )
		    {
			 *error = VF_NOTOK;
			 hf->hf_state = HFS_ERROR;
			 ERRTEXT( errtext, WRITE_PERM_DEN_ERRTEXT );
			 DBG1 (( MF__,"e06_wr_asc_var: error, %s\n", sqlerrs() ))
		    }
		    else
		    {
			/* ***** write ok ***** */

			* error        = VF_OK;
			hf -> hf_state = HFS_USED;
			hf -> hf_pos   = UNDEF;
		    }
		} /* unblocked, use current buffer */
	    } /* TRUE data record      */
	}
	DBG1 (( MF__,"wr_asc_var: returning \n" ));

} /* e06_write_asc_var */

/*==========================================================================*/

static void     e06_write_bin_fix ( hf , block , length , error , errtext )

struct HOSTFILE_ALL                    *hf;
tsp_vf_bufaddr                         block;
tsp00_Int4                               length;
tsp00_VfReturn                          *error;
tsp00_ErrText                            errtext;

{ /* e06_write_bin_fix */

	UINT4                                  byt_num;
	UINT4                                  byt_rem;
	long                                   src_pos;

	DBG1 (( MF__,"wr_bin_fix: called \n" ));
	DBG1 (( MF__,"wr_bin_fix: hf->hf_reclen   %d\n", hf->hf_reclen ));

	if ( hf -> hf_reclen >= MAXBUFFERLENGTH ||
	     hf -> hf_format == vf_4k_block     ||
	     hf -> hf_format == vf_stream  )
	    { /* buffer unit */
	    hf->hf_pos   = length - 1;
	    e06_put_xk_buffer ( hf, block, error, errtext );
	    }
	 else
	    { /* part of buffer */
	    byt_num = (UINT4) (hf->hf_maxpos - hf->hf_pos - 1);
	    if ( byt_num > hf->hf_reclen )
		 { /* enough space */
		 byt_num = hf->hf_reclen;
		 byt_rem = 0;
		 }
	    else
		 { /* reminder */
		 byt_rem = hf->hf_reclen - byt_num;
		 }

	    /* block position */
	    src_pos = 0;

	    if ( byt_num > ( UINT4 ) 0 )
		{ /* use current buffer */
		SAPDB_memcpy ( &hf->hf_bufptr[hf->hf_pos+1], ((char *)block), byt_num );
		hf->hf_pos += byt_num;
		src_pos     = byt_num;
		} /* use current buffer */
	    DBG1 (( MF__,"wr_bin_fix: byt_num         %d \n", byt_num));
	    DBG1 (( MF__,"wr_bin_fix: byt_rem         %d \n", byt_rem));
	    DBG1 (( MF__,"wr_bin_fix: hf->hf_pos      %d \n", hf->hf_pos ));
	    DBG1 (( MF__,"wr_bin_fix: hf->hf_bufend   %d \n", hf->hf_bufend ));
	    DBG1 (( MF__,"wr_bin_fix: hf->hf_bufbase  %d \n", hf->hf_bufbase ));

	    if ( byt_rem > ( UINT4 ) 0 )

		{  /* write buffer and start new buffer */
		e06_putbuffer ( hf, error, errtext );
		if ( *error == VF_OK)
		    {
		    if ( src_pos    + byt_rem <= MAXBUFFERLENGTH   ||
			 hf->hf_pos + byt_rem <= MAXBUFFERLENGTH )
			{
			SAPDB_memcpy ( &hf->hf_bufptr[hf->hf_pos+1],
				 &((char *)block)[src_pos],
				 byt_rem );
			}
		    else
			{
			DBG1 (( MF__,"wr_bin_fix: illegal call of SAPDB_memcpy \n" ));
			exit ( -1 );
			}

		    hf->hf_pos = byt_rem - 1;

		    } /* error ok */
		}  /* write buffer and start new buffer */
	    }   /* part of buffer */

	DBG1 (( MF__,"wr_bin_fix: returning \n" ));

}  /* e06_write_bin_fix */

/*==========================================================================*/

void                    en06_fseek ( hf_num , rec_no , error , errtext, allowtext )
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp00_Int4                        rec_no ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;
int                             allowtext ;
{
    long                        file_pos ;
    long                        last_pos ;
    long                        rec_pos ;
    struct HOSTFILE_ALL         * hf;

    DBG1 (( MF__,"sqlfseek: called \n" ));

    hf = & real_files [ * hf_num - 1 ] ;

    DBG1 (( MF__,"sqlfseek        : rec_no                       = %d\n",
			     rec_no ));
    DBG1 (( MF__,"sqlfseek        : hf -> hf_rd_eof    (1)       = %d\n",
			     hf -> hf_rd_eof ));


    /* ***** initialize return parameter ***** */

    * error = VF_OK ;
    SAPDB_memset ( errtext , ' ' , sizeof ( tsp00_ErrText ) ) ;


    /* ***** initialize eof parameter ***** */

    hf->hf_rd_eof = FALSE ;
    DBG1 (( MF__,"sqlfseek        : hf -> hf_rd_eof    (2)       = %d\n",
			     hf -> hf_rd_eof ));


    /* ***** check for valid internal file number ***** */

    if ( ! e06_valid_fileno ( hf ) )
    {
	* error = VF_NOTOK;
	ERRTEXT( errtext, BAD_FILENO_ERRTEXT );
	return;
    }


    /* ***** seek only for regular files (no stdio, tape, etc. ) ***** */
    /* ***** rec_no must be greater than zero                    ***** */

    if ( ( strcmp ( (const char *)hf->hf_name, std_in  ) != 0 ) &&
	 ( strcmp ( (const char *)hf->hf_name, std_out ) != 0 ) &&
	 ( strcmp ( (const char *)hf->hf_name, std_err ) != 0 ) &&
	 ( hf->hf_file_type != FIFOFILE ) )
    {
	/* ***** no stdio ***** */

	if ( rec_no > 0 )
	{
	    /* ***** record number ok ***** */

	    if ( hf->hf_format == vf_plaintext )
	    {
                if (!allowtext) {
                    /* ***** today :           ***** */
                    /* ***** no seek for ASCII ***** */

                    * error = VF_NOSEEK ;
                    ERRTEXT( errtext , NOSEEK_ERRTEXT );
                    return;
                }
                if ( hf->hf_direction != VREAD_EN00 )
                    e06_putbuffer( hf , error, errtext );

                if ( * error == VF_OK )
                {
		    /* adjust rec_no because externally, first byte is "1" */
		    --rec_no;
                    e06_posbuffer ( hf , rec_no , ABS_OFFSET ,
                                & file_pos , error , errtext );

                    if ( *error == VF_OK )
                    {
                        hf->hf_second_asc_rd = FALSE;
                        hf -> hf_maxpos = 0; /* This should have been
                            done in e06_posbuffer, but who knows what
                            this would break for binary files DD */
                        clear_hf_buf ( hf );
                        e06_getbuffer ( hf, error , errtext );
                    }
                }
	    }
	    else
	    {
		/* ***** binary ***** */

		if ( hf->hf_reclen  == 0 )
		{
		    /* ***** today :            ***** */
		    /* ***** no seek for binary ***** */
		    /* ***** variable           ***** */

		    * error = VF_NOSEEK ;
		    ERRTEXT( errtext , NOSEEK_ERRTEXT );
		    return;
		}
		else
		{
		    /* ***** seek for binary fixed ***** */

		    if ( hf->hf_format == vf_stream )
			rec_pos = rec_no + KB08 - 1 ;
		    else
			rec_pos = ( hf->hf_reclen * ( rec_no - 1 ) )
						  + hf->hf_HeaderLength - 1 ;

		    DBG1 (( MF__,"sqlfseek        : rec_pos                      = %d\n",
					     rec_pos ));

		    if ( hf->hf_format == vf_record &&
			 hf->hf_direction == VREAD_EN00 )
			rec_pos ++ ;

		    if ( hf->hf_file_type == REGFILE )
		    {
			/* ***** regular file ***** */

			last_pos = hf->hf_byte_num;
			* error = VF_OK;
		    }
		    else
		    {
			/* ***** no regular file ***** */

			last_pos = MB2 ;
			* error  = VF_OK ;
		    }

		    DBG1 (( MF__,"sqlfseek        : last_pos                     = %d\n",
					     last_pos ));

		    if  ( * error == VF_OK )
		    {
			if ( rec_pos > ( last_pos - hf->hf_reclen ))
			{
			    /* ***** rec_pos > .. ***** */

			    * error = VF_EOF;
			    ERRTEXT ( errtext , NOSEEK_EOF_ERRTEXT );
			    hf->hf_state = HFS_END;
			}
			else
			{
			    /* ***** rec_pos <= .. ***** */

			    if ( hf->hf_direction != VREAD_EN00 )
				e06_putbuffer( hf , error, errtext );

			    if ( * error == VF_OK )
			    {
				e06_posbuffer ( hf , rec_pos , ABS_OFFSET ,
					    & file_pos , error , errtext );

				if ( ( hf->hf_reclen < MAXBUFFERLENGTH )
					     &&
				     ( *error == VF_OK ) )
				{
				    /* ***** hf_reclen < mbl ***** */

				    clear_hf_buf ( hf );
				    e06_getbuffer ( hf, error , errtext );
				}
			    }
			}
		    }
		}
	    }
	}
	else
	{
	    /* ***** incorrect rec_no ***** */

	    hf->hf_state = HFS_ERROR ;
	    * error = VF_NOTOK;
	    ERRTEXT ( errtext , ILL_REC_NO_ERRTEXT );
	}
    }
    else
    {
	* error = VF_NOSEEK ;
	ERRTEXT( errtext , NOSEEK_ERRTEXT );
    }

    DBG1 (( MF__,"sqlfseek: returning \n" ));
}

void                    sqlfseek ( hf_num , rec_no , error , errtext )
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp00_Int4                        rec_no ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;
{
    en06_fseek (hf_num, rec_no, error, errtext, FALSE);
}

void                    sqlftextseek ( hf_num , rec_no , error , errtext )
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp00_Int4                        rec_no ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;
{
    en06_fseek (hf_num, rec_no, error, errtext, TRUE);
}

/*==========================================================================*/

void            sqlferase ( pascal_name , error , errtext )

tsp00_VFilename                             pascal_name ;
tsp00_VfReturn                             * error ;
tsp00_ErrText                               errtext ;

{
	VF_FILENAMEC                    hostfilename ;
	VF_FILENAMEC                    host_file_name ;
	tsp00_Bool                         not_ok ;
	int                             status ;
	VEN06_FILEDATA                  info_buffer ;

	DBG1 (( MF__,"sqlferase: called \n" ));


	/* ***** initializing ***** */

	* error = VF_OK ;
	SAPDB_memset ( errtext , ' ' , sizeof ( tsp00_ErrText ) ) ;


	/* ***** change file name string from EBCDIC to ASCII ***** */

	eo46PtoC ( (char *)host_file_name , pascal_name , sizeof ( tsp00_VFilename ) );

	DBG1 (( MF__,"sqlferase       : host_file_name               '%s'\n",
				host_file_name ));


	/* ***** substitute ENV vars ( if existing ) ***** */

	if ( strchr ( (const char *)host_file_name, '$' ) )
	{
		sql48_change_log ( host_file_name , hostfilename ,
				    & not_ok );

		DBG1 (( MF__,"sqlferase       : hostfilename                 '%s' \n",
					 hostfilename ));
		if ( not_ok )
		{
			* error       = VF_NOTOK;
			ERRTEXT( errtext, FN_EXPAND_ERRTEXT );
			return ;
		}
	}
	else
	{
		strcpy ( (char *)hostfilename , (const char *)host_file_name ) ;
	}


	/* ***** check file for special device ****** */

	DBG1 (( MF__,"sqlferase       : hostfilename                 = '%s'\n",
				  hostfilename ));

	status =  VEN06_STAT ( (const char *)hostfilename , & info_buffer );

	DBG1 (( MF__,"sqlferase       : status                       = %d\n",
				 status        ));

	if ( status == 0 )
	{
		/* ***** file status ok ***** */

		if ( VEN06_ISREGFILE ( info_buffer ) )
		{
			/* ***** ordinary file ***** */

			if ( VEN06_FILEDELETE ( (char *) hostfilename ) != 0 )
			{
				/* ***** unlink ***** */

				* error       = VF_NOTOK;
				ERRTEXT ( errtext, UNLINK_ERRTEXT );
			}
		}
		else
		{
			/* ***** no regfile ***** */

			* error       = VF_NOTOK;
			ERRTEXT( errtext, NO_REG_FILE );
		}
	}
	else
	{
		/* ***** error in stat ***** */

		* error       = VF_NOTOK;
		ERRTEXT ( errtext, "error by get file status" );
	}

	DBG1 (( MF__,"sqlferase: returning \n" ));

} /* sqlferase */

/*==========================================================================*/

void                    sqlftell ( hf_num , resultpos , error , errtext )
UINT4                           * hf_num ;	/* HOSTFILE_MIN..HOSTFILE_MAX */
tsp00_Int4                        * resultpos ;
tsp00_VfReturn                   * error ;
tsp00_ErrText                     errtext ;
{
    struct HOSTFILE_ALL         * hf;

    DBG1 (( MF__,"sqlftell: called \n" ));

    hf = & real_files [ * hf_num - 1 ] ;

    DBG1 (( MF__,"sqlftell        : hf -> hf_rd_eof    (1)       = %d\n",
			     hf -> hf_rd_eof ));


    /* ***** initialize return parameter ***** */

    * error = VF_OK ;
    SAPDB_memset ( errtext , ' ' , sizeof ( tsp00_ErrText ) ) ;

    /* ***** check for valid internal file number ***** */

    if ( ! e06_valid_fileno ( hf ) )
    {
	* error = VF_NOTOK;
	ERRTEXT( errtext, BAD_FILENO_ERRTEXT );
	return;
    }

    /* 
     * hf_pos is -1 after open or seek, pos into buf otherwise
     * bufbase is 1 before first index of buffer
     */
    *resultpos = (hf->hf_bufbase + 1) + max (hf->hf_pos, 0);
    /* adjust resultpos, because externally, first byte is "1" */
    ++(*resultpos);
    DBG1 (( MF__,"sqlftell:       bufbase: %d, pos: %d, tellpos: %d\n",
        hf->hf_bufbase, hf->hf_pos, *resultpos));

    DBG1 (( MF__,"sqlftell        : resultpos                    = %d\n",
			     *resultpos ));
    DBG1 (( MF__,"sqlftell: returning \n" ));
}
/*==========================================================================*/

static void     e06_getbuffer ( hf, error, errtext )

/*
 * Read from file to hf->hf_buf
 */

struct HOSTFILE_ALL                     * hf;
tsp00_VfReturn                           * error;
tsp00_ErrText                             errtext;

{
   tsp00_Int4                        byte_cnt;
   int                             read_status;

   DBG1 (( MF__,"e06_getbuffer: called \n" ));
   DBG1 (( MF__,"e06_getbuffer   : hf -> hf_name                = %s\n",
			     hf -> hf_name )) ;
   DBG1 (( MF__,"e06_getbuffer   : hf -> hf_readcnt             = %d\n",
			     hf -> hf_readcnt )) ;

   * error  = VF_OK;
   byte_cnt = 0;

   if ( ( ( hf->hf_direction == VREAD_EN00   ) ||
	  ( hf->hf_direction == VAPPEND_EN00 )    ) &&
	( hf->hf_state == HFS_USED  )   )
   {
      if ( hf -> hf_rd_eof )
      {
	 read_status = 0 ;
	 byte_cnt    = 0 ;
      }
      else
      {
#        ifdef DEVENV
	 read_status = ( ( byte_cnt = fileread ( hf->hf_bufptr, 1 ,
						  (int) hf->hf_readcnt,
						  hf->hf_rmfildes ))      < 0 );
	 hf -> hf_rd_eof = FALSE ;
#        else
	 read_status = VEN06_READFILE ( hf->hf_fildes, hf->hf_bufptr,
					(int) hf->hf_readcnt,
					& byte_cnt ,
					& hf -> hf_rd_eof ) ;
#        endif
      }

      DBG1 (( MF__,"e06_getbuffer   : byte_cnt                     = %d\n",
				byte_cnt ));
      DBG1 (( MF__,"e06_getbuffer   : read_status                  = %d\n",
				read_status ));
      DBG1 (( MF__,"e06_getbuffer   : hf -> hf_readcnt             = %d\n",
				hf -> hf_readcnt ));
      DBG1 (( MF__,"e06_getbuffer   : hf -> hf_rd_eof          (1) = %d\n",
				hf -> hf_rd_eof ));

      /* ***** read full buffer next time ***** */

      hf -> hf_readcnt = hf -> hf_bufend ;


      /* ***** check read status ***** */

      if ( read_status != 0 )
      {
	 /* ***** read_status != 0 ***** */

	 DBG1 (( MF__,"e06_getbuffer: read error, %s\n", sqlerrs() ));
	 * error         = VF_NOTOK ;
	 hf -> hf_state  = HFS_ERROR ;
	 hf -> hf_maxpos = UNDEF ;

	 ERRTEXT (errtext , sqlerrs() );
      }
      else
      {
	 /* ***** read_status == 0                     ***** */
	 /* ***** reset eof flag because you know that ***** */
	 /* ***** you are at the end of a file         ***** */

	 if  ( byte_cnt == 0 )
	 {
	    * error         = VF_EOF ;
	    hf -> hf_state  = HFS_END ;
	    byte_cnt        = UNDEF ;
	    hf -> hf_maxpos = UNDEF ;
	    hf -> hf_rd_eof = FALSE ;
	    ERRTEXT (errtext , BLANK_ERRTEXT ) ;
	 }
	 else
	 {
	    * error        = VF_OK ;
	    hf -> hf_state = HFS_USED ;
	 }

#if defined (OLDCODE)
	 hf -> hf_bufbase += hf -> hf_pos + 1 ;
#else
         hf -> hf_bufbase += hf -> hf_maxpos;
#endif
	 hf -> hf_pos      = UNDEF ;
	 hf -> hf_maxpos   = byte_cnt ;
      }
   }
   else
   {
      /* ***** direction != append and != read or file not used ***** */

      * error        = VF_NOTOK ;
      hf -> hf_state = HFS_END ;
      ERRTEXT( errtext, ILL_DIREC_ERRTEXT );
   }

   DBG1 (( MF__,"e06_getbuffer   : hf -> hf_pos                 = %d\n",
			     hf -> hf_pos ));
   DBG1 (( MF__,"e06_getbuffer   : hf -> hf_maxpos              = %d\n",
			     hf -> hf_maxpos ));
   DBG1 (( MF__,"e06_getbuffer   : hf -> hf_bufbase             = %d\n",
			     hf -> hf_bufbase ));
   DBG1 (( MF__,"e06_getbuffer   : hf -> hf_rd_eof          (2) = %d\n",
			     hf -> hf_rd_eof ));
   DBG1 (( MF__,"e06_getbuffer   : * error                      = %d\n",
			     * error ));
   return;
}

/*==========================================================================*/

static void     e06_get_xk_buffer ( hf , block , length , error , errtext )

struct HOSTFILE_ALL                    * hf ;
tsp_vf_bufaddr                         block ;
tsp00_Int4                               * length;
tsp00_VfReturn                          * error ;
tsp00_ErrText                            errtext ;


{ /* e06_get_xk_buffer */
	tsp00_Int4                               byte_cnt;
	int                                    read_status;


	DBG1 (( MF__,"e06_get_xk_buf: called \n" ));
	DBG1 (( MF__,"e06_get_xk_buf  : hf -> hf_fildes              = %d\n",
				  hf -> hf_fildes ));
	DBG1 (( MF__,"e06_get_xk_buf  : * length                     = %d\n",
				  * length ));
	DBG1 (( MF__,"e06_get_xk_buf  : block                        = 0x%x\n",
				  block ));

	* error = VF_OK;
	byte_cnt = 0;

	if ( ( ( hf->hf_direction == VREAD_EN00 ) ||
	       ( hf->hf_direction == VAPPEND_EN00 ) )
	     && ( hf->hf_state == HFS_USED  )   )

	{
	    /* ***** read ***** */

	    if ( hf -> hf_rd_eof )
	    {
		read_status = 0 ;
		byte_cnt    = 0 ;
	    }
	    else
	    {
		read_status = VEN06_READFILE ( hf -> hf_fildes , block ,
					  (int)(* length) , & byte_cnt ,
					   & hf -> hf_rd_eof );
	    }

	    DBG1 (( MF__,"e06_get_xk_buf  : read  byte_cnt               = %d\n",
					    byte_cnt ));
	    DBG1 (( MF__,"e06_get_xk_buf  : read_status                  = %d\n",
				      read_status ));
	    DBG1 (( MF__,"e06_get_xk_buf  : hf -> hf_readcnt             = %d\n",
				      hf -> hf_readcnt ));
	    DBG1 (( MF__,"e06_get_xk_buf  : hf -> hf_rd_eof          (1) = %d\n",
				      hf -> hf_rd_eof ));

	    hf->hf_readcnt = hf->hf_bufend;

	    if ( read_status != 0 )
	    {
		*error = VF_NOTOK ;
		hf->hf_state = HFS_ERROR ;
		hf->hf_maxpos = UNDEF ;
		DBG1 (( MF__,"e06_get_xk_buf  : read error, %s\n", sqlerrs() ));
		ERRTEXT (errtext , sqlerrs() );
	    }
	    else
	    {
		/* ***** read_status == 0                     ***** */
		/* ***** reset eof flag because you know that ***** */
		/* ***** you are at the end of a file         ***** */

		if ( byte_cnt == 0 )
		{
		    * error         = VF_EOF;
		    hf -> hf_state  = HFS_END;
		    hf -> hf_maxpos = UNDEF ;
		    hf -> hf_rd_eof = FALSE ;
		    ERRTEXT (errtext , BLANK_ERRTEXT ) ;
		}
		else
		{
		    *error = VF_OK;
		    hf->hf_state = HFS_USED;
		}

		hf->hf_bufbase += hf->hf_pos + 1;
		hf->hf_pos      = UNDEF  ;
		hf->hf_maxpos   = byte_cnt ;
	    }
	}
	else
	{
	    /* ***** dir. != append and != read or file not used ***** */

	    hf->hf_state = HFS_END;
	    *error = VF_NOTOK;
	    ERRTEXT( errtext, ILL_DIREC_ERRTEXT );
	}

	DBG1 (( MF__,"e06_get_xk_buf  : hf -> hf_pos                 = %d\n",
				  hf->hf_pos ));
	DBG1 (( MF__,"e06_get_xk_buf  : hf -> hf_maxpos              = %d\n",
				  hf->hf_maxpos ));
	DBG1 (( MF__,"e06_get_xk_buf  : hf -> hf_bufbase             = %d\n",
				  hf -> hf_bufbase ));
	DBG1 (( MF__,"e06_get_xk_buf  : hf -> hf_rd_eof          (2) = %d\n",
				  hf -> hf_rd_eof ));
	DBG1 (( MF__,"e06_get_xk_buf  : * error                      = %d\n",
				  * error       ));

	return;

} /* e06_get_xk_buffer */

/*==========================================================================*/

static void     e06_putbuffer ( hf , error , errtext )

/*
 * Write from hf->hf_buf to file
 */

struct HOSTFILE_ALL                    *hf;
tsp00_VfReturn                          *error;
tsp00_ErrText                            errtext;


{ /* e06_putbuffer */
	tsp00_Int4            res_pos , wr_pos ;
	int                 write_status ;


	DBG1 (( MF__,"e06_putbuf: called \n" ));
	DBG1 (( MF__,"e06_putbuf      : hf -> hf_pos            (1)  = %d\n",
				  hf -> hf_pos ));
	DBG1 (( MF__,"e06_putbuf      : hf -> hf_bufptr         (1)  = 0x%x\n",
				  hf -> hf_bufptr ));

	res_pos = wr_pos = 0;
	* error = VF_OK;

	if ( ( ( hf->hf_direction == VWRITE_EN00     ) ||
	       ( hf->hf_direction == VOVERWRITE_EN00 ) ||
	       ( hf->hf_direction == VAPPEND_EN00    )    ) &&
	     ( hf->hf_state == HFS_USED )                  )

	{ /* write */

	    if ( hf->hf_pos < 0 )
		return;

	    wr_pos = hf -> hf_pos + 1 ;

	    DBG1 (( MF__,"e06_putbuf      : wr_pos                       = %d\n",
				      wr_pos ));

	    if ( hf -> hf_pos > - 1 )
	    {   /* something in buffer */
		DBG1 (( MF__,"e06_putbuf: before VEN06_WRITEFILE--------\n" ));
		write_status = VEN06_WRITEFILE ( hf -> hf_fildes ,
						 ( char * ) hf -> hf_bufptr ,
						 (unsigned) wr_pos ,
						 & res_pos );
		DBG1 (( MF__,"e06_putbuf      : written bytes, res_pos       = %d\n",
							res_pos ));

		if ( write_status != 0 || res_pos != wr_pos /* - 1 ?? */)
		{   /* write failed */
		    *error = VF_NOTOK;
		    hf->hf_state = HFS_ERROR ;
		    ERRTEXT ( errtext, WRITE_PERM_DEN_ERRTEXT );
		    DBG1 (( MF__,"e06_putbuf: error, %s\n", sqlerrs() ))
		}
		else
		{ /* write ok */

		    DBG1 (( MF__,"e06_putbuf      :write ok,written bytes(wr_pos)= %d\n",
							wr_pos ));
		    DBG1 (( MF__,"e06_putbuf      : hf -> hf_bufbase (before)    = %d\n",
					      hf -> hf_bufbase  ));

		    hf->hf_bufbase += hf->hf_pos + 1 ;
		    hf->hf_maxpos   = hf->hf_bufend;
		    hf->hf_pos      = UNDEF ;

		    DBG1 (( MF__,"e06_putbuf      : hf -> hf_bufbase (after)     = %d\n",
					      hf -> hf_bufbase  ));
		    DBG1 (( MF__,"e06_putbuf      : hf -> hf_bufend=hf_maxpos    = %d\n",
					      hf -> hf_bufend  ));
		    DBG1 (( MF__,"e06_putbuf      : hf -> hf_pos             (2) = %d\n",
					      hf -> hf_pos ));

		}   /* write ok */
	    }   /* something in buffer */
	}   /* write */
	else
	    { /* no write */
	    *error = VF_NOTOK;
	    hf->hf_state = HFS_ERROR;
	    ERRTEXT( errtext, ILL_DIREC_ERRTEXT );
	    } /* no write */

	DBG1 (( MF__,"e06_putbuf      : * error                      = %d\n",
				  * error ));
	DBG1 (( MF__,"e06_putbuf      : hf -> hf_bufptr         (2)  = 0x%x\n",
				  hf -> hf_bufptr ));

} /* e06_putbuffer */

/*==========================================================================*/

static void     e06_pbuf_and_fill_up ( hf , error , errtext )

/*
 * Write from hf->hf_buf to file
 */

struct HOSTFILE_ALL                    * hf;
tsp00_VfReturn                          * error;
tsp00_ErrText                            errtext;


{
    tsp00_Int4            cur_file_pos, next_4k_pos;
    tsp00_Int4            res_pos , wr_pos ;
    int                 write_status ;


    DBG1 (( MF__,"e06_pbuf_and_fill_up : called \n" ));
    DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_pos            (1)  = %d\n",
			      hf -> hf_pos ));
    DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_bufptr         (1)  = 0x%x\n",
			      hf -> hf_bufptr ));

    res_pos = wr_pos = 0;
    * error = VF_OK;

    if ( ( ( hf -> hf_direction == VWRITE_EN00     ) ||
	   ( hf -> hf_direction == VOVERWRITE_EN00 ) ||
	   ( hf -> hf_direction == VAPPEND_EN00    )    ) &&
	 ( hf -> hf_state == HFS_USED )                  )

    {
	if ( ( hf -> hf_format == vf_plaintext ) &&
	     ( hf -> hf_file_type == SPECFILE  )
	   )
	{
	    cur_file_pos = hf -> hf_bufbase + hf -> hf_pos + 1 ;
	    next_4k_pos = ( cur_file_pos + 1 ) -
			  ( ( cur_file_pos + 1 ) % MAXBUFFERLENGTH );

	    DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_bufbase            = %d\n",
				      hf -> hf_bufbase ));
	    DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_pos           (1)  = %d\n",
				      hf -> hf_pos ));
	    DBG1 (( MF__,"e06_pbuf_and_fil: cur_file_pos                = %d\n",
				      cur_file_pos ));
	    DBG1 (( MF__,"e06_pbuf_and_fil: next_4k_pos            (1)  = %d\n",
				      next_4k_pos ));

	    if ( ( cur_file_pos + 1 ) % MAXBUFFERLENGTH != 0 )
		next_4k_pos += MAXBUFFERLENGTH;

	    DBG1 (( MF__,"e06_pbuf_and_fil: next_4k_pos            (2)  = %d\n",
				      next_4k_pos ));
	    DBG1 (( MF__,"e06_pbuf_and_fil: next_4k_pos-(cur_file_pos+1)= %d\n",
				      next_4k_pos - ( cur_file_pos + 1 ) ));


	    /* ***** fill up buffer ***** */

	    SAPDB_memset ( & hf -> hf_bufptr [ hf -> hf_pos + 1 ] , '\n' ,
		  ( next_4k_pos - ( cur_file_pos + 1 ) ) ) ;

	    wr_pos = MAXBUFFERLENGTH ;
	}
	else
	{
	    wr_pos = hf -> hf_pos + 1 ;
	}


	DBG1 (( MF__,"e06_pbuf_and_fil: wr_pos                       = %d\n",
				  wr_pos ));


	/* ***** write ***** */

	if ( hf -> hf_pos < 0 )
	    return;

	if ( hf -> hf_pos > - 1 )
	{
	    /* ***** something in buffer ***** */

	    DBG1 (( MF__,"e06_putbuf: before VEN06_WRITEFILE--------\n" ));

	    write_status = VEN06_WRITEFILE ( hf -> hf_fildes ,
					     ( char * ) hf -> hf_bufptr ,
					     (unsigned) wr_pos ,
					     & res_pos );

	    DBG1 (( MF__,"e06_pbuf_and_fil: written bytes, res_pos       = %d\n",
						     res_pos ));

	    if ( write_status != 0 || res_pos != wr_pos /* - 1 ?? */)
	    {
		/* ***** write failed ***** */

		* error = VF_NOTOK;
		hf -> hf_state = HFS_ERROR ;
		ERRTEXT ( errtext, WRITE_PERM_DEN_ERRTEXT );
		DBG1 (( MF__,"e06_pbuf_and_fil: error                        = %s\n",
					  sqlerrs() ))
	    }
	    else
	    {
		/* ***** write ok ***** */

		DBG1 (( MF__,"e06_pbuf_and_fil:write ok,written bytes(wr_pos)= %d\n",
								wr_pos ));
		DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_bufbase (before)    = %d\n",
					  hf -> hf_bufbase  ));

		hf -> hf_bufbase += wr_pos ;
		hf -> hf_maxpos   = hf -> hf_bufend;
		hf -> hf_pos      = UNDEF ;

		DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_bufbase (after)     = %d\n",
					  hf -> hf_bufbase  ));
		DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_bufend=hf_maxpos    = %d\n",
					  hf -> hf_bufend  ));
		DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_pos             (2) = %d\n",
					  hf -> hf_pos ));

	    }   /* write ok */
	}   /* something in buffer */
    }   /* write */
    else
    {
	/* ***** no write ***** */

	* error        = VF_NOTOK;
	hf -> hf_state = HFS_ERROR;
	ERRTEXT( errtext, ILL_DIREC_ERRTEXT );
    } /* no write */

    DBG1 (( MF__,"e06_pbuf_and_fil: * error                      = %d\n",
			      * error ));
    DBG1 (( MF__,"e06_pbuf_and_fil: hf -> hf_bufptr         (2)  = 0x%x\n",
			      hf -> hf_bufptr ));

}

/*==========================================================================*/

static void             e06_put_xk_buffer ( hf , block , error , errtext )

/*
 * Write from hf->hf_buf to file
 */

struct HOSTFILE_ALL                    *hf;
tsp_vf_bufaddr                         block ;
tsp00_VfReturn                          *error;
tsp00_ErrText                            errtext;


{ /* e06_put_xk_buffer */
	tsp00_Int4                       res_pos,wr_pos ;
	int                            write_status;

	DBG1 (( MF__,"put_xk_buf: called \n" ));

	res_pos = wr_pos =0;
	*error = VF_OK;

	if (( ( hf->hf_direction == VWRITE_EN00     ) ||
	       ( hf->hf_direction == VOVERWRITE_EN00 ) ||
	       ( hf->hf_direction == VAPPEND_EN00    )    ) &&
	     ( hf->hf_state == HFS_USED )                  )

	    { /* write */

	    wr_pos = hf -> hf_pos + 1 ;

	    if ( hf->hf_pos > UNDEF )
		{ /* something in buffer */

		DBG1 (( MF__,"e06_put_xk_buf: before VEN06_WRITEFILE--------\n" ));
		write_status = VEN06_WRITEFILE ( hf->hf_fildes, block,
						 wr_pos, & res_pos );
		DBG1 (( MF__,"put_xk_buf: written  %d \n", res_pos ));

		if ( write_status != 0 || res_pos != wr_pos )
		    { /* write failed */
		    *error = VF_NOTOK;
		    hf->hf_state = HFS_ERROR ;
		    ERRTEXT ( errtext ,WRITE_PERM_DEN_ERRTEXT );
		    DBG1 (( MF__,"e06_put_xk_buf: error, %s\n", sqlerrs() ))
		    }
		else
		    { /* write ok */

		    DBG1 (( MF__,"put_xk_buf: write sum %d \n", wr_pos ));

		    hf->hf_bufbase += hf->hf_pos + 1;
		    hf->hf_maxpos   = hf->hf_bufend;
		    hf->hf_pos      = UNDEF ;

		    }   /* write ok */
		}   /* something in buffer */
	    } /* write */
	else
	    { /* no write */
	    *error = VF_NOTOK;
	    hf->hf_state = HFS_ERROR;
	    ERRTEXT( errtext, ILL_DIREC_ERRTEXT );
	    } /* no write */

	DBG1 (( MF__,"put_xk_buf: error           %d \n", *error       ));

	return;

} /* e06_put_xk_buffer */

/*==========================================================================*/

static void             e06_posbuffer ( hf , offset , whence , file_pos ,
					error , errtext )

/*
 * Positions file and hf->hf_buf
 */

struct HOSTFILE_ALL                    * hf ;
long                                   offset ;
tsp00_Int2                               whence ;
long                                   * file_pos ;
tsp00_VfReturn                          * error ;
tsp00_ErrText                            errtext ;

{
	int                                seek_status;


	DBG1 (( MF__,"e06_posbuffer: called \n" ));
	DBG1 (( MF__,"e06_posbuffer   : offset                       = %d\n",
				 offset ));

	if ( ( hf -> hf_file_type == REGFILE  ) ||
	     ( hf -> hf_file_type == SPECFILE )    )
	{
	    if ( whence == ABS_OFFSET )
	    {
		seek_status = VEN06_SEEKABS( hf -> hf_fildes ,
					     offset , file_pos );
	    }
	    else
		if ( whence == SIZE_OFFSET )
		{
		    seek_status = VEN06_SEEKEND ( hf -> hf_fildes ,
						  offset , file_pos );
		}
		else
		{
		    DBG1 (( MF__,"e06_posbuffer   : illegal call of e06_posbuffer, bad whence parm \n" ));
		    exit ( -1 );
		}

	    if ( seek_status != 0 )
	    {
		* error          = VF_NOTOK ;
		hf -> hf_state   = HFS_ERROR ;
		hf -> hf_maxpos  = 0 ;
		hf -> hf_readcnt = hf->hf_HeaderLength/* MAXBUFFERLENGTH */ ;
		hf -> hf_pos     = UNDEF  ;
		ERRTEXT ( errtext ,SEEK_ERRTEXT) ;
	    }
	    else
	    {
		* error          = VF_OK ;
		hf -> hf_state   = HFS_USED ;
		hf -> hf_pos     = UNDEF ;
		hf -> hf_readcnt = hf -> hf_bufend -
				 ( UINT4 )( * file_pos % hf -> hf_bufend );
		hf -> hf_maxpos  = hf -> hf_readcnt ;
		hf -> hf_bufbase = ( * file_pos ) - 1;
	    }

	    DBG1 (( MF__,"e06_posbuffer   : hf -> hf_pos                 = %d\n",
				      hf -> hf_pos ));
	    DBG1 (( MF__,"e06_posbuffer   : hf -> hf_bufend              = %d\n",
				      hf -> hf_bufend ));
	    DBG1 (( MF__,"e06_posbuffer   : * file_pos                   = %d\n",
				      * file_pos ));
	    DBG1 (( MF__,"e06_posbuffer   : hf->hf_maxpos=hf->hf_readcnt = %d\n",
				      hf -> hf_maxpos ));
	    DBG1 (( MF__,"e06_posbuffer   : hf -> hf_bufbase             = %d\n",
				      hf -> hf_bufbase ));

	}

} /* e06_posbuffer*/

/*==========================================================================*/

static void clear_hf_buf (hf)


struct HOSTFILE_ALL                    *hf;

{ /* clear_hf_buf*/

	DBG1 (( MF__,"clear_hf_b: called \n" ));

	SAPDB_memset ( hf -> hf_bufptr , '\0' , hf -> hf_bufend );

	DBG1 (( MF__,"clear_hf_b: returning \n" ));

} /* clear_hf_buf*/

/*==========================================================================*/

static int      sql06_pread1  ( fd , packet , pktlgt , eof )
 
int                     fd ;
char                    * packet ;
int                     pktlgt ;
tsp00_Bool                 * eof ;
{
	int                     lgt , rc , sec_buf_lgt ;
	char                    sec_buf = FALSE ;
	char                    sec_buffer [ MAXBUFFERLENGTH ] ;

	DBG1 (( MF__,"sql06_pread : entered ===============\n" ))
	DBG1 (( MF__,"sql06_pread : * eof : %d\n",* eof ))

	for ( lgt = 0 ; lgt < pktlgt ; lgt += rc )
        {
	    if ( sec_buf )
	    {
		if ( ( pktlgt - lgt ) > MAXBUFFERLENGTH )
		    sec_buf_lgt = MAXBUFFERLENGTH ;
		else
		    sec_buf_lgt = pktlgt - lgt ;

		DBG1 (( MF__,"sql06_pread : sec_buf_lgt  %d\n" , sec_buf_lgt ))

		rc = read ( fd , sec_buffer , (unsigned) sec_buf_lgt ) ;
		if ( rc == 0 )
		{
		    DBG1 (( MF__,"sql06_pread : 1 EOF\n" ))
		    DBG1 (( MF__,"sql06_pread : 1 rc         %d \n" , rc ))
		    DBG1 (( MF__,"sql06_pread : 1 lgt        %d \n" , lgt ))
		    * eof = TRUE ;
		    return ( lgt ) ;
		}
		if ( rc < 0 )
		{
		    rc = errno ;
		    if ( rc != EINTR )
		    {
			MSGD (( -11555 ,  ERR_TYPE, "VIRTFILE",
			       "sql06_pread1: (1) returning with error: %s\n", sqlerrs() ))
			DBG1 (( MF__,"sql06_pread : 1 packet adr %d \n" , packet ))
			DBG1 (( MF__,"sql06_pread : 1 lgt        %d \n" , lgt ))
			DBG1 (( MF__,"sql06_pread : 1 pktlgt     %d \n" , pktlgt ))
			return ( - 1 ) ;
		    }
		    else
		    {
			DBG1 (( MF__,"sql06_pread : 1 interrupted \n" , rc ))
		    }
		    rc = 0 ;
		}
		DBG1 (( MF__,"sql06_pread : 1 read %d bytes \n" , rc ))
		DBG1 (( MF__,"sql06_pread : 1 p + l + rc %d \n" , packet + lgt + rc ))

		SAPDB_memcpy ( packet + lgt , sec_buffer , rc ) ;
		if ( rc > 0 && ( ! ( ( ( long ) packet + lgt + rc ) % 4 ) ) )
		    sec_buf = FALSE ;
	    }
	    else
	    {
		rc = read ( fd , packet + lgt , (unsigned) pktlgt - lgt ) ;
		if ( rc == 0 )
		{
		    DBG1 (( MF__,"sql06_pread : 2 EOF\n" ))
		    DBG1 (( MF__,"sql06_pread : 2 rc         %d \n" , rc ))
		    DBG1 (( MF__,"sql06_pread : 2 lgt        %d \n" , lgt ))
		    * eof = TRUE ;
		    return ( lgt ) ;
		}
		if ( rc < 0 )
		{
		    rc = errno ;
		    if ( rc != EINTR )
		    {
			MSGD (( -11555 , ERR_TYPE, "VIRTFILE",
			       "sql06_pread1: (2) returning with error: %s\n", sqlerrs() ))
			DBG1 (( MF__,"sql06_pread : 2 packet adr %d \n" , packet ))
			DBG1 (( MF__,"sql06_pread : 2 lgt        %d \n" , lgt ))
			DBG1 (( MF__,"sql06_pread : 2 pktlgt     %d \n" , pktlgt ))
			return ( - 1 ) ;
		    }
		    else
		    {
			DBG1 (( MF__,"sql06_pread : 2 interrupted \n" , rc ))
		    }
		    rc = 0 ;
		}
		DBG1 (( MF__,"sql06_pread : 2 read %d bytes \n" , rc ))
		DBG1 (( MF__,"sql06_pread : 2 p + l + rc %d \n" , packet + lgt + rc ))
		if ( rc > 0 && ( ( ( long ) packet + lgt + rc ) % 4 ) )
		    sec_buf = TRUE ;
	    }
        }
 
	DBG1 (( MF__,"sql06_pread: returning %d \n" , lgt ))
        return ( lgt ) ;
}

/*==========================================================================*/

int     sql06_fcntl ( hf , len )
struct HOSTFILE_ALL                    * hf ;
tsp00_Int4    len ;
{
    DBG1 (( MF__,"sql06_fcntl: called \n" )) ;
    DBG1 (( MF__,"sql06_fcntl : len                          = %d\n", len ))

    /* ***** if len <> 0 then check size ***** */
    /* ***** else no file locking        ***** */

    if ( len )
    {
	if ( len % MAXBUFFERLENGTH  )
	{
	    /* **** no file locking , block size <> n * 4 KB ***** */

	    DBG1 (( MF__,"sql06_fcntl : return 0 , len <> 0\n" ))
	    return ( 0 ) ;
	}
	else
	{
#if defined(_IBMR2) || defined(OSF1)
/* 1994-12-07 J.Bruehe : Inserted: AIX and OSF_1 restriction */
            if ( hf -> hf_file_type == FIFOFILE )
            {
	        /* ***** no FIFO locking on AIX and OSF_1 ***** */
	        DBG1 (( MF__,"sql06_fcntl : return 0 , FIFO not lockable \n" ))
	        return ( 0 ) ;
            }
            else
#endif
	    /* ***** file locking , block size = n * 4 KB ***** */
	    DBG1 (( MF__,"sql06_fcntl : return from fcntl\n" ))
	    return ( fcntl ( hf -> hf_fildes , F_SETLK , & file_lock ) == UNDEF ) ;
	}
    }
    else
    {
	/* ***** no file locking ***** */

	DBG1 (( MF__,"sql06_fcntl : return 0 , len = 0\n" ))
	return ( 0 ) ;
    }
}

/*==========================================================================*/
 
static  int     sql06c_set_signal ( )

{
   int             do_again ;

   /* ***** SIGPIPE muss abgefangen werden ***** */

   do_again = FALSE ;
   do
   {
      if ((long)( oldsigpipe  = signal ( SIGPIPE , sql06c_catch_sigpipe ))
								< 0 )
      {
	 if  (  errno == EINTR )
	    do_again = TRUE ;
	 else
	 {
	    /* ***** error ***** */
	    return ( -1 ) ;
	 }
      }
   }
   while ( do_again ) ;

   return ( 0 ) ;
}

 
/*==========================================================================*/
 
static  int    sql06c_reset_signal ( )

{
   int             do_again ;

   /* ***** alten Wert fuer SIGPIPE einsetzen ***** */

   do_again = FALSE ;
   do
   {
      if ( ( long ) ( signal ( SIGPIPE  , oldsigpipe ) ) < 0 )
	 if  (  errno == EINTR )
	    do_again = TRUE ;
	 else
	 {
	    /* ***** error ***** */
	    return ( - 1 ) ;
	 }
   }
   while ( do_again ) ;

   return ( 0 ) ;
}

/*==========================================================================*/
 
#if defined(OSF1) || ( defined(SVR4) && defined(I386) )
static SIGNALFUNCTYPE   sql06c_catch_sigpipe ( int unused )
#else
static SIGNALFUNCTYPE   sql06c_catch_sigpipe ()
#endif
{
   int             do_again ;

   do_again = FALSE ;
   do
   {
      if ( (long) signal ( SIGPIPE  , (void(*)(int))sql06c_catch_sigpipe ) < 0 )
      {
	 if  (  errno == EINTR )
	    do_again = TRUE ;
	 else
	 {
	    /* ***** error ***** */
	    return ;
	 }
      }
   }
   while ( do_again ) ;
}

#ifdef  DEVENV
/*==========================================================================*/

static  int     get_imake_path ( )

{
	int                     rc ;
	int                     lgt ;
	struct path_info        * pip ;
	char                    * ptr ;
	char                    path [ FILENAMESIZE ] ;


	DBG1 (( MF__,"getvdn: called \n" ))

	/*
	 *  1. Try VMAKE_PATH
	 */
	ptr = getenv ( "VMAKE_PATH" );
	for ( ; ptr && * ptr ; )
	{
	    for ( lgt = 0 ; lgt < sizeof(path) ; lgt ++ )
	    {
		if ( * ptr == PATH_DELIMITER || * ptr == '\0' ) break ;
		path [ lgt ] = * ptr ;
		ptr ++ ;
	    }
	    if ( lgt >= sizeof(path) )
	    {
		DBG1 (( MF__,"path name too long in VMAKE_PATH: '%s' \n", ptr ))
		return ( -2 );
	    }
	    path [ lgt ] = '\0' ;
	    if ( lgt == 0 )
	    {
		if ( * ptr != '\0' ) ptr ++ ; /* skip delimiter */
		continue ;
	    }
	    rc = append_path ( path , & vdn , & pip );
	    if ( rc < 0 )
	    {
		DBG1 (( MF__,"anaopt: append_path error %d \n", rc ))
		return ( rc );
	    }
	    else    DBG1 (( MF__,"anaopt: appended path '%s' \n", path ))
	}
	if ( vdn )
	{
	    for ( pip = vdn ; pip != NULL ; pip = pip->next )
		DBG1 (( MF__,"anaopt: vdn '%s' \n", pip->path ))
	    return ( 0 );
	}

	/*
	 *  2. Try  OWN , GRP , ALL , REL
	 */
	ptr = getenv ( "OWN" );
	if ( ptr != NULL && ptr [ 0 ] != '\0' )
	{
	    rc = append_path ( ptr , & vdn , & pip );
	    if ( rc < 0 )
	    {
		DBG1 (( MF__,"anaopt: append_path error %d \n", rc ))
		return ( -2 );
	    }
	    else    DBG1 (( MF__,"anaopt: appended path '%s' \n", ptr ))
	}
	ptr = getenv ( "GRP" );
	if ( ptr != NULL && ptr [ 0 ] != '\0' )
	{
	    rc = append_path ( ptr , & vdn , & pip );
	    if ( rc < 0 )
	    {
		DBG1 (( MF__,"anaopt: append_path error %d \n", rc ))
		return ( -2 );
	    }
	    else    DBG1 (( MF__,"anaopt: appended path '%s' \n", ptr ))
	}
	ptr = getenv ( "ALL" );
	if ( ptr != NULL && ptr [ 0 ] != '\0' )
	{
	    rc = append_path ( ptr , & vdn , & pip );
	    if ( rc < 0 )
	    {
		DBG1 (( MF__,"anaopt: append_path error %d \n", rc ))
		return ( -2 );
	    }
	    else    DBG1 (( MF__,"anaopt: appended path '%s' \n", ptr ))
	}
	ptr = getenv ( "REL" );
	if ( ptr != NULL && ptr [ 0 ] != '\0' )
	{
	    rc = append_path ( ptr , & vdn , & pip );
	    if ( rc < 0 )
	    {
		DBG1 (( MF__,"anaopt: append_path error %d \n", rc ))
		return ( -2 );
	    }
	    else    DBG1 (( MF__,"anaopt: appended path '%s' \n", ptr ))
	}
	if ( vdn )
	{
	    for ( pip = vdn ; pip != NULL ; pip = pip->next )
		DBG1 (( MF__,"anaopt: vdn '%s' \n", pip->path ))
	    return ( 0 );
	}

	/*
	 *  3. Try   VDN , VDN_PATH
	 */
	ptr = getenv ( "VDN" );
	if ( ptr != NULL && ptr [ 0 ] != '\0' )
	{
	    rc = append_path ( ptr , & vdn , & pip );
	    if ( rc < 0 )
	    {
		DBG1 (( MF__,"anaopt: append_path error %d \n", rc ))
		return ( -2 );
	    }
	    else    DBG1 (( MF__,"anaopt: appended path '%s' \n", ptr ))
	}
	ptr = getenv ( "VDN_PATH" );
	for ( ; ptr != NULL && * ptr != '\0' ; )
	{
	    for ( lgt = 0 ; lgt < sizeof(path) ; lgt ++ )
	    {
		if ( * ptr == PATH_DELIMITER || * ptr == '\0' ) break ;
		path [ lgt ] = * ptr ;
		ptr ++ ;
	    }
	    if ( lgt >= sizeof(path) )
	    {
		DBG1 (( MF__,"path name too long in VDN_PATH: '%s' \n", ptr ))
		return ( -2 );
	    }
	    path [ lgt ] = '\0' ;
	    if ( lgt == 0 )
	    {
		if ( * ptr != '\0' ) ptr ++ ; /* skip delimiter */
		continue ;
	    }
	    rc = append_path ( path , & vdn , & pip );
	    if ( rc < 0 )
	    {
		DBG1 (( MF__,"anaopt: append_path error %d \n", rc ))
		return ( rc );
	    }
	    else    DBG1 (( MF__,"anaopt: appended path '%s' \n", path ))
	}
	if ( vdn )
	{
	    for ( pip = vdn ; pip != NULL ; pip = pip->next )
		DBG1 (( MF__,"anaopt: vdn '%s' \n", pip->path ))
	    return ( 0 );
	}

	DBG1 (( MF__,"please specify VMAKE_PATH       or \n" ))
	DBG1 (( MF__,"               OWN,GRP,ALL,REL  or \n" ))
	DBG1 (( MF__,"               VDN and/or VDN_PATH \n" ))

	DBG1 (( MF__,"getvdn: returning -2 \n" ))
	return ( -2 );
}

/*==========================================================================*/

static  int     find_layer ( in_file , out_layer )

char                    * in_file ;
char                    * out_layer ;

{
	char                    * name ;


	DBG1 (( MF__,"find_layer      : in_file                        = %s\n",
				 in_file ))

	/*
	   Check whether the target is a module. The target is
	   assumed to be a module, if two conditions are met:
		A module's name needs to begin with three letters
			and two digits.
		There must be compilation information present for
			the module's layer. The module's layer is
			given by the second and third letter.
	*/

	if ( ! isalpha ( in_file [ 0 ] ) ||
	     ! isalpha ( in_file [ 1 ] ) ||
	     ! isalpha ( in_file [ 2 ] ) ||
	     ! isdigit ( in_file [ 3 ] ) ||
	     ! isdigit ( in_file [ 4 ] )    )
	{
		DBG1 (( MF__,"find_layer      : no valid module name, in_file  = %s\n",
					 in_file ))
		return ( 0 );
	}

	out_layer [ 0 ] = in_file [ 1 ] ;
	out_layer [ 1 ] = in_file [ 2 ] ;
	out_layer [ 2 ] = '\0' ;
	DBG1 (( MF__,"find_layer      : out_layer                      = %s\n",
				 out_layer ))
}

/*==========================================================================*/

static	int     append_path ( path , listpp , pipp )

char                    * path ;
struct path_info        ** listpp ;
struct path_info        ** pipp ;

{
	int                     rc ;
	int                     lgt ;
	struct path_info        ** spipp , * pip ;


	DBG1 (( MF__,"apdpth: path '%s' \n", path ))

	/* search end of list */
	for ( spipp = listpp ; (*spipp) != NULL ; spipp = & (*spipp)->next );

	pip = (struct path_info *) palloc ( sizeof(struct path_info) );
	if ( pip == NULL )
	{
	    DBG1 (( MF__,"insufficient dynamic memory \n" ))
	    return ( - 3 );
	}
	SAPDB_memset ( pip , '\0' , sizeof(struct path_info) );

	lgt = strlen ( path );

	/* search for a node separator */
	if ( strchr ( path , NODE_SEPARATOR ) )
	{
	    /* this is a remote path */
	    pip->path_status = PATH_REMOTE ;
	    /* remote objects denied on this path ? */
	    if ( (path [ lgt-1 ] == '/') && (path [ lgt-2 ] == '/') )
	    {
		pip->host_permission |= HOST_OBJECTS_DENIED ;
	    }
	    pip->host_permission |= HOST_TOOLS_DENIED ;
	}
	else
	{
	    /* this is a local path */
	    if ( spipp == listpp )
		    pip->path_status = PATH_OWN ;
	    else    pip->path_status = PATH_LOCAL ;
	    /* host_permission remains zero for local paths */
	}
	(void) strncpy ( pip->path , path , sizeof(pip->path)-1 );

	/* append to chain */
	* spipp = pip ;

	if ( pip->path_status == PATH_REMOTE )
	{
	    DBG1 (( MF__,"apdpth: appended remote path '%s' \n", pip->path ))
	}
	else
	{
	    DBG1 (( MF__,"apdpth: appended path '%s' \n", pip->path ))
	}
	rc = 1 ;

	* pipp = * spipp ;

	DBG1 (( MF__,"apdpth: returning %d \n", rc ))
	return ( rc );
}

/*==========================================================================*/


int     get_file ( name , dir , subdir1 , subdir2 , file_kind ,
		   file , date , is_remote )

char                    * name ;
char                    * dir ;
char                    * subdir1 ;
char                    * subdir2 ;
int                     file_kind ;
char                    * file ;
tsp00_Int4                * date ;
int			* is_remote ;

{
    int                     lgt , rc ;
    struct path_info        * pip ;
    struct stat             statbuf ;
    char                    path [ LINESIZE ] ;
		    /* LINESIZE: sprintf should not run out of space */


    DBG1 (( MF__,"get_file: name    '%s' \n", name ))
    DBG1 (( MF__,"get_file: dir     '%s' \n", dir ))
    DBG1 (( MF__,"get_file: subdir1 '%s' \n", subdir1 ))
    DBG1 (( MF__,"get_file: subdir2 '%s' \n", subdir2 ))

    * date = 0 ;
    * is_remote = 0 ;

    for ( pip = vdn ; pip != NULL ; pip = pip->next )
    {
	if ( ( ( file_kind == FK_OBJECT ) &&
	       ( pip->host_permission & HOST_OBJECTS_DENIED ) ) ||
	     ( ( file_kind == FK_TOOL ) &&
	       ( pip->host_permission & HOST_TOOLS_DENIED )   ) ||
	     (   file_kind == FK_CURR_SOURCE )                    )
	{
	    DBG1 (( MF__,"get_file: skipping remote dir '%s' \n", pip->path ))
	    continue ;
	}

	(void) sprintf ( path , dir , pip->path , subdir1 , subdir2 );
	lgt = strlen ( path );
	if ( lgt >= FILENAMESIZE )
	{
	    DBG1 (( MF__,"file name too long '%s' \n", path ))
	    return ( - 2 );
	}
	(void) strncpy  ( path + lgt , name , (CINT)(sizeof(path) - lgt) );
	if ( strlen ( path + lgt ) >= FILENAMESIZE - lgt )
	{
	    DBG1 (( MF__,"file name too long '%s' \n", path ))
	    return ( - 2 );
	}

	DBG1 (( MF__,"get_file: path    '%s' \n", path ))
	rc = filestat ( path , & statbuf );
	if ( rc < 0 )
	{
	    DBG1 (( MF__,"file not found    '%s'\n", path ))
	}
	else
	{
	    if ( * date && (* date < statbuf.st_mtime) &&
		 ( ((file_kind == FK_SOURCE) && vopt_source_warn) ||
		   ((file_kind == FK_OBJECT) && vopt_object_warn)    ) )
	    {
		/*FSt * /
		 *FSt * / DBG1 (( MF__,"WARNING: newer file on later tree \n" ))
		 *FSt * / DBG1 (( MF__,"%s %s\n", ascdate(date) , file ))
		 *FSt * / DBG1 (( MF__,"%s %s\n", ascdate(& statbuf.st_mtime) , path ))
		 *FSt **/
	    }

	    if ( ! * date ||
		 ( (statbuf.st_mtime > * date) &&
		   ( ((file_kind == FK_SOURCE) && vopt_source_copy) ||
		     ((file_kind == FK_OBJECT) && vopt_object_copy)    ) ) )
	    {
		(void) strcpy ( file , path );
		* date = statbuf.st_mtime ;
		* is_remote = ( pip->host_permission != 0 );

		/*FSt * / DBG1 (( MF__,"%s %s\n", ascdate(date) , file ))
		 *FSt **/

		if ( ( file_kind == FK_SOURCE &&
		      ! vopt_source_warn && ! vopt_source_copy ) ||
		     ( file_kind == FK_OBJECT &&
		      ! vopt_object_warn && ! vopt_object_copy )   )
		{
		    break ;
		}
	    }
	}
    }

    if ( * date )
    {
	DBG1 (( MF__,"get_file: returning %d \n", 1 ))
	return ( 1 );
    }

    /* in absence of a file create a new file name in home root */

    rc = new_file ( name , dir , subdir1 , subdir2 , file );
    DBG1 (( MF__,"new file          '%s' \n", file ))

    DBG1 (( MF__,"get_file: file not found, returning %d \n", rc ))
    return ( rc );
}


/*==========================================================================*/


int     new_file ( name , dir , subdir1 , subdir2 , file )

char                    * name ;
char                    * dir ;
char                    * subdir1 ;
char                    * subdir2 ;
char                    * file ;

{
    int                     lgt ;
    char                    path [ LINESIZE ] ;
		    /* LINESIZE: sprintf should not run out of space */


    DBG1 (( MF__,"new_file: name    '%s' \n", name ))
    DBG1 (( MF__,"new_file: dir     '%s' \n", dir ))
    DBG1 (( MF__,"new_file: subdir1 '%s' \n", subdir1 ))
    DBG1 (( MF__,"new_file: subdir2 '%s' \n", subdir2 ))

    if ( vdn == NULL )
    {
	DBG1 (( MF__,"vdn is NULL !!! \n" ))
	return ( - 2 );
    }
    (void) sprintf ( path , dir , vdn->path , subdir1 , subdir2 );
    lgt = strlen ( path );
    if ( lgt >= FILENAMESIZE )
    {
	DBG1 (( MF__,"file name too long '%s' \n", path ))
	return ( - 2 );
    }
    (void) strncpy  ( path + lgt , name , (CINT)(sizeof(path) - lgt) );
    lgt += strlen ( path + lgt );
    if ( lgt >= FILENAMESIZE )
    {
	DBG1 (( MF__,"file name too long '%.56s...' \n", path ))
	return ( - 2 );
    }
    SAPDB_memcpy ( file , path , lgt );
    file [ lgt ] = '\0' ;
    DBG1 (( MF__,"new file          '%s' \n", file ))

    DBG1 (( MF__,"new_file: returning %d \n" ))
    return ( 0 );
}


/*==========================================================================*/


char	** palloc ( 
unsigned		size )
{
	char			* mem_ptr , ** retval ;


	if ( ALLOC_MEM_EO57( (void **)&mem_ptr, size) != NO_ERROR_EO001 )
	{
	    DBG1 (( MF__,"FATAL: insufficient dynamic memory \n" ))
	    exit ( 3 );
	}

	if ( (long) mem_ptr % (long) sizeof(char **) )
	{
	    DBG1 (( MF__,"FATAL: malloc returned unaligned pointer 0x%08x \n",
								mem_ptr ))
	    exit ( 3 );
	}

#ifdef	lint
	retval = (char **) 0 ; /* suppress lint message:
				  'possible pointer alignment problem' */
#else
	retval = (char **) mem_ptr ;
#endif

	return ( retval );
}


/*==========================================================================*/


char    *  ascdate ( clockp )

tsp00_Int4                * clockp ;

{
	struct tm               tm_buffer ;
	struct tm               * tim ;
static  char                    line [ LINESIZE ] ;


	if ( * clockp == 0 )
	{
		SAPDB_memset  ( line , '\0' ,                    sizeof(line)   );
		(void) strncpy ( line , "not existent      ", sizeof(line)-1 );
		return ( line );
	}

	tim = localtime_r ( clockp, &tm_buffer );

	(void) sprintf ( line , "%02d.%02d.%02d %02d:%02d:%02d ",
		 tim->tm_year , tim->tm_mon+1 , tim->tm_mday ,
		 tim->tm_hour , tim->tm_min , tim->tm_sec );

	return ( line );
}


/*==========================================================================*/


int     filestat  ( name , statbuf )

char                    * name ;
struct stat             * statbuf ;

{
	int                     rc ;
	char                    * fname ;
#ifdef	SOCKETS
	int                     lgt ;
	struct imake_connection_info  * cip ;
	char                    * p , node [ NAMESIZE ] ;
#endif


	DBG1 (( MF__,"filestat: name '%s' \n", name ))

	fname = strchr ( name , NODE_SEPARATOR );
	if ( fname == NULL )
	{
		rc = stat ( name , statbuf );
		return ( rc );
	}

#ifndef SOCKETS
	DBG1 (( MF__,"filestat: remote access not implemented '%s' \n", name ))
	errno = EINVAL ;
	return ( -1 );
#else
	DBG1 (( MF__,"filestat: remote access \n" ))

	SAPDB_memset ( node , '\0' , sizeof(node) );
	lgt = fname - name ;
	if ( lgt >= sizeof(node) )
	{
		DBG1 (( MF__,"filestat: node name too long: '%s' \n", name ))
		errno = EINVAL ;
		return ( -1 );
	}
	SAPDB_memcpy ( node , name , lgt );
	DBG1 (( MF__,"filestat: node  '%s' \n", node  ))

	fname ++ ;
	DBG1 (( MF__,"filestat: fname '%s' \n", fname ))

	/* find existing or open new connection to server */

	rc = open_connection ( node , & cip );
	if ( rc )
	{
		DBG1 (( MF__,"filestat: cannot access '%s' \n", name ))
		errno = rc ;
		return ( -1 );
	}
	DBG1 (( MF__,"filestat: got connection \n" ))

	/* issue stat request */

	lgt = strlen ( fname ) + 1 ; /* include null character ('\0') */
	if ( lgt > MESSAGESIZE )
	{
		DBG1 (( MF__,"filestat: file name too long: '%s' \n", fname ))
		errno = EINVAL ;
		return ( -1 );
	}
	DBG1 (( MF__,"filestat: fname length %d \n", lgt ))

	SAPDB_memcpy (  remote_msg.data , fname , lgt );

	remote_msg.type         = REM_STAT ;
	remote_msg.reference    = 0 ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG1 (( MF__,"filestat: request length mismatch %d \n", rc ))
		errno = rc < 0 ? -rc : EIO ;
		return ( -1 );
	}
	DBG1 (( MF__,"filestat: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt - HEADER_SIZE != 30 )
	{
		if ( lgt - HEADER_SIZE == 2 )
		{
			/* got errno */
			p = (char*) remote_msg.data ;
			errno = p [  0 ]       & 0x00FF |
				p [  1 ] <<  8 & 0xFF00 ;

		}
		else
		{
			DBG1 (( MF__,"filestat: illegal reply size %d \n", lgt ))
			errno = EIO ;
		}
		return ( -1 );
	}
	DBG1 (( MF__,"filestat: got reply from server (%d bytes) \n", lgt ))

	p = (char*) remote_msg.data ;
	statbuf->st_mode  =   p [  0 ]       & 0x00FF |
				p [  1 ] <<  8 & 0xFF00 ;
	statbuf->st_ino   =   p [  2 ]       & 0x00FF |
				p [  3 ] <<  8 & 0xFF00 ;
	statbuf->st_dev   =   p [  4 ]       & 0x00FF |
				p [  5 ] <<  8 & 0xFF00 ;
	statbuf->st_rdev  =   p [  6 ]       & 0x00FF |
				p [  7 ] <<  8 & 0xFF00 ;
	statbuf->st_nlink =   p [  8 ]       & 0x00FF |
				p [  9 ] <<  8 & 0xFF00 ;
	statbuf->st_uid   =   p [ 10 ]       & 0x00FF |
				p [ 11 ] <<  8 & 0xFF00 ;
	statbuf->st_gid   =   p [ 12 ]       & 0x00FF |
				p [ 13 ] <<  8 & 0xFF00 ;
	statbuf->st_size  =   p [ 14 ]       & 0x000000FF |
				p [ 15 ] <<  8 & 0x0000FF00 |
				p [ 16 ] << 16 & 0x00FF0000 |
				p [ 17 ] << 24 & 0xFF000000 ;
	statbuf->st_atime =   p [ 18 ]       & 0x000000FF |
				p [ 19 ] <<  8 & 0x0000FF00 |
				p [ 20 ] << 16 & 0x00FF0000 |
				p [ 21 ] << 24 & 0xFF000000 ;
	statbuf->st_mtime =   p [ 22 ]       & 0x000000FF |
				p [ 23 ] <<  8 & 0x0000FF00 |
				p [ 24 ] << 16 & 0x00FF0000 |
				p [ 25 ] << 24 & 0xFF000000 ;
	statbuf->st_ctime =   p [ 26 ]       & 0x000000FF |
				p [ 27 ] <<  8 & 0x0000FF00 |
				p [ 28 ] << 16 & 0x00FF0000 |
				p [ 29 ] << 24 & 0xFF000000 ;

	DBG1 (( MF__,"filestat: stated '%s' \n", name ))
	return ( 0 );
#endif
}


/*==========================================================================*/


VFILE   * fileopen ( name , type )

char                    * name ;
char                    * type ;

{
	VFILE                   * vfp ;
	FILE                    * fp ;
	char                    * fname ;
#ifdef	SOCKETS
	int                     fno , lgt , typlgt , rc ;
	RFILE			* rfp ;
	struct imake_connection_info  * cip ;
	struct file_cache       * fcp ;
	char                    node [ NAMESIZE ] ;
#endif


	DBG1 (( MF__,"fileopen: name '%s' \n", name ))
	DBG1 (( MF__,"fileopen: type '%s' \n", type ))

	fname = strchr ( name , NODE_SEPARATOR );
	if ( fname == NULL )
	{
		fp = fopen ( name , type );
		if ( fp == NULL ) return ( NULL );
		vfp = (VFILE*) palloc ( sizeof(VFILE) );
		vfp->type = FT_LOCAL ;
		vfp->v_fp.l_fp = fp ;
		return ( vfp );
	}

#ifndef SOCKETS
	DBG1 (( MF__,"ERROR: remote access not implemented '%s' \n", name ))
	return ( NULL );
#else
	DBG1 (( MF__,"fileopen: remote access \n" ))

	/* find empty slot in remote files list */

	for ( fno = 0 ; fno < MAXREMOTEFILES ; fno ++ )
	{
		if ( remote_file [ fno ].in_use == 0 ) break ;
	}
	if ( fno >= MAXREMOTEFILES )
	{
		DBG1 (( MF__,"fileopen: no more remote files. in use are %d \n", fno ))
		return ( NULL );
	}
	DBG1 (( MF__,"fileopen: found free file no. %d \n", fno ))

	rfp = & remote_file [ fno ] ;

	SAPDB_memset ( node , '\0' , sizeof(node) );
	lgt = fname - name ;
	if ( lgt >= sizeof(node) )
	{
		DBG1 (( MF__,"fileopen: node name too long: '%s' \n", name ))
		return ( NULL );
	}
	SAPDB_memcpy ( node , name , lgt );
	DBG1 (( MF__,"fileopen: node  '%s' \n", node  ))

	fname ++ ;
	DBG1 (( MF__,"fileopen: fname '%s' \n", fname ))

	/* find existing or open new connection to server */

	rc = open_connection ( node , & cip );
	if ( rc )
	{
		DBG1 (( MF__,"fileopen: cannot access '%s' \n", name ))
		return ( NULL );
	}
	DBG1 (( MF__,"fileopen: got connection \n" ))

	/* allocate file cache */

	fcp = (struct file_cache *) palloc ( sizeof(struct file_cache) );
	if ( fcp == NULL )
	{
		DBG1 (( MF__,"insufficient dynamic memory \n" ))
		return ( NULL );
	}
	SAPDB_memset ( fcp , '\0' , sizeof(struct file_cache) );
	DBG1 (( MF__,"fileopen: got file cache \n" ))

	/* issue open request */

	lgt    = strlen ( fname ) + 1 ; /* include null character ('\0') */
	typlgt = strlen ( type  ) + 1 ; /* include null character ('\0') */
	if ( lgt + typlgt > MESSAGESIZE )
	{
		DBG1 (( MF__,"fileopen: file name too long: '%s' \n", fname ))
		FREE_MEM_EO57( fcp );
		return ( NULL );
	}
	DBG1 (( MF__,"fileopen: fname length %d \n", lgt    ))
	DBG1 (( MF__,"fileopen: type  length %d \n", typlgt ))

	SAPDB_memcpy (  remote_msg.data       , fname , lgt    );
	SAPDB_memcpy (  remote_msg.data + lgt , type  , typlgt );
	lgt += typlgt ;

	remote_msg.type         = REM_OPEN ;
	remote_msg.reference    = fno ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG1 (( MF__,"fileopen: request length mismatch %d \n", rc ))
		FREE_MEM_EO57( fcp );
		return ( NULL );
	}
	DBG1 (( MF__,"fileopen: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE )
	{
		DBG1 (( MF__,"fileopen: illegal reply size %d \n", lgt ))
		FREE_MEM_EO57( fcp );
		return ( NULL );
	}
	DBG1 (( MF__,"fileopen: got reply from server (%d bytes) \n", lgt ))

	if ( remote_msg.reference != fno )
	{
		DBG1 (( MF__,"fileopen: can't open '%s' \n", name ))
		FREE_MEM_EO57( fcp );
		return ( NULL );
	}

	lgt -= HEADER_SIZE ;
	SAPDB_memcpy (  fcp->data ,  remote_msg.data , lgt );
	fcp->remain = lgt ;
	fcp->current = 0 ;
	if ( remote_msg.type == REM_CLOSE ) fcp->eof = 1 ;

	rfp->fcp    = fcp ;
	rfp->cip    = cip ;
	rfp->ref    = fno ;
	rfp->in_use = 1 ;

	vfp = (VFILE*) palloc ( sizeof(VFILE) );
	vfp->type = FT_REMOTE ;
	vfp->v_fp.r_fp = rfp ;

	DBG1 (( MF__,"fileopen: opened '%s' \n", name ))
	return ( vfp );
#endif
}


/*==========================================================================*/


int     fileclose ( vfp )

VFILE                   * vfp ;

{
	int                     rc ;
	FILE			* fp ;
#ifdef	SOCKETS
	int                     lgt ;
	RFILE			* rfp ;
	struct imake_connection_info  * cip ;
	struct file_cache       * fcp ;
#endif


	DBG1 (( MF__,"fileclose: called \n" ))

	if ( vfp->type == FT_LOCAL )
	{
		fp = vfp->v_fp.l_fp ;
		rc = fclose ( fp );
		if ( ! rc ) FREE_MEM_EO57( vfp );
		return ( rc );
	}

#ifndef SOCKETS
	DBG1 (( MF__,"ERROR: remote access not implemented \n" ))
	return ( EOF );
#else
	DBG1 (( MF__,"fileclose: remote access \n" ))

	rfp = vfp->v_fp.r_fp ;

	if ( ! rfp->in_use )
	{
		DBG1 (( MF__,"fileclose: file not open \n" ))
		return ( EOF );
	}
	fcp = rfp->fcp ;

	if ( fcp->eof )
	{
		/* already closed by server */
		FREE_MEM_EO57( fcp );
		SAPDB_memset (  rfp , '\0' , sizeof(RFILE) );
		return ( 0 );
	}

	cip = rfp->cip ;

	lgt = 0 ;
	remote_msg.type         = REM_CLOSE ;
	remote_msg.reference    = rfp->ref ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE )
	{
		DBG1 (( MF__,"fileclose: request length mismatch %d \n", rc ))
		FREE_MEM_EO57( fcp );
		SAPDB_memset (  rfp , '\0' , sizeof(RFILE) );
		return ( EOF );
	}
	DBG1 (( MF__,"fileclose: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt != HEADER_SIZE )
	{
		DBG1 (( MF__,"fileclose: illegal reply size %d \n", lgt ))
		FREE_MEM_EO57( fcp );
		SAPDB_memset (  rfp , '\0' , sizeof(RFILE) );
		return ( EOF );
	}
	DBG1 (( MF__,"fileclose: got reply from server (%d bytes) \n", lgt ))

	FREE_MEM_EO57( vfp );
	FREE_MEM_EO57( fcp );
	SAPDB_memset (  rfp , '\0' , sizeof(RFILE) );

	DBG1 (( MF__,"fileclose: returning \n" ))
	return ( 0 );
#endif
}


/*==========================================================================*/


int     fileread  ( buf , siz , cnt , vfp )

char                    * buf ;
int                     siz ;
int                     cnt ;
VFILE                   * vfp ;

{
    int                     rc ;
    FILE                    * fp ;
#ifdef	SOCKETS
    int                     bufsiz , lgt, total;
    char                    * bufptr ;
    RFILE                   * rfp ;
    struct imake_connection_info  * cip ;
    struct file_cache       * fcp ;
#endif


    DBG1 (( MF__,"fileread: called \n" ))

    if ( vfp->type == FT_LOCAL )
    {
	fp = vfp->v_fp.l_fp ;
	rc = fread  ( (CPTR*) buf , (CINT) siz , (CINT) cnt , fp );
	return ( rc );
    }

#ifndef SOCKETS
    DBG1 (( MF__,"ERROR: remote access not implemented \n" ))
    return ( 0 );
#else
    DBG1 (( MF__,"fileread: remote access \n" ))

    rfp = vfp->v_fp.r_fp ;

    if ( ! rfp->in_use )
    {
	DBG1 (( MF__,"fileread: file not open \n" ))
	return ( 0 );
    }
    fcp = rfp->fcp ;
    cip = rfp->cip ;


    bufptr = buf ;
    total  = siz * cnt ;
    bufsiz = total ;

    while ( bufsiz > 0 )
    {
	lgt = bufsiz ;
	if ( lgt > fcp->remain ) lgt = fcp->remain ;
	SAPDB_memcpy ( bufptr , fcp->data + fcp->current , lgt );
	bufptr       += lgt ;
	bufsiz       -= lgt ;
	fcp->remain  -= lgt ;
	fcp->current += lgt ;
	DBG1 (( MF__,"got %d bytes from cache \n", lgt ))
	if ( bufsiz <= 0 ) break ;

	if ( fcp->eof )
	{
	    lgt = ( total - bufsiz ) / siz ;
	    DBG1 (( MF__,"fileread: returning %d \n", lgt ))
	    return ( lgt );
	}

	lgt = 0 ;
	remote_msg.type         = REM_READ ;
	remote_msg.reference    = rfp->ref ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
	    DBG1 (( MF__,"fileread: request size mismatch %d \n", rc ))
	    return ( 0 );
	}
	DBG1 (( MF__,"fileread: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE )
	{
	    DBG1 (( MF__,"fileread: illegal reply size %d \n", lgt ))
	    return ( 0 );
	}
	DBG1 (( MF__,"fileread: got reply from server (%d bytes) \n", lgt ))

	if ( remote_msg.type == REM_CLOSE ) fcp->eof = 1 ;

	lgt -= HEADER_SIZE ;
	SAPDB_memcpy ( fcp->data , remote_msg.data , lgt );
	fcp->remain = lgt ;
	fcp->current = 0 ;
    }

    DBG1 (( MF__,"fileread: returning %d \n", cnt ))
    return ( cnt );
#endif
}


/*==========================================================================*/


#ifdef  SOCKETS
int     open_connection ( node , cipp )

char                    * node ;
struct imake_connection_info  ** cipp ;

{
	int                             lgt , sd , rc ;
	tsp00_Int4                        my_time , srv_time , diff ;
	char                            * p ;
	struct hostent                  * hp ;
	struct servent                  * sp ;
	struct sockaddr                 sa ;
	short                           port ;
	struct imake_connection_info    * cip ;


	for ( cip = connections ; cip != NULL ; cip = cip->next )
	{
		if ( strcmp ( cip->node , node ) == 0 ) break ;
	}
	if ( cip != NULL )
	{
		DBG1 (( MF__,"node already connected '%s' \n", node ))
		* cipp = cip ;
		return ( 0 );
	}

	cip = (struct imake_connection_info *)
		palloc ( sizeof(struct imake_connection_info) );
	if ( cip == NULL )
	{
		DBG1 (( MF__,"isufficient dynamic memory \n" ))
		return ( ENOMEM );
	}
	SAPDB_memset (  cip , '\0' , sizeof(struct imake_connection_info) );

	lgt = strlen ( node );
	if ( lgt >= sizeof(cip->node) )
	{
		DBG1 (( MF__,"node name too long: '%s' \n", node ))
		FREE_MEM_EO57( cip );
		return ( EINVAL );
	}
	SAPDB_memcpy ( cip->node , node , lgt );

	errno = 0 ;
	sp = getservbyname ( SOCKET_SERVICE, SOCKET_PROTOCOL );
	if ( sp == NULL )
	{
		/* try a second time */
		errno = 0 ;
		sp = getservbyname ( SOCKET_SERVICE, SOCKET_PROTOCOL );
		if ( sp == NULL )
		{
			DBG1 (( MF__,"get service by name error, %s\n", sqlerrs() ))
			FREE_MEM_EO57( cip );
			return ( errno ? errno : EINVAL );
		}
	}
	DBG1 (( MF__,"got service by name \n" ))

	errno = 0 ;
	hp = gethostbyname ( node );
	if ( hp == NULL )
	{
		/* try a second time */
		errno = 0 ;
		hp = gethostbyname ( node );
		if ( hp == NULL )
		{
			DBG1 (( MF__,"get host by name error, %s\n", sqlerrs() ))
			FREE_MEM_EO57( cip );
			return ( errno ? errno : EINVAL );
		}
	}
	DBG1 (( MF__,"got host by name \n" ))

	sd = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( sd < 0 )
	{
		DBG1 (( MF__,"socket open error, %s\n", sqlerrs() ))
		FREE_MEM_EO57( cip );
		return ( errno );
	}
	cip->sd = sd ;
	DBG1 (( MF__,"opened socket %d \n", sd ))

	sa.sa_family = hp->h_addrtype;
	p = (char*)sa.sa_data;
	SAPDB_memset ( p, '\0', sizeof(sa.sa_data) );
	port = sp->s_port;
	SAPDB_memcpy ( p,  &port, 2 );
	p += 2;
	lgt = hp->h_length;
	SAPDB_memcpy ( p, hp->h_addr, lgt );
	p += lgt;

	rc = connect ( sd, &sa, sizeof(sa) );
	if ( rc )
	{
		if ( errno == EADDRNOTAVAIL )
		{
			DBG1 (( MF__,"server not reachable \n" ))
		}
		else    DBG1 (( MF__,"connect error, %s\n", sqlerrs() ))
		(void) close ( sd );
		FREE_MEM_EO57( cip );
		return ( errno );
	}
	DBG1 (( MF__,"connected to '%s' \n", node ))

	remote_msg.type         = REM_TIME ;
	remote_msg.reference    = 0 ;
	remote_msg.length [ 0 ] = 0 ;         /* low byte */
	remote_msg.length [ 1 ] = 0 ;         /* high byte */

	rc = put_message ( cip );
	if ( rc < HEADER_SIZE )
	{
		(void) close ( sd );
		FREE_MEM_EO57( cip );
		return ( rc < 0 ? -rc : EIO );
	}
	DBG1 (( MF__,"sent time request  to  server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt <= 0 )
	{
		(void) close ( sd );
		FREE_MEM_EO57( cip );
		return ( lgt < 0 ? -lgt : EIO );
	}
	DBG1 (( MF__,"got  time message from server (%d bytes) \n", rc ))

	my_time = time ( (time_t *) 0 );

	if ( lgt - HEADER_SIZE < 4 )
	{
		DBG1 (( MF__,"illegal time reply size %d \n", lgt ))
		(void) close ( sd );
		FREE_MEM_EO57( cip );
		return ( EIO );
	}
	p = (char*) remote_msg.data ;
	srv_time = p [ 0 ]       & 0xFF     | p [ 1 ] <<  8 & 0xFF00     |
		   p [ 2 ] << 16 & 0xFF0000 | p [ 3 ] << 24 & 0xFF000000 ;

	diff = my_time - srv_time ;
	if ( diff < 0 ) diff = -diff ;
	if ( diff <= HYSTERESIS )
	{
	    DBG1 (( MF__,"server time is %s \n", ascdate ( & srv_time ) ))
	    DBG1 (( MF__,"my     time is %s \n", ascdate ( & my_time  ) ))
	}
	else
	{
	    DBG1 (( MF__,"server time is %s \n", ascdate ( & srv_time ) ))
	    DBG1 (( MF__,"my     time is %s \n", ascdate ( & my_time  ) ))
	    DBG1 (( MF__,"ERROR: won't use server because of time difference \n" ))
	    (void) close ( sd );
	    FREE_MEM_EO57( cip );
	    return ( EIO );
	}

	cip->next = connections ;
	connections = cip ;
	* cipp = cip ;

	return ( 0 );
}


/*==========================================================================*/


int     get_message ( cip )

struct imake_connection_info  * cip ;

{
	int                     sd , msglgt , lgt , rc ;


	sd = cip->sd ;
	DBG1 (( MF__,"getmsg: called %d \n", sd ))

	lgt = cip->cache_lgt ;
	if ( lgt )
	{
		SAPDB_memcpy (  & remote_msg , cip->cache , lgt );
		cip->cache_lgt = 0 ;
	}

	for ( ; lgt < HEADER_SIZE ; lgt += rc )
	{
		rc = read ( sd , (char*) & remote_msg  + lgt ,
				     sizeof(remote_msg) - lgt );
		if ( rc <= 0 )
		{
			if ( rc < 0 )
			{
				DBG1 (( MF__,"message read error, %s\n", sqlerrs() ))
				remove_connection ( sd );
				return ( errno > 0 ? -errno : -EIO );
			}
			DBG1 (( MF__,"connection closed by server \n" ))
			remove_connection ( sd );
			return ( 0 );
		}
		DBG1 (( MF__,"getmsg: got %d bytes \n", rc ))
	}
	DBG1 (( MF__,"getmsg: header complete (got %d bytes) \n", lgt ))

	msglgt = HEADER_SIZE +
		 (remote_msg.length [ 0 ]      & 0x00FF |
		  remote_msg.length [ 1 ] << 8 & 0xFF00   );
	if ( msglgt < HEADER_SIZE || msglgt > sizeof(remote_msg) )
	{
		DBG1 (( MF__,"illegal message length %d \n", msglgt ))
		return ( -EIO );
	}
	DBG1 (( MF__,"getmsg: complete message length is %d bytes \n", msglgt ))

	for ( ; lgt < msglgt ; lgt += rc )
	{
		rc = read ( sd , (char*) & remote_msg  + lgt ,
				     sizeof(remote_msg) - lgt );
		if ( rc <= 0 )
		{
			if ( rc < 0 )
			{
				DBG1 (( MF__,"message read error, %s\n", sqlerrs() ))
				remove_connection ( sd );
				return ( errno > 0 ? -errno : -EIO );
			}
			DBG1 (( MF__,"connection closed by server \n" ))
			remove_connection ( sd );
			return ( 0 );
		}
		DBG1 (( MF__,"getmsg: got %d bytes \n", rc ))
	}
	DBG1 (( MF__,"getmsg: message complete (got %d bytes) \n", lgt ))

	if ( lgt > msglgt )
	{
		cip->cache_lgt = lgt - msglgt ;
		SAPDB_memcpy ( cip->cache ,  & remote_msg + msglgt ,
							cip->cache_lgt );
		lgt -= cip->cache_lgt ;
	}

	DBG1 (( MF__,"getmsg: returning %d \n", lgt ))
	return ( lgt );
}


/*==========================================================================*/


int     put_message ( cip )

struct imake_connection_info  * cip ;

{
	int                     sd , msglgt , rc ;


	sd = cip->sd ;
	DBG1 (( MF__,"putmsg: called %d \n", sd ))

	msglgt = HEADER_SIZE +
		 (remote_msg.length [ 0 ]      & 0x00FF |
		  remote_msg.length [ 1 ] << 8 & 0xFF00   );
	if ( msglgt < HEADER_SIZE || msglgt > sizeof(remote_msg) )
	{
		DBG1 (( MF__,"illegal message length %d \n", msglgt ))
		return ( -EIO );
	}
	DBG1 (( MF__,"putmsg: complete message length is %d bytes \n", msglgt ))

	rc = write ( sd , (char*) & remote_msg , (unsigned) msglgt );
	if ( rc != msglgt )
	{
		if ( rc < 0 )
		{
			rc = errno ;
			DBG1 (( MF__,"socket write error, %s\n", sqlerrs() ))
		}
		else
		{
			DBG1 (( MF__,"socket write size error %d \n", rc ))
		}
		remove_connection ( sd );
		return ( rc < 0 ? rc : -EIO );
	}

	DBG1 (( MF__,"putmsg: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


remove_connection ( sd )

{
	struct imake_connection_info  * cip , ** cipp ;


	for (   cipp = & connections ;
		(* cipp) != NULL ;
		cipp = & (* cipp)->next )
	{
		if ( (* cipp)->sd == sd ) break ;
	}
	cip = * cipp ;
	if ( ! cip ) return ;
	(void) close ( cip->sd );
	(* cipp) = cip->next ;
	FREE_MEM_EO57( cip );
}
#endif
#endif
