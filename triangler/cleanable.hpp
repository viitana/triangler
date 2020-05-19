#pragma once

template <typename T>
class CleanableBy
{
public:
	virtual ~CleanableBy() {}
	virtual void Clean(T* cleaner) = 0; 
};

template <typename T> 
class CleanableObserverOf
{
public:
	virtual ~CleanableObserverOf() {}
	virtual void NotifyDirty(T* cleanable) = 0;
	virtual void CleanObservees() = 0;
};
