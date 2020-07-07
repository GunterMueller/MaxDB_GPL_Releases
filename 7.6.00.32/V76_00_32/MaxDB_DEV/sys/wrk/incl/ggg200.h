/*!***************************************************************************
  module:       ggg200.h

  responsible:  TimW

  special area: Basic Algorithms

  description:  Implementation of basic algorithms.

  see:          http://pwww.bea.sap-ag.de/Kern/v72/BD-Sortieralgorithmen.doc

  copyright:    Copyright (c) 1999-2005 SAP AG

  created:      1999-08-01

  updated:
                1999-09-23  Alexander Kley
                            changed memory allocation in gg200merge
			                now memory has to be supplied by the user
				2001-07-23  Alexander Kley
				            -The iterator gg200MergeIterator was inserted
							-The template gg200merge was modified to use
							 less comparisons per merge step (see PTS 1111038) 


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef GGG200_H
#define GGG200_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define ASCENDING_GG200  true
#define DESCENDING_GG200 false

// minimal field width for the Quicksort algorithm
// smaller (sub)fields are sorted by InsertionSort
#define MIN_WIDTH            10

// maximal field length for the QuickSort algorithm and its stack
// ( log2(MAX_ELEMS_GG200)*2 )
#define MAX_ELEMS_GG200         2000000
#define MAX_STACK_ELEMS_GG200   32

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

struct tgg200_QuickSortItem
{
    tsp00_Int4  qsiLeft_gg200;
    tsp00_Int4  qsiRight_gg200;
};

/*===========================================================================*
 * DECLARATIONS                                                              *
 *===========================================================================*/

/*!
  Chapter: In-memory sorting

  description: Generic sort algorithms suitable for in-memory data.
*/

/*!
  class: cgg200ComparisonOperator

  description:
    Abstract base class for the implementation of comparison operators.

    Derived classes of this class enable the sort algorithms to sort
    data of different types

    In the derived class, implement the member function gg200Compare()
    so that it returns
    - 'l_less' if elem1 < elem2,
    - 'l_greater' if elem1 > elem2,
    - 'l_equal' if elem1 == elem2.

  template arguments:
    T  The type of the class to be compared.
*/
template <class T> class cgg200ComparisonOperator
{
public:

   /*!
     function: gg200Compare

     description:
       Compares two elements.

     returns:
       'l_less' if elem1 < elem2,
       'l_greater' if elem1 > elem2,
       'l_equal' if elem1 == elem2
    */
   virtual tsp00_LcompResult_Enum gg200Compare
      (const T &elem1, const T &elem2 ) = 0;

   /*!
     function: gg200IsEqual
    */
   inline bool gg200IsEqual( const T &elem1, const T &elem2 )
                  { return gg200Compare(elem1,elem2)==l_equal; }
   /*!
     function: gg200IsNotEqual
    */
   inline bool gg200IsNotEqual( const T &elem1, const T &elem2 )
                  { return gg200Compare(elem1,elem2)!=l_equal; }
   /*!
     function: gg200IsGreater
    */
   inline bool gg200IsGreater( const T &elem1, const T &elem2 )
                  { return gg200Compare(elem1,elem2)==l_greater; }
   /*!
     function: gg200IsLess
    */
   inline bool gg200IsLess( const T &elem1, const T &elem2 )
                  { return gg200Compare(elem1,elem2)==l_less; }
   /*!
     function: gg200IsGreaterOrEqual
    */
   inline bool gg200IsGreaterOrEqual( const T &elem1, const T &elem2 )
                  { return gg200Compare(elem1,elem2)!=l_less; }
   /*!
     function: gg200IsLessOrEqual
    */
   inline bool gg200IsLessOrEqual( const T &elem1, const T &elem2 )
                  { return gg200Compare(elem1,elem2)!=l_greater; }
};
/*! endclass: cgg200_CComparisonElement */

/*!
  function: gg200QuickSort

  template arguments:
    ElementClass  The type of the elements to be sorted.
    CompareClass  The comparison operator's type.

  arguments:
    Elements         [in/out]
    NumberOfElements [in]
    Compare          [in] optional

  return value:    Success or failure.

  description:
    Non recursive quicksort implementation for sorting lists of elements.

    A list consists of an array of 'NumberOfElements' elements of type
    'ElementClass'. A direct memory access via 'Elements[Index]' must be
    possible.

    Version 1 of the function needs a user defined comparison-operator which
    has to be derived from the abstract class 'cgg200ComparisonOperator'.
    Version 2 uses the C++-defined operators '<' and '>' and is suitable
    for elements of basic types (int, float, char).

  code:
    // Call example 1 (basic type)
    int MyArray[500];
    gg200QuickSort(MyArray, 500);

  code:
    // Call example 2 (user-defined type):
    class CMyCompOp: public cgg200ComparisonOperator<MyClass>
    {  // your comparison routine here
    };
    MyClass MyArray[500];
    CMyCompOp MyCompOp;
    gg200QuickSort(MyArray, 500, MyCompOp);
*/
template <class ElementClass, class CompareClass>
bool gg200QuickSort(
					ElementClass		*Elements,
					const tsp00_Int4	NumberOfElements,
					CompareClass		&Compare);

