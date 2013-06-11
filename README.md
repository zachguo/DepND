DepNegEx
========

Negation detection based on dependency parsing

## Basic Procedures ##

* Separate text input into sentences.
* Search for negation triggers throughout each sentence.
* For sentences containing negation triggers, parse each of them using a dependency parser.
* Use parsed tree and rules to determine the scope of negation.
* Extract keywords or entities in the scope of negation.
