	// internal toy
void cvar_settemp(string pKey, string pValue)
{
	//localcmd(strcat("\nsettemp ", t, " \"", s, "\"\n"));
	
	// duplicate what this alias does:
	// alias settemp "settemp_list \"1 $1 $settemp_var $settemp_list\"; set $settemp_var \"${$1}\"; settemp_var ${settemp_var}x; $1 \"$2\""
	
	cvar_set("settemp_list", strcat("1 ", pKey, " ", cvar_string("settemp_var"), " ", cvar_string("settemp_list")));
#ifdef MENUQC
	registercvar(cvar_string("settemp_var"), "", 0);
#else
	registercvar(cvar_string("settemp_var"), "");
#endif
	cvar_set(cvar_string("settemp_var"), cvar_string(pKey));
	cvar_set("settemp_var", strcat(cvar_string("settemp_var"), "x"));
	cvar_set(pKey, pValue);
}

void cvar_settemp_restore()
{
	// undo what cvar_settemp did
	float n, i;
	n = tokenize(cvar_string("settemp_list"));
	for(i = 0; i < n - 3; i += 3)
		cvar_set(argv(i + 1), cvar_string(argv(i + 2)));
	cvar_set("settemp_list", "0");
}

#ifdef HSOI
// HUGE SET - stored in a string
string HugeSetOfIntegers_empty()
{
	return "";
}
float HugeSetOfIntegers_get(string pArr, float i)
{
	return stof(substring(pArr, i * 4, 4));
}
float HugeSetOfIntegers_length(string pArr)
{
	return strlen(pArr) / 4;
}
string HugeSetOfIntegers_concat(string a1, string a2)
{
	return strcat(a1, a2);
}
string HugeSetOfIntegers_insert(string a1, float n, string a2)
	// special concat function to build up large lists in less time by binary concatenation
{
	string s;
	s = strcat("    ", ftos(n));
	return strcat(a1, substring(s, strlen(s) - 4, 4), a2);
}
#endif

// generic string stuff
float startsWith(string haystack, string needle)
{
	return substring(haystack, 0, strlen(needle)) == needle;
}
float startsWithNocase(string haystack, string needle)
{
	return strcasecmp(substring(haystack, 0, strlen(needle)), needle) == 0;
}
string extractRestOfLine(string haystack, string needle)
{
	if(startsWith(haystack, needle))
		return substring(haystack, strlen(needle), strlen(haystack) - strlen(needle));
	return string_null;
}
string car(string s)
{
	float o;
	o = strstrofs(s, " ", 0);
	if(o < 0)
		return s;
	return substring(s, 0, o);
}
string cdr(string s)
{
	float o;
	o = strstrofs(s, " ", 0);
	if(o < 0)
		return string_null;
	return substring(s, o + 1, strlen(s) - (o + 1));
}

float _MapInfo_Cache_Active;
float _MapInfo_Cache_DB_NameToIndex;
float _MapInfo_Cache_Buf_IndexToMapData;

void MapInfo_Cache_Destroy()
{
	if(!_MapInfo_Cache_Active)
		return;

	db_close(_MapInfo_Cache_DB_NameToIndex);
	buf_del(_MapInfo_Cache_Buf_IndexToMapData);
	_MapInfo_Cache_Active = 0;
}

void MapInfo_Cache_Create()
{
	MapInfo_Cache_Destroy();
	_MapInfo_Cache_DB_NameToIndex = db_create();
	_MapInfo_Cache_Buf_IndexToMapData = buf_create();
	_MapInfo_Cache_Active = 1;
}

void MapInfo_Cache_Invalidate()
{
	if(!_MapInfo_Cache_Active)
		return;

	MapInfo_Cache_Create();
}

void MapInfo_Cache_Store()
{
	float i;
	string s;
	if(!_MapInfo_Cache_Active)
		return;

	s = db_get(_MapInfo_Cache_DB_NameToIndex, MapInfo_Map_bspname);
	if(!s) // empty string is NOT valid here!
	{
		i = buf_getsize(_MapInfo_Cache_Buf_IndexToMapData);
		db_put(_MapInfo_Cache_DB_NameToIndex, MapInfo_Map_bspname, ftos(i));
	}
	else
		i = stof(s);

	// now store all the stuff
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, MapInfo_Map_bspname);
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, MapInfo_Map_title);
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, MapInfo_Map_description);
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, MapInfo_Map_author);
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, ftos(MapInfo_Map_supportedGametypes));
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, ftos(MapInfo_Map_supportedFeatures));
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, ftos(MapInfo_Map_diameter));
	bufstr_set(_MapInfo_Cache_Buf_IndexToMapData, i++, ftos(MapInfo_Map_spawnpoints));
}

