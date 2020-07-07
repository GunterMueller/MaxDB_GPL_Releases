/*!
  @file           gsp00_1.h
  @author         JoergM
  @special area   Compile time size check macros
  @brief          Macros that enable to check size of classes against size
              of member variable structure.

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


#ifndef GSP00_1_H
#define GSP00_1_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined(__cplusplus)
/*
 * Usefull for C++ code only
 */
#if defined(SUN) || defined(SOLARIS)
#define CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00(_class_name,_wanted_size_in_bytes)
#define CHECK_MAPPABLE_CLASS_UPPER_SIZE_SP00(_class_name,_upper_limit_in_bytes)
#define CHECK_MAPPABLE_CLASS_SIZE_SP00(_class_name,_layout)
#else
#define CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00(_class_name,_wanted_size_in_bytes) \
    void ____DoCompileTimeGivenSizeCheck____() \
    { \
        ____CompileTimeGivenSizeCheck____<sizeof(_class_name)> x00000; \
        ____CompileTimeGivenSizeCheck____<_wanted_size_in_bytes> y00000; \
        x00000 = y00000; \
    }

#define CHECK_MAPPABLE_CLASS_SIZE_SP00(_class_name,_layout) \
    void ____DoCompileTimeSizeCheck____() \
    { \
        ____CompileTimeSizeCheck____<sizeof(_class_name)> x00000; \
        ____CompileTimeSizeCheck____<sizeof(_class_name::_layout)> y00000; \
        x00000 = y00000; \
    }

#define CHECK_MAPPABLE_CLASS_UPPER_SIZE_SP00(_class_name,_upper_limit_in_bytes) \
    void ____DoCompileTimeSizeCheck____() \
    { \
	    union __upper_limit_sized_class \
        {  \
		  char buffer1[_upper_limit_in_bytes]; \
          char buffer2[sizeof(_class_name)];   \
        }; \
        ____CompileTimeGivenSizeCheck____<sizeof(__upper_limit_sized_class)> x00000;   \
        ____CompileTimeGivenSizeCheck____<_upper_limit_in_bytes>             y00000;   \
        x00000 = y00000; \
    }
#endif
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

template<int size_TV> class ____CompileTimeGivenSizeCheck____
{
    char buffer[size_TV];
public:
    void operator = (const ____CompileTimeGivenSizeCheck____<size_TV> & source) {}
};

template<int size_TV> class ____CompileTimeSizeCheck____
{
    char buffer[size_TV];
public:
    void operator = (const ____CompileTimeSizeCheck____<size_TV> & source) {}
};

#endif /* __cplusplus */

/*!
   @brief          Parameters used by this macro
   @check_mappable_class_given_size_sp00 and follows the conventions- FIRST parameter given is the CLASS NAME
        - SECOND parameter given is the NOT AUTOMATICALLY CALCULATED SIZE IN BYTES


   Each class that needs a check against 'given size' can use the macro

 */


/*!
   @brief          Parameters used by this macro
   @check_mappable_class_size_sp00 and follows the conventions- FIRST parameter given is the CLASS NAME
        - SECOND parameter given is the NAME OF A STRUCTURE containing all member variables


   Each class that needs a check against 'size as a structure' can use the macro

 */


/*
  Notes: CHECK_MAPPABLE_CLASS_UPPER_SIZE_SP00

  description: Parameters used by this macro
   
   Each class that must has a size less than an upper limit can use this macro
   to check that its size does not exceed a given upper limit:

          - FIRST parameter given is the CLASS NAME
          - SECOND parameter is the NOT AUTOMATICALLY UPPER LIMIT IN BYTES
 */

/*!
   @brief          Error messages returned during compile time
   @use the horizontal scroll bar to get to the 'information'- 'class ____CompileTimeGivenSizeCheck____<size>' (or there is no acceptable conversion)
  - 'class ____CompileTimeSizeCheck____<size>' (or there is no acceptable conversion)


   Unfortunatly error message text is not class specific. It always begins with the preamble
   " error C2679: binary '=' : no operator defined which takes a right-hand operand of type ".

 */


/*!
   @brief          An example code for securing a mappable class
   @code           
    
    
      // Example for 'standard' extension to a class. This implies you know how big a class
      // will become.
      // Advantage is maximum security with minimum change on existing classes
      // Disadvantage is calculator needed to find out the correct size, if class contains
      // a lot of members ( 6 is a magic number ... )
    
      class mappable_class
      {
      private:
          char a_member[4];
          char another_member[2];
    
      // ----BEGIN OF EXTRA CODE NEEDED----
          CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00( mappable_class, 6 )
      // -----END OF EXTRA CODE NEEDED-----
    
      private:
      //    char    do_not_add_any_member_variables_in_mappable_classes;
    
      public:
      //    virtual void never_use_virtual_functions_in_mappable_classes() {}
      };  
    
    
      // Example for a check 'virtual' extension to a class.
      // This implies you put all parameters into a structure. This will be
      // usefull if exact size is no problem, but sizeof struct must match
      // sizeof class.
      // Advantage is automatic checking.
      // Disadvantage is that this is only usefull if not to much changes on
      // existing code is implied.
    
      class mappable_class
      {
      private:
      // Second parameter NAME OF A STRUCTURE here "layout"
          struct layout
          {
              char a_member[4];
              char another_member[2];
          } values;
    
      // <<<<BEGIN OF EXTRA CODE NEEDED>>>>
          CHECK_MAPPABLE_CLASS_SIZE_SP00( mappable_class, layout )
      // <<<<<END OF EXTRA CODE NEEDED>>>>>
    
      private:
      //    char    do_not_add_any_member_variables_outside_layout_in_mappable_classes;
    
      public
      //    virtual void never_use_virtual_functions_in_mappable_classes() {}
      };  
    
    
    class mappable_class
      {
      private:
          char a_member[4];
          char another_member[2];
    
      // ----BEGIN OF EXTRA CODE NEEDED----
          CHECK_MAPPABLE_CLASS_UPPER_SIZE_SP00( mappable_class, 10 )
      // -----END OF EXTRA CODE NEEDED-----
    
      private:
      //    char  do_not_add_any_member_variables_in_mappable_classes_that_take_more_than_4_bytes;
    
      public:
      //    virtual void never_use_virtual_functions_in_mappable_classes() {}
      }; 
    
    
    @endcode        
 */

#endif  /* GSP00_1_H */
