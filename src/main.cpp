#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <thread>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <mutex>
#include <iostream>
#include <conio.h>
#include <cmath>
#include "wtypes.h"
#include "rotors.h"

// Function headers
void renderingThread(sf::RenderWindow* window);
void updateAnimations(float deltaTime);
void cleanUp();
void setupRotors();
void setupPlugboard();
void setupButtons();
void loadTexturesAndFonts();
void loadSounds(sf::SoundBuffer* keyPressedSoundBuffer,		sf::SoundBuffer* keyReleasedSoundBuffer,
				sf::SoundBuffer* buttonClickSoundBuffer,	sf::SoundBuffer* sendingMorseSoundBuffer,
				sf::SoundBuffer* receivingMorseSoundBuffer, sf::Sound*		 keyPressedSound,
				sf::Sound*		 keyReleasedSound,			sf::Sound*		 buttonClickSound,
				sf::Sound*		 sendingMorseSound,			sf::Sound*		 receivingMorseSound);

// Initial window size (changed when setting textures)
int windowSizeX = 1250;
int windowSizeY = 1250;

// Flag to know what should currently be displayed and what input should be taken
enum CurrentlyOpen { Main, PlugboardConf, SwapRotors };
CurrentlyOpen currentlyOpen = Main;

// Rotors, reflector and plugboard for encrypting 
std::array<Rotor, 3> rotors;
Reflector reflector;
Plugboard plugboard;

// Current cypher letter (empty if no current input)
char conversion = ' ';

// GUI
int imageScale = 1;
std::vector<PlugboardButton*> plugboardButtons;
std::vector<sf::RectangleShape> plugboardArrows;
PlugboardButton* plugboardInputToBeChanged = NULL;
Button returnButton;
Button swapRotorsButton;
Button swapButton1;
Button swapButton2;
Button configurePlugboardButton;
sf::RectangleShape enigmaRect;
sf::Texture enigmaTexture;
sf::Texture rotorWheelTexture;
sf::Texture rotorWindowTexture;
sf::Texture swapRotorsTexture;
sf::Texture returnButtonTexture;
sf::Texture arrowTexture;
sf::Texture configurePlugboardTexture;
sf::Texture plugboardButtonTexture;
sf::Texture plugboardLetterTexture;
sf::Font textFont;

