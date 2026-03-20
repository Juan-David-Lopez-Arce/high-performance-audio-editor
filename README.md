## High-Performance Audio Editor

A high-performance audio editor backend in C focused on memory efficiency, enabling zero-copy manipulation of large audio tracks through shared segment references.

At the core of the system is a custom data structure that allows multiple tracks to share references to the same underlying audio segments. Instead of duplicating audio data, segments are reused through pointer-based referencing, significantly reducing memory usage and improving performance when editing large files.

## Why This Project Matters

Naive audio editing systems duplicate data on every modification, leading to significant memory overhead and poor scalability.

This project explores how low-level systems design eliminates unnecessary copying by using shared segment references, enabling efficient manipulation of large audio datasets.

This project demonstrates how careful memory management and data structure design in C can:
- Reduce memory overhead
- Avoid unnecessary data duplication
- Enable scalable editing operations

## Features

- Efficient manipulation of audio samples
- Support for dynamic track operations (insert, delete, split, merge)
- Modular backend design for integration with audio processing systems

## Design Decisions

- **Zero-copy architecture:** Instead of duplicating audio data, tracks share references to segments, significantly reducing memory usage for large files.
- **Linked segment structure:** Chosen over contiguous arrays to support efficient insertions, deletions, and splits without shifting large blocks of memory, reducing the time complexity of such operations.
- **Memory vs complexity trade-off:** Prioritised memory efficiency and scalability at the cost of increased structural complexity.
- **Manual memory management:** Carefully handled heap allocation and deallocation to avoid leaks and fragmentation in a shared-reference system.

## Architecture

- Audio is represented as a linked sequence of segments, each pointing to a shared buffer
- Tracks maintain references to segments rather than owning raw data
- Editing operations (insert, split, merge) modify segment links instead of copying memory
- Reference sharing enables multiple tracks to reuse the same audio data safely

## Getting Started

### 1. Clone the repository
- git clone https://github.com/Juan-David-Lopez-Arce/high-performance-audio-editor.git
- cd high-performance-audio-editor

### 2. Compile
- make
   
### 3. Run

- This project is implemented as a backend library and does not include a standalone executable.
- It provides a set of functions for manipulating shared audio segments (e.g., insert, delete, split, merge). To test or use the system, you can create your own `main.c` file and call these functions to interact with the data structure.
- This design reflects the modular nature of the project, allowing it to be integrated into larger audio processing applications or extended with a custom frontend.

#### Example

```c
int main() {
    // initialize track
    // call insert/delete functions
    // print or verify results
}
```

## Key Learnings

- Advanced memory management in C
- Designing zero-copy data structures
- Trade-offs between performance and complexity
- Efficient handling of large datasets

## Challenges

- Designing a data structure that avoids copying large audio segments
- Managing memory safely in C without leaks or fragmentation
- Ensuring correctness when multiple tracks share segment references