template <class ElementClass>
bool gg200QuickSort(
					ElementClass		*Elements,
					const tsp00_Int4	NumberOfElements);

/*---------------------------------------------------------------------------*/

/*!
  function: gg200InsertionSort

  template arguments:
    ElementClass  The type of the elements to be sorted.
    CompareClass  The comparison operator's type.

  arguments:
    Elements         [in/out]
    NumberOfElements [in]
    Compare          [in]
    SortAscending    [in]

  returns: Success or failure.

  description:
    Sorting function for lists which are almost sorted. For all other lists,
    use 'gg200QuickSort'.

    A list consists of an array of 'NumberOfElements' elements of type
    'ElementClass'. A direct memory access via 'Elements[Index]' must be
    possible.
    The parameter 'SortAscending' defines the order,
      'true' means ascending,
      'false' means descending order.

   Version 1 of the function needs a user defined comparison operator which
   has to be derived from the abstract class cgg200ComparisonOperator.
   Version 2 uses the C++-defined operators '<' and '>' and is suitable for
   basic type elements (int, float, char).
*/
template <class ElementClass, class CompareClass>
bool gg200InsertionSort(
						ElementClass		*Elements,
						const tsp00_Int4	StartOffset,
						const tsp00_Int4	NumberOfElements,
						CompareClass		&Compare,
						const bool			SortAscending );

template <class ElementClass>
bool gg200InsertionSort(
						ElementClass		*Elements,
						const tsp00_Int4	StartOffset,
						const tsp00_Int4	NumberOfElements,
						const bool			SortAscending );

/*!
  endchapter: In-memory sorting
*/

/*---------------------------------------------------------------------------*/
/*!
  Chapter: External sorting

  description:
    Generic sort algorithms suitable for data from external media.

*/
/*---------------------------------------------------------------------------*/

/*!
  class: cgg200_MergeSource

  description:
    Base class for the source of the lists that should be
    merged via the 'gg200Merge' function.

  notes:
    The actual implementation might access arrays of elements in
    memory, or might get the elements from external media.

  template arguments:
    Element The type of the elements the source supplies.
*/
template <class Element>
class cgg200_MergeSource
{
  public:
   /*!
     function: gg200GetListCount

     description: Returns the number of lists that should be merged.
   */
   virtual tsp00_Uint4 gg200GetListCount() = 0;

   /*!
     function: gg200GetCurrentListElement

     description:
     Yields the current element from the specified list,
     if there is one.

     arguments:
       target   [out]  Target where element should be copied to.
       numList  [in]   Specifies the list from which to take
                       the element. Counting starts at 0.

     returns:        'True', if and only if an element has been
                     supplied.
   */
   virtual bool gg200GetCurrentListElement (unsigned int numList, Element & target ) = 0;

   /*!
     function: gg200GotoNextListElement

     description:
     Next element on the list becomes current element.

     arguments:
     numList  [in]   Specifies the list from which to take
       the element. Counting starts at 0.

   */
   virtual void gg200GotoNextListElement (unsigned int numList) = 0;


   /*!
     function: ~cgg200_MergeSource

     description: Destructor.
    */
   virtual ~cgg200_MergeSource(){};
};
/*! endclass: cgg200_MergeSource */

/*---------------------------------------------------------------------------*/

/*!
  class: cgg200_MergeTarget

  description:
    Base class for the target accepting the elements from
    the merging process in the 'gg200Merge' function.

  notes:
    The actual implementation might put the elements into arrays in memory
    or might put them onto external media.

  template arguments:
    Element The type of the elements the target accepts.
*/
template <class Element>
class cgg200_MergeTarget
{
  public:
   /*!
     function: gg200SetNextElement

     description: Sets the next element.

       This method will be called successively with elements in
       non-decreasing order.

     arguments: element [in] The next element output from the merging
                             process.

     returns: Whether the target might be supplied by
              further elements ('true') or not ('false').
   */
   virtual bool gg200SetNextElement (  Element element ) = 0;

