#include "pch.h"

#include "../Enigma/rotors.cpp"
#include "../Enigma/animation.cpp"



class AnimationTest : public ::testing::Test
{
protected:

	void SetUp() override 
	{
		sf::Texture texture;
		texture.create(100, 20);
		sf::Vector2i vector(10, 2);

		animation = Animation(&texture, vector, 1.5);
	}

	Animation animation;
};

TEST_F(AnimationTest, startAnimationTest)
{
	animation.startAnimation(1);
	ASSERT_EQ(1, animation.currentRow);
}

TEST_F(AnimationTest, incrementAnimationUpTest)
{
	animation.startAnimation(1);
	int currentRow = animation.incrementAnimation(1);
	ASSERT_EQ(currentRow, 2);
}

TEST_F(AnimationTest, incrementAnimationDownTest)
{
	animation.startAnimation(0);
	int currentRow = animation.incrementAnimation(-1);
	ASSERT_EQ(currentRow, 1);
}

TEST_F(AnimationTest, incrementAnimationUpALotTest)
{
	animation.startAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	animation.incrementAnimation(1);
	int currentRow = animation.incrementAnimation(1);
	ASSERT_EQ(currentRow, 1);
}

TEST_F(AnimationTest, updateTest)
{
	animation.startAnimation(0);
	ASSERT_EQ(animation.uvRect.left, 0);
	animation.update(0.5);
	ASSERT_EQ(animation.uvRect.left, 0);
	animation.update(1.0);
	ASSERT_EQ(animation.uvRect.left, 10);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 20);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 30);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 40);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 50);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 60);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 70);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 80);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 90);
	animation.update(1.5);
	ASSERT_EQ(animation.uvRect.left, 90);
}



class RotorTest : public ::testing::Test
{
protected:

	void SetUp() override 
	{
		std::array<int, 26> rotorConversions = { 9, 6, 3, 16, 14, 23, 20, 18, 2, 0, 12, 8, 5, 17, 21, 19, 15, 13, 4, 22, 10, 1, 11, 25, 24, 7 };
		sf::RectangleShape rectangle;
		sf::Texture rotorTexture;
		sf::Texture wheelTexture;
		rotorTexture.create(100, 260);
		wheelTexture.create(40, 50);
		sf::Text text;

		rotor = Rotor(rotorConversions, 16, rectangle, &rotorTexture, rectangle, &wheelTexture, 0, text);
	}

	Rotor rotor;
};

TEST_F(RotorTest, constructionTest)
{
	ASSERT_EQ(rotor.rotation, 1);
	ASSERT_EQ(rotor.turnoverPosition, 16);
	ASSERT_EQ(rotor.rotorWheelBeingDragged, false);
}

TEST_F(RotorTest, rotateUpTest)
{
	ASSERT_EQ(rotor.rotation, 1);
	rotor.rotateUp();
	ASSERT_EQ(rotor.rotation, 2);

	rotor.rotation = 26;
	ASSERT_EQ(rotor.rotation, 26);
	rotor.rotateUp();
	ASSERT_EQ(rotor.rotation, 1);
}

TEST_F(RotorTest, inConvertTest) {
	ASSERT_EQ(rotor.inConvert(0),   9);
	ASSERT_EQ(rotor.inConvert(1),   6);
	ASSERT_EQ(rotor.inConvert(2),   3);
	ASSERT_EQ(rotor.inConvert(3),  16);
	ASSERT_EQ(rotor.inConvert(4),  14);
	ASSERT_EQ(rotor.inConvert(5),  23);
	ASSERT_EQ(rotor.inConvert(6),  20);
	ASSERT_EQ(rotor.inConvert(7),  18);
	ASSERT_EQ(rotor.inConvert(8),   2);
	ASSERT_EQ(rotor.inConvert(9),   0);
	ASSERT_EQ(rotor.inConvert(10), 12);
	ASSERT_EQ(rotor.inConvert(11),  8);
	ASSERT_EQ(rotor.inConvert(12),  5);
	ASSERT_EQ(rotor.inConvert(13), 17);
	ASSERT_EQ(rotor.inConvert(14), 21);
	ASSERT_EQ(rotor.inConvert(15), 19);
	ASSERT_EQ(rotor.inConvert(16), 15);
	ASSERT_EQ(rotor.inConvert(17), 13);
	ASSERT_EQ(rotor.inConvert(18),  4);
	ASSERT_EQ(rotor.inConvert(19), 22);
	ASSERT_EQ(rotor.inConvert(20), 10);
	ASSERT_EQ(rotor.inConvert(21),  1);
	ASSERT_EQ(rotor.inConvert(22), 11);
	ASSERT_EQ(rotor.inConvert(23), 25);
	ASSERT_EQ(rotor.inConvert(24), 24);
	ASSERT_EQ(rotor.inConvert(25),  7);

	rotor.rotation = 2;
	ASSERT_EQ(rotor.inConvert(25), 9);
}

