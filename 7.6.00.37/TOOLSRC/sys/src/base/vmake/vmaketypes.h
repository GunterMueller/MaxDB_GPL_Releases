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
#ifndef VMAKE_VTYPES_H
#define VMAKE_VTYPES_H

#include <stdio.h>

/* sizes*/
#define         FILENAMESIZE            2048 /* old 256 PTS 1105037 */
#define         MESSAGESIZE             (4096 - 128) /* allow for headers */
#ifndef MAXFNMLGT   /* can be overridden by -DMAXFNMLGT=<number> */
#define         MAXFNMLGT               256
#endif
#if MAXFNMLGT & 1   /* odd ? */
#define         NAMESIZE                (MAXFNMLGT+1)
#else
#define         NAMESIZE                (MAXFNMLGT+2)
#endif
#if NAMESIZE > FILENAMESIZE
#undef      FILENAMESIZE
#define     FILENAMESIZE        NAMESIZE
#endif


/* types */

/* file system directory information */

struct dir_info
{
    struct dir_info     * right ;
    struct dir_info     * left  ;
    char                * dirpath ;
    struct dir_entry    * entries ;
	unsigned char       exists;
} ;

/* file system directory entry */

struct dir_entry
{
    struct dir_entry    * right ;
    struct dir_entry    * left  ;
    char                * name ;
} ;

/* layer compile information */

struct com_info
{
    struct com_info     * next ;
    char                * layer ;
    time_t				  date;           /* CTS 1111841 */
    struct mod_com_info * com_fast ;
    struct mod_com_info * com_quick ;
    struct mod_com_info * com_slow ;
} ;

/* list object for variants */
struct variant_info
{
	char				* name;
	struct variant_info	* next;
};

/* target status info */

struct trgstat_info
{
    unsigned int    level_i     : 2;
    unsigned int    level_s     : 2;
    unsigned int    level_p     : 2;
    unsigned int    level_c     : 2;
    unsigned int    level_o     : 2;
    unsigned int    shrglob     : 2;
    unsigned int    debug       : 1;
    unsigned int    profile     : 1;
    unsigned int    uncond      : 1;
    unsigned int    binary      : 1;
	unsigned int    ascii       : 1; /* CTS 1105829 */
	unsigned int    text        : 1; 
	unsigned int    realname    : 1; 
	unsigned int    checkdate   : 1;
    unsigned int    definition  : 1;
    unsigned int    noobject    : 1;
    unsigned int    noobjcopy   : 1;
    unsigned int    deptype     : 3;
    unsigned int    forcemake   : 1;
	unsigned int    distribute  : 1;
	unsigned int    nodebug     : 1; /* CTS 1102466 */
	unsigned int    exec        : 1; /* CTS 1110662 */
	unsigned int    ignore_err  : 1; /* CTS 1112286 */
	unsigned int    noextension : 1; /* CTS 1112700 */
	unsigned int    making      : 1; /* CTS 1112727 */
	unsigned int    nostlib     : 1;
	unsigned int    no_dep_remake : 1;
	unsigned int    deperror    : 1 ;
	unsigned int    on_success  : 1 ;
} ;


struct file_info
{
    struct file_info    * next;
    char                * file;
	char                nodistrib;   /* CTS 1110367 */
};


/* module compile information */

struct mod_com_info
{
    struct mod_com_info * next ;
    char                * name ;
/*	char                * file ; */
	struct file_info    * file;
	char                * variantfile ;
	char                * purename ;
	char                * name_without_global_variants ;

	char                * local_source;
	/*
	struct file_info    file ;
	*/
    char                special_target_flag ;
    char                * defaultlayer ;
    char                * objdemanddir ;
    char                * srcdemanddir ;
    char                version ;
    char                versdef ;
    char                remake ;
	char                nobind;         /* CTS 1105891 */
	char                nodistrib;      /* CTS 1110367 */
	char                language;       /* CTS 1112700 */
	int                 type;           /* CTS TYPE */
    char                nocomfile;
	char                * comfilename;
	int                 novariant;
	int                 on_success;
	int                 suppress_uncond;
	int                 comtype ;  /* CTS NEWVARIANT : 0 = no variant , 1 = old variant, 2 = new variant */
	char                * compiler;
	char                * localvariant;
	struct variant_info * allvariants;
	struct variant_info * novariants;
	regex_t             * comp_pattern ;
    struct trgstat_info stat ;
    struct dem_mod_info * objdemands ;
    struct dem_mod_info * srcdemands ;
    struct inc_info     * dependencies ;
    struct extdep_info  * ext_dependencies ;
    struct dsc_info     * descriptions ;
    struct inc_info     * includes ;
    struct obj_info     * objects ;
    struct env_info     * propagates ;
	struct opt_info     * parentlinkoptions;
	int                 protstyle; 
} ;

struct mod_com_info_list
{
	struct mod_com_info_list * next; 
	struct mod_com_info      * mcip;
	int                      mci_type;
} ;

/* library member information */

struct mem_info
{
    struct mem_info     * next ;
    char                * name ;
    time_t              date ;
} ;

/* external dependency information */

struct extdep_info
{
    struct extdep_info  * next ;
    char                * file ;
    time_t              date ;
} ;

/* object member information */

struct obj_info
{
    struct obj_info     * next ;
    char                * name_o ;
    char                * file ;
} ;


struct env_info
{
    struct env_info     * next;
	char                * variable ;
    char                * value;
};


