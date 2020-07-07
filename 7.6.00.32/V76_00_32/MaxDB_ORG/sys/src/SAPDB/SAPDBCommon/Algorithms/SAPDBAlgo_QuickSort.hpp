/*!
    @file     SAPDBAlgo_QuickSort.hpp
    @ingroup  SAPDBAlgo
    @author   Henrik
    @brief    Quicksort algorithm
    @see            

  responsible:  Henrik

  special area: Basic Algorithms

  description:  Implementation of Quicksort

  see:          http://pwww.bea.sap-ag.de/Kern/v72/BD-Sortieralgorithmen.doc

  copyright:    Copyright (c) 1999-2005 SAP AG

  created:      2002-04-19

  updated:

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


#ifndef SAPDBALGO_QUICKSORT_H
#define SAPDBALGO_QUICKSORT_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

// maximal field length for the QuickSort algorithm and its stack
#define SAPDBAlgo_MAX_QUICKSORT_ELEMENTS        16777216 // old:2000000
// old: ( log2(SAPDBAlgo_MAX_QUICKSORT_ELEMENTS)*2 )
// new: log(SAPDBAlgo_MAX_QUICKSORT_ELEMENTS)+1 = 25
#define SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES    32       

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

struct SAPDBAlgo_QuickSortItem
{
    SAPDB_Int4  qsiLeft;
    SAPDB_Int4  qsiRight;
};

/*===========================================================================*
 * DECLARATIONS                                                              *
 *===========================================================================*/

/*!
  Chapter: In-memory sorting

  description: Generic sort algorithms suitable for in-memory data.
*/

/*!
  function: SAPDBAlgo_QuickSort

  template arguments:
    TData       The type of the Data to be sorted.
    TComparator The comparison operator's type. Must implement the methods IsEqual and IsPredecessor of SAPDBAlgo_IComparator.
                Default template parameter is SAPDBAlgo_IComparator

  arguments:
    Data                [in/out] Pointer to array of TData _or_ pointer to array of pointer to TData
    NumberOfElements    [in]
    Comparator          [in] optional

  return value:    Success or failure.

  description:
    Non recursive quicksort implementation for sorting lists of Data.

    A list consists of an array of 'NumberOfElements' Data of type
    'TData'. A direct memory access via 'Data[Index]' must be
    possible.

    Version 1a uses the C++-defined operator '<' and is suitable
    for Data of basic types (int, float, char). The order of the sorted data 
    will be ASCENDING.
    
    Version 1b uses an array of pointer to TData instead of an array of TData

    Version 2a of the function needs a user defined comparison-operator which
    has to be derived from the abstract class 'SAPDBAlgo_IComparator'.
    The order of the sorted data depends on the Comparator as its 'Prior' method
    will be used.

    Version 2b uses an array of pointer to TData instead of an array of TData

  code:
    // Call example 1 (basic type)
    int MyArray[500];
    SAPDBAlgo_QuickSort(MyArray, 500);

*/

/*---------------------------------------------------------------------------*/

