/*
  This file is part of QLang.
 
  This  file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QLang. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef  else if
#define el  else

#include <iostream>
#include <vector>
#include <string.h>


#include "Basis.hpp"

#include "ParamParser.hpp"



namespace jix::ParamParser
{

  void Parse(int ArgC, char *ArgV[], u32 ModC, mod ModV[], u32 &FileC, char **&FileV)
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

          NConC = NMod->ModC; // NCon[j].ModC;
          NCon  = NMod->Mods; // NCon[j].Mods;

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


      // Input
      if (auto Pos = Cac.find('='); Pos != string::npos)
      {
        string Buf1 = Cac.substr(0, Pos);
        string Buf2 = Cac.substr(Pos +1);

        for (int j = 0; j < NMod->InputC; j++)
          if (NMod->Inputs[j].Name == Buf1)
          {
            NMod->Inputs[j].Value = strdup(Buf2.c_str());

            goto _l_NextPar;
          }
      }
      el
      {
        for (int j = 0; j < NMod->ParamC; j++)
          if (NMod->Params[j].Name == Cac)
          {
            NMod->Params[j].Def = Def;

            goto _l_NextPar;
          }
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



    NMod->Method(NMod); 
  }


  bool  GetPropP(mod *DefMod, char *ParamName)
  {
    for (int i = 0; i < DefMod->ParamC; i++)
      if (DefMod->Params[i].Name == string(ParamName))
        return DefMod->Params[i].Def;

    throw runtime_error("Parameter not found");
  }

  char* GetPropI(mod *DefMod, char *InputName)
  {
    for (int i = 0; i < DefMod->InputC; i++)
      if (DefMod->Inputs[i].Name == string(InputName))
        return DefMod->Inputs[i].Value;

    throw runtime_error("Input not found");
  }

}