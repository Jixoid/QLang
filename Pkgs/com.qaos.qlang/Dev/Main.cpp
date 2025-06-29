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
#include "Write_C.hpp"
#include "Ver.hpp"

#include "ParamParser.hpp"

#include "Main.hpp"


using namespace std;
using namespace jix;
namespace fs = std::filesystem;



unordered_map<string, cFile*> UnitList;


u32    FileC;
char **FileV;


void _Compile(ParamParser::mod *DefMod);


u32 ModC = 1;
ParamParser::mod *Mods = (ParamParser::mod[])
{
  {
    .Name = "compile",
    .Method = &_Compile,

    .Params = (ParamParser::param[])
    {
      {
        .Name = "rtti",
        .Def  = true,
      },
    },
    .ParamC = 1,

    .Inputs = (ParamParser::input[])
    {
      {
        .Name = "syslib",
        .Value = Nil,
      }
    },
    .InputC = 1,

    .Mods = {},
    .ModC = 0,
  },
};



bool __rtti;

string __syslib;


#define RES "\033[0m"
#define GRY "\033[1;30m"
#define RED "\033[1;31m"
#define GRE "\033[1;32m"
#define YEL "\033[1;33m"
#define BLU "\033[1;34m"
#define MAG "\033[1;35m"



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
    cerr << RED"An exception encountered!" RES << endl;
    cerr << "Class: " << typeid(E).name() << endl;
    cerr << "Message: " << E.what() << endl;

    return 2;
  }
}


string Dump(cObj *NObj, string Name = "")
{
  if (NObj == Nil)
    return (BLU"void" RES);

  
  ef (auto C = dynamic_cast<sVar*>(NObj); C != Nil)
    return Dump(C->A_Typ);


  ef (auto C = dynamic_cast<tRec*>(NObj); C != Nil)
  {
    string Buf;


    if (C->R_Anc != "")
      Buf = ": "+ C->R_Anc +" ";


    Buf += GRY"{";

    for (auto &X: C->Objs)
      if (dynamic_cast<iType*>(X.second))
        Buf += MAG"typ " GRE+ X.first +GRY" = "+ Dump(X.second) +GRY"; ";
      el
        Buf += YEL"var " GRE+ X.first +GRY": "+ Dump(X.second) +GRY"; ";


    return Buf +"}" RES;
  }


  ef (auto C = dynamic_cast<tRaw*>(NObj); C != Nil)
  {
    if (C->A_Type != Nil)
      return BLU +C->R_Type +GRY"("+C->A_Type->A_Sym+")" RES;
    
    el
      return BLU +C->R_Type +GRY"(" RED"<unknown>" GRY")" RES;
  }


  ef (auto C = dynamic_cast<tC*>(NObj); C != Nil)
    return format(MAG"c" GRY"(" RES"{}" GRY", " RES"{}" GRY")" RES, C->R_CType, C->Size);


  ef (auto C = dynamic_cast<tFun*>(NObj); C != Nil)
    return format(YEL"fun " GRE"{}" GRY":" RES" {} -> {}", Name, Dump(C->A_Par), Dump(C->A_Ret));


  el
    return (RED"<unknown>" RES);
}


void Logger(iCode::block *NCon, string OP = "") { for (auto &X: NCon->Codes)
{
  cout << OP;

  if (auto C = dynamic_cast<iCode::var*>(X); C != Nil)
  {
    cout << format(
      YEL"var " GRE"{}" GRY":" RES" {}" RES,
      C->Name,
      Dump(C->Type)
    ) << endl;
  }

  if  (auto C = dynamic_cast<iCode::block*>(X); C != Nil)
  {
    Logger(C, OP);
  }

}}

void Logger(iCon *NCon, string OP = "", bool Static = true) { for (auto &X: NCon->Objs)
{
  cout << OP;


  // Symbols s
  if (auto C = dynamic_cast<sFun*>(X.second); C != Nil)
  {
    cout << Dump(C->A_Type, X.first);

    if (Static)
      cout << format(
        GRY"  ![Symbol: {}{}]" RES,

        C->p_NoMangle ? (C->A_Name):(C->A_Sym.empty() ? RED"<unknown>" GRY:C->A_Sym),

        string(C->p_Extern ? ", extern":"")+
        (C->p_Export ? ", export":"")+
        (C->p_CDecl ? ", cdecl":"")+
        (C->p_Inline ? ", inline":"")+
        (C->p_NoExcept ? ", noexcept":"")+
        (C->p_NoMangle ? ", nomangle":"")
      );

    cout << endl;


    // Code
    Logger(&C->Code, OP+"  ");

    cout << endl;
  }


  ef (auto C = dynamic_cast<sVar*>(X.second); C != Nil)
  {
    cout << format(
      YEL"var " GRE"{}" GRY":" RES" {}" RES,
      X.first,
      Dump(C->A_Typ)
    );

    if (Static)
      cout << format(
        "  ![Symbol: {}]" RES,
        C->A_Sym.empty() ? RED"<unknown>" GRY:C->A_Sym
      );
    
    cout << endl;
  }


  // Virtual
  ef (auto C = dynamic_cast<vMod*>(X.second); C != Nil)
  {
    cout << format(RED"mod" RES" {}", X.first);

    if (Static)
      cout << format("  ![Symbol: {}]", C->A_Sym);

    cout << endl;


    Logger(C, OP+"  ");
  }

  
  // Types v
  ef (auto C = dynamic_cast<tRec*>(X.second); C != Nil)
  {
    cout << format(
      MAG"rec" GRE" {}" GRY"  ![VSym: {}, Size: " RES"{}" GRY"{}]" RES,
      X.first,
      C->A_Sym.empty() ? RED"<unknown>" GRY:C->A_Sym,
      C->Size,
      (C->p_Packed ? ", packed":"")
    ) << endl;


    Logger(C, OP+"  ", false);

    cout << endl;
  }


  ef (auto C = dynamic_cast<tC*>(X.second); C != Nil)
  {
    cout << format(
      MAG"typ " GRE"{} " GRY"= {}" GRY"  ![VSymbol: {}]" RES,
      X.first,
      Dump(C),
      C->A_Sym.empty() ? RED"<unknown>" GRY:C->A_Sym
    ) << endl;
  }


  ef (auto C = dynamic_cast<tFun*>(X.second); C != Nil)
  {
    cout << format("t {} = {}  ![VSymbol: {}]", X.first, Dump(C), C->A_Sym) << endl;
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

    cout << endl << "----- Read: " << Cac << " -----" << endl;


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
    cout << endl << "----- Parse -----" << endl;
    MiniMem = Parse(Cac);


    // Libs
    for (auto &X: MiniMem->Libs)
      X = __syslib +"/"+ X +".ql";
  
      
    LibMem = Main(MiniMem->Libs);


    // Proc
    cout << endl << "----- Proc -----" << endl;
    Procer(MiniMem);


    // Write
    cout << endl << "----- Write -----" << endl;
    WriteTo(MiniMem, Cac+".c");


    // Log
    cout << endl << "----- Log -----" << endl;
    Logger(MiniMem);


    Mems.push_back(MiniMem);
  }


  // Combine Mems

  return Nil;
}





void _Compile(ParamParser::mod *DefMod)
{
  if (FileC == 0)
    throw runtime_error("File queue is empty");


  __rtti = ParamParser::GetPropP(DefMod, "rtti");
  
  __syslib = ParamParser::GetPropI(DefMod, "syslib");



  vector<string> Files;

  for (int i = 0; i < FileC; i++)
    Files.push_back(string(FileV[i]));


  Main(Files);
}

