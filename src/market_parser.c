#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "market_parser.h"
#include "parson.h"

#define MARKET_COST_OVERFLOW 500

typedef struct PriceContainer PriceContainer;

typedef struct MarketItem MarketItem;

int serialize_sim_data(JSON_Value *sim_root_value, PriceContainer *price_containers);

int reset_sim_data(JSON_Value *sim_root_value);

int compare_bone_prices(PriceContainer *price_containers, MarketItem item, const char* key, int index);

int compare_skill_material_prices(PriceContainer *price_containers, MarketItem item, const char* key, int index);

int compare_sub_material_prices(PriceContainer *price_containers, MarketItem item, const char* key, int index);

void compare_item_prices(JSON_Array *dungeons, PriceContainer *price_containers, MarketItem item);


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

struct MarketItem {
    int market_parse_type; 
    int market_order_type; 
    double item_price;
    int id;
};

int parse_market_data(JSON_Value *sim_root_value, int market_parse_type) {
    struct PriceContainer *price_containers;
    struct MarketItem item;
    JSON_Object *game_values;
    JSON_Array *dungeons;
    
    JSON_Value *market_root_value;
    JSON_Object *market_data;
    JSON_Array *listings;
    JSON_Object *listing;
    size_t i;
    size_t j;
    
    /* Game will not display values over a certian value for items */
    /* TODO: Figure out the formula that is used in game to determine this value for items */
     
    if (access(MARKET_DATA_JSON, F_OK) != 0) {
        fprintf(stderr, "File not found in JSON folder: %s\n", MARKET_DATA_JSON);
        return -1;
    }
    
    reset_sim_data(sim_root_value);

    market_root_value = json_parse_file(MARKET_DATA_JSON);

    if (json_value_get_type(market_root_value) != JSONObject) {
        fprintf(stderr, "%s", "Market JSON - Incorrect JSON, expecting JSONObject\n");
        return -1;
    }

    game_values = json_value_get_object(sim_root_value);
    dungeons = json_object_get_array(game_values, "dungeons");
    price_containers = malloc(json_array_get_count(dungeons) * sizeof(struct PriceContainer)); 
    item.market_parse_type = market_parse_type;

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
        item.id = strtol(json_object_get_string(listing, "itemId"), NULL, 10);
        item.market_order_type = strtol(json_object_get_string(listing, "type"), NULL, 10);
        item.item_price = json_object_get_number(listing, "cost");
        
        compare_item_prices(dungeons, price_containers, item);
    }
    
    if (serialize_sim_data(sim_root_value, price_containers) == -1) {
        fprintf(stderr, "%s", "Failure in serialize_sim_data()\n");
        return -1;
    }

    free(price_containers);
    json_value_free(market_root_value);
    return 0;
}

void compare_item_prices(JSON_Array *dungeons, PriceContainer *price_containers, MarketItem item) {
    const char *key;
    JSON_Object *dungeon;
    JSON_Array *drops;
    JSON_Object *drop;
    size_t n;
    size_t j;

    for (n = 0; n < json_array_get_count(dungeons); n++) {
        dungeon = json_array_get_object(dungeons, n);
        drops = json_object_get_array(dungeon, "drops");
    
        /* Keys */
        if (json_object_dotget_number(dungeon, "key.id") == item.id) {
            if (price_containers[n].key_price == 0.0) {
                price_containers[n].key_price = item.item_price;
            } 

            if (item.market_parse_type == LOWEST_LISTING) {
                if (item.market_order_type == SELL_ORDER) {
                    if (item.item_price < price_containers[n].key_price || item.item_price > price_containers[n].key_price) {
                        price_containers[n].key_price = item.item_price;
                    }
                } else if (item.market_order_type == BUY_ORDER) {
                    if (item.item_price > price_containers[n].key_price) {
                        price_containers[n].key_price = item.item_price;
                    }
                }
            } else if(item.market_parse_type == ONLY_SELL_ORDER) {
                if (item.market_order_type == 1 && item.item_price < price_containers[n].key_price) {
                    price_containers[n].key_price = item.item_price;
                }
            } else if(item.market_parse_type == ONLY_BUY_ORDER) {
                if (item.market_order_type == 2 && item.item_price > price_containers[n].key_price) {
                    price_containers[n].key_price = item.item_price;
                }
            }

        }
        
        for (j = 0; j < json_array_get_count(drops); j++) {
            drop = json_array_get_object(drops, j);
            key = json_object_get_string(drop, "key");

            if(strcmp(key, "gold") == 0) 
                continue;

            if (json_object_get_number(drop, "id") == item.id) {
                if (compare_bone_prices(price_containers, item, key, n) == 1) continue;
                else if (compare_skill_material_prices(price_containers, item, key, n) == 1) continue;
                else if (compare_sub_material_prices(price_containers, item, key, n) == 1) continue;
            }
        }
    }
}