float MapInfo_Cache_Retrieve(string map)
{
	float i;
	string s;
	if(!_MapInfo_Cache_Active)
		return 0;

	s = db_get(_MapInfo_Cache_DB_NameToIndex, map);
	if(!s)
		return 0;
	i = stof(s);

	// now retrieve all the stuff
	MapInfo_Map_bspname = bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++);
	MapInfo_Map_title = bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++);
	MapInfo_Map_description = bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++);
	MapInfo_Map_author = bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++);
	MapInfo_Map_supportedGametypes = stof(bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++));
	MapInfo_Map_supportedFeatures = stof(bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++));
	MapInfo_Map_diameter = stof(bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++));
	MapInfo_Map_spawnpoints = stof(bufstr_get(_MapInfo_Cache_Buf_IndexToMapData, i++));
	return 1;
}

// GLOB HANDLING (for all BSP files)
float _MapInfo_globopen;
float _MapInfo_globcount; 
float _MapInfo_globhandle;
string _MapInfo_GlobItem(float i)
{
	string s;
	s = search_getfilename(_MapInfo_globhandle, i);
	return substring(s, 5, strlen(s) - 9); // without maps/ and .bsp
}

void MapInfo_Enumerate()
{
	if(_MapInfo_globopen)
		search_end(_MapInfo_globhandle);
	MapInfo_Cache_Invalidate();
	_MapInfo_globhandle = search_begin("maps/*.bsp", TRUE, TRUE);
	_MapInfo_globcount = search_getsize(_MapInfo_globhandle);
	_MapInfo_globopen = 1;
}

// filter the info by game type mask (updates MapInfo_count)
//
#ifdef HSOI
string _MapInfo_filtered;
float MapInfo_FilterList_Lookup(float i)
{
	return MapInfo_FilterList_Lookup(i);
}

string MapInfo_FilterGametype_Recursive(float pGametype, float pFeatures, float pBegin, float pEnd, float pAbortOnGenerate)
{
	float m, valid;
	string l, r;

	if(pBegin == pEnd)
		return HugeSetOfIntegers_empty();

	m = floor((pBegin + pEnd) / 2);

	l = MapInfo_FilterGametype_Recursive(pGametype, pFeatures, pBegin, m, pAbortOnGenerate);
	if not(l)
		return string_null; // BAIL OUT
	if(MapInfo_Get_ByName(_MapInfo_GlobItem(m), 1, 0) == 2) // if we generated one... BAIL OUT and let the caller continue in the next frame.
		if(pAbortOnGenerate)
		{
			MapInfo_progress = m / _MapInfo_globcount;
			return string_null; // BAIL OUT
		}
	valid = (((MapInfo_Map_supportedGametypes & pGametype) != 0) && ((MapInfo_Map_supportedFeatures & pFeatures) == pFeatures));
	r = MapInfo_FilterGametype_Recursive(pGametype, pFeatures, m + 1, pEnd, pAbortOnGenerate);
	if not(r)
		return string_null; // BAIL OUT

	if(valid)
		return HugeSetOfIntegers_insert(l, m, r);
	else
		return HugeSetOfIntegers_concat(l, r);
}

float MapInfo_FilterGametype(float pGametype, float pFeatures, float pAbortOnGenerate)
{
	if(_MapInfo_filtered)
		strunzone(_MapInfo_filtered);
	_MapInfo_filtered = MapInfo_FilterGametype_Recursive(pGametype, pFeatures, 0, _MapInfo_globcount, pAbortOnGenerate);
	if not(_MapInfo_filtered)
	{
		dprint("Autogenerated a .mapinfo, doing the rest later.\n");
		return 0;
	}
	_MapInfo_filtered = strzone(_MapInfo_filtered);
	MapInfo_count = HugeSetOfIntegers_length(_MapInfo_filtered);
	MapInfo_ClearTemps();
	return 1;
}
#else
float _MapInfo_filtered;
float _MapInfo_filtered_allocated;
float MapInfo_FilterList_Lookup(float i)
{
	return stof(bufstr_get(_MapInfo_filtered, i));
}

