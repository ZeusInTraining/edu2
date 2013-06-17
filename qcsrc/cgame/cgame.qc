// --------------------------------------------------------------------------
// BEGIN REQUIRED CSQC FUNCTIONS

// CSQC_Init : Called every time the CSQC code is initialized (essentially at map load)
void CSQC_Init(void)
{
  View_Init();

  precache_model("models/ipod.md3");
  precache_model("models/nokia.md3");

  registercommand( "messagemode" );
  registercommand( "musicplayer" );
  registercommand( "localsay" );
  registercommand( "taunt" );
  registercommand( "edu1_launchfile" );
  
  gecko_init_browser_data();
}

// CSQC_Shutdown : Called every time the CSQC code is shutdown (changing maps, quitting, etc)
void CSQC_Shutdown(void)
{
}

float musicPlayer;
float musicPlaying;
const float MUSICPLAYER_SCREENTIME = 3;
const float VIEWMODEL_MOVEDIST   = 5;
const float VIEWMODEL_MOVETIME   = 0.8;
float inChat;

void ShowChatViewModel(float instant)
{
  if (instant)
	ShowViewModelInstant ("models/ipod.md3"); // @@@ Use nokia once it works
  else
	ShowViewModelRaise ("models/ipod.md3", VIEWMODEL_MOVEDIST, VIEWMODEL_MOVETIME); // @@@ Use nokia once it works
}

void MyHideViewModel()
{
  HideViewModelLower (VIEWMODEL_MOVEDIST, VIEWMODEL_MOVETIME);
}

void GameCommand(string pCommand)
{
	float argc;

	// Tokenize String
	argc = tokenize(pCommand);
	
	string lCmd;
	lCmd = argv( 0 );
	if( lCmd == "chatclosed" && inChat ) {
		inChat = false;
		if (!musicPlayer) // Music player might have been enabled in the mean time
		  MyHideViewModel();
	}
}

// CSQC_ConsoleCommand : Used to parse commands in the console that have been registered with the "registercmd" function
// Return value should be 1 if CSQC handled the command, otherwise return 0 to have the engine handle it.
float CSQC_ConsoleCommand(string strMessage)
{
	float argc;

	// Tokenize String
	argc = tokenize(strMessage);

	// Acquire Command
	local string strCmd;
	strCmd = argv(0);

	if( strCmd == "messagemode" ) {
		ShowChatViewModel (true); // Instant b/c client time (and thus animation) gets frozen in menu
		musicPlayer = 0;
		inChat = true;
		localcmd( "menu_cmd directmenu ChatDialog\n" );
		return true;
	} else if( strCmd == "musicplayer" ) {
		if (musicPlaying)
		{
		  localcmd("cd stop");
		  musicPlaying = false;
		}
		else
		{
		  localcmd("cd play theme");
		  musicPlaying = true;
		}
		musicPlayer = time;
		ShowViewModelRaise ("models/ipod.md3", VIEWMODEL_MOVEDIST, VIEWMODEL_MOVETIME);

		return true;
	} else if( strCmd == "localsay" ) {
		// still forward it to the server..
		localcmd( "cmd ", strMessage, "\n" );
		return true;
	} else if( strCmd == "taunt" ) {
		localcmd( "cmd taunt\n" );
		return true;
	} else if( strCmd == "edu1_launchfile" ) {
		CreateEDU1LaunchFile (argc);
		return true;
	}

	return false;
}

// CSQC_InputEvent : Used to perform actions based on any key pressed or released by the client.
// Return value should be 1 if CSQC handled the key press, otherwise return 0 to have the key passed to the engine.
float CSQC_InputEvent(float bKeyReleased, float nKey, float nAscii)
{
	if (gecko_handle_input (bKeyReleased, nKey, nAscii))
		return true;
	return false;
}

// END REQUIRED CSQC FUNCTIONS
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// BEGIN OPTIONAL CSQC FUNCTIONS