int main()
{
	/* 
	* Load all textures, then set the window width to be half of total screen size and scale window height to match
	* the enigma texture aspect ratio. Save image scaling size for other textures
	*/
	loadTexturesAndFonts();
	windowSizeX = sf::VideoMode::getDesktopMode().width / 2;
	windowSizeY = windowSizeX * enigmaTexture.getSize().y / enigmaTexture.getSize().x;
	enigmaRect = sf::RectangleShape(sf::Vector2f(static_cast<float>(windowSizeX), static_cast<float>(windowSizeY)));
	enigmaRect.setTexture(&enigmaTexture);
	imageScale = windowSizeX / enigmaTexture.getSize().x;

	// Loading and setting up sound effects
	sf::SoundBuffer keyPressedSoundBuffer;
	sf::SoundBuffer keyReleasedSoundBuffer;
	sf::SoundBuffer buttonClickSoundBuffer;
	sf::SoundBuffer sendingMorseSoundBuffer;
	sf::SoundBuffer receivingMorseSoundBuffer;
	sf::Sound keyPressedSound;
	sf::Sound keyReleasedSound;
	sf::Sound buttonClickSound;
	sf::Sound sendingMorseSound;
	sf::Sound receivingMorseSound;
	loadSounds(&keyPressedSoundBuffer,    &keyReleasedSoundBuffer, 
			   &buttonClickSoundBuffer,   &sendingMorseSoundBuffer, 
			   &receivingMorseSoundBuffer,&keyPressedSound, 
			   &keyReleasedSound,		  &buttonClickSound, 
			   &sendingMorseSound,	  &receivingMorseSound);

	// Setup frontend and backend related to cyphering
	setupRotors();
	int reflectorConversions[26] = { 16, 24, 7, 14, 6, 13, 4, 2, 21, 15, 20, 25, 19, 5, 3, 9, 0, 23, 22, 12, 10, 8, 18, 17, 1, 11 };
	reflector = Reflector(reflectorConversions);
	setupPlugboard();

	// Other GUI stuff
	setupButtons();

	char lastKeyPressed = ' ';

	// Window, event and clock
	sf::RenderWindow window(sf::VideoMode(windowSizeX, windowSizeY), "Enigma");
	window.setFramerateLimit(30);
	window.setKeyRepeatEnabled(false);
	window.setActive(false);
	sf::Event event;
	float deltaTime = 0.0f;
	sf::Clock clock;

	// Seperate thread for rendering and drawing
	std::thread renderThread(renderingThread, &window);

	// Attempt to create connection to server for Morse code commuication
	bool serverConnected = false;
	float playMorseFor = 0.f;
	std::queue<float> signalQueue;
	sf::Clock morseClock;
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect("192.168.0.62", 53000, sf::seconds(2.5));
	if (status != sf::Socket::Done)
	{
		std::cout << "Could not connect to local server 192.168.0.62:53000, trying remote" << std::endl;

		status = socket.connect("2.123.89.5", 53000, sf::seconds(2.5));

		if (status != sf::Socket::Done)
		{
			std::cout << "Could not connect to remote server 2.123.89.5:53000" << std::endl;
		}

		else
		{
			serverConnected = true;
			std::cout << "Connection to 2.123.89.5:53000 successful" << std::endl;
			socket.setBlocking(false);
		}
	}

	else
	{
		serverConnected = true;
		std::cout << "Connection to 192.168.0.62:53000 successful" << std::endl;
		socket.setBlocking(false);
	}



	while (window.isOpen()) 
	{
		// A delay between event polls (greatly reduces CPU load)
		sf::sleep(sf::milliseconds(10));

		// Update animations
		deltaTime = clock.restart().asSeconds();
		updateAnimations(deltaTime);

		if (serverConnected)
		{
			if (playMorseFor > 0.0)
			{
				if (receivingMorseSound.getStatus() != sf::Sound::Playing)
				{
					receivingMorseSound.play();
				}

				else
				{
					playMorseFor -= deltaTime;

					if (playMorseFor < 0.f)
					{
						playMorseFor = 0.f;
					}
				}
			}

			else if (receivingMorseSound.getStatus() == sf::Sound::Playing)
			{
				receivingMorseSound.stop();
			}
		}

		if (serverConnected)
		{
			sf::Packet receivePacket;
			if (socket.receive(receivePacket) == sf::Socket::Done)
			{
				float seconds;
				receivePacket >> seconds;

				playMorseFor = seconds;

				std::cout << "Received signal " << seconds << " seconds long" << std::endl;
			}
		}



		while (window.pollEvent(event)) 
		{
			if (event.type == sf::Event::Closed) 
			{
				window.close();
			}

			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}

				else
				{
					lastKeyPressed = getKeyFromEvent(&event);

					if (currentlyOpen == Main)
					{
						if (event.key.code == sf::Keyboard::Space && serverConnected)
						{
							sendingMorseSound.play();
							morseClock.restart();
						}

						else if (lastKeyPressed != ' ')
						{
							/*
							* If convertable key pressed in the main screen then play sound effect, rotate rotors and display conversion
							*/
							keyPressedSound.play();

							if (rotors[2].rotation == rotors[2].turnoverPosition)
							{
								if (rotors[1].rotation == rotors[1].turnoverPosition)
									rotors[0].rotateUp();

								rotors[1].rotateUp();
							}
							rotors[2].rotateUp();

							conversion = getKeyFromIndex(getConversion(getCharIndex(lastKeyPressed), plugboard, rotors[0], rotors[1], rotors[2], reflector));
							
							// Display converted character
							std::string enigmaFilePath("textures/enigmas/enigma ");
							enigmaFilePath += conversion;
							enigmaFilePath += " Output.png";
							enigmaTexture.loadFromFile(enigmaFilePath);
						}
					}

					else if (currentlyOpen == PlugboardConf && plugboardInputToBeChanged != NULL)
					{
						int charVal = -1;

						if (getCharIndex(lastKeyPressed) >= 0 && getCharIndex(lastKeyPressed) < 26)
						{
							charVal = getCharIndex(lastKeyPressed);
						}

						else
						{
							lastKeyPressed = '-';
						}

						boolean mappingAllowed = true;

						for (PlugboardButton* p : plugboardButtons)
						{
							// Check if there are any plugboard inputs mapped to the same character, if so, block mapping
							if (p->charVal >= 0 && p->charVal < 26 && p->charVal == charVal && charVal != plugboardInputToBeChanged->charVal)
							{
								mappingAllowed = false;
							}
						}

						if (mappingAllowed)
						{
							plugboardInputToBeChanged->setLetter(charVal);
							plugboardInputToBeChanged->shape.setFillColor(sf::Color::White);
							plugboardInputToBeChanged = NULL;
						}
					}
				}
			}

			else if (event.type == sf::Event::KeyReleased)
			{
				if (currentlyOpen == Main)
				{
					if (event.key.code == sf::Keyboard::Space && serverConnected)
					{
						sendingMorseSound.stop();
						sf::Packet packet;
						packet << morseClock.getElapsedTime().asSeconds();

						if (socket.send(packet) != sf::Socket::Done)
						{
							std::cout << "Sending failed..." << std::endl;
						}
					}

					else if (getKeyFromEvent(&event) == lastKeyPressed)
					{
						if (conversion != ' ')
						{
							keyReleasedSound.play();
						}

						conversion = ' ';
						// Reset enigma texture to no output
						enigmaTexture.loadFromFile("textures/enigmas/enigma noOutput.png");
					}
				}
			}

			else if (event.type == sf::Event::MouseButtonPressed)
			{
				if (currentlyOpen == Main)
				{
					for (unsigned int i = 0; i < rotors.size(); i++)
					{
						if (rotors[i].rotorWheelClicked(event.mouseButton.x, event.mouseButton.y))
						{
							rotors[i].rotorWheelBeingDragged = true;
							rotors[i].currentDragCoordinates = sf::Vector2u(event.mouseButton.x, event.mouseButton.y);
						}
					}

					if (swapRotorsButton.isClicked(event.mouseButton.x, event.mouseButton.y))
					{
						buttonClickSound.play();
						currentlyOpen = SwapRotors;
					}

					if (configurePlugboardButton.isClicked(event.mouseButton.x, event.mouseButton.y))
					{
						buttonClickSound.play();
						currentlyOpen = PlugboardConf;
					}
				}

				else if (currentlyOpen == PlugboardConf)
				{
					for (PlugboardButton* p : plugboardButtons)
					{
						if (p->isClicked(event.mouseButton.x, event.mouseButton.y))
						{
							buttonClickSound.play();

							if (plugboardInputToBeChanged != NULL)
							{
								plugboardInputToBeChanged->shape.setFillColor(sf::Color::White);
							}

							// Save plugboard button that was clicked so that it can be mapped in the next key pressed event
							p->shape.setFillColor(sf::Color(50, 0, 0));
							plugboardInputToBeChanged = p;
						}
					}

					if (returnButton.isClicked(event.mouseButton.x, event.mouseButton.y))
					{
						buttonClickSound.play();
						currentlyOpen = Main;
					}
				}

				else if (currentlyOpen == SwapRotors)
				{
					if (returnButton.isClicked(event.mouseButton.x, event.mouseButton.y))
					{
						buttonClickSound.play();
						currentlyOpen = Main;
					}

					// Swapping rotor GUI positions and cyphering pipeline locations
					if (swapButton1.isClicked(event.mouseButton.x, event.mouseButton.y))
					{
						buttonClickSound.play();

						sf::Vector2f tempRotorWindowPosition = rotors[0].rotorWindow.getPosition();
						sf::Vector2f tempRotorWheelPosition = rotors[0].rotorWheel.getPosition();
						sf::Vector2f tempRotorNumberPosition = rotors[0].rotorNumber.getPosition();
						Rotor tempRotor = rotors[0];

						rotors[0] = rotors[1];
						rotors[0].rotorWindow.setPosition(tempRotorWindowPosition);
						rotors[0].rotorWheel.setPosition(tempRotorWheelPosition);
						rotors[0].rotorNumber.setPosition(tempRotorNumberPosition);

						tempRotor.rotorWindow.setPosition(rotors[1].rotorWindow.getPosition());
						tempRotor.rotorWheel.setPosition(rotors[1].rotorWheel.getPosition());
						tempRotor.rotorNumber.setPosition(rotors[1].rotorNumber.getPosition());

						rotors[1] = tempRotor;
					}

					if (swapButton2.isClicked(event.mouseButton.x, event.mouseButton.y))
					{
						buttonClickSound.play();

						sf::Vector2f tempRotorWindowPosition = rotors[1].rotorWindow.getPosition();
						sf::Vector2f tempRotorWheelPosition = rotors[1].rotorWheel.getPosition();
						sf::Vector2f tempRotorNumberPosition = rotors[1].rotorNumber.getPosition();
						Rotor tempRotor = rotors[1];

						rotors[1] = rotors[2];
						rotors[1].rotorWindow.setPosition(tempRotorWindowPosition);
						rotors[1].rotorWheel.setPosition(tempRotorWheelPosition);
						rotors[1].rotorNumber.setPosition(tempRotorNumberPosition);

						tempRotor.rotorWindow.setPosition(rotors[2].rotorWindow.getPosition());
						tempRotor.rotorWheel.setPosition(rotors[2].rotorWheel.getPosition());
						tempRotor.rotorNumber.setPosition(rotors[2].rotorNumber.getPosition());

						rotors[2] = tempRotor;
					}
				}
			}

			else if (currentlyOpen == Main && event.type == sf::Event::MouseButtonReleased)
			{
				for (unsigned int i = 0; i < rotors.size(); i++)
				{
					if (rotors[i].rotorWheelBeingDragged)
					{
						// If there was a rotor wheel being dragged, then set it has been released
						rotors[i].rotorWheelBeingDragged = false;

						/*
						* If dragging ended in the middle of animation then finish animation to 
						* the nearest rotor setting
						*/
						if (rotors[i].rotorWindowAnimation.currentRow == rotors[i].rotation - 1)
						{
							rotors[i].rotorWindowAnimation.finishAnimation(-1);
							rotors[i].rotorWheelAnimation.finishAnimation(-1);
						}

						else 
						{
							rotors[i].rotorWindowAnimation.finishAnimation(1);
							rotors[i].rotorWheelAnimation.finishAnimation(1);
						}
					}
				}
			}

			else if (currentlyOpen == Main && event.type == sf::Event::MouseMoved)
			{
				for (unsigned int i = 0; i < rotors.size(); i++)
				{
					if (rotors[i].rotorWheelBeingDragged)
					{
						/*
						* Update animation when wheel has been dragged a certain amount of pixels away
						* from the last animation update
						*/ 
						int mouseDragDiff = event.mouseMove.y - (int)rotors[i].currentDragCoordinates.y;
						if (std::abs(mouseDragDiff) > rotors[i].rotorWheel.getSize().y / 54)
						{
							/*
							* Need to know when the rotor has been moved over to another rotor setting,
							* so incrementAnimation returns the current texture row that it is on which 
							* corresponds to the current rotor setting
							*/
							int rot = rotors[i].rotorWindowAnimation.incrementAnimation(mouseDragDiff);
							rotors[i].rotorWheelAnimation.incrementAnimation(mouseDragDiff);

							if (rot != rotors[i].rotation)
							{
								rotors[i].rotation = rot;
								buttonClickSound.play();
							}

							rotors[i].currentDragCoordinates = sf::Vector2u(event.mouseMove.x, event.mouseMove.y);
						}
					}
				}
			}
		}
	}

	renderThread.join();
	cleanUp();

	return 0;
}



