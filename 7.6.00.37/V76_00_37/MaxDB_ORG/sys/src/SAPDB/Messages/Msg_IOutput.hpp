/*!
  @file    Msg_IOutput.hpp
  @author  JoergM
  @ingroup Common
  @brief   Message output interface class
  
  All message output classes must be derived from this class.

  This class is used as base class for Msg_List output channels.


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

#ifndef MSG_IOUTPUT_HPP
#define MSG_IOUTPUT_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_ISystem.hpp"

#include "Messages/Msg_List.hpp"
/*! forward desclereation for output messages do not work as long as there is still a #define Msg_List SAPDBErr_MessageList */
//class Msg_List;

/*!
  @class Msg_IOutput
  @brief Message output channel 'pure' interface class

  A message output channel is a class that allows to dump or write message output.
  The creation of a message output class allows to specify the output style and output format.
  For an output style that allows cyclic overwriting of message output an additional size is mandatory.
  The output format can be 'classic' which is list oriented and 'xml_alike' which produces XML like output.

  Each output class allows the output of message lists only

  During lifetime of an output channel the style and format are never changed
 */
class Msg_IOutput
{
public:
    //! constructor definition to allow passing of fatal errors to archive
    enum PassToErrorArchive { alwaysToArchive    //!< message is written to error archive log too
                            , toArchiveIfError   //!< only error messages are written to archive log too
                            , neverToArchive     //!< message is written by this instance only
    };
//    enum HandleError { PassToErrorArchive       //!< each error message is written to error log too
//                     , DoNotPassToErrorArchive  //!< each message is written by this instance only
//    };

    //! constructor definition to allow reusing of existing message output
    enum HandleExistingOutput { ContinueExistingOutput  //!< continue existing output
                              , RenameExistingOutput    //!< creates .old versions for existing output
                              , RemoveExistingOutput    //!< removes existing output
    };

    /*! output style specification is valid during lifetime of output channel */
    enum OutputStyle { Single  //!< append each message list at end of channel
                     , Triple  //!< cyclic overwrite message channel (meaning for file oriented channels only)
    };

    /*! style specification is valid during lifetime of output channel */
    enum Phase { NoOutput  //!< Never had any output so far
               , Starting  //!< write each message to .start file
               , Running   //!< write each message to .run file
               , Stopping  //!< write each message to .stop file
    };
    /*!
      each output allows to specify when it was created
     */
    typedef RTE_ISystem::DateTime      CreationDateTime;

    /*!
      @brief the output routine for informational messages

      The output routine internaly sets the message sequence number into the message list and if
      a new phase was started, initializes the corresponding output file.

      @param outputMsgList [in] the message to be written
      @param errList [out] filled if error occured
      @param toArchive [in] allows to modifiy error message output behaviour
      @return the sequence number of the output message, or 0 if output failed
     */
    virtual SAPDB_Bool Output(Msg_List /*const */&outputMsgList, Msg_List &errList, PassToErrorArchive toArchive=toArchiveIfError ) = 0;

    /*!
      @brief Get time of creation
      @return Get the time stamp stored when object was created
     */
    virtual CreationDateTime GetCreationDateTime() const = 0;

    /*!
      @brief Get output style
      @return The during construction specified output style
     */
    virtual OutputStyle const GetOutputStyle() const = 0;
    
    /*!
      @brief return reference to default diagnostic output channel

      This reference can be used to 
     */
    static Msg_IOutput &GetDiagnosticOutput();

    /*!
      @brief return reference to emergency diagnostic output channel

      This reference can be used in case the normal diagnostic messages output call returns an error message.
     */
    static Msg_IOutput &GetEmergencyOutput();

protected:
    // the following interface routines are used by runtime internaly only

    /*!
      @brief Get current phase

      The phase transition is done by calling StartupPhaseCompleted() and BeginOfTerminationPhase()
      @return One of the three different phases Starting,Running or Stopping
     */
    virtual Phase const GetPhase() const = 0;

public:
    /*!
      @brief Notify that startup phase is completed
      
      This call finalizes the starting phase and initializes the runing phase
      
      @param errList [out] filled in case of problems
      @return true if running phase entered, false else (errList filled)
     */
    virtual SAPDB_Bool StartupPhaseCompleted(Msg_List &errList) = 0;

    /*!
      @brief Notify that termination phase begins
      
      This call finalizes the running phase, if called before starting phase was completed.
      This call always finalizes the running phase and initializes the termination phase.
  
      @param errList [out] filled in case of problems
      @return true if termination phase entered, false else (errList filled)
     */
    virtual SAPDB_Bool BeginOfTerminationPhase(Msg_List &errList) = 0;

}; /* @endclass Msg_IOutput */

#endif /* MSG_IOUTPUT_HPP */