   /*!
     function: ~cgg200_MergeTarget

     description: Destructor.
    */
   virtual ~cgg200_MergeTarget(){};
};
/*! endclass: cgg200_MergeTarget */

/*---------------------------------------------------------------------------*/

/*!
  function: gg200Merge

  description:
    Merges a set of ordered lists into a single ordered
    list.

    Nothing particular is assumed about the implementations of the
    lists or the elements on the lists, except that: - the elements
    are subject to an ordering relation, - the lists are sorted in
    non-decreasing order.

  notes:
    This function is suitable for
    in-memory- as well as for external sorting.
    The implementation is multi-way merging via a priority queue
    ('funnel sort'), cf.R.Sedgewick, 'Algorithmen in C++', 1992.

  author: Mirko.Luedde@SAP.Com

  created:   1999-08-24

  updated:   1999-09-13
  updated:   2001-07_23 AK

  template arguments:
    Element       The type of the elements to be sorted. Consider choosing
                  this to be a pointer type, if the actual elements are large.
    Comparator    The type of the object that compares the elements.
                  Must be derived from class
                  'cgg200ComparisonOperator<Element>'.

  arguments:
    source [in]   An object that supplies the elements from
                  all the lists to be merged.
    target [in]   An object that accepts the elements output
                  from the merge function.
    comparator [in] An object implementing the ordering relation for the
                    elements.
    pWorkingSpace [in] Pointer to working space.
    WorkingSpaceSize [in] Size of working space.
	                  If 'n' elements of type 't_element' should be
			  merged, we must have:
			  WorkingSpaceSize >=
			  n * sizeof(cgg200_MergeNode<t_element>)).

  returns:        Whether the merging could be successfully completed.
*/
template <class Element, class Comparator>
bool gg200Merge (
   cgg200_MergeSource<Element> & source,
   cgg200_MergeTarget<Element> & target,
   Comparator                  & comparator,
   void *                        pWorkingSpace,
   tsp00_Uint4                   WorkingSpaceSize
   );


/*!
  endchapter: External sorting
*/

/*!----------------------------------------------------------------------------
  Chapter: Stacks, Queues, ...
  ----------------------------------------------------------------------------*/

/*!----------------------------------------------------------------------------
  class: cgg200_PriorityQueue

  description:
    A priority queue suitable for holding a set of ordered elements and
    for removing the respective minimal elements one by one.

  notes:
    The implementation is similar to the one described in R.Sedgewick,
    'Algorithmen in C++', 1992.

  template arguments:
    Comparand   The type of the elements to be put into the queue.
    Comparator  The type of the implementation of the ordering relation for
                the elements.  Must be derived from class
                'cgg200ComparisonOperator<Comparand>'.

  author: Mirko.Luedde@SAP.Com

  created:   1999-08-24

  updated:   1999-09-13
  updated:   2001-07-23 AK

   ----------------------------------------------------------------------------*/
template <class Comparand, class Comparator>
class cgg200_PriorityQueue
{
  public:

   /*!----------------------------------------------------------------------------
     function: gg200GetCapacity

     description:
       Returns the number of elements of type 'Comparand'
       that still fit into the (currently allocated) memory of the queue.
       (However, advanced implementations might allocate more memory,
       when needed).
   ----------------------------------------------------------------------------*/
   tsp00_Uint4 gg200GetCapacity();

   /*!----------------------------------------------------------------------------
     function: gg200Insert

     description:
       Inserts a new element into the queue, if there is enough memory.

       (Future implementations might try to acquire more memory
       than already obtained, when needed.)

     arguments:
       element [in] Element to be inserted.

     returns: Success ('true') or failure ('false') of insertion.
   ----------------------------------------------------------------------------*/
   bool gg200Insert ( const Comparand &element );

   /*!----------------------------------------------------------------------------
     function: remove

     description:
       Returns a minimal element and removes it from the queue,
       if there is an element.

     arguments:
       Element [out]  contains the removed element

     returns: Whether an element was removed ('true') or not ('false').
             If 'false', nothing will have been written to '*(pElement)'.
   ----------------------------------------------------------------------------*/
   bool gg200Remove ( Comparand &Element );

   /*!----------------------------------------------------------------------------
     function: cgg200_PriorityQueue

     arguments:
       comparator [in]       An implementation of the ordering relation.
       pWorkingSpace [out]   Memory for the queue. Must be aligned
                              for elements of the type 'Comparand'.
			      The queue will be able to hold
			      'WorkingSpaceSize / sizeof(Comparand)'
			      elements.
       WorkingSpaceSize [in] Size of memory in bytes.
   ----------------------------------------------------------------------------*/
   cgg200_PriorityQueue (Comparator &comparator,
	                     void       *pWorkingSpace,
	                     tsp00_Uint4  WorkingSpaceSize);
   
