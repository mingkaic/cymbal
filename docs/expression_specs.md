# Defining variables
```
let a[3,3];
```
tensor `a` with shape <3,3> and populated with unique symbols.
```
let a; 
```
scalar `a` with a unique symbol.
```
let b[2,2];
let a[3,3](0::2,1:) :: b;
```
tensor `a` with shape <3,3> with element with rows 0 and 2, and columns 1 and 2 referencing symbols in b.
```
let b;
let a[3,3] :: b;
```
tensor `a` with shape <3,3> with every element referencing the scalar symbal b.
```
let b[3,3];
let a[3,3] :: b;
```
tensor `a` with shape <3,3> with every element referencing tensor `b` one to one.
# Expressions
Elementary Operations
- `-a` negative a
- `a+b` adds a and b
- `a-b` subtracts b from a
- `a*b` multiplies a and b
- `a/b` divides a and b
Tensor Views

- `a(start:end:incr,:,:)` references slices of tensor a using numpy indexing syntax
- `a(...) :: b(...)` assign b view to a view.
# Data Input
