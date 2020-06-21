/* @(#)%M%   %I%   SAP   %E% */
/**********************************************************************/
/* $Id: //sapdb/V76/c_00/b_37/sys/src/sap/700/ssslxx.h#1 $
 **********************************************************************/
/*                                                                    */
/*  (C) Copyright 2000,  SAP AG, Walldorf                             */
/*                                                                    */
/**********************************************************************/


/*include-------------------------------------------------------------*/
/*                                                                    */
/* Description:                                                       */
/* ============                                                       */
/*                                                                    */
/* ssslxx.h:  ssl = Project  SAP Secure Socket Layer                  */
/*             xx = OS-independent code                               */
/*                                                                    */
/*                                                                    */
/* ssslxx.h contains all nescessary declarations for the              */
/* SAP Secure Socket Layer wrapping convenience API.                  */
/*                                                                    */
/* Underneath this API, a real third-party SSL implementation         */
/* is dynamically loaded as a shared library.                         */
/* Support for OpenSSL (version 0.9.4 as of this writing) and for     */
/* an SSL implementation from SECUDE are planned -- but the focus     */
/* will shift to the SECUDE library (as soon as SSL is available)     */
/*                                                                    */
/*                                                                    */
/* NOTICE:  Except for the Initialization call, this library is       */
/*          can be used in a multi-threaded environment -- provided   */
/*          that independent Handles are established by each thread   */
/*          and he underlying SSL implementation supports threads.    */
/*								      */
/*                                                                    */
/*                                                                    */
/* SAPSSL API:                                                        */
/* ===========                                                        */
/* SapSSLErrorName()	Return printable string for SAPRETURN retcode */
/* SapSSLSetTraceFile() Set the FILE * for the component trace        */
/* SapSSLInit()         Initialize the SapSSL library, dynamically    */
/*                        load the external SSL-library at runtime    */
/* SapSSLDone()         Release all resources related to SapSSL,      */
/*                        unload the external SSL-library             */
/* SapSSLSessionInit()  Create a SSSL_HDL session handle              */
/* SapSSLSessionDone()  Release all resources associated with this    */
/*			  SSSL_HDL session handle -- the actual       */
/*                        ssl crypto context remains cached globally  */
/* SapSSLSetNiHdl()     Use the supplied NI_HDL to establish the      */
/*                        SSL session.				      */
/* SapSSLSetTargetHostname() (Client only) set the target hostname    */
/*                        for an SSL client session that is matched   */
/*			  against the CN= part of the Server's Cert   */
/* SapSSLSessionStart() Initiate the SSL handshake on the session and */
/*                        SSL-authenticate the peer (if requested)    */
/* SapSSLGetPeerInfo()  Query information about the attributes of the */
/*                        SSL session (peer cert, issuer dn, subject dn, cipher) */
/* SapSSLRead()         Receive/Read SSL-protected data from SSSL_HDL */
/* SapSSLReadPending()  Check whether there's any app-data leftover   */
/*                        in the internal buffer from a previous      */
/*		          SapSSLRead() that returned full maxlen bytes*/
/* SapSSLWrite()        Send/Write data over SSSL_HDL                 */
/*                                                                    */
/**********************************************************************/

#ifndef __SSSLXX_H_
#define __SSSLXX_H_

/* One of our calls uses an NI_HDL parameter,                */
/* let's make sure we have included nixx.h behind this point */
#include "nixx.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define SSSL_INVALID_HDL  ((SSSL_HDL)0)

/******************************************/
/* Specify to SapSSLSessionInit() whether */
/* you want to accept a session (=Server) */
/* or initiate a session (=Client)	  */
/******************************************/
typedef enum sssl_role_t {
   SSSL_ROLE_SERVER = 0,
   SSSL_ROLE_CLIENT = 1
} SSSL_ROLE_T;