void renderingThread(sf::RenderWindow* window)
{
	sf::RectangleShape grayFilter(sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y)));
	grayFilter.setFillColor(sf::Color(0, 0, 0, 215));

	window->setActive(true);

	while (window->isOpen())
	{
		window->clear(sf::Color::Black);

		window->draw(enigmaRect);
		window->draw(configurePlugboardButton.shape);
		window->draw(swapRotorsButton.shape);

		for (unsigned int i = 0; i < rotors.size(); i++)
		{
			window->draw(rotors[i].rotorWheel);
			window->draw(rotors[i].rotorWindow);
		}

		if (currentlyOpen != Main)
		{
			window->draw(grayFilter);
			window->draw(returnButton.shape);

			if (currentlyOpen == SwapRotors)
			{
				window->draw(swapButton1.shape);
				window->draw(swapButton2.shape);

				for (unsigned int i = 0; i < rotors.size(); i++)
				{
					window->draw(rotors[i].rotorNumber);
				}
			}

			else if (currentlyOpen == PlugboardConf)
			{
				for (PlugboardButton* p : plugboardButtons)
				{
					window->draw(p->shape);
					window->draw(p->letter);
				}

				for (sf::RectangleShape r : plugboardArrows)
				{
					window->draw(r);
				}
			}
		}

		window->display();
	}
}

