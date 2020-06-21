/* @(#) $Id: //sapdb/V76/c_00/b_37/sys/src/sap/700/err.h#1 $ SAP*/

/*
 *     SAP AG Walldorf
 *     Systeme, Anwendungen und Produkte in der Datenverarbeitung
 *
 *     (C) Copyright SAP AG 1994
 */

#ifndef SAPERR_H
#define SAPERR_H

/*incl----------------------------------------------------------------*/
/* Beschreibung:                                                      */
/*                                                                    */
/* err.h is the header for the sap error info package. The purpose is */
/* to save and retrieve error informations in a unique format.        */
/*                                                                    */
/* A error informations consists of the following fields:             */
/*                                                                    */
/* Field           Semantics                                          */
/* ------------------------------------------------------------------ */
/* component   (*) Component, which reported the error (NI, CPIC, ..) */
/* version     (*) Version of that component                          */
/* module      (*) module, in which the error occured                 */
/* line        (*) line of that module                                */
/* descr       (*) brief description of the error                     */
/* rc          (*) return code                                        */
/* detail      (*) detailed description of the error                  */
/* system_call (*) failed system call                                 */
/* os_errno    (*) errno of the os                                    */
/* location    (*) description of the place, where the error occured  */
/*                 (may be host name and executable name)             */
/* time            time when the error happend                        */
/* release         SAP release                                        */
/* errno_txt       description of the os errno                        */
/* err_count       error counter                                      */
/*                                                                    */
/* The field marked with (*) must be supplied by the user. As long as */
/* not set the default value for this fields is the empty string.     */
/*                                                                    */
/* In order to support languages the fields descr and detail may      */
/* contain message numbers for language specific texts. A message     */
/* number must be placed at the beginn of the string within '(' and   */
/* ')'. Example:   (00451)this is a lanugage specific text.           */
/* Normaly, the language specific texts a read from a file. But you   */
/* can define a function to retrive your messages from anywhere       */
/* (see function ErrSetGetTxt). Within you language specific texts    */
/* you can use & and &<Number> to insert the variable parts.          */
/* This replace signs must match the "%"-signs of the original text.  */
/*                                                                    */
/* See example errtst.c and error message file errmsg.                */
/*                                                                    */
/* function overview                                                  */
/*                                                                    */
/* ErrSetMyLocation           defines location of a executable        */
/* ErrGetMyLocation           retrieves own location                  */
/* ErrSet / ErrSetSys         saves error informations                */
/* ErrGet/ ErrGetL            retrieves error informations            */
/* ErrGetFld / ErrGetFldL     retrieves single fields of the error    */
/*                            information                             */
/* ErrTrace                   writes the error information to a       */
/*                            specified target                        */
/* ErrGetAttr                 retrieves address and length of the     */
/*                            saved error information                 */
/* ErrCpTo / ErrCpFrom        copy of error informations              */
/* ErrCpToNet                 conv err info to net format and copy    */
/* ErrReplace                 replace a single field of the error     */
/*                            information                             */
/* ErrIsAInfo                 checks the existence of a error info    */
/* ErrClear                   clears a error information              */
/* ErrFree                    release all error resources allocated   */
/*                            for this thread                         */
/* ErrPr / ErrPrL             builds one string which contions the    */
/*                            error information                       */
/* ErrPrErrno                 gives the text to a os errno            */
/* ErrPrErrno                 gives the text to a os errno (mt safe)  */
/* ErrSetMsgFile              set name of the error message file      */
/* ErrSetGetTxt               set the function to read language       */
/*                            dependend texts                         */
/* ErrSetMsgKey               sets the key length of language         */
/*                            dependend texts                         */
/* ErrVersion                 get version of err api                  */
/* ErrSapDllRelease           get sap release of dptrcerr.dll         */
/*                                                                    */
/*end-----------------------------------------------------------------*/
                          
BEGIN_externC
/*--------------------------------------------------------------------*/
/* internal version of the err api                                    */
/*                                                                    */
/* Version 3:  ol 12.03.03 ErrPrErrno2 (mt-safe) added                */
/* Version 2:  se 19.10.95                                            */
/*             Neue Funktion ErrSapDllRelease zur Abfrage des         */
/*             SAP Release (aus SAP_RELEASE, saptypeb.h)              */
/* Version 1:                                                         */
/*                                                                    */
/* beginn of version maintenance                                      */
/*--------------------------------------------------------------------*/
#define INTERNAL_ERR_VERSION    2

