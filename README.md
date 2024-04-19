# Compiler Project
Author: Matthew Michaud (matthew.michaud@alumni.ucalgary.ca)

## Overview
Having enjoyed creating a compiler for a toy language in university, I wanted to build a compiler for a more substantial language. For this compiler, I'm playing around with a bespoke language design that's most related to C/C++ but will borrow ideas from Rust, Java, and Python. I'm also using a fascinating new tool called [Carburetta](https://carburetta.com/) that generates both the scanner and parser for the compiler.

I've nearly finished implementing a proof of concept with only a few things left to do to finish the code generator (targeting Linux on ARMv8 A64). There are probably many bugs to find once I finish the generator. One area needing significant work is the language's grammar since it presently contains many ambiguities.

I'm on the fence about my design for the compiler. In particular, I'm not content with the high degree of coupling to the structures that store the syntax tree. For now, I'm leaving it alone to read "Compilers: Principles, Techniques, & Tools (2nd ed.)" to review what I learned in university and gain some further insight to help me improve what I've done here.
