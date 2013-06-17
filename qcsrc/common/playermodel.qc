//
// using _ prefix for static functions

void PlayerModel_Init (entity player, float isLocalPlayer)
{
// temp code to allow for a quick 3rd person view
	isLocalPlayer = false;

	player.drawmask = MASK_NORMAL;
	player.renderflags = isLocalPlayer ? RF_EXTERNALMODEL : 0;
	player.scale = PLAYERMODEL_SCALE;
	
	player.torso = spawn();
	player.torso.drawmask = MASK_NORMAL;
	player.torso.renderflags = isLocalPlayer ? RF_EXTERNALMODEL : 0;
	player.torso.tag_entity = player;

	player.head = spawn();
	player.head.drawmask = MASK_NORMAL;
	player.head.renderflags = isLocalPlayer ? RF_EXTERNALMODEL : 0;
	player.head.tag_entity = player.torso;

	player.legs = player;
}

void PlayerModel_SetModel (entity player, string legsModel, float legsSkin, string torsoModel, float torsoSkin, string headModel, float headSkin)
{
	local string fullname;
	
	fullname = PlayerModel_GetSubModelPath( legsModel, PM_LEGS );
	precache_model(fullname);
	setmodel(player.legs, fullname);
#ifndef MENU
	setsize( player.legs, player.legs.mins * PLAYERMODEL_SCALE, player.legs.maxs * PLAYERMODEL_SCALE );
#endif
	player.legs.skin = legsSkin;

	fullname = PlayerModel_GetSubModelPath( torsoModel, PM_TORSO );
	precache_model(fullname);
	setmodel(player.torso, fullname);
	player.torso.skin = torsoSkin;

	fullname = PlayerModel_GetSubModelPath( headModel, PM_HEAD );
	precache_model(fullname);
	setmodel(player.head, fullname);
	player.head.skin = headSkin;

	player.torso.tag_index = gettagindex (player.torso.tag_entity, "tag_torso");
	player.head.tag_index = gettagindex (player.head.tag_entity, "tag_head");
	
	PlayerModel_LoadAnimationCfg(player.legs, strcat (PLAYERMODEL_PATH_PREFIX, legsModel));
	PlayerModel_LoadAnimationCfg(player.torso, strcat (PLAYERMODEL_PATH_PREFIX, torsoModel));
	// not needed at the moment
	//PlayerModel_LoadAnimationCfg(player.head, strcat (PLAYERMODEL_PATH_PREFIX, headModel));

	// reset the animations
	vector lAnimation;
	// TODO: add a function for this - Black
	entity lTemp;
	lTemp = player.legs;
	lAnimation = lTemp.(animData[ ANIM_LEGS_IDLE ]);
	player.legs.frame = lAnimation_x;
	
	lTemp = player.torso;
	lAnimation = lTemp.(animData[ ANIM_TORSO_STAND ]);
	player.torso.frame = lAnimation_x;
	
	vector lHeadOffset;
	lHeadOffset = gettaginfo( player.head, gettagindex( player.head, "tag_head" ) ) - player.legs.origin;
	lHeadOffset_z = lHeadOffset_z + player.head.size_z * 0.5;
	player.viewOffset = lHeadOffset;
}

void PlayerModel_SetPosition (entity player, vector org)
{
	setorigin( player, org );
}

void PlayerModel_SetAngles (entity player, vector playerAngles, vector torsoAngles, vector headAngles )
{
	player.angles = playerAngles;
	player.torso.angles = torsoAngles;
	player.head.angles = headAngles;
}

void PlayerModel_Destroy(entity player)
{
	remove( player.head );
	remove( player.torso );
}

// actually the animation faff belongs into its own file since legs and torso both need to be animated differently :-/
void _PlayerModel_AnimateEntity(entity animated, entity player)
{
	animated.currentAnimAccumulation = animated.currentAnimAccumulation + frametime;
	float lJumpFrames;
	lJumpFrames = floor( animated.currentAnimAccumulation / animated.currentAnimData_z );
	//print( vtos( animated.currentAnimData ), ftos( animated.currentAnimAccumulation ), " ", ftos( animated.frame ), "\n" );
	animated.currentAnimAccumulation = animated.currentAnimAccumulation - lJumpFrames * animated.currentAnimData_z;
	animated.frame = animated.frame + lJumpFrames;
	// we could loop through all frames instead if we had functions that were based on frame-specific actions
	if( animated.frame >= animated.currentAnimData_y ) {
		// animation done, what now?
		// call the call back
		if( animated.currentAnimOnFrameChange ) {
			animated.currentAnimOnFrameChange( player, true );
		}
	} else {
		if( animated.currentAnimOnFrameChange ) {
			animated.currentAnimOnFrameChange( player, false );
		}
	}

	//player.nextthink = time + ANIM_INTERVAL;
}

void PlayerModel_AnimationThink(entity player)
{
	_PlayerModel_AnimateEntity (player.legs, player);
	_PlayerModel_AnimateEntity (player.torso, player);
}

#define ANIM_TORSO	1
#define ANIM_LEGS	2

