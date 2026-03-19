#ifndef SOUND_SEG_H
#define SOUND_SEG_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "structs.h"

#define BYTES_PER_SAMPLE 2
#define HEADER_BYTES 44
#define SUBCHUNK_1_SIZE 16
#define SAMPLE_RATE 8000
#define BLOCK_ALLIGN  2
#define BYTE_RATE  16000
#define BITS_PER_SAMPLE 16

void tr_destroy();
struct sound_seg* tr_init();
void wav_save(const char* fname, const int16_t* src, size_t len);
void wav_load(const char* filename, int16_t* dest);
size_t tr_length(struct sound_seg* seg);
void tr_read(struct sound_seg* track, int16_t* dest, size_t pos, size_t len);
void tr_write(struct sound_seg* track, int16_t* src, size_t pos, size_t len);
bool tr_delete_range(struct sound_seg* track, size_t pos, size_t len);
char* tr_identify(struct sound_seg* target, struct sound_seg* ad);
void tr_insert(struct sound_seg* src_track,
    struct sound_seg* dest_track,
    size_t destpos, size_t srcpos, size_t len);

#endif