/*--------------------------------------------------------------------*/
/* Return-Codes                                                       */
/*--------------------------------------------------------------------*/
#define ERR_INFO_TOO_LONG       -1
#define ERR_NO_INFO_FOUND       -2
#define ERR_BAD_FIELD_ID        -3
#define ERR_BAD_LENGTH          -4
#define ERR_MALLOC_FAILED       -5
#define ERR_BAD_EYE_CATCHER     -6
#define ERR_INCOMPLETE          -7
#define ERR_NET_CONVERTING      -8

/*--------------------------------------------------------------------*/
/* Error-Info-Laenge                                                  */
/*--------------------------------------------------------------------*/
#define SAP_ERR_INFO_LN    500

/*--------------------------------------------------------------------*/
/* Existiert ein Error-Info ?                                         */
/* The global was removed to support platforms with threads - D.Rachal*/
/*--------------------------------------------------------------------*/
#define ERR_IS_A_INFO()                                                \
   ErrIsAInfo()

/*--------------------------------------------------------------------*/
/* Konstanten fuer den ErrSet-Aufruf                                  */
/*--------------------------------------------------------------------*/
#define ERR_NO_RC          0
#define ERR_NO_ERRNO       0
#define ERR_NO_LINE        0
#define ERR_NO_VERSION     0
#define ERR_NO_COUNTER     0

/*--------------------------------------------------------------------*/
/* Konstanten fuer den ErrGetL-Aufruf                                 */
/*--------------------------------------------------------------------*/
#define ERR_NO_LANGUAGE    cU(' ')

/*--------------------------------------------------------------------*/
/* Konstanten fuer die Funktion ErrGetFld und ErrReplace              */
/*--------------------------------------------------------------------*/
#define ERR_DESCR        2
#define ERR_RC           3
#define ERR_COMP         4
#define ERR_REL          5
#define ERR_VER          6
#define ERR_MOD          7
#define ERR_LINE         8
#define ERR_DETAIL       9
#define ERR_TIME         10
#define ERR_CALL         11
#define ERR_ERRNO        12
#define ERR_ERRNO_TXT    13
#define ERR_COUNTER      14
#define ERR_LOCATION     15
#define ERR_DESCR_MSG    16
#define ERR_DESCR_VARGS  17
#define ERR_DETAIL_MSG   18
#define ERR_DETAIL_VARGS 19

/*--------------------------------------------------------------------*/
/* function types:                                                    */
/* - type of the function, which reads the language file              */
/* - type of the function, which performs converts                    */
/*--------------------------------------------------------------------*/
typedef SAP_BOOL (* ERR_GET_TXT_PTR) (const SAP_UC * msg,
				      SAP_UC language,
				      SAP_UC * txt,
				      int txt_len);

typedef SAP_RAW (* ERR_CONV_PTR) (SAP_RAW c);

/*--------------------------------------------------------------------*/
/* Prototypen                                                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* ErrSetMyLocation                                                   */
/*                                                                    */
/* sets a pointer to own location string                              */
/*                                                                    */
/* INPUT   location      location string                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               pointer set                                  */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF_C ErrSetMyLocation (const SAP_CHAR * location,
				     ...);

/*--------------------------------------------------------------------*/
/* ErrGetMyLocation                                                   */
/*                                                                    */
/* gets a pointer to own location string                              */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* Pointer               pointer to location string                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAP_CHAR * API_DF ErrGetMyLocation (void);

/*--------------------------------------------------------------------*/
/* ErrSet / ErrSetSys                                                 */
/*                                                                    */
/* Save error information                                             */
/*                                                                    */
/* INPUT   component     name of the component, which reports the     */
/*			 error                                        */
/*         version       internal version of that component           */
/*         modul         .c modul, which reports the error            */
/*         line          line of the .c modul                         */
/*         write_log (2) determines whether a sap system log entry    */
/*                       is written or not                            */
/*         descr (1)     general error description                    */
/*         rc            return code                                  */
/*         detail (1)    detailed error description                   */
/*         system_call (2) failed system call                         */
/*         errno (2)     errno                                        */
/*         ...           variable parameter list according to format  */
/*                       strings descr and detail                     */
/*                                                                    */
/* (1) format string, may include %s, %d and so on                    */
/* (2) parameters for function ErrSetSys only                         */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               Error information saved                      */
/* ERR_INFO_TOO_LONG     Error information too long (max length: 500  */
/*                       bytes)                                       */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF_C ErrSet (const SAP_CHAR * component,
			   const int version,
			   const SAP_CHAR * module,
			   const int line,
			   const SAP_CHAR *descr,
			   const int rc,
			   const SAP_CHAR * detail,
			   ...);

