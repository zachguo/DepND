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
* All MST **should not cross punctuation marks** along the direction of flow, no matter the flow moves towards left or right (relatively to the position of trigger word). But maybe some arcs can (see below).

#### Specification

| Trigger_POS | Triggers | Rule |
|  --- | --- | --- |
| RB, DT, JJ | no, not, hardly, never | **gMST** |
| JJ | absent, negative, unable, unlikely | **gMST** |
| CC | neither, nor | **gMST** |
| VB*, IN | fail, lack, lacking, excluding, without | **sMST** |
| NN | none, lack, absence, failure (to/of)| **sMST** |

### Additions:
#### Minor rules:
* **ggMST** = maximal spanning tree from the immediate governor of the immediate governor;
* **$_Elevate** = elevate through all possible $ arcs;
* **SUB&Right** = only span towards right and span through SUB arc, span nothing if there's no SUB arc or right part.

#### Situations that cannot be coped with default rules:

| trigger | trg_pos | trg_dep | governor | gvn_pos | gvn_dep | Rule | Additional-Rules |
| --- | --- | --- | --- | --- | --- | --- | --- |
| not | RB | DEP | but,and | CC | NMOD | ggMST | or **CC_Elevate+gMST**; **SUB&Right** |
| not | RB | PMOD | in | IN | PMOD |gMST| PMOD arc **can** cross punctuation?|
| not | RB | VMOD | does,did,is | VBZ | ROOT | gMST | **cannot** span left towards "Although/IN", "Therefore/RB", "Moreover/RB" or "like/IN" through a VMOD arc; or **SUB&Right** |
| hardly | RB | AMOD | any | DT | NMOD | ggMST | or **AMOD_Elevate+gMST** |
| never | RB | NMOD | effect | NN | OBJ | gMST | **SUB&Right** |
| rather (than) | RB | PMOD | for | IN | | gMST | **SUB&Right** and only keep words whose indice are larger than that of "rather than" |
| negative | JJ | NMOD | regulation, factors | NN, NNS | ROOT, PMOD | gMST | only span through of/IN NMOD arc; "factors" has no children |
| absence | NN | PMOD | in | IN | NMOD | gMST | when "absence" is in a PP phrase, the head would be the preposition |
| none | NN | SUB | had | VBD | ROOT | gMST | **SUB&Right** |
| without | IN | PMOD | with | IN | PMOD | gMST | only span through PMOD towards right |
| lack | VBP | SBAR | | | | sMST | forbid VMOD branching to MD or VB* |
| lacking | VBG | VC | is | VBZ | SUB | gMST | SUB arc can span across punctuations |
| denied | VBN | VC | are | VBP | ROOT | gMST | only span through SUB arc |
| excluded | VBN | VC | be | VB | VC | ggMST | or **VC_Elevate+gMST** |

### Rules are vulnerable to:

* Performance of PP attachment in the parser;
> e.g. The PP "on sth" in "no effect on sth" is often wrongly attached to other NN rather than effect. As a result, one must design more complicated scope rules rather than a simple gMST.
* Inconsistency of decision in whether including the left part of parsed tree governed by a SUB arc.
> A safer solution may be just flow through towards right. Sacrifice false negative for false positive. If one want to include the part governed by a SUB arc, it's safer to avoid flowing across or just span through the SUB arc.

## Procedures after determining the scope
* Coping with double/triple negation.
* Output formatting.