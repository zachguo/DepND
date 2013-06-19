DepNegEx
========

Negation detection using dependency parsing.

## Basic Procedures ##

* Separate text input into sentences.
* Search for negation triggers throughout each sentence.
* For sentences containing negation triggers, parse each of them using a dependency parser.
* Use parsed tree and rules to determine the scope of negation.
* Extract keywords or entities in the scope of negation.

## Rules for determining scope

### Default rules:

#### Major rules:
* **gMST** = maximal spanning tree from the immediate governor;
* **sMST** = maximal spanning tree from self;
* All MST should not cross punctuation marks along the direction of flow, no matter the flow moves towards left or right. But maybe some arcs can (see below).

#### Specification
* For RB,DT,JJ triggers:
> including `no, not, hardly, absent, negative, unable`  
> **gMST**
* For CC triggers:
> including `neither, nor`  
> **gMST**
* For VB*,IN triggers:
> including `fail, lack, lacking, excluding, without`  
> **sMST**
* For NN triggers:
> including `none, lack, absence, failure`  
> **sMST**

### Additions:
#### Minor rules:
* **ggMST** = maximal spanning tree from the immediate governor of the immediate governor;

#### Situations that cannot be coped with default rules:

| trigger | trg_pos | trg_rel | governor | gvn_pos | gvn_rel_upper | Rule | Additional-Rules |
| --- |
| not | RB | DEP | but,and | CC | NMOD | ggMST | restricted to CC parent; only flow towards right |
| not | RB | PMOD | in | IN | PMOD |gMST| PMOD arc **can** cross punctuation?|
| not | RB | VMOD | does,did,is | VBZ | ROOT | gMST | **cannot** flow left towards "Although/IN", "Therefore/RB", "Moreover/RB" or "like/IN" through a VMOD arc |
| negative | JJ | NMOD | regulation, factors | NN, NNS | ROOT, PMOD | gMST | only flow through of/IN NMOD arc; "factors" has no children |
| without | IN | PMOD | with | IN | PMOD | gMST | only flow through PMOD towards right |
| absence | NN | PMOD | in | IN | NMOD | gMST | when "absence" is in a PP phrase, the head would be the preposition |
| lack | VBP | SBAR | | | | sMST | should not allow VMOD branching to MD or VB* |
| rather (than) | RB | PMOD | for | IN | | gMST | should only span towards right and only keep words whose induce are larger than that of "rather than" |
| lacking | VBG | VC | is | VBZ | SUB | gMST | SUB arc can cross punctuations |


### Rules are vulnerable to:

* Performance of PP attachment in the parser;
> e.g. The PP "on sth" in "no effect on sth" is often wrongly attached to other NN rather than effect. As a result, one must design more complicated scope rules rather than a simple gMST.
* Inconsistency of decision in whether including the left part of parsed tree governed by a SUB arc.
> A safer solution may be just flow through towards right. Sacrifice false negative for false positive. If one want to include the part governed by a SUB arc, it's safer to avoid flowing across the SUB arc.