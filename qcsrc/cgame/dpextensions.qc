//DP_GFX_QUAKE3MODELTAGS
//idea: id Software
//darkplaces implementation: LordHavoc
//field definitions:
.entity tag_entity; // entity this is attached to (call setattachment to set this)
.float tag_index; // which tag on that entity (0 is relative to the entity, > 0 is an index into the tags on the model if it has any) (call setattachment to set this)
//builtin definitions:
//void(entity e, entity tagentity, string tagname) setattachment = #443; // attachs e to a tag on tagentity (note: use "" to attach to entity origin/angles instead of a tag)
//description:
//allows entities to be visually attached to model tags (which follow animations perfectly) on other entities, for example attaching a weapon to a player's hand, or upper body attached to lower body, allowing it to change angles and frame separately (note: origin and angles are relative to the tag, use '0 0 0' for both if you want it to follow exactly, this is similar to viewmodelforclient's behavior).
//note 2: if the tag is not found, it defaults to "" (attach to origin/angles of entity)
//note 3: attaching to world turns off attachment
//note 4: the entity that this is attached to must be visible for this to work
//note 5: if an entity is attached to the player entity it will not be drawn in first person.
