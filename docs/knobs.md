Knobs (will) control all randomized choices made by the fuzzing engine. The
intent is to find optimal values for knobs using genetic algorithm, machine
learning, etc.

Examples of the choices that the engine can make using knobs:
* Choosing whether to add a given element to the corpus based on what
  features it has, its size, its resource consumption, etc.
* Choosing a corpus element to mutate, or an element pair to cross-over.
* Choosing how to mutate.

`Knobs` is effectively a fixed-size array of bytes with named elements.
The engine loads this array at startup or uses a default value zero.
The engine may also pass Knobs to a custom mutator that supports it.

Each knob has its own interpretation.
Some knobs are probability weights, with `0` meaning "never" or "rare"
 and 255 meaning "frequently".
Some knobs have a meaning in combination with other knobs, e.g.
 when choosing one of N strategies, N knobs will be used as weights.