   /*!----------------------------------------------------------------------------
     function   : cgg200_PriorityQueue
     description: default constructor
   ----------------------------------------------------------------------------*/
   cgg200_PriorityQueue (){};

   /*!----------------------------------------------------------------------------
     function: gg200Assign

     description: assigns the sources to merge and the working space to the iterator
   ----------------------------------------------------------------------------*/
   void gg200Assign (Comparator  &comparator,
	                 void        *pWorkingSpace,
	                 tsp00_Uint4  WorkingSpaceSize);

   /*!----------------------------------------------------------------------------
     function: gg200TopNode()

     description: returns the top node , i.e. the node with the smallest value
   ----------------------------------------------------------------------------*/
   Comparand & gg200TopNode();

   /*!----------------------------------------------------------------------------
     function: gg200IsEmpty()

     description: Displays if the queue contains any element
   ----------------------------------------------------------------------------*/
   bool gg200IsEmpty();

   /*!----------------------------------------------------------------------------
     function: gg200ReplaceTopNode()

     description: replaces the top node of the queue by the node given in the 
	              arguments and restores the consistency of the priority queue
   ----------------------------------------------------------------------------*/
   void gg200ReplaceTopNode (const Comparand &Element);

   /*!----------------------------------------------------------------------------
     function: gg200RemoveTopNode()

     description: removes the top node of the queue and
	              restores the consistency of the priority queue
   ----------------------------------------------------------------------------*/
   void gg200RemoveTopNode();

  private:
   /*!
     function: isLessOrEqual

     description: Convenient wrapper function for comparator object.
   */
   bool gg200_IsLessOrEqual ( const Comparand &e1, const Comparand &e2 );

   /*!
     function: gg200_MoveUp

     description: Moves an element up the heap until heap
       condition is satisfied. no checking.
   */
   void gg200_MoveUp ( unsigned int k );

   /*!
     function: gg200_MoveDown

     description: Moves an element down the heap until heap
       condition is satisfied. no checking.
   */
   void gg200_MoveDown ( unsigned int k );

   /*!
     variable: pHeap

     description: Pointer to array of elements.
   */
   Comparand * pHeap;

   /*!
      variable: size

      description: Currently allocated number of elements for the heap.
   */
   unsigned int size;

   /*!
     variable: usedSize

     description: Number of elements in the used part of the heap.
   */
   unsigned int usedSize;

   /*!
     variable: pComparator

     description: Pointer to object implementing the ordering relation.
   */
   Comparator * pComparator;
};
/*! endclass: cgg200_PriorityQueue */


/*!
  endchapter: Stacks, Queues, ...
*/

/*===========================================================================*
 * IMPLEMENTATIONS OF TEMPLATE FUNCTIONS                                      *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*!
  Chapter: Implementation details
*/
/*---------------------------------------------------------------------------*/

// InsertionSort-Implementation using a user-defined compare-operator
template <class ElementClass, class CompareClass>
bool gg200InsertionSort(
						ElementClass		*Elements,
						const tsp00_Int4	StartOffset,
						const tsp00_Int4	NumberOfElements,
						CompareClass		&Compare,
						const bool			SortAscending )
{
   tsp00_Int4	i;
   tsp00_Int4	j;
   tsp00_Int4	EndOffset;
   ElementClass v;

   EndOffset = StartOffset+NumberOfElements;

   for( i=StartOffset+1; i<EndOffset; i++ )
   {
      v = Elements[i];
      j = i;

      if( SortAscending )
         while( j>0 && Compare.gg200IsGreater( Elements[j-1], v ) )
         {
            Elements[j] = Elements[j-1];
            j--;
         }
      else
         while( j>0 && Compare.gg200IsLess( Elements[j-1], v )  )
         {
            Elements[j] = Elements[j-1];
            j--;
         }
      Elements[j] = v;
   }

   return true;
}

/*---------------------------------------------------------------------------*/

// InsertionSort implementation using the default compare-operators (<,>).
template <class ElementClass>
bool gg200InsertionSort(
						ElementClass		*Elements,
						const tsp00_Int4	StartOffset,
						const tsp00_Int4	NumberOfElements,
						const bool			SortAscending )
{
   tsp00_Int4	i;
   tsp00_Int4	j;
   tsp00_Int4	EndOffset;
   ElementClass v;

   EndOffset = StartOffset+NumberOfElements;
   for( i=StartOffset+1; i<EndOffset; i++ )
   {
      v = Elements[i];
      j = i;

      if( SortAscending )
         while( j>0 && Elements[j-1] > v )
         {
            Elements[j] = Elements[j-1];
            j--;
         }
      else
         while( j>0 && Elements[j-1] < v )
         {
            Elements[j] = Elements[j-1];
            j--;
         }
      Elements[j] = v;
   }

   return true;
}

