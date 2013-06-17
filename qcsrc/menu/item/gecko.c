// Andreas Kirsch Gecko item (to test it)
#ifdef INTERFACE
CLASS(Gecko) EXTENDS(Item)
	METHOD( Gecko, configureBrowser, void( entity, string, float ) )
	METHOD( Gecko, navigate, void( entity, string, float ) )
	METHOD( Gecko, goHome, void(entity) )
	METHOD( Gecko, draw, void(entity))
	METHOD( Gecko, keyDown, float(entity, float, float, float))
	METHOD( Gecko, keyUp, float(entity, float, float, float))
	METHOD( Gecko, executeScript, void(entity, string))
	METHOD( Gecko, mouseMove, float(entity, vector))
	METHOD( Gecko, mouseDrag, float(entity, vector))
	METHOD( Gecko, resizeNotify, void(entity, vector, vector, vector, vector))
	ATTRIB( Gecko, texturePath, string, string_null )
	ATTRIB( Gecko, textureExtent, vector, '0 0 0')
	ATTRIB( Gecko, homeURI, string, string_null )
ENDCLASS(Item)
#endif

#ifdef IMPLEMENTATION
// define static members
float _gecko_instanceNumber;

void configureBrowserGecko( entity me, string URI, float useTerminalBrowser ) {
	me.focusable = 1;

	//create a new gecko object if needed
	if( !me.texturePath ) {
		me.texturePath = strzone( strcat( "_dynamic/gecko/menu/",  ftos( _gecko_instanceNumber ) ) );
		_gecko_instanceNumber+=1;
		// TODO: add error checks
		gecko_create( me.texturePath );
	}
	string lFullURI;
	if( useTerminalBrowser ) {
		lFullURI = strcat( "http://www.icculus.org/~black/stuff/terminalbrowser.html?", URI );
	} else {
		lFullURI = URI;
	}
	// TODO: FIXME possible leak!
	me.homeURI = strzone( lFullURI );
	gecko_navigateex( me.texturePath, lFullURI);
	//gecko_navigate( me.texturePath, "about:blank" );
	//gecko_navigate( me.texturePath, "javascript:Darkplaces.query( 123 );" );
}

void navigateGecko( entity me, string URI, float useTerminalBrowser) {
	string lFullURI;
	if( useTerminalBrowser ) {
		lFullURI = strcat( "http://www.icculus.org/~black/stuff/terminalbrowser.html?", URI );
	} else {
		lFullURI = URI;
	}
	if not( me.homeURI ) {
		strunzone( me.homeURI );
	}
	me.homeURI = strzone( lFullURI );
	gecko_navigateex( me.texturePath, lFullURI);
}

void goHomeGecko( entity me ) {
	gecko_navigateex( me.texturePath, me.homeURI );
}

void executeScriptGecko( entity me, string pCode ) {
	gecko_navigateex( me.texturePath, strcat( "javascript:", pCode ) );
}

void drawGecko(entity me)
{
	vector drawSize;
  
	if( me.texturePath ) {
		/* The gecko browser is actually only drawn to a part of the
		   texture. Correct scaling so that part fills up the whole
		   item area. */
		drawSize_x = 1.0 / me.textureExtent_x;
		drawSize_y = 1.0 / me.textureExtent_y;
		draw_Picture( '0 0 0', strcat( "/", me.texturePath ), 
			drawSize, '1 1 1', 1.0 );
	} else {
		local vector fontsize;
		fontsize_x = fontsize_y = 1.0 / 30.0;
		fontsize_z = 0.0;
		draw_Text( '0 0 0', "Browser not initialized!", fontsize, '1 1 1', 1.0, 0 );
	}
}

float keyDownGecko(entity me, float scan, float ascii, float shift)
{
	if( scan == K_ESCAPE ) {
		return 0;
	}
	if (ascii >= 32)
		return gecko_keyevent( me.texturePath, ascii, GECKO_BUTTON_DOWN );
	else
		return gecko_keyevent( me.texturePath, scan, GECKO_BUTTON_DOWN );
}

float keyUpGecko(entity me, float scan, float ascii, float shift)
{
	if (ascii >= 32)
		return gecko_keyevent( me.texturePath, ascii, GECKO_BUTTON_UP );
	else
		return gecko_keyevent( me.texturePath, scan, GECKO_BUTTON_UP );
}

float mouseMoveGecko(entity me, vector pos)
{
	gecko_mousemove( me.texturePath, pos_x, pos_y );
	return 1;
}

float mouseDragGecko(entity me, vector pos)
{
	gecko_mousemove( me.texturePath, pos_x, pos_y );
	return 1;
}

void resizeNotifyGecko(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize)
{
	me.origin = absOrigin;
	me.size = absSize;
	
	vector lRealSize;
	lRealSize_x = absSize_x * cvar( "vid_width" ) / conwidth;
	lRealSize_y = absSize_y * cvar( "vid_height" ) / conheight;
	gecko_resize( me.texturePath, lRealSize_x, lRealSize_y );
	me.textureExtent = gecko_get_texture_extent( me.texturePath );
}

string toStringGecko(entity me)
{
	return me.texturePath;
}

#endif
