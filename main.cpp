#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <queue>
#include <deque>
#include <optional>

// add moving steps (with speed and smoothing), collisiom
// would be also cool to add coordinate translation for clipping (using travel) (you can go from side to side rn with move)

enum class Binding
{
	Left = 1 << 0,
	Right = 1 << 1,
	Top = 1 << 2,
	Bottom = 1 << 3,
	X = Left | Right,
	Y = Top | Bottom,
	Rect = X | Y
};

int operator&(Binding lhs, Binding rhs)
{
	return (int)lhs & (int)rhs;
}

sf::Vector2f operator*(const sf::Vector2f& lhs, const sf::Vector2f& rhs)
{
	return sf::Vector2f(lhs.x * rhs.x, lhs.y * rhs.y);
}

sf::Vector2f operator/(const sf::Vector2f& lhs, const sf::Vector2f& rhs)
{
	if (rhs.x == 0 || lhs.x == 0) return sf::Vector2f();

	return sf::Vector2f(lhs.x / rhs.x, lhs.y / rhs.y);
}

sf::Vector2f operator*(const sf::Vector2f& lhs, float rhs)
{
	return sf::Vector2f(lhs.x * rhs, lhs.y * rhs);
}

sf::Vector2f operator/(const sf::Vector2f& lhs, float rhs)
{
	return sf::Vector2f(lhs.x / rhs, lhs.y / rhs);
}

sf::Vector2f operator+(const sf::Vector2f& lhs, float rhs)
{
	return sf::Vector2f(lhs.x + rhs, lhs.y + rhs);
}

sf::Vector2f operator-(const sf::Vector2f& vector)
{
	return sf::Vector2f(-vector.x, -vector.y);
}

void operator+=(sf::Vector2f& lhs, float rhs)
{
	lhs = lhs + rhs;
}

sf::Vector2f abs(const sf::Vector2f& vector)
{
	return sf::Vector2f(std::abs(vector.x), std::abs(vector.y));
}

