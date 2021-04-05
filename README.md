# HustProgramDesign

​		Program Design, General Course Design of CSE College, HUST for students enrolled in 2018.

​		This is one possible solution for the Binary Sudoku Design. Currently we uploaded our codes of core algorithms to Ver_3.0 

## Version Comment

#### 	Ver_1.0 Standard DPLL Algorithm

#### 	Ver_2.0 DPLL with Advanced Simplification

​			Modularized basic algorithms and data structures.

​			In V1.0, we copy the whole simplified Clause Set every time we need to start a new branch in the search tree.

​			The core idea of this method is to replace the Clause Set in a search tree with an result array.

#### 	Ver_3.0 DPLL with CDCL & Implication Graph

​			Improves Modularity and adds new functions:

​					1) Generate a Binary Sudoku puzzle and ensure its unique solution;

​					2) Convert a generated Binary Sudoku problem into SAT CNF format, Tseytin Transformation applied.

#### 	Ver_3.1 UIP in CDCL

UIP means Unique Implication Point. In the Implication Graph, this point must satisfy:

​				1) In current decision level;

​				2) Stands on every path in current decision level to the conflict;

​				3) Closest to the conflict point among all points satisfy 1) and 2).

​			Searching for and using UIP help shrink the length of learnt clauses, boosting the solving process.

#### 	Ver_3.2 Dual Queue Analysis in CDCL

​			Minor Update -- Using two queues when analyzing the implication graph.

#### 	Ver_3.3  Definite Simplification before DPLL

​			Special case modification.

​			As converted Binary Sudoku problems usually contains lots of single clauses, when judging the status of the dataset, because of the Advanced Simplification we used, it could waste lots of time on those clauses which could be deleted at first.
​			Hence we add Definite Simplification and allow it to run for only once before the formal DPLL & CDCL process.
