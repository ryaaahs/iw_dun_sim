#include <stdio.h>; 
#include "market_parser.h";
#include "parson.h";

struct dungeon_one_hundred {
    int ancient_log_price;
    int infernal_ore_price;
    int snapdragon_price;
    int king_crab_price;
    int moonstone_price;
    int onyx_price;
}; 

void parse_market_data() {
    JSON_Value *root_value;

    /* Get access to the json and confirm the root value is a json type */ 
    root_value = json_parse_file(MARKET_DATA_JSON);
    if (json_value_get_type(root_value) != JSONObject) {
        fprintf(stderr, "%s", "Incorrect JSON\n");
        return -1;
    }
}