SAPRETURN API_DF_C ErrSetSys (const SAP_CHAR  *component,
			      const int        version,
			      const SAP_CHAR  *module,
			      const int        line,
			      const SAP_BOOL   write_log,
			      const SAP_CHAR  *descr,
			      const SAPRETURN  rc,
			      const SAP_CHAR  *detail,
			      const SAP_CHAR  *system_call,
			      const int        os_errno,
			      ...);

/*--------------------------------------------------------------------*/
/* ErrGet / ErrGetL                                                   */
/*                                                                    */
/* ErrGet retrieves a saved error information. If no error information*/
/* has been saved before, a output pointers points to a empty string  */
/* and ErrGet returns ERR_NO_INFO_FOUND.                              */
/*                                                                    */
/* ErrGetL works like ErrGet but tries to translate the fields        */
/* descr and detail into the specified language. To do this, ErrGetL  */
/* reads the Err-Language file.                                       */
/*                                                                    */
/* INPUT   language (1)  language for the fields descr and detail     */
/*                                                                    */
/* OUTPUT  component     name of the component, which reported the    */
/*                       error                                        */
/*         version       internal verion of that component            */
/*         module        .c modul, which reported the error           */
/*         line          line of the .c modul                         */
/*         descr         general error description                    */
/*         rc            return code                                  */
/*         detail        detailed error description                   */
/*         system_call   failed system call                           */
/*         errno_str     errno in string format                       */
/*         location      error location                               */
/*         time          time when the error occured                  */
/*         release       SAP release                                  */
/*         errno_txt     errno text                                   */
/*         err_count     error counter                                */
/*                                                                    */
/* (1) parameters for function ErrGetL only                           */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               error information retrieved                  */
/* ERR_NO_INFO_FOUND     no error information found                   */
/* ERR_BAD_EYE_CATCHER   eye catcher destroyed                        */
/* ERR_INCOMPLETE        incomplete error information                 */
/* ERR_BAD_LENGTH        bad length of error information              */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrGet (SAP_CHAR ** component,
		         SAP_CHAR ** version,
		         SAP_CHAR ** module,
		         SAP_CHAR ** line,
		         SAP_CHAR ** descr,
                         SAP_CHAR ** rc,
		         SAP_CHAR ** detail,
		         SAP_CHAR ** system_call,
		         SAP_CHAR ** errno_str,
		         SAP_CHAR ** location,
		         SAP_CHAR ** time,
		         SAP_CHAR ** release,
		         SAP_CHAR ** errno_txt,
		         SAP_CHAR ** err_count);

SAPRETURN API_DF ErrGetL (SAP_CHAR language,
		          SAP_CHAR ** component,
		          SAP_CHAR ** version,
		          SAP_CHAR ** module,
		          SAP_CHAR ** line,
		          SAP_CHAR ** descr,
                          SAP_CHAR ** rc,
		          SAP_CHAR ** detail,
		          SAP_CHAR ** system_call,
		          SAP_CHAR ** errno_str,
		          SAP_CHAR ** location,
		          SAP_CHAR ** time,
		          SAP_CHAR ** release,
		          SAP_CHAR ** errno_txt,
		          SAP_CHAR ** err_count);

