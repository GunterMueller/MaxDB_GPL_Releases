/*!
  @file           geo41.h
  @author         RaymondR
  @special area   byte swapping
  @brief          byte swapping
  @see            tst05.lnk

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



#ifndef GEO41_H
#define GEO41_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo200.h"
#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SWAP_TYPE_UDEFINED_EO41      0xFF

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



/*---------------------------------------------------------------------------*/
/*--------------------------- class teo41_Swap ------------------------------*/
/*---------------------------------------------------------------------------*/

/*!
  @class          teo41_Swap
   @Description    Swap class


                This class is used to implement classes that need a
                swap functionality. This class was designed to hide
                the local and peer swap type. To use this class 
                derive your own class for example a communication 
                header class from 'teo41_Swap'. These header class
                normally contains the peer swap type. To swap values
                outside of your header class a public method should
                return a 'teo41_Swap' object (&lt;swap_object>). 
                An external swap can be done by calling:       &lt;br>
                                                               &lt;br>
                  &lt;swap_object>.eo42_SwapValue(&lt;my_value>);    &lt;br>
                                                               &lt;br>
                A check if swappping is necessary can be done 
                as follows:                                    &lt;br>
                                                               &lt;br>
                  &lt;swap_object>.eo41_SwappingRequired();       &lt;br>
                                                               &lt;br>
                class teo41_Swap does not have a base class.

 */

class teo41_Swap
{
public:
  /*!
     @Description    Creates a swap object.

   */

                    teo41_Swap ();

  /*!
     @Description    Check if swapping is required
     @Return value   true means swapping required

   */

  inline bool const eo41_SwappingRequired  () const
                                           { return  (teo41_Swap::LocalSwapType_eo41 != PeerSwapType_eo41); };


  /*!
     @Description    Swap data if necessary.
     @param          Src [in] - Source value.
     @param          Ret [out] - Return value.              For tsp00_Int1 and tsp00_Uint1 normally no swap is necessary.               But to prevent swapping problems after the type of these              varaibles changes, you should call eo41_SwapValue anyway.
     @Return value   none

   */


  inline void       eo41_SwapValue          ( tsp00_Uint1 const             Src,
                                              tsp00_Uint1                   &Ret ) const { Ret = Src; }; // - nothing to do!!
  inline void       eo41_SwapValue          ( tsp00_Int2 const              Src,
                                              tsp00_Int2                    &Ret ) const;
  inline void       eo41_SwapValue          ( tsp00_Uint2 const             Src,
                                              tsp00_Uint2                   &Ret ) const;
  inline void       eo41_SwapValue          ( tsp00_Int4 const              Src,
                                              tsp00_Int4                    &Ret ) const;
  inline void       eo41_SwapValue          ( tsp00_Uint4 const             Src,
                                              tsp00_Uint4                   &Ret ) const;
  inline void       eo41_SwapValue          ( tsp00_Int8 const              Src,
                                              tsp00_Int8                    &Ret ) const;
  inline void       eo41_SwapValue          ( tsp00_Uint8 const             Src,
                                              tsp00_Uint8                   &Ret ) const;

  //
  // --- swap inplace
  //
  /*!
     @Description    Swap data inplace if necessary.


                For tsp00_Int1 and tsp00_Uint1 normally no swap is necessary. 
                But to prevent swapping problems after the type of these
                varaibles changes, you should call eo41_SwapValue anyway.
     @param          Val [in/out] - Value.
     @Return value   none

   */

  inline void       eo41_SwapValue      ( tsp00_Uint1                   &Val ) const {;}; // - nothing to do!
  inline void       eo41_SwapValue      ( tsp00_Int2                    &Val ) const;
  inline void       eo41_SwapValue      ( tsp00_Uint2                   &Val ) const;
  inline void       eo41_SwapValue      ( tsp00_Int4                    &Val ) const;
  inline void       eo41_SwapValue      ( tsp00_Uint4                   &Val ) const;
  inline void       eo41_SwapValue      ( tsp00_Int8                    &Val ) const;
  inline void       eo41_SwapValue      ( tsp00_Uint8                   &Val ) const;

  // - copy constructor
  inline teo41_Swap ( teo41_Swap const &Swap );

/*! @name Protected methods and member variables */
 /*@(*/
protected:
  /*!
     @Description    Creates a swap object.
     @param          SwapType [in] - Swap type of the peer side.

   */

                    teo41_Swap ( tsp00_Uint1 const  SwapType );


  /*!
     @Description    Set the swap type of the peer side.
     @param          SwapType [in] - Swap type of the peer side.
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error.
     @return         true if deleted or not existing
              false else (setting *pEventList)

   */

