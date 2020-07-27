#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

enum class side {LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];

int main() {

	VideoMode vm(1920, 1080);

	RenderWindow window(vm, "Timberr", Style::Fullscreen);

	int lastDrawn = 0;
	/*
	BACKGROUND SPRITE---------------------------------------------
	*/
	//Create a texture to hold graphic on the GPU
	Texture backgroundTexture;
	backgroundTexture.loadFromFile("graphics/background.png");

	//Create a sprite
	Sprite backgroundSprite;
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setPosition(0, 0);
	/*
	--------------------------------------------------------------
	*/

	//TREE SPRITE
	Texture treeTexture;
	treeTexture.loadFromFile("graphics/tree.png");
	Sprite treeSprite;
	treeSprite.setTexture(treeTexture);
	treeSprite.setPosition(810, 0);

	//BEE SPRITE
	Texture beeTexture;
	beeTexture.loadFromFile("graphics/bee.png");
	Sprite beeSprite;
	beeSprite.setTexture(beeTexture);
	beeSprite.setPosition(0, 800);

	//Is the bee moving?
	bool beeActive = false;

	//How fast can the bee fly
	float beeSpeed = 0.0f;

	//Make 3 cloud sprites from 1 texture
	Texture cloudTexture;
	cloudTexture.loadFromFile("graphics/cloud.png");
	Sprite clouds[3];
	for (int i = 0; i < 3; i++) {
		clouds[i].setTexture(cloudTexture);
		clouds[i].setPosition(0, i * 150);
	}
	//Are clouds active
	bool cloudsActive[3] = { false, false, false };

	//Cloud speed
	float cloudsSpeed[3] = { 0.0f, 0.0f, 0.0f };

	//Variable to control time
	Clock clock;

	//Time bar
	RectangleShape timeBar;
	float timeBarWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarWidth / 2, 980);

	Time gameTimeTotoal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarWidth / timeRemaining;

	//Track whether game is running
	bool paused = true;
	//Draw some Text
	int score = 0;

	Text scoreText;
	Text messageText;
	Text FPSText;

	//font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	scoreText.setFont(font);
	messageText.setFont(font);

	scoreText.setString("Score = 0");
	messageText.setString("Press Enter to Start!");

	scoreText.setCharacterSize(100);
	messageText.setCharacterSize(75);

	scoreText.setFillColor(Color::White);
	messageText.setFillColor(Color::White);

	//Position the Text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.width / 2, textRect.top + textRect.height / 2);
	
	scoreText.setPosition(20, 20);
	messageText.setPosition(1920.0f / 2.0f, 1080.0f / 2.0f);

	//Prepare branches
	Texture branchTexture;
	branchTexture.loadFromFile("graphics/branch.png");

	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(branchTexture);
		branches[i].setPosition(-2000, -2000);

		//set sprite origin at center
		branches[i].setOrigin(220, 20);
	}

	//Prepare the player
	Texture playerTexture;
	playerTexture.loadFromFile("graphics/player.png");
	Sprite playerSprite;
	playerSprite.setTexture(playerTexture);
	playerSprite.setPosition(580, 720);

	//The player starts on the left
	side playerSide = side::LEFT;

	//Prepare the gravestone
	Texture gravestoneTexture;
	gravestoneTexture.loadFromFile("graphics/rip.png");
	Sprite ripSprite;
	ripSprite.setTexture(gravestoneTexture);
	ripSprite.setPosition(600, 860);

	//Prepare the Axe
	Texture axeTexture;
	axeTexture.loadFromFile("graphics/axe.png");
	Sprite axeSprite;
	axeSprite.setTexture(axeTexture);
	axeSprite.setPosition(700, 830);

	//Line the axe with the tree
	const float AXE_POS_LEFT = 700;
	const float AXE_POS_RIGHT = 1075;

	//Prepare the flying Log
	Texture logTexture;
	logTexture.loadFromFile("graphics/log.png");
	Sprite logSprite;
	logSprite.setTexture(logTexture);
	logSprite.setPosition(810, 720);

	//Log variabes
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	//Control player input
	bool acceptInput = false;

	//Prepare sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);


	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	chop.setBuffer(deathBuffer);


	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound oot;
	chop.setBuffer(ootBuffer);

	while (window.isOpen()) {

		Event event;

		while (window.pollEvent(event))	{
			if (event.type == Event::KeyReleased && !paused) {
				//Listen for key press again
				acceptInput = true;

				//hide the axe
				axeSprite.setPosition(2000, axeSprite.getPosition().y);
			}
		}

		//Handle player input
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			//Reset time and score
			score = 0;
			timeRemaining = 6;

			//Make all branches disappear
			for (int i = 0; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			//Make sure grave is hidden
			ripSprite.setPosition(675, 2000);

			//Move player into position
			playerSprite.setPosition(580, 720);

			acceptInput = true;
		}

		if (acceptInput) {
			
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				playerSide = side::RIGHT;
				score++;

				//Add to amount of time remaining
				timeRemaining += (2 / score) + .15;

				axeSprite.setPosition(AXE_POS_RIGHT, axeSprite.getPosition().y);
				playerSprite.setPosition(1200, 720);

				//Update branches
				updateBranches(score);

				//set log flying left
				logSprite.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				chop.play();
			}
			
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				// Make sure the player is on the left
				playerSide = side::LEFT;

				score++;

				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				axeSprite.setPosition(AXE_POS_LEFT, axeSprite.getPosition().y);
				playerSprite.setPosition(580, 720);

				// update the branches
				updateBranches(score);

				// set the log flying
				logSprite.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;


				acceptInput = false;

				// Play a chop sound
				chop.play();

			}
		}

		if (!paused) {
			//Measure time
			Time dt = clock.restart();
			timeRemaining -= dt.asSeconds();

			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f)
			{
				//Pause the game
				paused = true;

				messageText.setString("Out of Time!");
				//Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				oot.play();
			}

			//Update scene
			//MANAGE THE BEE---------------------------------------------------------------------------------------------
			if (!beeActive) {
				//How fast is the bee
				srand((int)time(0) * 10); //Seed
				beeSpeed = (rand() % 200) - 200; //Gives random numbers between 200 to 399

				//How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				beeSprite.setPosition(2000, height);

				beeActive = true;
			}
			else {
				beeSprite.setPosition(beeSprite.getPosition().x + (beeSpeed * dt.asSeconds()), beeSprite.getPosition().y);
				if (beeSprite.getPosition().x < -100) {
					beeActive = false;
				}
			}
			//------------------------------------------------------------------------------------------------------------

			//MANAGE CLOUDS-----------------------------------------------------------------------------------------------
			for (int i = 0; i < 3; i++) {
				if (!cloudsActive[i]) {
					// How fast is the cloud
					srand((int)time(0) * 10 * i);
					cloudsSpeed[i] = (rand() % 200) + 100;

					// How high is the cloud
					srand((int)time(0) * 10 * i);
					float height = (rand() % 150);
					clouds[i].setPosition(-200, height);
					cloudsActive[i] = true;
				}
				else {
					clouds[i].setPosition(clouds[i].getPosition().x + (cloudsSpeed[i] * dt.asSeconds()), clouds[i].getPosition().y);

					// Has the cloud reached the right hand edge of the screen?
					if (clouds[i].getPosition().x > 1920)
					{
						// Set it up ready to be a whole new cloud next frame
						cloudsActive[i] = false;
					}
				}
			}
			//------------------------------------------------------------------------------------------------------------
		
			lastDrawn++;
			if (lastDrawn == 100) {
				// Update the score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());

				// Draw the fps
				std::stringstream ss2;
				ss2 << "FPS = " << 1 / dt.asSeconds();
				FPSText.setString(ss2.str());
				lastDrawn = 0;
			}

			//update branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;

				if (branchPositions[i] == side::LEFT) {
					//Move sprite to the left side
					branches[i].setPosition(610, height);
					branches[i].setOrigin(220, 40);
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT) {
					branches[i].setPosition(1330, height);
					branches[i].setOrigin(220, 40);
					branches[i].setRotation(0);
				}
				else {
					//Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			if (logActive) {
				logSprite.setPosition(logSprite.getPosition().x + (logSpeedX * dt.asSeconds()), logSprite.getPosition().y + (logSpeedY * dt.asSeconds()));
				if (logSprite.getPosition().x < -100 || logSprite.getPosition().x > 2000) {
					logActive = false;
					logSprite.setPosition(810, 720);
				}
			}

			if (branchPositions[5] == playerSide) {
				//death
				paused = true;
				acceptInput = true;
				//Draw the gravestone
				ripSprite.setPosition(525, 760);
				//Hide the player
				playerSprite.setPosition(2000, 660);
				//Change the text of the message
				messageText.setString("SQUISHED!!");
				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);

				// Play the death sound
				death.play();
			}
		}

		//DRAW
		//Clear everything from last frame 
		window.clear();

		//Draw scene here
		window.draw(backgroundSprite);	
		//Draw clouds
		for (int i = 0; i < 3; i++) {
			window.draw(clouds[i]);
		}
		//Draw branches
		for (int i = 0; i < NUM_BRANCHES; i++){
			window.draw(branches[i]);
		}
		//Draw tree
		window.draw(treeSprite);

		window.draw(playerSprite);

		window.draw(axeSprite);

		window.draw(logSprite);

		window.draw(ripSprite);

		//Draw bee
		window.draw(beeSprite);
		
		window.draw(scoreText);

		window.draw(timeBar);

		if(paused)
			window.draw(messageText);

		//Show everything we drew
		window.display();
	}

	return 0;
}

void updateBranches(int seed) {
	//Move all branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}

	//Spawn a new branch at position 0
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;

	default:
		branchPositions[0] = side::NONE;
		break;
	}
}