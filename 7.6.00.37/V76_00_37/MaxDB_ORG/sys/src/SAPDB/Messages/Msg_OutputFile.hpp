/*!
  @file    Msg_OutputFile.hpp
  @author  JoergM
  @ingroup Common
  @brief   Message output file class
  
  All message output classes are derived from class Msg_IOutput.

  This class is implementing the file output channel.
  
  -------------------------------------------------------------------------

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


#ifndef MSG_OUTPUTFILE_HPP
#define MSG_OUTPUTFILE_HPP

#include "Messages/Msg_List.hpp"
#include "Messages/Msg_IOutput.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "heo07.h"
#include "RunTime/Tasking/RTETask_Yield.h"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

#define RTEIO_FILE_NOT_OPEN "RTEIO", __FILE__, __LINE__, 1, "File $filename$ was not opened"

/*!
  @class Msg_OutputFile
  @brief Message file output channel class

  A message output channel is a class that allows to dump or write message output.
  The creation of a message output class allows to specify the output style and output format.
  For an output style that allows cyclic overwriting of message output an additional size is mandatory.
  The output format can be 'classic' which is list oriented and 'xml_alike' which produces XML like output.

  Each output class allows the output of message lists only

  During lifetime of an output channel the style and format are never changed
 */
class Msg_OutputFile : public Msg_IOutput
{
public:
    /*!
      each output generates a sequence number which is incremented during lifetime of output instance
     */
    typedef SAPDB_UInt8 SequenceNumber;

    /*!
      @brief ctor for cyclic written file

      This call only establishes parameter setting, but does not open the file

      @param filename             [in] path to file
      @param lockname             [in] name of registered spinlock to serialize message output
      @param handleExistingOutput [in] specify how to handle existing output files
      @param fixedSizeInBytes     [in] fixed size of file in bytes (if 0 use Append mode)
      @param pErrorArchive        [in] pointer to error archive if any
     */
    Msg_OutputFile( SAPDB_Char const      *filename
                  , SAPDB_UTF8 const      *lockname
                  , HandleExistingOutput const handleExistingOutput
                  , SAPDB_UInt8 const      fixedSizeInBytes
                  , Msg_IOutput           *pErrorArchive = 0 );

    /*!
      @brief dtor

      Always keep it closed since close may write footer or flush output buffer
     */
    virtual ~Msg_OutputFile()
    {
        Msg_List dummy;
        TryClose(dummy);
    }

    /*!
      @brief the output routine for informational messages

      @param outputMsgList [in] the message to be written
      @param errList [out] filled if error occured
      @param toArchive [in] allows to modifiy error message output behaviour

      @return true if output done, false if error (errList filled)
     */
    virtual SAPDB_Bool Output(Msg_List /*const*/ &outputMsgList, Msg_List &errList, PassToErrorArchive toArchive=toArchiveIfError );
	
    virtual SAPDB_Bool OutputToArchive(Msg_List /*const*/ &outputMsgList, Msg_List &errList )
    {
        if(!m_pErrorArchive )
        {
            return false;
        }
        // the error archive would give a sequence number to the list
        // but that would not be correct, because the output object instance 
        // increases the sequence numbers and gives them to the message lists
        // So we get the (next) sequence number from the outout object instance
        // and put it to the list manually.
        // If any other message list was written using the output object instance,
        // it would get that sequence number, too - But this message written here
        // is the very last one...
        outputMsgList.SetOutputIdentification( GetSequenceNumber()
					/*, SAPDB_UInt4 const outputThreadId=0
						, SAPDB_UInt4 const outputTaskId=0
						, SAPDB_UInt4 const outputSchedulerId=0 */);

        return m_pErrorArchive->Output( outputMsgList, errList );
    }

    /*!
      @brief Get time of creation
      @return Get the time stamp stored when object was created
     */
    virtual CreationDateTime GetCreationDateTime() const { return m_CreationDateTime; }

    /*!
      @brief Get output style
      
      If during creation a fixed size was specified, Msg_IOutput::Triple is returned, else Msg_IOutput::Append.

      The fixed size always specifies the size for the running phase only

      @return The used output style during running phase
     */

    virtual SAPDB_Bool HasFixedSize(void) const
    {
        return m_FixedSizeInBytes > 0;
    }

    virtual Msg_IOutput::OutputStyle const GetOutputStyle() const
    {
        if ( HasFixedSize() && ContinueExistingOutput == m_HandleExistingOutput )
        {
            return Msg_IOutput::Triple;
        }
        else
        {
            return Msg_IOutput::Single;
        }
    }

    // Runtime internal used phase changing routines

    /*!
      @brief Notify that startup phase is completed
      
      This call finalizes and closes the .start file and initializes and opens the .run file.
      All messages passed via the output routines are automatically rerouted to the .run file.

      If this call is issued before any message was written, no .start file is created.
  
      @param errList [out] filled in case of problems
      @return true if running phase entered, false else (errList filled)
     */
    virtual SAPDB_Bool StartupPhaseCompleted(Msg_List &errList)
    {
        Lock();
        if ( !PrepareRunningPhase(errList) )
        {
            Unlock();
            return false;
        }
        SAPDB_Bool retVal = true; 
        if(m_pErrorArchive)
        {
            retVal = m_pErrorArchive->StartupPhaseCompleted(errList);
        }
        Unlock();
        return retVal;
    }