  bool              eo41_SetPeerSwapType   ( tsp00_Uint1 const                  SwapType,
                                             teo200_EventList * const           pEventList = NULL );


  /*!
     @Description    Returns the swap type of the peer side.
     @Return value   Peer swap type

   */

  teo00_Uint1 const eo41_GetPeerSwapType () const { return PeerSwapType_eo41; };

  /*!
     @Description    Returns the swap type of this side.
     @Return value   Local swap type

   */

  static teo00_Uint1 const eo41_GetLocalSwapType ();


  /*!
     @Description    Check if swapping is required
     @param          SwapType [in] - Swap type of the peer side being                                         compared against.                                        If SWAP_TYPE_UDEFINED_EO41, the local                                         swap type will be compared against the                                         peer swap type set with the 'eo41_SetSwapType'                                         method or the constructor.
     @Return value   true means swapping required

   */

  inline bool const  eo41_SwappingRequired  ( tsp00_Uint1 const    SwapType ) const;


  /*!
     @Description    Old communication only, 6.2 - 7.2

   */

  //
  // - old communication only, 6.2 - 7.2
  //
  static tsp00_Uint1 eo41_LocalSwapTypeToOld ();
  static tsp00_Uint1 eo41_OldSwapTypeToNew   ( tsp00_Uint1 const  OldSwapType );

  /*@)*/

private:
  static void eo41_BuildLocalSwapType ();

private:
  void              eo41_SwapInt2           ( unsigned char const *         pUC,
                                              tsp00_Int2                    &Ret ) const; 

  void              eo41_SwapInt4           ( unsigned char const *         pUC,
                                              tsp00_Int4                    &Ret ) const;

  void              eo41_SwapInt8           ( unsigned char const *         pUC,
                                              tsp00_Int8                    &Ret ) const;

private:
  tsp00_Uint1          PeerSwapType_eo41;    // - swap type of the peer side.
  static teo00_Uint1   LocalSwapType_eo41;   // - swap type of this side
};



/*---------------------------------------------------------------------------*/

inline teo41_Swap::teo41_Swap( const teo41_Swap& Swap )
                        : PeerSwapType_eo41(Swap.PeerSwapType_eo41)
{ 
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::teo41_Swap"));
}


/*---------------------------------------------------------------------------*/

inline bool const  teo41_Swap::eo41_SwappingRequired( tsp00_Uint1 const    SwapType ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwappingRequired"));
  bool          SwapRequired;

  if ( SwapType == SWAP_TYPE_UDEFINED_EO41 )
    SwapRequired = (teo41_Swap::LocalSwapType_eo41 != PeerSwapType_eo41);
  else
    SwapRequired = (teo41_Swap::LocalSwapType_eo41 != SwapType);

  return SwapRequired;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue   ( tsp00_Int2 const   Src,
                                            tsp00_Int2         &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));

  if ( eo41_SwappingRequired() == false )
  {
    Ret = Src;
    return;
  }

  eo41_SwapInt2 ( (unsigned char const *)&Src, Ret );

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue   ( tsp00_Uint2 const  Src,
                                            tsp00_Uint2        &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));

  if ( eo41_SwappingRequired() == false )
  {
    Ret = Src;
    return;
  }

  eo41_SwapInt2 ( (unsigned char const *)&Src, (tsp00_Int2&)Ret );

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue   ( tsp00_Int4 const   Src,
                                            tsp00_Int4         &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));

  if ( eo41_SwappingRequired() == false )
  {
    Ret = Src;
    return;
  }

  eo41_SwapInt4 ( (unsigned char const *)&Src, Ret );

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue   ( tsp00_Uint4 const  Src,
                                            tsp00_Uint4        &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));

  if ( eo41_SwappingRequired() == false )
  {
    Ret = Src;
    return;
  }

  eo41_SwapInt4 ( (unsigned char const *)&Src, (tsp00_Int4&)Ret );

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue   ( tsp00_Int8 const   Src,
                                            tsp00_Int8         &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));

  if ( eo41_SwappingRequired() == false )
  {
    Ret = Src;
    return;
  }

  eo41_SwapInt8 ( (unsigned char const *)&Src, Ret );

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue   ( tsp00_Uint8 const  Src,
                                            tsp00_Uint8        &Ret ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));

  if ( eo41_SwappingRequired() == false )
  {
    Ret = Src;
    return;
  }

  eo41_SwapInt8 ( (unsigned char const *)&Src, (tsp00_Int8&)Ret );

  return;
}




