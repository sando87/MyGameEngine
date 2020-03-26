#include "oFormStats.h"
#include "jForm.h"
#include "jEventForm.h"
#include "jShaderUI.h"
#include "ObjPlayer.h"
#include <jUISystem.h>
#include <jView.h>
#include <jViewFont.h>

oFormStats::oFormStats()
{
}


oFormStats::~oFormStats()
{
}

void oFormStats::OnLoad()
{
	mForm = new jUISystem();

	jForm* compForm = new jForm(mForm);
	compForm->SetJsonFilename("panelStats.json");
	AddComponent(compForm);

	AddComponent(new jShaderUI(mForm));

	AddComponent(new jEventForm(mForm));

}

void oFormStats::OnStart()
{
	mObjPlayer = GetEngine().FindGameObject<ObjPlayer>();
	u32 dbID = mObjPlayer->GetID();

	mForm->FindView("Close")->EventMouseClick = [&](jView* view, Point2 pt) {
		SetEnable(false);
	};
	mForm->FindView("btnPA")->EventMouseClick = [dbID](jView* view, Point2 pt) {
		DBPlayer playerDB;
		playerDB.Load(dbID);
		if (playerDB.statsRemain > 0)
		{
			playerDB.statsPA += 1;
			playerDB.statsRemain -= 1;
			playerDB.Save();
		}
	};
	mForm->FindView("btnMA")->EventMouseClick = [dbID](jView* view, Point2 pt) {
		DBPlayer playerDB;
		playerDB.Load(dbID);
		if (playerDB.statsRemain > 0)
		{
			playerDB.statsMA += 1;
			playerDB.statsRemain -= 1;
			playerDB.Save();
		}
	};
	mForm->FindView("btnPD")->EventMouseClick = [dbID](jView* view, Point2 pt) {
		DBPlayer playerDB;
		playerDB.Load(dbID);
		if (playerDB.statsRemain > 0)
		{
			playerDB.statsPD += 1;
			playerDB.statsRemain -= 1;
			playerDB.Save();
		}
	};
	mForm->FindView("btnMD")->EventMouseClick = [dbID](jView* view, Point2 pt) {
		DBPlayer playerDB;
		playerDB.Load(dbID);
		if (playerDB.statsRemain > 0)
		{
			playerDB.statsMD += 1;
			playerDB.statsRemain -= 1;
			playerDB.Save();
		}
	};
	mForm->FindView("btnHP")->EventMouseClick = [dbID](jView* view, Point2 pt) {
		DBPlayer playerDB;
		playerDB.Load(dbID);
		if (playerDB.statsRemain > 0)
		{
			playerDB.statsHP += 1;
			playerDB.statsRemain -= 1;
			playerDB.Save();
		}
	};
	mForm->FindView("btnMP")->EventMouseClick = [dbID](jView* view, Point2 pt) {
		DBPlayer playerDB;
		playerDB.Load(dbID);
		if (playerDB.statsRemain > 0)
		{
			playerDB.statsMP += 1;
			playerDB.statsRemain -= 1;
			playerDB.Save();
		}
	};

	mlbPA = (jViewFont*)mForm->FindView("lbPA");
	mlbMA = (jViewFont*)mForm->FindView("lbMA");
	mlbPD = (jViewFont*)mForm->FindView("lbPD");
	mlbMD = (jViewFont*)mForm->FindView("lbMD");
	mlbHP = (jViewFont*)mForm->FindView("lbHP");
	mlbMP = (jViewFont*)mForm->FindView("lbMP");
	mlbLevel = (jViewFont*)mForm->FindView("lbLevel");
	mlbRemain = (jViewFont*)mForm->FindView("lbRemain");

	DBPlayer playerDB;
	playerDB.Load(dbID);
	mlbPA->Text = jUtils::ToString(playerDB.statsPA);
	mlbMA->Text = jUtils::ToString(playerDB.statsMA);
	mlbPD->Text = jUtils::ToString(playerDB.statsPD);
	mlbMD->Text = jUtils::ToString(playerDB.statsMD);
	mlbHP->Text = jUtils::ToString(playerDB.statsHP);
	mlbMP->Text = jUtils::ToString(playerDB.statsMP);
	mlbLevel->Text = "Level : " + jUtils::ToString(playerDB.level);
	mlbRemain->Text = "Remain : " +jUtils::ToString(playerDB.statsRemain);

	mForm->FindView("BackStat")->LoadAll();
}

void oFormStats::OnUpdate()
{
	static DBPlayer preDBPlayerInfo;
	u32 dbID = mObjPlayer->GetID();
	DBPlayer curDBPlayer;
	curDBPlayer.Load(dbID);
	if (curDBPlayer.statsPA != preDBPlayerInfo.statsPA)
	{
		mlbPA->Text = jUtils::ToString(curDBPlayer.statsPA);
		mlbPA->LoadAll();
		preDBPlayerInfo.statsPA = curDBPlayer.statsPA;
	}
	if (curDBPlayer.statsMA != preDBPlayerInfo.statsMA)
	{
		mlbMA->Text = jUtils::ToString(curDBPlayer.statsMA);
		mlbMA->LoadAll();
		preDBPlayerInfo.statsMA = curDBPlayer.statsMA;
	}
	if (curDBPlayer.statsPD != preDBPlayerInfo.statsPD)
	{
		mlbPD->Text = jUtils::ToString(curDBPlayer.statsPD);
		mlbPD->LoadAll();
		preDBPlayerInfo.statsPD = curDBPlayer.statsPD;
	}
	if (curDBPlayer.statsMD != preDBPlayerInfo.statsMD)
	{
		mlbMD->Text = jUtils::ToString(curDBPlayer.statsMD);
		mlbMD->LoadAll();
		preDBPlayerInfo.statsMD = curDBPlayer.statsMD;
	}
	if (curDBPlayer.statsHP != preDBPlayerInfo.statsHP)
	{
		mlbHP->Text = jUtils::ToString(curDBPlayer.statsHP);
		mlbHP->LoadAll();
		preDBPlayerInfo.statsHP = curDBPlayer.statsHP;
	}
	if (curDBPlayer.statsMP != preDBPlayerInfo.statsMP)
	{
		mlbMP->Text = jUtils::ToString(curDBPlayer.statsMP);
		mlbMP->LoadAll();
		preDBPlayerInfo.statsMP = curDBPlayer.statsMP;
	}
	if (curDBPlayer.level != preDBPlayerInfo.level)
	{
		mlbLevel->Text = "Level : " + jUtils::ToString(curDBPlayer.level);
		mlbLevel->LoadAll();
		preDBPlayerInfo.level = curDBPlayer.level;
	}
	if (curDBPlayer.statsRemain != preDBPlayerInfo.statsRemain)
	{
		mlbRemain->Text = "Remain : " + jUtils::ToString(curDBPlayer.statsRemain);
		mlbRemain->LoadAll();
		preDBPlayerInfo.statsRemain = curDBPlayer.statsRemain;
	}
}

