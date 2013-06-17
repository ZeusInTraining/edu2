#ifdef INTERFACE
CLASS(NexuizQuitDialog) EXTENDS(NexuizDialog)
	METHOD(NexuizQuitDialog, fill, void(entity))
	ATTRIB(NexuizQuitDialog, title, string, _("Quit"))
	ATTRIB(NexuizQuitDialog, color, vector, SKINCOLOR_DIALOG_QUIT)
	ATTRIB(NexuizQuitDialog, intendedWidth, float, 0.5)
	ATTRIB(NexuizQuitDialog, rows, float, 3)
	ATTRIB(NexuizQuitDialog, columns, float, 2)
ENDCLASS(NexuizQuitDialog)
#endif

#ifdef IMPLEMENTATION
void fillNexuizQuitDialog(entity me)
{
	me.moveable = true;

	entity e;
	me.TR(me);
		me.TD(me, 1, 2, makeNexuizTextLabel(0.5, _("Are you sure you want to quit?")));
	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizCommandButton(_("Yes"), '0.6 0 0', "quit", 0));
		me.TD(me, 1, 1, e = makeNexuizButton(_("No"), '0 0.6 0'));
			e.onClick = Dialog_Close;
			e.onClickEntity = me;
}
#endif
