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



class cParser
{
protected:
  // MainRet
  cFile *__Ret;


protected:
  // Memory
  bool  __Memory_ObjectExits(cMod *DMod, const string &Name)
  {
    for (auto &X: DMod->Objs)
      if (X.first == Name)
        return true;

    return false;
  }

  void  __Memory_ObjectAdd(cMod *DMod, const string &Name, cObj *Obj)
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


  bool   __Memory_ObjectExits(cRec *DRec, const string &Name)
  {
    for (auto &X: DRec->Objs)
      if (X.first == Name)
        return true;

    return false;
  }

  void   __Memory_ObjectAdd(cRec *DRec, const string &Name, cObj *Obj)
  {
    DRec->Objs.push_back({Name, Obj});
  }

  cObj* __Memory_ObjectGet(cRec *DRec, const string &Name)
  {
    for (auto &X: DRec->Objs)
      if (X.first == Name)
        return X.second;

    throw runtime_error("Object not found");
  }



protected:
  // File
  ifstream *Fin;
  u64 Fin_Size = 0;

  inline void  __File_Open(const string &FPath)
  {
    Fin = new ifstream(FPath, ios::binary);

    Fin->seekg(0, ios::end);
    Fin_Size = Fin->tellg();
    Fin->seekg(0, ios::beg);
  };

  inline void __File_Close()
  {
    delete Fin;
    Fin_Size = 0;
  };


protected:
  // Token
  vector<char>   Strings   = {'"', '\''};
  vector<string> Comment   = {"#", "//"};
  vector<char>   Seperater = {/*for comment -->*/'#',   '{','}', ':',';',',', '(',')', '<','>', '[',']', '-','+','/','*','!'};
  vector<string> BigSyms   = {/*for comment -->*/"//",  "->", "<<", ">>", "==", "<=", ">=", "!=", "<>"};
  vector<char>   IgnSyms   = {' ', (char)10, (char)13, (char)9};


  u32 Step = 0;
  vector<string> Tokens;
  u32 TokenC = 0;

  string *Word;
  #define GetW (*Word)


