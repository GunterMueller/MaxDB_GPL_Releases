/*!
  @file           veo964.cpp
  @author         RaymondR
  @special area   ping
  @brief          Ping to vserver / niserver
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdio.h>
#include "gsp00.h"
#include "gsp01.h"
#include "geo03.h"
#include "heo03.h"
#include "hsp100.h"
#include "RunTime/Security/RTESec_SAPSSL.h"
#include "RunTime/Security/RTESec_SSL.h"
#include "SAPDBCommon/SAPDB_StandardArgumentParser.hpp"
#include "RunTime/System/RTESys_MicroTime.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define OPTION_MESSAGE

#if defined (_WIN32)
#define     DELIMETER   "\\"
#else
#define     DELIMETER   "/"
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class  PingArgParser : public SAPDB_StandardArgumentParser
{
public:
    PingArgParser ( const SAPDB_Int4            argc,
                    const RTE_ArgChar* const*   argv,
                    const SAPDB_UInt4           sendBufferSize,
                    const SAPDB_UInt1           maximumHops,
                    const SAPDB_UInt4           numOfQueries,
                    const SAPDB_Bool            silent );

    enum { optID_Help,
           optID_HostName,
           optID_ServerDB,
           optID_NumOfQueries,
           optID_SendBufferSize,
           optID_Silent,
           optID_MaximumHops,
           optID_CaptureCertificate,
           optID_ImportCertificate };

    char const * const  HostName()  const          { return m_HostName; }
    char const * const  ServerDB()  const          { return m_ServerDB; } 
    SAPDB_UInt4         SendBufferSize() const     { return m_SendBufferSize; } 
    SAPDB_UInt1         MaximumHops() const        { return m_MaximumHops; } 
    SAPDB_UInt4         NumOfQueries() const       { return m_NumOfQueries; }
    SAPDB_Bool          Silent() const             { return m_Silent; }
    SAPDB_Bool          CaptureCertificate() const { return m_CaptureCertificate; }
    SAPDB_Bool          ImportCertificate () const { return m_ImportCertificate; }
        
protected:
    virtual SAPDB_Bool HandleNextOption  ( const SAPDB_Int4       optionID,
                                           const RTE_ArgChar*     optionArgument = 0 );

    virtual SAPDB_Bool ArgumentWithoutOption( const SAPDB_Int4       argIndex,
                                              const RTE_ArgChar*     argument );

    virtual SAPDB_Bool ParseFinished( const RTE_ArgChar* const* argvRemaining,
                                      SAPDB_Int4                argsRemaining,
                                      SAPDB_Int4                argsParsed,
                                      SAPDB_Bool                breakOptionFound );

    virtual SAPDB_Bool OptionHelpRequest( const SAPDB_Int4        optionID );

private:
    RTE_ArgChar          m_HostName[256];
    tsp00_DbNamec        m_ServerDB;
    SAPDB_UInt4          m_SendBufferSize;
    SAPDB_UInt1          m_MaximumHops;
    SAPDB_UInt4          m_NumOfQueries;
    SAPDB_Bool           m_Silent;
    SAPDB_Bool           m_CaptureCertificate;
    SAPDB_Bool           m_ImportCertificate;
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static PingArgParser::OptionDesc optionDesc[] = 
       {{ PingArgParser::optID_HostName,           RTE_ArgCharT('n'), RTE_ArgCharT("Host"),           OptionHasAnArgument      | OptionIsOptional, RTE_ArgCharT("<HostName>"),         RTE_ArgCharT("Host name/saprouter string.") },
        { PingArgParser::optID_ServerDB,           RTE_ArgCharT('d'), RTE_ArgCharT("ServerDB"),       OptionHasAnArgument      | OptionIsOptional, RTE_ArgCharT("<ServerDB>"),         RTE_ArgCharT("Name of the database.") },
        { PingArgParser::optID_SendBufferSize,     RTE_ArgCharT('b'), RTE_ArgCharT("SendBufferSize"), OptionHasAnArgument      | OptionIsOptional, RTE_ArgCharT("<Count>"),            RTE_ArgCharT("Size of the send buffer in bytes (min 512).") },
        { PingArgParser::optID_MaximumHops,        RTE_ArgCharT('H'), RTE_ArgCharT("MaxHops"),        OptionHasAnArgument      | OptionIsOptional, RTE_ArgCharT("<MaximumHops>"),      RTE_ArgCharT("Maximum number of hops on the way to the target.") },
        { PingArgParser::optID_NumOfQueries,       RTE_ArgCharT('q'), RTE_ArgCharT("Queries"),        OptionHasAnArgument      | OptionIsOptional, RTE_ArgCharT("<NumOfQueries>"),     RTE_ArgCharT("Number of queries per hop.") },
        { PingArgParser::optID_CaptureCertificate, RTE_ArgCharT('c'), RTE_ArgCharT("Capture"),        OptionIsOptional, 0,                                                             RTE_ArgCharT("Capture server certificate.") },
        { PingArgParser::optID_ImportCertificate,  RTE_ArgCharT('i'), RTE_ArgCharT("Import"),         OptionIsOptional, 0,                                                             RTE_ArgCharT("Import server certificate.") },
        { PingArgParser::optID_Silent,             RTE_ArgCharT('S'), RTE_ArgCharT("Silent"),         OptionIsOptional, 0,                                                             RTE_ArgCharT("Silent mode.") },
        { PingArgParser::optID_Help,               RTE_ArgCharT('h'), RTE_ArgCharT("Help"),           OptionArgumentIsOptional | OptionIsOptional, RTE_ArgCharT("<Option> | LONG"),    RTE_ArgCharT("Help, -h LONG shows long options.") },
};

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

PingArgParser::PingArgParser ( const SAPDB_Int4            argc,
                               const RTE_ArgChar* const*   argv,
                               const SAPDB_UInt4           sendBufferSize,
                               const SAPDB_UInt1           maximumHops,
                               const SAPDB_UInt4           numOfQueries,
                               const SAPDB_Bool            silent )

              : m_SendBufferSize(sendBufferSize),
                m_MaximumHops(maximumHops),
                m_NumOfQueries(numOfQueries),
                m_Silent(silent),
                m_CaptureCertificate(false),
                m_ImportCertificate(false),
                SAPDB_StandardArgumentParser ( argc, argv, PingArgParser::optID_Help, optionDesc, 
                                               NUM_OF_OPTIONS_DESC(optionDesc), false, false,  
                                               false ) // An automatic check not possible
                                                     // because we allow the first argument to be
                                                     // the host name ( Refer to 'ArgumentWithoutOption'
                                                     // and 'ParseFinished' )
{ 
    m_HostName[0] = '\0';
    m_ServerDB[0] = '\0';
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool PingArgParser::HandleNextOption ( const SAPDB_Int4       optionID,
                                             const RTE_ArgChar*     optionArgument )
{
  SAPDB_UInt4  Tmp;
  SAPDB_Bool   ok = true;

  switch (optionID)
  {
  case optID_HostName:
      strncpy ( m_HostName, optionArgument, sizeof(m_HostName) - 1 );
      m_HostName[sizeof(m_HostName) - 1] = '\0';
      break;

  case optID_CaptureCertificate:
      m_CaptureCertificate = true;
      break;

  case optID_ImportCertificate:
      m_ImportCertificate = true;
      break;

  case optID_Silent:
      m_Silent = true;
      break;

  case optID_ServerDB:
      strncpy ( m_ServerDB, optionArgument, sizeof(m_ServerDB) - 1 );
      m_ServerDB[sizeof(m_ServerDB) - 1] = '\0';
      break;

  case optID_SendBufferSize:
      if (( false == ArgumentToUInt ( optionArgument, m_SendBufferSize )) || 
          ( 512   >  m_SendBufferSize ))
      {
          printf ( "Invalid buffer size '%s' (min: 512)\n", optionArgument );
          ok = false;
      }
      break;

  case optID_MaximumHops:
      if (( false == ArgumentToUInt ( optionArgument, Tmp )) ||
          ( 255  < Tmp ))
      {
          printf ( "Invalid number of maximum hops '%s' (max: 255)\n", optionArgument );
          ok = false;
      }
      else
        m_MaximumHops = (SAPDB_UInt1)Tmp;
      break;

  case optID_NumOfQueries:
      if ( false == ArgumentToUInt ( optionArgument, m_NumOfQueries ))
      {
          printf ( "Invalid number of queries '%s'\n", optionArgument );
          ok = false;
      }
      break;

  default:
      printf ( "Unknown option identfier '%d'\n", optionID  );
      ok = false;
      break;
  }

  return ok;
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool PingArgParser::ArgumentWithoutOption( const SAPDB_Int4       argIndex,
                                                 const RTE_ArgChar*     argument )
{
  // The first argument might be the host name if the old
  // argument list is used "x_ping <hostname>"
  if ( argIndex == 1 )
  {
      strncpy ( m_HostName, argument, sizeof(m_HostName) - 1 );
      m_HostName[sizeof(m_HostName) - 1] = '\0';
      return true;
  }
  return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  PingArgParser::OptionHelpRequest( const SAPDB_Int4        optionID )
{
    switch (optionID)
    {
    case optID_NumOfQueries:
        printf (" \nNumber of queries:\n\n");
        printf (" Specifies the number of ping requests sent to destination.\n\n Default: %d\n", m_NumOfQueries );
        break;

    case optID_SendBufferSize:
        printf (" \nSend buffer size:\n\n");
        printf (" Specifies the amount of data that will be send.\n\n Default: %d\n", m_SendBufferSize );
        break;

    case optID_Silent:
        printf (" \nSilent mode:\n\n");
        printf (" This option hides the server reply information.\n" );
        printf (" Only the statisic information will be displayed.\n\n Default: %s\n", m_Silent ? "On" : "Off"  );
        break;

    case optID_MaximumHops:
        printf (" \nMaximum number of hops:\n\n");
        printf (" Specifies the maximum number of hops in the path to search for the target.\n");
        printf (" A hop value 0 will be replied by the first transition (e.g 'XSERVER').\n\n Default: %d \n", m_MaximumHops );
        break;

    case optID_CaptureCertificate:
        printf (" \nCapture server certificate:\n\n");
        printf (" The public key of the server certificate is written to the file:\n\n");
        printf ("   <IndepData>%s%s%s%s\n", DELIMETER, SECURITY_DIRECTORY, DELIMETER, CAPTURED_SERVER_CERT_FILENAME);
        printf ("\n The certificate container is not updated by this option. Use the\n import server certificate option instead.\n\n");
        break;

    case optID_ImportCertificate:
        printf (" \nImport server certificate:\n\n");
        printf (" The public key of the server certificate is imported to the\n anonymous client certificate container:\n\n");
        printf ("   <IndepData>%s%s%s%s\n", DELIMETER, SECURITY_DIRECTORY, DELIMETER, ANONYMOUS_PSE_FILE_NAME);
        break;
    default:
        return false;
    }
    
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool PingArgParser::ParseFinished( const RTE_ArgChar* const* argvRemaining,
                                         SAPDB_Int4                argsRemaining,
                                         SAPDB_Int4                argsParsed,
                                         SAPDB_Bool                breakOptionFound ) 
{ 
    // We have to check the if the host name or database name has specified because
    // 'handleMissingNoneOptionalOptions' was set to false.
    if (( 0 < argsRemaining ) || ( 0 == argsParsed ) || 
        (( '\0' == m_HostName[0] ) && ( '\0' == m_ServerDB[0] )))
    {
        Help();
        return false;
    }
    if  (( '\0' != m_ServerDB[0] ) && ( m_CaptureCertificate || m_ImportCertificate ))
    {
        printf ( "\nInvalid option combination\n" ) ;
        return false;
    }

    return true;
};

/*---------------------------------------------------------------------------*/

