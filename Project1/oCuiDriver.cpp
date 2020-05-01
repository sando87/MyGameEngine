#include "oCuiDriver.h"
#include "cCuiDriven.h"


oCuiDriver::oCuiDriver()
{
}


oCuiDriver::~oCuiDriver()
{
}

void oCuiDriver::OnLoad()
{
	mThread = thread([this]() {
		while (true)
		{
			string str;
			getline(cin, str);
			mCommandString = str;
			mUserInput = true;
		};
	});
}

void oCuiDriver::OnUpdate()
{
	if (mUserInput)
	{
		mUserInput = false;
		for (cCuiDriven* driven : mDrivens)
			if (driven->EventCommandInput)
				driven->EventCommandInput(mCommandString);
	}
}

