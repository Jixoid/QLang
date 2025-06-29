#define ef else if
#define el else

#include <iostream>
#include <format>
#include <fstream>

#include "Basis.hpp"

#include "QTypes.hpp"

#include "Write_C.hpp"


using namespace std;
using namespace jix;




string DumpC(cObj *NObj, string Name = "")
{
  if (NObj == Nil)
    return "void";

  
  ef (auto C = dynamic_cast<sVar*>(NObj); C != Nil)
  {
    return "var " +DumpC(C->A_Typ);
  }

  ef (auto C = dynamic_cast<tRec*>(NObj); C != Nil)
  {
    string Buf;


    if (C->R_Anc != "")
      Buf = ": "+ C->R_Anc +" ";


    Buf += "struct {";

    for (auto &X: C->Objs)
      if (dynamic_cast<iType*>(X.second))
        Buf += "type "+ X.first +" = "+ DumpC(X.second) +"; ";
      el
        Buf += format("{} {};", DumpC(X.second), X.first);


    return Buf +"}";
  }

  ef (auto C = dynamic_cast<tRaw*>(NObj); C != Nil)
  {
    return format("{}", C->A_Type->A_Sym);
  }

  ef (auto C = dynamic_cast<tC*>(NObj); C != Nil)
  {
    return format("c({})", C->R_CType);
  }

  ef (auto C = dynamic_cast<tFun*>(NObj); C != Nil)
  {
    return format("{} (*{})({})", DumpC(C->A_Ret), Name,  DumpC(C->A_Par));
  }

  el
    return "<unknown>";
}


void Writer(ofstream &Fout, iCon *NCon, string OP = "", bool Static = true);


