# include <Siv3D.hpp> // Siv3D v0.6.12

class InputDirector
{
public:
	Vec2 dir;
	int maxDistance;

	InputDirector()
	{
		maxDistance = 100;
		PinchStart = Point(0, 0);
	}

	void Update()
	{
		dir = Vec2(0,0);
		if (MouseL.down())
		{
			PinchStart = Cursor::Pos();
		}
		if (MouseL.pressed())
		{
			Point relate = Cursor::Pos() - PinchStart;
			Vec2 relateF = Vec2(relate);
			dir = relateF / maxDistance;
		}
	}

	void Draw()
	{

		if (MouseL.pressed())
		{
			Circle{ PinchStart ,maxDistance }.drawFrame(2.0, Palette::Lightgrey);
			Circle{ Cursor::Pos(), 20 }.drawFrame(5.0, Palette::White);
		}
	}
private:
	Point PinchStart;

};

InputDirector* inputDirector = new InputDirector();

class Actor
{

public:
	Vec2 pos;
	Rect* collision;
	String name;
	bool isDestroyed;

	Actor(String _name) {
		name = _name;
	}
	virtual void Update()
	{
		if (collision == nullptr) return;
		collision->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));
	}
	virtual void Draw()
	{
		if (collision == nullptr) return;
		collision->drawFrame(2.0, Palette::Greenyellow);
	}

private:

};

class Player : public Actor
{

public:
	float dir;
	float lastDir;
	Vec2 vel;
	Rect* leg;

	Player(String _name) : Actor(_name) {
		walkSpeed = 100;
		attackSpeed = 1000;
		repelInputAcceptDuration = 0.1s;
		attackingDuration = 0.2s;
		damageDuration = 2.0s;
		invTimeDuration = 1.0s;
		repelInputTimer.set(repelInputAcceptDuration);
		attackingTimer.set(attackingDuration);
		damagedTimer.set(damageDuration);
		invTimeTimer.set(invTimeDuration);

		pos = Scene::Center();
		collision = new Rect(100, 200);
		leg = new Rect(200, 40);
	}
	virtual void Update() override
	{
		leg->setPos(0, -1000);
		dir = 0;
		if (invTimeTimer.reachedZero())
		{
			invTimeTimer.reset();
			canHit = true;
		}
		else
		{
			if (invTimeTimer.isStarted())
			{
				canHit = false;
			}
		}
		if (state == U"damage")
		{
			vel.y += 900 * Scene::DeltaTime();

			if (pos.y > damagedPos.y)
			{
				vel = Vec2{ 0,0 };
			}

			if (damagedTimer.reachedZero())
			{
				damagedTimer.reset();
				state = U"default";
				invTimeTimer.restart();
			}
		}
		else
		{
			if (attackingTimer.reachedZero())
			{
				attackingTimer.reset();
				attackFootHeight = 0;
				canRepel = true;

				repelInputTimer.reset();
				state = U"default";
				invTimeTimer.restart();
			}
			if (attackingTimer.isRunning())
			{
				canHit = false;
				state = U"attack";
				vel.x = lastDir * attackSpeed;
				vel.y = 0;
				Point legpos = Point{ 0,(int32)(10 * attackFootHeight) - 50 };
				if (lastDir > 0)
				{
					leg->setPos(Arg::leftCenter(legpos + Point{ (int32)pos.x,(int32)pos.y }));
				}
				else
				{
					leg->setPos(Arg::rightCenter(legpos + Point{ (int32)pos.x,(int32)pos.y }));
				}


			}
			else
			{
				vel.y = inputDirector->dir.y * walkSpeed;
				vel.x = 0;

				dir = Sign(inputDirector->dir.x);

				if (Abs(inputDirector->dir.x) <= 0.1)
				{
					repelInputTimer.reset();
				}
				if (canRepel)
				{
					if (Abs(inputDirector->dir.x) > 0.1 && Abs(inputDirector->dir.x) < 0.9)
					{
						if (!repelInputTimer.isStarted())
						{
							repelInputTimer.restart();
						}
					}
					if (Abs(inputDirector->dir.x) >= 1)
					{
						if (!repelInputTimer.reachedZero() && repelInputTimer.isStarted())
						{
							canRepel = false;
							attackingTimer.restart();
						}
					}
				}

			}
		}
		

		pos += vel * Scene::DeltaTime();

		if (dir != 0)
		{
			lastDir = dir;
		}
		Actor::Update();
	}
	virtual void Draw() override
	{
		Circle{ pos + Vec2{0,(state == U"damage") ? 0 : -100},40}.draw(canHit ? Palette::Red : Palette::Pink);
		leg->drawFrame(2.0, Palette::Hotpink);
		Actor::Draw();
	}

