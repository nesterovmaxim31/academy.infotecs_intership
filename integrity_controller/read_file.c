#include "intcontr.h"

// Функция для обработки файла, а именно для нахождения его контрольной суммы и количества строк в нем
file_content_info_t* get_file_info(char *file_name) {
    file_content_info_t *file_info; 
    file_info = (file_content_info_t*)malloc(sizeof(file_content_info_t));
    FILE *file_pointer;

    file_pointer = fopen ( file_name , "r" );
    
    if (file_pointer == NULL) {
        syslog(LOG_ERR, "File %s error", file_name);
        perror("File error");
        exit(1);
    }

    
 
    fseek (file_pointer , 0 , SEEK_END);
    file_info -> file_size = ftell (file_pointer);
    rewind (file_pointer);

    file_info -> file_content = (file_content_t)malloc(file_info -> file_size);
    
    fread(file_info -> file_content,1,file_info -> file_size,file_pointer);

    fclose(file_pointer);
    
    return file_info;
}

// Функция для рекурсивного обхода всех файлов каталога
directory_info_t* directory_handler(char *path, directory_info_t* dir) {
    struct dirent *dp;
    DIR *current_dir = opendir(path);

    if (!current_dir){
        dir -> current_file_index++;
        dir -> all_files_path = (char**)realloc(dir -> all_files_path, dir -> current_file_index * 8);
        dir -> all_files_path[dir -> current_file_index - 1] = path;
        return dir;
    }

    while ((dp = readdir(current_dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            char *new_path = (char*)malloc(strlen(path) + strlen(dp -> d_name) + 2);
            strcpy(new_path, path);

            if (new_path[strlen(path) - 1] != '/') strcat(new_path, "/");
            
            strcat(new_path, dp->d_name);
            
            dir = directory_handler(new_path, dir);
        }
    }

    closedir(current_dir);

    return dir;
} 
