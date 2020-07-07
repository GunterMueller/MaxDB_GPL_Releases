/*!**************************************************************************

  module      : RTEComm_Swapping.hpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: byte swapping 
  description : byte swapping 


  last changed: 2002-11-15  13:55
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


*****************************************************************************/


#ifndef RTECOMM_SWAPPING_HPP
#define RTECOMM_SWAPPING_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_MessageList.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic Runtime_Trace; ///< global runtime trace object
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SWAP_TYPE_UDEFINED        0xFF

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  Class:        RTEComm_Swapping
  Description:  Swap class

                This class is used to implement communication classes 
                that need a swap functionality. This class was designed 
                to hide the local and peer swap type information. 
                To use this class create a swap member varaible in your 
                own class ( for example a communication header class ). 
                This header class normally knows the peer swap type
                information which has to be set with 'SetPeerSwapType'. 
                Now, all header class values where swapping is needed 
                can be swapped via 'SwapValue'.
                To swap values outside of your header class a public 
                method should return a 'RTEComm_Swapping' object 
                (<swap_object>). 

                A swap can be done by calling:                 <br>
                                                               <br>
                       <swap_object>.SwapValue(<my_value>);    <br>
                                                               <br>
                A check if swappping is necessary can be done 
                via:                                           <br>
                                                               <br>
                       <swap_object>.SwappingRequired();       <br>
                                                               <br>
  
                class RTEComm_Swapping does not have a base class.
*/
class RTEComm_Swapping
{
public:
  /*!---------------------------------------------------------------------------
     Function:     RTEComm_Swapping
     Description:  Creates a swap object.

     Arguments:    none
  */
                    RTEComm_Swapping ();

  /*!---------------------------------------------------------------------------
     Function:     RTEComm_Swapping
     Description:  Creates a swap object.

     Arguments:    SwapType      [in]      - Swap type of the peer side.

  */
                    RTEComm_Swapping ( SAPDB_UInt1 const  SwapType );

  /*!---------------------------------------------------------------------------
     Function:     PeerSwapTypeSet
     Description:  Returns true if the peer swap type was set.

     Arguments:    None

     Return value: true if the peer swap type was set
                   
  */
  bool const PeerSwapTypeSet () const { return PeerSwapType == SWAP_TYPE_UDEFINED; };

  /*!---------------------------------------------------------------------------
     Function:     SetPeerSwapType
     Description:  Set the swap type of the peer side.

     Arguments:    SwapType     [in]     - Swap type of the peer side.
                   MessageList  [inout]  - Message list for error messages

     return value: true   if deleted or not existing
                   false  else 

  */
  bool              SetPeerSwapType   ( SAPDB_UInt1 const      SwapType,
                                        SAPDBErr_MessageList   &MessageList );

  /*!---------------------------------------------------------------------------
     Function:     GetPeerSwapType
     Description:  Returns the swap type of the peer side.

     Arguments:    None

     Return value: Peer swap type
                   
  */
  SAPDB_UInt1 const GetPeerSwapType () const { return PeerSwapType; };

  /*!---------------------------------------------------------------------------
     Function:     GetLocalSwapType
     Description:  Returns the swap type of this side.

     Arguments:    None

     Return value: Local swap type
                   
  */
  static SAPDB_UInt1 const GetLocalSwapType ();


  /*!---------------------------------------------------------------------------
     Function:     SwappingRequired
     Description:  Check if swapping is required

     Arguments:    none

     Return value: true means swapping required
  */
  inline bool const SwappingRequired  () const
                 { return  (RTEComm_Swapping::LocalSwapType != PeerSwapType); };

  /*!---------------------------------------------------------------------------
     Function:     SwappingRequired
     Description:  Check if swapping is required

     Arguments:    SwapType      [in]      - Swap type of the peer side being 
                                             compared against.
                                             If SWAP_TYPE_UDEFINED, the local 
                                             swap type will be compared against the 
                                             peer swap type set with the 'SetSwapType' 
                                             method or the constructor.

     Return value: true means swapping required
  */
  inline bool const  SwappingRequired  ( SAPDB_UInt1 const    SwapType ) const;

