#include "AnimatedTexture.h"

Texture* AnimatedTexture::GetTexture(Duration _time)
{
	float timef = _time.count();

	float loopSum = 0;
	for (int32 i = 0; i < frames.size(); i++)
	{
		loopSum += frames.at(i).time.count();
	}

	int32 loop = timef / loopSum;
	timef = Fmod(timef, loopSum);

	int32 frame = 0;
	float progress = 0;
	for (int32 i = 0; i < frames.size() - 1; i++)
	{
		progress += frames.at(i + 1).time.count();
		if (progress >= timef)
		{
			break;
		}
		frame++;
	}
	if (!isLoop && loop > 0)
	{
		frame = frames.size() - 1;
	}

	return &(frames.at(frame).texture);
}

AnimatedTextureInstance::AnimatedTextureInstance(AnimatedTexture* _animatedTexture)
{
	tex = _animatedTexture;
}

Texture* AnimatedTextureInstance::GetTexture()
{
	return tex->GetTexture(time.elapsed());
}

void AnimatedTextureInstance::Start(Duration _starttime)
{
	time.set(_starttime);
	time.start();
}

void AnimatedTextureInstance::ReStartOnStopped()
{
	if (!time.isRunning())
	{
		time.restart();
	}
}

void AnimatedTextureInstance::Stop()
{
	time.pause();
}