float MapInfo_FilterGametype(float pGametype, float pFeatures, float pAbortOnGenerate)
{
	float i, j;
	if not(_MapInfo_filtered_allocated)
	{
		_MapInfo_filtered_allocated = 1;
		_MapInfo_filtered = buf_create();
	}
	MapInfo_count = 0;
	for(i = 0, j = 0; i < _MapInfo_globcount; ++i)
	{
		if(MapInfo_Get_ByName(_MapInfo_GlobItem(i), 1, 0) == 2) // if we generated one... BAIL OUT and let the caller continue in the next frame.
			if(pAbortOnGenerate)
			{
				dprint("Autogenerated a .mapinfo, doing the rest later.\n");
				MapInfo_progress = i / _MapInfo_globcount;
				return 0;
			}
		if(((MapInfo_Map_supportedGametypes & pGametype) != 0) && ((MapInfo_Map_supportedFeatures & pFeatures) == pFeatures))
			bufstr_set(_MapInfo_filtered, j++, ftos(i));
	}
	MapInfo_count = j;
	return 1;
}
#endif

// load info about the i-th map into the MapInfo_Map_* globals
string MapInfo_BSPName_ByID(float i)
{
	return _MapInfo_GlobItem(MapInfo_FilterList_Lookup(i));
}

string unquote(string s)
{
	float i, j, l;
	l = strlen(s);
	j = -1;
	for(i = 0; i < l; ++i)
	{
		string ch;
		ch = substring(s, i, 1);
		if(ch != " ") if(ch != "\"")
		{
			for(j = strlen(s) - i - 1; j > 0; --j)
			{
				ch = substring(s, i+j, 1);
				if(ch != " ") if(ch != "\"")
					return substring(s, i, j+1);
			}
			return substring(s, i, 1);
		}
	}
	return "";
}

float MapInfo_Get_ByID(float i)
{
	if(MapInfo_Get_ByName(MapInfo_BSPName_ByID(i), 0, 0))
		return 1;
	return 0;
}

float _MapInfo_Generate(string pFilename) // 0: failure, 1: ok ent, 2: ok bsp
{
	string fn;
	float fh;
	string s, k, v;
	vector o;
	float i;
	float inWorldspawn;
	float r;
	float twoBaseModes;

	vector mapMins, mapMaxs;

	r = 1;
	fn = strcat("maps/", pFilename, ".ent");
	fh = fopen(fn, FILE_READ);
	if(fh < 0)
	{
		r = 2;
		fn = strcat("maps/", pFilename, ".bsp");
		fh = fopen(fn, FILE_READ);
	}
	if(fh < 0)
		return 0;
	print("Analyzing ", fn, " to generate initial mapinfo; please edit that file later\n");

	inWorldspawn = 2;
	MapInfo_Map_supportedGametypes = 0;

	for(;;)
	{
		if not((s = fgets(fh)))
			break;
		if(inWorldspawn == 1)
			if(startsWith(s, "}"))
				inWorldspawn = 0;
		k = unquote(car(s));
		v = unquote(cdr(s));
		if(inWorldspawn)
		{
			if(k == "classname" && v == "worldspawn")
				inWorldspawn = 1;
			else if(k == "author")
				MapInfo_Map_author = v;
			else if(k == "_description")
				MapInfo_Map_description = v;
			else if(k == "message")
			{
				i = strstrofs(v, " by ", 0);
				if(MapInfo_Map_author == "<AUTHOR>" && i >= 0)
				{
					MapInfo_Map_title = substring(v, 0, i);
					MapInfo_Map_author = substring(v, i + 4, strlen(v) - (i + 4));
				}
				else
					MapInfo_Map_title = v;
			}
		}
		else
		{
			if(k == "origin")
			{
				o = stov(strcat("'", v, "'"));
				mapMins_x = min(mapMins_x, o_x);
				mapMins_y = min(mapMins_y, o_y);
				mapMins_z = min(mapMins_z, o_z);
				mapMaxs_x = max(mapMaxs_x, o_x);
				mapMaxs_y = max(mapMaxs_y, o_y);
				mapMaxs_z = max(mapMaxs_z, o_z);
			}
			else if(k == "classname")
			{
				if(v == "dom_controlpoint")
					MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_DOMINATION;
				else if(v == "item_flag_team2")
					MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_CTF;
				else if(v == "team_CTF_blueflag")
					MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_CTF;
				else if(v == "runematch_spawn_point")
					MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_RUNEMATCH;
				else if(v == "target_assault_roundend")
					MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_ASSAULT;
				else if(v == "onslaught_generator")
					MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_ONSLAUGHT;
				else if(v == "info_player_team1")
					++MapInfo_Map_spawnpoints;
				else if(v == "info_player_team2")
					++MapInfo_Map_spawnpoints;
				else if(v == "info_player_start")
					++MapInfo_Map_spawnpoints;
				else if(v == "info_player_deathmatch")
					++MapInfo_Map_spawnpoints;
				else if(v == "weapon_nex")
					{ }
				else if(v == "weapon_railgun")
					{ }
				else if(startsWith(v, "weapon_"))
					MapInfo_Map_supportedFeatures |= MAPINFO_FEATURE_WEAPONS;
			}
		}
	}
	if(inWorldspawn)
	{
		print(fn, " ended still in worldspawn, BUG\n");
		return 0;
	}
	MapInfo_Map_diameter = vlen(mapMaxs - mapMins);

	twoBaseModes = MapInfo_Map_supportedGametypes & (MAPINFO_TYPE_CTF | MAPINFO_TYPE_ASSAULT);
	if(twoBaseModes && (MapInfo_Map_supportedGametypes == twoBaseModes))
	{
		// we have a CTF-only or Assault-only map. Don't add other modes then,
		// as the map is too symmetric for them.
	}
	else
	{
		MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_DEATHMATCH;      // DM always works
		MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_LMS;             // LMS always works

		if(MapInfo_Map_spawnpoints >= 8  && MapInfo_Map_diameter > 4096)
			MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_TEAM_DEATHMATCH;
		if(                MapInfo_Map_diameter < 4096)
			MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_ARENA;
		if(MapInfo_Map_spawnpoints >= 12 && MapInfo_Map_diameter > 5120)
			MapInfo_Map_supportedGametypes |= MAPINFO_TYPE_KEYHUNT;
	}

	fclose(fh);

	return r;
}

