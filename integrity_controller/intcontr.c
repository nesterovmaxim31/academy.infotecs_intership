#include "intcontr.h"

// Функция для генерации списка контроля целостности
void generate_config(char *path, char *path_to_fileconf) {
    directory_info_t *dir = (directory_info_t*)malloc(sizeof(directory_info_t));

    dir ->  all_files_path = NULL;
    dir -> current_file_index = 0;
    dir = directory_handler(path, dir);
    FILE *file_pointer;

    if (path_to_fileconf == NULL) {
        path_to_fileconf = "config.txt";
    }
    
    // Для очистки всего содержимого файла, если оно было
    file_pointer = fopen(path_to_fileconf, "w");
    fclose(file_pointer);

    for (size_t i = 0; i < dir -> current_file_index; i++) {
        sha256sum file_sum = gen_file_checksum(dir ->all_files_path[i]);
        write_newline(file_sum, path_to_fileconf, dir -> all_files_path[i]);
    }


    if (strcmp(path_to_fileconf, ".fileconf.txt")) {
        printf("Целостность заверена. Список контроля целостности помещён в файл %s\n", path_to_fileconf);
        syslog(LOG_INFO, "Integrity certified. The checklist is securely placed on file %s", path_to_fileconf);
    }
    

    free(dir);
}

// Проверка на целостность будет осуществлять путём конфигурации нового
// списка контроля целостности (.config) и сравнения его с прошлым
void check_intergrity(char *path, char *config) {
    generate_config(path, ".fileconf.txt");
    if (compare_two_fileconf(config, ".fileconf.txt")) {
        puts("Нарушений контроля целостности не обнаружено");
        syslog(LOG_INFO, "Integrity is not broken");
    } else {
        puts("Обнаружены нарушения контроля целостности");
        syslog(LOG_ERR, "Integrity is broken");
    }
    system("rm .fileconf.txt");
}

// Функция для вывода в консоль help page (вспомогательная страница)
void print_help_page() {
    puts("Integrity controller. \n\
    --help              Указание на вывод help_page \n\
    --config            Указание пути до списка контроля целостности \n\
    --path              Указание пути до контролируемого каталога \n\
    --check_integrity  Указание на проведение контроля целостности контролируемого \n\
    каталога. При указание данной опции должны быть также прописаны --config и --path \n\
    --put_under_control Указание на постановку на контроль нового каталога. При \n\
    указании данной опции должна быть прописана --path \n\
    \nПримеры:\n\
    ./intcontr --path /home/user/Documents/Education --config education_check.txt --put_under_control\n\
    Данная команду производит генерацию списка контроля целостности для каталога /home/user/Documents/Education\n\
    и помещает конфигурацию в файл education_check.txt. Для проверки целостности данного каталога \n\
    Пропишите команду ниже:\n\
    ./intcontr --path /home/user/Documents/Education --config education_check.txt --check_integrity");
}


int main(int argc, char **argv) {
    struct input_st input;

    parse_input(argc, argv, &input);

    if (input.help) print_help_page();

    if (input.check_intergrity && input.put_under_control) {
        puts("Ошибка. Невозможно одновременно выполнить две операции\n");
        syslog(LOG_ERR, "Error. It is impossible to do both operations at once");
        return 1;
    } else if (input.check_intergrity) {
        if (input.config == NULL || input.path == NULL) {
            puts("Ошибка. Должны быть указаны и путь до каталога, и путь до списка контроля целостности");
            syslog(LOG_ERR, "Error. path and config args must be given");
            return 1;
        } 
        check_intergrity(input.path, input.config);
    } else if (input.put_under_control) {
         if (input.path == NULL) {
            puts("Ошибка. Должен быть указаны и путь до каталога");
            syslog(LOG_ERR, "Error. path argument must be given");
            return 1;
        } 
        generate_config(input.path, input.config);
    }

    return 0;
}