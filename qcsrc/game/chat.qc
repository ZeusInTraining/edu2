// AK

// cvar name edu2_localchatdistance
// cvar name edu2_localchatdistance_wall
void() LocalChat_Init = {
	registercvar( "edu2_localchatdistance", "400" );
	registercvar( "edu2_localchatdistance_wall", "60" );
};

// self is the current client
void( string pMessage ) LocalChat_Say = {
	float lSafeDistance, lMaxDistance;
	lSafeDistance = cvar( "edu2_localchatdistance_wall" );
	lMaxDistance = cvar( "edu2_localchatdistance" );
	
	vector lOrigin;
	lOrigin = self.origin;
	//lOrigin_z = self.maxs_z;
	
	entity lClient;
	lClient = world;
	while( (lClient = nextent( lClient )) != world && (lClient.flags & FL_CLIENT) != 0 ) {
		vector lOtherOrigin;
		lOtherOrigin = lClient.origin;
		//lOtherOrigin_z = lClient.maxs_z;
		float lDistance;
		lDistance = vlen( lOtherOrigin - lOrigin );
		if( lDistance > lMaxDistance ) {
			continue;
		}
		if( lDistance > lSafeDistance && lClient != self ) {
			// do a trace
			traceline( lOrigin, lOtherOrigin, TRACE_WORLDONLY, self );
			// there's something in-between
			if( trace_fraction < 0.99 ) {
				continue;
			}
		}
		sprint( lClient, chr2str(1), self.netname, ": ", pMessage, "\n" );
	}
};
