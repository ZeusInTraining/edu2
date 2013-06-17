#ifdef INTERFACE
CLASS(NexuizPlayerModelSelector) EXTENDS(NexuizImage)
	METHOD(NexuizPlayerModelSelector, configureNexuizPlayerModelSelector, void(entity))
	METHOD(NexuizPlayerModelSelector, loadCvars, void(entity))
	METHOD(NexuizPlayerModelSelector, saveCvars, void(entity))
	METHOD(NexuizPlayerModelSelector, draw, void(entity))
	METHOD(NexuizPlayerModelSelector, resizeNotify, void(entity, vector, vector, vector, vector))
	ATTRIBARRAY(NexuizPlayerModelSelector, currentModel, string, 3 /* PM_NUM_PARTS */)
	ATTRIBARRAY(NexuizPlayerModelSelector, currentSkin, float, 3 /* PM_NUM_PARTS */)
	ATTRIB(NexuizPlayerModelSelector, currentModelTitle, string, string_null)
	ATTRIB(NexuizPlayerModelSelector, currentModelTxtName, string, string_null)
	ATTRIB(NexuizPlayerModelSelector, currentModelDescription, string, string_null)
	ATTRIB(NexuizPlayerModelSelector, cvarName, string, "_cl_playermodel")
	METHOD(NexuizPlayerModelSelector, go, void(entity, float, float))
	ATTRIB(NexuizPlayerModelSelector, origin, vector, '0 0 0')
	ATTRIB(NexuizPlayerModelSelector, size, vector, '0 0 0')
	ATTRIB(NexuizPlayerModelSelector, realFontSize, vector, '0 0 0')
	ATTRIB(NexuizPlayerModelSelector, fontSize, float, SKINFONTSIZE_NORMAL)
	ATTRIB(NexuizPlayerModelSelector, titleFontSize, float, SKINFONTSIZE_TITLE)
	ATTRIB(NexuizPlayerModelSelector, playerModel, entity, NULL)
	METHOD(NexuizPlayerModelSelector, updatePlayerModel, void(entity))
ENDCLASS(NexuizPlayerModelSelector)
entity makeNexuizPlayerModelSelector();
void PlayerModelSelector_Next_Legs_Click(entity btn, entity me);
void PlayerModelSelector_Prev_Legs_Click(entity btn, entity me);
void PlayerModelSelector_Next_Torso_Click(entity btn, entity me);
void PlayerModelSelector_Prev_Torso_Click(entity btn, entity me);
void PlayerModelSelector_Next_Head_Click(entity btn, entity me);
void PlayerModelSelector_Prev_Head_Click(entity btn, entity me);
#endif

#ifdef IMPLEMENTATION
#define SAFE_UNZONE(var) if(var) strunzone(var)

entity makeNexuizPlayerModelSelector()
{
	entity me;
	me = spawnNexuizPlayerModelSelector();
	me.configureNexuizPlayerModelSelector(me);
	return me;
}

void configureNexuizPlayerModelSelectorNexuizPlayerModelSelector(entity me)
{
	me.configureNexuizImage(me, string_null, 263.0/360.0);
	me.loadCvars(me);

	me.playerModel = spawn();
	PlayerModel_Init( me.playerModel, false );
	PlayerModel_SetAngles( me.playerModel, '0 180 0', '0 0 0', '0 0 0' );
	me.updatePlayerModel( me );
}

void loadCvarsNexuizPlayerModelSelector(entity me)
{
	string models;
	// format: legs skin# torso skin# head skin#
	models = cvar_string( "_cl_playermodel" );
	float i;
	if( tokenize( models ) == 6 ) {
		// all is well
		for( i = 0 ; i < PM_NUM_PARTS ; i++ ) {
			SAFE_UNZONE( me.(currentModel[i]) );
			me.(currentModel[i]) = strzone( argv( 2 * i ) );
			me.(currentSkin[i]) = stof( argv( 2 * i + 1 ) );
			//print( me.(currentModel[i]), "; ", ftos( me.(currentSkin[i]) ), "\n" );
		}
	} else {
		for( i = 0 ; i < PM_NUM_PARTS ; i++ ) {
			SAFE_UNZONE( me.(currentModel[i]) );
			me.(currentModel[ i ]) = strzone( "Crow" );
			me.(currentSkin[ i ]) = 0;
		}
	}

	// TODO: check if all models exist
}

string( string model, float type, float skin ) NexuizPlayerModelSelector_BuildSkinPath = {
	return strcat( PlayerModel_GetSubModelPath( model, type ), "_", ftos( rint( skin ) ), ".skin" );
};

