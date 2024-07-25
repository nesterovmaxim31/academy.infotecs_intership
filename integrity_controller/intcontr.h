#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>
#include <string.h>
#include <syslog.h>
#include <ctype.h>

#include <openssl/sha.h>

#define SHA256HEXSIZE 32
#define MAXPATHLENGHT 1024

typedef unsigned char* sha256sum;
typedef unsigned char* file_content_t;


struct file_content_info_st {
    file_content_t file_content;
    size_t file_size;
};


struct directory_info_st {
    char **all_files_path;
    size_t current_file_index;
};


struct fileconf_line_st {
    char* checksum;
    char *file_path;
    int is_checked;
};


struct input_st {
    char *path;
    char *config;
    int check_intergrity;
    int put_under_control;
    int help;
};


typedef struct file_content_info_st file_content_info_t;
typedef struct directory_info_st directory_info_t;

/* Function prototypes */
/* intcontr.c */
void check_intergrity(char *path, char *path_to_fileconf);
void generate_config(char *path, char *path_to_fileconf);
void print_help_page();


/* read_file.c */
file_content_info_t* get_file_info(char *file_name);
directory_info_t* directory_handler(char *path, directory_info_t* dir);


/* checksum.c */
sha256sum gen_file_checksum(char *file_name);
int check_file_checksum(char *file_name, sha256sum previous_checksum);


/* intput_handler.c */
void parse_input(int argc, char** argv, struct input_st*);


/* fileconf_handler.c */
void write_newline(sha256sum file_sum, char *path_to_fileconf, char *path);
int check_fileconf(char *path);
int compare_two_fileconf(char *path_file_previous, char *path_file_current);