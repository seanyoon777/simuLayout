# simuLayout: Multi-agent Simulation Software for Crowd Crush Prevention

## Introduction
Last October, a devastating tragedy struck my hometown South Korea, where a fatal crowd crush incident claimed the lives of 159 young adults, out in the streets of Itaewon to celebrate Halloween. I was miles away at Stanford when the news reached me, but I couldn't shake off a sort of profound disquiet within myself. Well, they were mostly around my age, and had my friends and I been in Korea, we could have been one of them. The lingering feeling that I could've also lost a loved one, and the fact that someone's son or daughter had senselessly passed away, pushed me to delve deeper into the phenomena of crowd collapses.
<p align="center">
  <img src="images/itaewon.jpg" alt="Itaewon Tragedy." width="480"/>
  <br>News Coverage of the Itaewon Tragedy.
</p>
While diving into the topic, I came across a shockingly long list of such tragedies, from the 1990 Mecca tragedy in Saudi Arabia with 2,300 lost lives to the 1989 Hillsborough disaster that took 97. Shockingly, even events as familiar to us as Black Friday sales see similar incidents in large retail stores. 

Back in 2019, my friends and I had drafted a crowd collapse simulation paper using multi-agent simulations ([available HERE](https://www.seanyoonbio.com/_files/ugd/577e3b_b281bd087e224daca0bd597c9abb0c5f.pdf)) as part of a mathematical modelling competition. With the fresh pain of the recent Halloween incident, I was compelled to revisit our work and develop a more robust software to predict crowd movements in navigation environments (scenarios where agents have obstacles and destinations). 

## User Input and Example Output
The software will return a gif simulation in your directory, with a randomly generated SLURM-job-id-esque `unique_id` to avoid confusion. The simulation will look something like this: 
<p align="center">
  <img src="images/examplesimulation.png" alt="Example simulation" width="800"/>
  <br>Screenshots of Example Simulation (t = 120)
</p>
To do so, the inputs are as follows: 
- `-l` flag: A birds eye view of the store layout, in png, jpg, or dxf format. 
- `-s` flag: Store data file (elaborated below) in csv format. 
- `-d` flag: The destination to store the output gif file. 
- (Optional) `--nagents` flag: Specifies the number of agents. (Default 1000)
- (Optional) `--time` flag: Specifies the duration of the simulation, in seconds. (Default 100)
Note that the model utilizes a customized A* algorithm that accounts for collision detection and intra-agent path prediction, and thus the worst case time complexity is $O(b^d)$. Note that runtime will significantly get longer as the number of agents increase. 

### Store data
The software requires a very specific format of the store data, as it is used to dynamically predict the popularity of each item during a sale event. The columns are chosen as factors widely considered to impact item popularity, such as rating, discount, and scarcity. Specifically, it requires the columns `Item`, the names of each product; `Rating`, a user rating of the products (preferrably out of 5); `Regular Price` and `Discount Price`, the prices of each product prior to and during the event respectively; and `Quantity`, the available stocks for each product. An example dataset `store_data.csv` can be accessed in example_data. 

An example store layout png file, `layout.png`, can also be found in example_data, although it is not required that the input image be grayscale. 

## How does it work? 
In the initialization step, the simulation first creates a prediction of the popularity for each item given the product data. Then, the agents are initialized in an "entrance" area, where each agent is assigned a target product drawn from a Gaussian distribution that is modified to fit the products' popularities. Then, each agent iteratively searches for the optimal path using a customized A* algorithm that takes into account collision prevention with obstacles and agents. 

## Usage
This software is implemented in both C++ (experimental, faster but more prone to bugs) and Python. Either way, to use the software, 
1. Clone the repository onto your local machine: 
```
git clone git@github.com:seanyoon777/simuLayout.git simuLayout
```
You can replace the simuLayout with any directory path of your choice. 

### Python Version: 
2. Set up the Conda environment. Most of the requirements are widely used packages, such as scipy or pandas, except `fpdf`. So run either 
```
conda env create -f environment.yml
```
and run 
```
conda activate simuLayout
```
3. Navigate into the current directory. 
4. Run the command below on your terminal:
```
python main.py -l [layout file path] -s [store data file path] -d [directory you want to save your gif files]
```
### Experimental C++ Version: 
The libraries required for the C++ implementation are default libraries, so there isn't a separate docker file. 
2. Compile the files by running the command below on your terminal:
```
g++ main.cpp -I/headers -o simulation
```
3. Then run the command:
```
./simulation -l [path to layout] -s [path to store data] -d [path to destination] --nagents [number of agents] --time [simulation duration]
```

## Acknowledgements
The initial paper was drafted by Sean Yoon, Moonhyuk Chang, Yuree Oh, and Yeeun Park, with equal contribution. Also, the stb library used for the C++ implementation was developed by Sean Barrett ([available HERE](https://github.com/nothings/stb)). 