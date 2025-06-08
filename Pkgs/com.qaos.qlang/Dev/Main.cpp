/*
  This file is part of QLang.
 
  This  file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QLang. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <format>

#include "Basis.hpp"

#include "QTypes.hpp"
#include "Parser.hpp"
#include "Proc.hpp"
#include "Ver.hpp"

#include "ParamParser.hpp"

#include "Main.hpp"


using namespace std;
using namespace jix;
namespace fs = std::filesystem;



unordered_map<string, cFile*> UnitList;


int    FileC;
char **FileV;


void _Compile();


int ModC = 1;
ParamParser::mod *Mods = (ParamParser::mod[])
{
  {
    .Name = "compile",
    .Method = &_Compile,

    .Params = (ParamParser::param[])
    {
      {
        .Name = "test",
        .Def  = true,
      },
    },
    .ParamC = 1,

    .Mods = {},
    .ModC = 0,
  },
};



inline void Hello()
{
  cout << "QLang by jixoid" << endl;

  cout << "Ver: "<<Version[0]<<"."<<Version[1]<<"."<<Version[2]<<":"<<Version[3] << endl;
  cout << "Licence: LGPL v3" << endl;
}


int Main(int ArgC, char *ArgV[])
{
  Hello();


  ParamParser::Parse(ArgC, ArgV, ModC, Mods, FileC, FileV);

  return 0;
}


int main(int ArgC, char *ArgV[])
{
  try
  {
    return Main(ArgC, ArgV);
  }
  catch (runtime_error E)
  {
    cout << "An exception encountered!" << endl;
    cout << "Class: " << typeid(E).name() << endl;
    cout << "Message: " << E.what() << endl;

    return 2;
  }
}



string Logger(iType *NTyp)
{
  if (NTyp == Nil)
    return "void";

  ef (auto C = dynamic_cast<cRaw*>(NTyp); C != Nil)
    return C->R_Type;

  ef (auto C = dynamic_cast<cType_C*>(NTyp); C != Nil)
    return format("c({})", C->R_CType);

  ef (auto C = dynamic_cast<cFunT*>(NTyp); C != Nil)
  {
    string Buf;

    for (auto &X: C->R_Par)
      Buf += X +", ";

    return format("fun({}) -> {}", Buf, Logger(C->A_Ret));
  }

  el
    return "<unknown>";
}


void Logger(cRec *NRec, string OP = "") { for (auto &X: NRec->Objs)
{
  cout << OP;

  if (auto C = dynamic_cast<cVar*>(X.second); C != Nil)
    cout << format("v {}: {}", X.first, Logger(C->A_Typ)) << endl;


  ef (auto C = dynamic_cast<cRec*>(X.second); C != Nil)
  {
    cout << format("r {}", X.first) << endl;
    
    Logger(C, OP +"  ");
  }

  ef (auto C = dynamic_cast<cType_C*>(X.second); C != Nil)
  {
    cout << format("t {} = {}", X.first, Logger(C)) << endl;
  }

  ef (auto C = dynamic_cast<cFunT*>(X.second); C != Nil)
  {
    cout << format("t {} = {}", X.first, Logger(C)) << endl;
  }

}}

void Logger(cMod *NMod, string OP = "", bool Virt = false) { for (auto &X: NMod->Objs)
{
  cout << OP;


  // Canonicalize
  if (auto C = dynamic_cast<cFun*>(X.second); C != Nil)
    cout << format("f {}: {} {{Symbol: {}, Static: {}}}", X.first, Logger(C->A_Type), Virt ? "Nil":C->A_Sym, C->A_Static) << endl;


  ef (auto C = dynamic_cast<cVar*>(X.second); C != Nil)
    cout << format("v {}: {} {{Symbol: {}, Static: {}}}", X.first, Logger(C->A_Typ), Virt ? "Nil":C->A_Sym, C->A_Static) << endl;


  ef (auto C = dynamic_cast<cMod*>(X.second); C != Nil)
  {
    cout << format("m {}", X.first) << endl;

    Logger(C, OP+"  ");
  }

  
  ef (auto C = dynamic_cast<cRec*>(X.second); C != Nil)
  {
    cout << format("r {}", X.first) << endl;

    Logger(C, OP+"  ");
  }

  ef (auto C = dynamic_cast<cType_C*>(X.second); C != Nil)
  {
    cout << format("t {} = {}", X.first, Logger(C)) << endl;
  }

  ef (auto C = dynamic_cast<cFunT*>(X.second); C != Nil)
  {
    cout << format("t {} = {}", X.first, Logger(C)) << endl;
  }

}}


cFile* Main(vector<string> Files)
{
  auto __Unit_Remembed = [&](const string &Name) -> bool
  {
    return (UnitList.find(Name) != UnitList.end());
  };

  auto __Unit_Get = [&](const string &Name) -> cFile*
  {
    auto X = UnitList.find(Name);

    if (X == UnitList.end())
      throw runtime_error("Unit \""+Name+"\" not remembed");

    return X->second;
  };


  cFile *MiniMem;
  cFile *LibMem;
  vector<cFile*> Mems;



  for (auto &X: Files)
  {
    string Cac = fs::canonical(X);

    cout << endl << "@SYS: File: " << Cac << endl;


    // Check file
    if (! fs::exists(Cac))
      throw runtime_error("File is not exitst: " +Cac);

    if (! fs::is_regular_file(Cac))
      throw runtime_error("File is not a regular file: " +Cac);


    // Remembed
    if (__Unit_Remembed(Cac))
    {
      cout << "File remembed, skip it" << endl;

      Mems.push_back(__Unit_Get(Cac));
      continue;
    }


    // Parser
    cout << endl << "@SYS: Parse" << endl;
    MiniMem = Parse(Cac);


    // Libs
    //LibMem = Main(MiniMem->Libs);


    // Proc
    cout << endl << "@SYS: Proc" << endl;
    Procer(MiniMem);


    // Log
    cout << endl << "@SYS: Log" << endl;
    Logger(MiniMem);


    Mems.push_back(MiniMem);
  }


  // Combine Mems

  return Nil;
}





void _Compile()
{
  if (FileC == 0)
    throw runtime_error("File queue is empty");


  vector<string> Files;

  for (int i = 0; i < FileC; i++)
    Files.push_back(string(FileV[i]));


  Main(Files);
}

