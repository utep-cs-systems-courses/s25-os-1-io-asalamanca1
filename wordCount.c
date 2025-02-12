#include <fcntl.h>      // For file control operations
#include <unistd.h>     // For system calls like open(), read(), write(), and close()
#include <sys/stat.h>   // For getting file size
#include <sys/types.h>  // For standard system data types
#include <stdlib.h>     // For memory allocation
#include <stdio.h>
#include <ctype.h>      // For tolower()


//----------------- String Functions -------------------

// searches for a character in a string
char *str_chr(const char *s, int c) {
    // traverse through each char in s while char isnt null and char isnt equal to c
    while (*s != '\0' && *s != (char)c)
        ++s;
    // if s == c return address of s, if not then return null pointer
    return (*s == (char)c) ? (char *)s : NULL;
}

// returns pointer to first occurrence of any character in charset
char *str_p_brk(const char *s, const char *charset) {
    // for each char in s, return if char was found in charset
    while (*s != '\0') {
        // if curr char in s was found in charset, return address of curr char(being traversed) in s
        if (str_chr(charset, *s))
            return (char *)s;
        ++s;
    }
    //return null pointer if no chars in s were in charset
    return NULL;
}

// splits a string into tokens based on delimiters 
char *str_sep(char **stringp, const char *delim) {
    if (*stringp == NULL)
        return NULL;
    // store the location of the beginning of string
    char *token_start = *stringp;
    // store location of the end of string (where delim was found)
    char *token_end = str_p_brk(*stringp, delim);
    // if no delims were found in stringp, return location of the beginning of string
    if (token_end == NULL) {
        *stringp = NULL;
        return token_start;
    }
    // convert delim to a null char
    *token_end = '\0';
    // change location to beginning of string to after the null char where delim was found
    *stringp = token_end + 1;
    // return modified string (everything after the found delim)
    return token_start;
}

// gets the number of tokens in a string using the given delimiters.
static size_t count_tokens(const char *str, const char *delims) {
    // store count of number of delims in str
    size_t count = 0;
    // keep track of whether a delim was found in str or not
    int delim_not_in_token = 0;
    // traverse through each char in str
    while (*str != '\0') {
        // if curr char being traversed is in delims, set delim_not_in_token to false
        if (str_chr(delims, *str)) {
            delim_not_in_token = 0;
        } 
        // if curr char being traversed is not in delims, set delim_not_in_token to true
        else if (!delim_not_in_token) {
            delim_not_in_token = 1;
            // add 1 to token count
            count++;
        }
        str++;
    }
    // return number of tokens
    return count;
}

// use str_sep() to split a string into tokens and returns an array of token pointers
char **tokenize(char *str, const char *delims) {
    // get the number of tokens in str, split by delims
    size_t num_tokens = count_tokens(str, delims);
    // allocate memory for num_tokens char pointers
    char **tokens = malloc((num_tokens + 1) * sizeof(char *)); // +1 for null terminator
    // return null if malloc failed
    if (!tokens)
        return NULL;
    size_t index = 0;
    char *token;
    // tokenize string based on delims
    while ((token = str_sep(&str, delims)) != NULL) {
        if (*token != '\0') {  // skip empty tokens
            tokens[index++] = token;
        }
    }
    // null terminate array
    tokens[index] = NULL;
    // return null terminated array
    return tokens;
}

// returns the length of a string.
size_t str_len(const char *s) {
    // store length
    size_t i = 0;
    // traverse through each char and add 1 to length
    while (*s++)
        ++i;
    // return length
    return i;
}

// compare up to n characters of two strings.
// returns a-b
        // <0 if a<b
        // 0 if they are the same
        // >0 if a>b