// EDU2 handling:
// entities who dont want to be deleted simply dont remove themself on Ent_Remove
entity CSQC_Ent_Spawn(float realentnum) {
	// try to find the entity (if it already exists)
	entity lEntity;
	lEntity = findfloat( world, entnum, realentnum );
	if( lEntity == world ) {
		dprint( "Couldnt find ", ftos( realentnum ), "\n" );
		lEntity = spawn();
		lEntity.entnum = realentnum;
	}
	return lEntity;
}

// CSQC_Ent_Update : Called every frame that the server has indicated an update to the SSQC / CSQC entity has occured.
// The only parameter reflects if the entity is "new" to the client, meaning it just came into the client's PVS.
void CSQC_Ent_Update(float isNew)
{
	if( isNew ) {
		float entType;
		entType = readbyte();
		switch (entType)
		{
		case ET_GECKO:
		  gecko_terminal_spawnFromMessage();
		  break;
		case ET_PLAYER:
		  Player_SpawnFromMessage ();
		  break;
		}
	} else {
		// read over the entity type
		readbyte();
		if( self.UpdateEntity ) {
			self.UpdateEntity();
		}
	}
}

// CSQC_Ent_Remove : Called when the server requests a SSQC / CSQC entity to be removed.  Essentially call remove(self) as well.
void CSQC_Ent_Remove()
{
	if( self.RemoveEvent ) {
		self.RemoveEvent();
	} else {
		print( "Removing for real: ", self.targetURL, "\n" );
		remove(self);
	}
}

// CSQC_Parse_StuffCmd : Provides the stuffcmd string in the first parameter that the server provided.  To execute standard behavior, simply execute localcmd with the string.
void CSQC_Parse_StuffCmd(string strMessage)
{
	localcmd(strMessage);
}

// CSQC_Parse_Print : Provides the print string in the first parameter that the server provided.  To execute standard behavior, simply execute print with the string.
void CSQC_Parse_Print(string strMessage)
{
	// this is a hack to determine whether its a chat message
	//if( str2chr( strMessage, 0 ) != 1 ) {
		print( strMessage );
	//}
	localcmd( "menu_cmd print ", strMessage );
}

// CSQC_Parse_CenterPrint : Provides the centerprint string in the first parameter that the server provided.  To execute standard behavior, simply execute cprint with the string.
void CSQC_Parse_CenterPrint(string strMessage)
{
	centerprint(strMessage);
}

// CSQC_Parse_TempEntity : Handles all temporary entity network data in the CSQC layer.
// You must ALWAYS first acquire the temporary ID, which is sent as a byte.
// Return value should be 1 if CSQC handled the temporary entity, otherwise return 0 to have the engine process the event.
float CSQC_Parse_TempEntity()
{
	local float bHandled;
	bHandled  = true;
	// Acquire TE ID
	local float nTEID;
	nTEID = readbyte();

	switch(nTEID)
	{
		/*
		case TE_GUNSHOT:
			// Do something cool with TE_GUNSHOT!
			break;
		*/
		case TE_PLAYER_ACTION:
			{
				local float lClientNum;
				local float lAnim;

				lClientNum = readbyte();
				lAnim = readbyte();
				Player_DoAction (Player_EntityForClientNum (lClientNum), lAnim);
				bHandled = true;
			}
			break;
		default:
			// No special logic for this temporary entity; return 0 so the engine can handle it
			bHandled = false;
			break;
	}

	return bHandled;
}

// CSQC_UpdateView - Called every rendered frame on the client.  Useful for HUD drawing operations.
void CSQC_UpdateView(void)
{
	SysParms_Fetch();

	// Move music player offscreen after a small while
	if (musicPlayer
	  && (time - musicPlayer > MUSICPLAYER_SCREENTIME - VIEWMODEL_MOVETIME))
	{
		musicPlayer = 0;
		if (inChat)
		  ShowChatViewModel(true);
		else
		  MyHideViewModel();
	}
	
	View_Update();
	
	handle_gecko_terminal();
}

string Gecko_Query (string queryStr)
{
  if (queryStr == "hello")
	return "Hello from QuakeC";
  return "";
}
