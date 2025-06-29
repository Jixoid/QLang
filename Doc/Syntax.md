
___
## Record

`cRec(cObj, iType)`

Definition:
```kotlin
rec myRec {
  var X,Y: u32;
}

// or
type myRec = rec {
  var X,Y: u32;
}

// or
type myRec = {
  var X,Y: u32;
}


// inheritance
type myRec2 = rec: myRec {
  var Z: u32;
}


// anonymous record
var myVar: {
  var X,Y: u32;
}
```


___
## Function

`cFun(cObj, iSymb)`
`cFunT(cObj, iType)`

Definition:
```kotlin
fun Add: {X,Y: u32} -> u32
{
  ret (X + Y);
}

fun Big: {p0,p1: u32} -> {Big: u32; IsBig: bool}
{
  ret {(p0 > p1) ? p0:p1, p0>p1};
}


// Definition
type fAdd = fun: {X,Y: u32} -> u32

fun: main {} -> u32
{
  var myAdd: fAdd;
  myAdd ​= @Add;

  myAdd(2,3);
}
```
