#ifdef INTERFACE
CLASS(NexuizServerCreateTab) EXTENDS(NexuizTab)
	METHOD(NexuizServerCreateTab, fill, void(entity))
	METHOD(NexuizServerCreateTab, gameTypeChangeNotify, void(entity))
	ATTRIB(NexuizServerCreateTab, title, string, _("Create"))
	ATTRIB(NexuizServerCreateTab, intendedWidth, float, 0.9)
	ATTRIB(NexuizServerCreateTab, rows, float, 22)
	ATTRIB(NexuizServerCreateTab, columns, float, 6.5)

	ATTRIB(NexuizServerCreateTab, mapListBox, entity, NULL)
	ATTRIB(NexuizServerCreateTab, sliderFraglimit, entity, NULL)
	ATTRIB(NexuizServerCreateTab, sliderTimelimit, entity, NULL)
	ATTRIB(NexuizServerCreateTab, checkboxFraglimit, entity, NULL)
ENDCLASS(NexuizServerCreateTab)
entity makeNexuizServerCreateTab();
#endif

#ifdef IMPLEMENTATION

entity makeNexuizServerCreateTab()
{
	entity me;
	me = spawnNexuizServerCreateTab();
	me.configureDialog(me);
	return me;
}

void fillNexuizServerCreateTab(entity me)
{
	entity e, e0;
	float n;

	me.TR(me);
		n = 8 + 2 * !!cvar("developer");
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_dm", _("DM")));
			e0 = e;
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_tdm", _("TDM")));
			if(e.checked) e0 = NULL;
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_lms", _("LMS")));
			if(e.checked) e0 = NULL;
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_arena", _("Arena")));
			if(e.checked) e0 = NULL;
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_runematch", _("Rune")));
			if(e.checked) e0 = NULL;
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_domination", _("Dom")));
			if(e.checked) e0 = NULL;
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_keyhunt", _("Key Hunt")));
			if(e.checked) e0 = NULL;
		me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_ctf", _("CTF")));
			if(e.checked) e0 = NULL;
		if(cvar("developer"))
		{
			me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_assault", _("Assault")));
				if(e.checked) e0 = NULL;
			me.TD(me, 2, me.columns / n, e = makeNexuizGametypeButton(1, "g_onslaught", _("Onslaught")));
				if(e.checked) e0 = NULL;
		}
		if(e0)
		{
			//print("NO CHECK\n");
			e0.setChecked(e0, 1);
		}
	me.TR(me);
	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 3, e = makeNexuizTextLabel(0, _("Map list:")));
	me.TR(me);
		me.TD(me, me.rows - 7, 3, e = makeNexuizMapList());
		me.mapListBox = e;
	me.gotoRC(me, me.rows - 3, 0);
		me.TDempty(me, 0.5);
		me.TD(me, 1, 1, e = makeNexuizButton(_("All"), '0 0 0'));
			e.onClick = MapList_All;
			e.onClickEntity = me.mapListBox;
		me.TD(me, 1, 1, e = makeNexuizButton(_("None"), '0 0 0'));
			e.onClick = MapList_None;
			e.onClickEntity = me.mapListBox;
		me.TDempty(me, 0.5);

	me.gotoRC(me, 3, 3.5); me.setFirstColumn(me, me.currentColumn);
		me.TD(me, 1, 3, e = makeNexuizTextLabel(0, _("Settings:")));
	me.TR(me);
		me.sliderTimelimit = makeNexuizSlider(1.0, 60.0, 0.5, "timelimit_override");
		me.TD(me, 1, 1, e = makeNexuizSliderCheckBox(0, 1, me.sliderTimelimit, _("Time limit:")));
		me.TD(me, 1, 2, me.sliderTimelimit);
	me.TR(me);
		me.TDempty(me, 0.2);
		me.TD(me, 1, 2.8, e = makeNexuizSliderCheckBox(-1, 0, me.sliderTimelimit, _("Use map specified default")));
	me.TR(me);
		me.sliderFraglimit = makeNexuizSlider(1.0, 2000.0, 5, "fraglimit_override");
		me.TD(me, 1, 1, e = makeNexuizSliderCheckBox(0, 1, me.sliderFraglimit, _("Point limit:")));
			me.checkboxFraglimit = e;
		me.TD(me, 1, 2, me.sliderFraglimit);
	me.TR(me);
		me.TDempty(me, 0.2);
		me.TD(me, 1, 2.8, e = makeNexuizSliderCheckBox(-1, 0, me.sliderFraglimit, _("Use map specified default")));
	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Server UDP port:")));
		me.TD(me, 1, 0.5, e = makeNexuizInputBox(0, "port"));
	me.TR(me);
		me.TD(me, 1, 3, e = makeNexuizCheckBox(0, "g_antilag", _("AntiLag")));
	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Map voting:")));
		me.TD(me, 1, 2, e = makeNexuizTextSlider("g_maplist_votable"));
			e.addValue(e, _("No voting"), "0");
			e.addValue(e, _("2 choices"), "2");
			e.addValue(e, _("3 choices"), "3");
			e.addValue(e, _("4 choices"), "4");
			e.addValue(e, _("5 choices"), "5");
			e.addValue(e, _("6 choices"), "6");
			e.addValue(e, _("7 choices"), "7");
			e.configureNexuizTextSliderValues(e);
	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Number of players:")));
		me.TD(me, 1, 2, makeNexuizSlider(1, 32, 1, "menu_maxplayers"));
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Number of bots:")));
		me.TD(me, 1, 2, makeNexuizSlider(0, 7, 1, "bot_number"));
	me.TR(me);
		me.TDempty(me, 0.2);
		me.TD(me, 1, 0.8, e = makeNexuizTextLabel(0, _("Bot skill:")));
		me.TD(me, 1, 2, e = makeNexuizTextSlider("skill"));
			e.addValue(e, _("Botlike"), "0");
			e.addValue(e, _("Beginner"), "1");
			e.addValue(e, _("You will win"), "2");
			e.addValue(e, _("You can win"), "3");
			e.addValue(e, _("You might win"), "4");
			e.addValue(e, _("Advanced"), "5");
			e.addValue(e, _("Expert"), "6");
			e.addValue(e, _("Pro"), "7");
			e.addValue(e, _("Assassin"), "8");
			e.addValue(e, _("Unhuman"), "9");
			e.addValue(e, _("Godlike"), "10");
			e.configureNexuizTextSliderValues(e);
			setDependent(e, "bot_number", 0, -1);
	me.TR(me);
		me.TDempty(me, 0.2);
		me.TD(me, 1, 0.8, e = makeNexuizTextLabel(0, _("Bot names:")));
		me.TD(me, 1, 0.7, e = makeNexuizInputBox(1, "bot_prefix"));
			setDependent(e, "bot_number", 0, -1);
		me.TD(me, 1, 0.6, e = makeNexuizTextLabel(0.5, _("Spellbinder")));
			setDependent(e, "bot_number", 0, -1);
		me.TD(me, 1, 0.7, e = makeNexuizInputBox(1, "bot_suffix"));
			setDependent(e, "bot_number", 0, -1);
	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizButton(_("Mutators..."), '0 0 0'));
			e.onClick = DialogOpenButton_Click;
			e.onClickEntity = main.mutatorsDialog;
			main.mutatorsDialog.refilterEntity = me.mapListBox;
		me.TD(me, 1, 2, e0 = makeNexuizTextLabel(0, string_null));
			e0.textEntity = main.mutatorsDialog;
			e0.allowCut = 1;

	me.gotoRC(me, me.rows - 1, 0);
		me.TD(me, 1, me.columns, e = makeNexuizButton(_("Start!"), '0 0 0'));
			e.onClick = MapList_LoadMap;
			e.onClickEntity = me.mapListBox;
			me.mapListBox.startButton = e;

	me.gameTypeChangeNotify(me);
}

