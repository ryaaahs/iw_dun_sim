#include "parson.h"

enum d100_ids {
    ANCIENT_LOG_ID      = 17,
    INFERNAL_ORE_ID     = 28,
    SNAPDRAGON_ID       = 357,
    KING_CRAB_ID        = 342,
    MOONSTONE_ID        = 36,
    ONYX_ID             = 37
};

#define MARKET_DATA_JSON "./json/market_data.json"
#define SIMULATION_VALUES_JSON "./json/simulation_values.json"

int parse_market_data(JSON_Value *sim_root_value);
 
