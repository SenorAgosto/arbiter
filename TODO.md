# TODO

- GapFill needs to handle a gap fill into a range where we had an unrecoverable gap because the gap size was larger than we're configured to handle. 
- Test gaps on first sequence # after reset().
- Test receiving a sequence number that is less than the first expected sequence number for the first sequence number received; ensure we reject it.

### Think about 

- Change uses of SequenceType to expect a wrapper with an interface? 
    - e.g. how to handle rollover
    - handle cases where sequence numbers aren't incremented by 1.
