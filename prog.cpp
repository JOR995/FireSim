#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>

using namespace std;

enum CellState { Empty, Tree, Burning, Burnt }; //used for the state of each cell within the forest array
enum Direction { Centre, North, East, South, West }; //Used for the fire start location and wind direction parameters

//Class used for each entry in the forestGrid array, each object of the class contains x & y coords and the state for its entry in the array
class Cell
{
	int coordX;
	int coordY;
	CellState state;
	bool changed;

public:
	//Constructor assigns values to variables when object is created using passed parameters
	Cell(int yCoord, int xCoord, CellState type)
	{
		this->coordY = yCoord;
		this->coordX = xCoord;
		this->state = type;
		this->changed = false;
	}

	//Public get and set accessors for private variables to be called from other classes
	int getCoordX()
	{
		return coordX;
	}

	int getCoordY()
	{
		return coordY;
	}

	int getState()
	{
		return state;
	}

	void setState(CellState type)
	{
		state = type;
	}

	bool getChanged()
	{
		return changed;
	}

	void setChanged(bool value)
	{
		changed = value;
	}
};


//Class used to create and update the forestGrid, 2d array used to get details for each tree in the forest stored as an entry in the array
class ForestGrid
{
	//2d array of pointers to objects of class Cell
	Cell* forestCells[21][21];
	//Enum Direction used for startLocation and windDirection, also for limiting the available values for these variables
	Direction startLocation;
	Direction windDirection;
	short unsigned int burnChance;
	bool active;

public:
	//Constructor for class, takes stored inputs from user to assign values to some of its private variables
	ForestGrid(int fireChance, int startDirection, int wind)
	{
		//Used to give the srand() function a new seed. This is used to generate different, more randomized sets of numbers used for the fire spread
		srand(time(NULL));
		//Sets the simulation to be active, otherwise simulation would end before the fire even starts
		active = true;
		//Assigns these variables from the user inputs passed through as parameters
		burnChance = fireChance;
		startLocation = (Direction)startDirection;
		windDirection = (Direction)wind;

		//Goes through the 2d array for the first time and creates a new Cell object for each entry in the array
		//Passes Cell objects their x and y coordinates and sets them all to be trees except a single space permiter which are set to empty
		for (int x = 0; x < 21; x++)
		{
			for (int y = 0; y < 21; y++)
			{
				if (x == 0 || x == 21 - 1)
				{
					forestCells[x][y] = new Cell(y, x, Empty);
				}
				else if (y == 0 || y == 21 - 1)
				{
					forestCells[x][y] = new Cell(y, x, Empty);
				}
				else
				{
					forestCells[x][y] = new Cell(y, x, Tree);
				}
			}
		}

		//Uses the start location for the fire in a switch to determine which single Cell has its state changed to burning
		switch (startLocation)
		{
		case Centre:
			forestCells[10][10]->setState(Burning);
			break;
		case North:
			forestCells[1][10]->setState(Burning);
			break;
		case East:
			forestCells[10][19]->setState(Burning);
			break;
		case South:
			forestCells[19][10]->setState(Burning);
			break;
		case West:
			forestCells[10][1]->setState(Burning);
			break;
		default:
			forestCells[10][10]->setState(Burning);
			break;
		}
	}

	//Called with every time frame to update each Cell in the 2d array
	void updateArray()
	{
		int numBurning = 0;

		//Goes through the array and uses the state of each cell within a switch statement
		for (int x = 0; x < 21; x++)
		{
			for (int y = 0; y < 21; y++)
			{
				switch (forestCells[x][y]->getState())
				{
					//If the cell is burning it has a chance to change the state of its four neighbours to burning also
					//First checks that this burning cell was not set alight within this time step, this prevents trees that have been set to burning also lighting other trees all within a single time step
				case Burning:
					if (!forestCells[x][y]->getChanged())
					{
						if (forestCells[x - 1][y]->getState() == Tree)
						{
							//Generates a random number between 1 and 100
							int random = rand() % 100 + 1;

							//Then takes the wind direction into account
							//If wind direction is in the same direction as the neighbour then the chance of fire spread is increased by adding to the random number
							//If wind is in opposite direction as the neighbour then the chance is reduced by subtracting from random number
							//If wind direction is perpendicular to neighbour direction then chance is reduced but by less
							//If there is no wind then the generated random number is not affected
							if (windDirection == North)
							{
								random -= 20;
							}
							else if (windDirection == South)
							{
								random += 20;
							}
							else if (windDirection == East || windDirection == West)
							{
								random += 10;
							}
							//Then sees if the neighbour will catch fire by comparing the random number to the user inputted burn chance
							//If the random number is less than or equal to burn chance that the neighbour will have its state changed to burning
							//Otherwise the neighbour's state does not change
							if (random <= burnChance)
							{
								forestCells[x - 1][y]->setState(Burning);
								//Sets changed bool to true, so that this tree cannot also set others on fire within this time step
								//Also increments number of burning trees
								forestCells[x - 1][y]->setChanged(true);
								numBurning++;
							}
						}
						if (forestCells[x][y + 1]->getState() == Tree)
						{
							int random = rand() % 100 + 1;

							if (windDirection == East)
							{
								random -= 20;
							}
							else if (windDirection == West)
							{
								random += 20;
							}
							else if (windDirection == North || windDirection == South)
							{
								random += 10;
							}

							if (random <= burnChance)
							{
								forestCells[x][y + 1]->setState(Burning);
								forestCells[x][y + 1]->setChanged(true);
								numBurning++;
							}
						}
						if (forestCells[x + 1][y]->getState() == Tree)
						{
							int random = rand() % 100 + 1;

							if (windDirection == South)
							{
								random -= 20;
							}
							else if (windDirection == North)
							{
								random += 20;
							}
							else if (windDirection == East || windDirection == West)
							{
								random += 10;
							}

							if (random <= burnChance)
							{
								forestCells[x + 1][y]->setState(Burning);
								forestCells[x + 1][y]->setChanged(true);
								numBurning++;
							}
						}
						if (forestCells[x][y - 1]->getState() == Tree)
						{
							int random = rand() % 100 + 1;

							if (windDirection == West)
							{
								random -= 20;
							}
							else if (windDirection == East)
							{
								random += 20;
							}
							else if (windDirection == North || windDirection == South)
							{
								random += 10;
							}

							if (random <= burnChance)
							{
								forestCells[x][y - 1]->setState(Burning);
								forestCells[x][y - 1]->setChanged(true);
								numBurning++;

							}
						}
						//After checking all neighbours the state of this cell is then changed to burnt
						forestCells[x][y]->setState(Burnt);
						forestCells[x][y]->setChanged(true);
					}
					break;
				}
			}
		}
		//Each tree that is set on fire in this time step increments the numBurning variable, as long as this is greater than 0 the simulation continues
		if (numBurning == 0) active = false;
	}

