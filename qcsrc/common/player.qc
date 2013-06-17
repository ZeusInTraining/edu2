const float PLAYERUPD_LOCAL		= 1;
const float PLAYERUPD_NEWMODEL		= 2;
const float PLAYERUPD_CLIENTNUM		= 4;

#ifdef CLIENT
// TODO: instead of requiring the sqc to send so many coords optimize it to send just some flags, maybe?  And only send the angles that are necessary
// and use input_angles for the local player?
float Player_GetFittingAnimation()
{
	local vector v;
	local float best, s, anim;
	v = self.velocity;
	v_z = 0;
	makevectors(self.v_angle_y * '0 1 0');
	// less than 1 unit per second uses the stand anim
	best = 100;
	anim = ANIM_LEGS_IDLE;
	s = self.velocity * v_right  ;if (best < s) {best = s;anim = ANIM_LEGS_RUN;}
	s = s * -1                   ;if (best < s) {best = s;anim = ANIM_LEGS_RUN;}
	s = self.velocity * v_forward;if (best < s) {best = s;anim = ANIM_LEGS_RUN;}
	s = s * -1                   ;if (best < s) {best = s;anim = ANIM_LEGS_BACK;}

	if (anim == ANIM_LEGS_IDLE)
	{
		best = 1;
		s = self.velocity * v_right  ;if (best < s) {best = s;anim = ANIM_LEGS_RUN;}
		s = s * -1                   ;if (best < s) {best = s;anim = ANIM_LEGS_RUN;}
		s = self.velocity * v_forward;if (best < s) {best = s;anim = ANIM_LEGS_RUN;}
		s = s * -1                   ;if (best < s) {best = s;anim = ANIM_LEGS_BACK;}
	}
	//if (!(self.flags & FL_ONGROUND))
		//anim = ANIM_LEGS_JUMP;
	/*if (self.deadflag & DEAD_DYING)
		animtype = ANIMTYPE_DYING;
	if (self.deadflag & DEAD_DEAD)
		animtype = ANIMTYPE_DEAD;*/
	return anim;
}

void Player_OnFrameChange_Torso( entity player, float finished );
// TODO: if we could always use self, why have player then -- think about torso animations..
void Player_OnFrameChange_Legs( entity player, float finished )
{
	float lAnim;
	lAnim = Player_GetFittingAnimation();
	// reset animating if the old anim is finished or the target anim has changed
	if( finished || lAnim != self.currentAnim ) {
		PlayerModel_StartAnimation(player, lAnim,
			Player_OnFrameChange_Legs, Player_OnFrameChange_Torso);
	}
}

void Player_OnFrameChange_Torso( entity player, float finished )
{
	float lAnim;
	lAnim = ANIM_TORSO_STAND;
	if( finished ) {
		PlayerModel_StartAnimation(player, lAnim,
			Player_OnFrameChange_Legs, Player_OnFrameChange_Torso);
	}
}

void Player_Think()
{
	local vector playerAngles;
	local vector torsoAngles;
	local vector headAngles;
	// TODO: change the code back to make use of generic ANIMTYPES that convey more information
	// TODO: also think about what stuff should be part of PlayerModel and what of Player
	// I guess this angles and velocity faff doesnt need to be part of PlayerModel because PlayerModel will be the modul that the menu will use too
	// and we dont want to fake velocity and angles in there, do we?
	// Still give it a thought..

	// update vectors, too
	if (self.currentAnim == ANIM_LEGS_IDLE )
		playerAngles = self.v_angle_y * '0 1 0';
	else if (self.currentAnim == ANIM_LEGS_BACK )
		playerAngles = vectoyaw(self.velocity * -1) * '0 1 0';
	else
		playerAngles = vectoyaw(self.velocity) * '0 1 0';

	// update torso (relative to legs)
	torsoAngles_x = bound(-30, self.v_angle_x * -1, 30);
	torsoAngles_y = self.v_angle_y - playerAngles_y;
	// update head (relative to torso)
	headAngles_x = bound(-60, self.v_angle_x * -1, 60) - torsoAngles_x;

	PlayerModel_SetAngles (self, playerAngles, torsoAngles, headAngles);

	PlayerModel_AnimationThink( self );
	self.nextthink = time + ANIM_INTERVAL;
}

