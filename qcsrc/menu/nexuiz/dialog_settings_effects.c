#ifdef INTERFACE
CLASS(NexuizEffectsSettingsTab) EXTENDS(NexuizTab)
	METHOD(NexuizEffectsSettingsTab, fill, void(entity))
	ATTRIB(NexuizEffectsSettingsTab, title, string, _("Effects"))
	ATTRIB(NexuizEffectsSettingsTab, intendedWidth, float, 0.9)
	ATTRIB(NexuizEffectsSettingsTab, rows, float, 22)
	ATTRIB(NexuizEffectsSettingsTab, columns, float, 6.5)
ENDCLASS(NexuizEffectsSettingsTab)
entity makeNexuizEffectsSettingsTab();
#endif

#ifdef IMPLEMENTATION
entity makeNexuizEffectsSettingsTab()
{
	entity me;
	me = spawnNexuizEffectsSettingsTab();
	me.configureDialog(me);
	return me;
}
/*
 * cl_decals 1
 * cl_nogibs 0
 * cl_particles_quality 1
 * cl_particles_snow 1
 * r_bloom 1
 * r_coronas 1
 * r_glsl_deluxemapping 1
 * r_glsl_offsetmapping 1
 * r_glsl_offsetmapping_reliefmapping 0
 * r_hdr 0
 * r_shadow_usenormalmap 1
 * r_shadow_gloss 1
 * r_shadow_realtime_dlight 1
 * r_shadow_realtime_dlight_shadows 1
 * r_shadow_realtime_world 1
 * r_shadow_realtime_world_shadows 1
 * r_depthfirst 2
 * r_showsurfaces 0
 * r_water 1
 * r_water_resolutionmultiplier 0.5
 *
 *
 * [X] cl_decals
 * [X] !cl_nogibs
 * Particles: |--v--|
 * [X] Bloom [X] HDR
 * [X] Coronas
 * [X] Deluxemapping [X] Gloss
 * [X] Offsetmapping [X] Reliefmapping
 * [X] dlights [X] normalmaps [X] shadows
 * [X] rtworld [X] shadows
 * [X] depth first world [X] depth first models (?????)
 * [X] OMGLOLWTFBBQ (showsurfaces)
 * [X] water |----------v----|
 *
 *
 */
