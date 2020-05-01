#include "oFormMain.h"
#include "jForm.h"
#include "jEventForm.h"
#include "jShaderUI.h"
#include "jHealthPoint.h"
#include "ObjPlayer.h"
#include <jUISystem.h>
#include <jViewImage.h>


oFormMain::oFormMain()
{
}


oFormMain::~oFormMain()
{
}

void oFormMain::OnLoad()
{
	mForm = new jUISystem();

	jForm* compForm = new jForm(mForm);
	compForm->SetJsonFilename("panelMain.json");
	AddComponent(compForm);

	AddComponent(new jShaderUI(mForm));

	AddComponent(new jEventForm(mForm, 'S'));
}

void oFormMain::OnStart()
{
	mHPView = (jViewImage*)mForm->FindView("HP");
	mMPView = (jViewImage*)mForm->FindView("MP");
	mExpView = (jViewImage*)mForm->FindView("EXP");

	ObjPlayer* objPlayer = GetEngine().FindGameObject<ObjPlayer>();
	mPlayerDBId = objPlayer->GetID();
	mPlayerHP = objPlayer->FindComponent<jHealthPoint>();
}

void oFormMain::OnUpdate()
{
	double hpRate = mPlayerHP->CurSpec.hp / mPlayerHP->MaxSpec.hp;
	mHPView->ClipRateTop = 1.0f - (float)hpRate;
	mHPView->LoadAll();

	double mpRate = mPlayerHP->CurSpec.mp / mPlayerHP->MaxSpec.mp;
	mMPView->ClipRateTop = 1.0f - (float)mpRate;
	mMPView->LoadAll();

	DBPlayer dbPlayer;
	dbPlayer.Load(mPlayerDBId);
	double expRate = (dbPlayer.level * 100.0 - dbPlayer.exp) / 100.0;
	mExpView->ClipRateRight = expRate;
	mExpView->LoadAll();
}
