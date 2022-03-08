//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <deque>
//#include <stdlib.h>
//
//// do entity of sprite and clippinentity of drawable with 2 entities
//// perhaps clipping should be aproeprty of a view (we would have a clipping and a non clipping view)
//
//int main1()
//{
//	sf::RenderWindow window(sf::VideoMode(1920, 1080), "", sf::Style::Fullscreen);
//	window.setVerticalSyncEnabled(true);
//	sf::View backgroundView(sf::FloatRect(0, 0, 1920, 1080));
//	sf::View gameplayView(sf::FloatRect(0, 0, 1728, 1080));
//	sf::View sidebarView(sf::FloatRect(0, 0, 192, 1080));
//	gameplayView.setViewport(sf::FloatRect(0, 0, .9, 1));
//	sidebarView.setViewport(sf::FloatRect(.9, 0, .1, 1));
//
//	sf::Font font;
//	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
//	sf::Text text;
//	text.setFont(font);
//	text.setFillColor(sf::Color::Red);
//	text.setString("chuj");
//
//	sf::Texture backgroundTexture;
//	backgroundTexture.loadFromFile("./image5.jpg");
//	backgroundTexture.setRepeated(true);
//
//	sf::Texture playerTexture;
//	playerTexture.loadFromFile("./image.jpg");
//
//	sf::Texture seagullTexture;
//	seagullTexture.loadFromFile("./image2.jpg");
//
//	sf::Sprite backgroundSprite(backgroundTexture, sf::IntRect(0, 0, 1920, 2000));
//	backgroundSprite.setOrigin(0, 2000);
//	backgroundSprite.setPosition(0, gameplayView.getSize().y);
//
//	auto playerSprite = new sf::Sprite(playerTexture);
//	auto cloneSprite = new sf::Sprite(playerTexture);
//	const auto playerBounds = playerSprite->getLocalBounds();
//	playerSprite->setOrigin(playerBounds.width / 2, playerBounds.height / 2);
//	cloneSprite->setOrigin(playerBounds.width / 2, playerBounds.height / 2);
//	playerSprite->setScale(.1, .1); 
//	cloneSprite->setScale(.1, .1);
//	playerSprite->setPosition(gameplayView.getSize().x / 2, gameplayView.getSize().y / 2);
//	const sf::Vector2f playerOrigin(playerSprite->getOrigin().x * .1, playerSprite->getOrigin().y * .1);
//
//	std::deque<sf::Sprite*> seagulls;
//	int seagullTimer = 0;
//
//	bool mouseLocked = false;
//	sf::Event event;
//
//	auto controlMouse = [&]()
//	{
//		auto mousePosition = sf::Mouse::getPosition();
//		playerSprite->setPosition(mousePosition.x, mousePosition.y);
//	};
//
//	auto controlKeyboard = [&]()
//	{
//		sf::Vector2f offset;
//
//		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
//		{
//			offset.y -= 10;
//		}
//		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
//		{
//			offset.y += 10;
//		}
//		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
//		{
//			offset.x -= 10;
//		}
//		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
//		{
//			offset.x += 10;
//		}
//
//		if (offset.x != 0 || offset.y != 0)
//		{
//			mouseLocked = true;
//			playerSprite->move(offset);
//		}
//	};
//
//	auto shoot = [&]()
//	{
//		seagullTimer--;
//
//		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
//		{
//			if (seagullTimer <= 0)
//			{
//				auto seagull = new sf::Sprite(seagullTexture);
//				seagull->setOrigin(seagull->getLocalBounds().width / 2.f, 0);
//				seagull->setScale(.1, .1);
//				seagull->setPosition(playerSprite->getPosition().x, playerSprite->getPosition().y - playerOrigin.y);
//				seagulls.push_back(seagull);
//				seagullTimer = 30;
//			}
//		}
//
//		// seagull rendering step
//		if (!seagulls.empty())
//		{
//			if (seagulls.front()->getPosition().y < 0 - seagulls.front()->getTexture()->getSize().y * seagulls.front()->getScale().y)
//			{
//				delete seagulls.front();
//				seagulls.pop_front();
//			}
//
//			for (const auto s : seagulls)
//			{
//				s->move(0, -10);
//				window.draw(*s);
//			}
//		}
//	};
//
//	auto bind = [&]()
//	{
//		auto position = playerSprite->getPosition();
//
//		if (position.y < playerOrigin.y)
//		{
//			playerSprite->setPosition(position.x, playerOrigin.y);
//		}
//		else if (position.y > gameplayView.getSize().y - playerOrigin.y)
//		{
//			playerSprite->setPosition(position.x, gameplayView.getSize().y - playerOrigin.y);
//		}
//	};
//
//	auto clip = [&]()
//	{
//		auto position = playerSprite->getPosition();
//
//		if (position.x < playerOrigin.x)
//		{
//			cloneSprite->setPosition(gameplayView.getSize().x + position.x, position.y);
//			window.draw(*cloneSprite);
//
//			if (position.x <= 1)
//			{
//				sf::Vector2f newPosition(gameplayView.getSize().x - 3, cloneSprite->getPosition().y);
//				std::swap(playerSprite, cloneSprite);
//				playerSprite->setPosition(newPosition);
//
//				if (!mouseLocked) sf::Mouse::setPosition((sf::Vector2i)newPosition);
//			}
//		}
//		else if (position.x > gameplayView.getSize().x - playerOrigin.x)
//		{
//			cloneSprite->setPosition(-((int)gameplayView.getSize().x - position.x), position.y);
//			window.draw(*cloneSprite);
//
//			if (position.x >= gameplayView.getSize().x - 1)
//			{
//				sf::Vector2f newPosition(3, cloneSprite->getPosition().y);
//				std::swap(playerSprite, cloneSprite);
//				playerSprite->setPosition(newPosition);
//
//				if (!mouseLocked) sf::Mouse::setPosition((sf::Vector2i)newPosition);
//			}
//		}
//	};
//
//	sf::Texture enemyTexture;
//	enemyTexture.loadFromFile("./image6.jpg");
//
//	std::vector<sf::Sprite*> enemies;
//	int enemyTimer = 60 * 30;
//
//	auto enemy = [&]()
//	{
//		if (enemyTimer <= 0)
//		{
//			// find random number to spawn enemy
//
//		}
//	};
//
//	while (window.isOpen())
//	{
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//			{
//				window.close();
//			}
//			else if (event.type == sf::Event::MouseMoved)
//			{
//				mouseLocked = false;
//			}
//		}
//
//		auto mousePosition = sf::Mouse::getPosition();
//		text.setString("x:" + std::to_string(mousePosition.x) + "\ny:" + std::to_string(mousePosition.y));
//
//		window.clear(sf::Color::Black);
//		window.setView(backgroundView);
//		backgroundView.move(0, -5);
//		window.draw(backgroundSprite);
//		window.setView(sidebarView);
//		window.draw(text);
//		window.setView(gameplayView);
//		if (!mouseLocked) controlMouse();
//		controlKeyboard();
//		bind();
//		clip();
//		shoot();
//		window.draw(*playerSprite);
//		window.display();
//	}
//
//	return 0;
//}