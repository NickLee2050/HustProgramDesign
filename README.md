# HustProgramDesign
Program Design affiliated to General Curriculum Design of CSE College, HUST for students enrolled in 2018.

This is one possible solution for the Binary Sudoku Design.
Currently we uploaded our codes of core algorithms and some 
Here's the version comment:
  V1.0 Standard DPLL Algorithm
  
  V2.0 DPLL with Advanced Simplification
       Modularized basic algorithms and data structures.
       In V1.0, we copy the whole simplified Clause Set every time we need to start a new branch in the search tree.
       The core idea of this method is to replace the Clause Set in a search tree with an result array.
  V3.0 DPLL with CDCL & Implication Graph
       Improves Modularity and adds new functions:
          1) Generate a Binary Sudoku puzzle and ensure its unique solution;
          2) Convert a generated Binary Sudoku problem into SAT CNF format, Tseytin Transformation applied.
  V3.1 DPLL with CDCL & Implication Graph & UIP
       UIP means Unique Implication Point. In the Implication Graph, this point must satisfy:
          1) In current decision level;
          2) Stands on every path in current decision level to the confict;
          3) Closest to the conflict point among all points stasify 1) and 2).
       Searching for and using UIP help shrink the length of learnt clauses, boosting the solving process.
  V3.2 DPLL with CDCL & Implication Graph & UIP & Dual Queue Analysis
       Minor Update -- Using two queues when analyzing the implication graph.
  V3.3 DPLL with CDCL & Implication Graph & UIP & Dual Queue Analysis & Definite Simplication
       Special case modification.
       As converted Binary Sudoku problems usually contains lots of single clauses, when judging the status of the dataset,
       because of the Advanced Simplication we used, it could waste lots of time on those clauses which could be deleted at first.
       Hence we add Definite Simplication and allow it to run for only once before the formal DPLL & CDCL process.
