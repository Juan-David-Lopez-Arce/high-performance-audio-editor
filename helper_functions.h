#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <stdio.h>
#include "structs.h"

int count_digits(int index);
struct node* separate_parent(struct sound_seg* track, size_t pos, size_t len);
void destroy_node(struct node * node);
void split_node_into_2(struct node *node_1, int pos);
void split_node_into_3(struct node *node_1, int pos, int len);
struct node* node_init();
char* create_strings(int *pairs, size_t len);
struct node* get_previous_node(struct sound_seg *obj, struct node *node);
int16_t* samples_init(size_t size);
void put_child_in_parent_array(struct node* parent, struct node* child);
void remove_from_parent_array(struct node *target);
void split_children_into_2(struct node *parent_head);
void split_children_into_3(struct node *parent_head);
struct node* create_children(struct node *parent_head, size_t len);
struct node* get_last_element(struct node *cursor);

#endif