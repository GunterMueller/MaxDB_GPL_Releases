/*!---------------------------------------------------------------------
  @file           RTE_MySQLProxy.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Password Scrambling

  This contains a pseudo random generator in MySQL style and the MySql protocol specific
  Haskey creation and scrambling methods.
  
    What the kernel has to do
    
      Use RTE_MySQLScrypting::Instance().GenerateScramble()
      to initialize the RTE_MySQLScramble stored in acv for later usage in password check.
      The scramble text (fixed size) is also result for special GETCHALLENGE command

      Use RTE_MySQLScrypting::Instance().GenerateHash() to create the hash from uncrypted password

      Use RTE_MySQLCrypting::Instance().CheckScramble() to check received scrambled password hash
      which is transferred instead of encrypted password in  CONNECT command.
        
    What the proxy has to do: 

      Get the scramble text using SAPDBC GetChallenge interface, which calls the GETCHALLENGE command...
      Transfer the scrambled password hash as 'password' (including old version flag needed for correct 
      selection of comparison algorithm) in the data part of the connect statement.

    @code
    mySQL source code excerpts...

    from mysql_com.h:

            struct rand_struct
            {
                unsigned long m_Seed1,m_Seed2,m_maxValue;
                double max_value_dbl;
            };

            void randominit(struct rand_struct *,unsigned long m_Seed1, unsigned long m_Seed2);

            --->  RTE_MySQLPseudoRandomGenerator::RTE_MySQLPseudoRandomGenerator(m_Seed1, m_Seed2);

            double rnd(struct rand_struct *);

            ---> SAPDB_Real8 RTE_MySQLPseudoRandomGenerator::RandomNumber()

    from win_main() or main() in mysqld.cpp

        start_time=time((time_t*) 0);
        randominit(&sql_rand,(ulong) start_time,(ulong) start_time/2);

            ---> RTE_MySQLCrypting::RTE_MySQLCrypting() Singleton constructor

        // from create_new_thread() in mysqld.cpp

        for each new connection handling thread....

        for (int i=0; i < 8 ; i++) // Generate password teststring
        {
            initialScrambleText[i]= (char) (rnd(&sql_rand)*94+33);
        }
        initialScrambleText[8]=0;

           ---> RTE_MySQLCrypting::Instance().GenerateScramble(RTE_MySQLScramble &scramble);

        // from sql/password.c

        generate hash code from readable password

        void hash_password(ulong *result, const char *password)
        {
          register ulong nr=1345345333L, add=7, nr2=0x12345671L;
          ulong tmp;
          for (; *password ; password++)
          {
            if (*password == ' ' || *password == TAB)
              continue;         skipp space in password
            tmp= (ulong) (uchar) *password;
            nr^= (((nr & 63)+add)*tmp)+ (nr << 8);
            nr2+=(nr2 << 8) ^ nr;
            add+=tmp;
          }
          result[0]=nr & (((ulong) 1L << 31) -1L); --- Don't use sign bit (str2int)
          result[1]=nr2 & (((ulong) 1L << 31) -1L);
          return;
        }
        
            ---> RTE_MySQLCrypting::Instance().GenerateHash(SAPDBChar const *text, RTE_MySQLHash &hash);

        // from sql/password.c

        char *scramble(char *to,const char *message,const char *password,
                   my_bool old_ver)
        {
          struct rand_struct rand_st;
          ulong hash_pass[2],hash_message[2];
          if (password && password[0])
          {
            char *to_start=to;
            hash_password(hash_pass,password);
            hash_password(hash_message,message);
            if (old_ver)
              old_randominit(&rand_st,hash_pass[0] ^ hash_message[0]);
            else
              randominit(&rand_st,hash_pass[0] ^ hash_message[0],
                 hash_pass[1] ^ hash_message[1]);
            while (*message++)
              *to++= (char) (floor(rnd(&rand_st)*31)+64);
            if (!old_ver)
            {                       Make it harder to break
              char extra=(char) (floor(rnd(&rand_st)*31));
              while (to_start != to)
            *(to_start++)^=extra;
            }
          }
          *to=0;
          return to;
        }

              ---> RTE_MySQLCrypting::Instance().Scramble(SAPDBChar const *text, RTE_MySQLHash &hash);

      // from check_scramble() in password.c

         my_bool check_scramble(const char *scrambled, const char *message,
               ulong *hash_pass, my_bool old_ver)
        {
          struct rand_struct rand_st;
          ulong hash_message[2];
          char buff[16],*to,extra;
          const char *pos;

          hash_password(hash_message,message);
          if (old_ver)
            old_randominit(&rand_st,hash_pass[0] ^ hash_message[0]);
          else
            randominit(&rand_st,hash_pass[0] ^ hash_message[0],
                   hash_pass[1] ^ hash_message[1]);
          to=buff;
          for (pos=scrambled ; *pos ; pos++)
            *to++=(char) (floor(rnd(&rand_st)*31)+64);
          if (old_ver)
            extra=0;
          else
            extra=(char) (floor(rnd(&rand_st)*31));
          to=buff;
          while (*scrambled)
          {
            if (*scrambled++ != (char) (*to++ ^ extra))
              return 1;
          }
          return 0;
        }

        --->  SAPDB_Bool RTE_MySQLCrypting::CheckScramble( RTE_MySQLScramble const &scrambledPasswordHash,
                                                           RTE_MySQLScramble const &scramble,
                                                           RTE_MyHash const &passwordHash,
                                                           SAPDB_Bool old_version )

    // from check_user() in sql_parse.cpp

        thd->master_access=acl_getroot(thd, thd->host, thd->ip, thd->user,
                 passwd, thd->scramble,     <<<<---- == message
                 &thd->priv_user,
                 protocol_version == 9 ||
                 !(thd->client_capabilities &
                   CLIENT_LONG_PASSWORD),   <<<<---- == old_ver
                 &ur);

    // from acl_getroot in sql_acl.cpp

         ulong acl_getroot(THD *thd, const char *host, const char *ip, const char *user,
           const char *password,const char *message,char **priv_user,
           bool old_ver, USER_RESOURCES  *mqh)

         !check_scramble(
                 password,       <<<----- scrambled password
                 message,        <<<----- scramble array
                 acl_user->salt, <<<----- Stored 'hash result' ulong[2]
                 (my_bool) old_ver)))  <<<--- see above

    // from check_scramble() in password.c

         my_bool check_scramble(const char *scrambled, const char *message,
               ulong *hash_pass, my_bool old_ver)
        {
          struct rand_struct rand_st;
          ulong hash_message[2];
          char buff[16],*to,extra;
          const char *pos;

          hash_password(hash_message,message);
          if (old_ver)
            old_randominit(&rand_st,hash_pass[0] ^ hash_message[0]);
          else
            randominit(&rand_st,hash_pass[0] ^ hash_message[0],
                   hash_pass[1] ^ hash_message[1]);
          to=buff;
          for (pos=scrambled ; *pos ; pos++)
            *to++=(char) (floor(rnd(&rand_st)*31)+64);
          if (old_ver)
            extra=0;
          else
            extra=(char) (floor(rnd(&rand_st)*31));
          to=buff;
          while (*scrambled)
          {
            if (*scrambled++ != (char) (*to++ ^ extra))
              return 1;
          }
          return 0;
        }

        --->  SAPDB_Bool RTE_MySQLCrypting::CheckScramble( RTE_MySQLScramble const &scrambledPasswordHash,
                                                           RTE_MySQLScramble const &scramble,
                                                           RTE_MyHash const &passwordHash,
                                                           SAPDB_Bool old_version )
    @endcode

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
#ifndef RTE_MYSQLPROXY_HPP
#define RTE_MYSQLPROXY_HPP
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <math.h>
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"

/*! Internal layout for hashed password */
typedef SAPDB_UInt4 RTE_MySQLHash[2];
/*! Internal layout for scramble output: zero terminated 8 byte ascii readable random text */
typedef SAPDB_Char  RTE_MySQLScramble[8+1];

