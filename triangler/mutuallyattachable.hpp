#pragma once

template <typename T>
class MutuallyAttachableTo
{
public:
	virtual void AttachNoReciprocation(T* observer) = 0;
	virtual void Attach(T* observer) = 0;
};