void Player_UpdateReal (float isnew)
{
	local vector lOrigin;
	//local vector lAngles;
	local float playerFlags;

	playerFlags = readbyte();

	lOrigin_x = readcoord();
	lOrigin_y = readcoord();
	lOrigin_z = readcoord();

	if (playerFlags & PLAYERUPD_LOCAL) {
		localPlayer = self;
		net_pmove_org = lOrigin;
	}

	self.velocity_x = readcoord();
	self.velocity_y = readcoord();
	self.velocity_z = readcoord();

	self.v_angle_x = readangle();
	self.v_angle_y = readangle();
	self.v_angle_z = readangle();

	if (isnew)
	{
		PlayerModel_Init (self, playerFlags & PLAYERUPD_LOCAL);
	}

	if (playerFlags & PLAYERUPD_NEWMODEL)
	{
		string legsModel, torsoModel, headModel;
		float legsSkin, torsoSkin, headSkin;
		legsModel = readstring();
		legsSkin = readbyte();
		torsoModel = readstring();
		torsoSkin = readbyte();
		headModel = readstring();
		headSkin = readbyte();
		// @@@ Bit of abuse: see sending
		self.clientNum = readbyte() + 1;
		
		PlayerModel_SetModel (self,
		  legsModel, legsSkin,
		  torsoModel, torsoSkin,
		  headModel, headSkin);

		PlayerModel_StartAnimation(self, ANIM_LEGS_IDLE,
		  Player_OnFrameChange_Legs, Player_OnFrameChange_Torso);
		PlayerModel_StartAnimation(self, ANIM_TORSO_STAND,
		  Player_OnFrameChange_Legs, Player_OnFrameChange_Torso);
	}

	PlayerModel_SetPosition (self, lOrigin);
	self.think = Player_Think;
	self.nextthink = time;
}

void Player_Update ()
{
	Player_UpdateReal (false);
}

void Player_Remove ()
{
	PlayerModel_Destroy (self);
	remove (self);
}

void Player_SpawnFromMessage ()
{
	self.UpdateEntity = Player_Update;
	self.RemoveEvent = Player_Remove;
	Player_UpdateReal (true);
}

entity Player_EntityForClientNum (float clientnum)
{
	return findfloat (world, clientNum, clientnum + 1);
}

void Player_DoAction (entity player, float action)
{
	PlayerModel_StartAnimation(player, action, Player_OnFrameChange_Legs, Player_OnFrameChange_Torso);
}
#endif

#ifdef SERVER
string string_null;
#define SAFE_UNZONE(var) do { if(var) { strunzone(var); var = string_null; } } while( false )
#define SAFE_SET(var,value) do { SAFE_UNZONE( var ); var = strzone( value ); } while( false )

#define MAXPLAYERS	16
#define NUMSUBMODELS 3
.string playermodelCache[MAXPLAYERS];
.string subModels[NUMSUBMODELS];
.float subSkins[NUMSUBMODELS];