/*!
  @class RTE_MySQLPseudoRandomGenerator
  mySQL random generator implementation
 */
class RTE_MySQLPseudoRandomGenerator {
public:
    /*!
      @brief ctor without any initialization
      Only used to allow 'delayed' initialization by 'old_version' selection...
     */
    RTE_MySQLPseudoRandomGenerator()
        : m_maxValue(1)
        , m_Seed1(0)
        , m_Seed2(0)
    {}

    /*!
      @brief ctor
      Used if mySQL protocol_version != 9 and CLIENT_LONG_PASSWORD set
      @param init_seed1 [in] initial value for first seed
      @param init_seed2 [in] initial value for second seed
     */
    RTE_MySQLPseudoRandomGenerator(SAPDB_UInt4 init_seed1,
                                   SAPDB_UInt4 init_seed2)
    {
        m_maxValue= 0x3FFFFFFFL;
        m_Seed1 = init_seed1%m_maxValue;
        m_Seed2 = init_seed2%m_maxValue;
    }

    /*!
      @brief old ctor

      Used if mySQL protocol_version == 9 or CLIENT_LONG_PASSWORD not set
      @param init_seed [in] initial value for seeding
     */
    RTE_MySQLPseudoRandomGenerator(SAPDB_UInt4 init_seed)
    {
        m_maxValue= 0x01FFFFFFL;
        m_Seed1 = init_seed%m_maxValue;
        m_Seed2 = m_Seed1/2;
    }