TEST_F(RotorTest, outConvertTest) {
	ASSERT_EQ(rotor.outConvert(0),   9);
	ASSERT_EQ(rotor.outConvert(1),  21);
	ASSERT_EQ(rotor.outConvert(2),   8);
	ASSERT_EQ(rotor.outConvert(3),   2);
	ASSERT_EQ(rotor.outConvert(4),  18);
	ASSERT_EQ(rotor.outConvert(5),  12);
	ASSERT_EQ(rotor.outConvert(6),   1);
	ASSERT_EQ(rotor.outConvert(7),  25);
	ASSERT_EQ(rotor.outConvert(8),  11);
	ASSERT_EQ(rotor.outConvert(9),   0);
	ASSERT_EQ(rotor.outConvert(10), 20);
	ASSERT_EQ(rotor.outConvert(11), 22);
	ASSERT_EQ(rotor.outConvert(12), 10);
	ASSERT_EQ(rotor.outConvert(13), 17);
	ASSERT_EQ(rotor.outConvert(14),  4);
	ASSERT_EQ(rotor.outConvert(15), 16);
	ASSERT_EQ(rotor.outConvert(16),  3);
	ASSERT_EQ(rotor.outConvert(17), 13);
	ASSERT_EQ(rotor.outConvert(18),  7);
	ASSERT_EQ(rotor.outConvert(19), 15);
	ASSERT_EQ(rotor.outConvert(20),  6);
	ASSERT_EQ(rotor.outConvert(21), 14);
	ASSERT_EQ(rotor.outConvert(22), 19);
	ASSERT_EQ(rotor.outConvert(23),  5);
	ASSERT_EQ(rotor.outConvert(24), 24);
	ASSERT_EQ(rotor.outConvert(25), 23);

	rotor.rotation = 11;
	ASSERT_EQ(rotor.outConvert(0),  25);
}

TEST_F(RotorTest, updateAnimationTest)
{
	rotor.rotorWindowAnimation.startAnimation(0);
	rotor.rotorWheelAnimation.startAnimation(0);

	ASSERT_EQ(rotor.rotorWindowAnimation.uvRect.left, 0);
	ASSERT_EQ(rotor.rotorWheelAnimation.uvRect.left, 0);

	rotor.updateAnimation(0.0125f);

	ASSERT_EQ(rotor.rotorWindowAnimation.uvRect.left, 10);
	ASSERT_EQ(rotor.rotorWheelAnimation.uvRect.left, 4);
}

