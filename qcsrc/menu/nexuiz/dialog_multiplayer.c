#ifdef INTERFACE
CLASS(NexuizMultiplayerDialog) EXTENDS(NexuizDialog)
	METHOD(NexuizMultiplayerDialog, fill, void(entity))
	ATTRIB(NexuizMultiplayerDialog, title, string, _("Multiplayer"))
	ATTRIB(NexuizMultiplayerDialog, color, vector, SKINCOLOR_DIALOG_MULTIPLAYER)
	ATTRIB(NexuizMultiplayerDialog, intendedWidth, float, 0.96)
	ATTRIB(NexuizMultiplayerDialog, rows, float, 24)
	ATTRIB(NexuizMultiplayerDialog, columns, float, 6)
ENDCLASS(NexuizMultiplayerDialog)
#endif

#ifdef IMPLEMENTATION
void fillNexuizMultiplayerDialog(entity me)
{
	entity mc, e;
	mc = makeNexuizTabController(me.rows - 2);
	me.TR(me);
		me.TD(me, 1, 1, e = mc.makeTabButton(mc, _("Join"),  makeNexuizServerListTab()));
			setDependentStringNotEqual(e, "_cl_name", _("Player"));
		me.TD(me, 1, 1, e = mc.makeTabButton(mc, _("Player Setup"),  makeNexuizPlayerSettingsTab()));
			if(cvar_string("_cl_name") == _("Player"))
				e.onClick(e, e.onClickEntity); // lol animation
		me.TD(me, 1, 1, e = mc.makeTabButton(mc, _("Create"),  makeNexuizServerCreateTab()));
			setDependentStringNotEqual(e, "_cl_name", _("Player"));
	me.TR(me);
	me.TR(me);
		me.TD(me, me.rows - 2, me.columns, mc);
}
#endif
