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
  bool  __Memory_ObjectExits(iCon *DCon, const string &Name)
  {
    for (auto &X: DCon->Objs)
      if (X.first == Name)
        return true;

    return false;
  }

  void  __Memory_ObjectAdd(iCon *DCon, const string &Name, cObj *Obj)
  {
    DCon->Objs.push_back({Name, Obj});
  }

  cObj* __Memory_ObjectGet(iCon *DCon, const string &Name)
  {
    for (auto &X: DCon->Objs)
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
  vector<char>   Seperater = {/*for comment -->*/'#',   '{','}', ':',';',',', '(',')', '<','>', '[',']', '-','+','/','*','!'};
  vector<string> BigSyms   = {/*for comment -->*/"//",  "->", "<<", ">>", "==", "<=", ">=", "!=", "<>"};
  vector<char>   IgnSyms   = {' ', (char)10, (char)13, (char)9};


  u32 Step = 0;
  vector<string> Tokens;
  u32 TokenC = 0;

  string *Word;
  #define GetW (*Word)


  inline bool isIgn(char C)
  {
    return find(IgnSyms.begin(), IgnSyms.end(), C) != IgnSyms.end();
  }

  inline bool isSeperator(char C)
  {
      return find(Seperater.begin(), Seperater.end(), C) != Seperater.end();
  }

  inline bool isStringChar(char C)
  {
    return find(Strings.begin(), Strings.end(), C) != Strings.end();
  }

  inline bool startsWith(const string& str, const string& prefix)
  {
    return str.rfind(prefix, 0) == 0;
  }



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


      // String end
      if (_String)
      {
        Tok += Cac;

        if (Cac == _SymStr)
        {
          Tokens.push_back(Tok);
          TokenC += 1;

          Tok.clear();
          _String = false;
        }

        continue;
      }

      // String beg
      if (isStringChar(Cac))
      {
        if (! Tok.empty())
        {
          Tokens.push_back(Tok);
          TokenC += 1;

          Tok.clear();
        }
        
        _String = true;
        _SymStr = Cac;
        
        Tok += Cac;
        continue;
      }


      // Skip whitespace
      if (isIgn(Cac))
      {
        if (! Tok.empty())
        {
          Tokens.push_back(Tok);
          TokenC += 1;

          Tok.clear();
        }
        
        continue;
      }


      // Comment
      if (Cac == '#')
      {
        Fin->ignore(numeric_limits<streamsize>::max(), '\n');
        continue;
      }
      if (Cac == '/' && Fin->peek() == '/')
      {
        Fin->ignore(numeric_limits<streamsize>::max(), '\n');
        continue;
      }


      // Big symbols
      if (! Fin->eof())
      {
        char Nex = Fin->peek();
        string TChar = string({Cac}) +Nex;

        if (find(BigSyms.begin(), BigSyms.end(), TChar) != BigSyms.end())
        {
          if (! Tok.empty())
          {
            Tokens.push_back(Tok);
            TokenC += 1;
            
            Tok.clear();
          }

          Tokens.push_back(TChar);
          TokenC += 1;

          Fin->get(); // consume next

          continue;
        }
      }

      // Symbols
      if (isSeperator(Cac))
      {
        if (! Tok.empty()) {
          Tokens.push_back(Tok);
          TokenC += 1;

          Tok.clear();
        }

        Tokens.push_back(string({Cac}));
        TokenC += 1;

        continue;
      }


      // Add
      Tok += Cac;
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
  void __Int_Route(sType Types, iCon *DCon, bool &UseSkipped)
  {

    if (Types.count(eType::ctFun) && GetW == "fun")
    {
      UseSkipped = true;

      Next();
      sFun *X = __Int_Symb_Fun();
      X->Parent = DCon;

      X->A_Type->Parent = DCon;
      X->A_Type->A_Par->Parent = DCon;
      if (X->A_Type->A_Ret != Nil)
        X->A_Type->A_Ret->Parent = DCon;


      // Find
      if (__Memory_ObjectExits(DCon, X->A_Name))
        throw runtime_error("Object already exist: " +X->A_Name);
      
      
      __Memory_ObjectAdd(DCon, X->A_Name, X);
    }

    ef (Types.count(eType::ctVar) && GetW == "var")
    {
      UseSkipped = true;

      Next();
      auto Xs = __Int_Symb_Var();
      
      for (auto &X: Xs)
      {
        X.second->Parent = DCon;

        // Find
        if (__Memory_ObjectExits(DCon, X.first))
          throw runtime_error("Object already exist: " +X.first);
        
        
        __Memory_ObjectAdd(DCon, X.first, X.second);
      }

    }

    ef (Types.count(eType::ctRec) && GetW == "rec")
    {
      UseSkipped = true;

      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      Next();
      auto X = __Int_Type_Rec();
      X->Parent = DCon;


      // Find
      if (__Memory_ObjectExits(DCon, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DCon, Name, X);
    }

    ef (Types.count(eType::ctTyp) && GetW == "type")
    {
      UseSkipped = true;

      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      Next();
      auto X = __Int_Type_Type();
      X->Parent = DCon;

      // Find
      if (__Memory_ObjectExits(DCon, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DCon, Name, X);
    }

    ef (Types.count(eType::ctMod) && GetW == "mod")
    {
      UseSkipped = true;

      __Int_Mod(DCon);
    }

    ef (Types.count(eType::ctUse) && GetW == "use")
    {
      if (UseSkipped)
        throw runtime_error("Use must be used before definitions");

      __Int_Use();
    }

    el {
      auto Xs = __Int_Symb_Var();
      
      for (auto &X: Xs)
      {
        X.second->Parent = DCon;

        // Find
        if (__Memory_ObjectExits(DCon, X.first))
          throw runtime_error("Object already exist: " +X.first);
        
        
        __Memory_ObjectAdd(DCon, X.first, X.second);
      }

    }
    
  };

  void __Int_Route(sType Types, iCon *DCon)
  {

    if (Types.count(eType::ctFun) && GetW == "fun")
    {
      Next();
      sFun *X = __Int_Symb_Fun();
      X->Parent = DCon;

      X->A_Type->Parent = DCon;
      X->A_Type->A_Par->Parent = DCon;

      if (X->A_Type->A_Ret != Nil)
        X->A_Type->A_Ret->Parent = DCon;


      // Find
      if (__Memory_ObjectExits(DCon, X->A_Name))
        throw runtime_error("Object already exist: " +X->A_Name);
      
      
      __Memory_ObjectAdd(DCon, X->A_Name, X);
    }

    ef (Types.count(eType::ctVar) && GetW == "var")
    {
      Next();
      auto Xs = __Int_Symb_Var();
      
      for (auto &X: Xs)
      {
        X.second->Parent = DCon;

        // Find
        if (__Memory_ObjectExits(DCon, X.first))
          throw runtime_error("Object already exist: " +X.first);
        
        
        __Memory_ObjectAdd(DCon, X.first, X.second);
      }

    }

    ef (Types.count(eType::ctRec) && GetW == "rec")
    {
      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      Next();
      auto X = __Int_Type_Rec();
      X->Parent = DCon;

      // Find
      if (__Memory_ObjectExits(DCon, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DCon, Name, X);
    }

    ef (Types.count(eType::ctTyp) && GetW == "type")
    {
      // Name
      Next();
      __Is_Word(GetW);
      string Name = GetW;


      Next();
      auto X = __Int_Type_Type();
      X->Parent = DCon;

      // Find
      if (__Memory_ObjectExits(DCon, Name))
        throw runtime_error("Object already exist: " +Name);
      
      
      __Memory_ObjectAdd(DCon, Name, X);
    }

    ef (Types.count(eType::ctMod) && GetW == "mod")
    {
      __Int_Mod(DCon);
    }

    el {
      auto Xs = __Int_Symb_Var();
      
      for (auto &X: Xs)
      {
        X.second->Parent = DCon;

        // Find
        if (__Memory_ObjectExits(DCon, X.first))
          throw runtime_error("Object already exist: " +X.first);
        
        
        __Memory_ObjectAdd(DCon, X.first, X.second);
      }

    }

  };

  void __Cod_Route(iCode::block *Block)
  {

    if (GetW == "var")
    {
      Next();

      Block->Codes.push_back(__Cod_Var());
    }

    ef (GetW == "call" && false)
    {
      Next();
      
    }

    ef (GetW == "{")
    {
      Next();

      iCode::block *Sub = new iCode::block();
      Sub->Parent = Block;

      Block->Codes.push_back(Sub);

      while (GetW != "}")
      {
        __Cod_Route(Sub);

        if (GetW == ";") Next();
      }

      Next();
    }

    el
      throw runtime_error("Unknown identifier: " +GetW);

  }


  // Code
  iCode::var* __Cod_Var()
  {
    iCode::var *Ret = new iCode::var();


    // Interface
    __Is_Word(GetW);
    Ret->Name = GetW;
    Next();
    
    // :
    __Is_OK(":");
    Next();

    // Type
    Ret->Type = __Int_Type();


    // ; (optional)
    if (GetW == ";")
      Next();



    // Ret
    return Ret;
  }



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

  void __Int_Mod(iCon *DCon)
  {
    string Name;
    vMod *NMod;

    Next();

    // Name
    __Is_Word(GetW);
    Name = GetW;
    Next();


    // {
    __Is_OK("{");
    Next();


    // Find
    if (__Memory_ObjectExits(DCon, Name))
    {
      auto Cac = __Memory_ObjectGet(DCon, Name);

      if (auto C = dynamic_cast<vMod*>(Cac); C != Nil)
        NMod = C;

      el
        throw runtime_error("Object already exist");
    }
    
    el
    {  // New Mod
      NMod = new vMod();
      __Memory_ObjectAdd(DCon, Name, NMod);
    }


    //NMod->Parent = DMod;

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
    {
      Next();
      return __Int_Type_Rec();
    }

    ef (GetW == "{")
    {
      return __Int_Type_Rec();
    }

    ef (GetW == "c")
    {
      Next();
      return __Int_Type_C();
    }

    el {

      __Is_Word(GetW);
      string Name = GetW;
      Next();

      tRaw *Ret = new tRaw();
      Ret->R_Type = Name;
      
      return Ret;
    }

  }

  iType* __Int_Type_Type()
  {
    // =
    __Is_OK("=");
    Next();

    
    return __Int_Type();
  }

  tRec*  __Int_Type_Rec()
  {
    tRec* Ret = new tRec();


    // Anc
    if (GetW == ":")
    {
      Next();

      // Ancestor
      __Is_Word(GetW);
      Ret->R_Anc = GetW;
      Next();
    }



    // Sets
    if (GetW == "!")
    {
      Next();

      __Is_OK("[");
      Next();

      while (GetW != "]")
      {
        if (GetW == "packed")
          Ret->p_Packed = true;
        
        el
          throw runtime_error("Unknown set: " +GetW);

        Next();


        if (GetW == ",")
          Next();

      }

      Next();
    }



    // {
    __Is_OK("{");
    Next();


    // Content
    while (GetW != "}")
    {
      __Int_Route({ctVar, ctRec, ctTyp}, Ret);

      if (GetW == ";") Next();
    }

    Next();


    // Ret
    return Ret;
  };

  tFun*  __Int_Type_Fun()
  {
    tFun* Ret = new tFun();


    // :
    __Is_OK(":");
    Next();

    // {
    __Is_OK("{");
     

    // Params
    Ret->A_Par = __Int_Type_Rec();


    // Ret
    if (GetW == "->")
    {
      Next();

      // Type
      Ret->A_Ret = __Int_Type();
    }


    // Ret
    return Ret;
  }

  tC*    __Int_Type_C()
  {
    tC *Ret = new tC();


    // (
    __Is_OK("(");
    Next();


    // string
    __Is_String(GetW);
    Ret->R_CType = GetW;
    Next();


    // ,
    __Is_OK(",");
    Next();


    // size
    __Is_Number(GetW);
    Ret->Size = stoul(GetW);
    Next();


    // )
    __Is_OK(")");
    Next();


    // Ret
    return Ret;
  }



  // Symbols
  vector<pair<string, sVar*>> __Int_Symb_Var()
  {
    vector<pair<string, sVar*>> Ret;

    // Vars
    vector<string> Names;


    while (true)
    {
      // Name
      __Is_Word(GetW);
      Names.push_back(GetW);
      Next();


      if (GetW == ",")
      {
        Next();
        continue;
      }
      el
        break;
    }
    


    // :
    __Is_OK(":");
    Next();


    // Type
    iType *SType = __Int_Type();
    


    // Scan
    for (auto &X: Names)
    {
      // Add
      sVar *Obj = new sVar();
      Obj->A_Typ = SType;


      Ret.push_back({X, Obj});
    }


    // Ret
    return Ret;
  };

  sFun* __Int_Symb_Fun()
  {
    sFun* Ret;

    // Vars
    Ret = new sFun();


    // Name
    __Is_Word(GetW);
    Ret->A_Name = GetW;
    Next();


    // Type
    Ret->A_Type = __Int_Type_Fun();


    // Sets
    if (GetW == "!")
    {
      Next();

      __Is_OK("[");
      Next();

      while (GetW != "]")
      {
        if (GetW == "cdecl")
          Ret->p_CDecl = true;

        ef (GetW == "inline")
          Ret->p_Inline = true;

        ef (GetW == "export")
          Ret->p_Export = true;

        ef (GetW == "extern")
          Ret->p_Extern = true;

        ef (GetW == "noexcept")
          Ret->p_NoExcept = true;

        ef (GetW == "nomangle")
          Ret->p_NoMangle = true;
        
        el
          throw runtime_error("Unknown set: " +GetW);

        Next();


        if (GetW == ",")
          Next();

      }

      Next();
    }


    if (Ret->p_Extern && Ret->p_Inline)
      throw runtime_error("A function cannot be both extern and inline");

    if (Ret->p_Extern && Ret->p_NoExcept)
      throw runtime_error("A function cannot be both extern and noexcept");

    if (Ret->p_Inline && Ret->p_NoMangle)
      throw runtime_error("A function cannot be both inline and nomangle");


    // if extern, escape
    if (Ret->p_Extern)
      return Ret;
    


    // Code
    __Is_OK("{");
    Next();

    while (GetW != "}")
    {
      __Cod_Route(&Ret->Code);

      if (GetW == ";") Next();
    }

    Next();


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