TEST(ReflectorTest, reflectorTest)
{
	Reflector reflector;
	int reflectorConversions[26] = { 16, 24, 7, 14, 6, 13, 4, 2, 21, 15, 20, 25, 19, 5, 3, 9, 0, 23, 22, 12, 10, 8, 18, 17, 1, 11 };
	reflector = Reflector(reflectorConversions);

	ASSERT_EQ(reflector.inConvert(0),  16);
	ASSERT_EQ(reflector.inConvert(1),  24);
	ASSERT_EQ(reflector.inConvert(2),   7);
	ASSERT_EQ(reflector.inConvert(3),  14);
	ASSERT_EQ(reflector.inConvert(4),   6);
	ASSERT_EQ(reflector.inConvert(5),  13);
	ASSERT_EQ(reflector.inConvert(6),   4);
	ASSERT_EQ(reflector.inConvert(7),   2);
	ASSERT_EQ(reflector.inConvert(8),  21);
	ASSERT_EQ(reflector.inConvert(9),  15);
	ASSERT_EQ(reflector.inConvert(10), 20);
	ASSERT_EQ(reflector.inConvert(11), 25);
	ASSERT_EQ(reflector.inConvert(12), 19);
	ASSERT_EQ(reflector.inConvert(13),  5);
	ASSERT_EQ(reflector.inConvert(14),  3);
	ASSERT_EQ(reflector.inConvert(15),  9);
	ASSERT_EQ(reflector.inConvert(16),  0);
	ASSERT_EQ(reflector.inConvert(17), 23);
	ASSERT_EQ(reflector.inConvert(18), 22);
	ASSERT_EQ(reflector.inConvert(19), 12);
	ASSERT_EQ(reflector.inConvert(20), 10);
	ASSERT_EQ(reflector.inConvert(21),  8);
	ASSERT_EQ(reflector.inConvert(22), 18);
	ASSERT_EQ(reflector.inConvert(23), 17);
	ASSERT_EQ(reflector.inConvert(24),  1);
	ASSERT_EQ(reflector.inConvert(25), 11);
}



TEST(ButtonTest, constructButtonTest)
{
	sf::RectangleShape rect(sf::Vector2f(5.f, 5.f));
	Button button(rect);
	ASSERT_EQ(rect.getGlobalBounds(), button.shape.getGlobalBounds());
	ASSERT_EQ(rect.getSize(), button.shape.getSize());
}

TEST(ButtonTest, isClickedTest)
{
	sf::RectangleShape rect(sf::Vector2f(5.f, 5.f));
	Button button(rect);

	ASSERT_TRUE (button.isClicked(1, 1));
	ASSERT_FALSE(button.isClicked(6, 1));
}



class PlugboardButtonTest : public ::testing::Test
{
protected:

	sf::RectangleShape testShape  = sf::RectangleShape(sf::Vector2f(5.f, 5.f));
	sf::RectangleShape testLetter = sf::RectangleShape(sf::Vector2f(3.f, 3.f));

	PlugboardButton plugboardButton = PlugboardButton(testShape, testLetter, 1);
};

TEST_F(PlugboardButtonTest, constructionTest)
{
	ASSERT_EQ(plugboardButton.shape.getGlobalBounds(),   testShape.getGlobalBounds());
	ASSERT_EQ(plugboardButton.shape.getSize(),			 testShape.getSize());
	ASSERT_EQ(plugboardButton.letter.getGlobalBounds(),  testLetter.getGlobalBounds());
	ASSERT_EQ(plugboardButton.letter.getSize(),			 testLetter.getSize());
	ASSERT_EQ(plugboardButton.charVal, 1);
}

TEST_F(PlugboardButtonTest, isClickedTest)
{
	ASSERT_TRUE (plugboardButton.isClicked(1, 1));
	ASSERT_FALSE(plugboardButton.isClicked(6, 1));
}

TEST_F(PlugboardButtonTest, setLetterTest)
{
	sf::Texture texture;
	texture.create(270, 15);
	plugboardButton.letter.setTexture(&texture);

	ASSERT_EQ(plugboardButton.charVal, 1);
	plugboardButton.setLetter(2);
	ASSERT_EQ(plugboardButton.charVal, 2);
}



TEST(PlugboardTest, plugboardTest)
{
	Plugboard plugboard;

	sf::RectangleShape testShape = sf::RectangleShape(sf::Vector2f(5.f, 5.f));
	sf::RectangleShape testLetter = sf::RectangleShape(sf::Vector2f(3.f, 3.f));

	PlugboardButton plugboardButton1(testShape, testLetter, 0);
	PlugboardButton plugboardButton2(testShape, testLetter, 1);
	PlugboardButton plugboardButton3(testShape, testLetter, 2);
	PlugboardButton plugboardButton4(testShape, testLetter, 3);

	plugboard.insertPair(&plugboardButton1, &plugboardButton3);
	plugboard.insertPair(&plugboardButton2, &plugboardButton4);

	ASSERT_EQ(plugboard.convert(0), 2);
	ASSERT_EQ(plugboard.convert(2), 0);
	ASSERT_EQ(plugboard.convert(1), 3);
	ASSERT_EQ(plugboard.convert(3), 1);
	ASSERT_EQ(plugboard.convert(4), 4);
}



