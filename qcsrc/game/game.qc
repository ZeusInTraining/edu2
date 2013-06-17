// from BlackHC's Zymotic branch
// game/game.qc

//  worldspawn        - Called when the world.. umm.. spawns!
//  main              - Have no idea when this is called...
//  StartFrame        - Called at the start of each frame
//  ClientConnect     - Called when a client connected to the server
//  ClientDisconnect  - Called when a client disconnects from the server
//  ClientKill        - Called when a client issues the 'kill' command
//  PutClientInServer - Called to spawn the clients player entity
//  PlayerPreThink    - Called every frame, before physics.
//  PlayerPostThink   - Called every frame, AFTER physics.
//  SetNewParms       - Called on level change
//  SetChangeParms    - Called on level change

void() worldspawn = {
	LocalChat_Init();
}

void() info_player_start = {
};

void() info_player_deathmatch = {
};

void() light =  {
	remove( self );
};

void() StartFrame = {
};

void() PlayerPreThink = {
	if( self.button2 && self.flags & FL_ONGROUND) {
		self.button2 = 0;
		self.flags -= FL_ONGROUND;
		self.velocity_z = self.velocity_z + 270;
	}
};

void() ClientKill = {
};

void() ClientConnect = {
};

entity( string pInfoType ) ChooseSpawnPoint = {
	float lIndex;
	lIndex = random() * 11;
	entity lEntity;
	lEntity = NULL;
	while( lIndex > 0 ) {
		lEntity = find( lEntity, classname, pInfoType );
		--lIndex;
	}
	if( lEntity == NULL ) {
		return find( NULL, classname, pInfoType );
	}
	return lEntity;
};

void() PutClientInServer = {
	local entity lSpawnpoint;

	lSpawnpoint = ChooseSpawnPoint( "info_player_start" );
	if( lSpawnpoint == NULL ) {
		lSpawnpoint = ChooseSpawnPoint( "info_player_deathmatch" );
	}
	if( lSpawnpoint ) {
		setorigin( self, lSpawnpoint.origin + '0 0 1' + '0 0 10' );
	}
	self.health = 100;
	self.classname = "player";
	self.movetype = MOVETYPE_WALK;
	self.solid = SOLID_SLIDEBOX;
	self.takedamage = DAMAGE_YES;
	self.view_ofs = '0 0 55';
	self.flags = FL_CLIENT;

	// Dummy model to cause entity being sent to client
	precache_model("models/dummy.mdl");
	setmodel( self, "models/dummy.mdl" );

#ifdef CLIENTSIDE_PLAYERMODELS
	self.SendEntity = Player_Send;
	self.Version = self.Version + 1;

	Player_InitModel (self);
#endif

	setsize (self, VEC_HULL_MIN * PLAYERMODEL_SCALE, VEC_HULL_MAX * PLAYERMODEL_SCALE);
};

void() ClientDisconnect = {
	Player_FinishModel (self);
};

void() SetNewParms = {
};

void() SetChangeParms = {
};

void() PlayerPostThink = {
#ifdef CLIENTSIDE_PLAYERMODELS
	self.Version = self.Version + 1;
#endif
};

void SV_ParseClientCommand(string s) {
	float lArgc;
	lArgc = tokenize(s);
	
	string lCmd;
	lCmd = argv( 0 );
	
	if(lCmd == "playermodel") {
		clientcommand(self,s);
		//bprint( "switching model.. '", self.playermodel, "'\n" );
		Player_UpdateModel( self );
		self.Version = self.Version + 1;
	} else if(lCmd == "taunt") {
		Player_Taunts (self);
	} else if(lCmd == "localsay") {
		LocalChat_Say( substring( s, 8, 10000 ) );
	} else {
		clientcommand(self,s);
	}
}
