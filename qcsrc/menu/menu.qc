///////////////////////////////////////////////
// Menu Source File
///////////////////////
// This file belongs to dpmod/darkplaces
// AK contains all menu functions (especially the required ones)
///////////////////////////////////////////////

float mouseButtonsPressed;
vector menuMousePos;
float menuShiftState;
float menuPrevTime;
float menuAlpha;
float menuLogoAlpha;
float prevMenuAlpha;
float menuInitialized;
float menuNotTheFirstFrame;

// TODO: move into its own file
void() MessageLog_Init = {
	messageLog_StringBuffer = buf_create();
};

void( string message ) MessageLog_Add = {
	if( messageLog_endIndex < messageLog_maxNumMessages - 1 && !messageLog_wrapped ) {
		messageLog_endIndex = messageLog_endIndex + 1;
		bufstr_add( messageLog_StringBuffer, message, true );
	} else {
		messageLog_wrapped = true;
		messageLog_endIndex = mod( messageLog_endIndex + 1, messageLog_maxNumMessages );
		bufstr_set( messageLog_StringBuffer, messageLog_endIndex, message );
	}

	if( messageLog_listBox ) {
		messageLog_listBox.onAddMessage( messageLog_listBox );
	}
}

float() MessageLog_GetCount = {
	if( messageLog_wrapped ) {
		return messageLog_maxNumMessages;
	} else {
		return messageLog_endIndex;
	}
}

string( float index ) MessageLog_Get = {
	float wrappedIndex;
	if( messageLog_wrapped ) {
		wrappedIndex = mod( messageLog_endIndex + index - 1, messageLog_maxNumMessages );
	} else {
		wrappedIndex = index;
	}
	return bufstr_get( messageLog_StringBuffer, wrappedIndex );
}

void SUB_Null() { };

void() m_init =
{
	dprint_load();
	menu_registry = db_load( MENU_REGISTRY_FILE );
}

void UpdateConWidthHeight()
{
	float conwidth_s, conheight_s;
	conwidth_s = conwidth;
	conheight_s = conheight;
	conwidth = cvar("vid_conwidth");
	conheight = cvar("vid_conheight");
	if(conwidth < 800)
	{
		conheight *= 800 / conwidth;
		conwidth = 800;
	}
	if(conheight < 600)
	{
		conwidth *= 600 / conheight;
		conheight = 600;
	}
	if(main)
	{
		if(conwidth_s != conwidth || conheight_s != conheight)
		{
			draw_reset();
			main.resizeNotify(main, '0 0 0', eX * conwidth + eY * conheight, '0 0 0', eX * conwidth + eY * conheight);
		}
	}
}

void() m_init_delayed =
{
	float fh, glob, n, i;
	string s;

	dprint_load();

	// TODO: clean-up
	MessageLog_Init();

	menuInitialized = 0;
	if(!preMenuInit())
		return;
	menuInitialized = 1;
	GameCommand_Init();

	fh = -1;
	if(cvar_string("menu_skin") != "")
	{
		draw_currentSkin = strcat("gfx/menu/", cvar_string("menu_skin"));
		fh = fopen(strcat(draw_currentSkin, "/skinvalues.txt"), FILE_READ);
	}
	if(fh < 0)
	{
		draw_currentSkin = "gfx/menu/default";
		fh = fopen(strcat(draw_currentSkin, "/skinvalues.txt"), FILE_READ);
	}
	draw_currentSkin = strzone(draw_currentSkin);
	while((s = fgets(fh)))
		if(tokenize(s) == 2)
			Skin_ApplySetting(argv(0), argv(1));
	fclose(fh);

	glob = search_begin(strcat(draw_currentSkin, "/*.tga"), TRUE, TRUE);
	if(glob >= 0)
	{
		n = search_getsize(glob);
		for(i = 0; i < n; ++i)
			precache_pic(search_getfilename(glob, i));
		search_end(glob);
	}

	draw_setMousePointer(SKINGFX_CURSOR, SKINSIZE_CURSOR, SKINOFFSET_CURSOR);

	conwidth = conheight = -1;
	draw_reset();
	UpdateConWidthHeight();
	main = spawnMainWindow(); main.configureMainWindow(main);
	main.resizeNotify(main, '0 0 0', eX * conwidth + eY * conheight, '0 0 0', eX * conwidth + eY * conheight);
	main.focused = 1;
	menuShiftState = 0;
	menuMousePos = '0.5 0.5 0';

	if(Menu_Active)
		m_display(); // delayed menu display
};