// Quicksort implementation using the default compare operators (<,>).
template <class TData>
bool SAPDBAlgo_QuickSort(   TData*		            Data,
					        const SAPDB_UInt4	    NumberOfElements)
{
    SAPDB_Int4      i;
    SAPDB_Int4	    j;
    SAPDB_Int4	    Left;
    SAPDB_Int4	    Right;
    SAPDB_Int4      PivotIndex;
    SAPDB_Int4      StackIndex = 0;
    TData           AuxElem;
    TData           PivotElem;
    SAPDBAlgo_QuickSortItem    Stack[SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES];

    if ( NumberOfElements > SAPDBAlgo_MAX_QUICKSORT_ELEMENTS ) 
        return false;
    
    if ( (0 == NumberOfElements) || (Data == NULL) ) return true;

    Stack [StackIndex].qsiLeft = 0;
    Stack [StackIndex].qsiRight = NumberOfElements - 1;
    do
    {
        Left  = Stack [StackIndex].qsiLeft;
        Right = Stack [StackIndex].qsiRight;
        --StackIndex;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex = (Left + Right) >> 1;
            PivotElem  = Data [PivotIndex];
            do
            {
                while (Data [i] < PivotElem) i++;
                while (PivotElem < Data [j]) j--;

                if (i <= j)
                {
                    AuxElem  = Data [i];
                    Data [i] = Data [j];
                    Data [j] = AuxElem;
                    i++;
                    j--;
                }
            }
            while (i <= j);

            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = i;
                    Stack [StackIndex].qsiRight = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = Left;
                    Stack [StackIndex].qsiRight = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
    return true;
}

// Quicksort implementation using the default compare operators (<,>).
template <class TData>
bool SAPDBAlgo_QuickSort(   TData**		            DataPtr,
					        const SAPDB_UInt4	    NumberOfElements)
{
    SAPDB_Int4      i;
    SAPDB_Int4	    j;
    SAPDB_Int4	    Left;
    SAPDB_Int4	    Right;
    SAPDB_Int4      PivotIndex;
    SAPDB_Int4      StackIndex = 0;
    TData*          AuxElemPtr;
    TData*          PivotElemPtr;
    SAPDBAlgo_QuickSortItem    Stack[SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES];

    if ( NumberOfElements > SAPDBAlgo_MAX_QUICKSORT_ELEMENTS ) 
        return false;
    
    if ( (0 == NumberOfElements) || (DataPtr == NULL) ) return true;

    Stack [StackIndex].qsiLeft = 0;
    Stack [StackIndex].qsiRight = NumberOfElements - 1;
    do
    {
        Left  = Stack [StackIndex].qsiLeft;
        Right = Stack [StackIndex].qsiRight;
        --StackIndex;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex    = (Left + Right) >> 1;
            PivotElemPtr  = DataPtr[PivotIndex];
            do
            {
                while (*(DataPtr[i]) < *PivotElemPtr) i++;
                while (*PivotElemPtr < *(DataPtr[j])) j--;

                if (i <= j)
                {
                    AuxElemPtr   = DataPtr [i];
                    DataPtr [i]  = DataPtr [j];
                    DataPtr [j]  = AuxElemPtr;
                    i++;
                    j--;
                }
            }
            while (i <= j);

            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = i;
                    Stack [StackIndex].qsiRight = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = Left;
                    Stack [StackIndex].qsiRight = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
    return true;
}


template <class TData, class TComparator>
bool SAPDBAlgo_QuickSort(   TData*		            Data,
					        const SAPDB_UInt4	    NumberOfElements,
                            TComparator&            Comparator)
{
    SAPDB_Int4       i;
    SAPDB_Int4	     j;
    SAPDB_Int4	     Left;
    SAPDB_Int4	     Right;
    SAPDB_Int4       PivotIndex;
    SAPDB_Int4       StackIndex = 0;
    TData            AuxElem;
    TData            PivotElem;
    SAPDBAlgo_QuickSortItem    Stack[SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES];

    if ( NumberOfElements > SAPDBAlgo_MAX_QUICKSORT_ELEMENTS ) 
        return false;
    
    if ( (0 == NumberOfElements) || (Data == NULL) ) return true;

    Stack [StackIndex].qsiLeft  = 0;
    Stack [StackIndex].qsiRight = NumberOfElements - 1;
    do
    {
        Left  = Stack [StackIndex].qsiLeft;
        Right = Stack [StackIndex].qsiRight;
        --StackIndex;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex = (Left + Right) >> 1;
            PivotElem  = Data [PivotIndex];
            do
            {
                while ( Comparator.IsPredecessor(Data[i], PivotElem) ) i++;
                while ( Comparator.IsPredecessor(PivotElem, Data[j]) ) j--;

                if (i <= j)
                {
                    AuxElem = Data[i];
                    Data[i] = Data[j];
                    Data[j] = AuxElem;
                    i++;
                    j--;
                }
            }
            while (i <= j);

            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = i;
                    Stack [StackIndex].qsiRight = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = Left;
                    Stack [StackIndex].qsiRight = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
    return true;
}

template <class TData>
bool SAPDBAlgo_QuickSort(   TData*		                    Data,
					        const SAPDB_UInt4	            NumberOfElements,
                            SAPDBAlgo_IComparator<TData>&   Comparator)
{
    return SAPDBAlgo_QuickSort<TData, SAPDBAlgo_IComparator<TData> >(Data, NumberOfElements, Comparator);
}


template <class TData, class TComparator>
bool SAPDBAlgo_QuickSort(   TData**		            DataPtr,
					        const SAPDB_UInt4	    NumberOfElements,
                            TComparator&            Comparator)
{
    SAPDB_Int4       i;
    SAPDB_Int4	     j;
    SAPDB_Int4	     Left;
    SAPDB_Int4	     Right;
    SAPDB_Int4       PivotIndex;
    SAPDB_Int4       StackIndex = 0;
    TData*           AuxElemPtr;
    TData*           PivotElemPtr;
    SAPDBAlgo_QuickSortItem Stack[SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES];

    if ( NumberOfElements > SAPDBAlgo_MAX_QUICKSORT_ELEMENTS ) 
        return false;
    
    if ( (0 == NumberOfElements) || (DataPtr == NULL) ) return true;

    Stack [StackIndex].qsiLeft  = 0;
    Stack [StackIndex].qsiRight = NumberOfElements - 1;
    do
    {
        Left  = Stack [StackIndex].qsiLeft;
        Right = Stack [StackIndex].qsiRight;
        --StackIndex;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex    = (Left + Right) >> 1;
            PivotElemPtr  = DataPtr[PivotIndex];
            do
            {
                while ( Comparator.IsPredecessor(DataPtr[i], PivotElemPtr) ) i++;
                while ( Comparator.IsPredecessor(PivotElemPtr, DataPtr[j]) ) j--;

                if (i <= j)
                {
                    AuxElemPtr = DataPtr[i];
                    DataPtr[i] = DataPtr[j];
                    DataPtr[j] = AuxElemPtr;
                    i++;
                    j--;
                }
            }
            while (i <= j);

            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = i;
                    Stack [StackIndex].qsiRight = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    if ( SAPDBAlgo_MAX_QUICKSORT_STACKENTRIES <= StackIndex ) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft  = Left;
                    Stack [StackIndex].qsiRight = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
    return true;
}

template <class TData >
bool SAPDBAlgo_QuickSort(   TData**		                    DataPtr,
					        const SAPDB_UInt4	            NumberOfElements,
                            SAPDBAlgo_IComparator<TData>&   Comparator)
{
    return SAPDBAlgo_QuickSort<TData, SAPDBAlgo_IComparator<TData> >(DataPtr, NumberOfElements, Comparator);
}

/*---------------------------------------------------------------------------*/

#endif  /* SAPDBALGO_QUICKSORT_HPP */



