#ifndef MSSIM_LIST_HPP
#define MSSIM_LIST_HPP

template< typename T >
class MSSim_List 
{
public: 

  MSSim_List()
    : m_first( 0 )
    , m_last( 0 )
  {
  }

  void check() 
  {
    if( size() <= 1 )
    {
      if( ! ( m_first == m_last ) ) {
      }
//        DebugBreak(); 
    }
  }


  T* popFront()
  {
    T* cur = m_first; 
    if( m_first )
    {
      if( m_first->m_next )
      {
        m_first = m_first->m_next; 
      } else {
        m_first = 0; 
        m_last = 0; 
      }
      cur->m_next = 0; 
    }
    return cur; 
  }

  size_t size() 
  {
    size_t count = 0; 
    T* cur = m_first; 
    while( cur )
    {
      ++count; 
      cur = cur->m_next; 
    }
    return count; 
  }

  bool remove( T* t )
  {
    T* cur = m_first; 
    if( cur )
    {
      if( cur == t )
      {
        m_first = m_first->m_next; 
        if( ! m_first )
          m_last = 0; 
        return true; 
      } else {
        while( cur->m_next )
        {
          if( cur->m_next == t )
          {
            cur->m_next = t->m_next; 
            t->m_next = 0; 
            if( ! cur->m_next )
              m_last = cur; 
            return true; 
          }
          cur = cur->m_next; 
        }
      }
    }
    return false; 
  }

  void pushBack( T* t ) 
  {
    if( m_last )
    {
      m_last->m_next = t; 
      m_last = t; 
    } else {
      m_first = t; 
      m_last = t; 
    }
    t->m_next = 0; 
  }

  void pushFront( T* t )
  {
    if( m_first )
    {
      t->m_next = m_first; 
      m_first = t; 
    } else {
      t->m_next = 0; 
      m_first = t; 
      m_last = t; 
    }
  }

  bool contains( T* t )
  {
    T* cur = m_first; 
    while( cur )
    {
      if( cur == t )
        return true; 
      cur = cur->m_next; 
    }
    return false; 
  }

  void clear() 
  {
    m_first = 0; 
    m_last = 0; 
  }

  T* first() 
  {
    return m_first; 
  }

  T* last() 
  {
    return m_last; 
  }

private: 
  T* m_first; 
  T* m_last; 
};

#endif // #ifndef MSSIM_LIST_HPP
