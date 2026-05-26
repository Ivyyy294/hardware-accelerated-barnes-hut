#pragma once

#include "SessionVariables.h"

class Session
{
//Variables
public:
	SessionVariables m_variables;

//Methods
private:
	Session();

public:
	static Session& Me();
};