/***************************************************************************/
/* Specify to SapSSLSessionInit() whether or how you want the peer         */
/* being authenticated (cryptographically at the SSL level, of course)     */
/*                                                                         */
/* Server Side authentication types:					   */
/* ---------------------------------					   */
/*  SSSL_AUTH_NO_CLIENT_CERT   Expect anonymous clients, don't send	   */
/*			       clientCA_list so we never see a client cert */
/*                                                                         */
/*  SSSL_AUTH_ASK_CLIENT_CERT  Ask for a Client cert by sending a	   */
/*			       clientCA_list to the client		   */
/*                             still permit SSL connects of anonymous	   */
/*                             clients.					   */
/*                         CAVEATS: 1) MSIE 4 never prompts user for this. */
/*                                  2) All Browsers cache this decision!   */
/*                                                                         */
/*  SSSL_AUTH_REQUIRE_CLIENT_CERT					   */
/*			       Require a certificate-based client	   */
/*			       authentication to be performed before       */
/*			       data transfer will be permitted on this     */
/*			       connection.				   */
/*                                                                         */
/*                                                                         */
/* Client Side authentication types:					   */
/* -----------------------------------					   */
/*  SSSL_AUTH_NO_CLIENT_CERT   Establish an SSL context without client     */
/*			       client credentials/Certificate		   */
/*			       If the client needs to be authenticated     */
/*			       it must be done at the application level,   */
/*			       maybe with HTTP BASIC Authentication	   */
/*									   */
/*  SSSL_AUTH_USE_CLIENT_CERT						   */
/*			       Establish an SSL context with the default   */
/*			       SSL client credentials of the Server	   */
/*									   */
/*									   */
/* NOTES:								   */
/*  1)  When a specific SSL client credential/Certificate instead of the   */
/*      default ClientSSL credential should be used for ClientSSL	   */
/*      either of the AUTH parameters can be used for SapSSLSessionInit()  */
/*      the specific credential must be configured by a successor call     */
/*      to SapSSLSetSessionCredential() on the SSSL_HDL			   */
/*									   */
/*  2)  When a specific SSL server credential/Certificate instead of the   */
/*      default ServerSSL credential should be used for ServerSSL,	   */
/*      the specific credential must be configured by a successor call	   */
/*	to SapSSLSetSessionCredential() on the SSSL_HDL			   */
/*									   */
/*                                                                         */
/* If you're looking for HTTP basic authentication inside of a HTTPS       */
/* encrypted channel, you will NOT find it here.  That's an application    */
/* service and not an SSL service!					   */
/***************************************************************************/
typedef enum sssl_auth_type_t {
   SSSL_AUTH_NO_CLIENT_CERT	   = 0,  /* Server: don't ask the client for SSL credentials/Certificate */
				         /* Client: connect without SSL client credentials/Certificate   */

   SSSL_AUTH_ASK_CLIENT_CERT	   = 1,  /* Server: suggest to peer to provide SSL credentials/Certificate */
				         /*         NOTE: MSIE 4.0 silently ignores "ASK" and tries to get */
					 /*               the page without client cert - if successful,    */
					 /*               the user will never be shown the certificate     */
					 /*               selection box.  In case of failure, MSIE 4.0     */
					 /*               disconnects and shows the user the selection box */
					 /*         NOTE2: MSIE 4.0 caches whether it can access without   */
					 /*                certificate per SITE+port.  If the server wants */
					 /*                to require a certificate for other pages, the   */
					 /*                user will see a popup box reporting a strange   */
					 /*                error in the SCHANNEL.DLL ...                   */
					 /* Client: NOT APPLICABLE					   */

   SSSL_AUTH_REQUIRE_CLIENT_CERT   = 2,  /* Server: require client to use SSL client credentials/Certificate */
					 /* Client: NOT APPLICABLE					     */

   SSSL_AUTH_USE_CLIENT_CERT	   = 3	 /* Server: NOT APPLICABLE					    */
				         /* Client: use credentials/Certificate when the Server requests    */
					 /*	    it and ours matches the Server's CA clientlist	    */
					 /*         NOTE: When no custom credential/Certificate is provided */
					 /*	          via SapSSLSetSessionCredential(), the default     */
					 /*		  credential/Certificate of the Server will be used */

} SSSL_AUTH_T;

/******************************************************/
/* !! DO NOT USE THE FOLLOWING 3 CONSTANTS ANYMORE !! */
/******************************************************/
#define SSSL_AUTH_REQUIRE_SERVER_CERT	SSSL_AUTH_NO_CLIENT_CERT
#define SSSL_AUTH_ASK_SERVER_CERT       SSSL_AUTH_NO_CLIENT_CERT
#define SSSL_AUTH_NO_SERVER_CERT        SSSL_AUTH_NO_CLIENT_CERT


/*********************************************************/
/* Structure type sapssl_init_desc                       */
/* ===============================                       */
/* Configuring SapSSL explicitly at SapSSLInit() instead */
/* of having it read SAP profile parameters or falling   */
/* back on compiled-in defaults                          */
/*********************************************************/

typedef enum sssl_usage_flags_e {
    SSSL_UF_SERVER		    = (1<<0),
    SSSL_UF_CLIENT		    = (1<<1),
    SSSL_UF_ANON_CLIENT		    = (1<<2)
} sssl_usage_flags_et;

typedef enum sssl_param_flags_e {
    SSSL_PARM_SERVER_CACHE_SIZE     = (1<<0),
    SSSL_PARM_SERVER_CACHE_LIFETIME = (1<<1),
    SSSL_PARM_CLIENT_CACHE_SIZE	    = (1<<2),
    SSSL_PARM_CLIENT_CACHE_LIFETIME = (1<<3),
    /* Parameters/Flags for debugging purposes ONLY */
    SSSL_DBGPARM_ALLOW_EXPIRED_PSE  = (1<<16),
    SSSL_DBGPARM_USE_OPENSSL        = (1<<17)
} sssl_param_flags_et;