/*---------------------------------------------------------------------------*/
/*------------------------------ swap inplace -------------------------------*/
/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue( tsp00_Int2   &Val ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));
  tsp00_Int2    Ret;

  if ( eo41_SwappingRequired() == false )
    return;

  eo41_SwapInt2 ( (unsigned char const *)&Val, Ret );
  Val = Ret;

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue( tsp00_Uint2  &Val ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));
  tsp00_Uint2   Ret;

  if ( eo41_SwappingRequired() == false )
    return;

  eo41_SwapInt2 ( (unsigned char const *)&Val, (tsp00_Int2&)Ret );
  Val = Ret;

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue( tsp00_Int4   &Val ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));
  tsp00_Int4    Ret;

  if ( eo41_SwappingRequired() == false )
    return;

  eo41_SwapInt4 ( (unsigned char const *)&Val, Ret );
  Val = Ret;

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue( tsp00_Uint4  &Val ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));
  tsp00_Uint4   Ret;

  if ( eo41_SwappingRequired() == false )
    return;

  eo41_SwapInt4 ( (unsigned char const *)&Val, (tsp00_Int4&)Ret );
  Val = Ret;

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue( tsp00_Int8   &Val ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));
  tsp00_Int8    Ret;

  if ( eo41_SwappingRequired() == false )
    return;

  eo41_SwapInt8 ( (unsigned char const *)&Val, Ret );
  Val = Ret;

  return;
}

/*---------------------------------------------------------------------------*/

inline void  teo41_Swap::eo41_SwapValue( tsp00_Uint8  &Val ) const
{
  ROUTINE_DBG_MEO60 (_T("teo41_Swap::eo41_SwapValue"));
  tsp00_Uint8   Ret;

  if ( eo41_SwappingRequired() == false )
    return;

  eo41_SwapInt8 ( (unsigned char const *)&Val, (tsp00_Int8&)Ret );
  Val = Ret;

  return;
}

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  EXAMPES                                                                  *
 *===========================================================================*/

/*!
   @code           
    
      class Header : private teo41_Swap
      {
      private:
        struct _Header
        {
          tsp00_Uint4            Version;
          tsp00_Uint2            ListLen;
          tsp00_Uint1            SwapType;
          tsp00_Uint1            Filler;
        };
    
      public:
                            Header ( void* Buffer ) { m_Header = (_Header*) Buffer; };
    
        static tsp00_Uint4  GetLen    () { return sizeof(_Header); };
      
        void                SetValues ( tsp00_Uint4            Version,
                                        tsp00_Uint2            ListLen )
                                      { m_Header->Version = Version; 
                                        m_Header->DataLen = ListLen; };
    
        bool                GetValues ( tsp00_Uint4            &Version,
                                        tsp00_Uint2            &ListLen,
                                        teo41_Swap             &Swap,
                                        teo200_EventList *     pEventList = NULL );
        private:
          _Header *    m_Header;
      };
    
    // --------------------------------------------------------------------------
    
    bool  Header::GetValues ( tsp00_Uint4            &Version,
                              tsp00_Uint2            &ListLen,
                              teo41_Swap             &Swap,
                              teo200_EventList *     pEventList )
    { 
      bool Ok = true;
      
      Ok = eo41_SetPeerSwapType( m_Header->SwapType )
      
      // --- Swap the header values
      if ( Ok ) 
      {
        eo41_SwapValue(m_Header->Version,  Version );
        eo41_SwapValue(m_Header->ListLen,  ListLen );
    
        Swap = *this;
      }
      return Ok;
    } 
    
    .
    .
    .
    
    void  MyFunction ( char*                  Buffer,
                       tsp00_Uint4            &Version,
                       teo200_EventList *     pEventList )
     {
       bool             Ok;
       tsp00_Uint2      ListLen;
       teo41_Swap       Swap;
     
       Header           ListHeader( Buffer );
    
       struct {
           tsp00_Int2  Element1;
           tsp00_Uint2 Element2;
           tsp00_Int4  Element3;
           tsp00_Uint4 Element4;
               .
               .
       } MyList* List = REINTERPRET_CAST( MyList *,
                                          Buffer + ListHeader.GetLen() );
           
       Ok = ListHeader.GetValues ( Version, &DataLen, Swap, pEventList );  
         
     
       if ( Ok )
       {
         Swap.eo41_SwapValue( List->Element1 );
         Swap.eo41_SwapValue( List->Element2 );
         Swap.eo41_SwapValue( List->Element3 );
         Swap.eo41_SwapValue( List->Element4 );
         .
         .
       } 
     }
    
    
    @endcode        
 */



#endif  /* GEO41_H */
