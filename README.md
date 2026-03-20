## High-Performance Audio Editor

This project implements the backend of an audio editor in C, designed to efficiently manipulate large audio tracks through memory-safe and low-overhead operations. 

At the core of the system is a custom data structure that allows multiple tracks to share references to the same underlying audio segments. Instead of duplicating audio data, segments are reused through pointer-based referencing, significantly reducing memory usage and improving performance when editing large files.

## Why This Project Matters

Audio processing systems often deal with large datasets where naive copying leads to significant performance and memory issues.

This project demonstrates how careful memory management and data structure design in C can:
- Reduce memory overhead
- Avoid unnecessary data duplication
- Enable scalable editing operations

## Features

- Efficient manipulation of audio samples using custom memory-managed structures
- Support for dynamic track operations (insert, delete, split, merge)
- Zero-copy segment reuse to minimize memory usage
- Modular backend design for integration with audio processing systems

## Architecture

- Audio data is stored as segments in a linked structure
- Tracks reference segments instead of owning copies
- Editing operations update references rather than duplicating data
- Heap allocation is carefully managed to minimize fragmentation

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