void cleanUp()
{
	plugboardInputToBeChanged = NULL;
	for (PlugboardButton* p : plugboardButtons)
	{
		delete(p);
	}
	plugboardButtons.clear();

	delete(plugboardInputToBeChanged);
}

void updateAnimations(float deltaTime)
{
	for (unsigned int i = 0; i < rotors.size(); i++)
	{
		rotors[i].updateAnimation(deltaTime);
	}
}



void setupRotors()
{
	sf::RectangleShape rotorWheel1(sf::Vector2f(
		static_cast<float>(rotorWheelTexture.getSize().x / 11 * imageScale),
		static_cast<float>(rotorWheelTexture.getSize().y * imageScale)));
	rotorWheel1.setTexture(&rotorWheelTexture);
	rotorWheel1.setTextureRect(sf::IntRect(0, 0, rotorWheelTexture.getSize().x / 11, rotorWheelTexture.getSize().y));
	rotorWheel1.setOrigin(rotorWheel1.getSize().x / 2, rotorWheel1.getSize().y / 2);
	rotorWheel1.setPosition(sf::Vector2f(windowSizeX / 4.f, windowSizeY / 4.f));

	sf::RectangleShape rotorWindow1(sf::Vector2f(
		static_cast<float>(rotorWindowTexture.getSize().x / 10 * imageScale),
		static_cast<float>(rotorWindowTexture.getSize().y / 26 * imageScale)));
	rotorWindow1.setTexture(&rotorWindowTexture);
	rotorWindow1.setTextureRect(sf::IntRect(0, 0, rotorWindowTexture.getSize().x / 10, rotorWindowTexture.getSize().y / 26));
	rotorWindow1.setOrigin(rotorWindow1.getSize().x / 2, rotorWindow1.getSize().y / 2);
	rotorWindow1.setPosition(rotorWheel1.getPosition());
	rotorWindow1.move(sf::Vector2f(-rotorWindow1.getSize().x / 2 - rotorWheel1.getSize().x, 0.f));

	sf::RectangleShape rotorWindow2 = rotorWindow1;
	rotorWindow2.setPosition(rotorWheel1.getPosition());
	rotorWindow2.move(sf::Vector2f(rotorWindow1.getSize().x / 2 + rotorWheel1.getSize().x * 2, 0.f));

	sf::RectangleShape rotorWheel2 = rotorWheel1;
	rotorWheel2.setPosition(rotorWindow2.getPosition());
	rotorWheel2.move(sf::Vector2f(rotorWindow2.getSize().x / 2 + rotorWheel2.getSize().x, 0.f));

	sf::RectangleShape rotorWindow3 = rotorWindow2;
	rotorWindow3.setPosition(rotorWheel2.getPosition());
	rotorWindow3.move(sf::Vector2f(rotorWindow3.getSize().x / 2 + rotorWheel1.getSize().x * 2, 0.f));

	sf::RectangleShape rotorWheel3 = rotorWheel2;
	rotorWheel3.setPosition(rotorWindow3.getPosition());
	rotorWheel3.move(sf::Vector2f(rotorWindow3.getSize().x / 2 + rotorWheel2.getSize().x, 0.f));

	sf::Text rotor1Text;
	rotor1Text.setFont(textFont);
	rotor1Text.setCharacterSize(windowSizeX / 10);
	rotor1Text.setFillColor(sf::Color::White);
	rotor1Text.setString("I");
	sf::FloatRect textRect = rotor1Text.getLocalBounds();
	rotor1Text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	rotor1Text.setPosition(sf::Vector2f(windowSizeX / 4.0f, windowSizeY / 2.0f));

	sf::Text rotor2Text = rotor1Text;
	rotor2Text.setString("II");
	textRect = rotor2Text.getLocalBounds();
	rotor2Text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	rotor2Text.setPosition(sf::Vector2f(windowSizeX / 2.0f, windowSizeY / 2.0f));

	sf::Text rotor3Text = rotor2Text;
	rotor3Text.setString("III");
	textRect = rotor3Text.getLocalBounds();
	rotor3Text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	rotor3Text.setPosition(sf::Vector2f(windowSizeX - windowSizeX / 4.0f, windowSizeY / 2.0f));



	std::array<int, 26> rotor1Conversions = { 9, 6, 3, 16, 14, 23, 20, 18, 2, 0, 12, 8, 5, 17, 21, 19, 15, 13, 4, 22, 10, 1, 11, 25, 24, 7 };
	std::array<int, 26> rotor2Conversions = { 13, 19, 25, 15, 18, 5, 1, 14, 10, 12, 22, 17, 2, 9, 3, 8, 21, 11, 0, 4, 24, 20, 23, 7, 6, 16 };
	std::array<int, 26> rotor3Conversions = { 9, 21, 8, 20, 1, 7, 19, 2, 3, 24, 0, 10, 4, 16, 25, 15, 14, 18, 6, 23, 13, 17, 12, 22, 5, 11 };
	rotors[0] = Rotor(rotor1Conversions, 22, rotorWindow1, &rotorWindowTexture, rotorWheel1, &rotorWheelTexture, imageScale, rotor1Text);
	rotors[1] = Rotor(rotor2Conversions, 13,  rotorWindow2, &rotorWindowTexture, rotorWheel2, &rotorWheelTexture, imageScale, rotor2Text);
	rotors[2] = Rotor(rotor3Conversions, 7, rotorWindow3, &rotorWindowTexture, rotorWheel3,&rotorWheelTexture, imageScale, rotor3Text);
}

