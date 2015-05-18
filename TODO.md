# TODO

- AdvanceHead needs to handle slow-line overrun
- InitialState needs to handle first sequence # gaps
- GapFill needs to handle a gap fill into a range where we had an unrecoverable gap because the gap size was larger than we're configured to handle. 

### Think about 

- Change uses of SequenceType to expect a wrapper with an interface? 
    - e.g. how to handle rollover
    - handle cases where sequence numbers aren't incremented by 1.
