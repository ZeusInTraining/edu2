// Andreas Kirsch 08
// input box for chats
// TODO: rename to Edu2InputBox Black
#ifdef INTERFACE
CLASS(ChatInputBox) EXTENDS(InputBox)
	METHOD(ChatInputBox, configureChatInputBox, void(entity, float, float))
	//METHOD(ChatInputBox, focusLeave, void(entity))
	//METHOD(ChatInputBox, setText, void(entity, string))
	METHOD(ChatInputBox, keyDown, float(entity, float, float, float))
	ATTRIB(ChatInputBox, fontSize, float, SKINFONTSIZE_NORMAL)
	ATTRIB(ChatInputBox, image, string, SKINGFX_INPUTBOX)
	ATTRIB(ChatInputBox, onEnter, void(entity, entity), SUB_Null) // onEnterEntity, me
	ATTRIB(ChatInputBox, onEnterEntity, entity, NULL)
	ATTRIB(ChatInputBox, keepspaceLeft, float, SKINMARGIN_INPUTBOX)
	ATTRIB(ChatInputBox, keepspaceRight, float, SKINMARGIN_INPUTBOX)
	ATTRIB(ChatInputBox, color, vector, SKINCOLOR_INPUTBOX_N)
	ATTRIB(ChatInputBox, colorF, vector, SKINCOLOR_INPUTBOX_F)

	//ATTRIB(ChatInputBox, cvarName, string, string_null)
	//METHOD(ChatInputBox, loadCvars, void(entity))
	//METHOD(ChatInputBox, saveCvars, void(entity))
ENDCLASS(ChatInputBox)
entity makeChatInputBox( float lEditColorCodes, float lIsPasswordField );
#endif

#ifdef IMPLEMENTATION
entity makeChatInputBox( float lEditColorCodes, float lIsPasswordField )
{
	entity me;
	me = spawnChatInputBox();
	me.configureChatInputBox(me, lEditColorCodes, lIsPasswordField);
	return me;
}
void configureChatInputBoxChatInputBox(entity me, float lEditColorCodes, float lIsPasswordField )
{
	me.configureInputBox(me, "", 0, me.fontSize, me.image);
	me.editColorCodes = lEditColorCodes;
	me.isPassword = lIsPasswordField;
	me.cursorPos = strlen(me.text);
}
float keyDownChatInputBox(entity me, float key, float ascii, float shift)
{
	float returnValue;
	returnValue = keyDownInputBox( me, key, ascii, shift );
	if( key == K_ENTER ) {
		me.onEnter( me, me.onEnterEntity );
	}
	return returnValue;
}
#endif
