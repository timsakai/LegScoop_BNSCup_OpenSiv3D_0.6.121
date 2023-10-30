#pragma once
class InputDirector	//機能：クリック地点からの相対マウス位置を正規化して出力　inputDirector->dir　でとれる
{
public:
	Vec2 dir;
	int maxDistance;

	InputDirector();

	void Update();

	void Draw();
private:
	Point PinchStart;

};
