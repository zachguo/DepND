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

* For RB,DT,JJ,IN triggers:
> including `no, not, absent, negative, without`  
> **gMST** = maximal spanning tree from the immediate governor 
* For VB* triggers:
> including `fail, lack`  
> **sMST** = maximal spanning tree from self;
* All MST should not cross punctuation marks, but maybe some arcs can (see below).

### Situations that cannot be coped with default rules:

| trigger | trg_pos | trg_rel | governor | gvn_pos | gvn_rel_upper | Rule | Additional-Rules |
| --- |
| not | RB | DEP | but,and | CC | NMOD | ggMST | restricted to CC; only flow towards right |
| not | RB | PMOD | in | IN | PMOD |gMST| PMOD arc **can** cross punctuation?|
| not | RB | VMOD | does,is | VBZ | ROOT | gMST | **cannot** flow left towards "Although/IN", "Therefore/RB" or "like/IN" through a VMOD arc |
| negative | JJ | NMOD | regulation, factors | NN, NNS | ROOT, PMOD | gMST | only flow through of/IN NMOD arc; "factors" has no children |
| without | IN | PMOD | with | IN | PMOD | gMST | only flow through PMOD towards right |


* ggMST = maximal spanning tree from the immediate governor of the immediate governor;

### Rules are vulnerable to:

* Performance of PP attachment in the parser;
> e.g. The PP "on sth" in "no effect on sth" is often wrongly attached to other NN rather than effect. As a result, one must design more complicated scope rules rather than a simple gMST.
* Inconsistency of decision in whether including left part of parsed tree which is usually governed by a SUB arc.
> A safer solution may be just flow through towards right. Sacrifice false negative for false positive. However, this may fail to capture a correct scope in sentence like "Phorbol activation was positively modulated by Ca2+ influx while <SCOPE>TNF alpha activation was <NEG>not</NEG></SCOPE>." 