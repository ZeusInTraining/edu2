// AK 08 login
#ifdef INTERFACE
CLASS(LoginDialog) EXTENDS(NexuizDialog)
	METHOD(LoginDialog, fill, void(entity)) // to be overridden by user to fill the dialog with controls
	METHOD(LoginDialog, focusEnter, void(entity))

	ATTRIB(LoginDialog, title, string, _("Login")) // ;)
	ATTRIB(LoginDialog, color, vector, SKINCOLOR_DIALOG_TEAMSELECT)
	// TODO:
	ATTRIB(LoginDialog, titleHeight, float, SKINFONTSIZE_NORMAL * SKINHEIGHT_NORMAL * 1.2 ) // pixels
	ATTRIB(LoginDialog, titleFontSize, float, SKINFONTSIZE_NORMAL * 1.2 ) // pixels

	ATTRIB(LoginDialog, intendedWidth, float, 0.4) // 0.4
	ATTRIB(LoginDialog, rows, float, 4) // 4
	ATTRIB(LoginDialog, columns, float, 3) // 3
	ATTRIB(LoginDialog, name, string, "LoginDialog")
	ATTRIB(LoginDialog, closable, float, 0)

	ATTRIB(LoginDialog, username, entity, NULL)
	ATTRIB(LoginDialog, password, entity, NULL)
	//ATTRIB(LoginDialog, rememberMe, entity, NULL)
	ATTRIB(LoginDialog, okButton, entity, NULL)
	ATTRIB(LoginDialog, offlineButton, entity, NULL )
ENDCLASS(LoginDialog)
var void(entity) LoginDialog_ExecAfter_CB;
entity LoginDialog_ExecAfter_Entity;
#endif

#ifdef IMPLEMENTATION
#define REG_EDU2_USERNAME "edu2/login/username"
#define REG_EDU2_PASSWORD "edu2/login/password"

void LoginDialog_ReloadProfile()
{
	profile_browser.goHome( profile_browser );
}

/* JavaScript code:
document.loginform.email.value="";
document.loginform.pass.value="";
document.loginform.persistent.checked=true;
if( document.loginform.persistent.checked.dialog != null )
	document.loginform.persistent.checked.dialog.hide();
login_form_change();

this.disabled=true; document.loginform.submit();
document.loginform.submit();
*/

void LoginDialog_onOkClick(entity button, entity me)
{
	// TODO: delay the whole thing? Black
	db_put( menu_registry, REG_EDU2_USERNAME, me.username.text );
	db_put( menu_registry, REG_EDU2_PASSWORD, me.password.text );
	loginout_browser.executeScript( loginout_browser, 
	strcat(
"document.loginform.email.value%3D%22", me.username.text, "%22%3B%0Adocument.loginform.pass.value%3D%22", me.password.text,
"%22%3B%0Adocument.loginform.persistent.checked%3Dtrue%3B%0Aif%28%20document.loginform.persistent.checked.dialog%20%21%3D%20null%20%29%0A%09document.loginform.persistent.checked.dialog.hide%28%29%3B%0Alogin_form_change%28%29%3B%0A%0Athis.disabled%3Dtrue%3B%20document.loginform.submit%28%29%3B%0Adocument.loginform.submit%28%29%3B"
		) );
	
	// TODO: needs to be delayed Black!!
	menu_delayed_time = time + 3.0;
	menu_delayed_function = LoginDialog_ReloadProfile;

	registercvar( "edu2logincommands", "map $edu2map", 0 );
	localcmd( cvar_string( "edu2logincommands" ), "\n" );
	Edu2InfoTabsDialog_onLoggedIn();
	
	if( LoginDialog_ExecAfter_CB ) {
		LoginDialog_ExecAfter_CB( LoginDialog_ExecAfter_Entity );
		LoginDialog_ExecAfter_CB = SUB_Null;
	}
	
	me.close( me );
}

void LoginDialog_onOfflineClick(entity button, entity me) {
	LoginDialog_ExecAfter_CB = SUB_Null;
	me.close( me );
}

void focusEnterLoginDialog(entity me)
{
	SUPER.focusEnter( me );
	me.setFocus( me, me.inputBox );
}

void fillLoginDialog(entity me)
{
	me.moveable = true;

	me.username = makeChatInputBox( true, false );
	// load old key from registry
	me.username.setText( me.username, db_get( menu_registry, REG_EDU2_USERNAME ) );
	//me.username.onEnterEntity = me;
	//me.username.onEnter = LoginDialog_onEnter;
	
	me.password = makeChatInputBox( false, true );
	me.password.setText( me.password, db_get( menu_registry, REG_EDU2_PASSWORD ) ),
	
	//me.password.onEnterEntity = me;
	//me.password.onEnter = LoginDialog_onEnter;

	me.okButton = makeNexuizButton( _("Ok"), '0 0.6 0' );
	me.okButton.onClick = LoginDialog_onOkClick;
	me.okButton.onClickEntity = me;
	
	me.offlineButton = makeNexuizButton( _("Stay Offline"), '0.6 0 0' );
	me.offlineButton.onClick = LoginDialog_onOfflineClick;
	me.offlineButton.onClickEntity = me;

	me.TR( me );
	me.TD( me, 1, 1, makeNexuizTextLabel( 0, _("User:") ) );
	me.TD( me, 1, 2, me.username );
	me.TR( me );
	me.TD( me, 1, 1, makeNexuizTextLabel( 0, _("Password:") ) );
	me.TD( me, 1, 2, me.password );
	me.TR( me );
	
	loginout_browser = spawnGecko();
	//me.TD( me, 16, 10, loginout_browser );
	loginout_browser.configureBrowser( loginout_browser, "http://www.facebook.com", false );
	
	me.TR( me );
	me.TDempty( me, 1 );
	me.TD( me, 1, 1, me.okButton );
	me.TD( me, 1, 1, me.offlineButton );
}
#endif

// click. The C-word so you can grep for it.