  /*!---------------------------------------------------------------------------
     Function:     SwapValue
     Description:  Swap data if necessary.

     Arguments:    Src           [in]      - Source value.
                   Ret           [out]     - Return value.

                   For SAPDB_Int1 and SAPDB_UInt1 normally no swap is necessary. 
                   But to prevent swapping problems after the type of these
                   varaibles changes, you should call SwapValue anyway.

     Return value: none

     prototypes: 7
  */

  inline bool       SwapValue          ( SAPDB_UInt1 const             Src,
                                         SAPDB_UInt1                   &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const { Ret = Src; return true; }; // - nothing to do!!
  inline bool       SwapValue          ( SAPDB_Int2 const              Src,
                                         SAPDB_Int2                    &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue          ( SAPDB_UInt2 const             Src,
                                         SAPDB_UInt2                   &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue          ( SAPDB_Int4 const              Src,
                                         SAPDB_Int4                    &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue          ( SAPDB_UInt4 const             Src,
                                         SAPDB_UInt4                   &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue          ( SAPDB_Int8 const              Src,
                                         SAPDB_Int8                    &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue          ( SAPDB_UInt8 const             Src,
                                         SAPDB_UInt8                   &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;

  //
  // --- swap inplace
  //
  /*!---------------------------------------------------------------------------
     Function:     SwapValue
     Description:  Swap data inplace if necessary.

                   For SAPDB_Int1 and SAPDB_UInt1 normally no swap is necessary. 
                   But to prevent swapping problems after the type of these
                   varaibles changes, you should call SwapValue anyway.

     Arguments:    Val           [in/out]  - Value.

     Return value: none

     prototypes: 7
  */
  inline bool       SwapValue      ( SAPDB_UInt1                   &Val,
                                     SAPDBErr_MessageList          &MessageList ) const { return true;}; // - nothing to do!
  inline bool       SwapValue      ( SAPDB_Int2                    &Val,
                                     SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue      ( SAPDB_UInt2                   &Val,
                                     SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue      ( SAPDB_Int4                    &Val,
                                     SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue      ( SAPDB_UInt4                   &Val,
                                     SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue      ( SAPDB_Int8                    &Val,
                                     SAPDBErr_MessageList          &MessageList ) const;
  inline bool       SwapValue      ( SAPDB_UInt8                   &Val,
                                     SAPDBErr_MessageList          &MessageList ) const;

  // - copy constructor
  inline RTEComm_Swapping ( RTEComm_Swapping const &Swap );

/*! Chapter: Protected methods and member variables */
protected:

  /*!---------------------------------------------------------------------------
     Function:     NewSwapTypeToOld, OldSwapTypeToNew
     Description:  Old communication only, 6.2 - 7.2
     prototypes: 2
  */
  //
  // - old communication only, 6.2 - 7.2
  //
  static SAPDB_UInt1 LocalSwapTypeToOld ();
  static SAPDB_UInt1 OldSwapTypeToNew   ( SAPDB_UInt1 const  OldSwapType );

  /*! EndChapter: Protected methods and member variables  */

private:
  static void BuildLocalSwapType ();

private:
  bool              SwapInt2           ( unsigned char const *         pUC,
                                         SAPDB_Int2                    &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const; 

  bool              SwapInt4           ( unsigned char const *         pUC,
                                         SAPDB_Int4                    &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;

  bool              SwapInt8           ( unsigned char const *         pUC,
                                         SAPDB_Int8                    &Ret,
                                         SAPDBErr_MessageList          &MessageList ) const;

private:
  SAPDB_UInt1          PeerSwapType;    // - swap type of the peer side.
  static SAPDB_UInt1   LocalSwapType;   // - swap type of this side
};

/*! EndClass: RTEComm_Swapping */

/*---------------------------------------------------------------------------*/

inline RTEComm_Swapping::RTEComm_Swapping( const RTEComm_Swapping& Swap )
                        : PeerSwapType(Swap.PeerSwapType)
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::RTEComm_Swapping", Runtime_Trace, 5);
}


/*---------------------------------------------------------------------------*/

inline bool const  RTEComm_Swapping::SwappingRequired( SAPDB_UInt1 const    SwapType ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwappingRequired", Runtime_Trace, 5);
  bool          SwapRequired;

  if ( SwapType == SWAP_TYPE_UDEFINED )
    SwapRequired = (RTEComm_Swapping::LocalSwapType != PeerSwapType);
  else
    SwapRequired = (RTEComm_Swapping::LocalSwapType != SwapType);

  return SwapRequired;
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue ( SAPDB_Int2 const      Src,
                                           SAPDB_Int2            &Ret,
                                           SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);

  if ( SwappingRequired() == false )
  {
    Ret = Src;
    return true;
  }

  return SwapInt2 ( (unsigned char const *)&Src, Ret, MessageList );
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue   ( SAPDB_UInt2 const     Src,
                                             SAPDB_UInt2           &Ret,
                                             SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);

  if ( SwappingRequired() == false )
  {
    Ret = Src;
    return true;
  }

  return SwapInt2 ( (unsigned char const *)&Src, (SAPDB_Int2&)Ret, MessageList );
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue  ( SAPDB_Int4 const      Src,
                                            SAPDB_Int4            &Ret,
                                            SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);

  if ( SwappingRequired() == false )
  {
    Ret = Src;
    return true;
  }

  return SwapInt4 ( (unsigned char const *)&Src, Ret, MessageList );

}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue  ( SAPDB_UInt4 const     Src,
                                            SAPDB_UInt4           &Ret,
                                            SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);

  if ( SwappingRequired() == false )
  {
    Ret = Src;
    return true;
  }

  return SwapInt4 ( (unsigned char const *)&Src, (SAPDB_Int4&)Ret, MessageList );
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue  ( SAPDB_Int8 const      Src,
                                            SAPDB_Int8            &Ret,
                                            SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);

  if ( SwappingRequired() == false )
  {
    Ret = Src;
    return true;
  }

  return SwapInt8 ( (unsigned char const *)&Src, Ret, MessageList );
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue  ( SAPDB_UInt8 const     Src,
                                            SAPDB_UInt8           &Ret,
                                            SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);

  if ( SwappingRequired() == false )
  {
    Ret = Src;
    return true;
  }

  return SwapInt8 ( (unsigned char const *)&Src, (SAPDB_Int8&)Ret, MessageList );

}


/*---------------------------------------------------------------------------*/
/*------------------------------ swap inplace -------------------------------*/
/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue( SAPDB_Int2            &Val,
                                          SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);
  SAPDB_Int2    Ret;
  bool          Ok;

  if ( SwappingRequired() == false )
    return true;

  Ok  = SwapInt2 ( (unsigned char const *)&Val, Ret, MessageList );
  Val = Ret;

  return Ok;
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue( SAPDB_UInt2           &Val,
                                          SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);
  SAPDB_UInt2   Ret;
  bool          Ok;

  if ( SwappingRequired() == false )
    return true;

  Ok  = SwapInt2 ( (unsigned char const *)&Val, (SAPDB_Int2&)Ret, MessageList );
  Val = Ret;

  return Ok;
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue( SAPDB_Int4            &Val,
                                          SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);
  SAPDB_Int4    Ret;
  bool          Ok;

  if ( SwappingRequired() == false )
    return true;

  Ok  = SwapInt4 ( (unsigned char const *)&Val, Ret, MessageList );
  Val = Ret;

  return Ok;
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue( SAPDB_UInt4           &Val,
                                          SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);
  SAPDB_UInt4   Ret;
  bool          Ok;

  if ( SwappingRequired() == false )
    return true;

  Ok  = SwapInt4 ( (unsigned char const *)&Val, (SAPDB_Int4&)Ret, MessageList );
  Val = Ret;

  return Ok;
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue( SAPDB_Int8            &Val,
                                          SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);
  SAPDB_Int8    Ret;
  bool          Ok;

  if ( SwappingRequired() == false )
    return true;

  Ok  = SwapInt8 ( (unsigned char const *)&Val, Ret, MessageList );
  Val = Ret;

  return Ok;
}

/*---------------------------------------------------------------------------*/

inline bool  RTEComm_Swapping::SwapValue( SAPDB_UInt8           &Val,
                                          SAPDBErr_MessageList  &MessageList ) const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEComm_Swapping::SwapValue", Runtime_Trace, 5);
  SAPDB_UInt8   Ret;
  bool          Ok;

  if ( SwappingRequired() == false )
    return true;

  Ok  = SwapInt8 ( (unsigned char const *)&Val, (SAPDB_Int8&)Ret, MessageList );
  Val = Ret;

  return Ok;
}

/*===========================================================================*
 *  EXAMPES                                                                  *
 *===========================================================================*/
    
/*!
   Example: Example RTEComm_Swapping
   Code:
      class Header
      {
      private:
        struct _Header
        {
          SAPDB_UInt4            Version;
          SAPDB_UInt2            ListLen;
          SAPDB_UInt1            SwapType;
          SAPDB_UInt1            Filler;
        };
   
      public:
                            Header ( void* Buffer ) { m_Header = (_Header*) Buffer; };
   
        static SAPDB_UInt4  GetLen    () { return sizeof(_Header); };
      
        void                SetValues ( SAPDB_UInt4            Version,
                                        SAPDB_UInt2            ListLen )
                                      { m_Header->Version = Version; 
                                        m_Header->DataLen = ListLen; };
   
        bool                GetValues ( SAPDB_UInt4            &Version,
                                        SAPDB_UInt2            &ListLen,
                                        RTEComm_Swapping       &Swap,
                                        SAPDBErr_MessageList   &MessageList );
        private:
          _Header *        m_Header;
          RTEComm_Swapping m_Swap;
      };
   
    // --------------------------------------------------------------------------
   
    bool  Header::GetValues ( SAPDB_UInt4            &Version,
                              SAPDB_UInt2            &ListLen,
                              RTEComm_Swapping       &Swap,
                              SAPDBErr_MessageList   &MessageList )
    { 
      bool Ok = true;
      
      Ok = m_Swap.SetPeerSwapType( m_Header->SwapType )
      
      // --- Swap the header values
      if ( Ok ) 
      {
        m_Swap.SwapValue(m_Header->Version,  Version );
        m_Swap.SwapValue(m_Header->ListLen,  ListLen );
   
        Swap = m_Swap;
      }
      return Ok;
    } 
   
    .
    .
    .
   
    void  MyFunction ( char*                  Buffer,
                       SAPDB_UInt4            &Version,
                       SAPDBErr_MessageList   &MessageList )
     {
       bool             Ok;
       SAPDB_UInt2      ListLen;
       RTEComm_Swapping Swap;
     
       Header           ListHeader( Buffer );

       struct {
           SAPDB_Int2  Element1;
           SAPDB_UInt2 Element2;
           SAPDB_Int4  Element3;
           SAPDB_UInt4 Element4;
               .
               .
       } MyList* List = REINTERPRET_CAST( MyList *,
                                          Buffer + ListHeader.GetLen() );
           
       Ok = ListHeader.GetValues ( Version, &DataLen, Swap, MessageList );  
         
     
       if ( Ok ) Ok = Swap.SwapValue( List->Element1, MessageList );
       if ( Ok ) Ok = Swap.SwapValue( List->Element2, MessageList );
       if ( Ok ) Ok = Swap.SwapValue( List->Element3, MessageList );
       if ( Ok ) Ok = Swap.SwapValue( List->Element4, MessageList );
        .
       .
     }

*/

#endif  /* RTECOMM_SWAPPING_HPP */
