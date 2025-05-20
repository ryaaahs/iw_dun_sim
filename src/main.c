#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include "parson.h"
#include "market_parser.h"

double rand_zero_one() {
    return (double)rand() / RAND_MAX;
}

int rand_number_range(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

struct MaterialsProduced {
    long int total_gold;
    long int total_bone;
    long int total_bone_one;
    long int total_bone_two;
    long int total_logs;
    long int total_ores;
    long int total_flowers;
    long int total_fishes;
    long int total_gem_one;
    long int total_gem_two;
};

struct DropRates {
    float gold_rate;
    float log_rate;
    float ore_rate;
    float flower_rate;
    float fish_rate;
    float gem_one_rate;
    float gem_two_rate;
}; 

int main(int argc, char *argv[]) {
    char* file_name;
    int parse_market_file = 1;

    JSON_Value *root_value;
    JSON_Object *game_values;
    JSON_Object *config;
    JSON_Array *dungeons;
    JSON_Object *dungeon;
    JSON_Object *player;
    size_t i;
    size_t j;
    size_t k;

    unsigned int SIMULATED_HOURS;
    float PLAYER_KEY_PRESERVE;
    float PLAYER_DOUBLE_LOOT_CHANCE;
    float PLAYER_SAVAGE_CHANCE;
    float PLAYER_ADDITIONAL_COIN_CHANCE;
    unsigned int KEYS_PER_HOUR;
    unsigned int FOOD_PER_HOUR;
    int GOLD_MIN;
    int GOLD_MAX;

    unsigned int rolls = 0;
    unsigned int total_keys_used = 0;
    unsigned int total_preserved_keys = 0;
    unsigned int total_double_loot_procs = 0;

    srand(time(NULL));

    if (argc == 1) {
        printf("Please pass in a file name that exists in the current diectory.\n");
        printf("Arg 1 (string): file name containing simulation data");
        printf("Arg 2 (bool) [Default: 1]: Parse market_data.json to fill simulation_values.json");
        printf("Ex: iw_dun_sim simulation_values.json\n"); 
    } else if (argc == 2) {
        file_name = argv[1];
    }

    if (argc == 3) {
        parse_market_file = strtol(argv[2], NULL, 10);
    }
    
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

    SIMULATED_HOURS = json_object_dotget_number(config, "simulated_hours");
    PLAYER_KEY_PRESERVE = json_object_dotget_number(player, "preserve_dungeon_chance");
    PLAYER_DOUBLE_LOOT_CHANCE = json_object_dotget_number(player, "double_loot_chance");
    PLAYER_SAVAGE_CHANCE = json_object_dotget_number(player, "savage_bone_drop_chance");
    PLAYER_ADDITIONAL_COIN_CHANCE = json_object_dotget_number(player, "additional_coins_chance");

    /* Dungeon loop */
    /*
        if !key_pres
            key_rate;
        loop 3:
            loot
            if double loot
                loot
        Don't need to worry about eff as its baked into the kph
        if eff:
            if !key_pres
                key
            loop 3:
                loot
                if double loot
                loot
    */
    for (i = 0; i < json_array_get_count(dungeons); i++) {
        struct DropRates *drop_rates;
        struct MaterialsProduced *total_matierals;

        dungeon = json_array_get_object(dungeons, i);
        drop_rates = malloc(sizeof(struct DropRates));
        total_matierals = malloc(sizeof(struct MaterialsProduced));

        KEYS_PER_HOUR = json_object_dotget_number(dungeon, "monsters_hour") / 3;
        FOOD_PER_HOUR = json_object_get_number(dungeon, "food");

        GOLD_MIN = json_object_dotget_number(dungeon, "drops.gold.gold_min");
        GOLD_MAX = json_object_dotget_number(dungeon, "drops.gold.gold_max");

        drop_rates->gold_rate = json_object_dotget_number(dungeon, "drops.gold.rate");
        drop_rates->log_rate = json_object_dotget_number(dungeon, "drops.log.rate");
        drop_rates->ore_rate = json_object_dotget_number(dungeon, "drops.ore.rate");
        drop_rates->flower_rate = json_object_dotget_number(dungeon, "drops.flower.rate");
        drop_rates->fish_rate = json_object_dotget_number(dungeon, "drops.fish.rate");
        drop_rates->gem_one_rate = json_object_dotget_number(dungeon, "drops.gem_one.rate");
        drop_rates->gem_two_rate = json_object_dotget_number(dungeon, "drops.gem_two.rate");

        total_matierals->total_gold = 0;
        total_matierals->total_bone = 0;
        total_matierals->total_bone_one = 0;
        total_matierals->total_bone_two = 0;
        total_matierals->total_logs = 0;
        total_matierals->total_ores = 0;
        total_matierals->total_flowers = 0;
        total_matierals->total_fishes = 0;
        total_matierals->total_gem_one = 0;
        total_matierals->total_gem_two = 0;

        /* Get loot rolls */
        for (j = 0; j < (SIMULATED_HOURS * KEYS_PER_HOUR); j++) {
            rolls += 3;
            total_keys_used++;

            if (PLAYER_KEY_PRESERVE > 0 && PLAYER_KEY_PRESERVE >= rand_zero_one()) { 
                total_preserved_keys++;
            }

            if (PLAYER_DOUBLE_LOOT_CHANCE >= rand_zero_one()) {
                rolls += 3; 
                total_double_loot_procs++;
            }
        }

        printf("-------------------------------\n");
        printf("Usage\n");
        printf("Simulated Hours: %d\n", SIMULATED_HOURS);
        if (SIMULATED_HOURS > 1) printf("Rolls (AVG Hours): %d\n", rolls / SIMULATED_HOURS);
        printf("Rolls: %d\n", rolls);
        printf("Double Loot Procs: %d\n", total_double_loot_procs);
        printf("Total Keys: %d\n", (SIMULATED_HOURS * KEYS_PER_HOUR));
        printf("Total Keys Preserved: %d\n", total_preserved_keys);
        printf("-------------------------------\n");

        /* Run rolls aganist loot rates */ 
        for (j = 0; j < rolls; j++) {
            if (PLAYER_SAVAGE_CHANCE >= rand_zero_one()) {
                for (k = 0; k < 2; k++) {
                    if(rand_number_range(0,1)) {
                        total_matierals->total_bone_one++;
                    } else {
                        total_matierals->total_bone_two++;
                    }
                }
            } else {
                total_matierals->total_bone++;
            }

            if (drop_rates->gold_rate >= rand_zero_one()) {
                total_matierals->total_gold += rand_number_range(GOLD_MIN, GOLD_MAX);

                if (PLAYER_ADDITIONAL_COIN_CHANCE > 0 && PLAYER_ADDITIONAL_COIN_CHANCE >= rand_zero_one()) {
                    total_matierals->total_gold += rand_number_range(GOLD_MIN, GOLD_MAX);
                }
            }

            if (drop_rates->log_rate >= rand_zero_one()) {
                total_matierals->total_logs += rand_number_range(1, 8);
            }

            if (drop_rates->ore_rate >= rand_zero_one()) {
                total_matierals->total_ores += rand_number_range(1, 8);
            }
            
            if (drop_rates->flower_rate >= rand_zero_one()) {
                total_matierals->total_flowers += rand_number_range(1, 8);
            }

            if (drop_rates->fish_rate >= rand_zero_one()) {
                total_matierals->total_fishes += rand_number_range(1, 8);
            }

            if (drop_rates->gem_one_rate >= rand_zero_one()) {
                total_matierals->total_gem_one += rand_number_range(1, 2);
            }

            if (drop_rates->gem_two_rate >= rand_zero_one()) {
                total_matierals->total_gem_two += rand_number_range(1, 2);
            }
        }

        if (SIMULATED_HOURS > 1) {
            printf("Loot (AVG Hours)\n");
            printf("Gold: %ld\n", total_matierals->total_gold);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.bone.name"), total_matierals->total_bone / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.bone_one.name"), total_matierals->total_bone_one / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.bone_two.name"), total_matierals->total_bone_two / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.log.name"), total_matierals->total_logs / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.ore.name"), total_matierals->total_ores / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.flower.name"), total_matierals->total_flowers / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.fish.name"), total_matierals->total_fishes / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.gem_one.name"), total_matierals->total_gem_one / SIMULATED_HOURS);
            printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.gem_two.name"), total_matierals->total_gem_two / SIMULATED_HOURS);
            printf("-------------------------------\n");
        }

        printf("Loot\n");
        printf("Gold: %ld\n", total_matierals->total_gold);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.bone.name"), total_matierals->total_bone);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.bone_one.name"), total_matierals->total_bone_one);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.bone_two.name"), total_matierals->total_bone_two);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.log.name"), total_matierals->total_logs);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.ore.name"), total_matierals->total_ores);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.flower.name"), total_matierals->total_flowers);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.fish.name"), total_matierals->total_fishes);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.gem_one.name"), total_matierals->total_gem_one);
        printf("%s: %ld\n", json_object_dotget_string(dungeon, "drops.gem_two.name"), total_matierals->total_gem_two);
        printf("-------------------------------\n");

        free(drop_rates);
        free(total_matierals); 
    }

    if (parse_market_file)
        parse_market_data(root_value); 

    json_value_free(root_value);
    return 0;
}