typedef struct sapssl_init_s {
    size_t         struct_size;		/* size of this structure (for future extensibility) */
    SAP_UINT       usage_flags;		/* OR'ed sssl_usage_flags_et's			     */
    SAP_UINT       param_flags;		/* SHOULD BE 0 (Zero) unless you really know what you're doing */
    SAP_UC       * ssl_lib;		/* (fully qualified) path+filename of the SSL_LIB    */
    SAP_UC       * sapssls_filename;    /* OPT: explicit filename for SAPSSLS.PSE (server)   */
    SAP_UC       * sapsslc_filename;	/* OPT: explicit filename for SAPSSLC.PSE (client)   */
    SAP_UC       * sapssla_filename;	/* OPT: explicit filename for SAPSSLA.PSE (anon)     */
    SAP_UC	 * secudir;		/* OPT: location of SECUDIR			     */
    SAP_UC       * ciphersuites;	/* OPT: SSL ciphersuites			     */
    unsigned int   server_cache_size;     /* Max number of Server cache entries, default=1000 */
    unsigned int   server_cache_lifetime; /* Max session lifetime in server cache, default=3600 (seconds) */
    unsigned int   client_cache_size;     /* Max number of Client cache entries, default=1000 */
    unsigned int   client_cache_lifetime; /* Max session lifetime in client cache, default=3600 (seconds) */
} sapssl_init_desc, * sapssl_init_t;


typedef struct sssl_cert_array_s {
    SAP_RAW	 * asn1_blob;	     /* binary, ASN.1 DER encoded Certificate */
    SAP_INT	   asn1_blob_len;    /* length of binary Certificate          */
} sssl_cert_array_desc, * sssl_cert_array_t;


/*************************************************************/
/*************************************************************/



#ifdef SAPSSL_INTERNAL
#  include "ssslxxi.h"
#else
  /* the actual structure definition for sssl_hdl_s is */
  /* private to SapSSL and hidden from regular callers */
   struct sssl_hdl_s;
   typedef struct sssl_hdl_s * SSSL_HDL;
#endif


/*************************************************************/
/*************************************************************/


/**************************************************************/
/* List of all error codes that may be returned by individual */
/* Calls of the SapSSL API.                                   */
/*							      */
/* Only very few of these need special treatment by the       */
/* calling application.  Most of them just indicate different */
/* flavours of a fatal problem, hopefully facilitating debug  */
/* of the SapSSL code.  See the description of the individual */
/* API calls which errors should be individually recognized.  */
/**************************************************************/

/*-----------------------------------------------------------*/
/* Implementor's note:					     */
/*             Keep this list of SSSLERR_* error codes       */
/*             synchronized with the struct sapssl_errorname */
/*             in ssslxx.c, so that SapSSLErrorName() works  */
/*-----------------------------------------------------------*/

#define SSSLERR_LIB_NOT_FOUND	       ((SAPRETURN)-1)
#define SSSLERR_INCOMPATIBLE_LIB       ((SAPRETURN)-2)
#define SSSLERR_LIBINIT_FAILED	       ((SAPRETURN)-3)
#define SSSLERR_OUT_OF_MEMORY	       ((SAPRETURN)-4)
#define SSSLERR_FUNC_ARGS	       ((SAPRETURN)-5)
#define SSSLERR_INIT_FIRST	       ((SAPRETURN)-6)
#define SSSLERR_NOT_YET_IMPLEMENTED    ((SAPRETURN)-7)
#define SSSLERR_INTERNAL_BUG	       ((SAPRETURN)-8)
#define SSSLERR_UNKNOWN_BEHAVIOUR      ((SAPRETURN)-9)

#define SSSLERR_CONN_CLOSED	       ((SAPRETURN)-10)

#define SSSLERR_SERVER_ONLY	       ((SAPRETURN)-11)
#define SSSLERR_CLIENT_ONLY	       ((SAPRETURN)-12)
#define SSSLERR_NO_CONTEXT_YET	       ((SAPRETURN)-13)
#define SSSLERR_BAD_SSSL_HDL	       ((SAPRETURN)-14)
#define SSSLERR_BAD_AUTH_TYPE	       ((SAPRETURN)-15)
#define SSSLERR_BAD_ROLE	       ((SAPRETURN)-16)

#define SSSLERR_SESSION_NOT_STARTED    ((SAPRETURN)-17)
#define SSSLERR_SESSION_ALREADY_STARTED	((SAPRETURN)-18)
#define SSSLERR_PEER_INFO_FAILED       ((SAPRETURN)-19)

#define SSSLERR_NO_CLIENT_CERT	       ((SAPRETURN)-20)
#define SSSLERR_NO_SERVER_CERT	       ((SAPRETURN)-21)
#define SSSLERR_SAPPARAM_MISSING       ((SAPRETURN)-22)

#define SSSLERR_NEED_NI_HDL	       ((SAPRETURN)-23)
#define SSSLERR_BAD_NI_HDL	       ((SAPRETURN)-24)
#define SSSLERR_NO_SSL_REQUEST	       ((SAPRETURN)-25)
#define SSSLERR_HTTP_REQUEST	       ((SAPRETURN)-26)
#define SSSLERR_HTTPS_PROXY_REQUEST    ((SAPRETURN)-27)
#define SSSLERR_MISSING_TARGET_HOSTNAME ((SAPRETURN)-28)
#define SSSLERR_THREAD_MALFUNCT	       ((SAPRETURN)-29)
#define SSSLERR_SERVER_CERT_MISMATCH   ((SAPRETURN)-30)
#define SSSLERR_ROLE_NOT_AVAIL	       ((SAPRETURN)-31)


