/*



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





*/
#if !defined(XArray_H)
#define XArray_H

// ******************************************************************************
template <class T>class XArray  
{
protected:
	T*		m_Values;
	int		m_Size;
	int		m_AllocSize;
	int		m_GrowBy;
	bool	m_CopyWithMemMove;
public:
	XArray(bool copyWithMemMove=false);
	XArray(int size,int allocSize=0,int growBy=10,bool copyWithMemMove=false);
	virtual ~XArray();

	int		GetSize();
	int		GetAllocSize();
	void	SetSize(int size);
	int		Add(T& value);
	T&		GetAt(int idx);
	void	SetAt(int idx,T& value);
	void	SetAll(T& value);
	T		RemoveLast();
	
	void	Delete(int idx);
	int		Insert(int idx,T& value);

	void	SetGrowBy(int growBy);

protected:
	void	Grow();
	void	Init(int size,int allocSize,int growBy,bool copyWithMemMove);
};
// ******************************************************************************
template <class T> int XArray<T>::GetSize()			{ return m_Size;};
// ******************************************************************************
template <class T> int XArray<T>::GetAllocSize()	{ return m_AllocSize;};
// ******************************************************************************
template <class T> T& XArray<T>::GetAt(int idx)
{
	return m_Values[idx];	
}
// ******************************************************************************
template <class T> T XArray<T>::RemoveLast()
{
	//if(m_Size <=0) return -1;						// ???
	return m_Values[--m_Size];	
}
// ******************************************************************************
template <class T> XArray<T>::XArray(bool copyWithMemMove)
{
	Init(0,10,10,copyWithMemMove);
}
// ******************************************************************************
template <class T> XArray<T>::XArray(int size,int allocSize,int growBy,bool copyWithMemMove)
{
	Init(size,allocSize,growBy,copyWithMemMove);
}
// ******************************************************************************
template <class T> void XArray<T>::Init(int size,int allocSize,int growBy,bool copyWithMemMove)
{
	if(allocSize <= 0)
		allocSize	= size;
	if(allocSize == 0)
		allocSize	= 1;
	m_AllocSize		= allocSize;
	m_GrowBy		= growBy;
	m_CopyWithMemMove = copyWithMemMove;
	m_Values		= new T[m_AllocSize];
	m_Size			= size;
}
// ******************************************************************************
template <class T> XArray<T>::~XArray()
{
	delete [] m_Values;
}
// ******************************************************************************
template <class T> int	XArray<T>::Add(T& value)
{
	if(m_Size >= m_AllocSize)
		Grow();
	if(m_Size >= m_AllocSize)
		return -1;
	int idx = m_Size++;
	m_Values[idx] = value;
	return idx;
}
// ******************************************************************************
template <class T> void XArray<T>::SetAt(int idx,T& value)
{
	if(idx >= m_Size || idx < 0)
		return;
	m_Values[idx] = value;
}
// ******************************************************************************
template <class T> void XArray<T>::SetAll(T& value)
{
	for(int i=0;i<m_Size;i++)
		m_Values[i] = value;
}
// ******************************************************************************
template <class T> void XArray<T>::Delete(int idx)
{
	if(idx >= m_Size || idx < 0)
		return;
	if(idx == m_Size-1)
	{
		m_Size--;
		return;
	}
	if(m_CopyWithMemMove)
		memmove((void*)(m_Values+idx),(void*)(m_Values+idx+1),(m_Size-idx-1)*sizeof(T));
	else
		for(int i=idx;i<m_Size-1;i++)
			m_Values[i] = m_Values[i+1];
	m_Size--;
}
// ******************************************************************************
template <class T> int XArray<T>::Insert(int idx,T& value)
{
	if(idx > m_Size || idx < 0)
		return -1;
	if(idx == m_Size)
		return Add(value);
	if(m_Size >= m_AllocSize)
		Grow();
	if(m_Size >= m_AllocSize)
		return -2;

	if(m_CopyWithMemMove)
		memmove((void*)(m_Values+idx+1),(void*)(m_Values+idx),(m_Size-idx)*sizeof(T));
	else
		for(int i=m_Size-1;i>=idx;i--)
			m_Values[i+1] = m_Values[i];

	m_Values[idx] = value;
	m_Size++;
	return idx;
}
// ******************************************************************************
template <class T> void XArray<T>::SetGrowBy(int growBy)
{
	m_GrowBy = growBy;
}
// ******************************************************************************
template <class T> void XArray<T>::SetSize(int size)
{
	if(size <= m_AllocSize)
		m_Size = size;
	else
	{
		T* oldValues	= m_Values;
		m_AllocSize		= size;
		m_Values		= new T[m_AllocSize];
		if(m_CopyWithMemMove)
			memcpy(m_Values,oldValues,m_Size*sizeof(T));
		else
			for(int i=0;i<m_Size;i++)
				m_Values[i] = oldValues[i];

		delete [] oldValues;				
		m_Size			= size;
	}
}
// ******************************************************************************
template <class T> void XArray<T>::Grow()
{
	int size	= m_Size;
	SetSize(m_Size+m_GrowBy);
	m_Size		= size;
}
// ******************************************************************************

typedef XArray<int>		XArrayInt;
typedef XArray<char*>	XArrayCharP;

// ******************************************************************************

#endif // !XArray_H
