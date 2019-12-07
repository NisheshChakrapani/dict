#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "dict.h"

struct dict_item {
  // Each word is at most 100 bytes.`
  char word[100];
  // The length of each word.
  size_t len;
};

struct dict_t {
  // The path to the underlying file
  char *path;
  // The file descriptor of the opened file. Set by dictionary_open_map.
  int fd;
  // How many items the mmaped file should store (this will determine the size).
  // There are ~500k words in /usr/share/dict/words.
  size_t num_items;
  // A pointer to the first item.
  struct dict_item *base;
};

// Construct a new dict_t struct.
// data_file is where to write the data,
// num_items is how many items this data file should store.
struct dict_t* dictionary_new(char *data_file, size_t num_items) {
  struct dict_t *my_dict;
  my_dict = malloc(sizeof(my_dict));
  my_dict->path = data_file;
  my_dict->num_items = num_items;
  return my_dict;
}

// Computes the size of the underlying file based on the # of items and the size
// of a dict_item.
size_t dictionary_len(struct dict_t *dict) {
  return dict->num_items * sizeof(struct dict_item);
}

// This is a private helper function. It should:
// Open the underlying path (dict->path), ftruncate it to the appropriate length
// (dictionary_len), then mmap it.
int dictionary_open_map(struct dict_t *dict) {
  int fd = open(dict->path, O_CREAT | O_RDWR | O_TRUNC | S_IRUSR);
  if (fd == -1) {
    perror("Could not open file");
    return 0;
  }
  size_t len = dictionary_len(dict);
  if (ftruncate(fd, len) == -1) {
    perror("Could not truncate file");
    return 0;
  }
  void *base = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (base == MAP_FAILED) {
    perror("Could not map file");
    return 0;
  }
  dict->base = base;
  dict->fd = fd;
  return 1;
}

// Read the file at input. For each line in input, create a new dictionary
// entry.
int dictionary_generate(struct dict_t *dict, char *input) {
  FILE *file;
  char *line = NULL;
  size_t len = 0;
  char buffer[100];
  file = fopen(input, "r");
  if (file == NULL) {
    perror("Invalid file");
    return 0;
  }
  if (dictionary_open_map(dict) == 0) {
    return 0;
  }
  int i = 0;
  while (fgets(buffer, 100, file)) {
    char *temp;
    temp = strchr(buffer, '\n');
    if (temp != NULL) {
      *temp = 0;
    }
    dict->base[i].len = (size_t)strlen(read);
    strncpy(dict->base[i].word, read, strlen(read));
    i++;
  }
  msync(dict->base, dictionary_len(dict), MS_SYNC);
  return 1;
}

// load a dictionary that was generated by calling generate.
int dictionary_load(struct dict_t *dict) {
  int result = dictionary_open_map(dict);
  msync(dict->base, dictionary_len(dict), MS_SYNC);
  return result;
}

// Unmaps the given dictionary.
// Free/destroy the underlying dict. Does NOT delete the database file.
void dictionary_close(struct dict_t *dict) {
  munmap(dict, dict->num_items);
}

// returns pointer to word if it exists, null otherwise
char* dictionary_exists(struct dict_t *dict, char *word) {
  for (int i = 0; i < dict->num_items; i++) {
    if (strcmp(dict->base[i].word, word)) {
      char* wordPointer = dict->base[i].word;
    }
  }
  return NULL;
}

//// Count of words with len > n
int dictionary_larger_than(struct dict_t *dict, size_t n) {
  int count = 0;
  for (int i = 0; i < dict->num_items; i++) {
    if (dict->base[i].len > 0 && dict->base[i].len > n) {
      count++;
    }
  }
  return count;
}

// Count of words with len < n
int dictionary_smaller_than(struct dict_t *dict, size_t n) {
  int count = 0;
  for (int i = 0; i < dict->num_items; i++) {
    if (dict->base[i].len > 0 && dict->base[i].len < n) {
      count++;
    }
  }
  return count;
}

// Count of words with len == n
int dictionary_equal_to(struct dict_t *dict, size_t n) {
  int count = 0;
  for (int i = 0; i < dict->num_items; i++) {
    if (dict->base[i].len > 0 && dict->base[i].len == n) {
      count++;
    }
  }
  return count;
}
