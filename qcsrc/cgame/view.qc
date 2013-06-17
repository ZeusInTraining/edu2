vector viewer_org;

// Set up any things needed by UpdateView
void View_Init()
{
	ViewModel_Init ();
}

void View_Update()
{
	// ALWAYS Clear Current Scene First
	clearscene();

	// Assign Standard Viewflags
	// Draw the World (and sky)
	setproperty(VF_DRAWWORLD, true);
	// Draw the Crosshair
	setproperty(VF_DRAWCROSSHAIR, gecko_show_crosshair() /* @@@ Here? */);
	// Draw the Engine Status Bar (the default Quake HUD)

	// BlackHC dont want that active at the moment - not going to die anytime soon or need ammo stats anyway
	setproperty(VF_DRAWENGINESBAR, false);

	// Set view position to player position
	// use the client's view_offset as set by the server
#ifdef CLIENTSIDE_PLAYERMODELS
	/* WORKAROUND: When player entities are versioned, pmove_org isn't
	   updated at all (stays at 0 0 0). [my guess: pmove_org is set to
	    the origin of the engine player entity - which is not the same as
	    the client entity. or so ~ res]
	  Also, assigning pmove_org doesn't work either (prolly gets assigned
	  before View_Update() is called).
	 */
	viewer_org = net_pmove_org;
#else
	viewer_org = pmove_org;
#endif
	
// this is temporary code to allow us to use a third person cam
	//vector headOrigin;
	//headOrigin = gettaginfo( localPlayer.torso, gettagindex( localPlayer.torso, "tag_head" ) );
	//viewer_org_z = headOrigin_z;
	viewer_org = viewer_org + localPlayer.viewOffset;
	//viewer_org_z = viewer_org_z + getstati( STAT_VIEWHEIGHT );
	
	makevectors( input_angles );
	float factor;
	factor = min( 1.0, vlen( localPlayer.velocity ) / 200 );
	vector lTraceDirection;
	lTraceDirection = (-80 * v_forward + '0 0 30') * factor;
	traceline( viewer_org, viewer_org + lTraceDirection, true, localPlayer );
	if( trace_fraction != 1.0 ) {
		viewer_org = viewer_org + lTraceDirection * trace_fraction * 0.7;
	} else {
		viewer_org = viewer_org + lTraceDirection * trace_fraction;
	}
	
	if( trace_startsolid ) {
		// not good - need to change the model size
		dprint( "Head clipped!\n" );
	}
	float lAlpha;
	lAlpha= factor;
	if( lAlpha<= 0.1 ) {
		lAlpha= -1;
	} else if( lAlpha> 1 ) {
		lAlpha= 1.0;
	}
	localPlayer.head.alpha = lAlpha;
	localPlayer.legs.alpha = lAlpha;
	localPlayer.torso.alpha = lAlpha;

	setproperty(VF_ORIGIN, viewer_org);
	setproperty(VF_ANGLES, input_angles);

	ViewModel_Update();

	// Setup Entities to be Rendered (include all base types; normal, engine and viewmodels)
	addentities(MASK_NORMAL | MASK_ENGINE | MASK_ENGINEVIEWMODELS);

	// Render the Scene
	renderscene();
}