float get_animation_flag(float animNum)
{
  switch (animNum)
  {
  case ANIM_BOTH_DEATH1:
  case ANIM_BOTH_DEAD1:
  case ANIM_BOTH_DEATH2:
  case ANIM_BOTH_DEAD2:
  case ANIM_BOTH_DEATH3:
  case ANIM_BOTH_DEAD3:
	return ANIM_TORSO | ANIM_LEGS;
  case ANIM_TORSO_GESTURE:
  case ANIM_TORSO_ATTACK:
  case ANIM_TORSO_ATTACK2:
  case ANIM_TORSO_DROP:
  case ANIM_TORSO_RAISE:
  case ANIM_TORSO_STAND:
  case ANIM_TORSO_STAND2:
	return ANIM_TORSO;
  case ANIM_LEGS_WALKCR:
  case ANIM_LEGS_WALK:
  case ANIM_LEGS_RUN:
  case ANIM_LEGS_BACK:
  case ANIM_LEGS_SWIM:
  case ANIM_LEGS_JUMP:
  case ANIM_LEGS_LAND:
  case ANIM_LEGS_JUMPB:
  case ANIM_LEGS_LANDB:
  case ANIM_LEGS_IDLE:
  case ANIM_LEGS_IDLECR:
  case ANIM_LEGS_TURN:
	return ANIM_LEGS;
  }
  return 0;
}

void _PlayerModel_SetEntityAnim (entity ent, entity animDataHolder, float anim,
				 AnimOnFrameChange onFrameChange ) {
		ent.currentAnim = anim;
		ent.currentAnimData = animDataHolder.(animData[ anim ]);
		ent.currentAnimAccumulation = 0.0;
		ent.currentAnimOnFrameChange = onFrameChange;
		ent.frame = ent.currentAnimData_x;
}

void PlayerModel_StartAnimation(entity player, float anim,
				AnimOnFrameChange onFrameChange_Legs,
				AnimOnFrameChange onFrameChange_Torso ) {
	local float animFlag;

	animFlag = get_animation_flag (anim);
	// TODO: check for currentAnim == anim, etc.
	if (animFlag & ANIM_LEGS)
	{
		_PlayerModel_SetEntityAnim (player.legs, player.legs, anim,
			onFrameChange_Legs);
	}
	if (animFlag & ANIM_TORSO)
	{
		_PlayerModel_SetEntityAnim (player.torso, player.torso, anim,
			onFrameChange_Torso);
	}
}

// TODO: move this to some util.qc BlackHC
float isFloat( string t ) {
	return stof( t ) != 0 || substring( t, 0, 1 ) == "0";
}

void PlayerModel_LoadAnimationCfg(entity playerPart, string modelDir) {
	float lFileHandle;
	lFileHandle = fopen( strcat( modelDir, "/animation.cfg" ), FILE_READ );
	if (lFileHandle < 0) return;

	string lFileContent;
	lFileContent = "";
	string lLine;
	while(1) {
		lLine = fgets( lFileHandle );
		if not( lLine ) {
			break;
		}
		/* Ignore lines that don't start with a float value
		   (Directives such as headoffset, sex) */
		tokenize( lLine );
		if (!isFloat (argv (0))) continue;
		lFileContent = strcat( lFileContent, "\n", lLine );
	}

	fclose( lFileHandle );

	float lNumTokens;
	lNumTokens = tokenize( lFileContent );

	if( lNumTokens / 4 < ANIM_COUNT ) {
		print( "Bad animfile '", modelDir, "/animation.cfg'\n" );
		return;
	}

	float lTokenIndex;
	float lAnimIndex;
	float lSkip;
	lTokenIndex = 0;
	lSkip = 0;
	for( lAnimIndex = 0 ; lAnimIndex < ANIM_COUNT ; ++lAnimIndex ) {
		vector lAnimData;
		// first frame
		lAnimData_x = stof( argv( lTokenIndex ) );
		if( lAnimIndex == ANIM_LEGS_WALKCR ) {
			vector lGestureAnimData;
			lGestureAnimData = playerPart.(animData[ ANIM_TORSO_GESTURE ]);
			lSkip = lAnimData_x - lGestureAnimData_x;
		}
		lAnimData_x = lAnimData_x - lSkip;

		lTokenIndex = lTokenIndex + 1;
		// end frame
		lAnimData_y = stof( argv( lTokenIndex ) ) + lAnimData_x;
		// loop frames (ignored)
		lTokenIndex = lTokenIndex + 2;
		// fps (store as spf directly)
		lAnimData_z = 1.0 / stof( argv( lTokenIndex ) );
		lTokenIndex = lTokenIndex + 1;

		playerPart.(animData[ lAnimIndex ]) = lAnimData;
		//print( vtos( lAnimData ), "\n" );
	}
	dprint( "Loaded animfile '", modelDir, "/animation.cfg'\n" );
}

string( float type ) _PlayerModel_GetModelSuffix = {
	switch( type ) {
		case PM_LEGS:
			return PLAYERMODEL_LEGS_SUFFIX;
		case PM_TORSO:
			return PLAYERMODEL_TORSO_SUFFIX;
		case PM_HEAD:
			return PLAYERMODEL_HEAD_SUFFIX;
	}
}

string( string pModel, float pType ) PlayerModel_GetSubModelPath = {
	//print( "PlayerModel_GetSubModelPath:", pModel, " ", ftos( pType ), "\n" );
	return strcat( PLAYERMODEL_PATH_PREFIX, pModel, _PlayerModel_GetModelSuffix( pType ) );
};

string( string path, float type ) PlayerModel_GetModelFromPath = {
	// TODO: this could be solved by using constants
	return substring( path, PLAYERMODEL_PATH_PREFIX_LENGTH, strlen( path ) - PLAYERMODEL_PATH_PREFIX_LENGTH - strlen( _PlayerModel_GetModelSuffix( type ) ) );
};