int compare_bone_prices(PriceContainer *price_containers, MarketItem item, const char* key, int index) { 
    
    if (strcmp(key, "bone") == 0) {
        if (item.item_price > MARKET_COST_OVERFLOW)
            return 1;
        
        if (item.market_parse_type == 1) {
            if (price_containers[index].bone_price == 0) {
                price_containers[index].bone_price = item.item_price;
                return 1; 
            }

            if (item.market_order_type == 1) {
                if (item.item_price < price_containers[index].bone_price || item.item_price > price_containers[index].bone_price) {
                    price_containers[index].bone_price = item.item_price;
                    return 1; 
                }
            } else if (item.market_order_type == 2) {
                if (item.item_price > price_containers[index].bone_price) {
                    price_containers[index].bone_price = item.item_price;
                    return 1; 
                }
            }
        } else if(item.market_parse_type == 2 && item.market_order_type == 1) {

            if (price_containers[index].bone_price == 0) {
                price_containers[index].bone_price = item.item_price;
                return 1; 
            } else if (item.item_price < price_containers[index].bone_price) {
                price_containers[index].bone_price = item.item_price;
                return 1; 
            }
        } else if(item.market_parse_type == 3 && item.market_order_type == 2) {

            if (price_containers[index].bone_price == 0) {
                price_containers[index].bone_price = item.item_price;
                return 1; 
            } else if (item.item_price > price_containers[index].bone_price) {
                price_containers[index].bone_price = item.item_price;
                return 1; 
            }
        }

    } else if (strcmp(key, "bone_one") == 0) {
        if (item.item_price > MARKET_COST_OVERFLOW)
            return 1;

        if (item.market_parse_type == 1) {
            if (price_containers[index].bone_one_price == 0.0) {
                price_containers[index].bone_one_price = item.item_price;
                return 1; 
            }

            if (item.market_order_type == 1) {
                if (item.item_price < price_containers[index].bone_one_price || item.item_price > price_containers[index].bone_one_price) {
                    price_containers[index].bone_one_price = item.item_price;
                    return 1; 
                }
            } else if (item.market_order_type == 2) {
                if (item.item_price > price_containers[index].bone_one_price) {
                    price_containers[index].bone_one_price = item.item_price;
                    return 1; 
                }
            }
        } else if(item.market_parse_type == 2 && item.market_order_type == 1) {

            if (price_containers[index].bone_one_price == 0.0) {
                price_containers[index].bone_one_price = item.item_price;
                return 1; 
            }else if ( item.item_price < price_containers[index].bone_one_price) {
                price_containers[index].bone_one_price = item.item_price;
                return 1; 
            }
        } else if(item.market_parse_type == 3 && item.market_order_type == 2) {

            if (price_containers[index].bone_one_price == 0.0) {
                price_containers[index].bone_one_price = item.item_price;
                return 1; 
            } else if ( item.item_price > price_containers[index].bone_one_price) {
                price_containers[index].bone_one_price = item.item_price;
                return 1; 
            }
        }

    } else if (strcmp(key, "bone_two") == 0) {
        if (item.item_price > MARKET_COST_OVERFLOW)
            return 1;
        
        if (item.market_parse_type == 1) {

            if (price_containers[index].bone_two_price == 0.0) {
                price_containers[index].bone_two_price = item.item_price;
                return 1; 
            } 

            if (item.market_order_type == 1) {
                if (item.item_price < price_containers[index].bone_two_price || item.item_price > price_containers[index].bone_two_price) {
                    price_containers[index].bone_two_price = item.item_price;
                    return 1; 
                }
            } else if (item.market_order_type == 2) {
                if (item.item_price > price_containers[index].bone_two_price) {
                    price_containers[index].bone_two_price = item.item_price;
                    return 1; 
                }
            }
        } else if(item.market_parse_type == 2 && item.market_order_type == 1) {

            if (price_containers[index].bone_two_price == 0.0) {
                price_containers[index].bone_two_price = item.item_price;
                return 1; 
            } else if (item.item_price < price_containers[index].bone_two_price) {
                price_containers[index].bone_two_price = item.item_price;
                return 1; 
            }
        } else if(item.market_parse_type == 3 && item.market_order_type == 2) {
            
            if (price_containers[index].bone_two_price == 0.0) {
                price_containers[index].bone_two_price = item.item_price;
                return 1; 
            } else if (item.item_price > price_containers[index].bone_two_price) {
                price_containers[index].bone_two_price = item.item_price;
                return 1; 
            }
        }

    }
    return 0;
}