#define SSSLERR_PSE_ERROR	       ((SAPRETURN)-40)
#define SSSLERR_FILENAME_TRUNCATED     ((SAPRETURN)-41)
#define SSSLERR_CRED_NOT_FOUND	       ((SAPRETURN)-42)
#define SSSLERR_NOT_SUPPORTED_BY_SECUDE ((SAPRETURN)-43)

#define SSSLERR_NOT_IMPLEMENTED_SERVER ((SAPRETURN)-44)
#define SSSLERR_NOT_IMPLEMENTED_CLIENT ((SAPRETURN)-45)

/* The following errors are distinguished for debugging only,   */
/* Applications should NOT use the for any kind of flow control */
/* as they might be product-specific or being removed in future */
/* versions of this library!					*/
#define SSSLERR_CERT_FILE	       ((SAPRETURN)-50)
#define SSSLERR_PRIVKEY_FILE	       ((SAPRETURN)-51)
#define SSSLERR_CERT_PRIVKEY_MISMATCH  ((SAPRETURN)-52)
#define SSSLERR_SSL_SET_SSL_METHOD     ((SAPRETURN)-53)
#define SSSLERR_SSLCTX_NEW	       ((SAPRETURN)-54)
#define SSSLERR_SSL_NEW		       ((SAPRETURN)-55)
#define SSSLERR_SSL_ACCEPT	       ((SAPRETURN)-56)
#define SSSLERR_SSL_CONNECT	       ((SAPRETURN)-57)
#define SSSLERR_SSL_READ	       ((SAPRETURN)-58)
#define SSSLERR_SSL_WRITE	       ((SAPRETURN)-59)


#define SSSLERR_BAD_X500DN_IN_CERT     ((SAPRETURN)-80)

/*-------------------------------------------------------------------*/
/* SapSSLErrorName()						     */
/*								     */
/* converts the numerical return code from any SapSSL API call	     */
/* into a textual String representing the label/macro		     */
/*    i.e.   0 --> "SAP_O_K"					     */
/*	    -1 --> "SSSLERR_LIB_NOT_FOUND"			     */
/*								     */
/* INPUT    rc	     numeric return code			     */
/*								     */
/* RETURN            printable String				     */
/*		     the label for the return code		     */
/*-------------------------------------------------------------------*/
SAP_UC    * SapSSLErrorName(	 SAPRETURN     rc	    );


/*-------------------------------------------------------------------*/
/* SapSSLSetTraceFile()						     */
/*								     */
/* Defines/redirects the file handle for the component trace output  */
/* This routine **MUST** be called prior to any other SapSSL()       */
/* routine to ensure that we don't miss any helpful error messages   */
/*								     */
/* INPUT   fp	     a (FILE *) to receive the trace output of the   */
/*                   SapSSL library (errors and debugging info)      */
/*                   which will be used for TRC() style makros       */
/*                   of the dptrc library                            */
/*								     */
/* RETURN							     */
/*								     */
/*  SAP_O_K	succees						     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLSetTraceFile(	 FILE	     * fp	    );


/*-------------------------------------------------------------------*/
/* SapSSLInit()							     */
/*								     */
/* Process-wide initialization of the SapSSL library.  Needs to be   */
/* called from the main thread before any of the SapSSL services     */
/* can be used.  This call will try to load the external SSL library */
/* at runtime and fail if there is a problem (e.g. library not found)*/
/* Can safely be called multiple times (from the MAIN thread!)	     */
/*								     */
/* INPUT   use_profile  indicate whether SapSSL should retrieve its  */
/*			configuration information from the instance  */
/*			profile via sapgparam().		     */
/*								     */
/*	   init_params  may be 0				     */
/*                      The caller may supply SapSSL configuration   */
/*                      by passing an explicit sapssl_init_desc      */
/*                      with the desired configuration parameters    */
/*                      (see structure definition near the beggining */
/*                       of this header file)			     */
/*								     */
/*	   ret_reserved must be 0				     */
/*								     */
/* RETURN							     */
/*								     */
/*  SAP_O_K		       succees				     */
/*  SSSLERR_SAPPARAM_MISSING   required profile parameters missing   */
/*  SSSLERR_LIB_NOT_FOUND      could not find/load external SSL lib  */
/*  SSSLERR_INCOMPAT_LIBRARY   function entry points missing in      */
/*			         external SSL lib		     */
/*  SSSLERR_LIBINIT_FAILED     initialization of the external SSL    */
/*			         library failed			     */
/*  ... and several others					     */
/*      if any error occurs, you will find details in the trace file */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLInit(	 SAP_BOOL            use_profile,
			 sapssl_init_desc  * init_params,
			 void             ** ret_reserved );



/*-------------------------------------------------------------------*/
/* SapSSLDone()							     */
/*								     */
/* Process-wide cleanup of all SapSSL library resources.	     */
/* The dynamically loaded external SSL library will be unloaded.     */
/*								     */
/*								     */
/* RETURN							     */
/*								     */
/*  SAP_O_K	succees						     */
/*  ... you don't really care about the return code here, do you?    */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLDone( void );


