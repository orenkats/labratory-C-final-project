
The assembler operates through three main stages:

1. **Pre-Processing**
2. **First Pass**
3. **Second Pass**

These stages constitute the core algorithm of the assembler.

**Pre-Processing:**
In this stage, the pre-assembler reads the source file line by line, identifying macro definitions. When a macro is detected, its lines are added to a macro table and substituted in the source file.

**First Pass:**
During the first pass, symbols (labels) are identified and assigned numerical values corresponding to their memory addresses.

**Second Pass:**
In the second pass, the assembler produces the final machine code by replacing operation names with their binary equivalents and symbol names with their assigned memory locations.
