#include "tune.h"

Tune::Tune()
{
	id_ = lastId_++;
}

int Tune::id() const
{
	return id_;
}

bool Tune::operator==(const Tune& other)
{
	return id() == other.id();
}

int Tune::lastId_ = 0;
