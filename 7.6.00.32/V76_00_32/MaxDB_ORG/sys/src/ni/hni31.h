/*!*****************************************************************************
  module:       hni31.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  basic types for ni30.cpp,...,ni35.cpp
  last change:  1999-02-24 14:18
  version:      7.2
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 1999-2005 SAP AG



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




*******************************************************************************/

#ifndef _HNI31_H_
#define _HNI31_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include"hni00b.h"


// -----------------------------------------------------------------------------
// class ni31_ListElem
// -----------------------------------------------------------------------------

template<class infotype> class ni31_ListElem
{
    /*--------------------------------------------------------------------------
    public function: constructor

    description:  constructor, creates a ni31_ListElem containing a copy(!) of
                  the argument

    arguments:    c      [in] reference to original information

    return value: none
    --------------------------------------------------------------------------*/
    /*--------------------------------------------------------------------------
    public function: constructor

    description:  copy constructor, creates a ni31_ListElem containing a copy(!)
                  of the contents of the argument (== a copy of the argument)

    arguments:    o      [in] reference to original ni31_ListElem

    return value: none
    --------------------------------------------------------------------------*/
    /*!-------------------------------------------------------------------------
    public operator: infotype()

    description:  returning an constant copy of the information containend

    arguments:    none

    return value: infotype information contained in the object
    --------------------------------------------------------------------------*/
   /*--------------------------------------------------------------------------
    public function: GiveInfo()

    description:  returning a constant copy of the information containend

    arguments:    none

    return value: infotype information contained in the object
    --------------------------------------------------------------------------*/
  private:
    infotype   info;
    ni31_ListElem<infotype> * next;
  public:
    ni31_ListElem(const infotype &c);
    ni31_ListElem(ni31_ListElem<infotype> &o);
    
    ni31_ListElem<infotype> *&              Next();
    const ni31_ListElem<infotype> * /*const &*/ Next() const;
    const infotype                          GiveInfo() const;

         operator const infotype () const;
    void operator =              (const infotype &c);
};

// -----------------------------------------------------------------------------
// class ni31_List
// -----------------------------------------------------------------------------

template<class infotype> class ni31_List
{
  private:
    ni31_ListElem<infotype> *first;  
  public:
    ni31_List(void);
    ni31_List(const ni31_List<infotype> &o);
    ~ni31_List(void);

    ni31_ListElem<infotype> *       First();
    const ni31_ListElem<infotype> * First() const;
    ni31_ListElem<infotype> *       Add(const infotype &i);
    ni31_ListElem<infotype> *       Add(const infotype &i,ni31_ListElem<infotype> &p);
    //ni31_ListElem<infotype> * Add(const infotype &i,ni31_ListElem<infotype> *p);

    void MoveAllToBeginOf(ni31_List<infotype> & DestinationList);

    void Del(ni31_ListElem<infotype>&p);
    void Del();
    void Del(int n);
    void DelAll();

    void Reverse();

    unsigned long Length();
    int           IsEmpty();
    int           IsNotEmpty();

    ni31_ListElem<infotype> *   operator[](unsigned i);
    ni31_List<infotype> &       operator <<(const infotype &i);
    const ni31_List<infotype> & operator = (const ni31_List<infotype> &l);
    //int                       operator == (const ni31_List<infotype> &l) const; //needed ?
};

/*
template<class infotype> class Vector
{
 private:
  unsigned   size  ;
  infotype * vector;
  
 public:
  Vector();
  Vector(unsigned s);
  ~Vector();
  
  infotype &         operator [](unsigned i);
  Vector<infotype> & operator =(Vector<infotype> &s);
  void               SizeTo(unsigned s);
  void               ResizeTo(unsigned s);
  unsigned           Size() const;
};

template <class infotype> class Matrix
{
   private:
     unsigned NoRows,NoColumns;
     infotype * Mat;

   public:
     Matrix() 
       :NoRows(0),NoColumns(0),Mat(0)
	 {}
     Matrix(unsigned n,unsigned m)
       :NoRows(n),NoColumns(m)
       {
	 if(NoRows>0 && NoColumns>0)
           Mat=new infotype[NoRows*NoColumns];
       }

     ~Matrix()
       {if(NoRows>0 && NoColumns>0)delete [] Mat;}

     void SizeTo(unsigned n,unsigned m)
       {
         NoRows=n;
         NoColumns=m;
         if(Mat!=0)
           delete [] Mat;
         if(n*m>0) 
           Mat=new infotype[NoRows*NoColumns];
         else
           Mat=0;
       }  

     infotype * operator[](unsigned i)
       {
	 return Mat+i*NoColumns;
       }

     unsigned int Columns(void)
       {return NoColumns;} 

     unsigned int Rows(void)
       {return NoRows;}
 };
*/