#define MAX_LINE_LEN 80

static void ShowAndRemoveNiErrorTrace (int ShowBeforeRemove )
{
  tsp00_Pathc NiErrorTrace ;

  eo03NiBuildTracefileName  (NiErrorTrace) ;

  if ( ShowBeforeRemove )
  {
    FILE *stream = fopen ( NiErrorTrace, "r" ) ;
    if ( stream != (FILE *) NULL )
    { 
        char line[ MAX_LINE_LEN + 1 ] ;
        bool ShowOk ;
        do
          if ( (ShowOk = fgets ( line, MAX_LINE_LEN , stream ) != NULL) )
            printf ("%s", line ) ;
        while ( ShowOk ) ;
        fclose ( stream ) ;
    }
  else
    { perror ( "open error" ) ;
      printf ( "missing NiErrorTrace  >%s<", (char *) NiErrorTrace ) ;
    }
  }

  /* Remove Tracefile */
  unlink(NiErrorTrace);

  return ;
}

/*---------------------------------------------------------------------------*/

static int DBPing ( tsp00_Int4                         connection,
                    char*                              szServerNode,
                    char*                              szServerDB,
                    tsp00_Uint4                        queries,
                    tsp00_Uint4                        sendBufferSize,
                    tsp00_Uint1                        maxHops,
                    SAPDB_Bool                         silent,
                    tsp00_ErrTextc VAR_ARRAY_REF       ErrTextC )
{
    int               rc        = 0;
    SAPDB_UInt8       time      = 0;
    SAPDB_UInt8       sumTime   = 0;
    SAPDB_UInt8       startTime = 0;
    SAPDB_UInt8       stopTime  = 0;
    SAPDB_UInt8       avgTime   = 0;
    SAPDB_UInt8       maxTime   = 0;
    SAPDB_UInt8       minTime   = (tsp00_Uint4)-1;
    tsp00_Uint1       hops;
    tsp00_Uint4       loopCnt   = 0;
    tsp00_Uint1       hopCnt;
    tsp00_DbNamec     server;
    tsp00_Versionc    serverVersion;

    ErrTextC[0] = '\0';

    if ( szServerNode[0] == '\0' )
        printf("\nPinging %s", szServerDB );
    else
        printf("\nPinging %s on %s", szServerDB, szServerNode );

    RTESys_InitMicroSecTimer();

    hops = hopCnt = 0; // start with one hop

    if ( SqlDBPing ( connection, sendBufferSize, hops, server, serverVersion, ErrTextC ) == commErrOk_esp01 )
    {
        printf(" with %d bytes of data over a maximum of %d hops.", sendBufferSize, maxHops );

        if ( false == silent )
        {
            printf("\n\nHop  Server\n%2d   %s\n", hopCnt, (char*)server );

            while( !hops )
            {
                hops = ++hopCnt;

                if ( SqlDBPing ( connection, sendBufferSize, hops, server, serverVersion, ErrTextC ) != commErrOk_esp01)
                    break;

                printf(" %d   %s\n", hopCnt, (char*)server );
            }

            printf("\n", sendBufferSize );
        }

        do 
        {
            hops      = maxHops;
            startTime = RTESys_MicroSecTimer();

            if ( SqlDBPing ( connection, sendBufferSize, hops, server, serverVersion, ErrTextC ) == commErrOk_esp01 )
            {
                stopTime  = RTESys_MicroSecTimer();
                time      = stopTime - startTime;
                sumTime  += time;

                if ( time < minTime )
                    minTime = time;

                if ( time > maxTime )
                    maxTime = time;

                if ( false == silent )
                {
                    if ( time < 10000 )
                        printf( "%s: reply time=%-3dus\n", (char *)server, (tsp00_Uint4)time );
                    else
                        printf( "%s: reply time=%-3dms\n", (char *)server, (tsp00_Uint4)(time / 1000) );
                }
            }
            else
                rc = 1;
        }
        while (( rc == 0 ) && ( ++loopCnt < queries ));
    }
    else
    {
        printf("\n\n");
        rc = 1;
    }

    if ( rc == 0 )
    {
        avgTime = sumTime / loopCnt;

        printf( "\n\n%s: '%s'\n\n", (char *)server, 
                serverVersion[0] != '\0' ? (char *)serverVersion : "UNKNOWN");

        printf( "Approximate round trip times: \n"
                "Minimum = %d%s, Maximum = %d%s, Average = %d%s\n\n",
                (tsp00_Uint4)(( minTime < 10000 ) ? minTime : minTime / 1000),
                ( minTime < 10000 ) ? "us" : "ms",
                (tsp00_Uint4)(( maxTime < 10000 ) ? maxTime : maxTime / 1000),
                ( maxTime < 10000 ) ? "us" : "ms",
                (tsp00_Uint4)(( avgTime < 10000 ) ? avgTime : avgTime / 1000),
                ( avgTime < 10000 ) ? "us" : "ms" );
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

static int XserverPing ( char*                          szServerNode,
                         tsp00_ErrTextc VAR_ARRAY_REF   ErrTextC  )
{
    int               rc;
    SAPDB_Byte        dummy[1];
    tsp00_Versionc    serverVersion;
    serverVersion[0] = '\0' ;

    printf("\nChecking connection to '%s' ... ", szServerNode );

    if ( SqlAPing ( szServerNode, false, serverVersion, dummy[0], sizeof (dummy), ErrTextC ) == commErrOk_esp01 )
    {
        printf("ok.\n\n");
        printf("Version of Server is\n");
        printf("----------------------------------------------------------\n");
        printf("%s\n", serverVersion[0] != '\0' ? (char *)serverVersion : "UNKNOWN");
        printf("----------------------------------------------------------\n");
        rc = 0 ;
    }
    else
        rc = 1;

    return rc;
}

/*---------------------------------------------------------------------------*/

static int CertRequest ( char*                          szServerNode,
                         SAPDB_Bool                     captureCertificate,
                         SAPDB_Bool                     importCertificate,
                         tsp00_ErrTextc VAR_ARRAY_REF   ErrTextC  )
{
    int                 rc = 0;
    SAPDB_Byte          certBuffer[MAX_CERTIFICATE_LENGTH];
    tsp00_Versionc      serverVersion;
    tsp01_CommErr_Enum  ulCommState;

    serverVersion[0] = '\0' ;

    printf("\nGet a certificate from '%s' ... ", szServerNode );

    if ( SqlAPing ( szServerNode, true, serverVersion, certBuffer[0], 
                    sizeof (certBuffer), ErrTextC ) == commErrOk_esp01 )
    {
        tsp00_Pathc     dummyTraceName;

        dummyTraceName.Init ();
        //Giving NI tracefile name can be omitted here because NI is already initialized by 'SqlAPing'
        ulCommState = RTESec_SSLUtilInit (dummyTraceName, ErrTextC );

        if ( commErrOk_esp01 == ulCommState )
        {
            if ( captureCertificate && ( commErrOk_esp01 == ulCommState ) )
            {
                ulCommState = RTESec_SAPSSLWriteCertificateToFile (&certBuffer[0], sizeof (certBuffer), ErrTextC);

                if ( commErrOk_esp01 == ulCommState )
                {
                    printf("\n\n----------------------------------------------------------------------------\n");
                    printf("\nPublic Key of Server Certificate:\n");

                    ulCommState = RTESec_SAPSSLShowCertificate ( &certBuffer[0], ErrTextC );

                    if ( commErrOk_esp01 != ulCommState )
                        return 1;

                    printf("\nCapture Public Key of Server Certificate to File \'%s\' succeeded\n", CAPTURED_SERVER_CERT_FILENAME);
                    printf("----------------------------------------------------------------------------\n");
                }
            }
            else //importCertificate
            {
                ulCommState = RTESec_SAPSSLImportCertificate ( &certBuffer[0], ErrTextC );

                if (commErrOk_esp01 == ulCommState)
                {
                    printf("\n\n----------------------------------------------------------------------------\n");
                    printf("\nPublic Key of Server Certificate:\n");

                    ulCommState = RTESec_SAPSSLShowCertificate (&certBuffer[0], ErrTextC);

                    if (commErrOk_esp01 != ulCommState)
                        return 1;

                    printf("\nImport Public Key of Server Certificate to Keystore %s succeeded\n", ANONYMOUS_PSE_FILE_NAME);
                    printf("----------------------------------------------------------------------------\n");
                }
            }
        }

        RTESec_SAPSSLUtilClose ();
    }
    else
        rc = 1;

    return rc;
}

/*-----------------------------------------------------------*/
int main ( int argc, char ** argv )
/*-----------------------------------------------------------*/
{
    int               rc = 0;
    tsp00_ErrTextc    ErrTextC; 
    tsp00_Int4        connection;

    ErrTextC[0] = '\0';

    PingArgParser   ArgParser( argc,argv, 
                               512,      // default buffer size
                               10,       // default maximum hops
                               5,        // default queries
                               false );  // we are not silent (true), we are loud (false)

    if ( ArgParser.StartParsing() )
    {
        if ( ArgParser.ServerDB()[0] != '\0' )
        {
            if ( SqlPingConnect ((char*)ArgParser.HostName(), (char*)ArgParser.ServerDB(), connection, ErrTextC ) == commErrOk_esp01 )
            {
                rc = DBPing ( connection, (char*)ArgParser.HostName(), (char*)ArgParser.ServerDB(),
                              ArgParser.NumOfQueries(), ArgParser.SendBufferSize(), 
                              ArgParser.MaximumHops(), ArgParser.Silent(), ErrTextC );
                if ( rc != 0 )
                {
                   printf("\n\nError - '%s'\n\n", (char *)ErrTextC );
                }

                if ( SqlIsSaprouterString ((char*)ArgParser.HostName()) ) 
                    ShowAndRemoveNiErrorTrace(rc);

                SqlPingRelease(connection);
            }
            else
            {
                   printf("\n\nError - '%s'\n\n", (char *)ErrTextC );
            }
       }
       else
       {
           if ( ArgParser.CaptureCertificate() ||  ArgParser.ImportCertificate() )
           {
               if ( rc = CertRequest ( (char*)ArgParser.HostName(),
                                       ArgParser.CaptureCertificate(),
                                       ArgParser.ImportCertificate(),
                                       ErrTextC  ) != 0 )
               {
                   printf("\n\nError - '%s'\n\n", (char *)ErrTextC );
                   rc = 1 ;
               }
           }
           else
           {
               if ( ( rc = XserverPing ( (char*)ArgParser.HostName(), ErrTextC )) != 0 )
               {
                   printf("ERROR\n\n");
                   printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
                   printf("Error: %s\n\n", (char *)ErrTextC );
                   printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                   rc = 1 ;
               }
           }

           if ( SqlIsSaprouterString ((char*)ArgParser.HostName()) ) 
               ShowAndRemoveNiErrorTrace(+rc);
       }
    }

    return (rc);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
