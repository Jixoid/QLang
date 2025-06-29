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
#include <format>

#include "Basis.hpp"

#include "QTypes.hpp"

#include "Proc.hpp"


using namespace std;
using namespace jix;



// QABI
inline string NS_var(const string &Name)
{
  return format("_v{}{}", Name.size(), Name);
}

inline string NS_rec(const string &Name)
{
  return format("_r{}{}", Name.size(), Name);
}

inline string NS_typ(const string &Name)
{
  return format("_t{}{}", Name.size(), Name);
}

inline string NS_typC(const string &Name)
{
  return format("_tc{}{}", Name.size(), Name);
}

inline string NS_typF(const string &Name)
{
  return format("_tf{}{}", Name.size(), Name);
}

inline string NS_fun(const string &Name, const string &RetV, const vector<string> &Params)
{
  string Ret = format("_f{}{}", Name.size(), Name);

  // Ret
  if (! RetV.empty())
    Ret += format("__r{}{}", RetV.size(), RetV);

  // Params
  for (auto &X: Params)
    Ret += format("__p{}{}", X.size(), X);

  return Ret;
}

inline string NS_mod(const string &Name)
{
  return format("_m{}{}", Name.size(), Name);
}




// Check
iType* Find(iCon *Con, const string &Type, bool Except = true)
{
  // Default Context
  for (auto &X: Con->Objs)
    if (X.first == Type)
    {
      if (auto C = dynamic_cast<iType*>(X.second); C != Nil)
        return C;
      
      el 
        throw runtime_error("Symbol is not type");
    }


  // Parent
  if (Con->Parent != Nil)
    if (auto X = Find(Con->Parent, Type, false); X != Nil)
      return X;
  


  if (Except)
    throw runtime_error(format("Type not found: \"{}\"", Type));

  el
    return Nil; 
}


void IfRaw(iCon *Con, iType *Type)
{
  if (auto C = dynamic_cast<tRaw*>(Type); C != Nil)
    C->A_Type = Find(Con, C->R_Type);
}



void Procer(iCon *NCon, cObj *NObj, const string &Name, bool Static, string OP)
{
  // Symbols
  if (auto C = dynamic_cast<sFun*>(NObj); C != Nil)
  {
    if (Static)
      C->A_Sym = OP +NS_fun(Name, "", {});

    Procer(NCon, C->A_Type, "", false, C->A_Sym);
  }

  ef (auto C = dynamic_cast<sVar*>(NObj); C != Nil)
  {
    if (Static)
      C->A_Sym = OP +NS_var(Name);


    IfRaw(NCon, C->A_Typ);
  }


  // Types
  ef (auto C = dynamic_cast<tRaw*>(NObj); C != Nil)
  {
    C->A_Sym = OP +NS_typ(Name);

    IfRaw(NCon, C);
  }

  ef (auto C = dynamic_cast<tRec*>(NObj); C != Nil)
  {
    C->A_Sym = OP +NS_rec(Name);

    Procer(C, false, C->A_Sym);
  }

  ef (auto C = dynamic_cast<tC*>(NObj); C != Nil)
  {
    C->A_Sym = OP +NS_typC(Name);
  }

  ef (auto C = dynamic_cast<tFun*>(NObj); C != Nil)
  {
    C->A_Sym = OP +NS_typF(Name);

    if (C->A_Ret != Nil)
      Procer(NCon, C->A_Ret, "o", false, C->A_Sym);

    Procer(NCon, C->A_Par, "i", false, C->A_Sym);
  }
  
}

void Procer(iCon *NCon, bool Static, string OP)
{
  for (auto &X: NCon->Objs)
    Procer(NCon, X.second, X.first, Static, OP);
}

