// Ak 2008
#ifdef INTERFACE
CLASS(Edu2MessageLog) EXTENDS(NexuizListBox)
	METHOD(Edu2MessageLog, configureEdu2MessageLog, void(entity))
	METHOD(Edu2MessageLog, drawListBoxItem, void(entity, float, vector, float))
	METHOD(Edu2MessageLog, clickListBoxItem, void(entity, float, vector))
	//ATTRIB(Edu2MessageLog, fontSize, float, SKINFONTSIZE_NORMAL)

	METHOD(Edu2MessageLog, onAddMessage, void(entity))
ENDCLASS(Edu2MessageLog)
entity makeEdu2MessageLog();
#endif

#ifdef IMPLEMENTATION
entity makeEdu2MessageLog()
{
	entity me;
	me = spawnEdu2MessageLog();
	me.configureEdu2MessageLog(me);
	return me;
}
void configureEdu2MessageLogEdu2MessageLog(entity me)
{
	messageLog_listBox = me;
	me.configureNexuizListBox(me); // item height gets set up later
}
void clickListBoxItemEdu2MessageLog(entity me, float i, vector where)
{
	// itemclick, itemclick, does whatever itemclick does
}

void drawListBoxItemEdu2MessageLog(entity me, float i, vector absSize, float selected)
{
	string theText;
	vector theRealFontSize;
	theText = MessageLog_Get( i );
	//print( theText, "\n" );
	theRealFontSize_x = me.fontSize / absSize_x;
	theRealFontSize_y = me.fontSize / absSize_y;
	draw_Text('0 0 0', theText , theRealFontSize, '1 1 1', 1, true);
}

void onAddMessageEdu2MessageLog(entity me)
{
	me.nItems = MessageLog_GetCount();
	me.setSelected( me, me.nItems - 1 );
}
#endif