TEST(Conversions, getConversionTest)
{
	Rotor rotor1;
	Rotor rotor2;
	Rotor rotor3;
	Reflector reflector;
	Plugboard plugboard;

	sf::RectangleShape rectangle;
	sf::Texture rotorTexture;
	sf::Texture wheelTexture;
	sf::Text text;
	std::array<int, 26> rotor3Conversions = { 9,  21, 8,  20, 1,  7,  19, 2,  3,  24, 0,  10, 4,  16, 25, 15, 14, 18, 6,  23, 13, 17, 12, 22, 5,  11 };
	std::array<int, 26> rotor2Conversions = { 13, 19, 25, 15, 18, 5,  1,  14, 10, 12, 22, 17, 2,  9,  3,  8,  21, 11, 0,  4,  24, 20, 23, 7,  6,  16 };
	std::array<int, 26> rotor1Conversions = { 9,  6,  3,  16, 14, 23, 20, 18, 2,  0,  12, 8,  5,  17, 21, 19, 15, 13, 4,  22, 10, 1,  11, 25, 24, 7  };
	int reflectorConversions[26] =			{ 16, 24, 7,  14, 6,  13, 4,  2,  21, 15, 20, 25, 19, 5,  3,  9,  0,  23, 22, 12, 10, 8,  18, 17, 1,  11 };

	rotor1 = Rotor(rotor1Conversions, 16, rectangle, &rotorTexture, rectangle, &wheelTexture, 0, text);
	rotor2 = Rotor(rotor2Conversions, 16, rectangle, &rotorTexture, rectangle, &wheelTexture, 0, text);
	rotor3 = Rotor(rotor3Conversions, 16, rectangle, &rotorTexture, rectangle, &wheelTexture, 0, text);
	reflector = Reflector(reflectorConversions);

	sf::RectangleShape testShape = sf::RectangleShape(sf::Vector2f(5.f, 5.f));
	sf::RectangleShape testLetter = sf::RectangleShape(sf::Vector2f(3.f, 3.f));

	PlugboardButton plugboardButton0 (testShape, testLetter,  0);
	PlugboardButton plugboardButton1 (testShape, testLetter,  1);
	PlugboardButton plugboardButton2 (testShape, testLetter,  2);
	PlugboardButton plugboardButton3 (testShape, testLetter,  3);
	PlugboardButton plugboardButton4 (testShape, testLetter,  4);
	PlugboardButton plugboardButton5 (testShape, testLetter,  5);
	PlugboardButton plugboardButton6 (testShape, testLetter,  6);
	PlugboardButton plugboardButton7 (testShape, testLetter,  7);
	PlugboardButton plugboardButton8 (testShape, testLetter,  8);
	PlugboardButton plugboardButton9 (testShape, testLetter,  9);
	PlugboardButton plugboardButton10(testShape, testLetter, 10);
	PlugboardButton plugboardButton11(testShape, testLetter, 11);
	PlugboardButton plugboardButton12(testShape, testLetter, 12);
	PlugboardButton plugboardButton13(testShape, testLetter, 13);
	PlugboardButton plugboardButton14(testShape, testLetter, 14);
	PlugboardButton plugboardButton15(testShape, testLetter, 15);
	PlugboardButton plugboardButton16(testShape, testLetter, 16);
	PlugboardButton plugboardButton17(testShape, testLetter, 17);
	PlugboardButton plugboardButton18(testShape, testLetter, 18);
	PlugboardButton plugboardButton19(testShape, testLetter, 19);
	PlugboardButton plugboardButton20(testShape, testLetter, 20);
	PlugboardButton plugboardButton21(testShape, testLetter, 21);
	PlugboardButton plugboardButton22(testShape, testLetter, 22);
	PlugboardButton plugboardButton23(testShape, testLetter, 23);
	PlugboardButton plugboardButton24(testShape, testLetter, 24);
	PlugboardButton plugboardButton25(testShape, testLetter, 25);

	plugboard.insertPair(&plugboardButton0,  &plugboardButton8);
	plugboard.insertPair(&plugboardButton1,  &plugboardButton12);
	plugboard.insertPair(&plugboardButton2,  &plugboardButton4);
	plugboard.insertPair(&plugboardButton3,  &plugboardButton19);
	plugboard.insertPair(&plugboardButton5,  &plugboardButton6);
	plugboard.insertPair(&plugboardButton7,  &plugboardButton17);
	plugboard.insertPair(&plugboardButton9,  &plugboardButton24);
	plugboard.insertPair(&plugboardButton10, &plugboardButton18);
	plugboard.insertPair(&plugboardButton11, &plugboardButton16);
	plugboard.insertPair(&plugboardButton13, &plugboardButton25);
	plugboard.insertPair(&plugboardButton14, &plugboardButton23);
	plugboard.insertPair(&plugboardButton15, &plugboardButton22);
	plugboard.insertPair(&plugboardButton20, &plugboardButton21);

	ASSERT_EQ(getConversion(0,  plugboard, rotor1, rotor2, rotor3, reflector), 4);
	ASSERT_EQ(getConversion(1,  plugboard, rotor1, rotor2, rotor3, reflector), 10);
	ASSERT_EQ(getConversion(2,  plugboard, rotor1, rotor2, rotor3, reflector), 3);
	ASSERT_EQ(getConversion(3,  plugboard, rotor1, rotor2, rotor3, reflector), 2);
	ASSERT_EQ(getConversion(4,  plugboard, rotor1, rotor2, rotor3, reflector), 0);
	ASSERT_EQ(getConversion(5,  plugboard, rotor1, rotor2, rotor3, reflector), 15);
	ASSERT_EQ(getConversion(6,  plugboard, rotor1, rotor2, rotor3, reflector), 20);
	ASSERT_EQ(getConversion(7,  plugboard, rotor1, rotor2, rotor3, reflector), 23);
	ASSERT_EQ(getConversion(8,  plugboard, rotor1, rotor2, rotor3, reflector), 13);
	ASSERT_EQ(getConversion(9,  plugboard, rotor1, rotor2, rotor3, reflector), 18);
	ASSERT_EQ(getConversion(10, plugboard, rotor1, rotor2, rotor3, reflector), 1);
	ASSERT_EQ(getConversion(11, plugboard, rotor1, rotor2, rotor3, reflector), 21);
	ASSERT_EQ(getConversion(12, plugboard, rotor1, rotor2, rotor3, reflector), 24);
	ASSERT_EQ(getConversion(13, plugboard, rotor1, rotor2, rotor3, reflector), 8);
	ASSERT_EQ(getConversion(14, plugboard, rotor1, rotor2, rotor3, reflector), 16);
	ASSERT_EQ(getConversion(15, plugboard, rotor1, rotor2, rotor3, reflector), 5);
	ASSERT_EQ(getConversion(16, plugboard, rotor1, rotor2, rotor3, reflector), 14);
	ASSERT_EQ(getConversion(17, plugboard, rotor1, rotor2, rotor3, reflector), 22);
	ASSERT_EQ(getConversion(18, plugboard, rotor1, rotor2, rotor3, reflector), 9);
	ASSERT_EQ(getConversion(19, plugboard, rotor1, rotor2, rotor3, reflector), 25);
	ASSERT_EQ(getConversion(20, plugboard, rotor1, rotor2, rotor3, reflector), 6);
	ASSERT_EQ(getConversion(21, plugboard, rotor1, rotor2, rotor3, reflector), 11);
	ASSERT_EQ(getConversion(22, plugboard, rotor1, rotor2, rotor3, reflector), 17);
	ASSERT_EQ(getConversion(23, plugboard, rotor1, rotor2, rotor3, reflector), 7);
	ASSERT_EQ(getConversion(24, plugboard, rotor1, rotor2, rotor3, reflector), 12);
	ASSERT_EQ(getConversion(25, plugboard, rotor1, rotor2, rotor3, reflector), 19);
}