/*---------------------------------------------------------------------------*/

// Quicksort implementation using a user-defined compare-operator.
template <class ElementClass, class CompareClass>
bool gg200QuickSort(
                    ElementClass		*Elements,
                    const tsp00_Int4	NumberOfElements,
                    CompareClass		&Compare)
{
    tsp00_Int4              i;
    tsp00_Int4	            j;
    tsp00_Int4	            Left;
    tsp00_Int4	            Right;
    tsp00_Int4              PivotIndex;
    tsp00_Int4              StackIndex = 0;
    ElementClass            AuxElem;
    ElementClass            PivotElem;
    tgg200_QuickSortItem    Stack[MAX_STACK_ELEMS_GG200];

    if (MAX_ELEMS_GG200 <= NumberOfElements) return false;
    if (0 > NumberOfElements) return false;

    if ((0 == NumberOfElements) || (Elements == NULL)) return true;

    Stack [StackIndex].qsiLeft_gg200  = 0;
    Stack [StackIndex].qsiRight_gg200 = NumberOfElements - 1;
    do
    {
        Left  = Stack [StackIndex].qsiLeft_gg200;
        Right = Stack [StackIndex].qsiRight_gg200;
        StackIndex --;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex = (Left + Right) >> 1;
            PivotElem  = Elements [PivotIndex];
            do
            {
                while (Compare.gg200IsLess (Elements [i], PivotElem)) i++;
                while (Compare.gg200IsLess (PivotElem, Elements [j])) j--;

                if (i <= j)
                {
                    AuxElem      = Elements [i];
                    Elements [i] = Elements [j];
                    Elements [j] = AuxElem;
                    i++;
                    j--;
                }
            }
            while (i <= j);

            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    if (MAX_STACK_ELEMS_GG200 <= StackIndex) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft_gg200  = i;
                    Stack [StackIndex].qsiRight_gg200 = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    if (MAX_STACK_ELEMS_GG200 <= StackIndex) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft_gg200  = Left;
                    Stack [StackIndex].qsiRight_gg200 = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
    return true;
}

/*---------------------------------------------------------------------------*/

// Quicksort implementation using the default compare operators (<,>).
template <class ElementClass>
bool gg200QuickSort(
					ElementClass		*Elements,
					const tsp00_Int4	NumberOfElements)
{
    tsp00_Int4              i;
    tsp00_Int4	            j;
    tsp00_Int4	            Left;
    tsp00_Int4	            Right;
    tsp00_Int4              PivotIndex;
    tsp00_Int4              StackIndex = 0;
    ElementClass            AuxElem;
    ElementClass            PivotElem;
    tgg200_QuickSortItem    Stack[MAX_STACK_ELEMS_GG200];

    if (MAX_ELEMS_GG200 <= NumberOfElements) return false;
    if (0 > NumberOfElements) return false;

    if ((0 == NumberOfElements) || (Elements == NULL)) return true;

    Stack [StackIndex].qsiLeft_gg200  = 0;
    Stack [StackIndex].qsiRight_gg200 = NumberOfElements - 1;
    do
    {
        Left  = Stack [StackIndex].qsiLeft_gg200;
        Right = Stack [StackIndex].qsiRight_gg200;
        StackIndex --;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex = (Left + Right) >> 1;
            PivotElem  = Elements [PivotIndex];
            do
            {
                while (Elements [i] < PivotElem) i++;
                while (PivotElem    < Elements [j]) j--;

                if (i <= j)
                {
                    AuxElem      = Elements [i];
                    Elements [i] = Elements [j];
                    Elements [j] = AuxElem;
                    i++;
                    j--;
                }
            }
            while (i <= j);

            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    if (MAX_STACK_ELEMS_GG200 <= StackIndex) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft_gg200  = i;
                    Stack [StackIndex].qsiRight_gg200 = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    if (MAX_STACK_ELEMS_GG200 <= StackIndex) return false;
                    StackIndex++;
                    Stack [StackIndex].qsiLeft_gg200  = Left;
                    Stack [StackIndex].qsiRight_gg200 = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
    return true;
}

/*---------------------------------------------------------------------------*/

/*!
  class: cgg200_MergeNode

  description:
    For internal use by the 'gg200Merge' function only.

    The nodes to be put into the priority queue. The nodes must know
    about the list from which the element originated.
*/
template <class Element>
class cgg200_MergeNode
{
  public:
   Element element;
   unsigned int numOfList;
};
/*! endclass: cgg200_MergeNode */

/*---------------------------------------------------------------------------*/

/*!
  class: cgg200_MergeComparator

  description:
    For internal use by the 'gg200Merge' function only.

    The class to compare 'cgg200_MergeNode<Element>' objects.
*/

template <class Element, class Comparator>
class cgg200_MergeComparator :
public cgg200ComparisonOperator< cgg200_MergeNode<Element> >
{
  public:
      inline tsp00_LcompResult_Enum gg200Compare(
          const cgg200_MergeNode<Element> &n1,
          const cgg200_MergeNode<Element> &n2 )
      {
          return
              this->pComparator->gg200Compare
              ( n1.element, n2.element );
      }

      cgg200_MergeComparator( Comparator & comparator )
      {
          this->pComparator = &(comparator);
      }

  private:
      Comparator * pComparator;
};
/*!  endclass: cgg200_MergeComparator */

/*---------------------------------------------------------------------------*/

template <class Element, class Comparator>
bool gg200Merge (
   cgg200_MergeSource<Element> & source,
   cgg200_MergeTarget<Element> & target,
   Comparator                  & comparator,
   void *                        pWorkingSpace,
   tsp00_Uint4                   WorkingSpaceSize
   )
{
	unsigned int	n = source.gg200GetListCount();
	
	cgg200_MergeComparator<Element, Comparator>
		mergeComparator ( comparator );
	
	cgg200_PriorityQueue<
		cgg200_MergeNode<Element>,
		cgg200_MergeComparator<Element, Comparator>
		>
		queue (
		mergeComparator,
		pWorkingSpace,
		WorkingSpaceSize
		);

	cgg200_MergeNode<Element> node;
	
	if ( ! (queue.gg200GetCapacity() >= n) )
	{ /* insufficient memory */
		return false;
	}
	
	/* initialize the queue */
	for ( node.numOfList = 0; node.numOfList < n; ++ (node.numOfList) )
	{ 
		if ( source.gg200GetCurrentListElement( node.numOfList, node.element )
			== true )
		{
			/* there was an element on the list */
			
			/* put it into the queue. need not check return code since
			queue guarantees to hold 'n' elements. */
			queue.gg200Insert ( node );
		}
	}
	
	while (! queue.gg200IsEmpty())
	{ /* there was an element in the queue */
		
		/* put it into the target */
		if (false == target.gg200SetNextElement (queue.gg200TopNode().element))
		{
			/* stop merge since the target is full */
			break;
		}
		else
		{
			/* find the list where to get the next queue element */
			node.numOfList = queue.gg200TopNode().numOfList;
			source.gg200GotoNextListElement (node.numOfList);
			
			if (true == source.gg200GetCurrentListElement (node.numOfList, node.element))
			{		
				/* replace the top element of the queue and restore it  */
				queue.gg200ReplaceTopNode (node);
			}
			else
			{
				/* remove the top element of the queue and restore it  */
				queue.gg200RemoveTopNode();
			}
		}	
	}
	return true;
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
inline bool cgg200_PriorityQueue<Comparand,Comparator>::gg200Insert
( const Comparand &element )
{
   if ( this->usedSize >= this->size )
   { /* no memory. future implementations might try to obtain more
       memory. */

      return false;
   }

   /* append element */
   (this->pHeap)[this->usedSize] = element;

   ++ (this->usedSize);

   /* move element up, if necessary */
   this->gg200_MoveUp (this->usedSize - 1);

   return true;
}



/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
bool cgg200_PriorityQueue<Comparand,Comparator>::gg200IsEmpty()
{
   return (this->usedSize == 0);
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
bool cgg200_PriorityQueue<Comparand,Comparator>::gg200Remove
( Comparand &Element )
{
   if ( this->usedSize == 0 )
   { /* nothing in the heap */
      return false;
   }

   /* top element to be returned */
   Element = (this->pHeap)[0];

   -- (this->usedSize);

   /* put last element on top */
   (this->pHeap)[0] = (this->pHeap)[this->usedSize];

   /* move element down, if necessary */
   this->gg200_MoveDown (0);

   return true;
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
void cgg200_PriorityQueue<Comparand,Comparator>::gg200RemoveTopNode()
{
   if (this->usedSize == 0)
      return ;

   -- (this->usedSize);

   /* put last element on top */
   (this->pHeap)[0] = (this->pHeap)[this->usedSize];

   /* move element down, if necessary */
   this->gg200_MoveDown (0);
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
void cgg200_PriorityQueue<Comparand,Comparator>::gg200ReplaceTopNode
(const Comparand &Element )
{
   /* replace top element */
   (this->pHeap)[0] = Element;

   /* move element down, if necessary */
   this->gg200_MoveDown (0);
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
inline void cgg200_PriorityQueue<Comparand,Comparator>::gg200Assign
(
 Comparator & comparator,
 void *       pWorkingSpace,
 tsp00_Uint4  WorkingSpaceSize
 )
{
	this->pComparator = &(comparator);
	this->pHeap = (Comparand *) pWorkingSpace;
	this->usedSize = 0;
	if (this->pHeap != 0)
	{
		this->size = WorkingSpaceSize / sizeof(Comparand);
	}
	else
	{
		this->size = 0;
	}
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
inline Comparand & cgg200_PriorityQueue<Comparand,Comparator>::gg200TopNode()
{
	return this->pHeap[0];
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
cgg200_PriorityQueue<Comparand,Comparator>::cgg200_PriorityQueue
(
   Comparator & comparator,
   void *       pWorkingSpace,
   tsp00_Uint4  WorkingSpaceSize
   )
{
   gg200Assign(comparator, pWorkingSpace, WorkingSpaceSize);
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
inline tsp00_Uint4 cgg200_PriorityQueue<Comparand,Comparator>::gg200GetCapacity
()
{
   return ( (this->size) - (this->usedSize) );
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
inline bool cgg200_PriorityQueue<Comparand,Comparator>::gg200_IsLessOrEqual
( const Comparand &e1, const Comparand &e2 )
{
   return
      (this->pComparator)->gg200IsLessOrEqual( e1, e2 );
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
void cgg200_PriorityQueue<Comparand,Comparator>::gg200_MoveUp (unsigned int k)
{
   /* element to be moved up */
   Comparand e = (this->pHeap)[k];

   while ( k != 0 )
   { /* has a parent */

      /* parent node */
      unsigned int j = (k-1)/2;

      if ( this->gg200_IsLessOrEqual( (this->pHeap)[j], e ) )
      { /* heap condition fulfilled */
	 break;
      }

      /* move down parent */
      (this->pHeap)[k] = (this->pHeap)[j];

      k = j;
   }

   (this->pHeap)[k] = e;
}

/*---------------------------------------------------------------------------*/

template <class Comparand, class Comparator>
void cgg200_PriorityQueue<Comparand,Comparator>::gg200_MoveDown
( unsigned int k )
{
   /* to be moved down */
   Comparand e = (this->pHeap)[k];

   unsigned int j;

   while ( k < (this->usedSize / 2) )
   {  
   /* Node 'k' has children. The condition we intuitively might
	  want to test is '1 + 2*k < usedSize'. But this is the proper
	  condition only in an infinite cyclic ring and we must take care of
	   overflow in our finite cyclic ring. */
	   
	   /* first child */
	   j = 1 + 2*k;
	   
	   if ( ( j < (this->usedSize - 1) )
		   /* 'usedSize' may not be 0 here. */
		   &&
		   ! ( this->gg200_IsLessOrEqual( (this->pHeap)[j],
		   (this->pHeap)[1+j] ) ) )
	   { /* there is a child '1+j' and it is smaller than child 'j' */
		   ++ j;
	   }
	   
	   if ( this->gg200_IsLessOrEqual ( e, (this->pHeap)[j] ) )
	   { /* heap condition fulfilled */
		   break;
	   }
	   
	   /* move up smallest child */
	   (this->pHeap)[k] = (this->pHeap)[j];
	   
	   k = j;
   }
   
   (this->pHeap)[k] = e;
}


/*!
  endchapter: Implementation details
*/

/*!--------------------------------------------------------------------------
  chapter: Iterator
  ---------------------------------------------------------------------------*/

/*! -------------------------------------------------------------------------
class: gg200MergeIterator 

description:
    runs over a set of ordered lists and supplies their elements 
	in a order defined by a given comparator.

    Nothing particular is assumed about the implementations of the
    lists or the elements on the lists, except that: - the elements
    are subject to an ordering relation, - the lists are sorted in
    non-decreasing order.

  notes:
    This function is suitable for
    in-memory- as well as for external sorting.
    The implementation is multi-way merging via a priority queue
    ('funnel sort'), cf.R.Sedgewick, 'Algorithmen in C++', 1992.

  author : AK
  created:   2001-07_23 


  template arguments:
    Element       The type of the elements to be sorted. Consider choosing
                  this to be a pointer type, if the actual elements are large.
    Comparator    The type of the object that compares the elements.
                  Must be derived from class
                  'cgg200ComparisonOperator<Element>'.

-------------------------------------------------------------------------*/

template <class Element, class Comparator> class gg200MergeIterator
{
public:

	/*!-------------------------------------------------------------------------
	function:     gg200MergeIterator()
	description:  constructor
		   
	arguments:     comparator [in] An object implementing the ordering relation for the
                                   lis elements.
	return value: none
    --------------------------------------------------------------------------*/
	gg200MergeIterator (Comparator                  & comparator)
	:
	m_MergeComparator (comparator)
	{}

	/*!-------------------------------------------------------------------------
	function:     gg200Assign()
	description:  constructor
		   
	arguments: source           [in] An object that supplies the elements from
                                     all the lists to be merged.
               pWorkingSpace    [in] Pointer to working space.
               WorkingSpaceSize [in] Size of working space.
	                                 If 'n' elements of type 't_element' should be
			                         merged, we must have:
			                         WorkingSpaceSize >=
			                         n * sizeof(cgg200_MergeNode<t_element>)).
	return value: none
    --------------------------------------------------------------------------*/
	bool gg200Assign (cgg200_MergeSource<Element> & source,
		              void *                        pWorkingSpace,
		              tsp00_Uint4                   WorkingSpaceSize)
	{
		m_pSource = &source;
		unsigned int	n = m_pSource->gg200GetListCount();
        		
        m_Queue.gg200Assign (m_MergeComparator, pWorkingSpace, WorkingSpaceSize);

		/* check if there is sufficient memory */
		if ( ! (m_Queue.gg200GetCapacity() >= n) )
			return false;
		
		/* initialize the queue */
		for ( m_Node.numOfList = 0; m_Node.numOfList < n; ++ (m_Node.numOfList) )
		{ 
			/* check if there is  any element on this source list */
			if (m_pSource->gg200GetCurrentListElement (m_Node.numOfList, m_Node.element) == true )
			{
				/* put it into the queue. need not check return code since
				m_Queue guarantees to hold 'n' elements. */
				m_Queue.gg200Insert ( m_Node );
			}
		}
		return true;
	}

    /*!-------------------------------------------------------------------------
	function:     operator *()
	description:  dereferencing operator
		   
	arguments: none
	return value: (Element) returns the element in the lists at which the the iterator is currently pointing 
    --------------------------------------------------------------------------*/
	Element & operator *()
	{
		return m_Queue.gg200TopNode().element;
	}

	/*!-------------------------------------------------------------------------
	function:     IsReady()
	description:  displays whether the end of the iterator scope is reached 
		   
	arguments:     none
	return value: (bool) true if the end of the iterator scope is reached  
    --------------------------------------------------------------------------*/
	bool IsReady()
	{
		return m_Queue.gg200IsEmpty();
	} 

	/*!-------------------------------------------------------------------------
	function:     operator ++ ()
	description:  move the iterator to the next larger element in the lists
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
	void operator ++ ()
	{
		/* find source where to get the next elemt which is to insert into the queue */
		m_Node.numOfList = m_Queue.gg200TopNode().numOfList;
		
		/*  set the appropriate iterator to the next element to insert into the queue */
		m_pSource->gg200GotoNextListElement (m_Node.numOfList);
		
		/* check if there is still an element on this source list */
		if (m_pSource->gg200GetCurrentListElement (m_Node.numOfList, m_Node.element)== true )
		{				
		   /* replace the top element of the queue and restore the queue  */
			m_Queue.gg200ReplaceTopNode (m_Node);
		}
		else
		{
		   /* remove the top element of the queue and restore the queue  */
		   m_Queue.gg200RemoveTopNode();
		}
	}

	/* void operator ++ () = previous and slower implementation see PTS 1111038
	{
		m_Queue.gg200Remove (m_Node);
		m_pSource->gg200GotoNextListElement (m_Node.numOfList);
		
		if (m_pSource->gg200GetCurrentListElement (m_Node.numOfList, m_Node.element)== true )
			m_Queue.gg200Insert ( m_Node );
	}
    */  

private:	
	cgg200_MergeComparator<Element, Comparator>                                                    m_MergeComparator;
	cgg200_MergeNode<Element>                                                                      m_Node;
    cgg200_PriorityQueue< cgg200_MergeNode<Element>, cgg200_MergeComparator<Element, Comparator> > m_Queue;
	cgg200_MergeSource<Element>                                                                   *m_pSource;
};

/*! -------------------------------------------------------------------------
  endclass: gg200MergeIterator 
  ---------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------
  endchapter: Iterator
  ---------------------------------------------------------------------------*/



#endif  /* GGG200_H */



