//AK 08
#ifdef INTERFACE
CLASS(GameList) EXTENDS(NexuizListBox)
	METHOD(GameList, configureGameList, void(entity))
	ATTRIB(GameList, rowsPerItem, float, 10)
	METHOD(GameList, draw, void(entity))
	METHOD(GameList, drawListBoxItem, void(entity, float, vector, float))
	METHOD(GameList, clickListBoxItem, void(entity, float, vector))
	METHOD(GameList, resizeNotify, void(entity, vector, vector, vector, vector))
	METHOD(GameList, setSelected, void(entity, float))
	METHOD(GameList, keyDown, float(entity, float, float, float))

	ATTRIB(GameList, realFontSize, vector, '0 0 0')
	ATTRIB(GameList, columnPreviewOrigin, float, 0)
	ATTRIB(GameList, columnPreviewSize, float, 0)
	ATTRIB(GameList, columnNameOrigin, float, 0)
	ATTRIB(GameList, columnNameSize, float, 0)
	ATTRIB(GameList, columnCheckMarkOrigin, float, 0)
	ATTRIB(GameList, columnCheckMarkSize, float, 0)
	ATTRIB(GameList, checkMarkOrigin, vector, '0 0 0')
	ATTRIB(GameList, checkMarkSize, vector, '0 0 0')
	ATTRIB(GameList, realUpperMargin1, float, 0)
	ATTRIB(GameList, realUpperMargin2, float, 0)

	ATTRIB(GameList, lastClickedMap, float, -1)
	ATTRIB(GameList, lastClickedTime, float, 0)

	ATTRIB(GameList, origin, vector, '0 0 0')
	ATTRIB(GameList, itemAbsSize, vector, '0 0 0')
	ATTRIB(GameList, emptyLineHeight, float, 0.5)

	ATTRIB(GameList, campaignIndex, float, 0)
	ATTRIB(GameList, cvarName, string, string_null)
	METHOD(GameList, loadCvars, void(entity))
	METHOD(GameList, saveCvars, void(entity))
ENDCLASS(GameList)
entity makeGameList();
void GameList_StartGame(entity btn, entity me);
#endif

#ifdef IMPLEMENTATION
#define GAMELIST_MAX_ENTRIES 10
typedef void(entity) GameList_Callback; // void(entity button)
string gamelist_longdesc_wrapped[GAMELIST_MAX_ENTRIES];
string gamelist_longdesc[GAMELIST_MAX_ENTRIES];
string gamelist_shortname[GAMELIST_MAX_ENTRIES];
string gamelist_shortdesc[GAMELIST_MAX_ENTRIES];
var GameList_Callback gamelist_callback[GAMELIST_MAX_ENTRIES];
// registry entry: edu2/gamelist/$gameshortname/played 1 or 0/""

float gamelist_entries;

void( entity button ) MineSweeper_Start = {
	entity lDialog;
	lDialog = findstring( NULL, name, "MineSweeper" );
	DialogOpenButton_Click( button, lDialog );
};

void( entity button ) Edu1_Start = {
	registercvar( "edu1params", "map $edu1map", 0 );
	str_save( strcat( cvar_string( "edu1params" ), "\n" ), "edu1launch.txt" );
	localcmd( "quit\n" );
};

void( entity button ) DummyGame_Start = {
	DialogOpenButton_Click( button, gameStubDialog );
};

void rewarpGameList(float w, float l0, float emptyheight)
{
	float i, j;
	float n, take, cantake, l;
	string r, s;
	for(i = 0; i < gamelist_entries; ++i)
	{
		l = l0;
		if(gamelist_longdesc_wrapped[i])
			strunzone(gamelist_longdesc_wrapped[i]);
		n = tokenizebyseparator(gamelist_longdesc[i], "\n");
		r = "";
		for(j = 0; j < n; ++j)
		{
			s = argv(j);
			if(s == "")
			{
				l -= emptyheight;
				r = strcat(r, "\n");
				continue;
			}
			for(;;)
			{
				cantake = draw_TextLengthUpToWidth(s, w, 0);
				if(cantake > 0 && cantake < strlen(s))
				{
					take = cantake - 1;
					while(take > 0 && substring(s, take, 1) != " ")
						--take;
					if(take == 0)
					{
						if(--l < 0) goto toolong;
						r = strcat(r, substring(s, 0, cantake), "\n");
						s = substring(s, cantake, strlen(s) - cantake);
					}
					else
					{
						if(--l < 0) goto toolong;
						r = strcat(r, substring(s, 0, take), "\n");
						s = substring(s, take + 1, strlen(s) - take);
					}
				}
				else
				{
					if(--l < 0) goto toolong;
					r = strcat(r, s, "\n");
					break;
				}
			}
		}
		goto nottoolong;
:toolong
		while(substring(r, strlen(r) - 1, 1) == "\n")
			r = substring(r, 0, strlen(r) - 1);
		r = strcat(r, "...\n");
:nottoolong
		gamelist_longdesc_wrapped[i] = strzone(substring(r, 0, strlen(r) - 1));
	}
}

