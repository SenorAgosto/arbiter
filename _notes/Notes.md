# Implementation notes

- drop messages to far back (no longer in history), report to error policy 
- since I'll need to know the location in the cache for each line, store the actual index not an ever increasing value.
- head is the tail of history 
- save the line that is current head 

Message moves us forward:

Head moves forward one spot at a time.
(1) If the SequenceInfo holds a gap, report the gap as unrecoverable
(2) If not all lines reported, report the missing sequence number as an unrecoverable line gap; this indicates a line quality issue.
(3) overwrite the SequenceInfo with a new instance with the current sequence number & report the current line

Head moves forward overwriting the existing contents with the sequence numbers that _should_ be there.
There for accounting for gaps. 

Message is behind current:

(1) ensure we aren't trying to cross the tail boundary (where head is in the cache), if we are discard. 
(2) do not update the position of the line
(3) jump back to location & verify the sequence numbers match, if not discard. 
(4) see if the LineSet was complete, if so report a duplicate on line, if not add the line & discard. 
(5) if LineSet was empty, add the line and accept 

-------------

- determine if current line is head
- lookup the sequence number of SequenceInfo in cache at current line position.
- compare incoming sequence number with the current line position sequence number to determine if:
    - this is the correct next sequence number (incremented by 1)
    - a gap increment (a sequence # ahead of the next expected sequence number)
    - a gap fill (a sequence number behind the current sequence number)

States:
    - head advancement 
        - correct next sequence # -> assign cache new SequenceInfo object with sequence number & line
        - gap increment 
    - non-head advancement 
        - correct next sequence # -> just insert the line 
        - gap increment 
    - gap fill (head & non-head gap fill logic is the same)

    