void(entity me, float type, float direction) goNexuizPlayerModelSelector = {
	float lSearch;
	lSearch = search_begin( strcat( PlayerModel_GetSubModelPath( "*", type ), "_?*.skin" ), TRUE, TRUE);
	if(lSearch < 0)
		return;
	float lSearchSize;
	lSearchSize = search_getsize(lSearch);
	// now find the index of the current model
	float i;
	string lCurrentModel;
	lCurrentModel = NexuizPlayerModelSelector_BuildSkinPath( me.(currentModel[type]), type, me.(currentSkin[type]) );
	for(i = 0; i < lSearchSize; ++i) {
		if(search_getfilename(lSearch, i) == lCurrentModel)
			break;
	}

	if(i == lSearchSize)
	{
		if(direction < 0)
			i = lSearchSize - 1;
		else
			i = 0;
	}
	else
	{
		i = mod(i + direction + lSearchSize, lSearchSize);
	}

	string lNewModelAndSkin;
	lNewModelAndSkin = search_getfilename( lSearch, i );

	// we need to strip the _ off
	float lCount;
	lCount = tokenizebyseparator( lNewModelAndSkin, "_" );
	// assert( lCount >=2 ); // simply because we search for *_?*.skin so it needs to contain at least 2 tokens
	me.(currentSkin[type]) = stof( argv( lCount - 1 ) ); // (ignores .skin)
	SAFE_UNZONE( me.(currentModel[type]) );
	me.(currentModel[type]) = strzone( PlayerModel_GetModelFromPath( substring( lNewModelAndSkin, 0, strlen( lNewModelAndSkin ) - strlen( argv( lCount - 1 ) ) - 1 ), type ) );

	search_end(lSearch);
	me.updatePlayerModel(me);
};

void PlayerModelSelector_Next_Legs_Click(entity btn, entity me)
{
	me.go(me, PM_LEGS, +1);
	me.saveCvars(me);
}

void PlayerModelSelector_Prev_Legs_Click(entity btn, entity me)
{
	me.go(me, PM_LEGS, -1);
	me.saveCvars(me);
}

void PlayerModelSelector_Next_Torso_Click(entity btn, entity me)
{
	me.go(me, PM_TORSO, +1);
	me.saveCvars(me);
}

void PlayerModelSelector_Prev_Torso_Click(entity btn, entity me)
{
	me.go(me, PM_TORSO, -1);
	me.saveCvars(me);
}

void PlayerModelSelector_Next_Head_Click(entity btn, entity me)
{
	me.go(me, PM_HEAD, +1);
	me.saveCvars(me);
}

void PlayerModelSelector_Prev_Head_Click(entity btn, entity me)
{
	me.go(me, PM_HEAD, -1);
	me.saveCvars(me);
}

void saveCvarsNexuizPlayerModelSelector(entity me)
{
	// TODO rather set the _cl ones and apply later?
	//localcmd(strcat("playermodel ", me.(currentModel[0]), "\nplayerskin ", ftos(me.(currentSkin[0])), "\n"));
	localcmd( "playermodel \"" );
	float i;
	for( i = 0 ; i < PM_NUM_PARTS ; i++ ) {
		localcmd( me.(currentModel[i]), " ", ftos( me.(currentSkin[i]) ), " " );
	}
	localcmd( "\"\n" );
}

// for the scene model
.float alpha;

