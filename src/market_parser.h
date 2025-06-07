#include "parson.h"

#define MARKET_DATA_JSON "./json/market_data.json"
#define SIMULATION_VALUES_JSON "./json/simulation_values.json"

#define SELL_ORDER 1
#define BUY_ORDER 2

#define LOWEST_LISTING 1
#define ONLY_SELL_ORDER 2
#define ONLY_BUY_ORDER 3

int parse_market_data(JSON_Value *sim_root_value, int market_parse_type);
