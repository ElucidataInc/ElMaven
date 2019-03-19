# Code style guide for contributors
This document is an overview of the coding style that the developers would like
contributors to follow when writing any new C++ code for the project. A lot of
this document was inspired by other prominent style guides, most notably [Qt's
coding style guide](https://wiki.qt.io/Qt_Coding_Style).

The [clang-format](https://github.com/ElucidataInc/ElMaven/blob/develop/clang-format)
file in project's source directory can be used to format documents adhering to
this style guide (some manual formatting might still be needed to achieve exact
results):

```
clang-format -style=file
```

Please feel free to let the developers know if you feel that they have missed
something that should be included in this (clearly non-exhaustive) document, by
opening an discussion thread on GitHub. Suggestions for improving this guide
(with reasonable arguments) are also welcome.


### Code Layout
  * Use 4 spaces (not tabs) for each level of indentation. Spaces allow for
  more precise alignment whenever needed.
  * Indentation is not required before access modifiers (i.e., `public`,
  `private`, etc) and for contents enclosed is `namespace` blocks (although
  we do not enforce this).
  * Limit all lines to a maximum of 80 characters. This allows having multiple
  files open side by side.


### Declaration and naming
  * Declare each variable on a separate line.
  * Choose precise variable names describing their value. Single character
  names are only fine if they are used as temporary counters.
  * Use camel-case with first character in lowercase for variables and
  functions.
  * Use camel-case with first character in uppercase for classes, structs and
  namespaces.
  * Use camel-case starting with an underscore, for private class members
  (propertiesand methods).

```C++
// Incorrect
class somedialog {
    int h, w;

public:
    void event_handler();
}

// Correct
class SomeDialog
{
    int _height;
    int _width;

public:
    void eventHandler();
}
```
  * Use all uppercase letters for naming macros.
  * If a function has many arguments or if arguments do not fit on a single
  line, each parameter should be given in different line and left-aligned.

```C++
// Incorrect
int aFunctionWithLongArguments(firstArgumentIsLong, secondArgumentIsAlsoLong, thisOneExceededColumnLimit);

// Correct
int aFunctionWithLongArguments(firstArgumentIsLong,
                               secondArgumentIsAlsoLong,
                               thisOneExceededColumnLimit)
```


### Whitespaces
  * Use blank lines to separate suitable groups of statements.
  * Always use spaces between variables, operators and values.
  * Always use a single space after a keyword and before a curly brace.

```C++
// Incorrect
for(i=0;i<limit;++i){
    int x=5+i*2;
    magic(x);
}

// Correct
for (i = 0; i < limit; ++i) {
    int x = 5 + (i * 2);
    magic(x);
}
```


### Braces
Use [K&R](https://en.wikipedia.org/wiki/Indentation_style#K&R_style) style for
braces. Functions and classes have braces on a seaparate line of their own. For
everything else use attached braces The opening brace goes on the same line as
the start of the statement

```C++
// Incorrect
void SomeDialog::eventHandler() {
    doSomething();
    doMore();

    ...

    if (somethingHappened)
    {
        doThis();
    }
    else
    {
        doThat();
    }
}

// Correct
void SomeDialog::eventHandler()
{
    doSomething();
    doMore();

    ...

    if (somethingHappened) {
        doThis();
    } else {
        doThat();
    }
}
```

Curly braces may only be used when the body of a conditional statement contains
more than one line. Braces, however, _must_ be used if conditions themselves
occupy multiple lines or there is an accompanying `else` statement to an `if`.

```C++
 // Fine
if (someCondition)
    return false;

// Incorrect
if (firstConditionThatTakesALotOfSpace
    || secondConditionTakesALotOfSpaceAsWell)
    return false;

// Correct
if (firstConditionThatTakesALotOfSpace
    || secondConditionTakesALotOfSpaceAsWell) {
    return false;
}

// Incorrect
if (someCondition)
    doSomething();
else {
    doSomeOtherThing();
    return -1;
}

// Correct
if (someCondition) {
    doSomething();
} else {
    doSomeOtherThing();
    return -1;
}
```

Furthermore, please avoid having statements that are on the same line as their
`if`/`else` conditions or their `for`/`while` loop.

```C++
// Incorrect
for (auto elem : largeContainer) if (someCondition) doSomething();

// Correct
for (auto elem : largeContainer) {
    if (someCondition)
        doSomething();
}
```

The same goes for functions and methods — do not write function body on the same
line as their prototype. In fact, the declaration and definition of each method
and function should be in a separate header and a source file (and even inside
the source file, the body should start from the next line of the function
prototype, following
[K&R](https://en.wikipedia.org/wiki/Indentation_style#K&R_style) style).
However, these rule can be ignored in case of short `inline` functions, they can
be defined and declared on the same line in the header file.

```C++
// Okay
class A {
    int _privateValue;

public:
    inline int getValue() { return _privateValue; }
}
```


### Parentheses
  * Use parentheses to group expressions (even if you know precedence order of
  operations by heart!).
  * Commas go at the end of wrapped lines.
  * Operators start at the beginning of the new lines.

```C++
// Incorrect
if (longExpression +
    otherLongExpression +
    otherOtherLongExpression) {
}

// Correct
if (longExpression
    + otherLongExpression
    + otherOtherLongExpression) {
}
```


### Comments and documentation strings
  * Use comments sparingly for code or decisions that might be not be apparent
  otherwise.
  * Always document functions in header files using [Doxygen](http://doxygen.nl/manual/docblocks.html)
  style strings and format.


### Miscellaneous advice
  * Prefer to include standard headers before shared library headers which
  itself should be included before local header files.
  * Forward declare classes in header files and include their class headers in
  the corresponding C++ files only, unless, absolutely required.
  more information is required in header.
  * End your source files with an empty newline. Most modern compilers would not
  mind though even if EOF newline was absent.
  * Name all source files in lowercase without any separators between words.
  * When strictly following a rule makes your code look bad, feel free to break
  it, but please be consistent in whichever style to choose to adhere to.
