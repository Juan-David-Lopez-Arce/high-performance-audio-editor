#include "sound_seg.h"
#include "structs.h"
#include "helper_functions.h"


//Counts the number of digits for every index
int count_digits(int index) {
    if (index == 0) {
        return 1;  
    }
    int count = 0;
    while (index > 0) {
        index /= 10;  
        count++;    
    }
    return count;
}

//Format strings for tr_identify
char* create_strings(int *pairs, size_t len){
    //dynamically allocate memory for commas, newlines, null byte, and the necessary space for all digits
    char *str = (char*)malloc(100000*sizeof(char));
    if(str == NULL){
        printf("Malloc Failed\n");
    }

    if(len == 0){
        str[0] = '\0';
        return str;
    }
    //this array contains the number of digits of each index
    int *num_digits = (int*) malloc(len*sizeof(int));

    if(num_digits == NULL){
        printf("Malloc failed\n");
        return NULL;
    }

    int accum = 0;
    for(int i=0; i<len; i++){
        //get number of digits for every index so that we can track the string created
        num_digits[i] = count_digits(pairs[i]);
        accum += num_digits[i];
    }

    int pos_track = 0;
    for(int i=0; i<len; i++){
        sprintf(&str[pos_track], "%d", pairs[i]);
        pos_track += num_digits[i] * sizeof(char);
        if(i != len-1){
            if(i%2 == 0){
                sprintf(&str[pos_track], "%c", ',');
            }else{
                str[pos_track] = '\n';
            }
            pos_track++;
        }else{
            str[pos_track] = '\0';
        }
    }

    free(num_digits);
    return str;
}

// Initialize a new node in the linked list
struct node* node_init(){
    struct node *node = (struct node*) malloc(sizeof(struct node));
    if(node == NULL){
        printf("Malloc failed\n");
        return NULL;
    }
    node->samples = NULL;
    node->next = NULL;

    //first initialize with 1 element
    struct node **child = (struct node**) malloc(sizeof(struct node*));
    if(child == NULL){
        printf("Malloc failed\n");
        return NULL;
    }

    node->children = child;
    node->parent = NULL;
    node->size = 0;
    node->num_children = 0;
    node->is_child = false;
    return node;
}

//Initialize a new sample array
int16_t* samples_init(size_t size){
    int16_t *samples = (int16_t*) malloc(size*sizeof(int16_t));

    if(samples == NULL){
        printf("Malloc failed\n");
        return NULL;
    }

    return samples;
}

//removes a deleted child track from the children array of the parent;
void remove_from_parent_array(struct node *target){
    if(target->parent == NULL){
        return;
    }
    int array_size = target->parent->num_children;
    struct node **array = target->parent->children;
    int index = -1;
    // Find index
    for (int i = 0; i < array_size; i++) {
        if (array[i] == target) {
            index = i;
            break;
        }
    }
    if(index == -1){
        return;
    }
    // Shift elements left
    for (int i = index; i < array_size - 1; i++) {
        array[i] = array[i + 1];
    }
    target->parent->num_children--;
    if(target->parent->num_children > 0){
        struct node** tmp = realloc(array, (target->parent->num_children) * sizeof(struct node*));
        if (tmp != NULL) {
            target->parent->children = tmp;
        }
    }
}

//Free the memory of a node 
void destroy_node(struct node * node){
    if (node == NULL) {
        return;
    }
    //if the track is not a child, we free the memory. This is to avoid double-free on samples
    if(!node->is_child){
        free(node->samples);
    }else{
        remove_from_parent_array(node);
    }
    //set children pointers to null to avoid heap-use-after-free
    for(int i=0; i<node->num_children; i++){
        node->children[i]->parent = NULL;
        node->children[i] = NULL;
    }

    free(node->children);
    node->children = NULL;
    node->parent = NULL;
    node->samples = NULL;
    node->next = NULL;
    free(node);
    return;
}

//Returns the previous node
struct node* get_previous_node(struct sound_seg *obj, struct node *node) {
    struct node *cursor = obj->linked_list_head;

    if (cursor == NULL || cursor == node) {
        return NULL; 
    }

    while (cursor->next != NULL) {
        if (cursor->next == node) {
            return cursor;
        }
        cursor = cursor->next;
    }

    return NULL;
}

// Reallocates memory for the array of children and adds the child at the end
void put_child_in_parent_array(struct node* parent, struct node* child){
    if(parent->num_children == 1){
        parent->children[0] = child;
        return;
    }
    struct node** temp = realloc(parent->children, sizeof(struct node*) * parent->num_children);
    if (temp == NULL) {
        printf("Realloc failed\n");
        return;
    }

    parent->children = temp;
    parent->children[parent->num_children - 1] = child;
    return;
}

