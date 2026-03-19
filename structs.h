#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

struct sound_seg {
    //first node which will contain multiple samples
    struct node *linked_list_head;
};

struct node{
    size_t size;
    //next node in linked list
    struct node *next;
    //array of int16_t samples
    int16_t *samples;
    //array of children nodes (not null if the node is a parent)
    struct node **children;
    //parent node (not null if the node is a child)
    struct node *parent;
    //keep track of the number of children
    int num_children;
    //true if it is a child
    bool is_child;
};

#endif