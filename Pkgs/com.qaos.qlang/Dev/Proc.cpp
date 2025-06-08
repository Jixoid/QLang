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
  return format("·v{}{}", Name.size(), Name);
}

inline string NS_rec(const string &Name)
{
  return format("·r{}{}", Name.size(), Name);
}

inline string NS_fun(const string &Name, const string &RetV, const vector<string> &Params)
{
  string Ret = format("·f{}{}", Name.size(), Name);

  // Ret
  if (! RetV.empty())
    Ret += format("_r{}{}", RetV.size(), RetV);

  // Params
  for (auto &X: Params)
    Ret += format("_p{}{}", X.size(), X);

  return Ret;
}

inline string NS_mod(const string &Name)
{
  return format("·m{}{}", Name.size(), Name);
}




// Check
/*
iType* Find(cMod *Context, const string &Type, bool Except = true)
{
  // Default Context
  for (auto &X: Context->Objs)
    if (X.first == Type)
    {
      if (auto C = dynamic_cast<iType*>(X.second); C != Nil)
        return C;
      
      el 
        throw runtime_error("Symbol is not type");
    }


  // Parent
  if (Context->Parent != Nil)
    if (auto X = Find(Context->Parent, Type, false); X != Nil)
      return X;
  


  if (Except)
    throw runtime_error(format("Type not found: \"{}\"", Type));

  el
    return Nil; 
}


void Check(cMod *Context, cVar *Obj)
{
  cout << format("@CHECK(Var) {{R_Typ: {}}}", Obj->R_Typ) << endl;

  Obj->A_Typ = Find(Context, Obj->R_Typ);
}
*/



void Procer(cRec *NRec) { for (auto &X: NRec->Objs)
{
  // Canonicalize
  if (auto C = dynamic_cast<cRec*>(X.second); C != Nil)
  {
    Procer(C);
  }

}}

void Procer(cMod *NMod, bool Static, string OP) { for (auto &X: NMod->Objs)
{
  // Canonicalize
  if (auto C = dynamic_cast<cFun*>(X.second); C != Nil)
  {
    C->A_Sym = OP +NS_fun(X.first, "", {});
  }

  ef (auto C = dynamic_cast<cVar*>(X.second); C != Nil)
  {
    C->A_Sym = OP +NS_var(X.first);

    //Check(NMod, C);
  }

  ef (auto C = dynamic_cast<cRec*>(X.second); C != Nil)
  {
    Procer(C);
  }

  ef (auto C = dynamic_cast<cMod*>(X.second); C != Nil)
  {
    C->A_Sym = OP +NS_mod(X.first);

    Procer(C, Static, C->A_Sym);
  }

}}

