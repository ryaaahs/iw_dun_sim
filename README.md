# iw_dun_sim

# About
C CLI application to simulate the loot drops for IWRPG Dungeons

## Requirements
GNU Make  

## Source Usage:  
Create your own copy of `simulation_values_template.json` and rename to `simulation_values.json` within /json  
Update the values within the `simulation_values.json` file  
Pull market data from ironwoodrpg.com `getMarketData` API call and paste response into a new `market_data.json` file in /json   
Simulation values that need to be updated:  
    -config  
    -player  
    -dungeon `monsters_hour` and `food_hour` for each entry    
Once the simulation.json is fleshed out you can run the simulation!  
First build the project using `make`, this will create the build folder with objects/executable used to run the simulation.  
From there you can run the project using `make run`, default is `market_parse=1; display_type=100;`  
You can change the options by providing arguments like so `make run MARKET_PARSE=2 DISPLAY_TYPE=0`  

## Release Usage:
Download either the windows or linux build from the latest release  
Unzip the contents  
Rename `simulation_values_template.json` to `simulation_values.json` within /json  
Update the values within the `simulation_values.json` file  
Pull market json data from ironwoodrpg.com `getMarketData` API call and paste response into a new `market_data.json` file in /json   
Simulation values that need to be updated:  
    -config  
    -player  
    -dungeon `monsters_hour` and `food_hour` for each entry  
Open command prompt or terminal and run the application  

Windows sample: Run the `ironwood_dungeon_simulation.bat` file  
Linux sample: `./iw_dun_sim ./json/simulation_values.json 2 100`  

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
	Simulated Hours           24
	Rolls (AVG Hours)         643
	Rolls                     15,444
	Double Loot Procs         1,134
	Total Keys                1,440
	Total Keys Preserved      83
|--------------------------------------------------------------|
Consumed
	Item                      Gold Per Hour
	-----                     -----
	Pie                       69,400 / hour
	Potion                    30,889 / hour
	Sigil                     23,128 / hour
	Keys                      74,748 / hour
|--------------------------------------------------------------|
Loot (AVG Hours)
	Item                      Amount          Gold Per Hour
	-----                     -----           ----------
	Gold                      1               219,947 / hour
	Giant Fang                537             24,176 / hour
	Giant Bone                105             6,744 / hour
	Large Fang                107             7,284 / hour
	Ancient Log               145             8,878 / hour
	Infernal Ore              130             6,777 / hour
	Snapdragon                132             6,604 / hour
	King Crab                 138             8,566 / hour
	Moonstone                 4               3,990 / hour
	Onyx                      5               5,360 / hour
Total Gold (AVG Hours): 298,328
|--------------------------------------------------------------|
Loot (Total)
	Item                      Amount          Total Gold
	-----                     -----           ----------
	Gold                      5,278,747       5,278,747 coins
	Giant Fang                12,894          580,230 coins
	Giant Bone                2,529           161,856 coins
	Large Fang                2,571           174,828 coins
	Ancient Log               3,493           213,073 coins
	Infernal Ore              3,128           162,656 coins
	Snapdragon                3,170           158,500 coins
	King Crab                 3,316           205,592 coins
	Moonstone                 114             95,760 coins
	Onyx                      134             128,640 coins
Total Gold: 7,159,882
|--------------------------------------------------------------|
Profits
Gold/hour (Produced): 298,328
Gold/hour (Equipment): 198,165
Gold/hour (Total): 100,162
|--------------------------------------------------------------|
```