#pragma once

class Beeps
{
	HashTable<String, Audio> beeps;
	static Beeps* instance;
public:
	static Audio GetBeep(String _name);
	static bool AddBeep(String _name, Audio _audio);
};
