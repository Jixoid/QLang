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



class cMod;

class cObj
{
public:
  cMod *Parent = Nil;

  virtual ~cObj() = default;
};


// Basis types
class iType: public cObj
{
public:
  virtual ~iType() = default;
};

class iSymb: public cObj
{
public:
  virtual ~iSymb() = default;

  string A_Sym;

  bool A_Static;
  bool A_Dep = false;
};



// Virtual (Namespaces)
class cMod: public iSymb
{
public:
  vector<pair<string, cObj*>> Objs;
};


// Types
class cRaw: public iType
{
public:
  string R_Type;
};

class cRec: public iType
{
public:
  vector<pair<string, cObj*>> Objs;

  string R_Anc;
  cRec  *A_Anc;
};

class cType_C: public iType
{
public:
  string R_CType;
};

class cFunT: public iType
{
public:
  iType *A_Ret = Nil;

  vector<string> R_Par;
  vector<iType*> A_Par;
};


// Symbols
class cFun: public iSymb
{
public:
  cFunT *A_Type;
};

class cVar: public iSymb
{
public:
  iType *A_Typ;
};


// Other types
class cFile: public cMod
{
public:
  vector<string> Libs;
};