/* target information */

struct trg_info
{
    struct trg_info     * next ;
    char                * name ;
    int                 type ;
    char                version ;
    char                language ;
    char                * layer ;
	char				* own_layer ; /* PTS 1102210 */
	char                * local_source;
    int                 made ;
    int                 minpathlvl ;
    char                fixed_file ;
    struct file_info    file ;
	struct file_info    *variantfile ;
    char                * afile ;
	char                * compiler;
    int                 status ;
	int                 report ;
    time_t              date ;
    struct trgstat_info stat ;
    struct dsc_info     * descriptions ;
    struct dem_info     * objdemands ;
    struct dem_info     * srcdemands ;
    struct dep_info     * dependencies ;
    struct extdep_info  * ext_dependencies ;
    struct dep_info     * callers ;
    struct mem_info     * members ;
    struct obj_info     * objects ;
    struct dep_info     * remakes ;
	struct dep_info     * nobinds ;	 /* CTS 1105891 */
	struct dep_info     * on_success ;
    struct dep_info     * options ;
    struct env_info     * propagates ;
    /* PTS 1103486 */
	struct trg_info     * act_caller; /* active_caller */
	/* PTS 1103633 */
	int                 max_depmade;
	/* PTS 1104596 */
	struct trg_info     * other_dep_tip;

	struct dep_info     * rec_callers;

	char                * localvariant;
	struct variant_info * globalvariants;
	struct opt_info     * parentlinkoptions;
	struct opt_info     * linkoptions;
	char                * responsible; 
	int					finished;
	int					counted;
	int                 protstyle;
	char                * description_line;
} ;

/* optional target description information */

struct dsc_info
{
    struct dsc_info     * next ;
    char                * descriptor ;
    char                * value ;
} ;

/* dependency information */

struct dep_info
{
    struct dep_info     * next;
    struct trg_info     * target;
	struct opt_info     * options;
} ;

struct dem_info
{
    struct dem_info     * next ;
    char                * directory ;
    struct dep_info     * list ;
} ;


struct opt_info
{
    struct opt_info     * next ;
    char                * option ;
} ;


/* info for distrib lists */
struct dist_info      /* &gar CTS 1000237 */
{
	struct dist_info     * next ;
    char                 * name ;
};

/* path information is used for VMAKE_PATH */

struct path_info
{
    struct path_info    * next ;
    int                 host_permission ;
/*  path_status: number of path and status bit PATH_OWN, PATH_LOCAL, PATH_REMOTE*/
    int                 path_status ;
    char                * path ;
} ;

/* include information (00 respectively g.... modules ) */

struct inc_info
{
    struct inc_info     * next ;
    char                * name ;
    char                language ;
    char                disposition ;
} ;

struct dem_mod_info
{
    struct dem_mod_info * next ;
    char                * name ;
    char                language ;
    char                * directory ;
    char                disposition ;
} ;

/* extra files information */

struct extra_info
{
    struct extra_info   * next ;
    char                * name ;
	char                * layer ;
    char                * dir;
    char                nodot ;
    char                binary ;
	char                ascii ; /* CTS 1105829 */
	char                text ; 
	char                exec; /* CTS 1110662 */
    struct dsc_info     * descriptions ;
    struct inc_info     * includes ;
} ;

/* target list information */

struct list_info
{
    struct list_info    * next ;
    char				line [ 2 ]; /* allocated with required size */
} ;

/* language information */

struct lang_info
{
    struct lang_info    * next ;
    char                * unpackinc ;
    char                * unpackexp ;
    char                * unpackmod ;
    char                * compiler ;
    char                * suffix ;
	char                * objectext ;  /* CTS 1104953 */
    char                optchar ;
    char                language ;
} ;

struct langext_info
{
    struct langext_info * next;
    char                * ext ;
    char                language ;
};

/* layer compile information */

struct dirmap_info
{
    struct dirmap_info  * next ;
	char                * shortform ;
    char                * layer ;
} ;


/* a remote VMAKE_PATH uses vmakserv: connection information */

struct connection_info
{
    struct connection_info  * next ;
    char                    * node ;
    int                     sd ;
    int                     cache_lgt ;
    char                    cache [ MESSAGESIZE ] ;
} ;

/* remote message struct, HEADER_SIZE is the part preceeding the data */

struct message
{
    char                    type ;
    char                    reference ;
    unsigned char           length [ 2 ] ;
#define                     HEADER_SIZE     4
    char                    data [ MESSAGESIZE ] ;
} ;

/* file data from remote node is cached */

struct file_cache
{
    int     remain ;
    int     current ;
    int     eof ;
    char    data [ MESSAGESIZE ] ;
} ;

/* remote file struct */

typedef struct
{
    int                     in_use ;
    int                     ref ;
    struct connection_info  * cip ;
    struct file_cache       * fcp ;
} RFILE ;

/* virtual (local or remote) file struct */

typedef struct
{
    int         type ;
    union
    {
        FILE            * l_fp ;
        RFILE           * r_fp ;
    } v_fp ;
} VFILE ;


/* CTS 1112006 */
struct output_info
{
	char				* line;
	struct output_info	* next;
} ;

struct dscline_info
{
	char				* text;
	struct dscline_info	* next;
} ;


struct dscfile_info
{
	char				* type;
	char                * name;
	time_t              * date;
	struct dscline_info	* lines;
	struct dscfile_info	* next;
} ;


#endif
