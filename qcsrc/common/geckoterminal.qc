// shared geckoterminal entity

#ifdef SERVER
float( entity viewer ) gecko_terminal_send = {
	WriteByte( MSG_ENTITY, ET_GECKO );
	WriteShort( MSG_ENTITY, self.modelindex );
	WriteCoord( MSG_ENTITY, self.origin_x );
	WriteCoord( MSG_ENTITY, self.origin_y );
	WriteCoord( MSG_ENTITY, self.origin_z );
	WriteString( MSG_ENTITY, self.targetURL );
	return true;
}

void() gecko_terminal_reset_flags = {
	self.effects = 0;
}

void() gecko_terminal = {
	self.solid = SOLID_BSP;
	self.movetype = MOVETYPE_NONE;

	self.classname = "gecko_terminal";
	setorigin (self, self.origin);
	setmodel (self, self.model);
	
	self.SendEntity = gecko_terminal_send;
	// send it once
	self.Version = 1;
	
	self.effects = EF_NODEPTHTEST;
	self.think = gecko_terminal_reset_flags;
	self.nextthink = time + 0.01;
};
#endif
#ifdef CLIENT
entity gecko_terminal_current;
float gecko_terminal_hasFocus;

void() gecko_init_browser_data = {
	return;
}

// csqc-only fields
.string geckoInstanceName;
// used to determine the correct distance from the terminal - is there a better solution? (origin is always 0 0 0)
.vector centroid;

void() gecko_terminal_update =
{
	setmodelindex( self, readshort() );
	vector lOrigin;
	lOrigin_x = readcoord();
	lOrigin_y = readcoord();
	lOrigin_z = readcoord();
	setorigin( self, lOrigin );

	// TODO: copy String_* from the scmenu.. BlackHC
	strunzone( self.targetURL );
	self.targetURL = strzone( readstring() );
//	gecko_navigateex( self.geckoInstanceName, strcat( "javascript:setHome(\"http://", self.targetURL, "\");" ) );
}

void() gecko_terminal_removeEvent = {
	// dont delete terminals
	// they are static
}

void() gecko_terminal_spawnFromMessage = {
	// dont respawn if the terminal only reenters the view
	if( self.solid == 4 ) {
		gecko_terminal_update();
		return;
	}
	self.solid = 4;

	setmodelindex( self, readshort() );
	vector lOrigin;
	lOrigin_x = readcoord();
	lOrigin_y = readcoord();
	lOrigin_z = readcoord();
	setorigin( self, lOrigin );
	self.targetURL = strzone( readstring() );

	self.drawmask = MASK_NORMAL;

	float lSurfaceIndex;
	string lTextureName;

	for( lSurfaceIndex = 0; lTextureName = getsurfacetexture( self, lSurfaceIndex ) ; ++lSurfaceIndex ) {
		// test whether it's a dynamic texture
		if( substring( lTextureName, 0, GECKO_PATH_PREFIX_LENGTH ) == GECKO_PATH_PREFIX ) {
			// this is the surface now calculate the bary center
			float lNumPoints;
			lNumPoints = getsurfacenumpoints( self, lSurfaceIndex );
			float lIndex;
			vector lCentroid;
			lCentroid = '0 0 0';
			for( lIndex = 0 ; lIndex < lNumPoints ; ++lIndex ) {
				vector lPoint;
				lPoint = getsurfacepoint( self, lSurfaceIndex, lIndex );
				lCentroid = lCentroid + lPoint;
			}
			lCentroid = lCentroid * (1 / lNumPoints);
			self.centroid = lCentroid;
			break;
		}
	}

	// remove the "textures/"
	self.geckoInstanceName = strzone( substring( lTextureName, 9, 1000 ) );

	dprint( self.geckoInstanceName, ": ", self.targetURL, "\n" );

	gecko_create( self.geckoInstanceName );
	if( substring( self.targetURL, 0, 7 ) != "http://" ) {
		string lTemp;
		lTemp = strcat( "http://", self.targetURL );
		strunzone( self.targetURL );
		self.targetURL = strzone( lTemp );
	}
	
	registercvar( "gecko_terminal_width", "1024", CVAR_SAVE );
	registercvar( "gecko_terminal_height", "512", CVAR_SAVE );
	gecko_resize( self.geckoInstanceName, cvar( "gecko_terminal_width" ), cvar( "gecko_terminal_height" ) );
	gecko_navigateex( self.geckoInstanceName, strcat( "http://www.icculus.org/~black/stuff/terminalbrowser.html?", self.targetURL ) );

	self.classname = "gecko_terminal";
	self.RemoveEvent = gecko_terminal_removeEvent;
	self.UpdateEntity = gecko_terminal_update;
};

