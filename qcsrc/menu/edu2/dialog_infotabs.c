// AK 08
#ifdef INTERFACE
CLASS(Edu2InfoTabsDialog) EXTENDS(NexuizDialog)
	METHOD(Edu2InfoTabsDialog, fill, void(entity))
	ATTRIB(Edu2InfoTabsDialog, title, string, _("Edu2 Portal"))
	ATTRIB(Edu2InfoTabsDialog, color, vector, SKINCOLOR_DIALOG_MULTIPLAYER)
	ATTRIB(Edu2InfoTabsDialog, intendedWidth, float, 0.96)
	ATTRIB(Edu2InfoTabsDialog, rows, float, 26)
	ATTRIB(Edu2InfoTabsDialog, columns, float, 6)
	
	ATTRIB(Edu2InfoTabsDialog, campusButton, entity, NULL)
	ATTRIB(Edu2InfoTabsDialog, profileButton, entity, NULL)
	ATTRIB(Edu2InfoTabsDialog, charSetupButton, entity, NULL)
ENDCLASS(Edu2InfoTabsDialog)

void() Edu2InfoTabsDialog_onLoggedIn;
void() Edu2InfoTabsDialog_onLoggedOut;
void() Edu2InfoTabsDialog_updateLoginState;

entity loginout_browser;
#endif

#ifdef IMPLEMENTATION
entity Edu2InfoTabsDialog_logInOutButton;
float Edu2InfoTabsDialog_isLoggedIn;

void() Edu2InfoTabsDialog_updateLoginState = {
	if( gamestatus & GAME_CONNECTED ) {
		if( !Edu2InfoTabsDialog_isLoggedIn ) {
			Edu2InfoTabsDialog_onLoggedIn();
		}
		Edu2InfoTabsDialog_isLoggedIn = true;
	} else {
		if( Edu2InfoTabsDialog_isLoggedIn ) {
			Edu2InfoTabsDialog_onLoggedOut();
		}
		Edu2InfoTabsDialog_isLoggedIn = false;
	}
};

void() Edu2InfoTabsDialog_onLoggedIn = {
	entity e;
	e = Edu2InfoTabsDialog_logInOutButton;
	e.configureNexuizButton( e, _("Log Out"), '0.6 0 0' );
	
	// TODO: hack hack
	e.onClickEntity = findstring( NULL, name, "LogoutDialog" );
	
	entity lInfoDialog;
	lInfoDialog = e.parent;
	lInfoDialog.campusButton.disabled = false;
	lInfoDialog.profileButton.disabled = false;
	
	entity lCampusButton;
	lCampusButton = lInfoDialog.campusButton;
	lCampusButton.onClick( lCampusButton, lCampusButton.onClickEntity );
};

void() Edu2InfoTabsDialog_onLoggedOut = {
	entity e;
	e = Edu2InfoTabsDialog_logInOutButton;
	e.configureNexuizButton( e, _("Log In"), '0 0.6 0' );
	
	e.onClickEntity = findstring( NULL, name, "LoginDialog" );
	
	entity lInfoDialog;
	lInfoDialog = e.parent;
	lInfoDialog.campusButton.disabled = true;
	lInfoDialog.profileButton.disabled = true;
	
	entity lCharSetupButton;
	lCharSetupButton = lInfoDialog.charSetupButton;
	lCharSetupButton.onClick( lCharSetupButton, lCharSetupButton.onClickEntity );	
}

void fillEdu2InfoTabsDialog(entity me)
{
	entity mc, e;
	mc = makeNexuizTabController(me.rows - 2);
	me.TR(me);
		e = Edu2InfoTabsDialog_logInOutButton = makeNexuizButton( "", '1 1 1' );
		e.onClick = DialogOpenButton_Click;
		me.TD( me, 1, 1, e );
		
		me.TD(me, 1, 1, e = me.campusButton = mc.makeTabButton(mc, _("Campus"),  makeNexuizNewsDialog()));
		me.TD(me, 1, 1, e = me.profileButton = mc.makeTabButton(mc, _("Profile"),  makeEdu2ProfileDialog()));
			//setDependentStringNotEqual(e, "_cl_name", _("Player"));
			
		me.TD(me, 1, 1, e = me.charSetupButton = mc.makeTabButton(mc, _("Character Setup"),  makeNexuizPlayerSettingsTab()));
			if(cvar_string("_cl_name") == _("Player"))
				e.onClick(e, e.onClickEntity); // lol animation
		
		Edu2InfoTabsDialog_onLoggedOut();
	me.TR(me);
	me.TR(me);
		me.TD(me, me.rows - 2, me.columns, mc);
}
#endif

