#include <algorithm>

#include "menu.hpp"

const std::string Setting::GetName()
{
	return name_;
}

const std::string BoolSetting::GetValue() const
{
	return value_ ? "ON" : "OFF";
}

void BoolSetting::Increment()
{
	value_ = !value_;
}

void BoolSetting::Decrement()
{
	value_ = !value_;
}

void FloatSetting::Increment()
{
	value_ = std::min(max_, value_ + step_);
}

void FloatSetting::Decrement()
{
	value_ = std::max(min_, value_ - step_);
}

const std::string FloatSetting::GetValue() const
{
	char buf[16];
	std::snprintf(buf, 16, "%4.4f", value_);
	return buf;
}

TrianglerConfig::TrianglerConfig()
{
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_CAMSPEED, new FloatSetting("Camera speed", 0.01f, 0.001f, 0.001f, 1.f)));
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_GRIDHEIGHT, new FloatSetting("Grid height", 0.01f, 0.001f, 0.001f, 1.f)));
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_GRID_DRAW, new BoolSetting("Grid drawing", true)));
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_DIFFUSE, new BoolSetting("Diffuse shading", true)));
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_SPECULAR, new BoolSetting("Specular lights", true)));
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_DEBUG, new BoolSetting("Draw debug lines", false)));
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_WIREFRAME, new BoolSetting("Wireframe", false)));
	config_.insert(std::make_pair(TRIANGLER_SETTING_ID_FLAT, new BoolSetting("Flat shading", false)));
}

TrianglerConfig::~TrianglerConfig()
{
	for (auto& pair : config_)
		delete pair.second;
}

void TrianglerConfig::Input(int i)
{
	switch (i)
	{
		case GLFW_KEY_UP:
			idx_ = (idx_ + 1) % config_.size();
			break;
		case GLFW_KEY_DOWN:
			idx_--;
			if (idx_ < 0) idx_ = config_.size() - 1;
			break;
		case GLFW_KEY_LEFT:
			config_[idx_]->Decrement();
			break;
		case GLFW_KEY_RIGHT:
			config_[idx_]->Increment();
			break;
	}
}

std::vector<std::string> TrianglerConfig::GetMenu()
{
	std::vector<std::string> v;
	std::stringstream ss;

	for (auto& pair : config_)
	{
		ss << pair.second->GetName() << std::setw(26 - pair.second->GetName().size()) << pair.second->GetValue();
		v.push_back(ss.str());
		ss.str(std::string());
	}

	return v;
}
