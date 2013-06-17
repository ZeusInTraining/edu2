// AK 08
#ifdef INTERFACE
CLASS(LogoutDialog) EXTENDS(NexuizDialog)
	METHOD(LogoutDialog, fill, void(entity))
	ATTRIB(LogoutDialog, title, string, _("Logout"))
	ATTRIB(LogoutDialog, color, vector, SKINCOLOR_DIALOG_TEAMSELECT)
	ATTRIB(LogoutDialog, intendedWidth, float, 0.5)
	ATTRIB(LogoutDialog, rows, float, 3)
	ATTRIB(LogoutDialog, columns, float, 2)
	ATTRIB(LogoutDialog, name, string, "LogoutDialog")
	ATTRIB(LogoutDialog, closable, float, 0)
ENDCLASS(LogoutDialog)
#endif

#ifdef IMPLEMENTATION

/* JavaScript code:
for( var i = 0 ; i < document.links.length ; i++ ) {
    var str = String( document.links[i].href );
    if( str.search( "www.facebook.com/logout.php" ) != -1 ) {
        location.assign( str + "&confirm=1" );

    }
}
*/

void LogoutDialog_onYesClick(entity button, entity me)
{
	loginout_browser.executeScript( loginout_browser, 
	"for%28%20var%20i%20%3D%200%20%3B%20i%20%3C%20document.links.length%20%3B%20i++%20%29%20%7B%0A%20%20%20%20var%20str%20%3D%20String%28%20document.links%5Bi%5D.href%20%29%3B%0A%20%20%20%20if%28%20str.search%28%20%22www.facebook.com/logout.php%22%20%29%20%21%3D%20-1%20%29%20%7B%0A%20%20%20%20%20%20%20%20location.assign%28%20str%20+%20%22%26confirm%3D1%22%20%29%3B%0A%0A%20%20%20%20%7D%0A%7D%0A"
		);
	profile_browser.goHome( profile_browser );
	campus_browser.goHome( campus_browser );
	
	localcmd( "disconnect\n" );

	Edu2InfoTabsDialog_onLoggedOut();
	me.close( me );
}

void fillLogoutDialog(entity me)
{
	me.moveable = true;

	entity e;
	me.TR(me);
		me.TD(me, 1, 2, makeNexuizTextLabel(0.5, _("Are you sure you want to log out?")));
	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizButton(_("Yes"), '0.6 0 0'));
			e.onClick = LogoutDialog_onYesClick;
			e.onClickEntity = me;
		me.TD(me, 1, 1, e = makeNexuizButton(_("No"), '0 0.6 0'));
			e.onClick = Dialog_Close;
			e.onClickEntity = me;
}
#endif