entity makeGameList()
{
	entity me;
	me = spawnGameList();
	me.configureGameList(me);
	return me;
}
void configureGameListGameList(entity me)
{
	me.configureNexuizListBox(me);
	me.loadCvars(me);
}

void loadCvarsGameList(entity me)
{
	float i;
	i = 0;
#define GAMELIST_ENTRY(ax,sx,bx,fx)	gamelist_shortname[i] = ax; gamelist_shortdesc[i] = sx; gamelist_longdesc[ i ] = bx; gamelist_callback[i] = fx; i = i + 1
	GAMELIST_ENTRY( "edu1", "Edu 1", "Play the original and educate yourself while answering questions and having fun with sponges (and you even get Credits for it)!", Edu1_Start );
	GAMELIST_ENTRY( "minesweeper", "MineSweeper (Classic)", "The classical Windows game that has been ported to oh so many platforms. You can play it here, too, and help time pass when you're bored and, of course, earn useful Credits.", MineSweeper_Start );
	GAMELIST_ENTRY( "tabletennis", "Table Tennis 3D", "Play some rounds of tebletennis against your opponents and earn Credits with each score.", DummyGame_Start );
	GAMELIST_ENTRY( "yeti", "Yeti Game", "Enjoy this fluffy arcade platformer and help the yeti find his way back into his cave.", DummyGame_Start );
	GAMELIST_ENTRY( "moongame", "MoonGame Beat'em Up", "Feeling like beating someone up? Don't take it on your pets and better play this game. It's fun and you get Credits for it - and no jail time!", DummyGame_Start );
	gamelist_entries = i;

	// read campaign cvars
	/*if(campaign_name)
		strunzone(campaign_name);
	if(me.cvarName)
		strunzone(me.cvarName);
	campaign_name = strzone(cvar_string("g_campaign_name"));
	me.cvarName = strzone(strcat("g_campaign", campaign_name, "_index"));
	CampaignFile_Load(0, GAMELIST_MAX_ENTRIES);*/
	me.campaignIndex = bound(0, /*cvar(me.cvarName)*/ gamelist_entries, gamelist_entries);
	//cvar_set(me.cvarName, ftos(me.campaignIndex));
	if(me.columnNameSize)
		rewarpGameList(me.columnNameSize / me.realFontSize_x, me.rowsPerItem - 3, me.emptyLineHeight);
	me.nItems = min(me.campaignIndex + 2, gamelist_entries);
	me.selectedItem = min(me.campaignIndex, me.nItems - 1);
	me.scrollPos = me.nItems * me.itemHeight - 1;
}

void saveCvarsGameList(entity me)
{
	// write campaign cvars
	//cvar_set("g_campaign_name", campaign_name);
	// cvar_set(me.cvarName, ftos(me.campaignIndex)); // NOTE: only server QC does that!
}

void drawGameList(entity me)
{
	//if(cvar(me.cvarName) != me.campaignIndex || cvar_string("g_campaign_name") != campaign_name)
	//	me.loadCvars(me);
	drawListBox(me);
}