void Writer(ofstream &Fout, const string &Name, cObj *NObj, string OP, bool Static)
{
  if (NObj->D_Writed)
    return;
  NObj->D_Writed = true;


  // Symbols
  if (auto C = dynamic_cast<sFun*>(NObj); C != Nil)
  {
    Fout << OP << format("// @ql fun, call: {}\n", C->p_CDecl ? "cdecl":"qdecl");


    // Local types
    for (auto &X: C->A_Type->A_Par->Objs)
      if (auto T = dynamic_cast<iType*>(X.second); T != Nil)
        Writer(Fout, X.first, X.second, OP +"  ", false);

    // Depency
    for (auto &X: C->A_Type->A_Par->Objs)
      if (auto T = dynamic_cast<sVar*>(X.second); T != Nil)
        Writer(Fout, T->A_Typ->A_Sym, T->A_Typ, OP +"  ", false);


    // Return type
    if (C->A_Type->A_Ret != Nil)
      Writer(Fout, C->A_Type->A_Ret->A_Sym, C->A_Type->A_Ret, OP +"  ", true);


    // Main
    Fout << OP;
    
    if (C->p_Extern)
      Fout << "extern ";

    if (C->p_Inline)
      Fout << "inline ";
      

    if (C->p_CDecl)
    {
      string Buf;
      for (auto &X: C->A_Type->A_Par->Objs)
        if (auto T = dynamic_cast<sVar*>(X.second); T != Nil)
          Buf += format("{} {}, ", DumpC(T->A_Typ), X.first);

      if (! Buf.empty())
        Buf = Buf.substr(0, Buf.size()-2);

      Fout << format("\n{} {}({})\n{{}}\n\n",
        (C->A_Type->A_Ret == Nil) ? "void":C->A_Type->A_Ret->A_Sym,
        C->p_NoMangle ? (C->A_Name):(Static ? C->A_Sym:Name),
        Buf
      );
    }
    el
    {
      string Buf;

      if (C->A_Type->A_Ret != Nil)
        Buf += C->A_Type->A_Ret->A_Sym +" *___ret, ";

      for (auto &X: C->A_Type->A_Par->Objs)
        if (auto T = dynamic_cast<sVar*>(X.second); T != Nil)
          Buf += format("{} {}, ", DumpC(T->A_Typ), X.first);

      if (! Buf.empty())
        Buf = Buf.substr(0, Buf.size()-2);

      Fout << format("void {}({})\n{{}}\n\n",-
        C->p_NoMangle ? (C->A_Name):(Static ? C->A_Sym:Name),
        Buf
      );
    }


  }

  ef (auto C = dynamic_cast<sVar*>(NObj); C != Nil)
  {
    Fout << OP << format("{} {}; // @ql var\n", DumpC(C->A_Typ), Static ? C->A_Sym:Name);
  }


  // Virtual
  ef (auto C = dynamic_cast<vMod*>(NObj); C != Nil)
  {
    Writer(Fout, C, OP);
  }


  // Types v
  ef (auto C = dynamic_cast<tRec*>(NObj); C != Nil)
  {
    // Local types
    for (auto &X: C->Objs)
      if (auto T = dynamic_cast<iType*>(X.second); T != Nil)
        Writer(Fout, X.first, X.second, OP +"  ", false);

    // Depency
    for (auto &X: C->Objs)
      if (auto T = dynamic_cast<sVar*>(X.second); T != Nil)
        Writer(Fout, T->A_Typ->A_Sym, T->A_Typ, OP +"  ", false);



    Fout << OP << format("typedef struct {{\n");


    for (auto &X: C->Objs)
      if (auto T = dynamic_cast<iType*>(X.second); T == Nil)
        Writer(Fout, X.first, X.second, OP +"  ", false);



    Fout << OP << format("}} {}; // @ql rec\n\n", C->A_Sym);
  }

  ef (auto C = dynamic_cast<tFun*>(NObj); C != Nil)
  {
    // Return type
    if (C->A_Ret != Nil)
      Writer(Fout, C->A_Ret->A_Sym, C->A_Ret, OP +"  ", true);

    //Fout << OP << format("typedef {}; // @ql fun_t\n", DumpC(C, C->A_Sym));


    if (C->p_CDecl)
    {
      string Buf;
      for (auto &X: C->A_Par->Objs)
        if (auto T = dynamic_cast<sVar*>(X.second); T != Nil)
          Buf += format("{} {}, ", DumpC(T->A_Typ), X.first);

      if (! Buf.empty())
        Buf = Buf.substr(0, Buf.size()-2);

      Fout << OP << format("typedef {} (*{})({}); // @ql fun_t\n",
        (C->A_Ret == Nil) ? "void":C->A_Ret->A_Sym,
        C->A_Sym,
        Buf
      );
    }
    el
    {
      string Buf;

      if (C->A_Ret != Nil)
        Buf += C->A_Ret->A_Sym +" *___ret, ";

      for (auto &X: C->A_Par->Objs)
        if (auto T = dynamic_cast<sVar*>(X.second); T != Nil)
          Buf += format("{} {}, ", DumpC(T->A_Typ), X.first);

      if (! Buf.empty())
        Buf = Buf.substr(0, Buf.size()-2);

      Fout << OP << format("typedef void (*{})({}); // @ql fun_t\n",
        C->A_Sym,
        Buf
      );
    }

  }

  ef (auto C = dynamic_cast<tRaw*>(NObj); C != Nil)
  {
    Writer(Fout, C->A_Type->A_Sym, C->A_Type, OP, false);

    if (Name.empty())
      return;

    Fout << OP << format("typedef {} {}; // @ql raw\n", C->A_Type->A_Sym, Name);
  }

  ef (auto C = dynamic_cast<tC*>(NObj); C != Nil)
  {
    Fout << OP << format("typedef {} {}; // @ql c\n", C->R_CType.substr(1, C->R_CType.size() -2), C->A_Sym);
  }

}


void Writer(ofstream &Fout, iCon *NCon, string OP, bool Static)
{
  for (auto &X: NCon->Objs)
    if (auto T = dynamic_cast<iSymb*>(X.second); T != Nil)
      if (T->p_Export)
        Writer(Fout, X.first, X.second, OP, Static);
}




void WriteTo(cFile *Mem, string FPath)
{
  ofstream Fout(FPath);

  if (! Fout.is_open())
    throw runtime_error("File can not opened: " +FPath);


  Fout << "/* This file was created by qlang */\n\n\n";

  Writer(Fout, Mem);

  Fout.close();
}
