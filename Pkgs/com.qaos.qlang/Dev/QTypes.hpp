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
};

using sType = set<eType>;



class cMod;

class cObj
{
public:
  cMod *Parent = Nil;

  bool A_Static;

  string A_Sym;
  bool A_Dep = false;

  virtual ~cObj() = default;
};

using objList = vector<pair<string, cObj*>>;



class cMod: public cObj
{
public:
  objList Objs;
};

class cFile: public cMod
{
public:
  vector<string> Libs;
};


class cFun: public cObj
{
public:
  string R_Ret;
  vector<string> R_Par;
};

class cVar: public cObj
{
public:
  cObj *A_Typ;
  string R_Typ;
};

class cRec: public cMod
{
public:
  string R_Anc;
};

