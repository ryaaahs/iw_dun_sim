#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "market_parser.h"
#include "parson.h"

struct D100PriceContainer {
    double ancient_log_price;
    double infernal_ore_price;
    double snapdragon_price;
    double king_crab_price;
    double moonstone_price;
    double onyx_price;
}; 

int serialize_sim_data(JSON_Value *sim_root_value, struct D100PriceContainer *d100_pc);

int parse_market_data(JSON_Value *sim_root_value) {
    struct D100PriceContainer *d100_pc;
    JSON_Value *market_root_value;
    JSON_Object *market_data;
    JSON_Array *listings;
    JSON_Object *listing;
    size_t i;
    long int id;
    int type;
    double cost;
    
    if (access(MARKET_DATA_JSON, F_OK) != 0) {
        fprintf(stderr, "File not found in JSON folder: %s\n", MARKET_DATA_JSON);
        return 0;
    }
    
    market_root_value = json_parse_file(MARKET_DATA_JSON);

    if (json_value_get_type(market_root_value) != JSONObject) {
        fprintf(stderr, "%s", "Incorrect JSON, expecting JSONObject\n");
        return 0;
    }

    d100_pc = malloc(sizeof(struct D100PriceContainer));
    market_data = json_object(market_root_value);
    listings = json_object_get_array(market_data, "listings");

    for (i = 0; i < json_array_get_count(listings); i++) {
        listing = json_array_get_object(listings, i);
        id = strtol(json_object_get_string(listing, "itemId"), NULL, 10);
        /* Type 1 == Buy */
        /* Type 2 == Orders */
        type = strtol(json_object_get_string(listing, "type"), NULL, 10);
        cost = json_object_get_number(listing, "cost");

        /* TODO: Make this more generic with the other dungeons in the game */
        switch(id) {
            case ANCIENT_LOG_ID: 
                if((cost < d100_pc->ancient_log_price && type == 1) || (cost > d100_pc->ancient_log_price && type == 2)) {
                    d100_pc->ancient_log_price = cost;
                } else if (d100_pc->ancient_log_price == 0) {
                    d100_pc->ancient_log_price = cost;
                }
            break;
            case INFERNAL_ORE_ID: 
                if((cost < d100_pc->infernal_ore_price && type == 1) || (cost > d100_pc->infernal_ore_price && type == 2)) {
                    d100_pc->infernal_ore_price = cost;
                } else if (d100_pc->infernal_ore_price == 0) {
                    d100_pc->infernal_ore_price = cost;
                }
            break;
            case SNAPDRAGON_ID: 
                if((cost < d100_pc->snapdragon_price && type == 1) || (cost > d100_pc->snapdragon_price && type == 2)) {
                    d100_pc->snapdragon_price = cost;
                } else if (d100_pc->snapdragon_price == 0) {
                    d100_pc->snapdragon_price = cost;
                }
            break;
            case KING_CRAB_ID: 
                if((cost < d100_pc->king_crab_price && type == 1) || (cost > d100_pc->king_crab_price && type == 2)) {
                    d100_pc->king_crab_price = cost;
                } else if (d100_pc->king_crab_price == 0) {
                    d100_pc->king_crab_price = cost;
                }
            break;
            case MOONSTONE_ID: 
                if((cost < d100_pc->moonstone_price && type == 1) || (cost > d100_pc->moonstone_price && type == 2)) {
                    d100_pc->moonstone_price = cost;
                } else if (d100_pc->moonstone_price == 0) {
                    d100_pc->moonstone_price = cost;
                }
            break;
            case ONYX_ID: 
                if((cost < d100_pc->onyx_price && type == 1) || (cost > d100_pc->onyx_price && type == 2)) {
                    d100_pc->onyx_price = cost;
                } else if (d100_pc->onyx_price == 0) {
                    d100_pc->onyx_price = cost;
                }
            break;
        }        
    }
    
    printf("al:%f\n", d100_pc->ancient_log_price);
    printf("io:%f\n", d100_pc->infernal_ore_price);
    printf("sd:%f\n", d100_pc->snapdragon_price);
    printf("kc:%f\n", d100_pc->king_crab_price);
    printf("ms:%f\n", d100_pc->moonstone_price);
    printf("o:%f\n", d100_pc->onyx_price);

    if (serialize_sim_data(sim_root_value, d100_pc) == -1) {
        fprintf(stderr, "%s", "Failure in serialize_sim_data()\n");
        return 0;
    }

    free(d100_pc);
    json_value_free(market_root_value);
    return 0;
}

int serialize_sim_data(JSON_Value *sim_root_value, struct D100PriceContainer *d100_pc) {
    JSON_Object *game_values = json_value_get_object(sim_root_value); 

    json_object_dotset_number(game_values, "logs.ancient_log.value", d100_pc->ancient_log_price);
    json_object_dotset_number(game_values, "ores.infernal_ore.value", d100_pc->infernal_ore_price);
    json_object_dotset_number(game_values, "flowers.snapdragon.value", d100_pc->snapdragon_price);
    json_object_dotset_number(game_values, "fishes.king_crab.value", d100_pc->king_crab_price);
    json_object_dotset_number(game_values, "gemstones.moonstone.value", d100_pc->moonstone_price);
    json_object_dotset_number(game_values, "gemstones.onyx.value", d100_pc->onyx_price);

    if (json_serialize_to_file_pretty(sim_root_value, SIMULATION_VALUES_JSON) != JSONSuccess) {
        fprintf(stderr, "Failed to write updated JSON\n");
    }

    return 0;
}