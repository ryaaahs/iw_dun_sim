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

make run MARKET_PARSE=1 DISPLAY_TYPE=100
build/iw_dun_sim ./json/simulation_values.json 1 100
|--------------------------------------------------------------|
Usage
	Key                       Value
	-----                     -----
	Level                     100
	Simulated Hours           1
	Rolls                     498
	Double Loot Procs         27
	Total Keys                56
	Total Keys Preserved      4
|--------------------------------------------------------------|
Consumed
	Item                      Value
	-----                     -----
	Pie                       87,360 / hour
	Potion                    37,044 / hour
	Sigil                     2,912 / hour
	Keys                      87,880 / hour
|--------------------------------------------------------------|
Loot (Total)
	Item                      Amount          Gold
	-----                     -----           ----------
	Gold                      165,102         165,102 coins
	Giant Fang                424             39,856 coins
	Giant Bone                66              5,742 coins
	Large Fang                82              10,250 coins
	Ancient Log               150             13,200 coins
	Infernal Ore              88              6,688 coins
	Snapdragon                117             7,488 coins
	King Crab                 130             8,970 coins
	Moonstone                 1               840 coins
	Onyx                      6               5,760 coins
|--------------------------------------------------------------|
Profits
Gold/hour (Produced): 263,896
Gold/hour (Equipment): 215,196
Gold/hour (Total): 48,700
|--------------------------------------------------------------|
```