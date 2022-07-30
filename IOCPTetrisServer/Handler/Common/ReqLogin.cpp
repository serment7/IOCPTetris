#include "ReqLogin.h"
#include "../../Game/Player.h"

void Handler::ReqLogin::Logic(Player* player)
{
	res.newSessionID = player->sessionIndex;
	res.result.resultID = ResultID::SUCCESS;
}
