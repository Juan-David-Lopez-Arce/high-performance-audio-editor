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
   make
   
### 3. Run
   ./audio_editor

## Key Learnings

- Advanced memory management in C
- Designing zero-copy data structures
- Trade-offs between performance and complexity
- Efficient handling of large datasets

## Challenges

- Designing a data structure that avoids copying large audio segments
- Managing memory safely in C without leaks or fragmentation
- Ensuring correctness when multiple tracks share segment references