template<class infotype>
inline ni31_ListElem<infotype>::ni31_ListElem(const infotype &c)
{
  info=c;
}
 
template<class infotype>
inline ni31_ListElem<infotype>::ni31_ListElem(ni31_ListElem<infotype> &o)
{
  info=o.info;
}

template<class infotype>
inline const infotype ni31_ListElem<infotype>::GiveInfo() const
{
    return info;
}

template<class infotype>
inline ni31_ListElem<infotype>::operator const infotype() const
{
  return info;
}

template<class infotype>
inline void ni31_ListElem<infotype>::operator =(const infotype &c)
{
  info=c;
}  

template<class infotype>
inline ni31_ListElem<infotype> *& ni31_ListElem<infotype>::Next(void)
{
  return next;
}

template<class infotype>
inline const ni31_ListElem<infotype> * /*const &*/ ni31_ListElem<infotype>::Next() const
{
    return next;
}

template<class infotype> inline ni31_List<infotype>::ni31_List(void)
{
  first=0;
}

template<class infotype> inline ni31_List<infotype>::ni31_List(const ni31_List<infotype> &o)
{
  ni31_ListElem<infotype> *l,*lo;
  
  lo=o.first;
  if(o.first!=0)
    {
      l=first=new ni31_ListElem<infotype>(*(o.first));
      lo=lo->Next();   
      while(lo!=0)
	{
	  l->Next()=new ni31_ListElem<infotype>(*(lo));
	  l=l->Next();
	  lo=lo->Next();   
	}
      l->Next()=0;
    }
  else
    first=0;
}

template<class infotype> inline ni31_List<infotype>::~ni31_List(void)
{ 
  ni31_ListElem<infotype> *h=first,*n;
  
  n=h;
  while(0!=h)
    { 
      n=h;
      h=h->Next();
      delete n;
    }
}

template<class infotype>
inline ni31_ListElem<infotype> * ni31_List<infotype>::First(void)
{
  return first;
}

template<class infotype> inline ni31_ListElem<infotype> const * ni31_List<infotype>::First(void) const
{
  return first;
}

template<class infotype>
inline ni31_ListElem<infotype> * ni31_List<infotype>::operator[](unsigned i)
{
    ni31_ListElem<infotype> *h=first;

    while((i--)>0)
        h=h->Next();

    return h;  
}

template<class infotype>
inline ni31_ListElem<infotype> * ni31_List<infotype>::Add(const infotype &i)
{
    /*
    ni31_ListElem<infotype> *h=first;
    first=new ni31_ListElem<infotype>(i);
    first->Next()=h;
    return first;*/

    ni31_ListElem<infotype> *h=new ni31_ListElem<infotype>(i);

    if(0!=h)
    {
        h->Next()=first;
        first=h;
    }

    return h;
}

template<class infotype> inline ni31_List<infotype> & ni31_List<infotype>::operator <<(const infotype &i)
{
    this->Add(i);
    return *this;
}

template<class infotype>
inline ni31_ListElem<infotype> * ni31_List<infotype>::Add(const infotype &i, ni31_ListElem<infotype> &p)
{ 
    /*
      ni31_ListElem<infotype> *h=new ni31_ListElem<infotype>(i);
      h->Next()=p.Next();
      p.Next()=h;
      return h;
    */
    ni31_ListElem<infotype> *h=new ni31_ListElem<infotype>(i);

    if(0!=h)
    {
        h->Next()=p.Next();
        p.Next()=h;
    }

    return h;
}

/*
template<class infotype>
inline ni31_ListElem<infotype> * ni31_List<infotype>::Add(const infotype &i,
						ni31_ListElem<infotype> *p)
{
  ni31_ListElem<infotype> *h=new ni31_ListElem<infotype>(i);
  h->Next()=p->Next();
  p->Next()=h;
  return h; 
}
*/

template<class infotype>
inline void ni31_List<infotype>::MoveAllToBeginOf(ni31_List<infotype> & DestinationList)
{
    if(0!=first) //if there is at least one element to move
    {
        // find the last element of this list
        ni31_ListElem<infotype> *Last=first;
        
        while(0!=Last->Next())
            Last=Last->Next();

        //concatenate this list with DestinationList
        Last->Next()=DestinationList.first;
        
        //change begin of DestinationList
        DestinationList.first=first;

        // move -> this list is empty now
        first=0;
    }
}
	