TEST(Conversions, getCharIndexTest)
{
	ASSERT_EQ(getCharIndex('A'), 0);
	ASSERT_EQ(getCharIndex('M'), 12);
	ASSERT_EQ(getCharIndex('Z'), 25);
}

TEST(Conversions, getKeyFromIndexTest)
{
	ASSERT_EQ(getKeyFromIndex(0),  'A');
	ASSERT_EQ(getKeyFromIndex(12), 'M');
	ASSERT_EQ(getKeyFromIndex(25), 'Z');
}

TEST(Conversions, getKeyFromEventTest)
{
	sf::Event event;
	event.key.code = sf::Keyboard::A;
	ASSERT_EQ(getKeyFromEvent(&event), 'A');
	event.key.code = sf::Keyboard::B;
	ASSERT_EQ(getKeyFromEvent(&event), 'B');
	event.key.code = sf::Keyboard::C;
	ASSERT_EQ(getKeyFromEvent(&event), 'C');
	event.key.code = sf::Keyboard::D;
	ASSERT_EQ(getKeyFromEvent(&event), 'D');
	event.key.code = sf::Keyboard::E;
	ASSERT_EQ(getKeyFromEvent(&event), 'E');
	event.key.code = sf::Keyboard::F;
	ASSERT_EQ(getKeyFromEvent(&event), 'F');
	event.key.code = sf::Keyboard::G;
	ASSERT_EQ(getKeyFromEvent(&event), 'G');
	event.key.code = sf::Keyboard::H;
	ASSERT_EQ(getKeyFromEvent(&event), 'H');
	event.key.code = sf::Keyboard::I;
	ASSERT_EQ(getKeyFromEvent(&event), 'I');
	event.key.code = sf::Keyboard::J;
	ASSERT_EQ(getKeyFromEvent(&event), 'J');
	event.key.code = sf::Keyboard::K;
	ASSERT_EQ(getKeyFromEvent(&event), 'K');
	event.key.code = sf::Keyboard::L;
	ASSERT_EQ(getKeyFromEvent(&event), 'L');
	event.key.code = sf::Keyboard::M;
	ASSERT_EQ(getKeyFromEvent(&event), 'M');
	event.key.code = sf::Keyboard::N;
	ASSERT_EQ(getKeyFromEvent(&event), 'N');
	event.key.code = sf::Keyboard::O;
	ASSERT_EQ(getKeyFromEvent(&event), 'O');
	event.key.code = sf::Keyboard::P;
	ASSERT_EQ(getKeyFromEvent(&event), 'P');
	event.key.code = sf::Keyboard::Q;
	ASSERT_EQ(getKeyFromEvent(&event), 'Q');
	event.key.code = sf::Keyboard::R;
	ASSERT_EQ(getKeyFromEvent(&event), 'R');
	event.key.code = sf::Keyboard::S;
	ASSERT_EQ(getKeyFromEvent(&event), 'S');
	event.key.code = sf::Keyboard::T;
	ASSERT_EQ(getKeyFromEvent(&event), 'T');
	event.key.code = sf::Keyboard::U;
	ASSERT_EQ(getKeyFromEvent(&event), 'U');
	event.key.code = sf::Keyboard::V;
	ASSERT_EQ(getKeyFromEvent(&event), 'V');
	event.key.code = sf::Keyboard::W;
	ASSERT_EQ(getKeyFromEvent(&event), 'W');
	event.key.code = sf::Keyboard::X;
	ASSERT_EQ(getKeyFromEvent(&event), 'X');
	event.key.code = sf::Keyboard::Y;
	ASSERT_EQ(getKeyFromEvent(&event), 'Y');
	event.key.code = sf::Keyboard::Z;
	ASSERT_EQ(getKeyFromEvent(&event), 'Z');
	event.key.code = sf::Keyboard::Comma;
	ASSERT_EQ(getKeyFromEvent(&event), ' ');
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}