void(float key, float ascii) m_keyup =
{
	if(!menuInitialized)
		return;
	if(!Menu_Active)
		return;
	draw_reset();
	main.keyUp(main, key, ascii, menuShiftState);
	if(key >= K_MOUSE1 && key <= K_MOUSE3)
	{
		--mouseButtonsPressed;
		if(!mouseButtonsPressed)
			main.mouseRelease(main, menuMousePos);
		if(mouseButtonsPressed < 0)
		{
			mouseButtonsPressed = 0;
			print("Warning: released an already released button\n");
		}
	}
	if(key == K_ALT) menuShiftState -= (menuShiftState & S_ALT);
	if(key == K_CTRL) menuShiftState -= (menuShiftState & S_CTRL);
	if(key == K_SHIFT) menuShiftState -= (menuShiftState & S_SHIFT);
};

void(float key, float ascii) m_keydown =
{
	if(!menuInitialized)
		return;
	if(!Menu_Active)
		return;
	if(keyGrabber)
	{
		entity e;
		e = keyGrabber;
		keyGrabber = NULL;
		e.keyGrabbed(e, key, ascii);
	}
	else
	{
		draw_reset();
		if(!main.keyDown(main, key, ascii, menuShiftState))
			if(key == K_ESCAPE)
				if(gamestatus & (GAME_ISSERVER | GAME_CONNECTED | GAME_DEVELOPER)) // don't back out to console only
					m_hide(); // disable menu on unhandled ESC
		if(key >= K_MOUSE1 && key <= K_MOUSE3)
			if(!mouseButtonsPressed)
				main.mousePress(main, menuMousePos);
	}
	if(key >= K_MOUSE1 && key <= K_MOUSE3)
	{
		++mouseButtonsPressed;
		if(mouseButtonsPressed > 10)
		{
			mouseButtonsPressed = 10;
			print("Warning: pressed an already pressed button\n");
		}
	}
	if(key == K_ALT) menuShiftState |= S_ALT;
	if(key == K_CTRL) menuShiftState |= S_CTRL;
	if(key == K_SHIFT) menuShiftState |= S_SHIFT;
};

void(string img, float a) drawBackground =
{
	if( menuHideBackground ) {
		return;
	}
	vector sz;
	vector isz;
	sz = draw_PictureSize(img);
	// keep aspect of image
	if(sz_x * draw_scale_y >= sz_y * draw_scale_x)
	{
		// that is, sz_x/sz_y >= draw_scale_x/draw_scale_y
		// match up the height
		isz_y = 1;
		isz_x = isz_y * (sz_x / sz_y) * (draw_scale_y / draw_scale_x);
	}
	else
	{
		// that is, sz_x/sz_y <= draw_scale_x/draw_scale_y
		// match up the width
		isz_x = 1;
		isz_y = isz_x * (sz_y / sz_x) * (draw_scale_x / draw_scale_y);
	}
	draw_Picture('0.5 0.5 0' - 0.5 * isz, img, isz, '1 1 1', a);
}