int compare_skill_material_prices(PriceContainer *price_containers, MarketItem item, const char* key, int index) {
    
    if (strcmp(key, "log") == 0) {
        if (item.item_price > MARKET_COST_OVERFLOW)
            return 1;

        if (item.market_parse_type == LOWEST_LISTING) {
            if (price_containers[index].log_price == 0.0) {
                price_containers[index].log_price = item.item_price;
                return 1; 
            }

            if (item.market_order_type == SELL_ORDER) {
                if (item.item_price < price_containers[index].log_price || item.item_price > price_containers[index].log_price) {
                    price_containers[index].log_price = item.item_price;
                    return 1; 
                }
            } else if (item.market_order_type == BUY_ORDER) {
                if (item.item_price > price_containers[index].log_price) {
                    price_containers[index].log_price = item.item_price;
                    return 1; 
                }
            }
        } else if(item.market_parse_type == ONLY_SELL_ORDER && item.market_order_type == SELL_ORDER) {

            if (price_containers[index].log_price == 0.0) {
                price_containers[index].log_price = item.item_price;
                return 1; 
            } else if (item.item_price < price_containers[index].log_price) {
                price_containers[index].log_price = item.item_price;
                return 1; 
            }
        } else if(item.market_parse_type == ONLY_BUY_ORDER && item.market_order_type == BUY_ORDER) {
            
            if (price_containers[index].log_price == 0.0) {
                price_containers[index].log_price = item.item_price;
                return 1; 
            } else if (item.item_price > price_containers[index].log_price) {
                price_containers[index].log_price = item.item_price;
                return 1; 
            }
        }

    } else if (strcmp(key, "ore") == 0) {
        if (item.item_price > MARKET_COST_OVERFLOW)
            return 1;

        if (item.market_parse_type == LOWEST_LISTING) {
            if (price_containers[index].ore_price == 0.0) {
                price_containers[index].ore_price = item.item_price;
                return 1; 
            }

            if (item.market_order_type == SELL_ORDER) {
                if (item.item_price < price_containers[index].ore_price || item.item_price > price_containers[index].ore_price) {
                    price_containers[index].ore_price = item.item_price;
                    return 1; 
                }
            } else if (item.market_order_type == BUY_ORDER) {
                if (item.item_price > price_containers[index].ore_price) {
                    price_containers[index].ore_price = item.item_price;
                    return 1; 
                }
            }
        } else if(item.market_parse_type == ONLY_SELL_ORDER && item.market_order_type == SELL_ORDER) {

            if (price_containers[index].ore_price == 0.0) {
                price_containers[index].ore_price = item.item_price;
                return 1; 
            } else if (item.item_price < price_containers[index].ore_price) {
                price_containers[index].ore_price = item.item_price;
                return 1; 
            }
        } else if(item.market_parse_type == ONLY_BUY_ORDER && item.market_order_type == BUY_ORDER) {

            if (price_containers[index].ore_price == 0.0) {
                price_containers[index].ore_price = item.item_price;
                return 1; 
            } else if (item.item_price > price_containers[index].ore_price) {
                price_containers[index].ore_price = item.item_price;
                return 1; 
            }
        }

    } else if (strcmp(key, "flower") == 0) {
        if (item.item_price > MARKET_COST_OVERFLOW)
            return 1;

        if (item.market_parse_type == LOWEST_LISTING) {
            if (price_containers[index].flower_price == 0.0) {
                price_containers[index].flower_price = item.item_price;
                return 1; 
            }

            if (item.market_order_type == SELL_ORDER) {
                if (item.item_price < price_containers[index].flower_price || item.item_price > price_containers[index].flower_price) {
                    price_containers[index].flower_price = item.item_price;
                    return 1; 
                }
            } else if (item.market_order_type == BUY_ORDER) {
                if (item.item_price > price_containers[index].flower_price) {
                    price_containers[index].flower_price = item.item_price;
                    return 1; 
                }
            }
        } else if(item.market_parse_type == ONLY_SELL_ORDER && item.market_order_type == SELL_ORDER) {
            
            if (price_containers[index].flower_price == 0.0) {
                price_containers[index].flower_price = item.item_price;
                return 1; 
            } else if (item.item_price < price_containers[index].flower_price) {
                price_containers[index].flower_price = item.item_price;
                return 1; 
            }
        } else if(item.market_parse_type == ONLY_BUY_ORDER && item.market_order_type == BUY_ORDER) {
            
            if (price_containers[index].flower_price == 0.0) {
                price_containers[index].flower_price = item.item_price;
                return 1; 
            } else if ( item.item_price > price_containers[index].flower_price) {
                price_containers[index].flower_price = item.item_price;
                return 1; 
            }
        }

    } else if (strcmp(key, "fish") == 0) {
        if (item.item_price > MARKET_COST_OVERFLOW)
            return 1;

        if (item.market_parse_type == LOWEST_LISTING) {
            if (price_containers[index].fish_price == 0.0) {
                price_containers[index].fish_price = item.item_price;
                return 1; 
            } 

            if (item.market_order_type == SELL_ORDER) {
                if (item.item_price < price_containers[index].fish_price || item.item_price > price_containers[index].fish_price) {
                    price_containers[index].fish_price = item.item_price;
                    return 1; 
                }
            } else if (item.market_order_type == BUY_ORDER) {
                if (item.item_price > price_containers[index].fish_price) {
                    price_containers[index].fish_price = item.item_price;
                    return 1; 
                }
            }
        } else if(item.market_parse_type == ONLY_SELL_ORDER && item.market_order_type == SELL_ORDER) {

            if (price_containers[index].fish_price == 0.0) {
                price_containers[index].fish_price = item.item_price;
                return 1; 
            } else if (item.item_price < price_containers[index].fish_price) {
                price_containers[index].fish_price = item.item_price;
                return 1; 
            }
        } else if(item.market_parse_type == ONLY_BUY_ORDER && item.market_order_type == BUY_ORDER) {
            
            if (price_containers[index].fish_price == 0.0) {
                price_containers[index].fish_price = item.item_price;
                return 1; 
            } else if (item.item_price > price_containers[index].fish_price) {
                price_containers[index].fish_price = item.item_price;
                return 1; 
            }
        }
    }

    return 0;
}

