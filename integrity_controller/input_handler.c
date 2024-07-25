#include "intcontr.h"

// Функция для обработки вводимых данных
void parse_input(int argc, char** argv, struct input_st* input) {
    int c, option_index = 0;
    input -> config = NULL;
    input -> path = NULL;
    input -> put_under_control = 0;
    input -> check_intergrity = 0;
    input -> help = 0; 

    const struct option data_long[] = {
     {"path", required_argument, NULL, 0},
     {"config", required_argument, 0, 1},
     {"check_integrity", no_argument, 0, 2},
     {"put_under_control", no_argument, 0, 3},
     {"help", no_argument, 0, 4},
     {0, 0, 0, 0}};

     

    while ((c = getopt_long(argc, argv, "",data_long, &option_index)) != -1) {
        switch (c) {
            case 0:
                input -> path = optarg;
                break;
            case 1:
                input -> config = optarg;
                break;
            case 2:
                input -> check_intergrity = 1;
                break;
            case 3:
                input -> put_under_control = 1;
                break;
            case 4:
                input -> help = 1;
                break;
        }
    }
}