/*--------------------------------------------------------------------*/
/* ErrGetFld / ErrGetFldL                                             */
/*                                                                    */
/* retrieves a single field from the error info                       */
/*                                                                    */
/* ErrGetFldL works like ErrGetFld but tries to translate the field   */
/* into the specified language if field descr or detail is retrieved. */
/* To do this, ErrGetFldL reads the Err-Language file.                */
/*                                                                    */
/*                                                                    */
/* INPUT   field         field identification                         */
/*         language (1)  language                                     */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* (1) parameters for function ErrGetFldL only                        */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* Pointer               Pointer to selected field                    */
/* empty string          No error information found                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAP_CHAR * API_DF ErrGetFld (int field);
SAP_CHAR * API_DF ErrGetFldL (int field, 
		              SAP_CHAR language);

/*--------------------------------------------------------------------*/
/* ErrTrace                                                           */
/*                                                                    */
/* Write error information into dev-trace.                            */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               trace written                                */
/* ERR_NO_INFO_FOUND     no error information found                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrTrace (FILE * fp);

/*--------------------------------------------------------------------*/
/* ErrGetAttr                                                         */
/*                                                                    */
/* retrieves the attributes of the saved error information            */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT  adr           address of the saved error information       */
/*         len           length of the saved error information        */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               attributes retrieved                         */
/* ERR_NO_INFO_FOUND     no error information found                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrGetAttr (SAP_CHAR ** adr,
		             SAP_UINT * len);

/*--------------------------------------------------------------------*/
/* ErrCpTo                                                            */
/*                                                                    */
/* copies a external error information into the internal error        */
/* information area. Make sure, that adr is a valid error information.*/
/* This function may be needed, to send error informations from one   */
/* process to another one.                                            */
/*                                                                    */
/* INPUT   adr           address of the external error information    */
/*         len           length of the external error information     */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               external error information copied            */
/* SAP_ERR_INFO_LN       external error information too long          */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*                                                                    */
/* ErrCpNetToInt                                                      */
/* Like ErrCpTo, but converts it from net format (ASCII) if necessary */
/*                                                                    */
/* INPUT   net_err       address of the external error information    */
/*         net_err_len   length of the external error information     */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               external error information copied            */
/* SAP_ERR_INFO_LN       external error information too long          */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrCpTo       ( SAP_UC   * adr,
                                 SAP_UINT_U len);

SAPRETURN API_DF ErrCpNetToInt ( void     * net_err,
                                 SAP_UINT_R net_err_len);

/*--------------------------------------------------------------------*/
/* ErrCpFrom / ErrCpIntToNet / ErrCpIntToNet2                         */
/*                                                                    */
/* ErrCpFrom                                                          */
/* copies the internal error information to external storage          */
/*                                                                    */
/* ErrCpIntToNet                                                      */
/* copies the internal error information to external storage in       */
/* external format                                                    */
/*                                                                    */
/* ErrCpIntToNet2                                                     */
/* like ErrCpIntToNet but with a given error information              */
/*                                                                    */
/* INPUT   adr           address of the external storage              */
/*         len           length of the external storage               */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               error information copied                     */
/* ERR_INFO_TOO_LONG     Error information too long                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*                                                                    */
/* ErrCpIntToNet                                                      */
/* copies the internal error information to external storage and      */
/* converts it to net format (ASCII) if necessary                     */
/*                                                                    */
/* INPUT   net_err         address of the external storage            */
/*         net_err_len     pointer where ErrCpIntToNet will store the */
/*                         length of the net err. If Null, no length  */
/*                         will be stored                             */
/*         net_err_max_len length of external storage                 */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               error information copied                     */
/* ERR_INFO_TOO_LONG     Error information too long                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrCpFrom     ( SAP_UC     * adr,
                                 SAP_UINT_U   len);

SAPRETURN API_DF ErrCpIntToNet ( void       * net_err,
		                 SAP_UINT_R * net_err_len,
		                 SAP_UINT_R   net_err_max_len);
SAPRETURN API_DF ErrCpIntToNet2 (void       * net_err,
		                 SAP_UINT_R * net_err_len,
		                 SAP_UINT_R   net_err_max_len,
				 void       * err,
				 SAP_UINT_U   err_len);

/*--------------------------------------------------------------------*/
/* ErrReplace                                                         */
/*                                                                    */
/* replace a field of the error information                           */
/*                                                                    */
/* INPUT   field         field id, to be replaced (should be one of   */
/*                       the constants ERR_DESCR, ERR_RC and so on.   */
/*         value         new field value. Except the field rc and     */
/*                       errno value must be a string. If the fields  */
/*                       rc or errno should be replaced, value must   */
/*                       be a int.                                    */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               field value replaced                         */
/* ERR_NO_INFO_FOUND     no error information found                   */
/* ERR_INFO_TOO_LONG     Error information too long                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF_C ErrReplace (int field,
			       ...);

/*--------------------------------------------------------------------*/
/* ErrIsAInfo                                                         */
/*                                                                    */
/* checks, whether error information has been saved or not            */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* TRUE                  error informatin has been saved              */
/* FALSE                 no error information found                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAP_BOOL  API_DF ErrIsAInfo (void);

/*--------------------------------------------------------------------*/
/* ErrClear                                                           */
/*                                                                    */
/* clear error information                                            */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               error information cleared                    */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrClear (void);