float magnitude(const sf::Vector2f& vector)
{
	return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

float distance(const sf::Vector2f& lhs, const sf::Vector2f& rhs)
{
	return magnitude(lhs - rhs);
}

sf::Vector2f normalize(const sf::Vector2f& vector)
{
	auto m = magnitude(vector);

	return m > 0 ? vector / m : vector;
}


sf::FloatRect getViewSize(const sf::RenderWindow& window)
{
	auto windowSize = (sf::Vector2f)window.getSize();
	auto viewport = window.getView().getViewport();
	auto position = windowSize * sf::Vector2f(viewport.left, viewport.top);
	auto size = windowSize * sf::Vector2f(viewport.width, viewport.height);
	return sf::FloatRect(position, size);
}

class Entity : public sf::Sprite
{
public:
	Entity(const sf::Texture& texture) : sf::Sprite(texture)
	{
		recenterOrigin();
	}
	Entity(const sf::Texture& texture, const sf::IntRect& rectangle) : sf::Sprite(texture, rectangle)
	{
		recenterOrigin();
	}
	Entity()
	{ }

	sf::Vector2f getCenter();
	using sf::Sprite::getPosition;
	sf::Vector2f getPosition(const sf::FloatRect&);
	sf::Vector2f getPosition(const sf::Vector2f&);
	sf::Vector2f getPosition(const sf::FloatRect&, const sf::Vector2f&);
	using sf::Sprite::setPosition;
	void setPosition(const sf::Vector2f&, const sf::FloatRect&);
	void setPosition(const sf::Vector2f&, const sf::Vector2f&);
	void setPosition(const sf::Vector2f&, const sf::FloatRect&, const sf::Vector2f&);
	void recenterOrigin();
	void bind(const sf::FloatRect&, Binding);
	void bind(const sf::RenderWindow&, Binding);
	std::unique_ptr<Entity> clip(const sf::FloatRect&, Binding);
	std::unique_ptr<Entity> clip(const sf::RenderWindow&, Binding);
};

sf::Vector2f Entity::getCenter()
{
	return sf::Vector2f(getGlobalBounds().width / 2.f, getGlobalBounds().height / 2.f);
}

void Entity::recenterOrigin()
{
	setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
}

sf::Vector2f Entity::getPosition(const sf::Vector2f& point)
{
	return getPosition() + point - getOrigin() * getScale();
}

sf::Vector2f Entity::getPosition(const sf::FloatRect& bounds)
{
	return getPosition() - sf::Vector2f(bounds.left, bounds.top);
}

sf::Vector2f Entity::getPosition(const sf::FloatRect& bounds, const sf::Vector2f& point)
{
	return getPosition(point) - sf::Vector2f(bounds.left, bounds.top);
}

void Entity::setPosition(const sf::Vector2f& position, const sf::FloatRect& bounds)
{
	setPosition(position + sf::Vector2f(bounds.left, bounds.top));
}

// moving a particular point of a sprite to a particular location has been deemed impossible for ages
void Entity::setPosition(const sf::Vector2f& position, const sf::Vector2f& point)
{
	auto origin = getOrigin();
	//sf::Vector2f scaledPoint(point.x / scale.x, point.y / scale.y); // idk if to use that

	setOrigin(0, 0);
	setPosition(position - point + origin * getScale());
	setOrigin(origin);
}

void Entity::setPosition(const sf::Vector2f& position, const sf::FloatRect& bounds, const sf::Vector2f& point)
{
	setPosition(position, point);
	move(sf::Vector2f(bounds.left, bounds.top));
}

void Entity::bind(const sf::FloatRect& bounds, Binding type = Binding::Rect)
{
	auto position = getPosition(bounds, sf::Vector2f(0, 0));

	if (type & Binding::Left && position.x < 0)
	{
		position.x = 0;
	}
	else if (type & Binding::Right && position.x > bounds.width - getGlobalBounds().width)
	{
		position.x = bounds.width - getGlobalBounds().width;
	}

	if (type & Binding::Top && position.y < 0)
	{
		position.y = 0;
	}
	else if (type & Binding::Bottom && position.y > bounds.height - getGlobalBounds().height)
	{
		position.y = bounds.height - getGlobalBounds().height;
	}

	setPosition(position, bounds, sf::Vector2f(0,0));
}

void Entity::bind(const sf::RenderWindow& window, Binding type = Binding::Rect)
{
	bind(getViewSize(window), type);
}

std::unique_ptr<Entity> Entity::clip(const sf::FloatRect& bounds, Binding type = Binding::X)
{
	auto center = getCenter();
	auto position = getPosition(bounds, center);
	bool drawClone = false;

	sf::Vector2f clonePosition(position);
	std::unique_ptr<Entity> clone = nullptr;

	if (type & Binding::Left && position.x < center.x)
	{
		drawClone = true;

		if (position.x <= 1)
		{
			clonePosition = position;
			position.x = bounds.width - 3;
		}
		else
		{
			clonePosition.x = bounds.width + position.x;
		}
	}
	else if (type & Binding::Right && position.x > bounds.width - center.x)
	{
		drawClone = true;

		if (position.x >= bounds.width - 1)
		{
			clonePosition = position;
			position.x = 3;
		}
		else
		{
			clonePosition.x = -((int)bounds.width - position.x);
		}
	}

	if (type & Binding::Top && position.y < center.y)
	{
		drawClone = true;

		if (position.y <= 1)
		{
			clonePosition = position;
			position.y = bounds.height - 3;
		}
		else
		{
			clonePosition.y = bounds.height + position.y;
		}
	}
	else if (type & Binding::Bottom && position.y > bounds.height - center.y)
	{
		drawClone = true;

		if (position.y >= bounds.height - 1)
		{
			clonePosition = position;
			position.y = 3;
		}
		else
		{
			clonePosition.y = -((int)bounds.height - position.y);
		}
	}

	if (drawClone)
	{
		setPosition(position, bounds, center);
		clone = std::unique_ptr<Entity>(new Entity(*this));
		clone->setPosition(clonePosition, bounds);
	}

	return clone;
}

std::unique_ptr<Entity> Entity::clip(const sf::RenderWindow& window, Binding type = Binding::X)
{
	return clip(getViewSize(window), type);
}

// warning none of this will make sense if we want to apply multiple movements to an entity
struct Velocity
{
	float initial;
	float target;

	Velocity(float initial, float target) : initial(initial), target(target)
	{ }

	Velocity(float velocity) : Velocity(velocity, velocity)
	{ }

	// https://sciencing.com/acceleration-velocity-distance-7779124.html
	float getAcceleration(float distance) const
	{
		if (distance == 0) return 0;
		return (std::pow(target, 2) - std::pow(initial, 2)) / (distance * 2);
	}

	Velocity reverse() const
	{
		return Velocity(target, initial);
	}
};

struct Accelerate : public Velocity
{
	Accelerate(float target) 
		: Velocity { target >= 0 ? Velocity(0, target) : Velocity(target, 0) }
	{ }
};

struct MovementInfo
{
	sf::Vector2f offset;
	Velocity velocity;
	bool isAbsolute;


	MovementInfo(const sf::Vector2f& offset, const Velocity& velocity, bool isAbsolute = false)
		: offset(offset), velocity(velocity), isAbsolute(isAbsolute)
	{ }

	sf::Vector2f getRelativeOffset(const sf::Vector2f&) const;
};

sf::Vector2f MovementInfo::getRelativeOffset(const sf::Vector2f& position) const
{
	return isAbsolute ? offset - position : offset;
}

struct AbsoluteMovementInfo : public MovementInfo
{
	AbsoluteMovementInfo(const sf::Vector2f& offset, const Velocity& velocity)
		: MovementInfo(offset, velocity, true)
	{ }
};

struct Applicable
{
	virtual bool isFinished() const = 0;
	virtual bool apply(Entity&, double) = 0;
};

// we could use relative offset then we dont need distance distanceLeft secondsLeft direction
// which is a big step towards real time movement
// and instead we could store MovementInfo which could be beneficial for example when velocity is 0 and position absolute
// would it be wrong to say that a real time movement could only be absolute?
// because only when you have a target does it matter to keep track of distance etc in  real time
// so we should probably interface this and make 2 different movements
struct Movement
{
	sf::Vector2f direction;
	float acceleration;
	float distance;
	float distanceLeft;
	float secondsLeft;
	float currentVelocity;

	Movement(const MovementInfo& info, const Entity& entity)
	{
		currentVelocity = info.velocity.initial;
		auto offset = info.getRelativeOffset(entity.getPosition());
		direction = normalize(offset);
		distance = distanceLeft = magnitude(abs(offset));
		acceleration = info.velocity.getAcceleration(distanceLeft);
		secondsLeft = getDuration(info.velocity, distanceLeft, acceleration);
	}

	MovementInfo reverse();
	virtual bool apply(Entity&, double);

	bool isFinished() const
	{
		return secondsLeft <= 0;
	}

private:
	static float getDuration(const Velocity&, float, float);
	bool started = false;
};

// if we hold movementinfo this isnt static
float Movement::getDuration(const Velocity& velocity, float distance, float acceleration)
{
	if (acceleration == 0)
	{
		return distance / velocity.target;
	}
	else
	{
		// https://www.dummies.com/education/science/physics/how-to-calculate-time-and-distance-from-acceleration-and-velocity/
		auto x = std::sqrt(2 * distance * acceleration + (float)std::pow(velocity.initial, 2));
		auto t1 = (-velocity.initial + x) / acceleration;
		auto t2 = -((velocity.initial + x) / acceleration);

		if (t1 >= 0) return t1;
		if (t2 >= 0) return t2;
	}

	return 0;
}

MovementInfo Movement::reverse()
{
	auto newDistance = distance;
	if (!isFinished()) newDistance -= distanceLeft;

	// https://stackoverflow.com/questions/6247153/angle-from-2d-unit-vector
	auto angle = std::atan2(direction.y, direction.x);
	// https://www.physicsgoeasy.com/vector-components-from-magnitude-and-angle/
	auto offset = -(sf::Vector2f(std::cos(angle), std::sin(angle))* distance);

	if (acceleration == 0)
	{
		return MovementInfo(offset, Velocity(currentVelocity));
	}
	else
	{
		return MovementInfo(offset, Accelerate(currentVelocity - acceleration));
	}
}

// move center not origiin
bool Movement::apply(Entity& entity, double delta)
{
	if (isFinished()) return true;

	currentVelocity += acceleration * (float)delta;
	auto distance = direction * currentVelocity * delta;
	distanceLeft -= magnitude(abs(distance));
	secondsLeft -= delta;
	entity.move(distance);

	return false;
}

struct PathInfo
{
	std::deque<MovementInfo> path;

	void add(const MovementInfo&);
	void addMovement(const sf::Vector2f&, const Velocity&);
	void addDestination(const sf::Vector2f&, const Velocity&);
	PathInfo mirror() const;
};

void PathInfo::add(const MovementInfo& movementInfo)
{
	path.push_back(movementInfo);
}

void PathInfo::addMovement(const sf::Vector2f& offset, const Velocity& velocity)
{
	add(MovementInfo(offset, velocity));
}

void PathInfo::addDestination(const sf::Vector2f& destination, const Velocity& velocity)
{
	add(MovementInfo(destination, velocity, true));
}

PathInfo PathInfo::mirror() const
{
	PathInfo mirrorPath(*this);
	std::queue<MovementInfo> reversePath;

	for (auto it = path.crbegin(); it != path.crend(); ++it) 
	{
		MovementInfo info(*it);
		if (!info.isAbsolute) info.offset = -info.offset;
		reversePath.push(info);
	}

	for (; !reversePath.empty(); reversePath.pop())
	{
		mirrorPath.add(reversePath.front());
	}

	return mirrorPath;
}

class Path : Applicable
{
public:
	std::deque<MovementInfo> path;
	std::unique_ptr<Movement> currentMovement;
	bool loop;

	Path(const PathInfo& pathInfo, bool loop = false) : loop(loop), path(pathInfo.path)
	{ }

	bool isFinished() const override;
	bool apply(Entity&, double) override;
};

bool Path::isFinished() const
{
	return path.empty() && currentMovement == nullptr;
}

// check if looping and there is only one absolute movement
bool Path::apply(Entity& entity, double delta)
{
	if (currentMovement == nullptr)
	{
		if (path.empty()) return false;

		auto& movementInfo = path.front();
		currentMovement = std::unique_ptr<Movement>(new Movement(movementInfo, entity));

		if (loop) path.push_back(movementInfo);

		path.pop_front();
	}

	if (currentMovement->isFinished())
	{
		currentMovement = nullptr;
		return apply(entity, delta);
	}

	// we can check here if finished not like up there
	return currentMovement->apply(entity, delta);
}

// extend path with looping, backtrack
// write nestedpath or smth and try merging with path
// write compoundmovement or smth on top of that for accdec
// write circular movement
// final boss is rewriting Movement with real-time calculations to allow multiple movements at the same time for simulating forces

int main()
{
	sf::RenderWindow window(sf::VideoMode(960, 540), "");
	//window.setPosition(sf::Vector2i(960, 540));
	window.setVerticalSyncEnabled(true);
	//window.setFramerateLimit(30);

	sf::Texture playerTexture;
	playerTexture.loadFromFile("./image5.jpg"); // 1920x1080

	//sf::FloatRect bounds(480, 270, 480, 270);
	
	Entity playerEntity(playerTexture);
	playerEntity.setScale(.1, .1);
	//playerEntity.setPosition(sf::Vector2f(0, 540), sf::Vector2f(0, 108));

	auto info1 = MovementInfo(sf::Vector2f(960 - 192, 0), Velocity(500));
	auto info2 = MovementInfo(sf::Vector2f(0, 540 - 108), Velocity(500));
	auto info3 = MovementInfo(sf::Vector2f(-(960 - 192), 0), Velocity(500));
	auto info4 = MovementInfo(sf::Vector2f(0, -(540 - 108)), Velocity(500));
	auto movement1 = Movement(info2, playerEntity);
	auto movement2 = Movement(info3, playerEntity);

	//Path path;
	//path.add(info1);
	//path.add(info2);
	//path.add(info3);
	//path.add(info4);

	PathInfo pathInfo;
	pathInfo.add(info1);
	pathInfo.add(info2);
	pathInfo.add(info3);
	pathInfo.add(info4);
	Path path(pathInfo.mirror(), true);

	sf::Event event;
	sf::Clock timer;

	int i = 0;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		auto delta = timer.restart().asSeconds();

		window.clear(sf::Color::Black);

		//movement.apply(playerEntity, delta);
		//if (movement.isFinished())
		//{
		//	movement = Movement(movement.reverse(), playerEntity);
		//}

		//path.apply(playerEntity, delta);
		auto deg = 20 * (M_PI / 180);
		playerEntity.move(100 * std::sin(deg) * delta, 100 * std::cos(deg) * delta);
		window.draw(playerEntity);
		window.display();

		i++;
	}

	return 0;
}

//auto mousePosition = sf::Mouse::getPosition(window);
//playerEntity.setPosition(mousePosition.x, mousePosition.y);
//playerEntity.bind(window, Binding::Y);
//auto clone = playerEntity.clip(window);
//if (clone != nullptr)
//{
//	window.draw(*clone);
//	sf::Mouse::setPosition((sf::Vector2i)playerEntity.getPosition(), window);
//}