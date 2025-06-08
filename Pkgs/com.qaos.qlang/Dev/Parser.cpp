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
#include <fstream>
#include <functional>
#include <format>

#include "Basis.hpp"

#include "QTypes.hpp"

#include "Parser.hpp"


using namespace std;
using namespace jix;



// Memory
bool  __Memory_ObjectExits(cMod *DMod, const string &Name)
{
  for (auto &X: DMod->Objs)
    if (X.first == Name)
      return true;

  return false;
}

void  __Memory_ObjectAdd(cMod *DMod, const string &Name, cObj* Obj)
{
  DMod->Objs.push_back({Name, Obj});
}

cObj* __Memory_ObjectGet(cMod *DMod, const string &Name)
{
  for (auto &X: DMod->Objs)
    if (X.first == Name)
      return X.second;

  throw runtime_error("Object not found");
}




cFile* Parse(const string &FPath)
{
  // Prepare
  cFile *Ret = new cFile();


  // File
  ifstream *Fin;
  u64 Fin_Size = 0;

  auto __File_Open = [&]()
  {
    Fin = new ifstream(FPath, ios::binary);

    Fin->seekg(0, ios::end);
    Fin_Size = Fin->tellg();
    Fin->seekg(0, ios::beg);
  };

  auto __File_Close = [&]()
  {
    delete Fin;
    Fin_Size = 0;
  };



// Token
  vector<string> Comment({"#", "//"});
  vector<char>   Seperater({/*for comment*/'#',  '{','}', ':',';',',', '(',')', '<','>', '[',']', '-','+','/','*','!'});
  vector<string> BigSyms({/*for comment*/"//",  "->", "<<", ">>", "==", "<=", ">=", "!=", "<>"});
  vector<char>   IgnSyms({' ', (char)10, (char)13, (char)9});


  u32 Step = 0;
  vector<string> Tokens;
  u32 TokenC = 0;

  string *Word;
  #define GetW (*Word)


  auto __Tokenize = [&]()
  {
    string Tok;

    bool _Comment = false;
    bool _BigSym = false;
    char _SymCac;

    Tokens.clear();
    TokenC = 0;
    Step = 0;


    while ((Fin_Size > Fin->tellg()) && (TokenC <= 4096))
    {
      char Cac;
      Fin->read(&Cac, 1);


      // Comment
      if (_Comment)
      {
        _Comment = (Cac != (char)10);
        continue;
      }



      // BigSym
      if (_BigSym)
      {
        _BigSym = false;

        for (auto &X: BigSyms)
          if (string({_SymCac, Cac}) == X)
            goto _l_SymBig;


        goto _l_Sym;
      }


      // Ignore
      for (auto &X: IgnSyms)
        if (Cac == X)
          goto _l_Sep;


      // Symbol
      for (auto &X: Seperater)
        if (Cac == X)
        {
          if (! Tok.empty())
          {
            Tokens.push_back(Tok);
            TokenC += 1;

            Tok = "";
          }

          _BigSym = true;
          _SymCac = Cac;
          continue;
        }


      // Add
      Tok+= Cac;


      // Escape
      continue;

      _l_SymBig:
        if (! Tok.empty())
          Tok = "";

        for (auto &X: Comment)
          if (string({_SymCac, Cac}) == X)
          {
            Tok = "";
            _Comment = true;
            goto _l_NextChar;
          }

        Tokens.push_back(string({_SymCac, Cac}));
        TokenC+= 1;

        continue;


      _l_Sym:
        for (auto &X: Comment)
          if (Tok == X)
          {
            Tok = "";
            _Comment = true;
            goto _l_NextChar;
          }
      
        if (! Tok.empty())
        {
          Tokens.push_back(Tok);
          TokenC+= 1;

          Tok = ""; //Cac;
        }

        // Ignore
        for (auto &X: IgnSyms)
          if (Cac == X)
            goto _l_Sep;

        Tok += Cac;

        continue;


      _l_Sep:
        if (! Tok.empty())
        {
          Tokens.push_back(Tok);
          TokenC+= 1;

          Tok = "";
        }

        continue;


      _l_NextChar: {}
    }

    Word = &Tokens[0];
  };

  auto Next = [&]()
  {
    Step+= 1;

    if (Step > TokenC)
    {
      __Tokenize();

      if (TokenC == 0)
        throw runtime_error("File is end but content is continue");
    }

    Word = &Tokens[Step];

    //clog << "W: " << GetW << endl;
  };

  auto IsEnd = [&]() -> bool
  {
    if (Step < TokenC)
      return false;

    __Tokenize();

    return (TokenC == 0);
  };


// Word
  auto __Is_OK = [&](const string &Str)
  {
    if (GetW != Str)
      throw runtime_error("Word is not valid: " +GetW);
  };

  auto __Is_Number = [&](const string &Str)
  {
    if (! isdigit(Str[0]))
      throw runtime_error("Word is not number: " +GetW);
  };

  auto __Is_Symbol = [&](const string &Str)
  {
    if (find(Seperater.begin(), Seperater.end(), Str[0]) == Seperater.end())
      throw runtime_error("Word is not symbol: " +GetW);
  };

  auto __Is_Word  = [&](const string &Str)
  {
    if (isdigit(Str[0]))
      throw runtime_error("Word is not word: " +GetW);

    if (find(Seperater.begin(), Seperater.end(), Str[0]) != Seperater.end())
      throw runtime_error("Word is not word: " +GetW);

  };


// Interpreter
  function<void (cMod*)> __Int_var = Nil;
  function<void (cMod*)> __Int_rec = Nil;
  function<void (cMod*)> __Int_fun = Nil;
  function<void (cMod*)> __Int_mod = Nil;


  auto __Int_use = [&]()
  {
    Next();
    
    
    __Is_OK("<");
    Next();

    string Buf;
    while (GetW != ">")
    {
      Buf += GetW;
      Next();
    }

    __Is_OK(">");
    Next();


    Ret->Libs.push_back(Buf);
  };


  auto __Int_Route = [&](sType Types, cMod *DMod, bool *UseSkipped = Nil)
  {

    if (Types.count(eType::ctFun) && GetW == "fun")
    {
      if (UseSkipped != Nil)
        *UseSkipped = true;

      __Int_fun(DMod);
    }

    ef (Types.count(eType::ctVar) && GetW == "var")
    {
      if (UseSkipped != Nil)
        *UseSkipped = true;

      __Int_var(DMod);
    }

    ef (Types.count(eType::ctRec) && GetW == "rec")
    {
      if (UseSkipped != Nil)
        *UseSkipped = true;

      __Int_rec(DMod);
    }

    ef (Types.count(eType::ctMod) && GetW == "mod")
    {
      if (UseSkipped != Nil)
        *UseSkipped = true;

      __Int_mod(DMod);
    }

    ef (Types.count(eType::ctUse) && GetW == "use")
    {
      if (UseSkipped != Nil)
        if (*UseSkipped)
          throw runtime_error("Use must be used before definitions");

      __Int_use();
    }

    el
      throw runtime_error(format("Unknown identifier: {} {{Mod: {}}}",  GetW, "nil"));
  };



  __Int_var = [&](cMod *DMod)
  {
    vector<string> Names;
    string SType;

    cVar *Obj;

    Next();


    // Section
    _l_Sec:

      // Name
      __Is_Word(GetW);
      Names.push_back(GetW);
      Next();

      // ,
      if (GetW == ",")
      {
        Next();
        goto _l_Sec;
      }


    // :
    __Is_OK(":");
    Next();

    // Type
    __Is_Word(GetW);
    SType = GetW;
    Next();

    // ;
    __Is_OK(";");
    Next();


    // Scan
    for (auto &Cac: Names)
    {
      // Find
      if (__Memory_ObjectExits(DMod, Cac))
        throw runtime_error("Object alread exist");


      // Add
      Obj = new cVar();
      Obj->Parent = DMod;
      Obj->R_Typ = SType;

      __Memory_ObjectAdd(DMod, Cac, Obj);

      // Log
      cout << format("·v {} {{R_Typ: {}}}", Cac, Obj->R_Typ) << endl;
    }

  };

  __Int_rec = [&](cMod *DMod)
  {
    string Name;
    cRec *Obj = new cRec();
    Obj->Parent = DMod;
    Next();


    // Name
    __Is_Word(GetW);
    Name = GetW;
    Next();


    // Find
    if (__Memory_ObjectExits(DMod, Name))
      throw runtime_error("Object alread exist");


    if (GetW == ":")
    {
      Next();

      // Ancestor
      __Is_Word(GetW);
      Obj->R_Anc = GetW;
      Next();
    }


    // {
    __Is_OK("{");
    Next();


    // Content
    while (GetW != "}")
      __Int_Route({ctVar}, Obj);

    Next();


    // Add
    __Memory_ObjectAdd(DMod, Name, Obj);

    cout << format("·r {} {{R_Anc: {}}}", Name, Obj->R_Anc) << endl;
  };

  __Int_fun = [&](cMod *DMod)
  {
    string Name;
    string Buf;

    cFun *Obj = new cFun();
    Obj->Parent = DMod;
    Next();


    // Name
    __Is_Word(GetW);
    Name = GetW;
    Next();


    // (
    __Is_OK("(");
    Next();


    Obj->R_Par.clear();


    // Params
    while (GetW != ")")
    {
      // Name
      __Is_Word(GetW);
      Obj->R_Par.push_back(GetW);
      Next();

      // ;
      if (GetW == ";")
      {
        Next();
        continue;
      }
    }


    // )
    __Is_OK(")");
    Next();



    // Ret
    if (GetW == "->")
    {
      Next();

      // Type
      __Is_Word(GetW);
      Obj->R_Ret = {GetW};
      Next();
    }


    // Find
    if (__Memory_ObjectExits(DMod, Name))
      throw runtime_error("Object alread exist");

    __Memory_ObjectAdd(DMod, Name, Obj);


    // Code
    __Is_OK("{");
    Next();
    __Is_OK("}");
    Next();



    Buf = "";
    for (auto &Cac: Obj->R_Par)
      Buf+= Cac +" ";

    Buf = string(Buf.begin(), find_if_not(Buf.rbegin(), Buf.rend(), ::isspace).base());

  
    cout << format("·f {} {{R_Ret: {}, R_Par: {}}}", Name, Obj->R_Ret, Buf) << endl;
  };

  __Int_mod = [&](cMod *DMod)
  {
    string Name;
    cMod *NMod;

    Next();

    // Name
    __Is_Word(GetW);
    Name = GetW;
    Next();


    // {
    __Is_OK("{");
    Next();


    // Find
    if (__Memory_ObjectExits(DMod, Name))
    {
      NMod = (cMod*)__Memory_ObjectGet(DMod, Name);

      if (dynamic_cast<cMod*>(NMod) == Nil)
        throw runtime_error("Object already exist");
    }
    
    el
    {  // New Mod
      NMod = new cMod();
      __Memory_ObjectAdd(DMod, Name, NMod);
    }


    //NMod->Parent = DMod;


    cout << format("·m {}", Name) << endl;

    // Content
    while (GetW != "}")
      __Int_Route({ctMod, ctFun, ctVar, ctRec}, NMod);

    Next();
  };




  // Parse
  __File_Open();


  bool UseSkipped = false;

  while (! IsEnd())
    __Int_Route({ctUse, ctMod, ctFun, ctVar, ctRec}, Ret, &UseSkipped);

    
  __File_Close();

  
  return Ret;
}

