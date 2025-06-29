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

  struct input
  {
    char *Name;
    char *Value;
  };

  struct mod
  {
    char  *Name;
    void (*Method)(mod* DefMod);
    
    param *Params;
    u32    ParamC;

    input *Inputs;
    u32    InputC;

    mod   *Mods;
    u32    ModC;
  };


  bool  GetPropP(mod *DefMod, char *ParamName);
  char* GetPropI(mod *DefMod, char *InputName);

  void Parse(int ArgC, char *ArgV[], u32 ModC, mod ModV[],  u32 &FileC, char **&FileV);

}
