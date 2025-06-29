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
#include <set>

#include "Basis.hpp"


using namespace std;
using namespace jix;



enum eType
{
  ctUse,
  ctMod,
  ctFun,
  ctVar,
  ctRec,
  ctTyp,
};

using sType = set<eType>;



class iCon;

class cObj
{
public:
  iCon *Parent = Nil;

  string A_Sym;
  string A_Name;
  bool D_Writed = false;

  virtual ~cObj() = default;
};



// Basis types
class iType: public virtual cObj
{
public:
  u32 Size = 0;
};

class iSymb: public virtual cObj
{
public:
  bool p_Extern = false;
  bool p_Export = false;
};

class iCon: public virtual cObj
{
public:
  vector<pair<string, cObj*>> Objs;
};


// Code
namespace iCode
{
  class code
  {
  public:
    virtual ~code() = default;
  };


  class val: public virtual code
  {
  public:
    iType *Type = Nil;
  };



  // Block
  class block: public code
  {
  public:
    vector<code*> Codes;
    block *Parent = Nil;
  };


  // Codes
  class var: public val
  {
  public:
    string Name;
    iType *Type;
    val   *Default = Nil;
  };

  class assign: public code
  {
  public:
    var *Var;
    val *Value;
  };

  class call: public val
  {
  public:
    string Name;

    vector<val*> Pars;
  };


  // ...
  class _if: public code
  {
  public:
    val *Cond;

    block Then;
    block Else;
  };

  class _while: public code
  {
  public:
    val *Cond;

    block Body;
  };

}



// Virtual (Namespaces)
class vMod: public iSymb, public iCon
{
public:

};


// Types
class tRaw: public iType
{
public:
  string R_Type;
  iType *A_Type = Nil;
};

class tRec: public iType, public iCon
{
public:
  string R_Anc;
  tRec  *A_Anc;

  bool p_Packed;
};

class tFun: public iType
{
public:
  tRec  *A_Par;
  iType *A_Ret = Nil;

  bool p_CDecl = false;
};

class tC: public iType
{
public:
  string R_CType;
};


// Symbols
class sFun: public iSymb
{
public:
  tFun *A_Type;

  iCode::block Code;


  bool p_CDecl = false;
  bool p_Inline = false;
  bool p_NoExcept = false;
  bool p_NoMangle = false;
};

class sVar: public iSymb
{
public:
  iType *A_Typ;
};


// Other types
class cFile: public vMod
{
public:
  vector<string> Libs;
};