/*********************************************************************/
/*********************************************************************/
/*-------------------------------------------------------------------*/
/* You need to initialize SapSSL() with SapSSLInit() before any of   */
/* the following API calls will work!				     */
/*-------------------------------------------------------------------*/
/*********************************************************************/
/*********************************************************************/



/*-------------------------------------------------------------------*/
/* SapSSLSessionInit()						     */
/*								     */
/* Create a new SapSSL session handle (SSSL_HDL).		     */
/* SSSL_HDLs are thread-safe in "appartment model",		     */
/* i.e. different SSSL_HDLs may be used simultaneously in different  */
/* threads.							     */
/* DO NOT use the same SSSL_HDL in two threads simultaneously.       */
/*								     */
/* INPUT   role       Create a new SapSSL session handle for         */
/*                      - either a Server (SSSL_ROLE_SERVER)	     */
/*			- or a Client (SSSL_ROLE_CLIENT)	     */
/*			SSL endpoint				     */
/*								     */
/*         auth_type  Select whether/how you want the peer being     */
/*			authenticated at the SSL-level (cert-based)  */
/*                      (See comments for the SSL_AUTH_* constants   */
/*			 for further information)                    */
/*								     */
/* OUTPUT  sssl_hdl   New session handle			     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*  SSSLERR_BAD_ROLE	   invalid role paramter value 		     */
/*  SSSLERR_BAD_AUTH_TYPE  invalid auth_type parameter value	     */
/*  ... and several others					     */
/*								     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLSessionInit(	 SSSL_HDL    * sssl_hdl,
				 SSSL_ROLE_T   role,
				 SSSL_AUTH_T   auth_type    );


/*-------------------------------------------------------------------*/
/* SapSSLSetNiHdl()						     */
/*								     */
/* Pass down the NI_HDL with the TCP socket for the SSL connection   */
/* For servers, this must be an accept()ed socket, for clients this  */
/* must be a connect()ed socket.				     */
/*								     */
/* SapSSL will retrieve the OS-native socket handle for the supplied */
/* ni_hdl and pass that down into SAPCRYPTOLIB -- which is going to  */
/* perform native socket read/recv and write/send calls.             */
/* 								     */
/*								     */
/* INPUT    sssl_hdl	   A valid&fresh SapSSL session handle	     */
/*								     */
/*	    ni_hdl         handle to a connected(accepted) NI_HDL    */
/*		           which should be used to establish the     */
/*                         SSL-protected communication channel	     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*  SSSLERR_CONN_CLOSED    Either NiHdlGetPeer() or NiHdlGetName()   */
/*                           reported NIECONN_BROKEN for the supplied*/
/*                           ni_hdl, indicating that the peer has    */
/*                           already closed the communication channel*/
/*  SSSLERR_BAD_NI_HDL     A call to an Ni-function with ni_hdl as   */
/*                           the parameter failed with an error      */
/*                           other than NIECONN_BROKEN               */
/*  ... and several others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLSetNiHdl(	 SSSL_HDL      sssl_hdl,
				 NI_HDL	       hdl	    );

#if 0
/* SapSSLSessionReNego() not yet available */
SAPRETURN   SapSSLSessionReNego( SSSL_HDL      sssl_hdl,
			         SSSL_AUTH_T   auth_type    );
#endif


/*-------------------------------------------------------------------*/
/* SapSSLSetTargetHostname()		SSL CLIENT ONLY		     */
/*								     */
/*								     */
/* Set the Hostname of the TCP target for a client SSL session, so   */
/* that it can be matched against the hostname (pattern) in the      */
/* server's certificate.  This is the cheap Authentication that      */
/* for example Web-Browsers perform when establishing an HTTPS       */
/* connection.  For HTTPS (HTTP inside SSL), you MUST pass the       */
/* hostname EXACTLY as it is specified in the URL !! (btw. no port!) */
/*								     */
/* INPUT    sssl_hdl	   A valid&fresh SapSSL session handle	     */
/*								     */
/*	    hostname       The hostname of the target		     */
/*			   (e.g. from the URL)			     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*  SSSLERR_CLIENT_ONLY    this call is only for client SSL sessions */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLSetTargetHostname(   SSSL_HDL	     sssl_hdl,
				       SAP_UC	   * hostname    );


