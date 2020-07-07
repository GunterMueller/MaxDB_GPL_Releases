/*!
  @file           RTEDiag_Appldiag.cpp
  @author         JoergM
  @ingroup        RunTime
  @brief          UNIX protocol file logging process spawning

  Used especially to start a logger process that allows several processes to log into a single log file

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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include "RunTime/System/RTESys_Time.h"
#include "RunTime/Diagnose/RTEDiag_Appldiag.h"
#include "hsp77.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
/*----------------------------------------------------*/

/*! QUIET=ON */
class BufferedLine
{
/*! Maximum size of buffer line */
#define SIZE_OF_BUFFER_IN_BYTES 4096

public:
    /*!
      @brief buffer line ctor
      @param fd [in] file descriptor to read from
     */
	BufferedLine(RTE_FileHandle fd);
    /*!
      @brief read a signle line
      @return pointer to the buffered line
     */
	SAPDB_Char *ReadLine();
    /*!
      @brief buffer line dtor
     */
	~BufferedLine() {}
private:
	RTE_FileHandle  m_fd;
	SAPDB_Char m_inputBuffer[SIZE_OF_BUFFER_IN_BYTES];
	SAPDB_Char m_outputBuffer[SIZE_OF_BUFFER_IN_BYTES];
	SAPDB_Char *m_unreadPosition;
	SAPDB_Int4  m_unreadCharCount;
};

BufferedLine::BufferedLine(RTE_FileHandle fd)
: m_fd(fd),
  m_unreadPosition(0),
  m_unreadCharCount(0)
{
}

SAPDB_Char *
BufferedLine::ReadLine()
{
	SAPDB_Char *resultLine = 0;
	SAPDB_Char *outputPosition = &m_outputBuffer[0];
    SAPDB_Int4  outputBytes = 0;

	if ( m_unreadCharCount > 0 )
	{
		SAPDB_Int4 iSearch;
		for ( iSearch = 0; iSearch < m_unreadCharCount; iSearch++ )
		{
			if ( '\n' == (*outputPosition++ = *m_unreadPosition++) )
			{
				*outputPosition = 0;
				m_unreadCharCount -= (iSearch+1);
				resultLine = &m_outputBuffer[0];
				break;
			}
		}
		if ( iSearch >= m_unreadCharCount )
		{
			outputBytes += m_unreadCharCount;
		}
	}

	/* more characters are needed ... */
	while ( 0 == resultLine )
	{
		SAPDB_Int4 charsRead = read(m_fd, m_inputBuffer, SIZE_OF_BUFFER_IN_BYTES-1);

		if ( ((charsRead < 0) && (errno != EINTR))
		   || (0 == charsRead) )
		{
			break;
		}

		if ( charsRead > 0 )
		{
			m_unreadCharCount = charsRead;
			m_unreadPosition = &m_inputBuffer[0];

			for ( SAPDB_Int4 iSearch = 0; iSearch < m_unreadCharCount; ++iSearch )
			{
				if ( '\n' == ( (*outputPosition++) = (*m_unreadPosition++) ) )
				{
					*outputPosition = 0;
					m_unreadCharCount -= (iSearch+1);
					resultLine = &m_outputBuffer[0];
					break;
				}

				if ( (++outputBytes) >= (SIZE_OF_BUFFER_IN_BYTES-1) )
				{
					*outputPosition = 0;
					m_unreadCharCount -= (iSearch+1);
					resultLine = &m_outputBuffer[0];
					break;
				}
			}
		}
	}

	return resultLine;
}


/*--------------------------------------------------------*/

/*! QUIET=ON */
class CycleBuffer
{
public:
    /*! internal output types */
	enum OutputType {
		OutputLine,
		OutputHeader,
		OutputCleanup,
		OutputEndOfStartup,
		OutputCurrentWrite
	};

