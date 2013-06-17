#ifdef INTERFACE
CLASS(Edu2ProfileDialog) EXTENDS(NexuizTab)
	METHOD(Edu2ProfileDialog, fill, void(entity))
	ATTRIB(Edu2ProfileDialog, title, string, _("Profile"))
	ATTRIB(Edu2ProfileDialog, color, vector, SKINCOLOR_DIALOG_SETTINGS)
	ATTRIB(Edu2ProfileDialog, intendedWidth, float, 0.96)
	ATTRIB(Edu2ProfileDialog, rows, float, 24)
	ATTRIB(Edu2ProfileDialog, columns, float, 1)
ENDCLASS(Edu2ProfileDialog)
entity profile_browser;
#endif

#ifdef IMPLEMENTATION
entity makeEdu2ProfileDialog()
{
	entity me;
	me = spawnEdu2ProfileDialog();
	me.configureDialog(me);
	return me;
}

void fillEdu2ProfileDialog(entity me)
{
	entity e;
	me.TR(me);
		me.TD(me, 24, 1, e = profile_browser = spawnGecko());
		e.configureBrowser( e, "http://www.facebook.com", true );
}
#endif