    /*!
      @brief assignment operator
      For delayed initialization
     */
    RTE_MySQLPseudoRandomGenerator & operator=(const RTE_MySQLPseudoRandomGenerator& toCopy)
    {
        if ( this != &toCopy )
        {
            m_Seed1     = toCopy.m_Seed1;
            m_Seed2     = toCopy.m_Seed2;
            m_maxValue = toCopy.m_maxValue;
        }
        return *this;
    }

    /*!
      @brief create defined random number
      @return random number in range [0 - 1.[
     */
    SAPDB_Real8 RandomNumber()
    {
        m_Seed1 = (m_Seed1*3+m_Seed2) % m_maxValue;
        m_Seed2 = (m_Seed1+m_Seed2+33) % m_maxValue;
        return (((SAPDB_Real8)m_Seed1)/(SAPDB_Real8)m_maxValue);
    }

private:
    SAPDB_UInt4 m_Seed1;          ///< first seed value
    SAPDB_UInt4 m_Seed2;          ///< second seed value
    SAPDB_UInt4 m_maxValue;       ///< maximum value (version dependend)
}; /*!  @endclass RTE_MySQLPseudoRandomGenerator */

/*!
  @class RTE_MySQLCrypting
  Soxy crypting functions
 */
class RTE_MySQLCrypting : public SAPDB_Singleton
{
public:
    /*!
      @brief Returns the singleton instance
      @return Reference to the one and only 
     */
    static RTE_MySQLCrypting & Instance();

    /*!
      @brief Genererate the initial connection packet scramble text

      Scrambled text consists of zero terminated fixed length string of
      random ascii characters in range [33 - 127]

      @param scramble [out] the generated scramble text
     */
    void GenerateScramble(RTE_MySQLScramble &scramble)
    {
           SAPDB_Int iChar;
           for ( iChar = 0; iChar < sizeof(RTE_MySQLScramble); iChar++ )
           {
               scramble[iChar]= (SAPDB_Char)( m_Generator.RandomNumber() * 94+33 );
           }
           scramble[iChar] = 0;
    }
      
    /*!
      @brief Hash a zero terminated password or any other zero terminated text string

      Derived from hash_password() found in sql/password.c

      @param password [in] the ascii zero terminated password
      @param result [out] the hashed password
     */
    void GenerateHash(SAPDB_Char const *password, RTE_MySQLHash &result)
    {
        SAPDB_UInt4 firstNumber  = 1345345333L;
        SAPDB_UInt4 add = 7;
        SAPDB_UInt4 secondNumber = 0x12345671L;
        SAPDB_UInt4 tmp;
        for (; *password ; password++)
        {
            if (*password == ' ' || *password == '\t')
                continue; /* skip space and tab in password */
            tmp = (SAPDB_UInt4) (SAPDB_Byte) *password;
            firstNumber  ^= (((firstNumber & 63) + add ) * tmp ) + (firstNumber << 8);
            secondNumber += (secondNumber << 8) ^ firstNumber;
            add          += tmp;
        }
        result[0] = firstNumber &  0x7FFFFFFFL; /* Don't use sign bit (str2int) */;
        result[1] = secondNumber & 0x7FFFFFFFL;
    }

