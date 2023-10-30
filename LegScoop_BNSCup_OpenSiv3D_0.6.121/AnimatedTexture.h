#pragma once
struct TextureFrame
{
	Texture texture;
	Duration time;
};

struct AnimatedTexture
{
public:
	String animName;
	bool isLoop = false;
	Array<TextureFrame> frames;

	Texture* GetTexture(Duration _time);
};

class AnimatedTextureInstance
{
public:
	Stopwatch time;

	AnimatedTextureInstance(AnimatedTexture* _animatedTexture);

	Texture* GetTexture();

	void Start(Duration _starttime);

	void ReStartOnStopped();

	void Stop();
private:
	AnimatedTexture* tex;
};
