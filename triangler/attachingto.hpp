#pragma once

template <typename T>
class AttachingTo
{
public:
	virtual void Attach(T* other) = 0;
};