void setupPlugboard()
{
	float rectEdgeLength = (windowSizeX) / 14.f;
	float space = rectEdgeLength / 13.f;
	rectEdgeLength -= space;

	for (int i = 0; i < 13; i++)
	{
		sf::RectangleShape arrow(sf::Vector2f(
			static_cast<float>(arrowTexture.getSize().x * imageScale),
			static_cast<float>(arrowTexture.getSize().y * imageScale)));
		arrow.setTexture(&arrowTexture);
		arrow.setOrigin(arrow.getSize().x / 2.f, arrow.getSize().y / 2.f);
		arrow.rotate(90.f);
		arrow.setScale(sf::Vector2f(0.9f, 0.9f));
		arrow.setPosition(sf::Vector2f((rectEdgeLength + space)* (i + 1), windowSizeY / 2.f));
		plugboardArrows.push_back(arrow);

		sf::RectangleShape rectangle1(sf::Vector2f(rectEdgeLength, rectEdgeLength));
		rectangle1.setTexture(&plugboardButtonTexture);
		rectangle1.setOrigin(sf::Vector2f(rectangle1.getSize().x / 2.f, rectangle1.getSize().y / 2.f));
		rectangle1.setPosition(arrow.getPosition());
		rectangle1.move(sf::Vector2f(0.f, - (rectEdgeLength + arrow.getLocalBounds().height / 2)));

		sf::RectangleShape letter1(sf::Vector2f(
			static_cast<float>(plugboardLetterTexture.getSize().x / 27 * imageScale),
			static_cast<float>(plugboardLetterTexture.getSize().y * imageScale)));
		letter1.setTexture(&plugboardLetterTexture);
		letter1.setTextureRect(sf::IntRect(0, 0, plugboardLetterTexture.getSize().x / 27, plugboardLetterTexture.getSize().y));
		letter1.setOrigin(sf::Vector2f(letter1.getSize().x / 2.f, letter1.getSize().y / 2.f));
		letter1.setPosition(rectangle1.getPosition());

		PlugboardButton* plugboardButton1 = new PlugboardButton(rectangle1, letter1, getCharIndex(' '));
		plugboardButtons.push_back(plugboardButton1);



		sf::RectangleShape rectangle2 = rectangle1;
		rectangle2.setPosition(arrow.getPosition());
		rectangle2.move(sf::Vector2f(0.f, rectEdgeLength + arrow.getLocalBounds().height / 2.0f));

		sf::RectangleShape letter2 = letter1;
		letter2.setPosition(rectangle2.getPosition());

		PlugboardButton* plugboardButton2 = new PlugboardButton(rectangle2, letter2, getCharIndex(' '));
		plugboardButtons.push_back(plugboardButton2);

		plugboard.insertPair(plugboardButton1, plugboardButton2);
	}
}