  void __Tokenize()
  {
    string Tok;

    bool _Comment = false;
    bool _BigSym  = false;
    bool _String  = false;

    char _SymCac;
    char _SymStr;

    Tokens.clear();
    TokenC = 0;
    Step = 0;


    while ((Fin_Size > Fin->tellg()) && (TokenC <= 4096))
    {
      char Cac;
      Fin->read(&Cac, 1);


      // String
      if (_String)
      {
        if (Cac == _SymStr)
        {
          _String = false;

          Tok = _SymStr +Tok+ _SymStr;

          goto _l_Sep;
        }


        Tok += Cac;
        continue;
      }


      // Comment
      if (_Comment)
      {
        _Comment = (Cac != (char)10);
        continue;
      }


      // String
      for (auto &X: Strings)
        if (Cac == X)
        {
          _String = true;
          _SymStr = Cac;

          goto _l_Sep;
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

  void Next()
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

  inline bool IsEnd()
  {
    if (Step < TokenC)
      return false;

    __Tokenize();

    return (TokenC == 0);
  };


protected:
  // Word
  inline void __Is_OK(const string &Str)
  {
    if (GetW != Str)
      throw runtime_error("Word is not valid: " +GetW +", expected: " +Str);
  };

  inline void __Is_Number(const string &Str)
  {
    if (! isdigit(Str[0]))
      throw runtime_error("Word is not number: " +GetW);
  };

  inline void __Is_Symbol(const string &Str)
  {
    if (find(Seperater.begin(), Seperater.end(), Str[0]) == Seperater.end())
      throw runtime_error("Word is not symbol: " +GetW);
  };

  inline void __Is_Word(const string &Str)
  {
    if (isdigit(Str[0]))
      throw runtime_error("Word is not word: " +GetW);

    if (find(Seperater.begin(), Seperater.end(), Str[0]) != Seperater.end())
      throw runtime_error("Word is not word: " +GetW);

  };

  inline void __Is_String(const string &Str)
  {
    if (Str[0] != '"')
      throw runtime_error("word is not string");
  }


protected:
  // Interpreter
  void __Int_Route(sType Types, cMod *DMod, bool &UseSkipped)
  {

    if (Types.count(eType::ctFun) && GetW == "fun")
    {
      UseSkipped = true;

      auto X = __Int_Symb_Fun();

      // Find
      if (__Memory_ObjectExits(DMod, X.first))
        throw runtime_error("Object already exist: " +X.first);
      
      
      __Memory_ObjectAdd(DMod, X.first, X.second);
    }

    ef (Types.count(eType::ctVar) && GetW == "var")
    {
      UseSkipped = true;

      auto Xs = __Int_Symb_Var();
      
      for (auto &X: Xs)
      {
        // Find
        if (__Memory_ObjectExits(DMod, X.first))
          throw runtime_error("Object already exist: " +X.first);
        
        
        __Memory_ObjectAdd(DMod, X.first, X.second);
      }

    }

    ef (Types.count(eType::ctRec) && GetW == "rec")
    {
      UseSkipped = true;

      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      auto X = __Int_Type_Rec();

      // Find
      if (__Memory_ObjectExits(DMod, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DMod, Name, X);
    }

    ef (Types.count(eType::ctTyp) && GetW == "type")
    {
      UseSkipped = true;

      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      auto X = __Int_Type_Type();

      // Find
      if (__Memory_ObjectExits(DMod, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DMod, Name, X);
    }

    ef (Types.count(eType::ctMod) && GetW == "mod")
    {
      UseSkipped = true;

      __Int_Mod(DMod);
    }

    ef (Types.count(eType::ctUse) && GetW == "use")
    {
      if (UseSkipped)
        throw runtime_error("Use must be used before definitions");

      __Int_Use();
    }

    el
      throw runtime_error(format("Unknown identifier: {} {{Mod: {}}}",  GetW, "nil"));
  };

  void __Int_Route(sType Types, cMod *DMod)
  {

    if (Types.count(eType::ctFun) && GetW == "fun")
    {
      auto X = __Int_Symb_Fun();

      // Find
      if (__Memory_ObjectExits(DMod, X.first))
        throw runtime_error("Object already exist: " +X.first);
      
      
      __Memory_ObjectAdd(DMod, X.first, X.second);
    }

    ef (Types.count(eType::ctVar) && GetW == "var")
    {
      auto Xs = __Int_Symb_Var();
      
      for (auto &X: Xs)
      {
        // Find
        if (__Memory_ObjectExits(DMod, X.first))
          throw runtime_error("Object already exist: " +X.first);
        
        
        __Memory_ObjectAdd(DMod, X.first, X.second);
      }

    }

    ef (Types.count(eType::ctRec) && GetW == "rec")
    {
      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      auto X = __Int_Type_Rec();

      // Find
      if (__Memory_ObjectExits(DMod, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DMod, Name, X);
    }

    ef (Types.count(eType::ctTyp) && GetW == "type")
    {
      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      auto X = __Int_Type_Type();

      // Find
      if (__Memory_ObjectExits(DMod, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DMod, Name, X);
    }

    ef (Types.count(eType::ctMod) && GetW == "mod")
    {
      __Int_Mod(DMod);
    }

    el
      throw runtime_error(format("Unknown identifier: {} {{Mod: {}}}",  GetW, "nil"));
  };

  void __Int_Route(sType Types, cRec *DRec)
  {
    if (Types.count(eType::ctFun) && GetW == "fun")
    {
      auto X = __Int_Symb_Fun();

      // Find
      if (__Memory_ObjectExits(DRec, X.first))
        throw runtime_error("Object already exist: " +X.first);
      
      
      __Memory_ObjectAdd(DRec, X.first, X.second);
    }

    ef (Types.count(eType::ctVar) && GetW == "var")
    {
      auto Xs = __Int_Symb_Var();
      
      for (auto &X: Xs)
      {
        // Find
        if (__Memory_ObjectExits(DRec, X.first))
          throw runtime_error("Object already exist: " +X.first);
        
        
        __Memory_ObjectAdd(DRec, X.first, X.second);
      }

    }
    
    ef (Types.count(eType::ctRec) && GetW == "rec")
    {
      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      auto X = __Int_Type_Rec();

      // Find
      if (__Memory_ObjectExits(DRec, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DRec, Name, X);
    }

    ef (Types.count(eType::ctTyp) && GetW == "type")
    {
      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      auto X = __Int_Type_Type();

      // Find
      if (__Memory_ObjectExits(DRec, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DRec, Name, X);
    }

    el
      throw runtime_error(format("Unknown identifier: {} {{Mod: {}}}",  GetW, "nil"));
  };



  // SYS
  void __Int_Use()
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


    __Ret->Libs.push_back(Buf);
  };

  void __Int_Mod(cMod *DMod)
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


    cout << format("m {}", Name) << endl;

    // Content
    while (GetW != "}")
      __Int_Route({ctMod, ctFun, ctVar, ctRec, ctTyp}, NMod);

    Next();
  };



  // Types
  iType* __Int_Type()
  {
    if (GetW == "fun")
    {
      Next();
      return __Int_Type_Fun();
    }


    ef (GetW == "rec")
      return __Int_Type_Rec();


    ef (GetW == "c")
      return __Int_Type_C();


    el
    {
      __Is_Word(GetW);
      string Name = GetW;
      Next();

      cRaw *Ret = new cRaw();
      Ret->R_Type = Name;
      
      return Ret;
    }

    //throw runtime_error(format("Unknown identifier: {} {{Mod: {}}}",  GetW, "nil"));
  }

  iType* __Int_Type_Type()
  {
    Next();


    // =
    __Is_OK("=");
    Next();

    
    return __Int_Type();
  }

  cRec*    __Int_Type_Rec()
  {
    cRec* Ret = new cRec();
    Next();


    if (GetW == ":")
    {
      Next();

      // Ancestor
      __Is_Word(GetW);
      Ret->R_Anc = GetW;
      Next();
    }


    // {
    __Is_OK("{");
    Next();


    // Content
    while (GetW != "}")
      __Int_Route({ctVar, ctRec, ctTyp}, Ret);

    Next();


    cout << format("r {{R_Anc: {}}}", Ret->R_Anc) << endl;


    // Ret
    return Ret;
  };

  cFunT*   __Int_Type_Fun()
  {
    cFunT* Ret = new cFunT;


    // (
    __Is_OK("(");
    Next();


    // Params
    while (GetW != ")")
    {
      // Name
      __Is_Word(GetW);
      Ret->R_Par.push_back(GetW);
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
      Ret->A_Ret = __Int_Type();
    }



    cout << format("f") << endl;


    // Ret
    return Ret;
  }

  cType_C* __Int_Type_C()
  {
    cType_C *Ret = new cType_C();
    Next();


    // (
    __Is_OK("(");
    Next();


    // string
    __Is_String(GetW);
    Ret->R_CType = GetW;
    Next();

    // )
    __Is_OK(")");
    Next();


    // Ret
    return Ret;
  }



  // Symbols
  vector<pair<string, cVar*>> __Int_Symb_Var()
  {
    vector<pair<string, cVar*>> Ret;

    // Vars
    vector<string> Names;


    do
    {
      Next();

      // Name
      __Is_Word(GetW);
      Names.push_back(GetW);
      Next();
    }
    while (GetW == ",");
    


    // :
    __Is_OK(":");
    Next();


    // Type
    iType *SType = __Int_Type();
    


    // Scan
    for (auto &X: Names)
    {
      // Add
      cVar *Obj = new cVar();
      Obj->A_Typ = SType;


      Ret.push_back({X, Obj});

      // Log
      cout << format("v {}", X) << endl;
    }


    // Ret
    return Ret;
  };

  pair<string, cFun*> __Int_Symb_Fun()
  {
    pair<string, cFun*> Ret;

    // Vars
    Ret.second = new cFun();
    Next();


    // Name
    __Is_Word(GetW);
    Ret.first = GetW;
    Next();


    // Type
    Ret.second->A_Type = __Int_Type_Fun();


    // Code
    __Is_OK("{");
    Next();
    __Is_OK("}");
    Next();


  
    cout << format("f {}", Ret.first) << endl;


    // Ret
    return Ret;
  };



public:
  cFile* Parse(const string &FPath)
  {
    __Ret = new cFile();

    __File_Open(FPath);

    bool UseSkipped = false;

    while (! IsEnd())
      __Int_Route({ctUse, ctMod, ctFun, ctVar, ctRec, ctTyp}, __Ret, UseSkipped);

    __File_Close();


    return __Ret;
  }
};


cFile* Parse(const string &FPath)
{
  cParser Engine;

  return Engine.Parse(FPath);
}

