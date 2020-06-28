#include "vertexattributeinterface.hpp"

void VertexAttributeInterface::AttachNoReciprocation(Object3D* object)
{
	objects_.insert(object);
}

void VertexAttributeInterface::Attach(Object3D* object)
{
	AttachNoReciprocation(object);
	object->AttachNoReciprocation(this);
}
