#include "sound_seg.h"
#include "structs.h"
#include "helper_functions.h"

// Load a WAV file into buffer
void wav_load(const char* filename, int16_t* dest){

    FILE *f = fopen(filename, "rb");
    if(!f){
        return;
    }

    char header[HEADER_BYTES -4]; 

    int length_bytes;
    fread(header, 1, HEADER_BYTES - 4, f);
    //discard first 40 bytes of sthe header
    fread(&length_bytes, 4, 1, f);
    //read length_bytes bytes into the buffer
    for(int i=0; i<length_bytes/sizeof(int16_t); i++){
        fread(dest + i, sizeof(int16_t), 1, f);
    }

    fclose(f);
    return; 
}

// Create/write a WAV file from buffer
void wav_save(const char* fname, const int16_t* src, size_t len){
    FILE *f = fopen(fname, "wb");

    const int subchunk_2_size = len * sizeof(int16_t);
    const int chunk_size = 36 + subchunk_2_size;
    const int subchunk_1_size = 16;
    const short audio_format = 1;
    const short num_channels = 1;
    const int sample_rate = 8000;
    const short bits_per_sample = 16;
    const short block_align = num_channels * bits_per_sample / 8;
    const int byte_rate = sample_rate * block_align;

    fwrite("RIFF", 4, 1, f);
    fwrite(&chunk_size, 4, 1, f);
    fwrite("WAVE", 4, 1, f);
    fwrite("fmt ", 4, 1, f);
    fwrite(&subchunk_1_size, 4, 1, f);
    fwrite(&audio_format, 2, 1, f);
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);
    fwrite(&byte_rate, 4, 1, f);
    fwrite(&block_align, 2, 1, f);       
    fwrite(&bits_per_sample, 2, 1, f);   
    fwrite("data", 4, 1, f);
    fwrite(&subchunk_2_size, 4, 1, f);

    fwrite(src, sizeof(int16_t), len, f); 

    fclose(f);
}

// Initialize a new sound_seg object
struct sound_seg* tr_init() {
    struct sound_seg *track =  (struct sound_seg*) malloc(sizeof(struct sound_seg));
    if (track == NULL){
        printf("Malloc failed\n");
        return NULL;
    }

    //new initialization
    track->linked_list_head = node_init();
    return track;
}
 
// Destroy a sound_seg object and free all allocated memory
void tr_destroy(struct sound_seg* obj) {
    if(obj->linked_list_head != NULL){
        struct node *head = obj->linked_list_head;
        struct node *cursor = head->next;
        //free memory allocated for the nodes
        if(cursor != NULL){
            do{
                destroy_node(head);
                head = cursor;
                if(cursor != NULL){
                    cursor = cursor->next;
                }
            }while(head != NULL);
        }else{
            destroy_node(head);
        }
        head = NULL;
        cursor = NULL;
    }
    //free allocated memory for the sound_seg struct
    free(obj);
    obj = NULL;

    return;
}

// Return the length of the segment
size_t tr_length(struct sound_seg* seg) {
    struct node *cursor = seg->linked_list_head;
    size_t counter = 0;

    while(cursor != NULL){
        counter += cursor->size;
        cursor = cursor->next;
    }

    cursor = NULL;
    return counter;
}

// Read len elements from position pos into dest
void tr_read(struct sound_seg* track, int16_t* dest, size_t pos, size_t len) {
    //get to the position:pos within the linked list
    struct node *cursor = track->linked_list_head;

    int actual_pos = 0;
    int buff_counter = 0;
    bool end_loop = false;
    while(cursor != NULL && !end_loop){
        for(int i=0; i<cursor->size; i++){
            if(actual_pos == pos + len){
                end_loop = true;
                break;
            }
            if(actual_pos >= pos){
                dest[buff_counter++] = cursor->samples[i];
            }
            actual_pos++;
        }
        cursor = cursor->next;
    }
    cursor = NULL;
    
    return;
}

