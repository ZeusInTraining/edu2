#ifdef INTERFACE
CLASS(MainWindow) EXTENDS(ModalController)
	METHOD(MainWindow, configureMainWindow, void(entity))
	METHOD(MainWindow, focusEnter, void(entity))
	
	ATTRIB(MainWindow, mutatorsDialog, entity, NULL)
	ATTRIB(MainWindow, mapInfoDialog, entity, NULL)
	ATTRIB(MainWindow, userbindEditDialog, entity, NULL)
	ATTRIB(MainWindow, winnerDialog, entity, NULL)
	ATTRIB(MainWindow, fadedAlpha, float, SKINALPHA_BEHIND)
	ATTRIB(MainWindow, name, string, "main" )
ENDCLASS(MainWindow)
#endif

#ifdef IMPLEMENTATION

void DemoButton_Click(entity me, entity other)
{
	if(me.text == _("Do not press this button again!"))
		DialogOpenButton_Click(me, other);
	else
		me.setText(me, _("Do not press this button again!"));
}

void(entity me) focusEnterMainWindow = {
	menuHideBackground = false;
};

void configureMainWindowMainWindow(entity me)
{
	entity n, i;

	i = spawnNexuizTeamSelectDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	i = spawnChatDialog();
	i.configureDialog(i);
	me.addItem(me, i, '0 0.5 0', i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
	
	i = spawnLoginDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
	
	i = spawnLogoutDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	i = spawnMineSweeper();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
	
	i = spawnEdu2BuyDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	i = spawnGameStubDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	me.mutatorsDialog = i = spawnNexuizMutatorsDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	me.mapInfoDialog = i = spawnNexuizMapInfoDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	me.userbindEditDialog = i = spawnNexuizUserbindEditDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	me.winnerDialog = i = spawnNexuizWinnerDialog();
	i.configureDialog(i);
	me.addItemCentered(me, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);

	n = spawnNexuizNexposee();
	
		if(checkextension("DP_GECKO_SUPPORT"))
		{
			/*
			i = spawnNexuizNewsDialog();
			i.configureDialog(i);
			n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
			// replaces the singleplayer dialog
			n.setNexposee(n, i, SKINPOSITION_DIALOG_SINGLEPLAYER, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);
			
			i = spawnEdu2ProfileDialog();
			i.configureDialog(i);
			n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
			// replaces the multiplayer dialog
			n.setNexposee(n, i, SKINPOSITION_DIALOG_MULTIPLAYER, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);*/
			
			i = spawnEdu2InfoTabsDialog();
			i.configureDialog(i);
			n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
			// replaces the multiplayer dialog
			n.setNexposee(n, i, SKINPOSITION_DIALOG_MULTIPLAYER, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);
		}
	
		i = spawnGameListDialog();
		i.configureDialog(i);
		n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
		n.setNexposee(n, i, SKINPOSITION_DIALOG_SINGLEPLAYER, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);
	
	/*
		i = spawnNexuizMultiplayerDialog();
		i.configureDialog(i);
		n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
		n.setNexposee(n, i, SKINPOSITION_DIALOG_MULTIPLAYER, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);
	*/
		i = spawnNexuizSettingsDialog();
		i.configureDialog(i);
		n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
		n.setNexposee(n, i, SKINPOSITION_DIALOG_SETTINGS, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);

		i = spawnNexuizCreditsDialog();
		i.configureDialog(i);
		n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
		n.setNexposee(n, i, SKINPOSITION_DIALOG_CREDITS, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);
		n.pullNexposee(n, i, eY * (SKINHEIGHT_TITLE * SKINFONTSIZE_TITLE / conheight));

		// TODO: fix the position this is only for testing purposes!
		/*i = spawnMineSweeper();
		i.configureDialog(i);
		n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
		n.setNexposee(n, i, (SKINPOSITION_DIALOG_CREDITS + SKINPOSITION_DIALOG_QUIT) * 0.5, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);
		n.pullNexposee(n, i, eY * (SKINHEIGHT_TITLE * SKINFONTSIZE_TITLE / conheight));*/

		i = spawnNexuizQuitDialog();
		i.configureDialog(i);
		n.addItemCentered(n, i, i.intendedWidth * eX + i.intendedHeight * eY, SKINALPHAS_MAINMENU_z);
		n.setNexposee(n, i, SKINPOSITION_DIALOG_QUIT, SKINALPHAS_MAINMENU_x, SKINALPHAS_MAINMENU_y);
		n.pullNexposee(n, i, eY * (SKINHEIGHT_TITLE * SKINFONTSIZE_TITLE / conheight));
	me.addItem(me, n, '0 0 0', '1 1 0', SKINALPHAS_MAINMENU_z);
	me.moveItemAfter(me, n, NULL);

	me.initializeDialog(me, n);
	// we want the login dialog first!
	DialogOpenButton_Click_withCoords( NULL, findstring( NULL, name, "LoginDialog" ), '1 1 0', '0 0 0');
}
#endif

// click. The C-word so you can grep for it.