// only x and y are used
float(vector lAxis) _isAxis = {
	vector lDirection;
	lDirection = normalize( lAxis );
	if( fabs( lDirection_x ) > 0.9 || fabs( lDirection_y ) > 0.9 ) {
		return true;
	}
	return false;
}

void handle_gecko_terminal() {
	setsensitivityscale( 1.0 );

	vector lTraceEnd, lOrigin;
	lTraceEnd = cs_unproject( '0 0 100' );
	lOrigin = cs_unproject( '0 0 0' );
	traceline( lOrigin, lTraceEnd, 0, NULL );
	if( trace_ent == NULL || trace_ent.classname != "gecko_terminal" || trace_plane_dist > GECKO_TERMINAL_MAX_DISTANCE ) {
		gecko_terminal_current = NULL;
		gecko_terminal_hasFocus = false;
		return;
	}
	gecko_terminal_current = trace_ent;

	float lNearestSurface;
	lNearestSurface = getsurfacenearpoint( gecko_terminal_current, trace_endpos );
	vector lClippedPoint;
	lClippedPoint = getsurfaceclippedpoint( gecko_terminal_current, lNearestSurface, trace_endpos );
	// make sure that we actually hit the right model (this is because of the nasty bmodel bug
	if( vlen( lClippedPoint - trace_endpos ) < 1.0 ) {
		// ok.. whatever...!!!
		// test whether this is a gecko surface
		string lSurfaceTexture;
		lSurfaceTexture = getsurfacetexture( gecko_terminal_current, lNearestSurface );
		if( substring( lSurfaceTexture, 0, GECKO_PATH_PREFIX_LENGTH ) == GECKO_PATH_PREFIX ) {
			// we're in so.. we just need to determine the coords..
			float lNumPoints;
			lNumPoints = getsurfacenumpoints( gecko_terminal_current, lNearestSurface );
			//print( ftos( lNumPoints ), "\n" );
			
			//vector lNormal;
			//lNormal = getsurfacenormal( gecko_terminal_current, lNearestSurface );
			//te_spark( trace_endpos, lNormal, 100 );
			
			// we have a quad so: assert lNumPoints == 4
			vector lTexFirst, lOther;
			vector lTexAxis1, lTexAxis2;
			
			lTexFirst = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 0, SPA_TEXCOORDS0 );
			
			// determine the two main axes (this assumes that its a rectangular screen)
			float lAxis1Index, lAxis2Index;
			lOther = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 1, SPA_TEXCOORDS0 );
			if( _isAxis( lOther - lTexFirst ) ) {
				lAxis1Index = 1;
				lOther = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 2, SPA_TEXCOORDS0 );
				if( _isAxis( lOther - lTexFirst ) ) {
					lAxis2Index = 2;
				} else {
					lAxis2Index = 3;
				}
			} else {
				lAxis1Index = 2;
				lAxis2Index = 3;
			}
			
			lTexAxis1 = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, lAxis1Index, SPA_TEXCOORDS0 ) - lTexFirst;
			lTexAxis2 = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, lAxis2Index, SPA_TEXCOORDS0 ) - lTexFirst;
			//print( vtos( lTexFirst ), vtos( lTexAxis1 ), vtos( lTexAxis2 ), "\n" );
			vector lAxis1, lAxis2, lFirst;
			lFirst = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 0, SPA_POSITION );
			//te_spark( lFirst, lNormal, 100 );
			lAxis1 = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, lAxis1Index, SPA_POSITION ) - lFirst;
			//te_spark( lFirst + lAxis1, lNormal, 100 );
			lAxis2 = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, lAxis2Index, SPA_POSITION ) - lFirst;
			//te_spark( lFirst + lAxis2, lNormal, 100 );
			//print( vtos( lAxis1 ), vtos( lAxis2 ), "\n" );
			
			vector lDiff;
			lDiff = trace_endpos - lFirst;
			
			vector lMapped;
			lMapped = lDiff * lAxis1 / (lAxis1 * lAxis1) * lTexAxis1 + lDiff * lAxis2 / (lAxis2 * lAxis2) * lTexAxis2 + lTexFirst;
			
			float x,y;
			x = lMapped_x;
			y = lMapped_y;
			
			/*vector lFirst, lSecond, lThird;
			lFirst = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 0, SPA_POSITION );
			lSecond = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 1, SPA_POSITION );
			lThird = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 2, SPA_POSITION );

			vector lDiff;
			lDiff = lClippedPoint - lFirst;
			vector lV1, lV2;
			lV1 = lSecond - lFirst;
			lV2 = lThird - lFirst;

			float lU, lV, lW, lX, lY, lZ;
			if( lV1_x  * lV2_z !=  lV1_z * lV2_x ) {
				lU = lV1_x; lV = lV2_x; lW = lDiff_x;
				lX = lV1_z; lY = lV2_z; lZ = lDiff_z;
			} else if( lV1_x  * lV2_y !=  lV1_y * lV2_x ) {
				lU = lV1_x; lV = lV2_x; lW = lDiff_x;
				lX = lV1_y; lY = lV2_y; lZ = lDiff_y;
			} else {
				lU = lV1_y; lV = lV2_y; lW = lDiff_y;
				lX = lV1_z; lY = lV2_z; lZ = lDiff_z;
			}

			float lAlpha, lBeta, lDet;
			lDet = lU * lY - lV * lX;
			lAlpha = (lY * lW - lV * lZ) / lDet;
			lBeta = (lU * lZ - lX * lW) / lDet;

			// now grab the texcoords
			lFirst = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 0, SPA_TEXCOORDS0 );
			lSecond = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 1, SPA_TEXCOORDS0 );
			lThird = getsurfacepointattribute( gecko_terminal_current, lNearestSurface, 2, SPA_TEXCOORDS0 );

			//print( vtos( lFirst ), vtos( lSecond ), vtos( lThird ), "\n" );
			//print( ftos(lAlpha), " ", ftos(lBeta), "\n" );

			vector lMapped;
			lMapped = lFirst + lAlpha * (lSecond - lFirst);
			lMapped = lMapped + lBeta * (lThird - lFirst);
			float x,y;
			x = lMapped_x;
			y = lMapped_y;
			*/
			
			x = x - floor( x );
			y = y - floor( y );
			
			//print( ftos( x ), " ", ftos( y ), "\n" );
			
			gecko_mousemove( gecko_terminal_current.geckoInstanceName, x, y );

			// for the fun of it, we'll scale mouse sensitvity, too (according to the distance)
			float lDistance;
			lDistance = vlen( gecko_terminal_current.centroid - lOrigin );
			// the farther away we are, the slower is should react (to be more accurate)
			// the magic number is a good distance and we use 0.5 as minimum speed
			if( lDistance > 0 ) {
				float lScale;
				lScale = 0.5 + 30.0 / lDistance * 0.5;
				//print( ftos( lScale ), "\n" );
				setsensitivityscale( lScale );
			}

			// we need to draw the cursor (its in the screen center atm)
			vector lVideoSize;
			lVideoSize_x = vid_width;
			lVideoSize_y = vid_height;
			lVideoSize_z = 0;

			drawpic( lVideoSize * 0.5, GECKO_TERMINAL_CURSOR, draw_getimagesize( GECKO_TERMINAL_CURSOR ) * GECKO_TERMINAL_CURSOR_SCALE, '1 1 1', 1.0, 0 );
			return;
		}
	}
};

