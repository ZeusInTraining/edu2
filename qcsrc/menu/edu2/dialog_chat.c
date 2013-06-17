#ifdef INTERFACE
CLASS(ChatDialog) EXTENDS(NexuizRootDialog)
	METHOD(ChatDialog, fill, void(entity)) // to be overridden by user to fill the dialog with controls
	METHOD(ChatDialog, arrange, void(entity))
	METHOD(ChatDialog, focusEnter, void(entity))
	METHOD(ChatDialog, hideNotify, void(entity))
	METHOD(ChatDialog, resizeNotify, void(entity, vector, vector, vector, vector))

	ATTRIB(ChatDialog, title, string, _("Chat")) // ;)
	ATTRIB(ChatDialog, color, vector, SKINCOLOR_DIALOG_TEAMSELECT)
	ATTRIB(ChatDialog, titleHeight, float, SKINFONTSIZE_NORMAL * SKINHEIGHT_NORMAL ) // pixels
	ATTRIB(ChatDialog, titleFontSize, float, SKINFONTSIZE_NORMAL ) // pixels

	ATTRIB(ChatDialog, intendedWidth, float, 0.6)
	ATTRIB(ChatDialog, rows, float, 1)
	ATTRIB(ChatDialog, columns, float, 16)
	ATTRIB(ChatDialog, name, string, "ChatDialog")

	ATTRIB(ChatDialog, inputBox, entity, NULL)
	ATTRIB(ChatDialog, messageLog, entity, NULL)
	ATTRIB(ChatDialog, logButton, entity, NULL)

	// logLines = logMode * 5;
	ATTRIB(ChatDialog, logMode, float, 0 )
ENDCLASS(ChatDialog)
#endif

#ifdef IMPLEMENTATION
void ChatDialog_onEnter(entity inputbox, entity me )
{
	localcmd( "localsay ", inputbox.text, "\n" );
	inputbox.setText( inputbox, "" );
	//m_goto( "" );
}

void ChatDialog_onLogClick(entity button, entity me) {
	if( me.logMode != 0 ) {
		me.removeItem( me, me.messageLog );
	}
	me.removeItem( me, me.inputBox );
	me.removeItem( me, me.logButton );

	me.logMode = mod( me.logMode + 1, 4 );

	me.rows = me.logMode * 5 + 1;
	me.setup( me );
	me.arrange( me );

	main.removeItem( main, me );

	vector newSize;
	newSize = me.intendedWidth * eX + me.intendedHeight * eY;
	main.addItem( main, me, me.Container_origin + me.Container_size - newSize, newSize, SKINALPHAS_MAINMENU_z);

	// force a renotification of all items
	RenotifyAll();
}

void hideNotifyChatDialog(entity me)
{
	// HACK to hide the mobile/viewmodel when you only press escape.
	localcmd("cl_cmd chatclosed\n");
	SUPER.hideNotify( me );
}

void focusEnterChatDialog(entity me)
{
	SUPER.focusEnter( me );
	me.setFocus( me, me.inputBox );
}

void resizeNotifyChatDialog(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize) {
	SUPER.resizeNotify( me, relOrigin, relSize, absOrigin, absSize );
}

void arrangeChatDialog(entity me)
{
	me.gotoRC(me, 0, 0);

	float logNumLines;
	logNumLines = me.logMode * 5;
	if( logNumLines > 0 ) {
		me.TD(me, logNumLines, me.columns, me.messageLog );
	}
	me.gotoRC(me, logNumLines, 0);
		me.TD(me, 1, me.columns - 1, me.inputBox );
		me.TD(me, 1, 1, me.logButton );
}

void fillChatDialog(entity me)
{
	me.moveable = true;

	me.messageLog = makeEdu2MessageLog();
	me.inputBox = makeChatInputBox( true, false );
	me.inputBox.onEnterEntity = me;
	me.inputBox.onEnter = ChatDialog_onEnter;

	me.logButton = makeNexuizButton( _("Log"), '0 0 0' );
	me.logButton.onClick = ChatDialog_onLogClick;
	me.logButton.onClickEntity = me;

	me.arrange( me );
}
#endif

// click. The C-word so you can grep for it.