void fillNexuizEffectsSettingsTab(entity me)
{
	entity e;
	float n;
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Quality preset:")));
		n = 6 + !!cvar("developer");
		if(cvar("developer"))
			me.TD(me, 1, 5.5 / n, e = makeNexuizCommandButton(_("OMG!"), '1 0 1', "exec omg.cfg", 0));
		me.TD(me, 1, 5.5 / n, e = makeNexuizCommandButton(_("Low"), '0 0 0', "exec low.cfg", 0));
		me.TD(me, 1, 5.5 / n, e = makeNexuizCommandButton(_("Medium"), '0 0 0', "exec med.cfg", 0));
		me.TD(me, 1, 5.5 / n, e = makeNexuizCommandButton(_("Normal"), '0 0 0', "exec normal.cfg", 0));
		me.TD(me, 1, 5.5 / n, e = makeNexuizCommandButton(_("High"), '0 0 0', "exec high.cfg", 0));
		me.TD(me, 1, 5.5 / n, e = makeNexuizCommandButton(_("Ultra"), '0 0 0', "exec ultra.cfg", 0));
		me.TD(me, 1, 5.5 / n, e = makeNexuizCommandButton(_("Ultimate"), '0 0 0', "exec ultimate.cfg", 0));

	me.TR(me);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Texture quality:")));
		me.TD(me, 1, 2, e = makeNexuizTextSlider("gl_picmip"));
			if(cvar("developer"))
				e.addValue(e, _("Leet"), "1337");
			e.addValue(e, _("Lowest"), "4");
			e.addValue(e, _("Low"), "3");
			e.addValue(e, _("Normal"), "2");
			e.addValue(e, _("Good"), "1");
			e.addValue(e, _("Best"), "0");
			e.configureNexuizTextSliderValues(e);
	me.TR(me);
		me.TDempty(me, 0.2);
		me.TD(me, 1, 2.8, e = makeNexuizCheckBox(1, "r_picmipworld", _("Reduce model texture quality only")));
			setDependent(e, "gl_picmip", 0.5, -0.5);

	me.TR(me);

	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Particle quality:")));
		me.TD(me, 1, 2, e = makeNexuizSlider(0.1, 1.0, 0.05, "cl_particles_quality"));
	me.TR(me);
		me.TD(me, 1, 3, e = makeNexuizCheckBox(0, "cl_decals", _("Decals")));
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Gibs:")));
		me.TD(me, 1, 2, e = makeNexuizTextSlider("cl_nogibs"));
			if(cvar("developer"))
				e.addValue(e, _("German"), "1");
			e.addValue(e, _("Few"), "0.75");
			e.addValue(e, _("Many"), "0.5");
			e.addValue(e, _("Lots"), "1");
			e.configureNexuizTextSliderValues(e);

	me.TR(me);

	me.TR(me);
		me.TD(me, 1, 3, e = makeNexuizCheckBox(0, "r_coronas", _("Coronas")));
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizCheckBox(0, "r_bloom", _("Bloom")));
			setDependent(e, "r_hdr", 0, 0);
		me.TD(me, 1, 2, e = makeNexuizCheckBox(0, "r_hdr", _("High Dynamic Range (HDR)")));

	me.TR(me);

	me.TR(me);
		me.TD(me, 1, 3, e = makeNexuizCheckBox(0, "showfps", _("Show frames per second")));

	me.gotoRC(me, 2, 3.5); me.setFirstColumn(me, me.currentColumn);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Anisotropy:")));
		me.TD(me, 1, 2, e = makeNexuizTextSlider("gl_texture_anisotropy"));
			e.addValue(e, "1x", "1");
			e.addValue(e, "2x", "2");
			e.addValue(e, "4x", "4");
			e.addValue(e, "8x", "8");
			e.addValue(e, "16x", "16");
			e.configureNexuizTextSliderValues(e);
	me.TR(me);
		me.TD(me, 1, 1.5, e = makeNexuizCheckBox(0, "r_glsl_deluxemapping", _("Deluxe mapping")));
			setDependent(e, "r_glsl", 1, 1);
		me.TD(me, 1, 1.5, e = makeNexuizCheckBox(0, "r_shadow_gloss", _("Gloss")));
			setDependentAND(e, "r_glsl", 1, 1, "r_glsl_deluxemapping", 1, 1);

	me.TR(me);

	me.TR(me);
		me.TD(me, 1, 2, e = makeNexuizCheckBox(0, "r_shadow_realtime_dlight", _("Realtime dynamic lighting")));
		me.TD(me, 1, 1, e = makeNexuizCheckBox(0, "r_shadow_realtime_dlight_shadows", _("Shadows")));
			setDependent(e, "r_shadow_realtime_dlight", 1, 1);

	me.TR(me);
		me.TD(me, 1, 2, e = makeNexuizCheckBox(0, "r_shadow_realtime_world", _("Realtime world lighting")));
		me.TD(me, 1, 1, e = makeNexuizCheckBox(0, "r_shadow_realtime_world_shadows", _("Shadows")));
			setDependent(e, "r_shadow_realtime_world", 1, 1);

	me.TR(me);
		me.TDempty(me, 0.2);
		me.TD(me, 1, 2.8, e = makeNexuizCheckBox(0, "r_shadow_usenormalmap", _("Use normal maps")));
			setDependentOR(e, "r_shadow_realtime_dlight", 1, 1, "r_shadow_realtime_world", 1, 1);

	me.TR(me);

	me.TR(me);
		me.TD(me, 1, 1.5, e = makeNexuizCheckBox(0, "r_glsl_offsetmapping", _("Offset mapping")));
			setDependent(e, "r_glsl", 1, 1);
		me.TD(me, 1, 1.5, e = makeNexuizCheckBox(0, "r_glsl_offsetmapping_reliefmapping", _("Relief mapping")));
			setDependentAND(e, "r_glsl", 1, 1, "r_glsl_offsetmapping", 1, 1);
	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizCheckBox(0, "r_water", _("Reflections")));
			setDependent(e, "r_glsl", 1, 1);
		me.TD(me, 1, 2, e = makeNexuizTextSlider("r_water_resolutionmultiplier"));
			e.addValue(e, _("Blurred"), "0.25");
			e.addValue(e, _("Good"), "0.5");
			e.addValue(e, _("Sharp"), "1");
			e.addValue(e, _("Insane"), "2");
			e.configureNexuizTextSliderValues(e);
			setDependentAND(e, "r_glsl", 1, 1, "r_water", 1, 1);

	me.TR(me);

	me.TR(me);
		me.TD(me, 1, 1, e = makeNexuizTextLabel(0, _("Damage view kick:")));
		me.TD(me, 1, 2, e = makeNexuizSlider(0, 0.5, 0.05, "v_kicktime"));

	me.TR(me);

	me.TR(me);
		if(cvar("developer"))
			me.TD(me, 1, 3, e = makeNexuizCheckBox(0, "r_showsurfaces", _("Show surfaces")));

	me.gotoRC(me, me.rows - 1, 0);
		me.TD(me, 1, me.columns, makeNexuizCommandButton(_("Apply immediately"), '0 0 0', "sendcvar cl_nogibs; r_restart", COMMANDBUTTON_APPLY));
}
/*
 * [X] depth first world [X] depth first models (?????)
 * [X] OMGLOLWTFBBQ (showsurfaces)
 *
 *
 */
#endif