// Write len elements from src into position pos
void tr_write(struct sound_seg* track, int16_t* src, size_t pos, size_t len) {
    //get to the position:pos within the linked list
    struct node *cursor = track->linked_list_head;
    struct node* new_node;

    //case 1: The samples array of the head is null, meaning that elements can only be inserted from index 0
    if(cursor->samples == NULL){
        cursor->samples = samples_init(len);
        cursor->size = len;
        for(int i=0; i<len; i++){
            cursor->samples[i] = src[i];
        }
        return;
    }

    //case 2: Pos == length of the entire track
    if(pos == tr_length(track)){
        while(cursor->next != NULL){
            cursor = cursor->next;
        }
        if(cursor->size == 0){
            
        }
        new_node = node_init();
        new_node->samples = samples_init(len);
        new_node->size = len;
        cursor->next = new_node;
        for(int i=0; i<len; i++){
            new_node->samples[i] = src[i];
        }
        return;
    }

    //case 3: There are already some samples in the track and we want to override and even add samples to the end of the track
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

    //start_index contains the index within the node at which pos is located
    //cursor contains the node at which pos is located
    int samples_changed = 0;
    int index = start_index;
    bool break_loop = false;
    //loop through the nodes
    while(true){
        //loop trough the values inside the nodes
        while(index < cursor->size){
            cursor->samples[index++] = src[samples_changed++];
            if(samples_changed == len){
                break_loop = true;
                break;
            }
        }
        if(break_loop){
            break;
        }
        if(cursor->next == NULL){
            break;
        }
        cursor = cursor->next;
        index = 0;
    }

    //Create a new node if the number of samples to be written exceeds the length of the original track
    if(samples_changed != len){
        new_node = node_init();
        new_node->samples = samples_init(len - samples_changed);
        new_node->size = len - samples_changed;
        cursor->next = new_node;

        int i = 0;
        while(samples_changed < len){
            new_node->samples[i++] = src[samples_changed++];
        }
    }
    return;
}

// Delete a range of elements from the track
bool tr_delete_range(struct sound_seg* track, size_t pos, size_t len) {
    struct node *cursor = track->linked_list_head;
    struct node *starting_node;
    //check if any of the samples within range is a parent
    //we first go to pos and then traverse the nodes from pos to pos+len and check if any sample is a parent
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
    //we already know the node at which pos is located. Therefore, we save it
    starting_node = cursor;
    //here we are traversing the nodes from pos to pos+len to check if any of these is a parent
    int count_samples = cursor->size - start_index;
    while(true){
        //check if it is a parent
        if(cursor->num_children > 0){
            return false;
        }
        if(count_samples >= len){
            break;
        }
        cursor = cursor->next;
        count_samples += cursor->size;
    }

    //if none of the samples are parents (or a children), then delete the range
    int samples_left = len;
    int position = start_index;
    struct node* prev;
    struct node* temp;
    int prev_size = 0;
    //we put the starting node (where pos is located) back into cursor
    cursor = starting_node;
    while(samples_left > 0){
        if(cursor->size - position > samples_left){
            //if the track to delete in all in the middle of one node
            if(position > 0){
                //split the node into 3 and delete the middle node
                split_node_into_3(cursor, position, len);
                temp = cursor->next->next;
                destroy_node(cursor->next);
                cursor->next = temp;
            }else{
                split_node_into_2(cursor, samples_left);
                prev = get_previous_node(track, cursor);
                if(prev != NULL){
                    temp = prev->next;
                    prev->next = temp->next;
                    //printf("DESTROYED_NODE_IS_CHILD: %d\n", temp->is_child);
                    //printf("NEXT_NODE_IS_CHILD: %d\n", prev->next->is_child);
                    //fflush(stdout);
                    destroy_node(temp);
                }else{
                    //this means that cursor is the head of the linked list
                    track->linked_list_head = cursor->next;
                    destroy_node(cursor);
                }
            }
            break;

        }else if(cursor->size - position <= samples_left){
            //store the size of the cursor before its split
            prev_size = cursor->size;
            if(position > 0){
                split_node_into_2(cursor, position);
                temp = cursor->next->next;
                destroy_node(cursor->next);
                cursor->next = temp;
            }else{
                prev = get_previous_node(track, cursor);
                if(prev != NULL){
                    temp = prev->next;
                    prev->next = temp->next;
                    cursor = prev->next;
                    destroy_node(temp);
                    if(prev_size - position == samples_left){
                        break;
                    }else{
                        samples_left -= (prev_size - position);
                        position = 0;
                        continue;
                    }
                }else{
                    track->linked_list_head = cursor->next;
                    destroy_node(cursor);
                    cursor = track->linked_list_head;
                    if(prev_size - position == samples_left){
                        break;
                    }else{
                        samples_left -= (prev_size - position);
                        position = 0;
                        continue;
                    }
                }
            }
            samples_left -= (prev_size - position);
            position = 0;
            cursor = cursor->next;
        }
    }
    return true;
}

