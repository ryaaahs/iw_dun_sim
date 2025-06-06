# iw_dun_sim

# About
C CLI application to simulate the loot drops for IWRPG Dungeons

## Requirements
GNU Make  

## Usage:  
Create your own copy of `simulation_values_template.json` and rename to `simulation_values.json` within /json  
Update the values within the `simulation_values.json` file  
Pull market data from ironwoodrpg.com `getMarketData` API call and paste response into a new `market_data.json` file in /json   
Values that need to be updated:  
    -config  
    -player  
    -dungeon `monsters_hour` and `food_hour` for each entry    

Once the simulation.json is fleshed out you can run the simulation!  
First build the project using `make`, this will create the build folder with objects/executable used to run the simulation.  
From there you can run the project using `make run`, default is `market_parse=1; display_type=100;`  
You can change the options by providing arguments like so `make run MARKET_PARSE=2 DISPLAY_TYPE=0`  

## Example Output:
```
./build/iw_dun_sim ./json/simulation_values.json
NAME
	iw_dun_sim - Dungeon Simulator for IWRPG game
SYNOPSIS
	iw_dun_sim [SIM_JSON]... [MARKET_PARSE]... [DISPLAY_TYPE]
ARGUMENTS
	[SIM_JSON] - Simulation json file located in your json folder
	[MARKET_PARSE] - Options to filter the market data
		[0] - Skip Market Parse
		[1] - Lowest sellable value between Sell/Buy
		[2] - Only use Sell Order
		[3] - Only use Buy Order
	[DISPLAY_TYPE] - What Dungeons do you want to display
		[0] - All
		[25] - Dungeon 25
		[40] - Dungeon 40
		[55] - Dungeon 55
		[70] - Dungeon 70
		[85] - Dungeon 85
		[100] - Dungeon 100

./build/iw_dun_sim ./json/simulation_values.json 1 100
|-------------------------------|
Usage
	Key                       Value
	-----                     -----
	Level                     100
	Simulated Hours           24
	Rolls (AVG Hours)         457
	Rolls                     10,983
	Double Loot Procs         997
	Total Keys                2,664
	Total Keys Preserved      170
-------------------------------
Loot (AVG Hours)
	Item                      Amount          Gold Per Hour
	-----                     -----           ----------
	Gold                      1               240,889 / hour
	Giant Fang                383             47,937 / hour
	Giant Bone                75              8,175 / hour
	Large Fang                73              10,108 / hour
	Ancient Log               100             10,949 / hour
	Infernal Ore              95              8,142 / hour
	Snapdragon                94              9,289 / hour
	King Crab                 97              9,775 / hour
	Moonstone                 2               2,345 / hour
	Onyx                      3               3,560 / hour
-------------------------------
Loot (Total)
	Item                      Amount          Gold Per Hour
	-----                     -----           ----------
	Gold                      5,781,339       240,889 / hour
	Giant Fang                9,204           1,150,500 / hour
	Giant Bone                1,800           196,200 / hour
	Large Fang                1,758           242,604 / hour
	Ancient Log               2,411           262,799 / hour
	Infernal Ore              2,299           195,415 / hour
	Snapdragon                2,275           222,950 / hour
	King Crab                 2,346           234,600 / hour
	Moonstone                 67              56,280 / hour
	Onyx                      89              85,440 / hour
-------------------------------
// Placeholder - Will need to updated
Gold/hour (Produced): 351,171
Gold/hour (Equipment): 351,171
Gold/hour (Total): 351,171
-------------------------------

```