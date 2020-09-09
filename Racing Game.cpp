// Assignment 3.cpp: A program using the TL-Engine
//Nicholas Poole

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
#include <sstream>
using namespace std;
#include <iostream>

enum collisionSide { leftSide, rightSide, frontSide, backSide, noSide };
collisionSide SphereToBox(float pointX, float pointZ, float cubeXLength, float cubeZLength, float cubeXPos, float cubeZPos, float sphereRadius, float carOldX, float carOldZ);

bool SphereToSphere(float raceCarXPos, float raceCarZPos, float raceCarRadius, float objectXPos, float ObjectZPos, float objectRadius);

void resetCarPosition(IModel* raceCar, IModel* dummy1, IModel* dummy2, float carOldX, float carOldZ, float dummy1OldX, float dummy1OldZ, float dummy2OldX, float dummy2OldZ);

struct vector2D
{
	float x;
	float z;
};

vector2D scalar(float s, vector2D v)
{
	return { s * v.x, s * v.z };
}

vector2D sum3(vector2D v1, vector2D v2, vector2D v3)
{
	return{ v1.x + v2.x + v3.x, v1.z + v2.z + v3.z };
}


void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./Media");

	/**** Set up your scene here ****/

	const int kIsleQuantity = 80;										// Number if isles used in the game
	const int kIsleChainQuantity = 4;									// Number of 'chains' of walls used to combine collision boxes /2
	const int kIsleCornerQuantity = 4;                                  // Number of 'corner' blocks used in the game
	const int kWallQuantity = 80;										// Number of walls used in the game
	const int kCheckpointQuantity = 4;
	const int kCheckpointLegDummyQuantity = kCheckpointQuantity * 2;	// The number of checkpoint legs in the game (2 per checkpoint)
	const int kWaypointQuantity = 10;									// Number of dummys used to direct the AI car
	const int checkpointLegRadius = 1;									// radius of a checkpoint leg
	const int kLargeTankQuantity = 4;									// The number of large water tanks used in the game
	const int kSmallTankQuantity = 97;									// The number of small water tanks used in the game (Used for corners)
	const float kSmallTankRadius = 4.0f;								// Radius of a small water tank
	const float rotationSpeed = 0.5f;									// The speed at thich the car turns (Handling)
	const float dummyRadius = 2.0f;                                     // The radius of the dummy models parented to the Players racecar. (used for collisions)
	const float isleWidth = 5.35f;										// The width (x) of an isle block
	const float isleLength = 6.83f;										// The length (z) of an isle block
	const float isleCornerWidth = 10.0f;								// The width (x) of an isle corner block
	const float isleCornerLength = 9.9f;								// The length (z) of an isle corner block
	const float isleChainLength = 130.0f;                               // the length (z) of a chain of walls
	const float checkpointLength = 2.5f;								// the length (z) of a checkpoint
	const float checkpointWidth = 19.72f;								// the width (x) of a checkpoint
	//const int carRotationSpeed = 5;										
	//const int carRotationUpperLimit = 10;
	//const int carRotationLowerLimit = 0;
	//int carRotationCounter = 0;
	float carOldX = 0.0f;                                               // The X position of the car from the previous frame (used for repositioning in collisions)
	float carOldZ = 0.0f;												// The Z position of the car from the previous frame (used for repositioning in collisions)
	float dummy1OldX = 0.0f;											// The X position of the cars front dummy from the previous frame (used for repositioning in collisions)
	float dummy1OldZ = 0.0f;											// The Z position of the cars front dummy from the previous frame (used for repositioning in collisions)
	float dummy2OldX = 0.0f;											// The X position of the cars rear dummy from the previous frame (used for repositioning in collisions)
	float dummy2OldZ = 0.0f;											// The z position of the cars rear dummy from the previous frame (used for repositioning in collisions)

	int carHP = 100;													// The current hp of the car (starsting from 100 then decreasing)

	float cdTime = 3.5f;												// The starting value of the countdown timer (in seconds)
	float boostTimer = 3.0f;											// The time limit of the boost per usage (in seconds)
	float overheatTimer = 5.0f;											// the cars overheat timer (in seconds)

	int lapNumber = 0;													// The current number of laps completed
	int lapLimit = 2;													// The maximum number of laps

	int cameraRotationLimit = 20;										// The initial starting angle of the camera that is adjusted by the player
	int cameraUpperLimit = 80;											// The maximum angle of the camera
	int cameraLowerLimit = 0;											// The minimum angle of the camera

	bool boostEnabled = false;											// Boolean to check if boost function is enabled (true whilst boost button is held down)
	bool boostOverheated = false;										// Boolean to inform the game that the user has overheated the car by over using the boost
	bool thirdPerson = true;											// Boolean to check which camera mode the user is in (true while in third person)
	bool cameraFP = false;												// Boolean to check if the camera is in first person 

	const int half = 2;
	const int rightAngle = 90;
	const int mirror = 180;

	IMesh* checkpointMesh;
	IMesh* isleMesh;
	IMesh* wallMesh;
	IMesh* skyboxMesh;
	IMesh* raceCarMesh;
	IMesh* groundMesh;
	IMesh* dummyMesh;
	IMesh* floorMesh;
	IMesh* isleCrossMesh;
	IMesh* isleCornerMesh;
	IMesh* waterTankLargeMesh;
	IMesh* waterTankSmallMesh;

	IModel* checkpoint[kCheckpointQuantity];
	IModel* isle[kIsleQuantity];
	IModel* wall[kWallQuantity];
	IModel* skybox;
	IModel* raceCar;
	IModel* ground;
	IModel* dummy[2];												// Dummy1 is the front collision sphere of the racecar
	//IModel* dummy2;												// Dummy2 is the rear collision sphere of the racecar
	IModel* aiDummy1;											// AIDummy1 is the front collision sphere of the AI racecar
	IModel* aiDummy2;											// AIDummy2 is the rear collision sphere of the AI racecar
	//IModel* needleDummy;										
	IModel* floor;
	IModel* isleCross;
	IModel* isleCorner[kIsleCornerQuantity];
	IModel* waterTankLarge[kLargeTankQuantity];
	IModel* waterTankSmall[kSmallTankQuantity];
	IModel* checkpointLegDummy[kCheckpointLegDummyQuantity];
	IModel* aiPathDummy[kWaypointQuantity];
	IModel* aiRaceCar;
	IModel* cameraDummy;

	checkpointMesh = myEngine->LoadMesh("Checkpoint.x");
	isleMesh = myEngine->LoadMesh("IsleStraight.x");
	wallMesh = myEngine->LoadMesh("Wall.x");
	skyboxMesh = myEngine->LoadMesh("Skybox 07.x");
	raceCarMesh = myEngine->LoadMesh("race2.x");
	groundMesh = myEngine->LoadMesh("ground.x");
	dummyMesh = myEngine->LoadMesh("Dummy.x");
	floorMesh = myEngine->LoadMesh("Walkway.X");
	isleCrossMesh = myEngine->LoadMesh("IsleCross.X");
	isleCornerMesh = myEngine->LoadMesh("IsleCorner.X");
	waterTankLargeMesh = myEngine->LoadMesh("TankLarge1.x");
	waterTankSmallMesh = myEngine->LoadMesh("TankSmall2.x");

	const EKeyCode kQuitKey = Key_Escape;	    // Closes the game
	const EKeyCode kAccelerateKey = Key_W;		// Accelerates the car
	const EKeyCode kBreakKey = Key_S;			// Decelerates the car
	const EKeyCode kRightTurnKey = Key_D;		// Turns the car right
	const EKeyCode kLeftTurnKey = Key_A;		// Turns the car left
	const EKeyCode kStartKey = Key_Space;		// Starts the race
	const EKeyCode kcameraFPKey = Key_2;		// Sets the camera to First Person
	const EKeyCode kcameraTPKey = Key_1;		// Sets the camera to First Person
	const EKeyCode kBoostKey = Key_Space;		// Enables the boost feature
	const EKeyCode kCameraUpKey = Key_Up;		// Moves the camera forwards
	const EKeyCode kCameraDownKey = Key_Down;	// Moves the camera down
	const EKeyCode kCameraLeftKey = Key_Left;	// Moves the camera left
	const EKeyCode kCameraRightKey = Key_Right;	// Moves the camera right
	const EKeyCode kRestartKey = Key_R;			// Restarts the game once complete

	enum gameState { Start, Ready, Stage1, Stage2, Stage3, Stage4, Complete }; // Enum containing all possible states that the game can be in.
	gameState currentGameState = Start; //Initial state of the game, waiting for the user to start

	bool stageComplete[kCheckpointQuantity] = { false, false, false, false };												// Boolean values for all of the checkpoints, when the user passes one it is set to true
	bool aiPathwayCheckpoint[kWaypointQuantity] = { false, false, false, false, false, false, false, false, false, false }; // Boolean values for all AI waypoints, When the AI car reaches one it becomes true
	bool raceComplete = false; //Boolean to know if the race has been completed

	// X Coordinates of every isle in the game
	const float islePosX[kIsleQuantity]{ -15.0f, 15.0f, -15.0f ,15.0f, -15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f,
										 -15.0f, 15.0f, -15.0f, 15.0f ,-15.0f, 15.0f ,-15.0f, 15.0f ,-15.0f, 15.0f ,-15.0f, 15.0f ,-15.0f, 15.0f ,-15.0f, 15.0f ,-15.0f, 15.0f ,-15.0f, 15.0f,
										  -148.0f, -148.0f, -135.0f, -135.0f, -122.0f, -122.0f, -108.0f, -108.0f, -95.0f, -95.0f, -82.0f, -82.0f, -69.0f, -69.0f, -56.0f, -56.0f, -43.0f, -43.0f, -30.0f, -30.0f,
										  30.0f, 30.0f, 43.0f, 43.0f, 56.0f, 56.0f, 69.0f, 69.0f, 82.0f, 82.0f, 95.0f, 95.0f, 108.0f, 108.0f, 121.0f, 121.0f, 134.0f, 134.0f, 147.0f, 147.0f };
	// Y Coordinate of every isle in the game
	const float islePosY = 0.0f;
	// Z Coordinates of every isle in the game
	const float isleSPosZ[kIsleQuantity]{ 40.0f, 40.0f, 53.0f, 53.0f, 67.0f, 67.0f, 80.0f, 80.0f, 93.0f, 93.0f, 106.0f, 106.0f, 119.0f, 119.0f, 132.0f, 132.0f, 145.0f, 145.0f, 158.0f, 158.0f,
										  214.0f, 214.0f, 227.0f, 227.0f, 240.0f, 240.0f, 253.0f, 253.0f, 266.0f, 266.0f, 279.0f, 279.0f, 292.0f, 292.0f, 305.0f, 305.0f, 318.0f, 318.0f, 331.0f, 331.0f,
										  170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f,
										  170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f };

	// X Coordinates for every horizontal isle 'chain' in the game
	const float hIsleCollisionX[kIsleChainQuantity]{ -84.0f, -84.0f, 84.0f, 84.0f };
	// Y Coordinates for every isle 'chain' in the game
	const float isleCollisionY = 0.0f;
	// Z Coordinates for every horizontal isle 'chain' in the game
	const float hIsleCollisionZ[kIsleChainQuantity]{ 170.0f, 200.0f, 170.0f, 200.0f };
	// X Coordinates for every verticle isle 'chain' in the game
	const float vIsleCollisionX[kIsleChainQuantity]{ -15.0f, 15.0f, -15.0f, 15.0f };
	// Z Coordinates for every verticle isle 'chain' in the game
	const float vIsleCollisionZ[kIsleChainQuantity]{ 105.0f, 105.0f, 268.0f, 268.0f };

	// X Coordinates for every wall in the game
	const float wallPosX[kWallQuantity]{ -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f, -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f,
										 -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f, -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f, 15.0f, -15.0f, -15.0f, 15.0f,
										 -141.0f, -141.0f, -128.0f, -128.0f, -114.0f, -114.0f, -101.0f, -101.0f, -88.0f, -88.0f, -75.0f, -75.0f, -61.0f, -61.0f, -49.0f, -49.0f, -36.0f, -36.0f, -23.0f, -23.0f,
										  24.0f, 24.0f, 37.0f, 37.0f, 50.0f, 50.0f, 63.0f, 63.0f, 76.0f, 76.0f, 89.0f, 89.0f, 102.0f, 102.0f, 115.0f, 115.0f, 128.0f, 128.0f, 141.0f, 141.0f };
	// y Coordinates for every wall in the game
	const float wallPosY = 0.0f;
	// Z Coordinates for every wall in the game
	const float wallPosZ[kWallQuantity]{ 46.0f, 46.0f, 59.0f, 59.0f, 73.0f, 73.0f, 86.0f, 86.0f, 99.0f, 99.0f, 112.0f, 112.0f, 125.0f, 125.0f, 138.0f, 138.0f, 151.0f, 151.0f, 164.0f, 164.0f,
										207.0f, 207.0f, 220.0f, 220.0f, 233.0f, 233.0f, 246.0f, 246.0f, 259.0f, 259.0f, 272.0f, 272.0f, 285.0f, 285.0f, 298.0f, 298.0f, 311.0f, 311.0f, 324.0f, 324.0f,
										170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f,
										170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f, 170.0f, 200.0f };

	// X Position of every checkpoint in the game
	const float checkpointPosX[kCheckpointQuantity]{ 0.0f, 0.0f, -15.0f, 147.0f };
	// Y Position of every checkpoint in the game
	float checkpointPosY = 0.0f;
	// Z Position of every checkpoint in the game
	const float checkpointPosZ[kCheckpointQuantity]{ 170.0f, 330.0f, 186.0f, 186.0f };

	// X Position of every isle 'corner' in the game
	const float isleCornerPosX[kIsleCornerQuantity]{ -15.0f, 15.0f, -15.0f, 15.0f };
	// Y Position of every isle 'corner' in the game
	const float isleCornerPosY = 0.0f;
	// Z Position of every isle 'corner' in the game
	const float isleCornerPosZ[kIsleCornerQuantity]{ 171.0f, 171.0f, 201.0f, 201.0f };


	// Unused
	// X Position of every Large water tank in the game
	const float waterTankLargePosX[kLargeTankQuantity]{};
	// Y Position of every Large water tank in the game
	const float waterTankLargePosY = 0.0f;
	// Z Position of every Large water tank in the game
	const float waterTankLargePosZ[kLargeTankQuantity]{};

	// X Position of every Small water tank in the game
	const float waterTankSmallPosX[kSmallTankQuantity]{ 0.0f, -15.0f, -15.0f, -17.0f, -21.0f, -27.0f, -31.0f, -39.0f, -47.0f, -55.0f, 15.0f, 15.0f, 15.0f, 11.0f, 7.0f, 3.0f, -1.0f, -5.0f, -10.0f, -15.0f, -20.0f, -28.0f, -36.0f, -44.0f, -52.0f,
														-152.0f,-160.0f,-168.0f,-173.0f,-178.0f,-183.0f,-184.0f,-184.0f,-184.0f,-152.0f, -160.0f, -168.0f, -176.0f, -184.0f, -192.0f, -200.0f, -204.0f, -208.0f, -213.0f, -215.0f, -215.0f, -215.0f, -215.0f, -215.0f,
														 152.0f, 160.0f, 168.0f, 173.0f, 178.0f, 183.0f, 184.0f, 184.0f, 184.0f, 152.0f,  160.0f,  168.0f,  176.0f,  184.0f,  192.0f,  200.0f,  204.0f,  208.0f,  213.0f,  215.0f,  215.0f,  215.0f,  215.0f,  215.0f,
														 15.0f, 15.0f, 17.0f, 21.0f, 27.0f, 31.0f, 39.0f, 47.0f, 55.0f, -15.0f, -15.0f, -15.0f, -11.0f, -7.0f, -3.0f, 1.0f, 5.0f, 10.0f, 15.0f, 20.0f, 28.0f, 36.0f, 44.0f, 52.0f };
	// Y Position of every Small water tank in the game
	const float waterTankSmallPosY = 0.0f;
	// Z Position of every Small water tank in the game
	const float waterTankSmallPosZ[kSmallTankQuantity]{ 185.0f, 335.0f, 343.0f, 351.0f, 361.0f, 369.0f, 374.0f, 375.0f, 375.0f, 375.0f, 335.0f, 343.0f, 351.0f, 359.0f, 367.0f, 374.0f, 382.0f, 386.0f, 390.0f, 395.0f, 397.0f, 397.0f, 397.0f, 397.0f, 397.0f,
														200.0f, 200.0f, 202.0f, 206.0f, 212.0f, 216.0f, 224.0f, 232.0f, 240.0f, 170.0f, 170.0f, 170.0f, 174.0f, 178.0f, 182.0f, 186.0f, 190.0f, 195.0f, 200.0f, 205.0f, 213.0f, 221.0f, 229.0f, 237.0f,
														170.0f, 170.0f, 168.0f, 164.0f, 158.0f, 154.0f, 146.0f, 138.0f, 130.0f, 200.0f, 200.0f, 200.0f, 196.0f, 192.0f, 188.0f, 185.0f, 181.0f, 176.0f, 171.0f, 166.0f, 159.0f, 151.0f, 143.0f, 135.0f,
														42.0f,  34.0f,  26.0f,  16.0f, 8.0f   , 3.0f,   2.0f,   2.0f,   2.0f,   42.0f,  34.0f,  26.0f,   14.0f, 6.0f,  -1.0f,   -9.0f,  -13.0f,  -17.0f, -22.0f, -24.0f, -24.0f, -24.0f , -24.0f, -24.0f };


	// X Position of the AI pathfinding waypoints
	const float aiDummyPosX[kWaypointQuantity]{ 0.0f, 0.0f,  0.0f,  0.0f, -145.0f, -70.0f, 70.0f, 145.0f };
	// Y Position of the AI pathfinding waypoints
	const float aiDummyPosY = 0.0f;
	// Z Position of the AI pathfinding waypoints
	const float aiDummyPosZ[kWaypointQuantity]{ 00.0f, 160.0f, 280.0f, 324.0f, 195.0f, 195.0f, 195.0f, 195.0f };


	for (int i = 0; i < kIsleQuantity; i++)										// For loop to generate all Isles in the game
	{
		isle[i] = isleMesh->CreateModel(islePosX[i], islePosY, isleSPosZ[i]);	// Cycles through each Isle and gets the associated coordinates from the arrays.

		if (i >= (kIsleQuantity / half))											// If the Isle is in the second half of the array, rotate 90 degrees (first half is verticle Isles, second is horizontal Isles)
		{
			isle[i]->RotateY(rightAngle);
		}
	}

	for (int i = 0; i < kWallQuantity; i++)										// For loop to generate all Walls in the game
	{
		wall[i] = wallMesh->CreateModel(wallPosX[i], wallPosY, wallPosZ[i]);	// Cycles through each Wall and gets the associated coordinates from the arrays.
		if (i >= (kWallQuantity / half))
		{																		// If the Wall is in the second half of the array, rotate 90 degrees (first half is verticle Walls, second is horizontal Walls)
			wall[i]->RotateY(rightAngle);
		}
	}

	for (int i = 0; i < kCheckpointQuantity; i++)								// For loop to generate all Checkpoints in the game
	{																			// Cycles through each Checkpoint and gets the associated coordinates from the arrays.
		checkpoint[i] = checkpointMesh->CreateModel(checkpointPosX[i], checkpointPosY, checkpointPosZ[i]);
		if (i >= (kCheckpointQuantity / half))
		{
			checkpoint[i]->RotateY(rightAngle);											// If the Wall is in the second half of the array, rotate 90 degrees (first half is verticle Checkpoints, second is horizontal Checkpoints)
		}
	}

	for (int i = 0; i < kCheckpointQuantity; i++)								// For loop that cycles through all Checkpoints in the game
	{
		const float offset = 9.0f;												// The distance from the centre of the checkpoint to each leg

		if (i >= (kCheckpointQuantity / half))										// Checks if the checkpoints are in the first half of the array (horizontal facing)
		{																		// Generates the dummys used for collision in the horizontal checkpoint legs (2 each)
			checkpointLegDummy[i] = dummyMesh->CreateModel(checkpointPosX[i], checkpointPosY, checkpointPosZ[i] - offset);
			checkpointLegDummy[i + kCheckpointQuantity] = dummyMesh->CreateModel(checkpointPosX[i], checkpointPosY, checkpointPosZ[i] + offset);
		}
		else																	// Else if the checkpoints are in the second half of the array (verticle facing)
		{																		// Generates the dummys used for collision in the verticle checkpoint legs (2 each)
			checkpointLegDummy[i] = dummyMesh->CreateModel(checkpointPosX[i] - offset, checkpointPosY, checkpointPosZ[i]);
			checkpointLegDummy[i + kCheckpointQuantity] = dummyMesh->CreateModel(checkpointPosX[i] + offset, checkpointPosY, checkpointPosZ[i]);
		}

	}

	for (int i = 0; i < kIsleCornerQuantity; i++)								// For loop that generates all isle 'corners' in the game
	{																			// Cycles through each isle 'corner' and gets the associated coordinates from the arrays.
		isleCorner[i] = isleCornerMesh->CreateModel(isleCornerPosX[i], isleCornerPosY, isleCornerPosZ[i]);
	}

	isleCorner[0]->RotateY(-mirror);												// Rotates each isle 'corner' appropriately to make it face the right way
	isleCorner[1]->RotateY(rightAngle);
	isleCorner[2]->RotateY(-rightAngle);
	isleCorner[3]->RotateY(0);

	for (int i = 0; i < kSmallTankQuantity; i++)								// For loop that generates all small water tanks in the game
	{																			// Cycles through each small water tank and gets the associated coordinates from the arrays.
		waterTankSmall[i] = waterTankSmallMesh->CreateModel(waterTankSmallPosX[i], waterTankSmallPosY, waterTankSmallPosZ[i]);
	}

	waterTankSmall[0]->SetY(-5.0f);												// Reduces the y position of the first small water tank, sinking it into the floor
	waterTankSmall[0]->RotateX(10.0f);											// Rotates the first small water tank 35 degrees

	for (int i = 0; i < kWaypointQuantity; i++)                                 // For loop that generates all the AI waypoints in the game
	{																			// Cycles through each waypoint and gets the associated coordinates from the arrays.
		aiPathDummy[i] = dummyMesh->CreateModel(aiDummyPosX[i], aiDummyPosY, aiDummyPosZ[i]);
	}

	skybox = skyboxMesh->CreateModel(0.0f, -960.0f, 0.0f);						// Creates the sky box at the given coordinates
	raceCar = raceCarMesh->CreateModel(0.0f, 0.0f, 30.0f);						// Creates the users racecar at the given coordinates
	ground = groundMesh->CreateModel(0.0f, 0.0f, 0.0f);							// Creates the ground at the given coordinates
	dummy[0] = dummyMesh->CreateModel(0.0f, 0.0f, 3.0f);						// Creates the players cars front dummy at the given coordinates
	dummy[1] = dummyMesh->CreateModel(0.0f, 0.0f, -3.0f);						// Creates the players cars rear dummy at the given coordinates
	aiDummy1 = dummyMesh->CreateModel(0.0f, 0.0f, 3.0f);						// Creates the AI cars front dummy at the given coordinates
	aiDummy2 = dummyMesh->CreateModel(0.0f, 0.0f, -3.0f);						// Creates the AI cars rear dummy at the given coordinates
	floor = floorMesh->CreateModel(0.0f, 0.0f, 0.0f);							// Creates the floor at the giver coordinates
	aiRaceCar = raceCarMesh->CreateModel(0.0f, 0.0f, -20.0f);					// Creates the AI racecar at the given coordinates 
	cameraDummy = dummyMesh->CreateModel(0.0f, 0.0f, 0.0f);


	dummy[0]->AttachToParent(raceCar);											// Attatches the front dummy to the players car
	dummy[1]->AttachToParent(raceCar);											// Attatches the rear dummy to the players car
	aiDummy1->AttachToParent(aiRaceCar);										// Attatches the AI front dummy to the AI car
	aiDummy2->AttachToParent(aiRaceCar);										// Attatches the AI rear dummy to the AI car
	cameraDummy->AttachToParent(raceCar);

	ICamera* myCamera;															// Creates the camera
	myCamera = myEngine->CreateCamera(kManual);
	myCamera->AttachToParent(cameraDummy);										// Attatches the camera to the players racecar
	myCamera->Move(0, 10, -20);													// moves the camera back into third person view
	myCamera->RotateX(20);														// rotates the gamera 20 degrees to look down


	vector2D momentum{ 0.0f, 0.0f };                                            // The X and Z components of the Racecars momentum vector
	vector2D thrust{ 0.0f, 0.0f };												// The X and Z components of the Racecars thrust vector
	vector2D drag{ 0.0f, 0.0f };												// The X and Z components of the Racecars drag vector

	myEngine->Timer();															// Calls the timer for the first frame

	float matrix[4][4];
	float thrustFactor = 0.00f;
	const float thrustUpperLimit = 0.5f;
	const float thrustLowerLimit = 0.2f;
	const float dragCoefficient = -0.01f;

	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);					// Initializes the text used to display default text in the game
	IFont* cdFont = myEngine->LoadFont("Comic Sans MS", 136);					// Initializes the text ised to display the race countdown
	IFont* hpFont = myEngine->LoadFont("Comic Sans MS", 36);					// Initializes the text used to display the players cars HP
	IFont* boostFont = myEngine->LoadFont("Comic Sans MS", 136);				// Initializes the text used to show when the boost is active

	ISprite* needle = myEngine->CreateSprite("needle.png", 350, 650);			// Creates the sprite of the speedometers needle
	ISprite* speedo = myEngine->CreateSprite("speedo.png", 350, 620);			// Creates the sprite of the speedometer
	ISprite* backdrop = myEngine->CreateSprite("ui_backdrop.jpg", 300, 550);	// Creates the sprite of the text backdrop


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		carOldX = raceCar->GetX();												// Sets the race cars current X pos for use in collisions
		carOldZ = raceCar->GetZ();												// Sets the race cars current Z pos for use in collisions
		dummy1OldX = dummy[0]->GetX();											// Sets the race cars front dummys current X pos for use in collisions
		dummy1OldZ = dummy[0]->GetZ();											// Sets the race cars front dummys curremt Z pos for use in collisions
		dummy2OldX = dummy[1]->GetX();											// Sets the race cars rear dummys current X pos for use in collisions
		dummy2OldZ = dummy[1]->GetZ();											// Sets the race cars rear dummys curremt Z pos for use in collisions

		stringstream outText;													// Creates a string stream for outputting text
		int speedOutput = (fabs(momentum.z) + fabs(momentum.x)) * 100;			// Generates the speed readout by adding together the absoloute values of both momentum vectors
		outText << "Speed: " << speedOutput << "mph";							// Displays the speed on the screen
		myFont->Draw(outText.str(), 20, 20);
		outText.str("");

		outText << "HP: " << carHP;												// Displays the players HP on the screen
		hpFont->Draw(outText.str(), 20, 50);
		outText.str("");

		needle->SetX(350 + speedOutput * 6);									// Moves the needle of the speedometer to match the cars speed

		float frameTime = myEngine->Timer();									// generates the current frames frametime by calling the timer
		float gameSpeed = thrustFactor * frameTime;								// creates the speed of the game by multiplying framtime by thrust

		// Draw the scene
		myEngine->DrawScene();

		if (currentGameState == Start)											// Checks the state of the game
		{
			outText << "Hit Space To Start";									// Tells the user to hit space to begin the race
			myFont->Draw(outText.str(), 300, 550);								// Displays this on screen
			outText.str("");

			if (myEngine->KeyHit(kStartKey))									// Waits for the user to hit space to start the race
			{
				currentGameState = Ready;										// Changes the game state to ready
			}
		}
		if (currentGameState == Ready)
		{

			stringstream countDown;												// Initializes the countdown
			cdTime -= frameTime;												// Removes the frame time from the counter each frame
			countDown << (int)cdTime;											// Displays the amount of time remaining as a while number
			cdFont->Draw(countDown.str(), 625, 280);

			if (cdTime < 1)														// Checks to see if the countdown has ended
			{
				currentGameState = Stage1;										// Starts the race
			}
		}
		if (currentGameState == Stage1)											// Checks the state of the game
		{
			outText << "Go!";													// Outputs the word 'Go!' to the screen to show the race has begun
			myFont->Draw(outText.str(), 300, 550);
			outText.str("");
			if (stageComplete[0] == true)										// Checks to see if the player has passed the first checkpoint
			{
				currentGameState = Stage2;										// changes the game to the next stage
			}
		}
		if (currentGameState == Stage2)											// Checks the state of the game
		{
			outText << "Stage 1 Complete!";										// Displays a message to show the player has passed the first checkpoint
			myFont->Draw(outText.str(), 300, 550);
			outText.str("");
			if (stageComplete[1] == true)										// Checks to see if the player has passed the second checkpoint	
			{
				currentGameState = Stage3;
			}
		}
		if (currentGameState == Stage3)											// Checks the state of the game
		{
			outText << "Stage 2 Complete!";										// Displays a message to show the player has passed the second checkpoint
			myFont->Draw(outText.str(), 300, 550);
			outText.str("");
			if (stageComplete[2] == true)										// Checks to see if the player has passed the third checkpoint	
			{
				currentGameState = Stage4;										// Changes the game to the next stage
			}
		}
		if (currentGameState == Stage4)											// Checks the state of the game
		{
			outText << "Stage 3 Complete!";										// Displays a message to show the player has passed the third checkpoint
			myFont->Draw(outText.str(), 300, 550);
			outText.str("");
			if (stageComplete[3] == true)										// Checks to see if the player has passed the fourth checkpoint	
			{
				lapNumber++;													// Adds this lap to the counter
				if (lapNumber == lapLimit)										// Checks if the player has completed all laps
				{
					currentGameState = Complete;								// Changes the game to the complete stage
				}


				else
				{

					currentGameState = Stage1;									// Restarts the Lap

					for (int i = 0; i < kCheckpointQuantity; i++)
					{
						stageComplete[i] = false;								// Checkpoints are reset
					}
				}

			}

		}
		if (currentGameState == Complete)										// Checks to see if the race has been completed
		{
			outText << "Race Complete! Hit 'R' to Restart!";					// Displays a message telling the user that the race is over
			myFont->Draw(outText.str(), 300, 550);
			outText.str("");

			if (myEngine->KeyHit(kRestartKey))
			{
				currentGameState = Ready;
				lapNumber = 0;

				raceCar->SetPosition(0.0f, 0.0f, 30.0f);
				momentum = { 0.0f, 0.0f };
				raceCar->LookAt(checkpoint[0]);

				for (int i = 0; i < kCheckpointQuantity; i++)
				{
					stageComplete[i] = false;								// Checkpoints are reset
				}
			}
		}

		if (currentGameState != Start && currentGameState != Complete && currentGameState != Ready) // Checks to see if the game is in a state in which the car can move
		{
			const int thrustIncrease = 0.002f;

			//get the players cars facing vector
			raceCar->GetMatrix(&matrix[0][0]);
			vector2D facingVector = { matrix[2][0], matrix[2][2] };

			//Calculate cars thrust (Based on keyboard input(w))
			if (myEngine->KeyHeld(kAccelerateKey))          // Checks if the user is holding the accelerator
			{


				if (thrustFactor < thrustUpperLimit)        // Checks if the car has reached its top speed
				{
					thrustFactor = thrustFactor + 0.002f;	// Increases the cars speed slightly
				}
				thrust = scalar(gameSpeed, facingVector);   // Calculates thrust by scaling the facing vector by the game speed
			}
			else if (myEngine->KeyHeld(kBreakKey))          // Checks if the user is holding the break
			{
				if (thrustFactor > thrustLowerLimit)        // Checks if the user is going the maximum speed in reverse
				{
					thrustFactor = thrustFactor - 0.002f;	// Slows the care down slightly every frame, if there is no forward mvement, the car will begin to move backwards
				}
				thrust = scalar(-gameSpeed, facingVector);  // Calculates thrust by scaling the facing vector by the game speed
			}
			else
			{
				thrust = { 0.0f, 0.0f };                    // If neither key is held, the cars thrust is not changed 
			}
			if (myEngine->KeyHeld(kLeftTurnKey))            // Checks if the user is holding the left turn key
			{
				raceCar->RotateY(-rotationSpeed);			// Rotates the car slightly to the left
			}
			if (myEngine->KeyHeld(kRightTurnKey))			// Checks if the user is holding the right turn key
			{
				raceCar->RotateY(rotationSpeed);			// Rotates the car slightly to the right
			}

			boostEnabled = false;							// Initially sets the cars boost to false

			if (myEngine->KeyHeld(kBoostKey))				// Checks if the player is holding the boost key
			{
				boostTimer -= frameTime;					// Starts to remove frame tiome from the boost timer while in use
				boostEnabled = true;						// Sets boost enabled to true

				if (boostTimer > 0)							// Checks the boost is both in use and has not run out
				{
					thrust.x = thrust.x * 1.5;				// Amplifies the cars X thrust by 1.5
					thrust.z = thrust.z * 1.5;				// Amplifies the cars Z thrust by 1.5

					outText << "Boost Active";				// Displays a message so the user knows the boost is in use
					boostFont->Draw(outText.str(), 200, 550);
					outText.str("");
				}
			}
			if (boostEnabled == false && boostTimer > 0)    // Checks to see if the boost button has been released and not exceeded the timer
			{
				boostTimer = 3.0f;
			}
			if (boostTimer < 0)								// Checks if the boost has exceeded the timer and the user is still holding the key
			{
				boostOverheated = true;						// Sets the car to overheated
			}

			if (boostOverheated == true)					// Checks if the car has overheated
			{
				thrust.x = thrust.x / 2;					// Halves the cars X thrust vector
				thrust.z = thrust.z / 2;					// Halves the cars Z thrust vector
				//boostTimer += frameTime * (3 / 5);			// Recharges the boost timer back to its origional amount

				outText << "Boost Overheated";				// Displays a message to show the car has overheated
				myFont->Draw(outText.str(), 200, 550);
				outText.str("");

				overheatTimer -= frameTime;					// Starts a timer for the overheat

				if (overheatTimer < 0)						// Checks to see if the overheat timer has expired
				{
					boostOverheated = false;				// removes the overheat
					overheatTimer = 5.0f;					// resets the overheat timer
					boostTimer = 3.0f;						// resets the boost timer
				}
			}

			//calculate drag (done after key inputs)(based on previous momentum)

			drag = scalar(dragCoefficient, momentum);

			//calculate momentum (based on thrust, drag and previous momentum)

			momentum = sum3(momentum, thrust, drag);

			////////////////////////
			//Check for Collisions//
			////////////////////////

			// isle collisions

			for (int h = 0; h < 2; h++) // Nested for loop to check both the front and rear dummys for the players car
			{
				for (int i = 0; i < kIsleChainQuantity; i++) // For loop that cycles through all verticle wall 'chains' in the game
				{
					// Checks to see if the players racecar has collided with any of the vertical wall 'chains', if so this function will return which side has been collided with in the form of an enum
					collisionSide collision = SphereToBox(dummy[h]->GetX(), dummy[h]->GetZ(), isleWidth, isleChainLength, vIsleCollisionX[i], vIsleCollisionZ[i], dummyRadius, carOldX, carOldZ);

					if (collision == frontSide || collision == backSide) // Checks if the collision occured on the front or back side of the wall 'chain'
					{
						resetCarPosition(raceCar, dummy[0], dummy[1], carOldX, carOldZ, dummy1OldX, dummy1OldZ, dummy2OldX, dummy2OldZ); // Resets the X and Z positions of the car and its dummys to the previous frames

						momentum.z = -momentum.z; // Inverts the cars Z momentum

						carHP -= 1;               // Removes 1 from the cars total HP
					}
					else if (collision == leftSide || collision == rightSide) // Checks if the collision occured on the left or right side of the wall 'chain'
					{
						resetCarPosition(raceCar, dummy[0], dummy[1], carOldX, carOldZ, dummy1OldX, dummy1OldZ, dummy2OldX, dummy2OldZ); // Resets the X and Z positions of the car and its dummys to the previous frames

						momentum.x = -momentum.x; // Inverts the cars Z momentum

						carHP -= 1;				  // Removes 1 from the cars total HP
					}
				}

				for (int i = 0; i < kIsleChainQuantity; i++) // For loop that cycles through all horizontal wall 'chains' in the game
				{
					// Checks to see if the players racecar has collided with any of the horizontal wall 'chains', if so this function will return which side has been collided with in the form of an enum
					collisionSide collision = SphereToBox(dummy[h]->GetX(), dummy[h]->GetZ(), isleChainLength, isleWidth, hIsleCollisionX[i], hIsleCollisionZ[i], dummyRadius, carOldX, carOldZ);

					if (collision == frontSide || collision == backSide)  // Checks if the collision occured on the front or back side of the wall 'chain'
					{
						resetCarPosition(raceCar, dummy[0], dummy[1], carOldX, carOldZ, dummy1OldX, dummy1OldZ, dummy2OldX, dummy2OldZ); // Resets the X and Z positions of the car and its dummys to the previous frames

						momentum.z = -momentum.z; // Inverts the cars Z momentum

						carHP -= 1;				  // Removes 1 from the cars total HP
					}
					else if (collision == leftSide || collision == rightSide)
					{
						resetCarPosition(raceCar, dummy[0], dummy[1], carOldX, carOldZ, dummy1OldX, dummy1OldZ, dummy2OldX, dummy2OldZ); // Resets the X and Z positions of the car and its dummys to the previous frames

						momentum.x = -momentum.x; // Inverts the cars Z momentum
						carHP -= 1;				  // Removes 1 from the cars total HP
					}
				}
			}

			// Water Tower Collisions

			for (int h = 0; h < 2; h++)						 // Nested for loop to check both the front and rear dummys for the players car
			{
				for (int i = 0; i < kSmallTankQuantity; i++) // For loop that runs through every small water tank in the game
				{
					// Check to see if the racecar has collided with any of the water tanks
					if (SphereToSphere(dummy[h]->GetX(), dummy[h]->GetZ(), dummyRadius, waterTankSmall[i]->GetX(), waterTankSmall[i]->GetZ(), kSmallTankRadius))
					{
						resetCarPosition(raceCar, dummy[0], dummy[1], carOldX, carOldZ, dummy1OldX, dummy1OldZ, dummy2OldX, dummy2OldZ); // Resets the X and Z positions of the car and its dummys to the previous frames

						momentum.x -= momentum.x * 2;		// Inverts and amplifies the cars X momentum vector to make it bounce
						momentum.z -= momentum.z * 2;		// Inverts and amplifies the cars X momentum vector to make it bounce

						carHP -= 1;							// Removes 1 from the cars total HP
					}
				}
			}

			// checkpoint collisions

			// collisions for checkpoint legs

			for (int h = 0; h < 2; h++) // Nested for loop to check both the front and rear dummys for the players car
			{
				for (int i = 0; i < kCheckpointLegDummyQuantity; i++) // For loop that runs through every checkpoint leg in the game
				{
					// Check to see if the racecar has collided with any of the checkpoint legs
					if (SphereToSphere(dummy[h]->GetX(), dummy[h]->GetZ(), dummyRadius, checkpointLegDummy[i]->GetX(), checkpointLegDummy[i]->GetZ(), checkpointLegRadius))
					{
						resetCarPosition(raceCar, dummy[0], dummy[1], carOldX, carOldZ, dummy1OldX, dummy1OldZ, dummy2OldX, dummy2OldZ); // Resets the X and Z positions of the car and its dummys to the previous frames

						momentum.x -= momentum.x * 2;		// Inverts and amplifies the cars X momentum vector to make it bounce
						momentum.z -= momentum.z * 2;		// Inverts and amplifies the cars X momentum vector to make it bounce

						carHP -= 1;							// Removes 1 from the cars total HP
					}
				}
			}

			//move the hover car (based on new momentum)

			raceCar->Move(momentum.x, 0.0f, momentum.z);

			//checkpoint collisions

			// Checks to see if the racecar has passed through the first checkpoint
			collisionSide collision = SphereToBox(dummy[0]->GetX(), dummy[0]->GetZ(), checkpointWidth, checkpointLength, checkpointPosX[0], checkpointPosZ[0], dummyRadius, carOldX, carOldZ);

			if (collision != noSide)		// If there has been a collision
			{
				stageComplete[0] = true;	// Checkpoint is set to complete
			}


			if (stageComplete[0] == true)
			{
				// Checks to see if the racecar has passed through second the checkpoint
				collisionSide collision = SphereToBox(dummy[0]->GetX(), dummy[0]->GetZ(), checkpointWidth, checkpointLength, checkpointPosX[1], checkpointPosZ[1], dummyRadius, carOldX, carOldZ);

				if (collision != noSide)		// If there has been a collision
				{
					stageComplete[1] = true;	// Checkpoint is set to complete
				}
			}

			if (stageComplete[1] == true)
			{
				// Checks to see if the racecar has passed through the third checkpoint
				collisionSide collision = SphereToBox(dummy[0]->GetX(), dummy[0]->GetZ(), checkpointWidth, checkpointLength, checkpointPosX[2], checkpointPosZ[2], dummyRadius, carOldX, carOldZ);

				if (collision != noSide)		// If there has been a collision
				{
					stageComplete[2] = true;	// Checkpoint is set to complete
				}
			}

			if (stageComplete[2] == true)
			{
				// Checks to see if the racecar has passed through the fourth checkpoint
				collisionSide collision = SphereToBox(dummy[0]->GetX(), dummy[0]->GetZ(), checkpointWidth, checkpointLength, checkpointPosX[3], checkpointPosZ[3], dummyRadius, carOldX, carOldZ);

				if (collision != noSide)		// If there has been a collision
				{
					stageComplete[3] = true;	// Checkpoint is set to complete
				}
			}

			// AI Pathway dummy collisions

			for (int i = 0; i < kWaypointQuantity; i++) // Loop that runs through all ai pathfinding nodes
			{
				if (SphereToSphere(aiDummy1->GetX(), aiDummy1->GetZ(), dummyRadius, aiPathDummy[i]->GetX(), aiPathDummy[i]->GetZ(), dummyRadius)) // Checks to see if the AI car is colliding with any pathfinding dummys.
				{
					aiPathwayCheckpoint[i] = true; // If colliding with a dummy, the collision setting is set to true to show the car has reached this point.
				}
			}

			for (int i = 1; i < kWaypointQuantity; i++) // Loop that runs through all ai pathfinding nodes
			{
				if (aiPathwayCheckpoint[i - 1] == true)	// Checks if the previous pathfinding node has been reached ((starts from 1, checkpoint[0] is at the cars starting location))
				{
					aiRaceCar->LookAt(aiPathDummy[i]);  // If this node has been reached, the car will Look at the neck one				
				}
			}
			aiRaceCar->SetY(10);		// Used for testing the ai to see w hy it is dissapearing
			aiRaceCar->MoveLocalZ(0.1); // Moves the AI car in the direction it was looking.

			cout << aiRaceCar->GetX() << aiRaceCar->GetZ() << endl; // Outputs the AI cars coordinates to the console, used to track it in testing


			if (myEngine->KeyHit(kcameraTPKey))   // Checks to see if the user has selected third person mode.
			{
				cameraDummy->SetPosition(raceCar->GetX(), 0.0f, raceCar->GetZ()); // resets the position of the camera

				if (thirdPerson == false)         // Checks to see the game is not already in third person mode
				{
					thirdPerson = true;			  // Sets third person mode to true
					myCamera->Move(0, 18, -55);	  // Moves the camera into third person
				}
			}
			if (myEngine->KeyHit(kcameraFPKey))   // Checks to see if the user has selected first person mode.
			{
				if (thirdPerson == true)		  // Checks to see the game is not already in first person mode
				{
					thirdPerson = false;		  // Sets third person mode to false
					myCamera->Move(0, -18, 55);	  // Moves the camera into first person
				}
			}

			if (myEngine->KeyHit(kQuitKey))       // Checks to see if the player has hit escape
			{
				myEngine->Stop();				  // stops the game and ends the game loop.
			}

			if (myEngine->KeyHeld(kCameraDownKey))
			{
				/*if (cameraRotationLimit > cameraLowerLimit && thirdPerson == true)
				{
					cameraDummy->RotateX(-rotationSpeed);
					cameraRotationLimit--;
				}*/
				cameraDummy->MoveZ(-0.1);      // Moves the camera back
			}

			if (myEngine->KeyHeld(kCameraUpKey))
			{
				/*if (cameraRotationLimit < cameraUpperLimit && thirdPerson == true)
				{
					cameraDummy->RotateX(rotationSpeed);
					cameraRotationLimit++;
				}*/
				cameraDummy->MoveZ(0.1);       // Moves the forwards
			}

			if (myEngine->KeyHeld(kCameraLeftKey))
			{
				/*if (cameraRotationLimit < cameraUpperLimit && thirdPerson == true)
				{
				cameraDummy->RotateX(rotationSpeed);
				cameraRotationLimit++;
				}*/
				cameraDummy->MoveX(-0.1);      // Moves the camera left
			}

			if (myEngine->KeyHeld(kCameraRightKey))
			{
				/*if (cameraRotationLimit < cameraUpperLimit && thirdPerson == true)
				{
				cameraDummy->RotateX(rotationSpeed);
				cameraRotationLimit++;
				}*/
				cameraDummy->MoveX(0.1);       // Moves the camera right
			}

			cameraDummy->RotateY(myEngine->GetMouseMovementX() * frameTime * 10);  // rotates the camera according to mouse movement
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}


