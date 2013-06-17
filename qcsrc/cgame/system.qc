float vid_width, vid_height;
float vid_realwidth, vid_realheight;

vector pl_view_forward;
vector pl_view_right;
vector pl_view_up;
float pl_xyspeed;

//float player_flags;
//float spec;

void SysParms_Fetch (void)
{
	vid_width = cvar("vid_conwidth");
	vid_height = cvar("vid_conheight");
	vid_realwidth = cvar("vid_width");
	vid_realheight = cvar("vid_height");
	makevectors(input_angles);
	pl_view_forward = v_forward;
	pl_view_right = v_right;
	pl_view_up = v_up;
	pl_xyspeed = floor(sqrt(pmove_vel_x*pmove_vel_x + pmove_vel_y*pmove_vel_y));

	// sorry but this is no deathmatch game...
//	f = getstati(STAT_HEALTH);
//	spec = (f == -666);
//	if(f < 1 && !(player_flags & PFL_DEAD))
//		player_flags |= PFL_DEAD;
//	else if(f >= 1 && player_flags & PFL_DEAD)
//		player_flags -= player_flags & (PFL_DEAD | PFL_GIBBED);
}