void setupButtons()
{
	sf::RectangleShape returnButtonRect(sf::Vector2f(
		static_cast<float>(returnButtonTexture.getSize().x * imageScale),
		static_cast<float>(returnButtonTexture.getSize().y * imageScale)));
	returnButtonRect.setTexture(&returnButtonTexture);
	returnButtonRect.setOrigin(returnButtonRect.getSize().x / 2.f, returnButtonRect.getSize().y / 2.f);
	returnButtonRect.setPosition(sf::Vector2f(
		static_cast<float>(returnButtonRect.getSize().x),
		static_cast<float>(returnButtonRect.getSize().y)));
	returnButton = Button(returnButtonRect);

	sf::RectangleShape swapRotorsRect(sf::Vector2f(
		static_cast<float>(swapRotorsTexture.getSize().x * imageScale),
		static_cast<float>(swapRotorsTexture.getSize().y * imageScale)));
	swapRotorsRect.setTexture(&swapRotorsTexture);
	swapRotorsRect.setOrigin(swapRotorsRect.getSize().x / 2.f, swapRotorsRect.getSize().y / 2.f);
	swapRotorsRect.setPosition(sf::Vector2f(windowSizeX - windowSizeX / 4.25f, windowSizeY / 3.f));
	swapRotorsButton = Button(swapRotorsRect);

	sf::RectangleShape swapArrow1(sf::Vector2f(
		static_cast<float>(arrowTexture.getSize().x * imageScale),
		static_cast<float>(arrowTexture.getSize().y * imageScale)));
	swapArrow1.setTexture(&arrowTexture);
	swapArrow1.setOrigin(swapArrow1.getSize().x / 2.f, swapArrow1.getSize().y / 2.f);
	swapArrow1.setPosition(sf::Vector2f(windowSizeX / 3.f, windowSizeY / 2.f));
	sf::RectangleShape swapArrow2 = swapArrow1;
	swapArrow2.setPosition(sf::Vector2f(windowSizeX - windowSizeX / 3.f, windowSizeY / 2.f));
	swapButton1 = Button(swapArrow1);
	swapButton2 = Button(swapArrow2);

	sf::RectangleShape configurePlugboardRect(sf::Vector2f(
		static_cast<float>(configurePlugboardTexture.getSize().x * imageScale),
		static_cast<float>(configurePlugboardTexture.getSize().y * imageScale)));
	configurePlugboardRect.setTexture(&configurePlugboardTexture);
	configurePlugboardRect.setOrigin(configurePlugboardRect.getSize().x / 2, configurePlugboardRect.getSize().y / 2);
	configurePlugboardRect.setPosition(sf::Vector2f(windowSizeX - windowSizeX / 4.25f, windowSizeY / 4.f));
	configurePlugboardButton = Button(configurePlugboardRect);
}