/*-------------------------------------------------------------------*/
/* SapSSLSessionStart()						     */
/*								     */
/* Start the SSL context negotiation with the Peer.  This call is    */
/* for both, client and servers.				     */
/*								     */
/*								     */
/* INPUT    sssl_hdl	   A valid&fresh SapSSL session handle where */
/*                         the SSL context negotiation (and probably */
/*                         the SSL authentication) should be started */
/*								     */
/* REMARKS                                                           */
/*  When this function returns SSSLERR_SERVER_CERT_MISMATCH          */
/*  the sssl_hdl remains still usable.  In case that an application  */
/*  caller is absolutely positively convinced that a mismatch can    */
/*  be ignored then it could proceed as if SAP_O_K was returned.     */
/*                                                                   */
/*  However, the application caller is STRONGLY ADVISED to ask an    */
/*  interactive user to confirm ignoring the mismatch and/or to      */
/*  write/log a diagnostic message which indicates that a mismatch   */
/*  between the supplied target_hostname and the hostname from the   */
/*  server's certificate occurred and is ignored.                    */
/*                                                                   */
/*                                                                   */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*  SSSLERR_NO_SSL_REQUEST The client is not talking SSL with us     */
/*			     no hints about the client's protocol    */
/*			     available				     */
/*  SSSLERR_HTTP_REQUEST   The client is NOT talking SSL with us     */
/*			     it looks like cleartext HTTP	     */
/*  SSSLERR_HTTPS_REQUEST  The client is NOT talking SSL with us     */
/*                           it looks like a HTTPS-Proxy CONNECT     */
/*  SSSLERR_NO_CLIENT_CERT As per your request, this session	     */
/*			     REQUIRES SSL client authentication,     */
/*			     however the client did not send his cert*/
/*                                                                   */
/*  SSSLERR_CONN_CLOSED	   The peer prematurely closed the socket    */
/*			     Web Browsers do this frequently;	     */
/*			     in particular MSIE 4 before it prompts  */
/*			     the user with a certificate selection   */
/*			     popup with SSSL_AUTH_REQUIRE_CLIENT_CERT*/
/*								     */
/*  SSSLERR_MISSING_TARGET_HOSTNAME				     */
/*         (Client only)   For SSL client sessions, the hostname of  */
/*                           the TCP target must first be set via    */
/*                           SapSSLSetTargetHostname() so that the   */
/*                           Server certificate can be verified.     */
/*								     */
/*  SSSLERR_SERVER_CERT_MISMATCH				     */
/*	   (Client only)   The hostname in the "CN=" part of	     */
/*			     the Server's Certificate does not match */
/*			     the hostname that was passed in via     */
/*			     SapSSLSetTargetHostname()		     */
/*								     */
/*  ... and several others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLSessionStart(	 SSSL_HDL      sssl_hdl	    );


/*-------------------------------------------------------------------*/
/* SapSSLGetPeerInfo()						     */
/*								     */
/* Query information about the peer's identity (provided he has	     */
/* been authenticated) and about the cipher that is being used on    */
/* the SSL channel associated with the referenced SapSSL session.    */
/*								     */
/* NOTICE: 1) All return parameters to this function are optional,   */
/*            i.e. if NULL is passed in, no value will be returned   */
/*								     */
/*         2) All return parameters cert,subject_dn,issuer_dn        */
/*            will reference read-only memory when they return values*/
/*            The values remain valid until the associated sssl_hdl  */
/*            is disposed via SapSSLSessionDone()                    */
/*								     */
/*         3) If the peer/client was not authenticated, then the     */
/*            return values for cert,cert_len,subject_dn,issuer_dn   */
/*	      will be 0 (when they're requested)		     */
/*								     */
/*								     */
/* INPUT   sssl_hdl	   A handle to an established SapSSL session */
/*								     */
/* OUTPUT  cert            (readonly) pointer to DER-encoded binary  */
/*                         blob of the peer's X.509 certificate      */
/*								     */
/*         cert_len	   length of the certificate blob            */
/*								     */
/*         subject_dn	   printable representation of the subject   */
/*			   distinguished name in the peer's cert     */
/*								     */
/*	   issuer_dn	   printable representation of the isser     */
/*			   distinguised name in the peer's cert	     */
/*								     */
/*	   cipher	   printable name of the cipher-suite that   */
/*			   was negotiated to protect this particular */
/*			   SSL communication channel.		     */
/*								     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*  SSSLERR_NO_CLIENT_CERT As per your request, this session	     */
/*	   (Server only)     REQUIRES SSL client authentication,     */
/*			     however the client did not send his cert*/
/*  ... and several others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLGetPeerInfo(	 SSSL_HDL      sssl_hdl,
				 SAP_RAW    ** cert,
				 SAP_INT     * cert_len,
				 SAP_UC	    ** subject_dn,
				 SAP_UC	    ** issuer_dn,
				 SAP_UC	    ** cipher    );


/*-------------------------------------------------------------------*/
/* SapSSLGetPeerInfo2()						     */
/*								     */
/* Query further information about the peer's identity and the       */
/* SSL cipher suite that is used for the referenced SapSSL session   */
/*								     */
/* NOTICE: 1) All return parameters to this function are optional,   */
/*            i.e. if NULL is passed in, no value will be returned   */
/*								     */
/*         2) All return parameters cert_array, cipher_id            */
/*            will reference read-only memory when they return values*/
/*            The values remain valid until the associated sssl_hdl  */
/*            is disposed via SapSSLSessionDone()                    */
/*								     */
/*         3) If the peer was not authenticated, then the            */
/*            return values for cert_array, cert_array_len           */
/*	      will be 0 (when they're requested)		     */
/*								     */
/*								     */
/* INPUT   sssl_hdl	   A handle to an established SapSSL session */
/*								     */
/*         omit_rootca_cert  don't include any RootCA certificate    */
/*                         in the returned cert_array which may      */
/*                         have been sent by the SSL peer            */
/*								     */
/* OUTPUT  cert_array      (readonly) pointer to the array of        */
/*                         binary ASN.1-DER-encoded X.509 certs      */
/*                         of the peer and the forward chain up to   */
/*                         (but *NOT* including) its RootCA cert.    */
/*								     */
/*         cert_array_len  number of actual certificates in the array*/
/*								     */
/*         cipher_id       (readonly) pointer to the octets that     */
/*                         indicate the SSL cipher suite that is     */
/*                         being used for an established SSL session */
/*                                                                   */
/*         cipher_id_len   number of actual octets used              */
/*                         SSL v3 and TLS use 2 octets,		     */
/*                         however SSL v2 uses 3 octets              */
/*								     */
/*         cipher_keysize  the number of (effective) key bits        */
/*                         used by the SSL ciphersuite for the       */
/*                         current SSL session.                      */
/*                         e.g.    0 - no encryption in use          */
/*				  40 - "exportable" ciphersuite      */
/*                                56 - probably single-DES           */
/*                               128 - RC4-128, IDEA, others         */
/*                               168 - 3DES                          */
/*                                                                   */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*  SSSLERR_NO_CLIENT_CERT As per your request, this session	     */
/*	   (Server only)     REQUIRES SSL client authentication,     */
/*			     however the client did not send his cert*/
/*  SSSLERR_NO_SERVER_CERT The certificate (chain) of the server is  */
/*                           not available (this should be unusual)  */
/*  ... and several others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLGetPeerInfo2(	 SSSL_HDL		  sssl_hdl,
			         SAP_BOOL		  omit_rootca_cert,
				 sssl_cert_array_desc **  cert_array,
				 SAP_INT	       *  cert_array_len,
				 SAP_RAW	      **  cipher_id,
				 SAP_INT	       *  cipher_id_len,
				 SAP_INT               *  cipher_keysize );


/*-------------------------------------------------------------------*/
/* SapSSLRead()							     */
/*								     */
/* Read application data from the communication peer (may block)     */
/*								     */
/* With SSL all protocol handshake messages and all application data */
/* is wrapped into "SSL Records" of at most 16 KByte each.  When     */
/* reading data from the network, each SSL Record is individually    */
/* read from the network socket and completely processed before the  */
/* next record is read.						     */
/*								     */
/* SSL_read() will continuously try reading the socket (and block    */
/* when necessary) until a full SSL Record has been received.        */
/*								     */
/* When the received SSL Record contains application data,           */
/* SSL_read() will return all of the data in the record -- or if the */
/* supplied buffer is too small, it will only fill the supplied      */
/* buffer and keep the rest in the incoming SSL record buffer, where */
/* the next call to SSL_read() will find it.                         */
/*								     */
/* This means when the caller of SapSSLRead() supplies only a small  */
/* buffer, then it may happen that SapSSLRead() fills up this buffer */
/* completely.  In this situation the caller doesn't know whether    */
/* and how much data remained in the incoming SSL record buffer.     */
/* It is important to understand that the availability of further    */
/* buffered data in the SSL record buffer is NOT visible on the      */
/* underlying network socket (as a "readable" Event).		     */
/*								     */
/* Some callers prefer to wait on the Socket using Select/Poll	     */
/* rather than wait in a blocking call to SapSSLRead().              */
/* Those callers are advised to call SapSSLReadPending() in case     */
/* that SapSSLRead() fills their supplied buffer completely, so      */
/* that they can find out whether there is still data waiting        */
/* on the SSL context that they can retrieve via SapSSLRead()        */
/* directly without blocking.					     */
/*								     */
/*								     */
/* INPUT    sssl_hdl	   A handle to an established SapSSL session */
/*								     */
/*	    buf		   Pointer to a preallocated buffer that is  */
/*			   to receive app data from the session	     */
/*								     */
/*	    maxlen	   the maximum amount of data that should be */
/*			   (or can be) read into "buf" during this   */
/*			   call					     */
/*								     */
/*	    timeout	   NOT YET IMPLEMENTED -- use NI_BLOCK	     */
/*								     */
/* OUTPUT   readlen	   the amount of data that was actually      */
/*			   read and copied into "buf"		     */
/*								     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*								     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*								     */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*                                                                   */
/*  SSSLERR_CONN_CLOSED	   The peer (prematurely) closed the socket  */
/*			     Web Browsers do this frequently;	     */
/*			     in particular MSIE 4 before it prompts  */
/*			     the user with a certificate selection   */
/*			     popup with SSSL_AUTH_REQUIRE_CLIENT_CERT*/
/*			     Or when (impatient) users press "STOP"  */
/*			     on their browsers, close the browser    */
/*			     window, hit another hyperlink, etc.     */
/*								     */
/*  SSSLERR_NO_CLIENT_CERT As per your request, this session	     */
/*			     REQUIRES SSL client authentication,     */
/*			     however the client did not send his cert*/
/*								     */
/*  ... and several others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLRead(		 SSSL_HDL      sssl_hdl,
				 SAP_RAW     * buf,
				 SAP_INT       maxlen,
				 SAP_INT       timeout,
				 SAP_INT     * readlen	    );

/*-------------------------------------------------------------------*/
/* SapSSLReadPending()						     */
/*								     */
/* Check whether application data __remained__ in the incoming       */
/* SSL-Record buffer after the last call to SapSSLRead().            */
/*                                                                   */
/* You may want to call this Function whenever SapSSLRead() has      */
/* filled the supplied buffer completely (i.e. *readlen == maxlen)   */
/*								     */
/* If the buffer supplied to the last SapSSLRead() call was only     */
/* partly filled then there should not be any remaining data in the  */
/* internal SSL-Record buffer and this call will return 0.           */
/*								     */
/* INPUT    sssl_hdl	 A handle to an established SapSSL session   */
/*								     */
/* OUTPUT   pendlen	 The amount of data that is still waiting    */
/*								     */
/*     pendlen > 0    => data is pending, please call SapSSLRead()   */
/*                       The value should give the amount of         */
/*		         waiting bytes, however the comments in the  */
/*                       SSL implementation suggest that there may   */
/*                       be situations where this value refers to    */
/*                       protocol handshake data rather than         */
/*                       application data.  In these situations      */
/*                       SapSSLRead() should be called as well,      */
/*                       however the size of the application data    */
/*                       in the next SSL record is still unknown     */
/*								     */
/*     pendlen == 0   => no data is pending, the next call to        */
/*                       SapSSLRead() will perform a (blocking)      */
/*		         network receive() operation                 */
/*			 you could select/poll the socket instead    */
/*                       to find out when it might be appropriate    */
/*                       to call SapSSLRead() again		     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*								     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*								     */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*                                                                   */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLReadPending(	 SSSL_HDL      sssl_hdl,
				 SAP_INT     * pendlen	    );

