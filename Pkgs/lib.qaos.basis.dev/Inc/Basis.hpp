/*
  This file is part of QLang.
 
  This  file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QLang. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>

#include <stdint.h>
#include <stdatomic.h>

using namespace std;



namespace jix
{  

  // Int
  using i8   = int8_t;
  using i16  = int16_t;
  using i32  = int32_t;
  using i64  = int64_t;



  // UInt
  using u8   = uint8_t;
  using u16  = uint16_t;
  using u32  = uint32_t;
  using u64  = uint64_t;



  // System
  using iPtr = intptr_t;
  using uPtr = uintptr_t;
  
  using handle = uPtr;
  using ohid   = uPtr;
  


  // Char
  using c8  = char;
  using c16 = char16_t;



  // Float
  using f32 = float;
  using f64 = double;

  

  // IDs
  using idU = uid_t;
  using idG = gid_t;
  using idP = pid_t;
  using idT = pthread_t;

  struct idA
  {
    idP Pid;
    idU Uid;
    idG Gid;
  };
  


  // Point
  using point = void*;
  #define Nil  nullptr

  struct data_
  {
    point Point;
    uPtr  Size;
  };

  

  // String
  struct jstring
  {
    void (*Dis)(jstring*);
    char* Str;
  };

}