float( entity viewer ) Player_Send = {
	local float playerFlags;

	playerFlags = 0;
	if (self == viewer) playerFlags |= PLAYERUPD_LOCAL;

	if (self.playermodel != viewer.(playermodelCache[self.clientNum]))
	{
		playerFlags |= PLAYERUPD_NEWMODEL;
		//print( "Viewer: ", ftos( viewer.clientNum ), " Other: ", ftos( viewer.clientNum ), " - new model!\n" );
	}

	WriteByte( MSG_ENTITY, ET_PLAYER );
	WriteByte( MSG_ENTITY, playerFlags );

	WriteCoord( MSG_ENTITY, self.origin_x );
	WriteCoord( MSG_ENTITY, self.origin_y );
	WriteCoord( MSG_ENTITY, self.origin_z );

	WriteCoord( MSG_ENTITY, self.velocity_x );
	WriteCoord( MSG_ENTITY, self.velocity_y );
	WriteCoord( MSG_ENTITY, self.velocity_z );

	WriteAngle( MSG_ENTITY, self.v_angle_x );
	WriteAngle( MSG_ENTITY, self.v_angle_y );
	WriteAngle( MSG_ENTITY, self.v_angle_z );

	if (playerFlags & PLAYERUPD_NEWMODEL)
	{
		float i;
		SAFE_SET( viewer.(playermodelCache[self.clientNum]), strzone( self.playermodel ) );

		for( i = 0 ; i < NUMSUBMODELS ; ++i ) {
			WriteString (MSG_ENTITY, self.(subModels[i]) );
			WriteByte( MSG_ENTITY, self.(subSkins[i]) );
		}

		// @@@ Bit of abuse: make sure client num is sent to every other client
		WriteByte (MSG_ENTITY, self.clientNum);
	}
	return true;
}

float usedClientNums[MAXPLAYERS];

void Player_InitModel (entity player)
{
	local float i;
	// FIXME: The server keeps a "clientnum" anyway, find out if that can be get
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if (!usedClientNums[i])
		{
			player.clientNum = i;
			usedClientNums[i] = true;
			break;
		}
	}

	local entity allplayers;
	allplayers = findchain (classname, "player");
	while (allplayers)
	{
		SAFE_SET( allplayers.(playermodelCache[self.clientNum]), "" );
		allplayers = allplayers.chain;
	}

	Player_UpdateModel( player );
}

void Player_FinishModel (entity player)
{
	float i;
	for( i = 0 ; i < NUMSUBMODELS ; i++ ) {
		SAFE_UNZONE( player.(subModels[i]) );
	}
	for( i = 0 ; i < MAXPLAYERS ; i++ ) {
		SAFE_UNZONE( player.(playermodelCache[i]) );
	}
	usedClientNums[player.clientNum] = false;
}

void precache_player_model(entity e)
{
	precache_model(strcat(PLAYERMODEL_PATH_PREFIX, e.(subModels[0]), PLAYERMODEL_LEGS_SUFFIX));
	precache_model(strcat(PLAYERMODEL_PATH_PREFIX, e.(subModels[1]), PLAYERMODEL_TORSO_SUFFIX));
	precache_model(strcat(PLAYERMODEL_PATH_PREFIX, e.(subModels[2]), PLAYERMODEL_HEAD_SUFFIX));
}

void Player_UpdateModel (entity me)
{
	float i;
	if( tokenize( me.playermodel ) == 6 ) {
		// all is well
		for( i = 0 ; i < NUMSUBMODELS ; i++ ) {
			SAFE_SET( me.(subModels[i]), argv( 2 * i ) );
			me.(subSkins[i]) = stof( argv( 2 * i + 1 ) );
			//print( me.(currentModel[i]), "; ", ftos( me.(currentSkin[i]) ), "\n" );
		}
	} else {
		for( i = 0 ; i < NUMSUBMODELS ; i++ ) {
			SAFE_SET( me.(subModels[i]), "Crow" );
			me.(subSkins[ i ]) = 0;
		}
	}
	precache_player_model(me);
}

void Player_Taunts (entity player)
{
	//print( "Player taunts\n" );
	WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
	WriteByte (MSG_BROADCAST, TE_PLAYER_ACTION);
	WriteByte (MSG_BROADCAST, player.clientNum);
	WriteByte (MSG_BROADCAST, ANIM_TORSO_GESTURE);
}
#undef SAFE_UNZONE
#endif
