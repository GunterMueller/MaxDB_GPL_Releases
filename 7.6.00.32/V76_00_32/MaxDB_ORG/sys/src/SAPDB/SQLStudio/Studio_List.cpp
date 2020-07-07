/*!===========================================================================

  module:       Studio_List.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS

  special area: Common

  description:  Implementation of the doubly linked list class Studio_List.

  see:          

  created:      2000-08-17

  updated:      2000-08-17

  ----------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG
  



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




============================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/Studio_List.hpp"


/*===========================================================================*
 *     Studio_List::Studio_List                                                *
 *===========================================================================*/
Studio_List::Studio_List ()
{
  mFirst   = 0;
  mLast    = 0;
  mCurrent = 0;
  mSize    = 0;
}


/*===========================================================================*
 *     Studio_List::Studio_List                                                *
 *===========================================================================*/
Studio_List::Studio_List (const Studio_List& list)
{
  mFirst   = list.mFirst;
  mLast    = list.mLast;
  mCurrent = list.mCurrent;
  mSize    = list.mSize;
}


/*===========================================================================*
 *     Studio_List::~Studio_List                                               *
 *===========================================================================*/
Studio_List::~Studio_List ()
{
  mCurrent = mFirst;
  
  while (mSize > 0)
    remove();
}


/*===========================================================================*
 *     Studio_List::append                                                    *
 *===========================================================================*/
void Studio_List::append (Studio_ListData data)
{
  Studio_ListElement *element;
  
  /*-------------------------------------------------------------------------*
   * Create a new list element.                                              *
   *-------------------------------------------------------------------------*/
  element = new Studio_ListElement;
  element->mData = data;
  
  /*-------------------------------------------------------------------------*
   * Change the pointers.                                                    *
   *-------------------------------------------------------------------------*/
  if (mLast)
    mLast->mNext = element;
  else
    mFirst = element;

  element->mNext = 0;
  element->mPrev = mLast;
  
  mLast = mCurrent = element;

  /*-------------------------------------------------------------------------*
   * Update the size.                                                        *
   *-------------------------------------------------------------------------*/
  mSize++;
}


/*===========================================================================*
 *     Studio_List::insert                                                    *
 *===========================================================================*/
void Studio_List::insert (Studio_ListData data, SAPDB_UInt index)
{
  SAPDB_UInt          i;
  Studio_ListElement *element;
  
  /*-------------------------------------------------------------------------*
   * Create a new list element.                                              *
   *-------------------------------------------------------------------------*/
  element = new Studio_ListElement;
  element->mData = data;
  
  /*-------------------------------------------------------------------------*
   * If the insert position is behind the last element --> call the append   *
   * method.                                                                 *
   *-------------------------------------------------------------------------*/
  if (index >= mSize)
  {
    append (element);
    return;
  }

  /*-------------------------------------------------------------------------*
   * Position the pointer to the current element to the insertion point.     *
   *-------------------------------------------------------------------------*/
  mCurrent = mFirst;

  for (i = 0; i < index; i ++)
    next();

  /*-------------------------------------------------------------------------*
   * Change the pointers.                                                    *
   *-------------------------------------------------------------------------*/
  if (mCurrent->mPrev)
    mCurrent->mPrev->mNext = element;
  else
    mFirst = element;

  mCurrent->mPrev = element;
  element->mNext  = mCurrent;
  element->mPrev  = mCurrent->mPrev;
  
  mCurrent = element;

  /*-------------------------------------------------------------------------*
   * Update the size.                                                        *
   *-------------------------------------------------------------------------*/
  mSize++;
}


/*===========================================================================*
 *     Studio_List::remove                                                    *
 *===========================================================================*/
Studio_ListData Studio_List::remove ()
{
  Studio_ListPtr     newCurrent;
  Studio_ListData    data;
  
  /*-------------------------------------------------------------------------*
   * Check if the current pointer is valid.                                  *
   *-------------------------------------------------------------------------*/
  if (!mCurrent)
    return 0;

  /*-------------------------------------------------------------------------*
   * Change the pointers.                                                    *
   *-------------------------------------------------------------------------*/
  if (mCurrent->mPrev)
    mCurrent->mPrev->mNext = mCurrent->mNext;

  if (mCurrent->mNext)
  {
    mCurrent->mNext->mPrev = mCurrent->mPrev;
    newCurrent = mCurrent->mNext;
  }
  else
    newCurrent = mCurrent->mPrev;

  if (mCurrent == mLast)
    mLast = mCurrent->mPrev;

  if (mCurrent == mFirst)
    mFirst = mCurrent->mNext;
  
  mCurrent->mNext = 0;
  mCurrent->mPrev = 0;
  
  /*-------------------------------------------------------------------------*
   * Update the size.                                                        *
   *-------------------------------------------------------------------------*/
  mSize--;
  
  /*-------------------------------------------------------------------------*
   * If the flag was set, the list elements memory is freed.                 *
   *-------------------------------------------------------------------------*/  
  data = mCurrent->mData;
  delete mCurrent;
  mCurrent = newCurrent;

  return data;
}


