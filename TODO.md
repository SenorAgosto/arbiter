# TODO

- head forward gap fill needs to handle slow line overrun better. Currently, I'm checking for an overrun each time through the loop, should probably calculate the overrun once for each other line and make the necessary adjustments. 

- do some testing around SequenceType rollovers.
    - does advance head work correctly on a rollover?
    - does head forward gap work correctly crossing a rollover boundary? 

### Think about 

- Change uses of SequenceType to expect a wrapper with an interface? 
    - e.g. how to handle rollover
    - handle cases where sequence numbers aren't incremented by 1.
