# TODO

- AdvanceHead needs to handle slow-line overrun
- InitialState needs to handle first sequence # gaps

### Think about 

- Change uses of SequenceType to expect a wrapper with an interface? 
    - e.g. how to handle rollover
    - handle cases where sequence numbers aren't incremented by 1.