void _MapInfo_Map_Reset()
{
	MapInfo_Map_title = "<TITLE>";
	MapInfo_Map_description = "<DESCRIPTION>";
	MapInfo_Map_author = "<AUTHOR>";
	MapInfo_Map_supportedGametypes = 0;
	MapInfo_Map_supportedFeatures = 0;
	MapInfo_Map_diameter = 0;
	MapInfo_Map_spawnpoints = 0;
}

void _MapInfo_Map_ApplyGametype(string s, float pWantedType, float pThisType)
{
	MapInfo_Map_supportedGametypes |= pThisType;
	if(!(pThisType & pWantedType))
		return;
	
	cvar_set("fraglimit", car(s));
	s = cdr(s);

	cvar_set("timelimit", car(s));
	s = cdr(s);

	if(pWantedType == MAPINFO_TYPE_TEAM_DEATHMATCH)
	{
		cvar_set("g_tdm_teams", car(s));
		s = cdr(s);
	}

	if(pWantedType == MAPINFO_TYPE_KEYHUNT)
	{
		cvar_set("g_keyhunt_teams", car(s));
		s = cdr(s);
	}
}

float MapInfo_Type_FromString(string t)
{
	if     (t == "dm")    return MAPINFO_TYPE_DEATHMATCH;
	else if(t == "tdm")   return MAPINFO_TYPE_TEAM_DEATHMATCH;
	else if(t == "dom")   return MAPINFO_TYPE_DOMINATION;
	else if(t == "ctf")   return MAPINFO_TYPE_CTF;
	else if(t == "rune")  return MAPINFO_TYPE_RUNEMATCH;
	else if(t == "lms")   return MAPINFO_TYPE_LMS;
	else if(t == "arena") return MAPINFO_TYPE_ARENA;
	else if(t == "kh")    return MAPINFO_TYPE_KEYHUNT;
	else if(t == "as")    return MAPINFO_TYPE_ASSAULT;
	else if(t == "ons")   return MAPINFO_TYPE_ONSLAUGHT;
	else if(t == "all")   return MAPINFO_TYPE_ALL;
	else                  return 0;
}

