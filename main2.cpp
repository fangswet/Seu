//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <deque>
//#include <stdlib.h>
//
//enum class Binding
//{
//	Left = 1 << 0,
//	Right = 1 << 1,
//	Top = 1 << 2,
//	Bottom = 1 << 3,
//	X = Left | Right,
//	Y = Top | Bottom,
//	Rect = X | Y
//};
//
//int operator&(Binding lhs, Binding rhs)
//{
//	return (int)lhs & (int)rhs;
//}
//
//// origin of entity is always center
//class Entity : public sf::Sprite
//{
//public:
//	Entity(const sf::Texture& texture) : sf::Sprite(texture)
//	{
//		recenterOrigin();
//	}
//	Entity(const sf::Texture& texture, const sf::IntRect& rectangle) : sf::Sprite(texture, rectangle)
//	{
//		recenterOrigin();
//	}
//	Entity()
//	{ }
//
//	sf::Vector2f getCenter();
//	void recenterOrigin();
//	void bind(const sf::FloatRect&, Binding);
//	void bind(const sf::RenderWindow&, Binding);
//};
//
//sf::Vector2f Entity::getCenter()
//{
//	return sf::Vector2f(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f);
//}
//
//void Entity::recenterOrigin()
//{
//	setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
//}
//
//// this basically works but would be tidier if origin was center always ( because both scenarios do the same work)
//void Entity::bind(const sf::FloatRect& bounds, Binding type = Binding::Rect)
//{
//	sf::Vector2f origin(getOrigin().x * getScale().x, getOrigin().y * getScale().y);
//	auto offset = sf::Vector2f(bounds.left, bounds.top) + origin;
//	auto position = getPosition() - offset;
//	// getPosition() - origin = getCorner()
//	// getPosition() - bounds = relatePosition()
//	// or smth like dat
//
//	if (type & Binding::Left && position.x < 0)
//	{
//		position.x = 0;
//	}
//	if (type & Binding::Right && position.x > bounds.width - getGlobalBounds().width) // maybe needs else
//	{
//		position.x = bounds.width - getGlobalBounds().width;
//	}
//
//	if (type & Binding::Top && position.y < 0)
//	{
//		position.y = 0;
//	}
//	if (type & Binding::Bottom && position.y > bounds.height - getGlobalBounds().height)
//	{
//		position.y = bounds.height - getGlobalBounds().height;
//	}
//
//	setPosition(position + offset);
//}
//
//// idk if possible but we could do an overload with view.getSize()
//// because with this the current view is implied
//void Entity::bind(const sf::RenderWindow& window, Binding type = Binding::Rect)
//{
//	auto windowSize = (sf::Vector2f)window.getSize();
//	auto viewport = window.getView().getViewport();
//	bind(sf::FloatRect(windowSize.x * viewport.left, windowSize.y * viewport.top, windowSize.x * viewport.width, windowSize.y * viewport.height), type);
//}
//
//class ClippingEntity : public Entity
//{
//public:
//	ClippingEntity(const sf::Texture& texture) : Entity(texture) 
//	{ }
//	ClippingEntity(const sf::Texture& texture, const sf::IntRect& rectangle) : Entity(texture, rectangle)
//	{ }
//
//	std::unique_ptr<Entity> clip(const sf::IntRect&, Binding type);
//	void clip(sf::RenderWindow&, Binding type);
//	void draw(sf::RenderWindow&, const sf::RenderStates);
//};
//
//// this works but using Rect would require a special case where one of the axes is flipped for coherency
//// so fix later now use x or y
//std::unique_ptr<Entity> ClippingEntity::clip(const sf::IntRect& bounds, Binding type = Binding::Rect)
//{
//	// get position of corner, add center = position of center
//	// figure out how to skip corner and go from origin to center
//	sf::Vector2f origin(getOrigin().x * getScale().x, getOrigin().y * getScale().y);
//	auto cornerPosition = getPosition() - origin;
//	auto centerPosition = cornerPosition + getCenter();
//	auto adjustedCenterPosition = centerPosition - sf::Vector2f(bounds.left, bounds.top);
//
//	sf::Vector2f clonePosition(adjustedCenterPosition);
//	bool drawClone = false;
//
//	// we should generalize that because the relative positioning allows for that
//	// and also this is too much the same code
//	if (type & Binding::Left && adjustedCenterPosition.x < origin.x)
//	{
//		drawClone = true;
//
//		if (adjustedCenterPosition.x <= 1)
//		{
//			clonePosition = getPosition();
//			setPosition(bounds.width - 3, clonePosition.y);
//		}
//		else
//		{
//			clonePosition.x = bounds.width + adjustedCenterPosition.x;
//		}
//	}
//	else if (type & Binding::Right && adjustedCenterPosition.x > bounds.width - origin.x)
//	{
//		drawClone = true;
//
//		if (adjustedCenterPosition.x >= bounds.width - 1)
//		{
//			clonePosition = getPosition();
//			setPosition(3, clonePosition.y);
//		}
//		else
//		{
//			clonePosition.x = -((int)bounds.width - adjustedCenterPosition.x);
//		}
//	}
//	
//	if (type & Binding::Top && adjustedCenterPosition.y < origin.y)
//	{
//		drawClone = true;
//
//		if (adjustedCenterPosition.y <= 1)
//		{
//			clonePosition = getPosition();
//			setPosition(clonePosition.x, bounds.height - 3);
//		}
//		else
//		{
//			clonePosition.y = bounds.height + adjustedCenterPosition.y;
//		}
//	}
//	else if (type & Binding::Bottom && adjustedCenterPosition.y > bounds.height - origin.y)
//	{
//		drawClone = true;
//
//		if (adjustedCenterPosition.y >= bounds.height - 1)
//		{
//			clonePosition = getPosition();
//			setPosition(clonePosition.x, 3);
//		}
//		else
//		{
//			clonePosition.y = -((int)bounds.height - adjustedCenterPosition.y);
//		}
//	}
//
//	std::unique_ptr<Entity> clone = nullptr;
//
//	if (drawClone)
//	{
//		clone = std::unique_ptr<Entity>(new Entity(*this));
//		clone->setPosition(clonePosition);
//	}
//
//	return clone;
//}
//
//int main()
//{
//	// we need to understand window scaling
//	sf::RenderWindow window(sf::VideoMode(1920, 1080), "", sf::Style::Fullscreen);
//	window.setVerticalSyncEnabled(true);
//	sf::View backgroundView(sf::FloatRect(0, 0, 1920, 1080));
//	sf::FloatRect gameplayBounds(0, 0, 1728, 1080);
//	sf::View gameplayView(gameplayBounds);
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
//	sf::Sprite playerSprite(playerTexture);
//	const auto playerBounds = playerSprite.getLocalBounds();
//	playerSprite.setOrigin(playerBounds.width / 2, playerBounds.height / 2);
//	playerSprite.setScale(.1, .1);
//	playerSprite.setPosition(gameplayView.getSize().x / 2, gameplayView.getSize().y / 2);
//
//	// we can either make clippablentity accept a prefoiusly constructed sprite / entity
//	// or inherit entity and swap the entites position manually not swap <- this is more logical i thiunk because the api stays the same
//	ClippingEntity playerEntity(playerTexture);
//	playerEntity.setScale(.1, .1);
//	playerEntity.setPosition(gameplayView.getSize().x / 2, gameplayView.getSize().y / 2);
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
//		playerEntity.setPosition(mousePosition.x, mousePosition.y);
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
//			playerEntity.move(offset);
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
//				seagull->setPosition(playerEntity.getPosition().x, playerEntity.getPosition().y - playerEntity.getOrigin().y);
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
//		text.setString("x:" + std::to_string(mousePosition.x) + 
//			"\ny:" + std::to_string(mousePosition.y) + 
//			"\nex:" + std::to_string((int)playerEntity.getPosition().x) + 
//			"\ney:" + std::to_string((int)playerEntity.getPosition().y));
//
//		window.clear(sf::Color::Black);
//		window.setView(backgroundView);
//		backgroundView.move(0, -5);
//		window.draw(backgroundSprite);
//		window.setView(sidebarView);
//		window.draw(text);
//		window.setView(gameplayView);
//		//controlKeyboard();
//		if (!mouseLocked) controlMouse();
//		playerEntity.bind(sf::FloatRect(960, 540, 960, 540), Binding::Y);
//		//shoot();
//		window.draw(playerSprite);
//		window.draw(playerEntity);
//		//auto clone = playerEntity.clip((sf::IntRect)gameplayBounds, Binding::X);
//		//sf::Mouse::setPosition((sf::Vector2i)playerEntity.getPosition());
//		//if (clone != nullptr) window.draw(*clone);
//		window.display();
//	}
//
//	return 0;
//}