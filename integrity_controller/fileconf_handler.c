#include "intcontr.h"

// Функция для записи новой строки в список контроля целостности
void write_newline(sha256sum file_sum, char *path_to_fileconf, char *path) {
    FILE *file_pointer;

    file_pointer = fopen(path_to_fileconf, "a");

    if (file_pointer == NULL) {
        syslog(LOG_ERR, "File %s error", path_to_fileconf);
        perror("File error");
        exit(1);
    }

    for (size_t i = 0; i < SHA256HEXSIZE; i++) {
        fprintf(file_pointer, "%02x", file_sum[i]);
    }
    fprintf(file_pointer, "    %s\n", path);

    fclose(file_pointer);
}

// Функция для нахождения количетсва строк в файле
int get_file_linenumber(char *file_path) {
    int ans = 0;
    char ch;
    FILE *file_pointer;

    file_pointer = fopen(file_path, "r");

    if (file_pointer == NULL) {
        syslog(LOG_ERR, "File %s error", file_path);
        perror("File error");
        exit(1);
    }

    while(!feof(file_pointer))
    {
        ch = fgetc(file_pointer);
        if(ch == '\n') {
            ans++;
        }
    }

    fclose(file_pointer);

    return ans;
}

// Функция для проверки корректности хэш-суммы
int check_sha256sum(char *file_sum) {
    int is_valid = 1;

    if (strlen(file_sum) != SHA256HEXSIZE * 2) is_valid = 0;

    for (size_t i = 0; i < SHA256HEXSIZE * 2; i++) {
        if (!isxdigit(file_sum[i])) is_valid = 0;
        break;
    }

    return is_valid;
}

// Функция для заполнения структуры данных fileconf_line_st путём чтения
// списка контроля целостности
void gen_fileconf_struct(char *file_path, struct fileconf_line_st *lines) {
    FILE *file_pointer;
    size_t line_size = 0, _buffer_size, i = 0, k = 0;
    char *line = NULL;

    file_pointer = fopen(file_path, "r");

    if (file_pointer == NULL) {
        syslog(LOG_ERR, "File %s error", file_path);
        perror("File error");
        exit(1);
    }

    line_size = getline(&line, &_buffer_size, file_pointer);
    while (line_size != -1) {
        lines[i].checksum = (char*)malloc(SHA256HEXSIZE * 2);
        lines[i].file_path = (char*)malloc(MAXPATHLENGHT);

        k = sscanf(line, "%s    %1024[^\n]s", lines[i].checksum, lines[i].file_path);

        if (!check_sha256sum(lines[i].checksum)) {
            printf("Configuration file %s is damaged\n", file_path);
            syslog(LOG_ERR, "Configuration file is damaged");
            exit(1);
        }

        lines[i].is_checked = 0;
        i++;

        line_size = getline(&line, &_buffer_size, file_pointer);
    }

    fclose(file_pointer);
}

// Функция для сравнивания двух списков контроля целостности
int compare_two_fileconf(char *path_file_previous, char *path_file_current) {
    int previous_file_linenumber, current_file_linenumber, is_equal = 1;
    struct fileconf_line_st *lines_current, *lines_previous;

    previous_file_linenumber = get_file_linenumber(path_file_previous);
    current_file_linenumber = get_file_linenumber(path_file_current);

    lines_previous = (struct fileconf_line_st*)malloc(sizeof(struct fileconf_line_st) * previous_file_linenumber);
    lines_current = (struct fileconf_line_st*)malloc(sizeof(struct fileconf_line_st) * current_file_linenumber);
    
    gen_fileconf_struct(path_file_previous, lines_previous);
    gen_fileconf_struct(path_file_current, lines_current);

    // Сравнение целостности файлов 
    for (size_t i = 0; i < previous_file_linenumber; i++) {
        for (size_t j = 0; j < current_file_linenumber; j++) {
            // Целостность файла не повреждена
            if (!strcmp(lines_previous[i].checksum, lines_current[j].checksum) &&
                !strcmp(lines_previous[i].file_path, lines_current[j].file_path)) {
                    // printf("File %s is good!\n", lines_current[j].file_path);
                    lines_previous[i].is_checked = 1;
                    lines_current[j].is_checked = 1;
                    break;
                }
            // Целостность файла повреждена
            else if (strcmp(lines_previous[i].checksum, lines_current[j].checksum) &&
                !strcmp(lines_previous[i].file_path, lines_current[j].file_path)) {
                    lines_previous[i].is_checked = 1;
                    lines_current[j].is_checked = 1;
                    syslog(LOG_ERR, "Integrity of file %s is broken", lines_current[j].file_path);
                    is_equal = 0;
                    break;
                }
        }
    }

    // Нахождение удалённых файлов
    for (size_t i = 0; i < previous_file_linenumber; i++) {
        if (!lines_previous[i].is_checked) {
            is_equal = 0;
            syslog(LOG_ERR, "Integrity is broken. File %s has been deleted", lines_previous[i].file_path);
        }
    }

    // Нахождение добавленных файлов
    for (size_t i = 0; i < current_file_linenumber; i++) {
        if (!lines_current[i].is_checked) {
            is_equal = 0;
            syslog(LOG_ERR, "Integrity is broken. New file %s has been added", lines_current[i].file_path);
        }
    }

    return is_equal;
}

