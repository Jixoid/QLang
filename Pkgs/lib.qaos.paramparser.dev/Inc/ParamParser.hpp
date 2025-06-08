/*
  This file is part of QLang.
 
  This  file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QLang. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include "Basis.hpp"


using namespace std;
using namespace jix;



namespace jix::ParamParser
{
  struct param
  {
    char *Name;
    bool  Def;
  };

  struct mod
  {
    char  *Name;
    void (*Method)();
    
    param *Params;
    i32    ParamC;

    mod   *Mods;
    i32    ModC;
  };


  void Parse(int ArgC, char *ArgV[], i32 ModC, mod ModV[],  i32 &FileC, char **&FileV);

}