void GameType_ConfigureSliders(entity e, entity l, string pLabel, float pMin, float pMax, float pStep, string pCvar)
{
	e.configureNexuizSlider(e, pMin, pMax, pStep, pCvar);
	l.setText(l, pLabel);
}

void gameTypeChangeNotifyNexuizServerCreateTab(entity me)
{
	// tell the map list to update
	float gt;
	entity e, l;
	gt = MapInfo_CurrentGametype();
	e = me.sliderFraglimit;
	l = me.checkboxFraglimit;
	switch(gt)
	{
		case MAPINFO_TYPE_CTF:        GameType_ConfigureSliders(e, l, _("Point limit:"),  50,  500, 10, "g_ctf_capture_limit");      break;
		case MAPINFO_TYPE_DOMINATION: GameType_ConfigureSliders(e, l, _("Point limit:"),  50,  500, 10, "g_domination_point_limit"); break;
		case MAPINFO_TYPE_KEYHUNT:    GameType_ConfigureSliders(e, l, _("Point limit:"), 200, 1500, 50, "g_keyhunt_point_limit");    break;
		case MAPINFO_TYPE_RUNEMATCH:  GameType_ConfigureSliders(e, l, _("Point limit:"),  50,  500, 10, "g_runematch_point_limit");  break;
		case MAPINFO_TYPE_LMS:        GameType_ConfigureSliders(e, l, _("Lives:"),         3,   50,  1, "g_lms_lives_override");     break;
		default:                      GameType_ConfigureSliders(e, l, _("Frag limit:"),    5,  100,  5, "fraglimit_override");       break;
	}
	me.mapListBox.refilter(me.mapListBox);
}

#endif
