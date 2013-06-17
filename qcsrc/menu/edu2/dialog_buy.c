#ifdef INTERFACE
CLASS(Edu2BuyDialog) EXTENDS(NexuizRootDialog)
	METHOD(Edu2BuyDialog, fill, void(entity))
	ATTRIB(Edu2BuyDialog, title, string, _("Shop"))
	ATTRIB(Edu2BuyDialog, color, vector, SKINCOLOR_DIALOG_SETTINGS)
	ATTRIB(Edu2BuyDialog, intendedWidth, float, 0.96)
	ATTRIB(Edu2BuyDialog, rows, float, 24)
	ATTRIB(Edu2BuyDialog, columns, float, 1)
	ATTRIB(Edu2BuyDialog, name, string, "BuyDialog")
ENDCLASS(Edu2BuyDialog)
#endif

#ifdef IMPLEMENTATION
entity makeEdu2BuyDialog()
{
	entity me;
	me = spawnEdu2BuyDialog();
	me.configureDialog(me);
	return me;
}

void fillEdu2BuyDialog(entity me)
{
	entity e;
	me.TR(me);
		me.TD(me, 24, 1, e = spawnGecko());
		e.configureBrowser( e, "http://www.amazon.com", true );
}
#endif