int compare_sub_material_prices(PriceContainer *price_containers, MarketItem item, const char* key, int index) {
    
    if (strcmp(key, "gemstone_one") == 0) {
        if (item.market_parse_type == LOWEST_LISTING) {
            if (price_containers[index].gemstone_one_price == 0.0) {
                price_containers[index].gemstone_one_price = item.item_price;
            }

            if (item.market_order_type == SELL_ORDER) {
                if (item.item_price < price_containers[index].gemstone_one_price || item.item_price > price_containers[index].gemstone_one_price) {
                    price_containers[index].gemstone_one_price = item.item_price;
                }
            } else if (item.market_order_type == BUY_ORDER) {
                if (item.item_price > price_containers[index].gemstone_one_price) {
                    price_containers[index].gemstone_one_price = item.item_price;
                }
            }
        } else if(item.market_parse_type == ONLY_SELL_ORDER && item.market_order_type == SELL_ORDER) {
            
            if (price_containers[index].gemstone_one_price == 0.0) {
                price_containers[index].gemstone_one_price = item.item_price;
            } else if (item.item_price < price_containers[index].gemstone_one_price) {
                price_containers[index].gemstone_one_price = item.item_price;
            }
        } else if(item.market_parse_type == ONLY_BUY_ORDER && item.market_order_type == BUY_ORDER) {
            
            if (price_containers[index].gemstone_one_price == 0.0) {
                price_containers[index].gemstone_one_price = item.item_price;
            } else if (item.item_price > price_containers[index].gemstone_one_price) {
                price_containers[index].gemstone_one_price = item.item_price;
            }
        }

    } else if (strcmp(key, "gemstone_two") == 0) {
        
        if (item.market_parse_type == LOWEST_LISTING) {
            if (price_containers[index].gemstone_two_price == 0.0) {
                price_containers[index].gemstone_two_price = item.item_price;
            } 

            if (item.market_order_type == SELL_ORDER) {
                if (item.item_price < price_containers[index].gemstone_two_price || item.item_price > price_containers[index].gemstone_two_price) {
                    price_containers[index].gemstone_two_price = item.item_price;
                }
            } else if (item.market_order_type == BUY_ORDER) {
                if (item.item_price > price_containers[index].gemstone_two_price) {
                    price_containers[index].gemstone_two_price = item.item_price;
                }
            }
        } else if(item.market_parse_type == ONLY_SELL_ORDER && item.market_order_type == SELL_ORDER) {
            
            if (price_containers[index].gemstone_two_price == 0.0) {
                price_containers[index].gemstone_two_price = item.item_price;
            } else if (item.item_price < price_containers[index].gemstone_two_price) {
                price_containers[index].gemstone_two_price = item.item_price;
            }
        } else if(item.market_parse_type == ONLY_BUY_ORDER && item.market_order_type == BUY_ORDER) {
            
            if (price_containers[index].gemstone_two_price == 0.0) {
                price_containers[index].gemstone_two_price = item.item_price;
            } else if (item.item_price > price_containers[index].gemstone_two_price) {
                price_containers[index].gemstone_two_price = item.item_price;
            }
        }
    }

    return 0;
}