    /*!
      @brief Check received scrambled password hash against hashed password using scrable text
      
      Derived from check_scramble() in sql/password.c

      @param receivedScrambledPasswordHash [out] scrambled password hash to check
      @param scramble [in] the connection scramble text
      @param passwordHash [in] the hashed password information
      @param old_version [in] select which version to use
      @return true if password is wrong, false if password passed the check
     */
    SAPDB_Bool CheckScramble( RTE_MySQLScramble const &receivedScrambledPasswordHash,
                              SAPDB_Bool               old_version,
                              RTE_MySQLScramble const &scramble,
                              RTE_MySQLHash     const &passwordHash )
    {
        RTE_MySQLPseudoRandomGenerator generator;
        RTE_MySQLHash scrambleHash;

        if ( 0 == receivedScrambledPasswordHash[0] )
        { return true; } /* forbidden empty scrambled password hash received */

        if ( 0 != receivedScrambledPasswordHash[sizeof(RTE_MySQLScramble)-1] )
        { return true; } /* always zero terminated scramble text... */

        GenerateHash(&scramble[0], scrambleHash);

        if ( old_version )
        {
            generator = RTE_MySQLPseudoRandomGenerator ( passwordHash[0]^scrambleHash[0] );
        }
        else
        {
            generator = RTE_MySQLPseudoRandomGenerator ( passwordHash[0]^scrambleHash[0],
                                                         passwordHash[1]^scrambleHash[1] );
        }

        RTE_MySQLScramble checkBuffer;
        SAPDB_Char       *pCheck;
        SAPDB_Char const *pScrambled;

        for ( pCheck = &checkBuffer[0], pScrambled = &receivedScrambledPasswordHash[0];
              *pScrambled;
              pScrambled++)
        {
            *pCheck++ = (SAPDB_Char) (floor(generator.RandomNumber()*31)+64);
        }

        if (old_version)
        {
            for ( pCheck = &checkBuffer[0], pScrambled = &receivedScrambledPasswordHash[0];
                  *pScrambled ;
                  pScrambled++)
            {
                if ( *pScrambled != *pCheck++ )
                {
                    return true;
                }
            }
        }
        else
        {
            SAPDB_Char extraScrambler = (SAPDB_Char) (floor(generator.RandomNumber()*31));

            for ( pCheck = &checkBuffer[0], pScrambled = &receivedScrambledPasswordHash[0];
                  *pScrambled ;
                  pScrambled++)
            {
                if ( *pScrambled != (SAPDB_Char) (*pCheck++ ^ extraScrambler) )
                {
                    return true;
                }
            }
        }

        return false;
    }

    /*!
      @brief Scramble a given password (only needed in MySQL Client code...)
      
      If no password is given or an empty string, the scrambled password hash is a single zero byte only...
      This code is not really needed in proxy or kernel, but exp0lains what the client does...

      Derived from  scramble() in sql/password.c

      @param scrambledPasswordHash [out] output buffer for scrambled text
      @param scramble [in] scramble text
      @param password [in] password used to scramble
      @param old_version [in] select which version to use
      @return pointer to trailing zero byte at and of scrambledPasswordHash
     */
    SAPDB_Char *Scramble( RTE_MySQLScramble       &scrambledPasswordHash,
                          RTE_MySQLScramble const &scramble,
                          SAPDB_Char        const *password,
                          SAPDB_Bool               old_version)
    {
       SAPDB_Char *pScrambled = &scrambledPasswordHash[0];

        if ( 0 == scramble[sizeof(RTE_MySQLScramble)-1]
            && password && password[0] )
        {
            RTE_MySQLPseudoRandomGenerator generator;
            RTE_MySQLHash hashedScramble;
            RTE_MySQLHash hashedPassword;

            GenerateHash(&scramble[0], hashedScramble);
            GenerateHash(password, hashedPassword);

            if ( old_version )
            {
                generator = RTE_MySQLPseudoRandomGenerator ( hashedPassword[0]^hashedScramble[0] );
            }
            else
            {
                generator = RTE_MySQLPseudoRandomGenerator ( hashedPassword[0]^hashedScramble[0],
                                                             hashedPassword[1]^hashedScramble[1] );
            }

            for ( int iScramble = sizeof(RTE_MySQLScramble)-1; iScramble > 0; --iScramble )
            {
                *pScrambled++ = (SAPDB_Char)(floor(generator.RandomNumber() * 31) + 64);
            }

            if (!old_version)
            {
                SAPDB_Char *pExtraScrambled = &scrambledPasswordHash[0];
                SAPDB_Char extraScrambler =(SAPDB_Char) (floor(generator.RandomNumber()*31));

                while ( pExtraScrambled != pScrambled )
                {
                    *(pExtraScrambled++) ^= extraScrambler;
                }
            }
        }

        *pScrambled = 0;

        return pScrambled;
    }


private:
    /*! the singleton instance */
    static RTE_MySQLCrypting *m_Instance;

    /*! The generator used to create initial scramble text inside requestor thread */
    RTE_MySQLPseudoRandomGenerator m_Generator;

    /*!
    @brief the singleton constructor

    Initializes the singletion own random generator that is used to feed all other random generators
    seeds and the scramble texts.
     */
    RTE_MySQLCrypting();

}; /*!  @endclass RTE_MySQLCrypting */
#endif /* RTE_MYSQLPROXY_HPP */
