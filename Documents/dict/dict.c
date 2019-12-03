#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "dict.h"

struct dict_item {
  // Each word is at most 100 bytes.
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
  printf(my_dict->path);
  printf("test");
  my_dict->num_items = num_items;
  my_dict->fd = dictionary_open_map(my_dict);
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
  int fd = open(dict->path, dict->num_items);
  dict->fd = fd;
  size_t len = dictionary_len(dict);
  if (ftruncate(fd, len) != 0) {
    perror("Unable to truncate file");
    return EXIT_FAILURE;
  }
  char *region = mmap(NULL, len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
  printf(region);
  return fd;
}

// Read the file at input. For each line in input, create a new dictionary
// entry.
int dictionary_generate(struct dict_t *dict, char *input) {
  // FILE *fp;
  // char *line = NULL;
  // size_t len = 0;
  // ssize_t read;
  // fp = fopen(input, "r");
  // if (fp == NULL) {
  //   perror("Invalid file");
  //   return EXIT_FAILURE;
  // }
  // while ((read = getline(&line, &len, fp)) != -1) {
    
  // }
  // return EXIT_SUCCESS;
}

// load a dictionary that was generated by calling generate.
int dictionary_load(struct dict_t *dict) {
  return 0;
}

// Unmaps the given dictionary.
// Free/destroy the underlying dict. Does NOT delete the database file.
void dictionary_close(struct dict_t *dict) {
  munmap(dict->base, dictionary_len(dict));
  //dict.Dispose();
}

// returns pointer to word if it exists, null otherwise
char* dictionary_exists(struct dict_t *dict, char *word) {
  return "";
}

//// Count of words with len > n
int dictionary_larger_than(struct dict_t *dict, size_t n) {
  return 0;
}

// Count of words with len < n
int dictionary_smaller_than(struct dict_t *dict, size_t n) {
  return 0;
}

// Count of words with len == n
int dictionary_equal_to(struct dict_t *dict, size_t n) {
  return 0;
}
