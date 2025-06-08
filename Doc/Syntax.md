
> [!warning] This document is part of the **QLang**
> Licensed under the [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).
>
> Copyright (c) 2025 by Kadir Aydın.


___
## Type C

`cType_C(cObj, iType)`

> [!warning] Usage
> This is a usage that should be avoided.

Definition:
```kotlin
type u32 = c("unsigned int");

// anonymous decl
var X: c("unsigned int");
```


___
## Record

`cRec(cObj, iType)`

Definition:
```kotlin
rec myRec {
  var X,Y: u32;
};

// or
type myRec = rec {
  var X,Y: u32;
};


// inheritance
type myRec2 = rec(myRec) {
  var Z: u32;
}


// anonymous record
var myVar: rec {
  var X,Y: u32;
};
```

