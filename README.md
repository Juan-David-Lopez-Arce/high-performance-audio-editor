## High-Performance Audio Editor

This project implements the backend of a high-performance audio editor in C, focusing on efficient memory management and scalable manipulation of audio data. The system supports core track operations such as insertions, deletions, and segment edits while maintaining performance and minimizing memory overhead.

At the core of the design is a custom data structure built using linked lists and low-level heap allocation. This structure enables reuse of audio segments across tracks without duplication, significantly reducing memory usage and improving efficiency when working with large audio files.

Key features include:

- Efficient handling of audio samples through custom memory-managed data structures.
- Support for dynamic track operations (insert, delete, split, merge).
- Reuse of audio segments to avoid redundant data storage.
- Modular design enabling integration with higher-level audio processing tools.

This project emphasizes systems-level programming, memory optimization, and data structure design, showcasing how low-level C can be used to build performant and scalable applications.

### Getting Started

1. **Clone the repository**

   git clone https://github.com/Juan-David-Lopez-Arce/high-performance-audio-editor.git
   cd high-performance-audio-editor
   

2. **Compile**

   make sound_seg.o
