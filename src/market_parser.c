#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "market_parser.h"
#include "parson.h"

struct PriceContainer {
    double key_price;
    double bone_price;
    double bone_one_price;
    double bone_two_price;
    double log_price;
    double ore_price;
    double flower_price;
    double fish_price;
    double gemstone_one_price;
    double gemstone_two_price;
}; 

int serialize_sim_data(JSON_Value *sim_root_value, struct PriceContainer *price_containers);

int parse_market_data(JSON_Value *sim_root_value) {
    struct PriceContainer *price_containers;
    JSON_Object *game_values;
    JSON_Array *dungeons;
    JSON_Object *dungeon;
    JSON_Array *drops;
    JSON_Object *drop;

    JSON_Value *market_root_value;
    JSON_Object *market_data;
    JSON_Array *listings;
    JSON_Object *listing;
    size_t i;
    size_t j;
    size_t n;

    /* Game will not display values over a certian value for items */
    /* TODO: Figure out the formula that is used in game to determine this value for items */
    int MARKET_COST_OVERFLOW = 500;

    long int id;
    int market_order_type;
    double cost;
    const char *key;
    
    if (access(MARKET_DATA_JSON, F_OK) != 0) {
        fprintf(stderr, "File not found in JSON folder: %s\n", MARKET_DATA_JSON);
        return -1;
    }
    
    market_root_value = json_parse_file(MARKET_DATA_JSON);

    if (json_value_get_type(market_root_value) != JSONObject) {
        fprintf(stderr, "%s", "Incorrect JSON, expecting JSONObject\n");
        return -1;
    }

    game_values = json_value_get_object(sim_root_value);
    dungeons = json_object_get_array(game_values, "dungeons");
    price_containers = malloc(json_array_get_count(dungeons) * sizeof(struct PriceContainer)); 

    /* Initialize struct to remove valgrind errors */
    for (j = 0; j < json_array_get_count(dungeons); j++) {
        price_containers[j].key_price = 0.0;
        price_containers[j].bone_price = 0.0;
        price_containers[j].bone_one_price = 0.0;
        price_containers[j].bone_two_price = 0.0;
        price_containers[j].log_price = 0.0;
        price_containers[j].ore_price = 0.0;
        price_containers[j].flower_price = 0.0;
        price_containers[j].fish_price = 0.0;
        price_containers[j].gemstone_one_price = 0.0;
        price_containers[j].gemstone_two_price = 0.0;
    }

    market_data = json_object(market_root_value);
    listings = json_object_get_array(market_data, "listings");

    for (i = 0; i < json_array_get_count(listings); i++) {
        listing = json_array_get_object(listings, i);
        id = strtol(json_object_get_string(listing, "itemId"), NULL, 10);
        /* Type 1 == Buy */
        /* Type 2 == Orders */
        market_order_type = strtol(json_object_get_string(listing, "type"), NULL, 10);
        cost = json_object_get_number(listing, "cost");

        for (n = 0; n < json_array_get_count(dungeons); n++) {
            dungeon = json_array_get_object(dungeons, n);
            drops = json_object_get_array(dungeon, "drops");
        
            /* Keys */
            if (json_object_dotget_number(dungeon, "key.id") == id) {
                if (price_containers[n].key_price == 0.0) {
                    price_containers[n].key_price = cost;
                } else {
                    if (market_order_type == 1) {
                        if (cost < price_containers[n].key_price || cost > price_containers[n].key_price) {
                            price_containers[n].key_price = cost;
                        }
                    } else if (market_order_type == 2) {
                        if (cost > price_containers[n].key_price) {
                            price_containers[n].key_price = cost;
                        }
                    }
                } 
            }

            for (j = 0; j < json_array_get_count(drops); j++) {
                drop = json_array_get_object(drops, j);
                key = json_object_get_string(drop, "key");

                if(strcmp(key, "gold") == 0) 
                    continue;
                    
                if (json_object_get_number(drop, "id") == id) {
                    if (strcmp(key, "bone") == 0) {
                        if (cost > MARKET_COST_OVERFLOW)
                            continue;

                        if (price_containers[n].bone_price == 0.0) {
                            price_containers[n].bone_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].bone_price || cost > price_containers[n].bone_price) {
                                    price_containers[n].bone_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].bone_price) {
                                    price_containers[n].bone_price = cost;
                                }
                            }
                        } 
                    } else if (strcmp(key, "bone_one") == 0) {
                        if (cost > MARKET_COST_OVERFLOW)
                            continue;

                        if (price_containers[n].bone_one_price == 0.0) {
                            price_containers[n].bone_one_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].bone_one_price || cost > price_containers[n].bone_one_price) {
                                    price_containers[n].bone_one_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].bone_one_price) {
                                    price_containers[n].bone_one_price = cost;
                                }
                            }
                        } 
                    } else if (strcmp(key, "bone_two") == 0) {
                        if (cost > MARKET_COST_OVERFLOW)
                            continue;

                        if (price_containers[n].bone_two_price == 0.0) {
                            price_containers[n].bone_two_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].bone_two_price || cost > price_containers[n].bone_two_price) {
                                    price_containers[n].bone_two_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].bone_two_price) {
                                    price_containers[n].bone_two_price = cost;
                                }
                            }
                        } 
                    } else if (strcmp(key, "log") == 0) {
                        if (cost > MARKET_COST_OVERFLOW)
                            continue;

                        if (price_containers[n].log_price == 0.0) {
                            price_containers[n].log_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].log_price || cost > price_containers[n].log_price) {
                                    price_containers[n].log_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].log_price) {
                                    price_containers[n].log_price = cost;
                                }
                            }
                        } 
                    } else if (strcmp(key, "ore") == 0) {
                        if (cost > MARKET_COST_OVERFLOW)
                            continue;

                        if (price_containers[n].ore_price == 0.0) {
                            price_containers[n].ore_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].ore_price || cost > price_containers[n].ore_price) {
                                    price_containers[n].ore_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].ore_price) {
                                    price_containers[n].ore_price = cost;
                                }
                            }
                        }
                    } else if (strcmp(key, "flower") == 0) {
                        if (cost > MARKET_COST_OVERFLOW)
                            continue;

                        if (price_containers[n].flower_price == 0.0) {
                            price_containers[n].flower_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].flower_price || cost > price_containers[n].flower_price) {
                                    price_containers[n].flower_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].flower_price) {
                                    price_containers[n].flower_price = cost;
                                }
                            }
                        }
                    } else if (strcmp(key, "fish") == 0) {
                        if (cost > MARKET_COST_OVERFLOW)
                            continue;

                        if (price_containers[n].fish_price == 0.0) {
                            price_containers[n].fish_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].fish_price || cost > price_containers[n].fish_price) {
                                    price_containers[n].fish_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].fish_price) {
                                    price_containers[n].fish_price = cost;
                                }
                            }
                        }
                    } else if (strcmp(key, "gemstone_one") == 0) {
                        if (price_containers[n].gemstone_one_price == 0.0) {
                            price_containers[n].gemstone_one_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].gemstone_one_price || cost > price_containers[n].gemstone_one_price) {
                                    price_containers[n].gemstone_one_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].gemstone_one_price) {
                                    price_containers[n].gemstone_one_price = cost;
                                }
                            }
                        }
                    } else if (strcmp(key, "gemstone_two") == 0) {
                        if (price_containers[n].gemstone_two_price == 0.0) {
                            price_containers[n].gemstone_two_price = cost;
                        } else {
                            if (market_order_type == 1) {
                                if (cost < price_containers[n].gemstone_two_price || cost > price_containers[n].gemstone_two_price) {
                                    price_containers[n].gemstone_two_price = cost;
                                }
                            } else if (market_order_type == 2) {
                                if (cost > price_containers[n].gemstone_two_price) {
                                    price_containers[n].gemstone_two_price = cost;
                                }
                            }
                        }
                    }
                }
            }
        }        
    }
    
    if (serialize_sim_data(sim_root_value, price_containers) == -1) {
        fprintf(stderr, "%s", "Failure in serialize_sim_data()\n");
        return -1;
    }

    free(price_containers);
    json_value_free(market_root_value);
    return 0;
}