void loadTexturesAndFonts()
{
	enigmaTexture			 .loadFromFile("textures/enigmas/enigma noOutput.png");
	rotorWheelTexture		 .loadFromFile("textures/rotor wheel sheet.png");
	rotorWindowTexture		 .loadFromFile("textures/rotor window sheet.png");
	plugboardButtonTexture	 .loadFromFile("textures/plugboard button.png");
	returnButtonTexture		 .loadFromFile("textures/return button.png");
	swapRotorsTexture		 .loadFromFile("textures/swap rotors button.png");
	arrowTexture			 .loadFromFile("textures/arrow.png");
	configurePlugboardTexture.loadFromFile("textures/configure plugboard button.png");
	plugboardLetterTexture	 .loadFromFile("textures/plugboard letters.png");

	textFont.loadFromFile("fonts/JMH Cthulhumbus Arcade.otf");
}

void loadSounds(sf::SoundBuffer* keyPressedSoundBuffer,		sf::SoundBuffer* keyReleasedSoundBuffer,
				sf::SoundBuffer* buttonClickSoundBuffer,	sf::SoundBuffer* sendingMorseSoundBuffer, 
				sf::SoundBuffer* receivingMorseSoundBuffer, sf::Sound* keyPressedSound, 
				sf::Sound* keyReleasedSound,				sf::Sound* buttonClickSound, 
				sf::Sound* sendingMorseSound,				sf::Sound* receivingMorseSound)
{
	keyPressedSoundBuffer->	   loadFromFile("sound effects/key pressed.wav");
	keyReleasedSoundBuffer->   loadFromFile("sound effects/key released.wav");
	buttonClickSoundBuffer->   loadFromFile("sound effects/button click.wav");
	sendingMorseSoundBuffer->  loadFromFile("sound effects/morse.wav");
	receivingMorseSoundBuffer->loadFromFile("sound effects/morse.wav");

	keyPressedSound->	 setBuffer(*keyPressedSoundBuffer);
	keyReleasedSound->	 setBuffer(*keyReleasedSoundBuffer);
	buttonClickSound->	 setBuffer(*buttonClickSoundBuffer);
	sendingMorseSound->	 setBuffer(*sendingMorseSoundBuffer);
	receivingMorseSound->setBuffer(*receivingMorseSoundBuffer);
}