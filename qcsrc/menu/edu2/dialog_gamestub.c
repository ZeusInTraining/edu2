// AK 08
// AK 08
#ifdef INTERFACE
CLASS(GameStubDialog) EXTENDS(NexuizDialog)
	METHOD(GameStubDialog, fill, void(entity))
	ATTRIB(GameStubDialog, title, string, _("Subgame"))
	ATTRIB(GameStubDialog, color, vector, SKINCOLOR_DIALOG_TEAMSELECT)
	ATTRIB(GameStubDialog, intendedWidth, float, 0.5)
	ATTRIB(GameStubDialog, rows, float, 3)
	ATTRIB(GameStubDialog, columns, float, 2)
	ATTRIB(GameStubDialog, name, string, "GameStubDialog")
	ATTRIB(GameStubDialog, closable, float, 0)
ENDCLASS(GameStubDialog)
entity gameStubDialog;
#endif

#ifdef IMPLEMENTATION

void fillGameStubDialog(entity me)
{
	gameStubDialog = me;
	
	me.moveable = true;

	entity e;
	me.TR(me);
		me.TD(me, 1, 2, makeNexuizTextLabel(0.5, _("Unfortunately there is no game and the cake is a lie.")));
	me.TR(me);
	me.TR(me);
		me.TDempty(me, 0.5);
		me.TD(me, 1, 1, e = makeNexuizButton(_("Okay"), '0.6 0 0'));
			e.onClick = Dialog_Close;
			e.onClickEntity = me;
}
#endif
