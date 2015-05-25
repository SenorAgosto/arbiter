# TODO

- head forward gap fill needs to handle too large of a gap? 
- do some testing around SequenceType rollovers.
    - does advance head work correctly on a rollover?
    - does head forward gap work correctly crossing a rollover boundary? 

### Think about 

- Change uses of SequenceType to expect a wrapper with an interface? 
    - e.g. how to handle rollover
    - handle cases where sequence numbers aren't incremented by 1.