// Returns a string containing <start>,<end> ad pairs in target
char* tr_identify(struct sound_seg* target, struct sound_seg* ad){
    double auto_corr_ad = 0;
    struct node *ad_node = ad->linked_list_head;

    //compute the auto correlation for the ad
    int16_t value;
    while(ad_node != NULL){
        for(int i=0; i<ad_node->size; i++){
            value = ad_node->samples[i];
            auto_corr_ad += value*value;
        }
        ad_node = ad_node->next;
    }

    int *index_array = (int*) malloc(sizeof(int));
    if(index_array == NULL){
        printf("Malloc failed\n");
        return NULL;
    }

    size_t ad_length = tr_length(ad);
    int16_t *buffer_target = (int16_t*) malloc(sizeof(int16_t) * ad_length);
    if(buffer_target == NULL){
        printf("Malloc failed\n");
        return NULL;
    }
    
    int16_t *buffer_ad = (int16_t*) malloc(sizeof(int16_t) * ad_length);
    if(buffer_ad == NULL){
        printf("Malloc failed\n");
        return NULL;
    }
    tr_read(ad, buffer_ad, 0, ad_length);

    double corr = 0;
    int index = 0;
    int count_indexes = 0;
    while(index + ad_length <= tr_length(target)){
        tr_read(target, buffer_target, index, ad_length);
        corr = 0;
        for(int i=0; i<ad_length; i++){
            corr += buffer_target[i] * buffer_ad[i];
        }

        if (corr >= 0.95*auto_corr_ad){
            index_array = (int*) realloc(index_array, sizeof(int)*(count_indexes+2));
            if(index_array == NULL){
                printf("Realloc failed\n");
                return NULL;
            }
            index_array[count_indexes] = index;
            count_indexes++;
            index_array[count_indexes] = index + ad_length -1;
            count_indexes++;
        }
        index++;
    }

    for(int i=0; i<count_indexes; i++){
        printf("%d ", index_array[i]);
    }

    free(buffer_target);
    free(buffer_ad);
    buffer_ad = NULL;
    buffer_target = NULL;

    //Get the formatted strings from a helper function
    char *strs = create_strings(index_array, count_indexes);
    //free the malloc
    free(index_array);
    return strs;
}

// Insert a portion of src_track into dest_track at position destpos
void tr_insert(struct sound_seg* src_track,
    struct sound_seg* dest_track,
    size_t destpos, size_t srcpos, size_t len){
    
    //first we separate the parent from its track
    struct node *parent_head = separate_parent(src_track, srcpos, len);
    //then we create children for every parent track created
    struct node *child_head = create_children(parent_head, len);
    //then we insert the children in the destination track
    struct node *dest_cursor = dest_track->linked_list_head;
    
    //Case 1: destpos == dest_track->size
    size_t dest_size = tr_length(dest_track);
    if(destpos == dest_size && dest_size != 0){
        dest_cursor = get_last_element(dest_cursor);
        dest_cursor->next = child_head;
        return;
    }
    
    //extra case: Inserting right after initializing a new track
    if(dest_size == 0){
        destroy_node(dest_track->linked_list_head);
        dest_track->linked_list_head = child_head;
        return;
    }
        
    //Case 2: destpos == 0
    struct node *temp = dest_track->linked_list_head;
    struct node *child_cursor = child_head;
    if(destpos == 0){
        dest_track->linked_list_head = child_head;
        child_cursor = get_last_element(child_cursor);
        child_cursor->next = temp;
        return;
    }
    
    //Case 3: destpos is in the middle of some other track
    //To do this, we first go to destpos
    struct node *cursor = dest_track->linked_list_head;
    int length_tracker = 0;
    int start_index = 0;
    while(true){
        if(length_tracker + cursor->size > destpos){
            start_index = destpos - length_tracker;
            break;
        }
        length_tracker += cursor->size;
        cursor = cursor->next;
    }
    
    //Case 3.1: The start_pos == 0, in which case we just insert before
    struct node *prev_node = NULL;
    struct node *next = cursor;
    if(start_index == 0){
        prev_node = get_previous_node(dest_track, cursor);
        prev_node->next = child_head;
        child_cursor = get_last_element(child_cursor);
        child_cursor->next = next;
        return;
    }
    
    //Case 3.2: The index within the node is nonzero. Then we split it and insert the linked list in the middle
    split_node_into_2(cursor, start_index);
    temp = cursor->next;
    cursor->next = child_head;
    cursor = get_last_element(cursor);
    cursor->next = temp;
   
    return;
}