/*-------------------------------------------------------------------*/
/* SapSSLWrite()						     */
/*								     */
/*								     */
/*								     */
/* INPUT    sssl_hdl	   A handle to an established SapSSL session */
/*								     */
/*	    buf		   A pointer to the data that is to be       */
/*			   transmitted to the peer		     */
/*								     */
/*	    len		   The length of the data in buf	     */
/*								     */
/*	    timeout	   NOT YET IMPLEMENTED -- use NI_BLOCK	     */
/*								     */
/* OUTPUT   writelen	   The amount of data that was written	     */
/*			   upon success, this should always be the   */
/*			   same as the given input "len"	     */
/*								     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*								     */
/*  SSSLERR_FUNC_ARGS      at least one parameter has an invalid     */
/*			     value (usually a required output	     */
/*			     parameter is missing (i.e. is NULL))    */
/*								     */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*                                                                   */
/*  SSSLERR_CONN_CLOSED	   The peer (prematurely) closed the socket  */
/*			     Web Browsers do this frequently;	     */
/*			     in particular MSIE 4 before it prompts  */
/*			     the user with a certificate selection   */
/*			     popup with SSSL_AUTH_REQUIRE_CLIENT_CERT*/
/*			     Or when (impatient) users press "STOP"  */
/*			     on their browsers, close the browser    */
/*			     window, hit another hyperlink, etc.     */
/*								     */
/*  SSSLERR_NO_CLIENT_CERT As per your request, this session	     */
/*			     REQUIRES SSL client authentication,     */
/*			     however the client did not send his cert*/
/*								     */
/*  ... and several others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLWrite(	 SSSL_HDL      sssl_hdl,
				 SAP_RAW     * buf,
				 SAP_INT       len,
				 SAP_INT       timeout,
				 SAP_INT     * writelen	    );


/*-------------------------------------------------------------------*/
/* SapSSLSessionDone()						     */
/*								     */
/*								     */
/*								     */
/* INPUT    sssl_hdl	   A handle to an established SapSSL session */
/*								     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*								     */
/*  ... and several others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLSessionDone(	 SSSL_HDL    * sssl_hdl	    );  



/*-------------------------------------------------------------------*/
/* SapSSLSetSessionCredential()		   			     */
/*								     */
/* NOT_YET_IMPLEMENTED						     */
/*								     */
/* INPUT    sssl_hdl	   A valid&fresh SapSSL session handle	     */
/*								     */
/*	    filename	   The (file)name for an SSL credential,     */
/*			   SecudeSSL: filename of a PSE		     */
/*			   OpenSSL:   filename of a "*.pem" file     */
/*				      containing Key and Cert	     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*  SSSLERR_INIT_FIRST     you must call SapSSLInit() once	     */
/*  SSSLERR_BAD_SSSL_HDL   the parameter sssl_hdl is not a valid     */
/*                           SapSSL session handle		     */
/*								     */
/*  SSSLERR_CRED_NOT_FOUND the named credential was not found	     */
/*								     */
/*  ... and a few others					     */
/*-------------------------------------------------------------------*/
SAPRETURN   SapSSLSetSessionCredential(
				 SSSL_HDL	sssl_hdl,
				 SAP_UC	      *	credname    );

/*-------------------------------------------------------------------*/
/* SapSSL()		   					     */
/*								     */
/*								     */
/* INPUT							     */
/*								     */
/* OUTPUT							     */
/*								     */
/* RETURN							     */
/*  SAP_O_K		   speaks for itself			     */
/*								     */
/*-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif  /* __SSSLXX_H_ */