void drawNexuizPlayerModelSelector(entity me)
{
	float i, n;
	vector o;

	/*me.src = me.currentModelName;
	drawImage(me);
	me.src = string_null;*/
	
	// from player.qc
	// TODO: hack hack hack - black
	// DP's menu rendering uses the client state for caching models so we gotta precache them everyframe to make sure
	// they stay precached :(
	string lTemp;
	lTemp = strcat(PLAYERMODEL_PATH_PREFIX, me.(currentModel[PM_LEGS]), PLAYERMODEL_LEGS_SUFFIX);
	precache_model(lTemp);
	setmodel( me.playerModel.legs, lTemp );
	
	lTemp = strcat(PLAYERMODEL_PATH_PREFIX, me.(currentModel[PM_TORSO]), PLAYERMODEL_TORSO_SUFFIX);
	precache_model( lTemp );
	setmodel( me.playerModel.torso, lTemp );
	
	lTemp = strcat(PLAYERMODEL_PATH_PREFIX, me.(currentModel[PM_HEAD]), PLAYERMODEL_HEAD_SUFFIX);
	precache_model( lTemp );
	setmodel( me.playerModel.head, lTemp );
	
	R_ClearScene();
	// this requires a DP extension :)
	if( draw_alpha >= 0.5 ) {
		me.playerModel.legs.alpha = 0;
		me.playerModel.torso.alpha = 0;
		me.playerModel.head.alpha = 0;
	} else if( draw_alpha != 0.0 ) {
		me.playerModel.legs.alpha = draw_alpha;
		me.playerModel.torso.alpha = draw_alpha;
		me.playerModel.head.alpha = draw_alpha;
	} else {
		me.playerModel.legs.alpha = -1.0;
		me.playerModel.torso.alpha = -1.0;
		me.playerModel.head.alpha = -1.0;
	}
	vector lSize;
	lSize = boxToGlobalSize(me.imgSize, draw_scale);
	if( lSize_x == 0 || lSize_y == 0 ) {
		return;
	}
	
	PlayerModel_AnimationThink( me.playerModel );
	R_SetView (VF_MIN, boxToGlobal(me.imgOrigin, draw_shift, draw_scale));
	R_SetView (VF_SIZE, lSize);
	R_SetView (VF_FOVX, (me.imgSize_x / me.imgSize_y) * 90);
	R_SetView (VF_FOVY, 100);
	R_SetView (VF_CLEARSCREEN, 0 );
	R_SetView (VF_ORIGIN, '-60 0 10'); // stov( cvar_string( "vf_origin" ) ) ); //"10 0 50"
	R_SetView (VF_ANGLES, '10 0 0'); // stov( cvar_string( "vf_angles" ) ) ); //'100 0 180' );
	// this could be replaced by AddEntities but.. not sure whether its worth the processing time
	R_AddEntity (me.playerModel.legs);
	R_AddEntity (me.playerModel.head);
	R_AddEntity (me.playerModel.torso);
	R_RenderScene();
	R_ClearScene();

	// draw text on the image, handle \n in the description
	draw_CenterText('0.5 0 0', me.currentModelTitle, me.realFontSize * (me.titleFontSize / me.fontSize), SKINCOLOR_MODELTITLE, SKINALPHA_MODELTITLE, FALSE);

	o = '0.5 1 0' - eY * me.realFontSize_y * ((n = tokenizebyseparator(me.currentModelDescription, "\n")) + 0.5);
	for(i = 0; i < n; ++i)
	{
		draw_CenterText(o, argv(i), me.realFontSize, '1 1 1', 1, FALSE);
		o += eY * me.realFontSize_y;
	}
}

void resizeNotifyNexuizPlayerModelSelector(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize)
{
	resizeNotifyImage(me, relOrigin, relSize, absOrigin, absSize);
	me.origin = absOrigin;
	me.size = absSize;
	me.realFontSize_y = me.fontSize / absSize_y;
	me.realFontSize_x = me.fontSize / absSize_x;
}

void(entity player, float finished) NexuizPlayerModel_Torso_AnimOnFrameChange;
void(entity player, float finished) NexuizPlayerModel_Legs_AnimOnFrameChange;

void(entity player, float finished) NexuizPlayerModel_Torso_AnimOnFrameChange = {
	if( finished ) {
		PlayerModel_StartAnimation( player, ANIM_TORSO_STAND, NexuizPlayerModel_Legs_AnimOnFrameChange, NexuizPlayerModel_Torso_AnimOnFrameChange );
	}
};

void(entity player, float finished) NexuizPlayerModel_Legs_AnimOnFrameChange = {
	if( finished ) {
		PlayerModel_StartAnimation( player, ANIM_LEGS_IDLE, NexuizPlayerModel_Legs_AnimOnFrameChange, NexuizPlayerModel_Torso_AnimOnFrameChange );
	}
};

void updatePlayerModelNexuizPlayerModelSelector(entity me)
{
	PlayerModel_SetModel( me.playerModel,  me.(currentModel[PM_LEGS]), me.(currentSkin[PM_LEGS]),  me.(currentModel[PM_TORSO]), me.(currentSkin[PM_TORSO]),  me.(currentModel[PM_HEAD]), me.(currentSkin[PM_HEAD]) );

	PlayerModel_StartAnimation(me.playerModel, ANIM_LEGS_IDLE,
		NexuizPlayerModel_Legs_AnimOnFrameChange, NexuizPlayerModel_Torso_AnimOnFrameChange);
	PlayerModel_StartAnimation(me.playerModel, ANIM_TORSO_STAND,
		NexuizPlayerModel_Legs_AnimOnFrameChange, NexuizPlayerModel_Torso_AnimOnFrameChange);
}

#undef SAFE_UNZONE
#endif
