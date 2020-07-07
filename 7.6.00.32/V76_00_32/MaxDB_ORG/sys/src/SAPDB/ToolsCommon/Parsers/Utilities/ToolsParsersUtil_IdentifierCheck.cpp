/*!
    @file    ToolsParsersUtil_IdentifierCheck.cpp
    @author  BerndV
    @brief   Class to analyze parameter values

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <ctype.h>
#include <string.h>

#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp"

/*
  -----------------------------------------------------------------------------
  definitions
  -----------------------------------------------------------------------------
 */

// This array contains at the index of a uppercase letter the 
// corresponding lowercase letter. The array contains all letters
// for the "simple identifiers" (with the "extended language letters")
unsigned char ToolsParsersUtil_IdentifierCheck::oTolower[] = 
{
//   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79
     0, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
//  80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95 
   112,113,114,115,116,117,118,119,120,121,122,  0,  0,  0,  0,  0,
//  96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,154,  0,156,  0,158,  0,
// 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,
// 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207
   224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
// 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 
   240,241,242,243,244,245,246,  0,248,249,250,251,252,253,  0,  0,
// 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// This array contains at the index of a lowercase letter the 
// corresponding uppercase letter. The array contains all letters
// for the "simple identifiers" (with the "extended language letters")
unsigned char ToolsParsersUtil_IdentifierCheck::oToupper[] = 
{
//   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//  96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111
     0, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
// 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,  0,  0,  0,  0,  0,
// 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,138,  0,140,  0,142,  0,
// 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239
   192,193,194,195,196,197,198,199,200,201,202,203,204,204,206,207,
// 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 
   208,209,210,211,212,213,214,  0,216,217,218,219,220,221,  0,159
};

// This array contains at the index of a character true if the character
// will be valid for the first character of a "simple identifier"
bool ToolsParsersUtil_IdentifierCheck::oSimpleFirst[] = 
{
//   0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  16      17      18      19      20      21      22      23      24      25      26      27      28      29      30      31 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  32      33      34      35      36      37      38      39      40      41      42      43      44      45      46      47 
   false,  false,  false,  true ,  true ,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  48      49      50      51      52      53      54      55      56      57      58      59      60      61      62      63
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  64      65      66      67      68      69      70      71      72      73      74      75      76      77      78      79
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
//  80      81      82      83      84      85      86      87      88      89      90      91      92      93      94      95 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,
//  96      97      98      99     100     101     102     103     104     105     106     107     108     109     110     111
   false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 112     113     114     115     116     117     118     119     120     121     122     123     124     125     126     127 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,
// 128     129     130     131     132     133     134     135     136     137     138     139     140     141     142     143
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  true ,  false,  true ,  false,  true ,  false,
// 144     145     146     147     148     149     150     151     152     153     154     155     156     157     158     159 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  true ,  false,  true ,  false,  true ,  true ,
// 160     161     162     163     164     165     166     167     168     169     170     171     172     173     174     175 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 176     177     178     179     180     181     182     183     184     185     186     187     188     189     190     191
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 192     193     194     195     196     197     198     199     200     201     202     203     204     205     206     207
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 208     209     210     211     212     213     214     215     216     217     218     219     220     221     222     223 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 224     225     226     227     228     229     230     231     232     233     234     235     236     237     238     239
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 240     241     242     243     244     245     246     247     248     249     250     251     252     253     254     255 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true 
};

// This array contains at the index of a character true if the character
// will be valid for character of a "simple identifier" after the first character.
bool ToolsParsersUtil_IdentifierCheck::oSimpleTail[] = 
{
//   0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  16      17      18      19      20      21      22      23      24      25      26      27      28      29      30      31 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  32      33      34      35      36      37      38      39      40      41      42      43      44      45      46      47 
   false,  false,  false,  true ,  true ,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  48      49      50      51      52      53      54      55      56      57      58      59      60      61      62      63
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,  false,
//  64      65      66      67      68      69      70      71      72      73      74      75      76      77      78      79
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
//  80      81      82      83      84      85      86      87      88      89      90      91      92      93      94      95 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  true ,
//  96      97      98      99     100     101     102     103     104     105     106     107     108     109     110     111
   false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 112     113     114     115     116     117     118     119     120     121     122     123     124     125     126     127 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,
// 128     129     130     131     132     133     134     135     136     137     138     139     140     141     142     143
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  true ,  false,  true ,  false,  true ,  false,
// 144     145     146     147     148     149     150     151     152     153     154     155     156     157     158     159 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  true ,  false,  true ,  false,  true ,  true ,
// 160     161     162     163     164     165     166     167     168     169     170     171     172     173     174     175 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 176     177     178     179     180     181     182     183     184     185     186     187     188     189     190     191
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 192     193     194     195     196     197     198     199     200     201     202     203     204     205     206     207
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 208     209     210     211     212     213     214     215     216     217     218     219     220     221     222     223 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 224     225     226     227     228     229     230     231     232     233     234     235     236     237     238     239
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 240     241     242     243     244     245     246     247     248     249     250     251     252     253     254     255 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true 
};

// This array contains at the index of a character true if the character
// will be valid for the first character of a "very simple identifier"
bool ToolsParsersUtil_IdentifierCheck::oVerySimpleFirst[] = 
{
//   0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  16      17      18      19      20      21      22      23      24      25      26      27      28      29      30      31 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  32      33      34      35      36      37      38      39      40      41      42      43      44      45      46      47 
   false,  false,  false,  true ,  true ,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  48      49      50      51      52      53      54      55      56      57      58      59      60      61      62      63
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  64      65      66      67      68      69      70      71      72      73      74      75      76      77      78      79
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
//  80      81      82      83      84      85      86      87      88      89      90      91      92      93      94      95 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,
//  96      97      98      99     100     101     102     103     104     105     106     107     108     109     110     111
   false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 112     113     114     115     116     117     118     119     120     121     122     123     124     125     126     127 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,
// 128     129     130     131     132     133     134     135     136     137     138     139     140     141     142     143
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 144     145     146     147     148     149     150     151     152     153     154     155     156     157     158     159 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 160     161     162     163     164     165     166     167     168     169     170     171     172     173     174     175 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 176     177     178     179     180     181     182     183     184     185     186     187     188     189     190     191
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 192     193     194     195     196     197     198     199     200     201     202     203     204     205     206     207
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 208     209     210     211     212     213     214     215     216     217     218     219     220     221     222     223 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 224     225     226     227     228     229     230     231     232     233     234     235     236     237     238     239
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 240     241     242     243     244     245     246     247     248     249     250     251     252     253     254     255 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false
};

// This array contains at the index of a character true if the character
// will be valid for character of a "very simple identifier" after the first character.
bool ToolsParsersUtil_IdentifierCheck::oVerySimpleTail[] = 
{
//   0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  16      17      18      19      20      21      22      23      24      25      26      27      28      29      30      31 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  32      33      34      35      36      37      38      39      40      41      42      43      44      45      46      47 
   false,  false,  false,  true ,  true ,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
//  48      49      50      51      52      53      54      55      56      57      58      59      60      61      62      63
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,  false,
//  64      65      66      67      68      69      70      71      72      73      74      75      76      77      78      79
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
//  80      81      82      83      84      85      86      87      88      89      90      91      92      93      94      95 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  true ,
//  96      97      98      99     100     101     102     103     104     105     106     107     108     109     110     111
   false,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,
// 112     113     114     115     116     117     118     119     120     121     122     123     124     125     126     127 
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,
// 128     129     130     131     132     133     134     135     136     137     138     139     140     141     142     143
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 144     145     146     147     148     149     150     151     152     153     154     155     156     157     158     159 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 160     161     162     163     164     165     166     167     168     169     170     171     172     173     174     175 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 176     177     178     179     180     181     182     183     184     185     186     187     188     189     190     191
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 192     193     194     195     196     197     198     199     200     201     202     203     204     205     206     207
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 208     209     210     211     212     213     214     215     216     217     218     219     220     221     222     223 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 224     225     226     227     228     229     230     231     232     233     234     235     236     237     238     239
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,
// 240     241     242     243     244     245     246     247     248     249     250     251     252     253     254     255 
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false
};

/* @brief static member function */
bool ToolsParsersUtil_IdentifierCheck::IsSpecialIdentifier
  ( const char * szString )
{
  return (strlen(szString) > 2) && (szString[0] == '"') && (szString[strlen(szString)-1] == '"');
} // end ToolsParsersUtil_IdentifierCheck::IsSpecialIdentifier

/* @brief static member function */
bool ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier
  ( const unsigned char * szString )
{
  return IsIdentifier(szString, oVerySimpleFirst, oVerySimpleTail);
} // end ToolsParsersUtil_IdentifierCheck::IsVerySimpleIdentifier

/* @brief static member function */
bool ToolsParsersUtil_IdentifierCheck::IsSimpleIdentifier
  ( const unsigned char * szString )
{
  return IsIdentifier(szString, oSimpleFirst, oSimpleTail);
} // end ToolsParsersUtil_IdentifierCheck::IsSimpleIdentifier

/* @brief private static member function */
bool ToolsParsersUtil_IdentifierCheck::IsIdentifier
  ( const unsigned char * szString,
    const bool            oFirst[],
    const bool            oTail[] )
{
  // check for empty string
  if (*szString == 0) return false;
  // check first character
  if (!oFirst[*szString]) return false;
  // iterate through string
  while ( (*(++szString) != 0) && oTail[*szString]);
  // true if all characters passed
  if (*szString == 0) return true;
  // otherwise false
  return false;
} // end ToolsParsersUtil_IdentifierCheck::IsIdentifier

/* @brief static member function */
void ToolsParsersUtil_IdentifierCheck::KernelToupper
  ( unsigned char       * szString )
{
  while (*szString != 0) {
    *szString= (oToupper[*szString] != 0) ? oToupper[*szString] : *szString;
    ++szString;
  } // end while
} // end ToolsParsersUtil_IdentifierCheck::KernelToupper

/* @brief static member function */
void ToolsParsersUtil_IdentifierCheck::KernelTolower
  ( unsigned char       * szString )
{
  while (*szString != 0) {
    *szString= (oTolower[*szString] != 0) ? oTolower[*szString] : *szString;
    ++szString;
  } // end while
} // end ToolsParsersUtil_IdentifierCheck::KernelTolower

/* @brief static member function */
void ToolsParsersUtil_IdentifierCheck::CRuntimeToupper
  ( unsigned char * szString )
{
  while (*szString != 0) {
    *szString = toupper(*szString);
    ++szString;
  } // end while
} // end ToolsParsersUtil_IdentifierCheck::CRuntimeToupper

/* @brief static member function */
void ToolsParsersUtil_IdentifierCheck::CRuntimeTolower
  ( unsigned char * szString )
{
  while (*szString != 0) {
    *szString = tolower(*szString);
    ++szString;
  } // end while
} // end ToolsParsersUtil_IdentifierCheck::CRuntimeTolower