// load info about a map by name into the MapInfo_Map_* globals
float MapInfo_Get_ByName(string pFilename, float pAllowGenerate, float pGametypeToSet)
{
	string fn;
	string s, t;
	float fh, fh2;
	float r, f;

	if(pGametypeToSet == 0)
		if(MapInfo_Cache_Retrieve(pFilename))
			return 1;

	r = 1;

	MapInfo_Map_bspname = pFilename;

	// default all generic fields so they have "good" values in case something fails
	fn = strcat("maps/", pFilename, ".mapinfo");
	fh = fopen(fn, FILE_READ);
	if(fh < 0)
	{
		if(!pAllowGenerate)
			return 0;
		_MapInfo_Map_Reset();
		r = _MapInfo_Generate(pFilename);
		if(!r)
			return 0;
		fh = fopen(fn, FILE_WRITE);
		fputs(fh, strcat("title ", MapInfo_Map_title, "\n"));
		fputs(fh, strcat("description ", MapInfo_Map_description, "\n"));
		fputs(fh, strcat("author ", MapInfo_Map_author, "\n"));
		fputs(fh, strcat("_diameter ", ftos(MapInfo_Map_diameter), "\n"));
		fputs(fh, strcat("_spawnpoints ", ftos(MapInfo_Map_spawnpoints), "\n"));
		if(MapInfo_Map_supportedFeatures & MAPINFO_FEATURE_WEAPONS)       fputs(fh, "has weapons\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_DEATHMATCH)      fputs(fh, "type dm 30 20\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_TEAM_DEATHMATCH) fputs(fh, "type tdm 50 20 2\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_DOMINATION)      fputs(fh, "type dom 200 20\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_CTF)             fputs(fh, "type ctf 300 20\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_RUNEMATCH)       fputs(fh, "type rune 200 20\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_LMS)             fputs(fh, "type lms 9 20\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_ARENA)           fputs(fh, "type arena 10 20\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_KEYHUNT)         fputs(fh, "type kh 1000 20 3\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_ASSAULT)         fputs(fh, "type as 20\n");
		if(MapInfo_Map_supportedGametypes & MAPINFO_TYPE_ONSLAUGHT)       fputs(fh, "type ons 20\n");

		fh2 = fopen(strcat("scripts/", pFilename, ".arena"), FILE_READ);
		if(fh2 >= 0)
		{
			fclose(fh2);
			fputs(fh, "settemp_for_type all sv_q3acompat_machineshotgunswap 1\n");
		}

		fclose(fh);
		r = 2;
		// return r;
		fh = fopen(fn, FILE_READ);
		if(fh < 0)
			error("... but I just wrote it!");
	}

	_MapInfo_Map_Reset();
	for(;;)
	{
		if not((s = fgets(fh)))
			break;
		t = car(s); s = cdr(s);
		if     (t == "title")
			MapInfo_Map_title = s;
		else if(t == "description")
			MapInfo_Map_description = s;
		else if(t == "author")
			MapInfo_Map_author = s;
		else if(t == "_diameter")
			MapInfo_Map_diameter = stof(s);
		else if(t == "_spawnpoints")
			MapInfo_Map_spawnpoints = stof(s);
		else if(t == "has")
		{
			t = car(s); s = cdr(s);
			if     (t == "weapons") MapInfo_Map_supportedFeatures |= MAPINFO_FEATURE_WEAPONS;
			else
				dprint("Map ", pFilename, " supports unknown feature ", t, ", ignored\n");
		}
		else if(t == "type")
		{
			t = car(s); s = cdr(s);
			f = MapInfo_Type_FromString(t);
			if(f)
				_MapInfo_Map_ApplyGametype (s, pGametypeToSet, f);
			else
				dprint("Map ", pFilename, " supports unknown game type ", t, ", ignored\n");
		}
		else if(t == "settemp_for_type")
		{
			t = car(s); s = cdr(s);
			if((f = MapInfo_Type_FromString(t)))
			{
				if(f & pGametypeToSet)
				{
					t = car(s); s = cdr(s);
					if(strstrofs(t, "\"", 0) >= 0)
						print("Map ", pFilename, " contains a potentially harmful setting, ignored\n");
					else if(strstrofs(t, "\\", 0) >= 0)
						print("Map ", pFilename, " contains a potentially harmful setting, ignored\n");
					else if(strstrofs(t, ";", 0) >= 0)
						print("Map ", pFilename, " contains a potentially harmful setting, ignored\n");
					else if(strstrofs(s, "\"", 0) >= 0)
						print("Map ", pFilename, " contains a potentially harmful setting, ignored\n");
					else if(strstrofs(s, "\\", 0) >= 0)
						print("Map ", pFilename, " contains a potentially harmful setting, ignored\n");
					else if(strstrofs(s, ";", 0) >= 0)
						print("Map ", pFilename, " contains a potentially harmful setting, ignored\n");
					else
					{
						dprint("Applying temporary setting ", t, " := ", s, "\n");
						cvar_settemp(t, s);
					}
				}
			}
			else
			{
				dprint("Map ", pFilename, " has a setting for unknown game type ", t, ", ignored\n");
			}
		}
		else
			dprint("Map ", pFilename, " provides unknown info item ", t, ", ignored\n");
	}
	fclose(fh);
	if(pGametypeToSet)
		if(!(MapInfo_Map_supportedGametypes & pGametypeToSet))
			error("Can't select the requested game type. Bailing out.");
	MapInfo_Cache_Store();
	if(MapInfo_Map_supportedGametypes != 0)
		return r;
	dprint("Map ", pFilename, " supports no game types, ignored\n");
	return 0;
}

float MapInfo_FindName(string s)
{
	// if there is exactly one map of prefix s, return it
	// if not, return the null string
	// note that DP sorts glob results... so I can use a binary search
	float l, r, m, cmp;
	l = 0;
	r = MapInfo_count;
	// invariants: r is behind s, l-1 is equal or before
	while(l != r)
	{
		m = floor((l + r) / 2);
		MapInfo_FindName_match = _MapInfo_GlobItem(MapInfo_FilterList_Lookup(m));
		cmp = strcasecmp(MapInfo_FindName_match, s);
		if(cmp == 0)
			return m; // found and good
		if(cmp < 0)
			l = m + 1; // l-1 is before s
		else
			r = m; // behind s
	}
	MapInfo_FindName_match = _MapInfo_GlobItem(MapInfo_FilterList_Lookup(l));
	MapInfo_FindName_firstResult = l;
	// r == l, so: l is behind s, l-1 is before
	// SO: if there is any, l is the one with the right prefix
	//     and l+1 may be one too
	if(l == MapInfo_count)
	{
		MapInfo_FindName_match = string_null;
		MapInfo_FindName_firstResult = -1;
		return -1; // no MapInfo_FindName_match, behind last item
	}
	if(!startsWithNocase(MapInfo_FindName_match, s))
	{
		MapInfo_FindName_match = string_null;
		MapInfo_FindName_firstResult = -1;
		return -1; // wrong prefix
	}
	if(l == MapInfo_count - 1)
		return l; // last one, nothing can follow => unique
	if(startsWithNocase(_MapInfo_GlobItem(MapInfo_FilterList_Lookup(l + 1)), s))
	{
		MapInfo_FindName_match = string_null;
		return -1; // ambigous MapInfo_FindName_match
	}
	return l;
}

string MapInfo_FixName(string s)
{
	MapInfo_FindName(s);
	return MapInfo_FindName_match;
}

float MapInfo_CurrentFeatures()
{
	float req;
	req = 0;
	if(!(cvar("g_lms") || cvar("g_instagib") || cvar("g_minstagib") || cvar("g_nixnex") || cvar("g_rocketarena")))
		req |= MAPINFO_FEATURE_WEAPONS;
	return req;
}

float MapInfo_CurrentGametype()
{
	if(cvar("g_domination"))
		return MAPINFO_TYPE_DOMINATION;
	else if(cvar("g_ctf"))
		return MAPINFO_TYPE_CTF;
	else if(cvar("g_runematch"))
		return MAPINFO_TYPE_RUNEMATCH;
	else if(cvar("g_tdm"))
		return MAPINFO_TYPE_TEAM_DEATHMATCH;
	else if(cvar("g_assault"))
		return MAPINFO_TYPE_ASSAULT;
	else if(cvar("g_lms"))
		return MAPINFO_TYPE_LMS;
	else if(cvar("g_arena"))
		return MAPINFO_TYPE_ARENA;
	else if(cvar("g_keyhunt"))
		return MAPINFO_TYPE_KEYHUNT;
	else if(cvar("g_onslaught"))
		return MAPINFO_TYPE_ONSLAUGHT;
	else
		return MAPINFO_TYPE_DEATHMATCH;
}

float _MapInfo_CheckMap(string s) // returns 0 if the map can't be played with the current settings, 1 otherwise
{
	if(!MapInfo_Get_ByName(s, 1, 0))
		return 0;
	if((MapInfo_Map_supportedGametypes & MapInfo_CurrentGametype()) == 0)
		return 0;
	if((MapInfo_Map_supportedFeatures & MapInfo_CurrentFeatures()) != MapInfo_CurrentFeatures())
		return 0;
	return 1;
}

float MapInfo_CheckMap(string s) // returns 0 if the map can't be played with the current settings, 1 otherwise
{
	float r;
	r = _MapInfo_CheckMap(s);
	MapInfo_ClearTemps();
	return r;
}

void MapInfo_SwitchGameType(float t)
{
	cvar_set("gamecfg",      "0");
	cvar_set("g_dm",         (t == MAPINFO_TYPE_DEATHMATCH)      ? "1" : "0");
	cvar_set("g_tdm",        (t == MAPINFO_TYPE_TEAM_DEATHMATCH) ? "1" : "0");
	cvar_set("g_domination", (t == MAPINFO_TYPE_DOMINATION)      ? "1" : "0");
	cvar_set("g_ctf",        (t == MAPINFO_TYPE_CTF)             ? "1" : "0");
	cvar_set("g_runematch",  (t == MAPINFO_TYPE_RUNEMATCH)       ? "1" : "0");
	cvar_set("g_lms",        (t == MAPINFO_TYPE_LMS)             ? "1" : "0");
	cvar_set("g_arena",      (t == MAPINFO_TYPE_ARENA)           ? "1" : "0");
	cvar_set("g_keyhunt",    (t == MAPINFO_TYPE_KEYHUNT)         ? "1" : "0");
	cvar_set("g_assault",    (t == MAPINFO_TYPE_ASSAULT)         ? "1" : "0");
	cvar_set("g_onslaught",  (t == MAPINFO_TYPE_ONSLAUGHT)       ? "1" : "0");
}

void MapInfo_LoadMap(string s)
{
	MapInfo_Map_supportedGametypes = 0;
	if(!MapInfo_CheckMap(s))
	{
		print("EMERGENCY: can't play the selected map in the given game mode. Falling back to DM.\n");
		MapInfo_SwitchGameType(MAPINFO_TYPE_DEATHMATCH);
	}
	localcmd(strcat("\nsettemp_restore\nchangelevel ", s, "\n"));
}

string MapInfo_ListAllowedMaps()
{
	string out;
	float i;

	// to make absolutely sure:
	MapInfo_Enumerate();
	MapInfo_FilterGametype(MapInfo_CurrentGametype(), MapInfo_CurrentFeatures(), 0);

	out = "";
	for(i = 0; i < MapInfo_count; ++i)
		out = strcat(out, " ", _MapInfo_GlobItem(MapInfo_FilterList_Lookup(i)));
	return substring(out, 1, strlen(out) - 1);
}

void MapInfo_LoadMapSettings(string s) // to be called from worldspawn
{
	float t;
	if(!_MapInfo_CheckMap(s)) // with underscore, it keeps temps
	{
		if(MapInfo_Map_supportedGametypes <= 0)
			error("Mapinfo system is not functional at all. BAILED OUT.\n");

		t = 1;
		while(!(MapInfo_Map_supportedGametypes & 1))
		{
			t *= 2;
			MapInfo_Map_supportedGametypes = floor(MapInfo_Map_supportedGametypes / 2);
		}
		// t is now a supported mode!
		print("EMERGENCY: can't play the selected map in the given game mode. Falling back to a supported mode.\n");
		MapInfo_SwitchGameType(t);
	}
	cvar_settemp_restore();
	MapInfo_Get_ByName(s, 1, MapInfo_CurrentGametype());
	MapInfo_ClearTemps();
}

void MapInfo_ClearTemps()
{
	MapInfo_Map_bspname = string_null;
	MapInfo_Map_title = string_null;
	MapInfo_Map_description = string_null;
	MapInfo_Map_author = string_null;
	MapInfo_Map_supportedGametypes = 0;
	MapInfo_Map_supportedFeatures = 0;
	MapInfo_Map_diameter = 0;
	MapInfo_Map_spawnpoints = 0;
}