	void OnHit()
	{
		if (canHit)
		{
			state = U"damage";
			damagedPos = pos;
			vel = Vec2{ 100,-200 };
			damagedTimer.restart();
			canHit = false;
		}
	}

private:

	float walkSpeed;
	float attackSpeed;
	Duration repelInputAcceptDuration;
	Timer repelInputTimer;
	Duration attackingDuration;
	Timer attackingTimer;
	float attackFootHeight = 0.0f;
	int32 attackFootHeightCount = 0;
	bool canRepel = true;

	Duration damageDuration;
	Timer damagedTimer;
	Vec2 damagedPos;
	bool canHit = true;
	Duration invTimeDuration;
	Timer invTimeTimer;

	String state = U"default";
};

class Climber : public Actor
{

public:
	float dir;
	float lastDir;
	Vec2 vel;

	Climber(String _name,Vec2 _pos) : Actor(_name) {
		Array<float> walkptn = { 100,200,300 };
		walkSpeed = walkptn.choice();
		attackSpeed = 1000;
		pos = _pos;
		collision = new Rect(100, 200);
		collision->setPos(Arg::bottomCenter((int32)pos.x, (int32)pos.y));
		vel = OffsetCircular{ Vec2{0,0},walkSpeed,70_deg };
	}
	virtual void Update() override
	{
		if (state == U"defeated")
		{
			vel.y += 900 * Scene::DeltaTime();
		}

		pos += vel * Scene::DeltaTime();

		if (!collision->intersects(Rect{ -1000,0,4000,2000 }))
		{
			isDestroyed = true;
		}

		Actor::Update();
	}
	virtual void Draw() override
	{
		Actor::Draw();
	}

	void OnCollsitionLeg()
	{
		if (state != U"defeated")
		{

			state = U"defeated";
			vel = RandomVec2(RectF{ -1,-2,2,2 }) * 50;
		}
	}

private:

	float walkSpeed;
	float attackSpeed;

	String state = U"default";
};

void Main()
{
	Window::Resize(1600, 900);
	Scene::SetBackground(Palette::Forestgreen);

	Player* player = new Player(U"player");

	Array<Climber*> climbers;
	Timer climberGenerate;
	climberGenerate.set(1s);
	climberGenerate.start();
	RectF climberGenerateRect{ -500, 800 ,450 ,450 };
	//RectF climberGenerateRect{ 0, 0 ,450 ,450 };

	while (System::Update())
	{
		inputDirector->Update();

		if (climberGenerate.reachedZero())
		{
			climbers << new Climber(U"climber", RandomVec2( climberGenerateRect ));
			climberGenerate.restart();
		}

		climbers.each([](Climber* item) { item->Update(); });
		climbers.each([&player](Climber* item) {
				if (item->collision->intersects(*(player->leg)))
				{
					item->OnCollsitionLeg();
				}
				if (item->collision->intersects(*(player->collision)))
				{
					player->OnHit();
				}
			});
		climbers.remove_if([](Climber* item) { return item->isDestroyed; });
		player->Update();

		climbers.each([](Climber* item) { item->Draw(); });
		player->Draw();
		inputDirector->Draw();
	}
}

