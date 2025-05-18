#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "parson.h"
#include <unistd.h>


double rand_zero_one() {
    return (double)rand() / RAND_MAX;
}

int rand_number_range(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    char* file_name;
    int parse_market_file = 1;

    if (argc == 1) {
        printf("Please pass in a file name that exists in the current diectory.\n");
        printf("Arg 1 (string): file name containing simulation data");
        printf("Arg 2 (bool) [Default: 1]: Parse market_data.json to fill value.json");
        printf("Ex: iw_dun_sim value.json\n"); 
    } else if (argc == 2) {
        file_name = argv[1];
    }

    if (argc == 3) {
        parse_market_file = argv[2];
    }

    JSON_Value *root_value;
    JSON_Object *game_values;
    JSON_Object *config;
    JSON_Array *dungeons;
    JSON_Object *dungeon;
    JSON_Object *player;

    /* Get access to the json and confirm the root value is a json type */ 
    root_value = json_parse_file(file_name);
    if (json_value_get_type(root_value) != JSONObject) {
        fprintf(stderr, "%s", "Incorrect JSON\n");
        return -1;
    }

    game_values = json_value_get_object(root_value);
    config = json_object_get_object(game_values, "config");
    player = json_object_get_object(game_values, "player");
    dungeons = json_object_get_array(game_values, "dungeons");

    size_t i;

    /* Dungeon loop */
    for (i = 0; i < json_array_get_count(dungeons); i++) {
        dungeon = json_array_get_object(dungeons, i);
        printf("%f\n", json_object_get_number(dungeon, "level"));
        printf("%f\n", json_object_get_number(dungeon, "monsters_hour"));
        printf("%f\n", json_object_get_number(dungeon, "food"));

        printf("%f\n", json_object_dotget_number(dungeon, "drops.gold.gold_min"));
        printf("%f\n", json_object_dotget_number(dungeon, "drops.gold.gold_max"));
    }

    json_value_free(root_value);
    return 0;
}