/*--------------------------------------------------------------------*/
/* ErrFree                                                            */
/*                                                                    */
/* release all error resources allocated for this thread              */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               error resources freed                        */
/*                                                                    */
/*--------------------------------------------------------------------*/

void API_DF ErrFree (void * data);

/*--------------------------------------------------------------------*/
/* ErrProExit                                                         */
/*                                                                    */
/* release the Tls using by the errlib                                */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/*--------------------------------------------------------------------*/

void API_DF ErrProExit (void);

/*--------------------------------------------------------------------*/
/* ErrSetLength                                                       */
/*                                                                    */
/* set length of error information. This function should be used      */
/* with caution.                                                      */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               length set                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrSetLength (SAP_UINT len);

/*--------------------------------------------------------------------*/
/* ErrPr / ErrPrL                                                     */
/*                                                                    */
/* converts the error information to a string and returns a pointer   */
/* to that string.                                                    */
/*                                                                    */
/* ErrPrL works like ErrPr but tries to translate the fields          */
/* descr and detail into the specified language. To do this, ErrPrL   */
/* reads the Err-Language file.                                       */
/*                                                                    */
/* ErrPrL works like ErrPr, but converts the error                    */
/*                                                                    */
/* INPUT   language (1)  language                                     */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* (1) parameters for function ErrPrL only                            */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* ""                    if no error information available            */
/* Error String          if error informatin available                */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAP_CHAR * API_DF ErrPr (void);
SAP_CHAR * API_DF ErrPrL (SAP_CHAR language);

/*--------------------------------------------------------------------*/
/* ErrPrErrno  (NICHT thread safe - nicht mehr benutzen)              */
/* ErrPrErrno2 (thread safe Version)                                  */
/*                                                                    */
/* retrieves the error text to a given errno number                   */
/*                                                                    */
/* INPUT   errno         errno                                        */
/*         maxlen        Groesse von buf (sizeofU(buf))               */
/*                                                                    */
/* OUTPUT                                                             */
/*         buf           Ausgabepuffer (nach Aufruf NULL-terminiert)  */
/*                                                                    */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* ""                    if no error text found                       */
/* Error Text            if error text found                          */
/*                                                                    */
/*--------------------------------------------------------------------*/

/* !!! ErrPrErrno() nicht mehr benutzen, weil nicht thread-safe!!! */
SAP_CHAR * API_DF ErrPrErrno (int loc_errno);

SAP_CHAR * API_DF ErrPrErrno2 (int loc_errno, SAP_UC *buf, SAP_INT maxlen);

/*--------------------------------------------------------------------*/
/* ErrSetMsgFile                                                      */
/*                                                                    */
/* sets the name of the error language file                           */
/*                                                                    */
/* INPUT   file_name     name of the error language file              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               file name saved                              */
/* ERR_MALLOC_FAILED     no space to save the file name               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrSetMsgFile (const SAP_CHAR * file_name);

/*--------------------------------------------------------------------*/
/* ErrSetGetTxt                                                       */
/*                                                                    */
/* defines the function to read the language depended messages        */
/*                                                                    */
/* INPUT   func pointer  pointer to function, which reads the         */
/*                       language depended messages                   */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               pointer saved                                */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrSetGetTxt (const ERR_GET_TXT_PTR f);

/*--------------------------------------------------------------------*/
/* ErrSetMsgKey                                                       */
/*                                                                    */
/* defines the key length for language depended messages              */
/*                                                                    */
/* INPUT   key_len       key length                                   */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_O_K               key length set                               */
/* ERR_BAD_LENGTH        bad key length                               */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAPRETURN API_DF ErrSetMsgKey (SAP_USHORT key_len);

/*--------------------------------------------------------------------*/
/* ErrVersion                                                         */
/*                                                                    */
/* returns the err api's version.                                     */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT                                                             */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* Version of err api                                                 */
/*                                                                    */
/*--------------------------------------------------------------------*/

SAP_INT API_DF ErrVersion (void);

/*--------------------------------------------------------------------*/
/* ErrSapDllRelease                                                   */
/*                                                                    */
/* returns sap release of dptrcerr.dll (used unter windows)           */
/*                                                                    */
/* INPUT                                                              */
/*                                                                    */
/* OUTPUT  SAP_RELEASE of dptrcerr.dll                                */
/*                                                                    */
/* RETURN                                                             */
/*                                                                    */
/* SAP_RELEASE of dptrcerr.dll                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/

void API_DF ErrSapDllRelease (SAP_UC *, int);

END_externC

#endif /* SAPERR_H */