int str_n_cmp(const char *s1, const char *s2, size_t n) {
    if (n == 0) return 0;
    // loop until we reach n chars, a null terminator, or a mismatch
    while (n-- && *s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    // return the difference between the first mismatched chars
    return ((const unsigned char)*s1) - ((const unsigned char)*s2);
}
int str_n_cmp_desc(const char *s1, const char *s2, size_t n) {
    if (n == 0) return 0;
    // loop until we reach n chars, a null terminator, or a mismatch
    while (n-- && *s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    // return the difference between the first mismatched chars
    return ((const unsigned char)*s2) - ((const unsigned char)*s1);
}

// compares two strings completely
int str_cmp(const char *s1, const char *s2) {
    // size_t-1 represents the largest possible val of size_t
    // allows str_n_cmp compare the entire string without a limit
    // ensures that n will never reach 0 before the null terminator is found so function compares the entire strings completely
    return str_n_cmp_desc(s1, s2, (size_t)-1);
}

// Copies the string from src to dst.
char *str_cpy(char *restrict dst, const char *restrict src) {
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

// returns a new string which is a lowercase version of s
char *str_to_lower(const char *s) {
    // get the length of s
    size_t len = str_len(s);
    // allocate memory for null terminated, string length of s + 1
    char *lower = malloc(len + 1);
    if (!lower) return NULL;
    // convert each char to lowercase
    for (size_t i = 0; i < len; i++) {
        lower[i] = tolower((unsigned char)s[i]);
    }
    // null terminate array
    lower[len] = '\0';
    return lower;
}

//----------------- Hash Table Implementation -------------------

// each node in the hash table (a dictionary key/value pair)
typedef struct node {
    char *key;
    int value;
    struct node *next;
} node;

// hash table structure
typedef struct {
    node **nodes;  // linked list of nodes
    int table_size; 
} my_hashtable;

// creates a new hash table of the given size
my_hashtable *hashtable_create(int table_size) {
    my_hashtable *hashtable = malloc(sizeof(my_hashtable));
    if (!hashtable)
        return NULL;
    hashtable->table_size = table_size;
    // allocate memory according to the number of nodes in hashtable
    hashtable->nodes = malloc(sizeof(node*) * table_size);
    if (!hashtable->nodes) {
        free(hashtable);
        return NULL;
    }
    // instantiate all nodes in hash table to null pointer
    for (int i = 0; i < table_size; ++i)
        hashtable->nodes[i] = NULL;
    return hashtable;
}

// hash function for strings
unsigned int hash(const char *key, int table_size) {
    // stores the computed hash value
    unsigned long int value = 0;
    // stores the length of key
    unsigned int key_len = str_len(key);
    // traverse through each char in the key
    for (unsigned int i = 0; i < key_len; ++i)
        // multiply the curr val by 37 and add the ascii val of char
        value = value * 37 + key[i];
    // make sure the hash value fits within the table size by taking the modulous
    return value % table_size;
}

// creates a key/value pair.
node *hashtable_pair(const char *key, int value) {
    // allocate memory for a new node
    node *item = malloc(sizeof(node));
    if (!item)
        return NULL;
    // allocate memory for the key string including null terminator
    item->key = malloc(str_len(key) + 1);
    if (!item->key) {
        free(item);
        return NULL;
    }
    // copy the key into the newly allocated memory
    str_cpy(item->key, key);
    // store the value in the node
    item->value = value;
    // set the next pointer to null
    item->next = NULL;
    return item;
}

// inserts a key into the hash table or increments its count if it already exists.
void hashtable_set(my_hashtable *hashtable, const char *key, int value) {
    // get the hashtable size
    int table_size = hashtable->table_size;
    // compute the hash slot for the key
    unsigned int slot = hash(key, table_size);
    // get the node at the computed slot
    node *item = hashtable->nodes[slot];

    // if the slot is empty, insert the new key value pair
    if (item == NULL) {
        hashtable->nodes[slot] = hashtable_pair(key, value);
        return;
    }

    node *prev = NULL;
    while (item != NULL) {
        // key exists, so increment its value by 1, this stores the count of each word
        if (str_cmp(item->key, key) == 0) {
            item->value += value;
            return;
        }
        // keep track of prev ndoe
        prev = item;
        // traverse to next node
        item = item->next;
    }
    // key not found, append a new pair to the linked list
    prev->next = hashtable_pair(key, value);
}

// retrieves the value associated with a key (or 0 if not found).
int hashtable_get(my_hashtable *hashtable, const char *key) {
    // get the size of hash table
    int table_size = hashtable->table_size;
    //compute the hash slot for the key
    unsigned int slot = hash(key, table_size);
    // get the first node in slot
    node *item = hashtable->nodes[slot];
    // traverse the linked list in case of collisions (multiple nodes in the same slot)
    while (item != NULL) {
        // if the key matches, return its value
        if (str_cmp(item->key, key) == 0)
            return item->value;
        // move on to next node in chain
        item = item->next;
    }
    // key not found so return 0
    return 0;
}

// prints the contents of the hash table 
void print_hashtable(my_hashtable *hashtable) {
    // get the hash table size
    int table_size = hashtable->table_size;
    // iterate through each slot in hashtable
    for (int i = 0; i < table_size; ++i) {
        // get the first node in slot
        node *item = hashtable->nodes[i];
        // traverse linked list and print each key value pair
        while (item != NULL) {
            printf("%s %d\n", item->key, item->value);
            item = item->next;
        }
    }
}

//----------------- Comparator for Sorting Hash Table nodes -------------------

// sort hash table nodes (pointed to by node*) in ascending order by key.
int compare_nodes_asc(const void *a, const void *b) {
    // pointers to node pointers
    // type casted because qsort requires void pointers.
    const node *e1 = *(const node **)a;
    const node *e2 = *(const node **)b;
    // compare the keys using string comparison
    // str_cmp returns a-b
        // <0 if a<b
        // 0 if they are the same
        // >0 if a>b
    return str_cmp(e1->key, e2->key);
}

//----------------- Main -------------------

int main(int argc, char *argv[]){

    // incorrect number of arguments provided
    if (argc < 3) {
        printf("Incorrect number of arguments providedd");
        return 1;
    }

    // first argument represents input file name
    char *inputFileName = argv[1];
    // second argument represents output file name
    char *outFileName = argv[2];

    // print input and output file names for users
    printf("inputFileName: %s\n", inputFileName);
    printf("outputFileName: %s\n", outFileName);

    // open the input file in read only mode
    int fd = open(inputFileName, O_RDONLY);
    // if open() returns -1, there was an error opening the file
    if (fd == -1) {
        printf("Error opening file!\n");
        return 1;
    }

    // struct to store the file size
    struct stat fileStat;
    // use fstat to get the file size and store at fileStat struct
    // if fstat returns -1, there was an error getting file size
    if (fstat(fd, &fileStat) == -1) {
        printf("Error getting file size!\n");
        close(fd);
        return 1;
    }
    // store size of file using fileStats st_size attribute
    size_t fileSize = fileStat.st_size;

    // allocate a buffer to hold the file contents of size fileSize
    char *buffer = malloc(fileSize + 1);
    // char *buffer = malloc(10000000);
    if (!buffer) {
        printf("Memory allocation failed!\n");
        close(fd);
        return 1;
    }

    // read the file into the buffer
    ssize_t bytesRead = read(fd, buffer, fileSize);
    if (bytesRead == -1) {
        printf("Error reading file!\n");
        free(buffer);
        close(fd);
        return 1;
    }
    // null terminate the buffer.
    buffer[bytesRead] = '\0';  
    // close file descriptor because we already have file content stored in buffer
    close(fd);

    // convert the buffer to lowercase.
    char *lower_buffer = str_to_lower(buffer);
    free(buffer);
    buffer = lower_buffer;

    // count the number of tokens and store size
    size_t num_tokens = count_tokens(buffer, " \n\t.,!?;:-'\"");
    // tokenize the buffer using common word delimiters.
    char **tokens = tokenize(buffer, " \n\t.,!?;:-'\"");
    if (!tokens) {
        printf("Tokenization failed!\n");
        free(buffer);
        return 1;
    }

    // create a hash table with size based on the number of tokens
    int table_size = num_tokens;  
    my_hashtable *hashtable = hashtable_create(table_size);
    if (!hashtable) {
        printf("Hash table creation failed!\n");
        free(tokens);
        free(buffer);
        return 1;
    }

    // insert tokens into the hash table, incrementing counts for duplicate words
    for (char **t = tokens; *t != NULL; t++) {
        if (**t != '\0') {  // skip empty tokens
            // if key doesnt exist we set keys value to 1
            // if key exists we add 1 to current keys value
            hashtable_set(hashtable, *t, 1);
        }
    }

    // free the tokens array since we no longer need it.
    free(tokens);

    // count the number of nodes in the hash table
    int total_nodes = 0;
    for (int i = 0; i < hashtable->table_size; ++i) {
        node *item = hashtable->nodes[i];
        while (item != NULL) {
            total_nodes++;
            item = item->next;
        }
    }
    // allocate memory for array to store hash table nodes for sorting
    node **nodes_array = malloc(total_nodes * sizeof(node *));
    if (!nodes_array) {
        printf("Memory allocation failed for nodes array!\n");
        free(buffer);
        return 1;
    }
    // populate array with hash table nodes
    int index = 0;
    for (int i = 0; i < hashtable->table_size; ++i) {
        node *item = hashtable->nodes[i];
        while (item != NULL) {
            nodes_array[index++] = item;
            item = item->next;
        }
    }

    // sort the nodes array in ascending order by key
    qsort(nodes_array, total_nodes, sizeof(node *), compare_nodes_asc);

    // ------------------------------------------------------
    // open the output file for writing using open()
        // O_WRONLY: opens file for writing only
        // OCREAT: creates file if it doesnt exist
        // O_TRUNC: clears files contents if it already exists
        // S_IRWXU (0700 in octal or rwx------ in permissions)
    int outfile_fd = open(outFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    // open() returns -1 if file couldnt be opened
    if (outfile_fd==-1) {
        printf("Error opening output file!\n");
        free(nodes_array);
        free(buffer);
        return 1;
    }
    // buffer to hold the formatted string for each hash table entry
    char hashtable_buffer[2000];
    // write each key with its occurrence count to the output file
    for (int i = 0; i < total_nodes; i++) {
        // len stores number of chars written
        int len = sprintf(hashtable_buffer, "%s %d\n", nodes_array[i]->key, nodes_array[i]->value);
        // write the formatted string to the file
        if (write(outfile_fd, hashtable_buffer, len) == -1) {
            
            close(outfile_fd);
            free(nodes_array);
            free(buffer);
            return 1;
        }
    }
    close(outfile_fd);

    

    // free the nodes array.
    free(nodes_array);

    // free each hash table item
    // iterate through each slot in hash table
    for (int i = 0; i < hashtable->table_size; ++i) {
        // get the head of the linked list at the current hash table slot
        node *item = hashtable->nodes[i];
        // traverse the linked list and free each node
        while (item) {
            node *temp = item;
            item = item->next;
            free(temp->key);
            free(temp);
        }
    }
    // free all nodes in hashtable
    free(hashtable->nodes);
    // free the hash table
    free(hashtable);

    // Free the buffer.
    free(buffer);

    return 0;
}
