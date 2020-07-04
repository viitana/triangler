#pragma once

class CleanableObserver;

class Cleanable
{
public:
	virtual ~Cleanable() {}
	virtual void Clean() = 0;
	virtual void AddObserver(CleanableObserver* observer) = 0;
};

class CleanableObserver
{
public:
	virtual ~CleanableObserver() {}
	virtual void NotifyDirty(Cleanable* cleanable) = 0;
	virtual void CleanObservees2() = 0;
};
