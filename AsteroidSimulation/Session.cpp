#include "pch.h"
#include "Session.h"

Session::Session()
{
}

Session& Session::Me()
{
	static Session me;
	return me;
}
