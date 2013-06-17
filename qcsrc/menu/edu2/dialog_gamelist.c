//AK 08
#ifdef INTERFACE
CLASS(GameListDialog) EXTENDS(NexuizDialog)
	METHOD(GameListDialog, fill, void(entity))
	ATTRIB(GameListDialog, title, string, _("Fun Zone"))
	ATTRIB(GameListDialog, color, vector, SKINCOLOR_DIALOG_SINGLEPLAYER)
	ATTRIB(GameListDialog, intendedWidth, float, 0.80)
	ATTRIB(GameListDialog, rows, float, 21)
	ATTRIB(GameListDialog, columns, float, 5)
	ATTRIB(GameListDialog, campaignBox, entity, NULL)
	ATTRIB(GameListDialog, name, string, "GameListDialog")
ENDCLASS(GameListDialog)
#endif

#ifdef IMPLEMENTATION
void fillGameListDialog(entity me)
{
	entity e;

	me.TR(me);
		me.TD(me, me.rows - 1, me.columns, me.campaignBox = makeGameList());

	me.gotoRC(me, me.rows - 1, 0);
		me.TD(me, 1, me.columns, e = makeNexuizButton(_("Start!"), '0 0 0'));
			e.onClick = GameList_StartGame;
			e.onClickEntity = me.campaignBox;
}
#endif