    /*!
      @brief cycle buffer ctor
      @param fd [in] the file handle
      @param fileLimit [in] the maximum size of file in bytes
      @param formOutputLine [in] the function used for output buffer filling
     */
	CycleBuffer( RTE_FileHandle fd, 
		         RTE_FileOffset fileLimit,
                 const SAPDB_Char *(*formOutputLine)(OutputType outputType, SAPDB_Int4 *outLength, const SAPDB_Char *inputLine) )
		: m_fd(fd),
		  m_fileLimit(fileLimit),
		  m_formOutputLine(formOutputLine)
	{
		(void)(*formOutputLine)(OutputHeader, &m_headerLength, "");
		m_endOfStartupPosition = m_nextOutput = (RTE_FileOffset)m_headerLength;

		m_endOfStartup = (*formOutputLine)(OutputEndOfStartup, &m_endOfStartupLength, "");

		m_currentWrite = (*formOutputLine)(OutputCurrentWrite, &m_currentWriteLength, "");

		m_cleanLine = (*formOutputLine)(OutputCleanup, &m_cleanLineLength, "");
	}

    /*!
      @brief write an output line
      @param outputLine [in] buffer with output information
     */
	void WriteLine(const SAPDB_Char *outputLine);

private:
	RTE_FileHandle m_fd;
	RTE_FileOffset m_fileLimit;
	const SAPDB_Char *(*m_formOutputLine)(OutputType outputType, SAPDB_Int4 *outLength, const SAPDB_Char *inputLine);
	SAPDB_Int4         m_headerLength;
	const SAPDB_Char *m_endOfStartup;
	SAPDB_Int4         m_endOfStartupLength;
	const SAPDB_Char *m_currentWrite;
	SAPDB_Int4         m_currentWriteLength;
	RTE_FileOffset m_nextOutput;
	RTE_FileOffset m_endOfStartupPosition;
	const SAPDB_Char *m_cleanLine;
	SAPDB_Int4         m_cleanLineLength;
};

void CycleBuffer::WriteLine(const SAPDB_Char *outputLine)
{
	SAPDB_Int4 realOutputLength;
	const SAPDB_Char *realOutputLine = (*m_formOutputLine)(OutputLine, &realOutputLength, outputLine);

	if ( m_fileLimit > 0 )
	{
		if ( lseek(m_fd, m_nextOutput, SEEK_SET) < 0 )
		{
			perror("lseek(m_nextOutput):");
		}
		m_nextOutput += realOutputLength;

		if ( (m_nextOutput + m_currentWriteLength) > m_fileLimit )
		{
			SAPDB_Int4 lengthToFill = (SAPDB_Int4)(m_fileLimit + realOutputLength - m_nextOutput);
			while ( lengthToFill > 0 )
			{
				if ( (m_cleanLineLength-1) > lengthToFill )
				{
					if ( write(m_fd, m_cleanLine, lengthToFill) < 0 )
					{
						perror("write(m_cleanLine):");
					}
				}
				else
				{
					if ( write(m_fd, m_cleanLine, m_cleanLineLength-1) < 0 )
					{
						perror("write(m_cleanLine):");
					}
				}
				lengthToFill -= (m_cleanLineLength - 1);
			}
			if ( lseek(m_fd, m_endOfStartupPosition, SEEK_SET) < 0 )
			{
				perror("lseek(m_endOfStartupPosition):");
			}
			m_nextOutput = m_endOfStartupPosition + realOutputLength;
		}
	}

	if ( write(m_fd, realOutputLine, realOutputLength) < 0 )
	{
		perror("write(outputLine):");
	}
	if ( 0 == strncmp(realOutputLine, m_endOfStartup, m_endOfStartupLength) )
	{
		m_endOfStartupPosition = m_nextOutput;
	}

	if ( m_fileLimit > 0 )
	{
		if ( write(m_fd, m_currentWrite, m_currentWriteLength) < 0 )
		{
			perror("write(currentWrite):");
		}
	}
}

/*--------------------------*/