int serialize_sim_data(JSON_Value *sim_root_value, PriceContainer *price_containers) {
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

// Refactor this into serialize
int reset_sim_data(JSON_Value *sim_root_value) {
    JSON_Object *game_values;
    
    game_values = json_value_get_object(sim_root_value);

    // Keys
    json_object_dotset_number(game_values, "keys.dungeon_key_25.value", 0);
    json_object_dotset_number(game_values, "keys.dungeon_key_40.value", 0);
    json_object_dotset_number(game_values, "keys.dungeon_key_55.value", 0);
    json_object_dotset_number(game_values, "keys.dungeon_key_70.value", 0);
    json_object_dotset_number(game_values, "keys.dungeon_key_100.value", 0);

    // Logs
    json_object_dotset_number(game_values, "logs.pine_log.value", 0);
    json_object_dotset_number(game_values, "logs.spruce_log.value", 0);
    json_object_dotset_number(game_values, "logs.birch_log.value", 0);
    json_object_dotset_number(game_values, "logs.teak_log.value", 0);
    json_object_dotset_number(game_values, "logs.mahogany_log.value", 0);
    json_object_dotset_number(game_values, "logs.ironbark_log.value", 0);
    json_object_dotset_number(game_values, "logs.redwood_log.value", 0);
    json_object_dotset_number(game_values, "logs.ancient_log.value", 0);

    // Ores
    json_object_dotset_number(game_values, "ores.copper_ore.value", 0);
    json_object_dotset_number(game_values, "ores.iron_ore.value", 0);
    json_object_dotset_number(game_values, "ores.silver_ore.value", 0);
    json_object_dotset_number(game_values, "ores.gold_ore.value", 0);
    json_object_dotset_number(game_values, "ores.cobalt_ore.value", 0);
    json_object_dotset_number(game_values, "ores.obsidian_ore.value", 0);
    json_object_dotset_number(game_values, "ores.astral_ore.value", 0);
    json_object_dotset_number(game_values, "ores.infernal_ore.value", 0);

    // Flowers
    json_object_dotset_number(game_values, "flowers.peony.value", 0);
    json_object_dotset_number(game_values, "flowers.tulip.value", 0);
    json_object_dotset_number(game_values, "flowers.rose.value", 0);
    json_object_dotset_number(game_values, "flowers.daisy.value", 0);
    json_object_dotset_number(game_values, "flowers.lilac.value", 0);
    json_object_dotset_number(game_values, "flowers.hyacinth.value", 0);
    json_object_dotset_number(game_values, "flowers.nemesia.value", 0);
    json_object_dotset_number(game_values, "flowers.snapdragon.value", 0);

    // Fishes
    json_object_dotset_number(game_values, "fishes.raw_shrimp.value", 0);
    json_object_dotset_number(game_values, "fishes.raw_cod.value", 0);
    json_object_dotset_number(game_values, "fishes.raw_salmon.value", 0);
    json_object_dotset_number(game_values, "fishes.raw_bass.value", 0);
    json_object_dotset_number(game_values, "fishes.raw_lobster.value", 0);
    json_object_dotset_number(game_values, "fishes.raw_swordfish.value", 0);
    json_object_dotset_number(game_values, "fishes.raw_shark.value", 0);
    json_object_dotset_number(game_values, "fishes.king_crab.value", 0);

    // Gemstones
    json_object_dotset_number(game_values, "gemstones.ruby.value", 0);
    json_object_dotset_number(game_values, "gemstones.topaz.value", 0);
    json_object_dotset_number(game_values, "gemstones.emerald.value", 0);
    json_object_dotset_number(game_values, "gemstones.amethyst.value", 0);
    json_object_dotset_number(game_values, "gemstones.citrine.value", 0);
    json_object_dotset_number(game_values, "gemstones.diamond.value", 0);
    json_object_dotset_number(game_values, "gemstones.moonstone.value", 0);
    json_object_dotset_number(game_values, "gemstones.onyx.value", 0);

    // Bones
    json_object_dotset_number(game_values, "bones.bone.value", 0);
    json_object_dotset_number(game_values, "bones.fang.value", 0);
    json_object_dotset_number(game_values, "bones.medium_bone.value", 0);
    json_object_dotset_number(game_values, "bones.medium_fang.value", 0);
    json_object_dotset_number(game_values, "bones.large_bone.value", 0);
    json_object_dotset_number(game_values, "bones.large_fang.value", 0);
    json_object_dotset_number(game_values, "bones.giant_bone.value", 0);
    json_object_dotset_number(game_values, "bones.giant_fang.value", 0);

    if (json_serialize_to_file_pretty(sim_root_value, SIMULATION_VALUES_JSON) != JSONSuccess) {
        fprintf(stderr, "Failed to write updated JSON\n");
        return -1;
    }

    return 0;
}