int serialize_sim_data(JSON_Value *sim_root_value, struct PriceContainer *price_containers) {
    JSON_Object *game_values;
    JSON_Array *dungeons;
    JSON_Object *dungeon;
    JSON_Array *drops;
    JSON_Object *drop;
    size_t i;
    size_t j;
    char buffer[50];
    const char* key;
    const char* tag;  

    game_values = json_value_get_object(sim_root_value); 
    dungeons = json_object_get_array(game_values, "dungeons");

    for (i = 0; i < json_array_get_count(dungeons); i++) {
        dungeon = json_array_get_object(dungeons, i);
        drops = json_object_get_array(dungeon, "drops");

        /* Key */
        tag = json_object_dotget_string(dungeon, "key.tag");
        snprintf(buffer, 50, "%s%s%s", "keys.", tag, ".value");
        json_object_dotset_number(game_values, buffer, price_containers[i].key_price);

        for (j = 0; j < json_array_get_count(drops); j++) {
            drop = json_array_get_object(drops, j);
            key = json_object_get_string(drop, "key");
            tag = json_object_get_string(drop, "tag");

            if (strcmp(key, "bone") == 0) {
                snprintf(buffer, 50, "%s%s%s", "bones.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].bone_price);
            } else if (strcmp(key, "bone_one") == 0) {
                snprintf(buffer, 50, "%s%s%s", "bones.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].bone_one_price);
            } else if (strcmp(key, "bone_two") == 0) {
                snprintf(buffer, 50, "%s%s%s", "bones.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].bone_two_price);
            } else if (strcmp(key, "log") == 0) {
                snprintf(buffer, 50, "%s%s%s", "logs.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].log_price);
            } else if (strcmp(key, "ore") == 0) {
                snprintf(buffer, 50, "%s%s%s", "ores.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].ore_price);
            } else if (strcmp(key, "flower") == 0) {
                snprintf(buffer, 50, "%s%s%s", "flowers.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].flower_price);
            } else if (strcmp(key, "fish") == 0) {
                snprintf(buffer, 50, "%s%s%s", "fishes.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].fish_price);
            } else if (strcmp(key, "gemstone_one") == 0) {
                snprintf(buffer, 50, "%s%s%s", "gemstones.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].gemstone_one_price);
            } else if (strcmp(key, "gemstone_two") == 0) {
                snprintf(buffer, 50, "%s%s%s", "gemstones.", tag, ".value");
                json_object_dotset_number(game_values, buffer, price_containers[i].gemstone_two_price);
            }
        }
    }

    if (json_serialize_to_file_pretty(sim_root_value, SIMULATION_VALUES_JSON) != JSONSuccess) {
        fprintf(stderr, "Failed to write updated JSON\n");
        return -1;
    }

    return 0;
}