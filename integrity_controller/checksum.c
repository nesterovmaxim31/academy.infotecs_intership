#include "intcontr.h"

// Функция для генерации хэш-суммы SHA256
sha256sum gen_file_checksum(char *file_name) {
    sha256sum new_sum = (sha256sum)malloc(SHA256HEXSIZE);
    file_content_info_t *file;

    file = get_file_info(file_name);

    memcpy(new_sum ,SHA256(file -> file_content, file -> file_size, NULL), SHA256HEXSIZE);

    free(file);
    
    return new_sum;
}

// Функция для сранения предыдущей и текущей хэш-суммы для одного файла 
int check_file_checksum(char *file_name, sha256sum previous_checksum) {
    sha256sum new_sum;
    file_content_info_t *file;

    file = get_file_info(file_name);

    new_sum = SHA256(file -> file_content, file -> file_size, NULL);

    free(file);

    if (new_sum == previous_checksum) return 1;
    else return 0;
}