/*! QUIET=ON */
SAPDB_Int4 RTEDiag_SpawnLogger( 
	SAPDB_Char *argv[],
	SAPDB_Char *newTitle,
    RTE_FileHandle outputFd,
    RTE_FileOffset fileLimit,
	const SAPDB_Char *(*formOutputLine)(CycleBuffer::OutputType outputType, SAPDB_Int4 *outLength, const SAPDB_Char *inputLine),
    SAPDB_Char const *namedPipe = (SAPDB_Char const *)0)
{
	pid_t loggerPid;
	RTE_FileHandle   loggerFd;
	RTE_FileHandle   pipeDesc[2]; /* used only if pipeName == 0 */
	const SAPDB_Char *header;
	SAPDB_Int4   headerLength;

	if ( ftruncate(outputFd, fileLimit) < 0 )
	{
		perror("ftrucute failed:");
		return -1;
	}

	if ( lseek(outputFd, (RTE_FileOffset)0, SEEK_SET) < 0 )
	{
		perror("lseek failed:");
		return -1;
	}

	header = (*formOutputLine)(CycleBuffer::OutputHeader, &headerLength, "");
	if ( header )
	{
		if ( write(outputFd, header, headerLength) < 0 )
		{
			perror("write failed:");
			return -1;
		}
	}
	else
	{
		headerLength = 0;
	}

	/* Formatting logging output file */
	if ( fileLimit > 0 )
	{
		/* 80 characters:    12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
		const SAPDB_Char *cleanupLine;
		SAPDB_Int4         cleanupLineLength;

		cleanupLine = (*formOutputLine)(CycleBuffer::OutputCleanup, &cleanupLineLength, "");

		RTE_FileOffset currentOffset = headerLength;
		while ( currentOffset < fileLimit )
		{
			RTE_FileOffset remainingLength = fileLimit - currentOffset;
			if ( cleanupLineLength > remainingLength )
			{
				if ( write(outputFd, cleanupLine + cleanupLineLength - remainingLength, remainingLength) < 0 )
				{
					perror("restOfCleanupLine write failed:");
					return -1;
				}
			} 
			else
			{
				if ( write(outputFd, cleanupLine, cleanupLineLength) < 0 )
				{
					perror("cleanupLine write failed:");
					return -1;
				}
			}
			currentOffset += cleanupLineLength;
		}

		if ( lseek(outputFd, headerLength, SEEK_SET) < 0 )
		{
			perror("lseek failed:");
			return -1;
		}
	}

	loggerFd = dup(outputFd);
	if ( loggerFd < 0 )
	{
		perror("dup failed:");
		return -1;
	}

    if ( !namedPipe )
    {
	    if ( pipe(pipeDesc) < 0 )
	    {
		    perror("pipe failed:");
		    close(loggerFd);
		    return -1;
	    }
    }

	loggerPid = fork();

	if ( ((pid_t)-1) == loggerPid )
	{
		perror("fork failed:");
		close(loggerFd);

        if ( !namedPipe )
        {
            close(pipeDesc[0]);
    		close(pipeDesc[1]);
        }

        return -1;
	}

	if ( 0 == loggerPid )
	{
        SAPDB_Char *line;
        RTE_FileHandle inputPipeFd;

        if ( !namedPipe )
        {
    		close(pipeDesc[1]); /* Logger does not report back to starter */
            inputPipeFd = pipeDesc[0];
        }
        else
        {
            inputPipeFd = open(namedPipe, O_RDONLY);
        }

        if ( 0 != inputPipeFd && 0 != loggerFd )
        {
            close(0);
        }

        if ( 1 != inputPipeFd && 1 != loggerFd )
        {
            close(1);
        }

        if ( 1 < inputPipeFd )
        {
            inputPipeFd = dup(inputPipeFd);
        }

        if ( 1 < loggerFd )
        {
            loggerFd = dup(loggerFd);
        }

        if ( 2 != loggerFd )
        {
            dup2(loggerFd, 2);
        }

        for ( int fd = sysconf ( _SC_OPEN_MAX ) - 1; fd > 2; --fd )
        {
            close(fd);
        }

		CycleBuffer  outputFile(loggerFd, fileLimit, formOutputLine);
        if ( inputPipeFd < 0 )
        {
    		outputFile.WriteLine("FATAL: Logger process aborted since input pipe could not be opened.\n");
            exit(1);
        }

		BufferedLine inputLine(inputPipeFd);

		/* modify visible line in ps output... */
		int argv0Len = (int)strlen(argv[0]);
		SAPDB_MemFillNoCheck(argv[0], 0, argv0Len);

		if ( strlen(newTitle) < argv0Len )
		{
			strcpy(argv[0], newTitle);
		}
		else
		{
			SAPDB_MemCopyNoCheck(argv[0], newTitle, argv0Len);
		}

		for ( int iarg = 1; argv[iarg] != 0; iarg++)
		{
			SAPDB_MemFillNoCheck( argv[iarg], 0, strlen(argv[iarg]));
		}

        while ( (line = inputLine.ReadLine()) != 0 )
		{
			outputFile.WriteLine(line);
		}

        close(loggerFd);
    	close(inputPipeFd);
        if ( namedPipe )
        {
            (void)unlink(namedPipe);
        }
        exit(0);
		return 0;
	}

	/* log producing process */

	close(loggerFd);

    RTE_FileHandle outputPipeFd;

    if ( !namedPipe )
    {
        (void)close(pipeDesc[0]); /* never read from logger */
	    outputPipeFd = pipeDesc[1];
    }
    else
    {
        outputPipeFd = open(namedPipe, O_WRONLY);
    }

    return dup2(outputPipeFd, outputFd);
}