template<class infotype>
inline void ni31_List<infotype>::Del(ni31_ListElem<infotype>&p)
{
  ni31_ListElem<infotype> *h=p.Next();
  p.Next()=h->Next();
  delete h;   
}
     
template<class infotype> inline void ni31_List<infotype>::Del(void)
{
    ni31_ListElem<infotype> *h=first;

    if(h!=0)
    {
        first=h->Next();
        delete h;
    }
}

template<class infotype> inline void ni31_List<infotype>::Del(int n)
{
    ni31_ListElem<infotype> *h=first; 
    while(h!=0 && n-->0)
    {
        first=h->Next();
        delete h;
        h=first;
    }
}

template<class infotype> inline void ni31_List<infotype>::DelAll(void)
{
  ni31_ListElem<infotype> *h;
  while(first!=0)
    {
      h=first;
      first=first->Next();
      delete h;
    }
}

template<class infotype>
void ni31_List<infotype>::Reverse()
{
    if(0!=first) //is anything in the list?
    {
        ni31_ListElem<infotype> *OldPrevious=first,
                                *ElementToChange=first->Next();

        if(0!=ElementToChange) //is more than one element in the list?
        {
            ni31_ListElem<infotype> *OldNext=ElementToChange->Next();
            first->Next()=0; //the first element will be the last

            while(0!=ElementToChange) //while we have an element to change
            {
                ElementToChange->Next()=OldPrevious; //change it

                //go on in the list
                OldPrevious=ElementToChange;
                ElementToChange=OldNext;

                if(0!=OldNext)
                {    
                    OldNext=OldNext->Next();
                }
            }

            first=OldPrevious; //OldPrevious contains now the last changed element
        }
    }
}

template<class infotype>
inline const ni31_List<infotype> & ni31_List<infotype>::operator = (const ni31_List<infotype> &l)
{
  ni31_ListElem<infotype> *h,*hnew;
  
  while(first!=0)
    {
      h=first;
      first=first->Next();
      delete h;
    }
  h=l.first;
  if(h!=0)
    { 
      first=hnew=new ni31_ListElem<infotype>(*h);
      h=h->Next();
    }
  while(h!=0)
    { 
      hnew->Next()=new ni31_ListElem<infotype>(*h);
      hnew=hnew->Next();
      h=h->Next(); 
    } 
  hnew->Next()=0;

  return *this;
}

template<class infotype> inline unsigned long ni31_List<infotype>::Length()
{
  ni31_ListElem<infotype> *h=first;
  unsigned long int i=0;
  while(h!=0)
    { 
      h=h->Next();
      i++;
    }  
  return i; 
}

template<class infotype> inline int ni31_List<infotype>::IsEmpty()
{
    return 0==first;
}

template<class infotype> inline int ni31_List<infotype>::IsNotEmpty()
{
    return 0!=first;
}



/*
template<class infotype> inline Vector<infotype>::Vector()
{
  size=0;
}

template<class infotype> inline Vector<infotype>::Vector(unsigned s)
{
  if((size=s)>0) 
    vector=new infotype[size];
}

template<class infotype> inline Vector<infotype>::~Vector()
{
  if(size>0)
    delete [] vector;
}

template<class infotype>
inline infotype & Vector<infotype>::operator [] (unsigned i)
{
  return vector[i];
}

template<class infotype>
inline Vector<infotype> & Vector<infotype>::operator = (Vector<infotype> &s)
{
  if(size>0)
    delete [] vector;
  if((size=s.size)>0)
    {
      vector=new infotype[size];
      memcpy((void *)vector,(void *)s.vector,size*sizeof(infotype));
    }
  return *this;
}

template<class infotype> inline void Vector<infotype>::SizeTo(unsigned s)
{
  if(size>0)
    delete [] vector;
  if((size=s)>0)
    vector=new infotype[size];
}

template<class infotype> inline void Vector<infotype>::ResizeTo(unsigned s)
{
  infotype *h=vector;
  unsigned oldsize=size;
  
  if((size=s)>0)
    {
      vector=new infotype[size];
      if(oldsize>0)
	{
	  memcpy((void *)vector,(void *)h,
		 (oldsize<s?oldsize:s)*sizeof(infotype));
	  delete [] h;
	}
    }
}

template<class infotype> inline unsigned Vector<infotype>::Size() const
{
  return size;
}
*/

#endif
