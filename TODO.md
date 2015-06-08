# TODO

- improve unit testing to prove resulting states are correct. Implement a unit test accessor as a friend to the sequence arbiter and contained classes. 

- look at possible memory size reductions if they're available. 

- move the errorPolicy into sequence arbiter and expose it through a function. Also support external errorPolicy as a reference through the reference handle idiom. 

- do some testing around SequenceType rollovers.
    - does advance head work correctly on a rollover?
    - does head forward gap work correctly crossing a rollover boundary? 

### Think about 

- Change uses of SequenceType to expect a wrapper with an interface? 
    - e.g. how to handle rollover
    - handle cases where sequence numbers aren't incremented by 1.
