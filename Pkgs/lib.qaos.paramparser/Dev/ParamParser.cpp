/*
  This file is part of QLang.
 
  This  file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QLang. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef  else if

#include <iostream>
#include <vector>
#include <regex>


#include "Basis.hpp"

#include "ParamParser.hpp"



namespace jix::ParamParser
{  

  void Parse(int ArgC, char *ArgV[], i32 ModC, mod ModV[], i32 &FileC, char **&FileV)
  {
    mod *NCon  = ModV;
    i32  NConC = ModC;
    
    mod *NMod = Nil;



    // GetMode
    for (int i = 1; i < ArgC; ++i)
    {
      string Cac(ArgV[i]);
      
      if (Cac.at(0) != ':')
        continue;

      Cac.erase(0,1);

      
      for (int j = 0; j < NConC; j++)
        if (NCon[j].Name == Cac)
        {
          NMod = &NCon[j];

          NConC = NCon[j].ModC;
          NCon  = NCon[j].Mods;

          goto _l_NextMod;
        }

        
      throw runtime_error("Invalid mod: " +Cac);

      _l_NextMod: {}
    }

    if (NMod == Nil)
      throw runtime_error("Not selected any mode");

      

    // GetPars
    for (int i = 1; i < ArgC; i++)
    {
      string Cac(ArgV[i]);

      bool Def = true;

      if (Cac.at(0) != '-')
        continue;

      Cac.erase(0,1);

      if (Cac.at(0) == '-')
      {
        Def = false;
        Cac.erase(0,1);
      }



      for (int j = 0; j < NMod->ParamC; j++)
        if (NMod->Params[j].Name == Cac)
        {
          NMod->Params[j].Def = Def;

          //WriteLn('per: ' +Cac, ' = ', Def);

          goto _l_NextPar;
        }

      throw runtime_error("Unknown param: " +Cac);

      _l_NextPar: {}
    }



    FileC = 0;
    FileV = Nil;

    // File
    for (int i = 1; i < ArgC; ++i)
    {
      string Cac(ArgV[i]);

      if (Cac.at(0) == ':')
        continue;

      ef (Cac.at(0) == '-')
        continue;

      
      FileC += 1;
      FileV = (char**)realloc((point)FileV, FileC *sizeof(char*));

      FileV[FileC-1] = ArgV[i];
    }



    NMod->Method(); 
  }

}