void resizeNotifyGameList(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize)
{
	me.origin = absOrigin;
	me.itemAbsSize = '0 0 0';
	resizeNotifyNexuizListBox(me, relOrigin, relSize, absOrigin, absSize);

	me.realFontSize_y = me.fontSize / (me.itemAbsSize_y = (absSize_y * me.itemHeight));
	me.realFontSize_x = me.fontSize / (me.itemAbsSize_x = (absSize_x * (1 - me.controlWidth)));
	me.realUpperMargin1 = 0.5 * me.realFontSize_y;
	me.realUpperMargin2 = me.realUpperMargin1 + 2 * me.realFontSize_y;

	me.checkMarkSize = (eX * (me.itemAbsSize_y / me.itemAbsSize_x) + eY) * 0.5;

	me.columnPreviewOrigin = 0;
	me.columnPreviewSize = me.itemAbsSize_y / me.itemAbsSize_x * 4 / 3;
	me.columnCheckMarkSize = me.checkMarkSize_x;
	me.columnNameSize = 1 - me.columnPreviewSize - me.columnCheckMarkSize - 4 * me.realFontSize_x;
	me.columnNameOrigin = me.columnPreviewOrigin + me.columnPreviewSize + me.realFontSize_x;
	me.columnCheckMarkOrigin = me.columnNameOrigin + me.columnNameSize + me.realFontSize_x * 2;

	me.checkMarkOrigin = eY + eX * (me.columnCheckMarkOrigin + me.columnCheckMarkSize) - me.checkMarkSize;

	rewarpGameList(me.columnNameSize / me.realFontSize_x, me.rowsPerItem - 3, me.emptyLineHeight);
}
void clickListBoxItemGameList(entity me, float i, vector where)
{
	if(i == me.lastClickedMap)
		if(time < me.lastClickedTime + 0.3)
		{
			// DOUBLE CLICK!
			// start game
			GameList_StartGame(me, me);
			return;
		}
	me.lastClickedMap = i;
	me.lastClickedTime = time;
}
void drawListBoxItemGameList(entity me, float i, vector absSize, float isSelected)
{
	string s;
	float p;
	vector theColor;
	float theAlpha;
	float j, n;
	vector o;

	if(i < me.campaignIndex)
	{
		theAlpha = SKINALPHA_CAMPAIGN_SELECTABLE;
		theColor = SKINCOLOR_CAMPAIGN_SELECTABLE;
	}
	else if(i == me.campaignIndex)
	{
		theAlpha = SKINALPHA_CAMPAIGN_CURRENT;
		theColor = SKINCOLOR_CAMPAIGN_CURRENT;
	}
	else
	{
		theAlpha = SKINALPHA_CAMPAIGN_FUTURE;
		theColor = SKINCOLOR_CAMPAIGN_FUTURE;
	}

	if(isSelected)
		draw_Fill('0 0 0', '1 1 0', SKINCOLOR_LISTBOX_SELECTED, SKINALPHA_LISTBOX_SELECTED);

	s = ftos(p);
	draw_Picture(me.columnPreviewOrigin * eX, strcat("/gfx/subgames/", gamelist_shortname[i]), me.columnPreviewSize * eX + eY, '1 1 1', theAlpha);
	if(db_get( menu_registry, strcat( "edu2/gamelist/", gamelist_shortname[i], "/played" ) ) == "1")
		draw_Picture(me.checkMarkOrigin, "checkmark", me.checkMarkSize, '1 1 1', 1);
	if(i <= me.campaignIndex)
		s = gamelist_shortdesc[i]; // fteqcc sucks
	else
		s = "???";
	s = draw_TextShortenToWidth(strcat(/*_("Game "), ftos(i + 1), " - ",*/ s), me.columnNameSize / me.realFontSize_x, 0);
	draw_Text(me.realUpperMargin1 * eY + (me.columnNameOrigin + 0.00 * (me.columnNameSize - draw_TextWidth(s, 0) * me.realFontSize_x)) * eX, s, me.realFontSize, theColor, theAlpha, 0);

	if(i <= me.campaignIndex)
	{
		s = gamelist_longdesc_wrapped[i];
		n = tokenizebyseparator(s, "\n");
		o = me.realUpperMargin2 * eY + me.columnNameOrigin * eX;
		for(j = 0; j < n; ++j)
			if(argv(j) != "")
			{
				draw_Text(o, argv(j), me.realFontSize, theColor, theAlpha * SKINALPHA_CAMPAIGN_DESCRIPTION, 0);
				o_y += me.realFontSize_y;
			}
			else
				o_y += me.realFontSize_y * me.emptyLineHeight;
	}
}
void GameList_StartAfterLogin(entity me);
void GameList_StartGame(entity btn, entity me)
{
	if(me.selectedItem >= me.nItems || me.selectedItem < 0)
		return;
	// TODO: move this somewhere else or write a function for it
	// not connected?
	if( gamestatus & GAME_CONNECTED ) {
		float i;
		i = me.selectedItem;
		db_put( menu_registry, strcat( "edu2/gamelist/", gamelist_shortname[i], "/played" ), "1" );
		(gamelist_callback[i])( btn );
	} else {
		LoginDialog_ExecAfter_CB = GameList_StartAfterLogin;
		LoginDialog_ExecAfter_Entity = me;
		DialogOpenButton_Click( btn, findstring( NULL, name, "LoginDialog" ) );
	}
}
void GameList_StartAfterLogin(entity me) {
	print( "blub\n" );
	GameList_StartGame( NULL, me );
}
void setSelectedGameList(entity me, float i)
{
	// prevent too late items from being played
	setSelectedListBox(me, min(i, me.campaignIndex));
}

float keyDownGameList(entity me, float scan, float ascii, float shift)
{
	if(scan == K_ENTER || scan == K_SPACE)
		GameList_StartGame(me, me);
	else
		return keyDownListBox(me, scan, ascii, shift);
	return 1;
}
#endif