float() gecko_show_crosshair =
{
	return gecko_terminal_current == NULL;
}

// key queue
#define GECKO_KEY_QUEUE_SIZE 	4
float gecko_key_queue[GECKO_KEY_QUEUE_SIZE];
// incremented for all other keys when another key is released (the highest value/oldest key is released)
float gecko_key_queue_order[GECKO_KEY_QUEUE_SIZE];

void() gecko_key_queue_release_oldest = {
	float i, oldest;
	oldest = 0;
	for( i = 0 ; i < GECKO_KEY_QUEUE_SIZE ; i++ ) {
	}
}


float gecko_handle_input (float event, float scancode, float input_ascii)
{
	if( gecko_terminal_current != NULL ) {
		if( !gecko_terminal_hasFocus ) {
			if( scancode == K_MOUSE1 && event == 0 ) {
				gecko_terminal_hasFocus = true;
			} else {
				return false;
			}
		} else if( scancode == K_ESCAPE && event == 0 ) {
			gecko_terminal_hasFocus = false;
		}

		float eventtype;
		if( event == 0 ) {
			eventtype = GECKO_BUTTON_DOWN;
		} else if( event == 1 ) {
			eventtype = GECKO_BUTTON_UP;
		}

		float keycode;
		keycode = ((input_ascii >= 32) && (input_ascii < 128)) ? input_ascii : scancode;

		if ((keycode == K_ALT) || (keycode == K_CTRL) || (keycode == K_SHIFT))
			return false;

		//print("scancode = ", ftos (scancode), " input_ascii = ", ftos (input_ascii), "\n");

		gecko_keyevent( gecko_terminal_current.geckoInstanceName, keycode, eventtype );
		// always return true when we are still in focus (also eat the escape)
		return true;
	}
	return false;
}
#endif