//splits the children of a parent node that was split
void split_children_into_2(struct node *parent_head){
    //split children ONLY if their size doesnt match yours
    for(int i=0; i<parent_head->num_children; i++){
        struct node *child_1 = parent_head->children[i];
        //if parent matches size with child, it means that the child was split beforehand
        if(parent_head->size == child_1->size){
            continue;
        }
        //perform logic to split the child node
        struct node *child_2 = node_init();
        //shares the same samples array as its parent
        child_2->samples = parent_head->next->samples;
        //has the same size as its parent
        child_2->size = parent_head->next->size;
        //child 1 shares samples with parent_head
        child_1->samples = parent_head->samples;
        //has the same size as its parent
        child_1->size = parent_head->size;

        child_2->num_children = child_1->num_children;
        child_2->is_child = child_1->is_child;

        //link the nodes
        struct node *temp = child_1->next;
        child_1->next = child_2;
        child_2->next = temp;

        //put the child in the other parent track created previously
        put_child_in_parent_array(parent_head->next, child_2);
        //now the parent of the newly created node will be the newly created parent node
        child_2->parent = parent_head->next;
    }
    return;
}

//splits children into 3 parts
void split_children_into_3(struct node *parent_head){
    for(int i=0; i<parent_head->num_children; i++){
        struct node *child_1 = parent_head->children[i];
        //if parent matches size with child, it means that the child was split beforehand
        if(parent_head->size == child_1->size){
            continue;
        }

        struct node *child_2 = node_init();
        struct node *child_3 = node_init();

        //copy the samples on the two new childs
        child_2->samples = parent_head->next->samples;
        child_2->size = parent_head->next->size;

        child_3->samples = parent_head->next->next->samples;
        child_3->size = parent_head->next->next->size;

        child_1->samples = parent_head->samples;
        child_1->size = parent_head->size;

        child_2->num_children = child_1->num_children;
        child_3->num_children = child_1->num_children;

        child_2->is_child = child_1->is_child;
        child_3->is_child = child_1->is_child;

        //now, link the nodes
        struct node *temp = child_1->next;
        child_1->next = child_2;
        child_2->next = child_3;
        child_3->next = temp;

        child_1->parent = parent_head;
        child_2->parent = parent_head->next;
        child_3->parent = child_2->parent->next;

        put_child_in_parent_array(child_1->parent, child_1);
        put_child_in_parent_array(child_2->parent, child_2);
        put_child_in_parent_array(child_3->parent, child_3);
    }
}

//splits a node into 3 parts
void split_node_into_3(struct node *node_1, int pos, int len){

    struct node *node_2 = node_init();
    struct node *node_3 = node_init();

    node_2->samples = samples_init((size_t) len);
    node_3->samples = samples_init((size_t) (node_1->size - (pos + len)));

    //copy the samples on the two new nodes
    int node_1_index = pos;
    for(int i=0; i<len; i++){
        node_2->samples[i] = node_1->samples[node_1_index++];
    }
    node_2->size = (size_t) len;

    for(int i=0; i<node_1->size - (pos + len); i++){
        node_3->samples[i] = node_1->samples[node_1_index++];
    }
    node_3->size = (size_t) (node_1->size - (pos + len));

    //Now we have to realloc the array (samples) of the original node
    node_1->samples = (int16_t*) realloc(node_1->samples, sizeof(int16_t) * pos);

    if(node_1->samples == NULL){
        printf("Realloc failed\n");
        return;
    }

    node_1->size = (size_t) pos;

    node_2->num_children = node_1->num_children;
    node_3->num_children = node_1->num_children;

    node_2->is_child = node_1->is_child;
    node_3->is_child = node_1->is_child;

    //now, link the nodes
    struct node *temp = node_1->next;
    node_1->next = node_2;
    node_2->next = node_3;
    node_3->next = temp;

    //If its a parent, split all children
    if(node_1->num_children != 0){
        split_children_into_3(node_1);
        return;
    }

    //If its a child, split the parent and let it split all of its children
    struct node *parent_1;
    struct node *parent_2;
    struct node *parent_3;
    struct node *temp_next;
    if(node_1->is_child){
        parent_2 = node_init();
        parent_3 = node_init();
        parent_1 = node_1->parent;
        temp_next = parent_1->next;
        parent_1->next = parent_2;
        parent_2->next = parent_3;
        parent_3->next = temp_next;

        parent_1->samples = node_1->samples;
        parent_2->samples = node_2->samples;
        parent_3->samples = node_3->samples;

        node_1->parent = parent_1;
        node_2->parent = parent_2;
        node_3->parent = parent_3;

        parent_1->size = node_1->size;
        parent_2->size = node_2->size;
        parent_3->size = node_3->size;

        parent_2->num_children = parent_1->num_children;
        parent_3->num_children = parent_1->num_children;

        put_child_in_parent_array(parent_2, node_2);
        put_child_in_parent_array(parent_3, node_3);
        split_children_into_3(parent_1);
        return;
    }
    return;
}

