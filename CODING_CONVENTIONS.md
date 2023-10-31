Coding Convetions (WIP)
==================================

### Files:
Start filenames with lower case \
Use underline for multi word filenames  \
`header_file.hpp` \
`source_file.cpp`
### Header:
At the beginn of a header use:
```c++
#pragma once
```
### Conditional Statements:
```c++
// do (pad parenthesized expressions)
if ( x ) {
}
// or (else on same line as brackets)
if ( x ) {
} else {
}
// or (pad parenthesized expressions)
x = ( y * 0.5f );
```

### Function Names:
```c++
// names start with upper case
// Prefix arguments with 'a', variable name in camel case
void Function(int aFoo);
// use camel case for mulit-word names
void FunctionNamingIsEasy();
```

### Variable Names:
```c++
// variables start with lower case
int x;
// typedefs end with _t
typedef int handle_t;
// structs end with _s
struct entity_s;
// old enums, think about scope
enum Fruit {FRUIT_APPLE, FRUIT_ORANGE};
// preferred enums
enum class Fruit {APPLE_PENCIL, ORANGE};
```

### Class Names:
```c++
namespace namespace_name_lower_case_separated_by_underscores
{ // Braces for namespaces, types, methods start in the next line

    class ClassName
    { 
    public:
        void MethodName() const {
            int localVariablesInCamelCase = 0; 
        }
    protected:
        // The more public it is, the more important it is. Therefore,
        // order like follows: public on top, then protected, then private
        // within, functions first then variables

        template <typename Template, typename Parameters>
        void templatedMethod() {
            // Template type parameters are named in camel case,
            // starting with a capital letter.
        }
    private:
        // Prefix members with 'm', variable name in camel case
        ComplexType mValue;
    };
}
// New line at the end of the file:

```