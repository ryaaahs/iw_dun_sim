#include <stdio.h>
#include "parson.h"

int main(void) {
    JSON_Value *root_value;
    JSON_Object *game_values;
    JSON_Object *config;
    JSON_Array *dungeons;
    JSON_Object *dungeon;
    JSON_Object *player;
    /*
    JSON_Array *key_values;
    JSON_Object *logs;
    JSON_Object *ores;
    JSON_Object *flowers;
    JSON_Object *fishes;
    JSON_Object *gemstones;
    JSON_Object *bones;*/
    
    /* Get access to the json and confirm the root value is a json type */ 
    root_value = json_parse_file("json/values.json");
    if (json_value_get_type(root_value) != JSONObject) {
        fprintf(stderr, "%s", "Incorrect JSON\n");
        return -1;
    }

    game_values = json_value_get_object(root_value);
    config = json_object_get_object(game_values, "config");
    player = json_object_get_object(game_values, "player");
    dungeons = json_object_get_array(game_values, "dungeons");


    const double BASE_POTION_SPEED = 180.0;
    const double BASE_POTENT_MULTIPLER = 6.0;
    const unsigned int HOURS_IN_DAY = 24;
    const unsigned int LOOT_ROLLS_PER_KEY = 3;
    const unsigned int HOUR_IN_SECONDS = 3600;
    const unsigned int DISPLAY_PRECISION = 15;
    const unsigned int SIMULATED_HOURS = json_object_get_string(config, "simulated_hours"); 
    const int SIMPLE_DISPLAY = json_object_get_string(config, "simple_display"); 
    const double POTION_COST_PER_HOUR = (HOUR_IN_SECONDS / json_object_get_number(player, "potion_speed")) * json_object_get_number(player, "potion_cost");
    const double SIM_POTION_COST = POTION_COST_PER_HOUR * SIMULATED_HOURS;
    const double INSATIABLE_PIES_PER_HOUR = ((HOUR_IN_SECONDS * json_object_get_number(player, "insatiable_consume_rate")) / json_object_get_number(player, "food_health"));
    const double INSATIABLE_COST_PER_HOUR = INSATIABLE_PIES_PER_HOUR * json_object_get_number(player, "food_cost");

    double food_cost_per_hour = 0.0;
    double sim_food_cost = 0.0;
    unsigned int keys_per_hour = 0;
    double keys_cost_per_hour = 0;

    long long generated_sim_gold = 0;

    int total_loot_cycles = 0;
    int total_keys_used = 0;
    int total_preserved_keys = 0;
    int total_bone = 0;
    int total_bone_one = 0;
    int total_bone_two = 0;
    int total_log = 0;
    int total_ore = 0;
    int total_flower = 0;
    int total_fish = 0;
    int total_gem_one = 0;
    int total_gem_two = 0;
    unsigned int counter = 0;
    unsigned int rolls = 0;
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

    return 0;
}