    /*!
      @brief Notify that termination phase begins
      
      This call finalizes and closes the .start file (if called before startup phase was completed)
      and closes the .run file if it exists already.
      All messages passed via the output routines are automatically rerouted to the .stop file.

      If this call is issued before any message was written, no .start file is created.
  
      @param errList [out] filled in case of problems
      @return true if termination phase entered, false else (errList filled)
     */
    virtual SAPDB_Bool BeginOfTerminationPhase(Msg_List &errList)
    {
        if( Stopping != m_CurrentPhase )
        {
            m_CurrentPhase = Running;
            if ( !PrepareTerminatingPhase(errList) )
            {
                return false;
            }
            m_CurrentPhase = Stopping;
        }
        if(m_pErrorArchive)
            return m_pErrorArchive->BeginOfTerminationPhase(errList);
        else
            return true;
    }

    /*!
      @brief Get current phase

      The phase transition is done by calling StartupPhaseCompleted() and BeginOfTerminationPhase()
      @return One of the three different phases Starting,Running or Stopping
     */
    virtual Phase const GetPhase() const { return m_CurrentPhase; }

    virtual SequenceNumber const GetSequenceNumber(void)
    {
        return m_SequenceNumber;
    }
protected:
    virtual void Lock(void)
    {
        sqlbeginmutex(&m_MessageMutex);
    }

    virtual void Unlock(void)
    {
        sqlendmutex(&m_MessageMutex);
    }
    
private:
    /*!
      @brief copy the file name
      @param filename [in] to copy
     */
    void CopyFilename(SAPDB_Char const *filename)
    {
        SAPDB_Int4 iFilechar = 0;

        for ( iFilechar = 0; iFilechar < (sizeof(RTE_Path)-1); iFilechar++ )
        {
            m_Filename[iFilechar] = filename[iFilechar];
        }
        m_Filename[iFilechar] = 0;
    }

    /*!
      @brief spinlock protected open
      @param errList [out] filled in case of problem
      @return true if open succeded, false if still closed (errList filled)
     */
    SAPDB_Bool TryOpen(SAPDB_Char* fileName,Msg_List &errList);
        
    /*!
      @brief spinlock protected close
      @param errList [out] filled in case of problem
      @return true if close succeded, false if still open (errList filled)
     */
    SAPDB_Bool TryClose(Msg_List &errList);
        
    /*!
      @brief spinlock protected output
      @brief the output routine for informational messages

      @param outputMsgList [in] the message to be written
      @param errList [out] filled if error occured

      @return the sequence number of the output message, or 0 if output failed
     */
    SAPDB_Bool TryOutput(SAPDB_Char *outputMsgBuffer, SAPDB_UInt4 neededSize, Msg_List &errList, SAPDB_Bool emergency=false);

    /*!
       @brief preparing for starting phase
       @param errList [out] filled in case of problems
       @return true if preparation succeeded, false if failed
     */
protected:
    virtual SAPDB_Bool PrepareStartingPhase(Msg_List &errList);
    /*!
       @brief preparing for running phase
       @param errList [out] filled in case of problems
       @return true if preparation succeeded, false if failed
     */
    virtual SAPDB_Bool PrepareRunningPhase(Msg_List &errList);

    /*!
       @brief preparing for terminating phase
       @param errList [out] filled in case of problems
       @return true if preparation succeeded, false if failed
     */
    virtual SAPDB_Bool PrepareTerminatingPhase(Msg_List &errList);

protected:
    SequenceNumber             m_SequenceNumber;        //!< the sequence number advanced with each MsgList output
    RTE_Path                   m_Filename;              //!< the copy of the given file name
    RTE_FileHandle             m_FileHandle;            //!< handle to the file to write to
    SAPDB_UInt8                m_FilePosition;          //!< current position in file
    SAPDB_UInt8                m_StartOfCyclicPart;

private:
    teo07_Mutex                m_MessageMutex;           //!< this mutex serializes output the prevent message list to interleave.
    /*!
       @brief append a block of size bytes filled with spaces to the end of the file
       @return true if succeeded, false if failed
     */
    SAPDB_Bool ExtendFile(SAPDB_UInt8 size,Msg_List &errList);
    SAPDB_Bool WriteExtendingFailedMessage(Msg_List &errList);

    SAPDB_UInt8 const          m_FixedSizeInBytes;      //!< the fixed file size of 0 if to be calculated on demand
    Phase                      m_CurrentPhase;          //!< the current output phase
    HandleExistingOutput const m_HandleExistingOutput;  //!< how to handle exisiting output
    CreationDateTime           m_CreationDateTime;      //!< the timestamp when file was created
    Msg_IOutput              * m_pErrorArchive;         //!< this archive is used if output detect error message and PassToErrorArchive
    SAPDB_UInt8                m_currentFileSize;       //!< current size of file (if not cyclic)
    static SAPDB_Char          m_clear[];               
 }; /* @endclass Msg_OutputFile */

#endif /* MSG_OUTPUTFILE_HPP */