void() m_draw =
{
	float t;
	float realFrametime;

	if(main)
		UpdateConWidthHeight();

	if(!menuInitialized)
	{
		// TODO draw an info image about this situation
		m_init_delayed();
		return;
	}
	if(!menuNotTheFirstFrame)
	{
		menuNotTheFirstFrame = 1;
		if(Menu_Active)
		if(!cvar("menu_video_played"))
		{
			localcmd("set menu_video_played 1\n");
			menuLogoAlpha = -0.8; // no idea why, but when I start this at zero, it jumps instead of fading
		}
	}

	t = gettime();
	realFrametime = frametime = min(0.2, t - menuPrevTime);
	menuPrevTime = t;
	time += frametime;

	t = cvar("menu_slowmo");
	if(t)
	{
		frametime *= t;
		realFrametime *= t;
	}

	if(Menu_Active)
	{
		// dont hide the menu when we are connected 
		//if(getmousetarget() == MT_MENU && (getkeydest() == KEY_MENU || getkeydest() == KEY_MENU_GRABBED))
			setkeydest(keyGrabber ? KEY_MENU_GRABBED : KEY_MENU);
		//else {
			//m_hide();
		//}
	}
	
	if(cvar("cl_capturevideo"))
		frametime = cvar("menu_slowmo") / cvar("cl_capturevideo_fps"); // make capturevideo work smoothly

	dprint_load();
	gamestatus = 0;
	if(isserver())
		gamestatus = gamestatus | GAME_ISSERVER;
	if(clientstate() == CS_CONNECTED)
		gamestatus = gamestatus | GAME_CONNECTED;
	if(cvar("developer"))
		gamestatus = gamestatus | GAME_DEVELOPER;

	prevMenuAlpha = menuAlpha;
	if(Menu_Active)
	{
		if(menuAlpha == 0 && menuLogoAlpha < 2)
		{
			menuLogoAlpha = menuLogoAlpha + frametime * 2;
		}
		else
		{
			menuAlpha = min(1, menuAlpha + frametime * 5);
			menuLogoAlpha = 2;
		}
	}
	else
	{
		menuAlpha = max(0, menuAlpha - frametime * 5);
		menuLogoAlpha = 2;
	}

	draw_reset();

	if(!(gamestatus & (GAME_CONNECTED | GAME_ISSERVER)))
	{
		if(menuLogoAlpha > 0)
		{
			drawBackground(SKINGFX_BACKGROUND, bound(0, menuLogoAlpha, 1));
			if(menuAlpha <= 0 && SKINALPHA_CURSOR_INTRO > 0)
			{
				draw_alpha = SKINALPHA_CURSOR_INTRO * bound(0, menuLogoAlpha, 1);
				draw_drawMousePointer(menuMousePos);
				draw_alpha = 1;
			}
		}
	}
	else if(SKINALPHA_BACKGROUND_INGAME)
	{
		if(menuAlpha > 0)
			drawBackground(SKINGFX_BACKGROUND_INGAME, menuAlpha * SKINALPHA_BACKGROUND_INGAME);
	}
	
	// TODO: move this somewhere else - doesnt really make sense to have that here BlackHC
	Edu2InfoTabsDialog_updateLoginState();
	
	// TODO: move this somewhere else - Black
	if( menu_delayed_time != -1 && menu_delayed_time < time ) {
		menu_delayed_function();
		menu_delayed_time = -1;
	}

	draw_reset();
	preMenuDraw();
	draw_reset();

	if(menuAlpha <= 0)
	{
		if(prevMenuAlpha > 0)
			main.initializeDialog(main, main.firstChild);
		draw_reset();
		postMenuDraw();
		return;
	}

	draw_alpha *= menuAlpha;

	if(frametime > 0)
	{
		vector dMouse;
		dMouse = getmousepos() * (frametime / realFrametime); // for capturevideo
		if(dMouse != '0 0 0')
		{
			dMouse = globalToBoxSize(dMouse, draw_scale);
			menuMousePos += dMouse * 1; // TODO use a cvar here
			menuMousePos_x = bound(0, menuMousePos_x, 1);
			menuMousePos_y = bound(0, menuMousePos_y, 1);
			if(mouseButtonsPressed)
				main.mouseDrag(main, menuMousePos);
			else
				main.mouseMove(main, menuMousePos);
		}
	}
	main.draw(main);
	draw_alpha = max(draw_alpha, SKINALPHA_CURSOR_INTRO * bound(0, menuLogoAlpha, 1));

	draw_drawMousePointer(menuMousePos);

	draw_reset();
	postMenuDraw();

	frametime = 0;
};

void() m_display =
{
	Menu_Active = true;
	setkeydest(KEY_MENU);
	setmousetarget(MT_MENU);

	if(!menuInitialized)
		return;

	if(mouseButtonsPressed)
		main.mouseRelease(main, menuMousePos);
	mouseButtonsPressed = 0;

	main.focusEnter(main);
	main.showNotify(main);
};

void() m_hide =
{
	Menu_Active = false;
	setkeydest(KEY_GAME);
	setmousetarget(MT_CLIENT);

	if(!menuInitialized)
		return;

	main.focusLeave(main);
	main.hideNotify(main);
};

void() m_toggle =
{
	if(Menu_Active)
		m_hide();
	else
		m_display();
};

void() m_shutdown =
{
	m_hide();
	// save the registry
	db_save( menu_registry, MENU_REGISTRY_FILE );
	db_close( menu_registry );
};

void(string itemname) m_goto =
{
	entity e;
	if(!menuInitialized)
		return;
	if(itemname == "") // this can be called by GameCommand
	{
		if(gamestatus & (GAME_ISSERVER | GAME_CONNECTED))
			m_hide();
		else
		{
			main.initializeDialog(main, main.firstChild);
			m_display();
		}
	}
	else
	{
		e = findstring(NULL, name, itemname);
		if(e && e.parent == main)
		{
			m_hide();
			main.initializeDialog(main, e);
			m_display();
		}
	}
}

// semi-nasty hack (at least its encapsulated)
void RenotifyAll()
{
	conwidth = 0;
	UpdateConWidthHeight();
}