collisionSide SphereToBox(float pointX, float pointZ, float cubeXLength, float cubeZLength, float cubeXPos, float cubeZPos, float sphereRadius, float carOldX, float carOldZ)
{
	float minX = cubeXPos - (cubeXLength / 2.0f);			// Runs a sphere to box collision detection
	float maxX = cubeXPos + (cubeXLength / 2.0f);			// The bounding box is created using the objects position, its width, and its length.
	float minZ = cubeZPos - (cubeZLength / 2.0f);
	float maxZ = cubeZPos + (cubeZLength / 2.0f);

	collisionSide result = noSide;

	if ((pointX > minX - sphereRadius && pointX < maxX + sphereRadius) && // The function checks if the sphere, created with an objects position and radius, is overlapping the box
		(pointZ > minZ - sphereRadius && pointZ < maxZ + sphereRadius))
	{
		if (carOldX < minX) result = leftSide;		//Checks if the car was on the left side of the object before colliding
		else if (carOldX > maxX) result = rightSide;	//Checks if the car was on the right side of the object before colliding
		else if (carOldZ < minZ) result = frontSide;	//Checks if the car was in front of the object before colliding
		else if (carOldZ > maxZ)  result = backSide;	//Checks if the car was behind the object before colliding
	}

	return  (result);

}
bool SphereToSphere(float raceCarXPos, float raceCarZPos, float raceCarRadius, float objectXPos, float ObjectZPos, float objectRadius)
{
	float distX = objectXPos - raceCarXPos;					// Runs a sphere to sphere collision detection.
	float distZ = ObjectZPos - raceCarZPos;					// An objects bounding sphere is created using its position and its radius
	float distance = sqrt(distX * distX + distZ * distZ);		// the function then checks if these spheres are overlapping

	return (distance < (raceCarRadius + objectRadius));
}

void resetCarPosition(IModel* raceCar, IModel* dummy1, IModel* dummy2, float carOldX, float carOldZ, float dummy1OldX, float dummy1OldZ, float dummy2OldX, float dummy2OldZ)
{
	raceCar->SetX(carOldX);     //Sets the race cars X position to what it was in the previous frame
	raceCar->SetZ(carOldZ);		//Sets the race cars Z position to what it was in the previous frame
	dummy1->SetX(dummy1OldX);	//Sets the race cars front fummys X position to what it was in the previous frame
	dummy1->SetZ(dummy1OldZ);	//Sets the race cars front dummys Z position to what it was in the previous frame
	dummy2->SetX(dummy2OldX);	//Sets the race cars rear fummys X position to what it was in the previous frame
	dummy2->SetZ(dummy2OldZ);	//Sets the race cars rear dummys Z position to what it was in the previous frame
}