//splits a node into 2 parts
void split_node_into_2(struct node *node_1, int pos){
    struct node *node_2 = node_init();
    //copy the samples on the new node
    node_2->samples = samples_init((size_t) (node_1->size - pos));

    int node_1_index = pos;
    for(int i=0; i<node_1->size - pos; i++){
        node_2->samples[i] = node_1->samples[node_1_index++];
    }

    node_2->size = node_1->size - pos;

    //Now we have to realloc the array (samples) of the original node
    node_1->samples = (int16_t*) realloc(node_1->samples, sizeof(int16_t) * pos);
    
    if(node_1->samples == NULL){
        printf("Realloc failed\n");
        return;
    }

    node_1->size = (size_t) pos;

    node_2->num_children = node_1->num_children;
    node_2->is_child = node_1->is_child;

    //link the nodes
    struct node *temp = node_1->next;
    node_1->next = node_2;
    node_2->next = temp;

    //If its a parent, split all children
    if(node_1->num_children != 0){
        split_children_into_2(node_1);
        return;
    }

    //If its a child, split the parent and let it split all of its children
    struct node *tem_next;
    struct node *parent_next;
    if(node_1->is_child){
        parent_next = node_init();
        tem_next = node_1->parent->next;
        parent_next->next = tem_next;
        node_1->parent->samples = node_1->samples;
        node_1->parent->next = parent_next;
        parent_next->samples = node_2->samples;
        node_2->parent = parent_next;
        parent_next->size = node_2->size;
        node_1->parent->size = node_1->size;
        parent_next->num_children = node_1->parent->num_children;
        put_child_in_parent_array(node_2->parent, node_2);
        split_children_into_2(node_1->parent);
        return;
    }
    return;
}

// Separates the parent track from the sound segment
struct node* separate_parent(struct sound_seg* track, size_t pos, size_t len){
    struct node *cursor = track->linked_list_head;
    //head will hold the first node of new parent track
    struct node *head = NULL;

    //we first go to pos
    int length_tracker = 0;
    int start_index;
    while(true){
        if (length_tracker + cursor->size > pos){
            start_index = pos - length_tracker;
            break;
        }
        length_tracker += cursor->size;
        cursor = cursor->next;
    }
    //cursor is the node at which pos is located
    //start index is the index within the cursor at which the parent starts

    //then we separate the parent nodes from the rest of the track
    int samples_left = len;
    int position = start_index;
    int iterations = 0;
    while(samples_left > 0){
        if(cursor->size - position > samples_left){
            //if the track to delete in all in the middle of one node
            if(position > 0){
                //split the node into 3
                split_node_into_3(cursor, position, len);
                //return the head of the parent linked list

                //we dont check the number of iterations because we know that if position>0 this means
                //that we are just separating elements from one node
                head = cursor->next;
                return head;
            }else{
                split_node_into_2(cursor, samples_left);
                //return head of linked list
                if(iterations == 0){
                    return cursor;
                }
                //finish the loop because the range ends in this node
                break;
            }
        }else if(cursor->size - position <= samples_left){
            if(position > 0){
                split_node_into_2(cursor, position);
                //in this case, we know that this will always happen in the first iteration
                head = cursor->next;
                cursor = head;
                samples_left -= head->size;
            }else{
                samples_left -= cursor->size;
                if(iterations == 0){
                    head = cursor;
                }
            }
        }
        position = 0;
        cursor = cursor->next;
        iterations++;
    }
    return head;
}

//creates children who share the same samples array as their parent nodes
struct node* create_children(struct node *parent_head, size_t len){
    struct node *cursor = parent_head;
    struct node *prev_child = NULL;
    struct node *head_children = NULL;
    int accum_len = 0;
    while(accum_len != len){
        struct node *child = node_init();
        if(accum_len > 0){
            prev_child->next = child;
        }else{
            head_children = child;
        }
        child->size = cursor->size;
        child->samples = cursor->samples;
        //if the source is not chuld, then 
        if(!cursor->is_child){
            child->parent = cursor;
        }else{
            child->parent = cursor->parent;
        }
        child->parent->num_children++;
        child->is_child = true;
        //put the child reference in the parent children array
        put_child_in_parent_array(child->parent, child);
        prev_child = child;
        accum_len += cursor->size;
        cursor = cursor->next;
    }

    return head_children;
}

//returns the last node of a linked list
struct node* get_last_element(struct node *cursor){
    while(cursor->next != NULL){
        cursor = cursor->next;
    }
    return cursor;
}