/*----------------------*/

/*! QUIET=ON */
static const SAPDB_Char *RTEDiag_AppldiagTimestampOutputLine(CycleBuffer::OutputType outputType, SAPDB_Int4 *outLength, const SAPDB_Char *inputLine)
{
	const SAPDB_Char *outputLine;
	static SAPDB_Char outputBuffer[SIZE_OF_BUFFER_IN_BYTES+6];

	switch(outputType)
	{
	case CycleBuffer::OutputLine:
		if ( inputLine )
		{
			if ( strncmp("----", inputLine, 4) == 0
			  || ( strlen(inputLine) > 46
			  && '-' == inputLine[4]
			  && '-' == inputLine[7]
			  && ' ' == inputLine[10]
		      && ':' == inputLine[13]
			  && ':' == inputLine[16]
			  && ' ' == inputLine[19] ) )
			{
				outputLine = inputLine;
			}
			else
			{
				RTESys_Timestamp timeBuffer;
    	        sp77sprintf ( outputBuffer, sizeof(outputBuffer),
			              "%s     0    0              %s",
                           RTESys_FillProtocolTimeStamp(timeBuffer),
			               inputLine );
				outputLine = outputBuffer;
			}
		}
		else
		{
			outputLine = "";
		}
		break;
	case CycleBuffer::OutputHeader:
		outputLine = "--------------------------------------------------------------------------------\n"
                     "Date       Time       PID Typ MsgID  Label    Message-Text\n"
					 "--------------------------------------------------------------------------------\n";
		break;
	case CycleBuffer::OutputCleanup:
		outputLine = "                                                                                \n";
		break;
    case CycleBuffer::OutputEndOfStartup:
		outputLine = "--------------------------------- end of startup -------------------------------\n";
		break;
	case CycleBuffer::OutputCurrentWrite:
		outputLine = "------------------------------ current write position --------------------------";
		break;
	default:
		outputLine = "";
	}

	if ( outLength )
	{
		*outLength = (SAPDB_Int4)strlen(outputLine);
	}
	return outputLine;
}