	//Called to return the 2d array
	Cell* (*getArray())[21]
	{
		return forestCells;
	}

		bool getActive()
	{
		return active;
	}

	//Class destructor, when called destorys all Cell objects within the 2d array
	~ForestGrid()
	{
		for each (Cell* cell in forestCells)
		{
			//_Destroy(cell);
		}
	}
};


//Class for taking the 2d array from the ForestGrid class and displaying it to the user using ASCII symbols
class DisplayGrid
{
	char displayCells[21][21];

public:
	//No-argument constructor called at the start of the program
	DisplayGrid() {}

	//Uses the 2d array passed as a parameter to convert the state of each cell into a single symbol
	void updateForest(Cell* forestCells[][21])
	{
		for (int x = 0; x < 21; x++)
		{
			for (int y = 0; y < 21; y++)
			{
				//Resets the changed bool for each cell back to false so they can be changed within the next time step
				forestCells[x][y]->setChanged(false);

				//Uses the state of each cell in the passed 2d array to change the char entry in the same position within the displayCells 2d array
				switch (forestCells[x][y]->getState())
				{
				case Empty:
					displayCells[x][y] = { '.' };
					break;
				case Tree:
					displayCells[x][y] = { '!' };
					break;
				case Burning:
					displayCells[x][y] = { '#' };
					break;
				case Burnt:
					displayCells[x][y] = { '~' };
					break;
				default:
					break;
				}
			}
		}
	}

	//The displayCells array is then displayed to the user through the console as a 21 x 21 grid of symbols
	void displayForest()
	{
		cout << endl;
		for (int x = 0; x < 21; x++)
		{
			for (int y = 0; y < 21; y++)
			{
				cout << displayCells[x][y] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
};


//Class used to start the programer and create objects for ForestGrid and DisplayGrid classes
class Simulator
{
	short unsigned int burnChance, startDirection, windDirection;

	void startSim()
	{
		//Three private variables are assigned values using user inputs
		cout << "Fire Simulator:" << endl << "Key:" << endl;
		cout << "! = Tree" << endl << "# = Burning Tree" << endl << "~ = Burnt Tree" << endl << ". = Empty Space" << endl;
		system("PAUSE");

		cout << "\nEnter an integer between 0 - 100 for the probability of a tree catching fire: ";
		cin >> burnChance;
		if (burnChance < 0 || burnChance > 100) burnChance = 50;

		cout << "\nEnter a number corresponding to a starting location:" << endl;
		cout << "0 = Centre, 1 = North, 2 = East, 3 = South, 4 = West: ";
		cin >> startDirection;

		cout << "\nEnter a number corresponding to a wind direction:" << endl;
		cout << "0 = No Wind, 1 = North, 2 = East, 3 = South, 4 = West: ";
		cin >> windDirection;

		//ForestGrid object created and passed user input variables
		ForestGrid* forest = new ForestGrid(burnChance, startDirection, windDirection);
		//DisplayGrid object created using no-argument constructor
		DisplayGrid* displayGrid = new DisplayGrid();

		displayGrid->updateForest(forest->getArray());
		displayGrid->displayForest();
		system("PAUSE");

		//Calls methods from the forest and displayGird objects
		//Passes the 2d array from the forest object to the displayGrid
		//This is repeated while the simulation is active (has trees that are burning)
		while (forest->getActive())
		{
			forest->updateArray();
			displayGrid->updateForest(forest->getArray());
			displayGrid->displayForest();
			system("PAUSE");
		}

		//When the simulation ends the forest and displayGrid objects are deleted to free up memory
		delete(forest);
		delete(displayGrid);
	}

public:
	//No-argument contructor called from main, used to start simulation
	Simulator()
	{
		startSim();
	};

	//Called when simulation has finished, uses user input to either restart the simulator or close the program
	bool finishSim(bool restart)
	{
		char response;

		cout << "Simulation finished. Would you like to restart? Y/N ";
		cin >> response;

		if (response == 'Y' || response == 'y')
		{
			system("cls");
			restart = true;
		}
		else if (response == 'N' || response == 'n')
		{
			restart = false;
		}
		return restart;
	}
};


//Main used to create the first object, sim which starts the simulator
//This is repeated for as long the user chooses to restart the simulator upon it finishing 
int main(void)
{
	bool restart = true;

	while (restart)
	{
		Simulator sim;
		restart = sim.finishSim(restart);
	}

	return 0;
}