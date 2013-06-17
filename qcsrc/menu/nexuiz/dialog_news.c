#ifdef INTERFACE
CLASS(NexuizNewsDialog) EXTENDS(NexuizTab)
	METHOD(NexuizNewsDialog, fill, void(entity))
	ATTRIB(NexuizNewsDialog, title, string, _("Campus"))
	ATTRIB(NexuizNewsDialog, color, vector, SKINCOLOR_DIALOG_SETTINGS)
	ATTRIB(NexuizNewsDialog, intendedWidth, float, 0.96)
	ATTRIB(NexuizNewsDialog, rows, float, 24)
	ATTRIB(NexuizNewsDialog, columns, float, 1)
ENDCLASS(NexuizNewsDialog)
entity campus_browser;
#endif

#ifdef IMPLEMENTATION
entity makeNexuizNewsDialog()
{
	entity me;
	me = spawnNexuizNewsDialog();
	me.configureDialog(me);
	return me;
}

void fillNexuizNewsDialog(entity me)
{
	entity e;
	me.TR(me);
		me.TD(me, 24, 1, e = campus_browser = spawnGecko());
		e.configureBrowser( e, "http://campus.edu-shooter.de", true );
}
#endif
