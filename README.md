# Arbiter

A collection of message arbiters used for maintaining idempotency in distributed applications. 

Currently this library supplies a Sequence Arbiter. 

### Sequence Arbiter 

A sequence arbiter is an arbiter that looks at one or more streams of sequential message identifiers (sequence numbers) and determines whether a message with the identifier should be accept for processing or rejected. Sequence numbers will be accepted the first time they are seen, and rejected when they are duplicates to previously seen values.

Our sequence arbiter detects and reports the following error conditions on sequence streams:

- First sequence number out-of-sequence
- Duplicate on line
- Gap
- GapFill 
- Line position overrun
- Unrecoverable gaps
- Unrecoverable line gap

#### First sequence number out-of-sequence 

The first sequence passed to the arbiter was less than the expected first sequence number. This can happen for example, if the first sequence number is supposed to be 1 but a 0 is received. 

#### Duplicate on line

A common use case for arbitration is for an application to send duplicate content on two streams for the purposes of redundancy. If we call our lines A and B, it would be normal to see the same sequence number arrive from line A and then later from line B. It would be abnormal to see the same sequence arrive on line A twice. 

This error indicates we received the same sequence number multiple times on the _same_ line. Reporting this error condition is somewhat unique for our arbiter implementation.

#### Gap 

There was a gap in the expected sequence of identifiers. E.g, 0, 1, 2, 5. Has a gap starting at 3 of length 2, 3 and 4 are missing.

#### Gap Fill 

This informs the user code a gap was filled, a common occurrence when processing sequence numbers out-of-sequence.

#### Line position overrun 

Called when arbitrating multiple lines, and the lead line overruns a slow line in the arbiter's cache history. A slow line can indicate connection problems, other network related errors, etc.

#### Unrecoverable gaps 

When the lead line overwrites a value in the arbiter's history (used as a circular buffer), and the history value holds a gap, we report this as unrecoverable. Meaning there was a gap which has been removed from the arbiter's history. If the gap is filled with an out of order sequence number arriving at a later point in time, it will be rejected by the arbiter. 

#### Unrecoverable line gap 

When the lead line overwrites a value in the arbiter's history and the history value has a line gap - which is not a true gap, but a situation where one or more lines never reported receiving the sequence number - an unrecoverable line gap is reported. Having line gaps informs us whether or not we have a quality issue on the line, if a line starts having a lot of line gaps it can indicate a line quality or other networking issue.

#### Reset

We provide a reset method for applications needing to reset the sequence stream during the course of normal operation.

#### Thread safety

We implement no synchronization inside the arbiter, it is therefore not thread-safe. 

### Dependencies 

- c++11 
- Cmake 2.8+

Used for unit testing on all platforms: 

- [UnitTest++](https://github.com/unittest-cpp/unittest-cpp). Unit test framework.

### Paper 

Read the paper about the [Duplicates on a Line Error](http://paxos1977.github.io/arbiter/)

### Contributors 

Austin Gilbert <ceretullis@gmail.com>

### License

4-Clause BSD license, see [LICENSE.md](LICENSE.md) for details. Other licensing available upon request. 
