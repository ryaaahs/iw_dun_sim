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
make help
build/iw_dun_sim json/simulation_values.json
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

make run
build/iw_dun_sim json/simulation_values.json 1 100
|--------------------------------------------------------------|
Usage
	Key                       Value
	-----                     -----
	Level                     100
	Simulated Hours           168
	Rolls (AVG Hours)         499
	Rolls                     83,976
	Double Loot Procs         4,588
	Total Keys                9,408
	Total Keys Preserved      585
|--------------------------------------------------------------|
Consumed
	Item                      Gold Per Hour
	-----                     -----
	Pie                       87,360 / hour
	Potion                    37,044 / hour
	Sigil                     2,912 / hour
	Keys                      87,704 / hour
|--------------------------------------------------------------|
Loot (AVG Hours)
	Item                      Amount          Gold Per Hour
	-----                     -----           ----------
	Gold                      1               170,994 / hour
	Giant Fang                419             36,079 / hour
	Giant Bone                80              6,754 / hour
	Large Fang                80              9,549 / hour
	Ancient Log               108             9,662 / hour
	Infernal Ore              107             7,714 / hour
	Snapdragon                107             6,547 / hour
	King Crab                 106             8,304 / hour
	Moonstone                 3               3,210 / hour
	Onyx                      4               3,874 / hour
Total Gold (AVG Hours): 262,691
|--------------------------------------------------------------|
Loot (Total)
	Item                      Amount          Total Gold
	-----                     -----           ----------
	Gold                      28,727,144      28,727,144 coins
	Giant Fang                70,481          6,061,366 coins
	Giant Bone                13,509          1,134,756 coins
	Large Fang                13,481          1,604,239 coins
	Ancient Log               18,239          1,623,271 coins
	Infernal Ore              18,001          1,296,072 coins
	Snapdragon                18,033          1,100,013 coins
	King Crab                 17,886          1,395,108 coins
	Moonstone                 642             539,280 coins
	Onyx                      678             650,880 coins
Total Gold: 44,132,129
|--------------------------------------------------------------|
Profits
Gold/hour (Produced): 262,691
Gold/hour (Equipment): 215,020
Gold/hour (Total): 47,670
|--------------------------------------------------------------|
```