/*===========================================================================*
 *     Studio_List::remove                                                    *
 *===========================================================================*/
Studio_ListData Studio_List::remove (SAPDB_UInt index, SAPDB_UInt number)
{
  SAPDB_UInt      i, n;
  Studio_ListData  last;
  
  /*-------------------------------------------------------------------------*
   * If the position is behind the last element --> return a 0 pointer.   *
   *-------------------------------------------------------------------------*/
  if (index >= mSize)
    return 0;

  /*-------------------------------------------------------------------------*
   * Position the pointer to the current element to the given position.      *
   *-------------------------------------------------------------------------*/
  mCurrent = mFirst;

  for (i = 0; i < index; i ++)
    next();

  /*-------------------------------------------------------------------------*
   * For each element to remove call the above remove method.                *
   *-------------------------------------------------------------------------*/
  if (index + number > mSize)
    n = mSize - index;
  else
    n = number;
  
  for (i = 0; i < n; i ++)
    last = remove ();
  
  return last;
}


/*===========================================================================*
 *     Studio_List::remove                                                    *
 *===========================================================================*/
Studio_ListData Studio_List::remove (Studio_ListData element)
{
  SAPDB_UInt      i;
  
  /*-------------------------------------------------------------------------*
   * Search the list for the given element.                                  *
   *-------------------------------------------------------------------------*/
  mCurrent = mFirst;
  
  for (i = 0; i < mSize; i++)
  {
    if (mCurrent->mData == element)
      return remove ();

    next();
  }
  
  return 0;
}


/*===========================================================================*
 *     Studio_List::clear                                                     *
 *===========================================================================*/
void Studio_List::clear ()
{
  mCurrent = mFirst;
  
  while (mSize > 0)
    remove();

  mFirst = mLast = mCurrent = 0;
}


/*===========================================================================*
 *     Studio_List::twist                                                     *
 *===========================================================================*/
void Studio_List::twist (SAPDB_UInt index)
{
  SAPDB_UInt      i;
  Studio_ListPtr   item1, item2, item3, item4;
  
  /*-------------------------------------------------------------------------*
   * If the given index is the last element or behind nothing has to be done.*
   *-------------------------------------------------------------------------*/
  if (index >= mSize - 1)
    return;
  
  /*-------------------------------------------------------------------------*
   * Position the pointer to the current element to the given position.      *
   *-------------------------------------------------------------------------*/
  mCurrent = mFirst;
  
  for (i = 0; i < index; i++)
    next();

  /*-------------------------------------------------------------------------*
   * Exchange the current element with the next element.                     *
   *-------------------------------------------------------------------------*/
  item1 = mCurrent->mPrev;
  item2 = mCurrent;
  item3 = mCurrent->mNext;
  item4 = item3 ? mCurrent->mNext->mNext : 0;

  if (item1)
  {
    item1->mNext = item3;
    item3->mPrev = item1;
  }
  else
  {
    item3->mPrev = 0;
    mFirst = item3;
  }

  if (item4)
  {
    item2->mNext = item4;
    item4->mPrev = item2;
  }
  else
  {
    item2->mNext = 0;
    mLast = item2;
  }
  
  item3->mNext = item2;
  item2->mPrev = item3;
}


/*===========================================================================*
 *     Studio_List::first                                                     *
 *===========================================================================*/
Studio_ListData Studio_List::first ()
{
  mCurrent = mFirst;
  return (mCurrent ? mCurrent->mData : 0);
}


/*===========================================================================*
 *     Studio_List::last                                                      *
 *===========================================================================*/
Studio_ListData Studio_List::last ()
{
  mCurrent = mLast;
  return (mCurrent ? mCurrent->mData : 0);
}


/*===========================================================================*
 *     Studio_List::next                                                      *
 *===========================================================================*/
Studio_ListData Studio_List::next ()
{
  if (!mCurrent) return 0;

  if (mCurrent->mNext)
  {
    mCurrent = mCurrent->mNext;
    return mCurrent->mData;
  }
  else
    return 0;
}


/*===========================================================================*
 *     Studio_List::prev                                                      *
 *===========================================================================*/
Studio_ListData Studio_List::prev ()
{
  if (!mCurrent) return 0;

  if (mCurrent->mPrev)
  {
    mCurrent = mCurrent->mPrev;
    return mCurrent->mData;
  }
  else
    return 0;
}


/*===========================================================================*
 *     Studio_List::at                                                        *
 *===========================================================================*/
Studio_ListData Studio_List::at (SAPDB_UInt index)
{
  Studio_ListPtr  item;
  SAPDB_UInt     i;
  
  /*-------------------------------------------------------------------------*
   * If the given index is greater than the list size --> return NULL.       *
   *-------------------------------------------------------------------------*/
  if (index >= mSize)
    return 0;
  
  /*-------------------------------------------------------------------------*
   * Position the pointer to the current element to the given position.      *
   *-------------------------------------------------------------------------*/
  item = mFirst;
  for (i = 0; i < index; i++)
    item = item->mNext;
  
  return item->mData;
}
