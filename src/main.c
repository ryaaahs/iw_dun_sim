#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "parson.h"
#include "market_parser.h"

double rand_zero_one() {
    return (double)rand() / RAND_MAX;
}

int rand_number_range(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

/* https://stackoverflow.com/a/1449859 */
void printf_commas(int value) {
    int value_container = 0;
    int scale = 1;
    
    /* Print a negative sign and flip the value to positive for remaining calulcation */
    if (value < 0) {
        printf ("-");
        value = -value;
    }

    /* Scale the value down to the lowest denomination and print it */
    while (value >= 1000) {
        value_container = value_container + scale * (value % 1000);
        value /= 1000;
        scale *= 1000;
    }
    printf ("%d", value);

    /* Work our way back up using the value container and scale. */
    while (scale != 1) {
        scale /= 1000;
        value = value_container / scale;
        value_container = value_container % scale;
        printf (",%03d", value);
    }
    printf("\n");
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

struct Item {
    const char* name;
    const char* tag;
    const char* key;
    int min;
    int max;
    float rate;
};

int main(int argc, char *argv[]) {
    char* file_name;
    int parse_market_file;
    int display_type;

    JSON_Value *root_value;
    JSON_Object *game_values;
    JSON_Object *config;
    JSON_Array *dungeons;
    JSON_Object *dungeon;
    JSON_Array *drops;
    JSON_Object *drop;
    JSON_Object *player;
    size_t i;
    size_t j;
    size_t k;
    size_t n;

    unsigned int SIMULATED_HOURS;
    float PLAYER_KEY_PRESERVE;
    float PLAYER_DOUBLE_LOOT_CHANCE;
    float PLAYER_SAVAGE_CHANCE;
    float PLAYER_ADDITIONAL_COIN_CHANCE;
    unsigned int KEYS_PER_HOUR;
    unsigned int FOOD_PER_HOUR;

    unsigned int rolls = 0;
    unsigned int total_keys_used = 0;
    unsigned int total_preserved_keys = 0;
    unsigned int total_double_loot_procs = 0;

    struct MaterialsProduced *total_matierals; 

    srand(time(NULL));

    if (argc != 4) {
        printf("NAME\n");
        printf("\tiw_dun_sim - Dungeon Simulator for IWRPG game\n");
        printf("SYNOPSIS\n");
        printf("\tiw_dun_sim [SIM_JSON]... [MARKET_PARSE]... [DISPLAY_TYPE]\n");
        printf("ARGUMENTS\n");
        printf("\t[SIM_JSON] - Simulation json file located in your json folder\n");
        printf("\t[MARKET_PARSE] - Options to filter the market data\n");
        printf("\t\t[0] - Skip Market Parse\n");
        printf("\t\t[1] - Lowest sellable value between Sell/Buy\n");
        printf("\t\t[2] - Only use Sell Order\n");
        printf("\t\t[3] - Only use Buy Order\n");
        printf("\t[DISPLAY_TYPE] - What Dungeons do you want to display\n");
        printf("\t\t[0] - All\n");
        printf("\t\t[25] - Dungeon 25\n");
        printf("\t\t[40] - Dungeon 40\n");
        printf("\t\t[55] - Dungeon 55\n");
        printf("\t\t[70] - Dungeon 70\n");
        printf("\t\t[85] - Dungeon 85\n");
        printf("\t\t[100] - Dungeon 100\n");

        return -1;
    } else if (argc == 4) {
        file_name = argv[1];
        parse_market_file = strtol(argv[2], NULL, 10);
        display_type = strtol(argv[3], NULL, 10);
    }
    
    /* Get access to the json and confirm the root value is a json type */ 
    root_value = json_parse_file(file_name);
    if (json_value_get_type(root_value) != JSONObject) {
        fprintf(stderr, "%s", "Incorrect JSON, expecting JSONObject\n");
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

    total_matierals = malloc(sizeof(struct MaterialsProduced));

    if (parse_market_file != 0) {
        parse_market_data(root_value, parse_market_file);
    }
         
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
        struct Item *item_drops;

        dungeon = json_array_get_object(dungeons, i);
        drops = json_object_get_array(dungeon, "drops");
        item_drops = malloc(json_array_get_count(drops) * sizeof(struct Item)); 
        rolls = 0;

        if (display_type != 0) {
            if (json_object_get_number(dungeon, "level") != display_type)
                continue;
        }

        for (j = 0; j < json_array_get_count(drops); j++) {
            drop = json_array_get_object(drops, j);
            item_drops[j].name = json_object_get_string(drop, "name");
            item_drops[j].tag = json_object_get_string(drop, "tag");
            item_drops[j].key = json_object_get_string(drop, "key");
            item_drops[j].min = json_object_get_number(drop, "min");
            item_drops[j].max = json_object_get_number(drop, "max");
            item_drops[j].rate = json_object_get_number(drop, "rate");
        }

        KEYS_PER_HOUR = json_object_dotget_number(dungeon, "monsters_hour") / 3;
        FOOD_PER_HOUR = json_object_get_number(dungeon, "food");

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

        printf("Usage\n");
        printf("Level: %.0f\n", json_object_get_number(dungeon, "level"));
        printf("Simulated Hours: "); printf_commas(SIMULATED_HOURS);
        if (SIMULATED_HOURS > 1) {
            printf("Rolls (AVG Hours): "); printf_commas(rolls / SIMULATED_HOURS);
        } 
        printf("Rolls: "); printf_commas(rolls);
        printf("Double Loot Procs: ");  printf_commas(total_double_loot_procs);
        printf("Total Keys: "); printf_commas((SIMULATED_HOURS * KEYS_PER_HOUR));
        printf("Total Keys Preserved: "); printf_commas(total_preserved_keys);
        printf("-------------------------------\n");

        /* Run rolls aganist loot rates */ 
        for (j = 0; j < rolls; j++) {
            for (k = 0; k < json_array_get_count(drops); k++) {
                if (strcmp(item_drops[k].key, "bone") == 0) {
                    if (PLAYER_SAVAGE_CHANCE >= rand_zero_one()) {
                        for (n = 0; n < 2; n++) {
                            if(rand_number_range(0,1)) {
                                total_matierals->total_bone_one++;
                            } else {
                                total_matierals->total_bone_two++;
                            }
                        }
                    } else {
                        total_matierals->total_bone++;
                    }
                } else if (strcmp(item_drops[k].key, "bone_one") == 0 || strcmp(item_drops[k].key, "bone_two") == 0) {
                    continue;
                } else if (strcmp(item_drops[k].key, "gold") == 0) {
                    if (item_drops[k].rate >= rand_zero_one()) {
                        total_matierals->total_gold += rand_number_range(item_drops[k].min, item_drops[k].max);
        
                        if (PLAYER_ADDITIONAL_COIN_CHANCE > 0 && PLAYER_ADDITIONAL_COIN_CHANCE >= rand_zero_one()) {
                            total_matierals->total_gold += rand_number_range(item_drops[k].min, item_drops[k].max);
                        }
                    }
                } else {
                    /* Materials */
                    if (strcmp(item_drops[k].key, "log") == 0) {
                        if (item_drops[k].rate >= rand_zero_one()) {
                            total_matierals->total_logs += rand_number_range(item_drops[k].min, item_drops[k].max);
                        }
                    } else if (strcmp(item_drops[k].key, "ore") == 0) {
                        if (item_drops[k].rate >= rand_zero_one()) {
                            total_matierals->total_ores += rand_number_range(item_drops[k].min, item_drops[k].max);
                        }
                    } else if (strcmp(item_drops[k].key, "flower") == 0) {
                        if (item_drops[k].rate >= rand_zero_one()) {
                            total_matierals->total_flowers += rand_number_range(item_drops[k].min, item_drops[k].max);
                        }
                    } else if (strcmp(item_drops[k].key, "fish") == 0) {
                        if (item_drops[k].rate >= rand_zero_one()) {
                            total_matierals->total_fishes += rand_number_range(item_drops[k].min, item_drops[k].max);
                        }
                    } else if (strcmp(item_drops[k].key, "gemstone_one") == 0) {
                        if (item_drops[k].rate >= rand_zero_one()) {
                            total_matierals->total_gem_one += rand_number_range(item_drops[k].min, item_drops[k].max);
                        }
                    } else if (strcmp(item_drops[k].key, "gemstone_two") == 0) {
                        if (item_drops[k].rate >= rand_zero_one()) {
                            total_matierals->total_gem_two += rand_number_range(item_drops[k].min, item_drops[k].max);
                        }
                    }
                }
            }
        }
        
        if (SIMULATED_HOURS > 1) {
            printf("Loot (AVG Hours)\n");
            for (j = 0; j < json_array_get_count(drops); j++) {
                printf("%s: ", item_drops[j].name); 

                if (strcmp(item_drops[j].key, "gold") == 0) {
                    printf_commas(total_matierals->total_gold / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "bone") == 0) {
                    printf_commas(total_matierals->total_bone / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "bone_one") == 0) {
                    printf_commas(total_matierals->total_bone_one / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "bone_two") == 0) {
                    printf_commas(total_matierals->total_bone_two / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "log") == 0) {
                    printf_commas(total_matierals->total_logs / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "ore") == 0) {
                    printf_commas(total_matierals->total_ores / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "flower") == 0) {
                    printf_commas(total_matierals->total_flowers / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "fish") == 0) {
                    printf_commas(total_matierals->total_fishes / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "gemstone_one") == 0) {
                    printf_commas(total_matierals->total_gem_one / SIMULATED_HOURS);
                } else if (strcmp(item_drops[j].key, "gemstone_two") == 0) {
                    printf_commas(total_matierals->total_gem_two / SIMULATED_HOURS);
                }
            }
            printf("-------------------------------\n");
        }

        for (j = 0; j < json_array_get_count(drops); j++) {
            printf("%s: ", item_drops[j].name); 

            if (strcmp(item_drops[j].key, "gold") == 0) {
                printf_commas(total_matierals->total_gold);
            } else if (strcmp(item_drops[j].key, "bone") == 0) {
                printf_commas(total_matierals->total_bone);
            } else if (strcmp(item_drops[j].key, "bone_one") == 0) {
                printf_commas(total_matierals->total_bone_one);
            } else if (strcmp(item_drops[j].key, "bone_two") == 0) {
                printf_commas(total_matierals->total_bone_two);
            } else if (strcmp(item_drops[j].key, "log") == 0) {
                printf_commas(total_matierals->total_logs);
            } else if (strcmp(item_drops[j].key, "ore") == 0) {
                printf_commas(total_matierals->total_ores);
            } else if (strcmp(item_drops[j].key, "flower") == 0) {
                printf_commas(total_matierals->total_flowers);
            } else if (strcmp(item_drops[j].key, "fish") == 0) {
                printf_commas(total_matierals->total_fishes);
            } else if (strcmp(item_drops[j].key, "gemstone_one") == 0) {
                printf_commas(total_matierals->total_gem_one);
            } else if (strcmp(item_drops[j].key, "gemstone_two") == 0) {
                printf_commas(total_matierals->total_gem_two);
            }
        }
        printf("|-------------------------------|\n");
        
        free(item_drops);
    }

    free(total_matierals); 
    json_value_free(root_value);
    return 0;
}