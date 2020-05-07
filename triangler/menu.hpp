#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <map>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define TRIANGLER_SETTING_ID_CAMSPEED 0u
#define TRIANGLER_SETTING_ID_GRIDHEIGHT 1u
#define TRIANGLER_SETTING_ID_GRID_DRAW 2u
#define TRIANGLER_SETTING_ID_DIFFUSE 3u
#define TRIANGLER_SETTING_ID_SPECULAR 4u
#define TRIANGLER_SETTING_ID_DEBUG 5u
#define TRIANGLER_SETTING_ID_WIREFRAME 6u

class Setting
{
public:
	Setting(const char* name)
		: name_(name) {}
	const std::string GetName();
	virtual const std::string GetValue() const = 0;
	virtual const float GetFloat() const = 0;
	virtual const bool GetBool() const = 0;
	virtual void SetFloat(const float value) = 0;
	virtual void SetBool(const bool value) = 0;
	virtual void Increment() = 0;
	virtual void Decrement() = 0;
private:
	std::string name_;
};

class BoolSetting : public Setting
{
public:
	BoolSetting(const char* name, bool v)
		: Setting(name), value_(v) {}
	const std::string GetValue() const;
	const float GetFloat() const { return 0.f; }
	const bool GetBool() const { return value_; }
	void SetFloat(const float value) {}
	void SetBool(const bool value) { value_ = value; }
	void Increment();
	void Decrement();
private:
	bool value_;
};

class FloatSetting : public Setting
{
public:
	FloatSetting(const char* name, float value, float step, float min, float max)
		: Setting(name), value_(value), step_(step), min_(min), max_(max) {}
	const std::string GetValue() const;
	const float GetFloat() const { return value_; }
	const bool GetBool() const { return false; }
	void SetFloat(const float value) { value_ = value; }
	void SetBool(const bool value) {}
	void Increment();
	void Decrement();
private:
	float value_, step_, min_, max_;
};

class TrianglerConfig
{
public:
	TrianglerConfig();
	~TrianglerConfig();
	void Input(int i);
	const unsigned GetIndex() const { return idx_; }
	std::vector<std::string> GetMenu();
	Setting& operator[] (const unsigned key) { return *config_[key]; }
	void SetFloat(const unsigned key, const float value) { config_[key]->SetFloat(value); }
	void SetBool(const unsigned key, const bool value) { config_[key]->SetBool(value); }
private:
	std::map<unsigned, Setting*> config_; 
	int idx_ = 0;
};