/*! QUIET=ON */
static const SAPDB_Char *RTEDiag_AppldiagSimpleOutputLine(CycleBuffer::OutputType outputType, SAPDB_Int4 *outLength, const SAPDB_Char *inputLine)
{
	const SAPDB_Char *outputLine;
	static SAPDB_Char outputBuffer[SIZE_OF_BUFFER_IN_BYTES+6];

	switch(outputType)
	{
	case CycleBuffer::OutputLine:
		if ( inputLine )
		{
			outputLine = inputLine;
		}
		else
		{
			outputLine = "";
		}
		break;
	case CycleBuffer::OutputHeader:
		outputLine = "--------------------------------------------------------------------------------\n"
                     "Message-Text\n"
					 "--------------------------------------------------------------------------------\n";
		break;
	case CycleBuffer::OutputCleanup:
		outputLine = "                                                                                \n";
		break;
    case CycleBuffer::OutputEndOfStartup:
		outputLine = "--------------------------------- end of startup -------------------------------\n";
		break;
	case CycleBuffer::OutputCurrentWrite:
		outputLine = "------------------------------ current write position --------------------------";
		break;
	default:
		outputLine = "";
	}

	if ( outLength )
	{
		*outLength = (SAPDB_Int4)strlen(outputLine);
	}
	return outputLine;
}

/*! QUIET=ON */
extern "C" SAPDB_Int4 RTEDiag_AppldiagLogger( 
	SAPDB_Char *argv[],
	SAPDB_Char *newTitle,
    SAPDB_Int4 outputFd, /* copy of sql22_fd */
	SAPDB_Int4 useTimestamp, /* copy of sql22_time_stamp */
    RTE_FileOffset fileLimit) /* new parameter */
{
	if ( useTimestamp )
	{
		return RTEDiag_SpawnLogger( argv, newTitle, outputFd, fileLimit, RTEDiag_AppldiagTimestampOutputLine );
	}
	else
	{
		return RTEDiag_SpawnLogger( argv, newTitle, outputFd, fileLimit, RTEDiag_AppldiagSimpleOutputLine );
	}
}

/*-----------------------------------------------------------*/

/*! QUIET=ON */
extern "C" SAPDB_Int4 RTEDiag_XserverLogger( 
	SAPDB_Char *argv[],
    SAPDB_Int4 outputFd, /* copy of sql22_fd */
    RTE_FileOffset fileLimit,
    SAPDB_Char const *protocolPipeName) /* new parameter */
{
	static char xserverTitle[] = "xserver.prt logger";
	return RTEDiag_SpawnLogger( argv, 
                                &xserverTitle[0],
                                outputFd,
                                fileLimit,
                                RTEDiag_AppldiagTimestampOutputLine,
                                protocolPipeName );
}

/*-----------------------------------------------------------*/

/*! QUIET=ON */
#ifdef TEST
int main (int, SAPDB_Char *[])
{
	RTE_SystemRc rc;
	RTE_FileHandle outputFd = open("log.txt", O_WRONLY|O_CREAT, 0666);
	if ( outputFd < 0 )
	{
		perror("open log.txt failed:");
		exit(1);
	}

	if ( RTE_SpawnLogger(outputFd, 550, RTEDiag_FormOutputLine) < 0 )
	{
		perror("Failed to spawn logger:");
		close(outputFd);
		exit(1);
	}

	const SAPDB_Char *line;

	line = "12324 First line\n";
	saveWrite(outputFd, line, strlen(line));
	line = "12325 Second line\n";
	saveWrite(outputFd, line, strlen(line));
	line = "another line\n";
	saveWrite(outputFd, line, strlen(line));
	line = RTEDiag_FormOutputLine(CycleBuffer::OutputEndOfStartup, 0, 0);
	saveWrite(outputFd, line, strlen(line));

	for ( SAPDB_Int4 i = 0; i < 40; i++ )
	{
		SAPDB_Int4 written = 0;
		SAPDB_Int4 length;
		SAPDB_Char singleLine[80];
		sprintf(singleLine, "%05d Some more messages\n", i);
		line = singleLine;
		length = (SAPDB_Int4)strlen(line);

		saveWrite( outputFd, line, length);
	}

	return 0;
}
#endif /* TEST */
/*!
QUIET=OFF
 */
