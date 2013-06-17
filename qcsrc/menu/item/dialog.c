// Note: this class is called Dialog, but it can also handle a tab under the following conditions:
// - isTabRoot is 0
// - backgroundImage is the tab's background
// - closable is 0
// - rootDialog is 0
// - title is ""
// - marginTop is
// - intendedHeight ends up to be the tab's actual height, or at least close
// - titleFontSize is 0
// - marginTop cancels out as much of titleHeight as needed (that is, it should be actualMarginTop - titleHeight)
// To ensure the latter, you best create all tabs FIRST and insert the tabbed
// control to your dialog THEN - with the right height
//
// a subclass may help with using this as a tab

#ifdef INTERFACE
CLASS(Dialog) EXTENDS(InputContainer)
	METHOD(Dialog, configureDialog, void(entity)) // no runtime configuration, all parameters are given in the code!
	METHOD(Dialog, fill, void(entity)) // to be overridden by user to fill the dialog with controls
	METHOD(Dialog, keyDown, float(entity, float, float, float))
	METHOD(Dialog, close, void(entity))
	METHOD(Dialog, addItemSimple, void(entity, float, float, float, float, entity, vector))

	METHOD(Dialog, setup, void(entity))

	METHOD(Dialog, TD, void(entity, float, float, entity))
	METHOD(Dialog, TDNoMargin, void(entity, float, float, entity, vector))
	METHOD(Dialog, TDempty, void(entity, float))
	METHOD(Dialog, setFirstColumn, void(entity, float))
	METHOD(Dialog, TR, void(entity))
	METHOD(Dialog, gotoRC, void(entity, float, float))

	ATTRIB(Dialog, isTabRoot, float, 1)
	ATTRIB(Dialog, closeButton, entity, NULL)
	ATTRIB(Dialog, intendedHeight, float, 0)
	ATTRIB(Dialog, itemOrigin, vector, '0 0 0')
	ATTRIB(Dialog, itemSize, vector, '0 0 0')
	ATTRIB(Dialog, itemSpacing, vector, '0 0 0')
	ATTRIB(Dialog, currentRow, float, 0)
	ATTRIB(Dialog, currentColumn, float, 0)
	ATTRIB(Dialog, firstColumn, float, 0)

	ATTRIB(Dialog, moveable, float, 0)
	ATTRIB(Dialog, titleBar, entity, NULL)

	// to be customized
	ATTRIB(Dialog, closable, float, 1)
	ATTRIB(Dialog, title, string, "Form1") // ;)
	ATTRIB(Dialog, color, vector, '1 0.5 1')
	ATTRIB(Dialog, intendedWidth, float, 0)
	ATTRIB(Dialog, rows, float, 3)
	ATTRIB(Dialog, columns, float, 2)

	ATTRIB(Dialog, marginTop, float, 0) // pixels
	ATTRIB(Dialog, marginBottom, float, 0) // pixels
	ATTRIB(Dialog, marginLeft, float, 0) // pixels
	ATTRIB(Dialog, marginRight, float, 0) // pixels
	ATTRIB(Dialog, columnSpacing, float, 0) // pixels
	ATTRIB(Dialog, rowSpacing, float, 0) // pixels
	ATTRIB(Dialog, rowHeight, float, 0) // pixels
	ATTRIB(Dialog, titleHeight, float, 0) // pixels
	ATTRIB(Dialog, titleFontSize, float, 0) // pixels; if 0, title causes no margin
	ATTRIB(Dialog, titleFont, float, 0)

	ATTRIB(BorderImage, tileBarContainerSize_Nexposeed, vector, '0 0 0')
	ATTRIB(BorderImage, tileBarContainerOrigin_Nexposeed, vector, '0 0 0')
	ATTRIB(BorderImage, isNexposeeTitleBar, float, 0)
	ATTRIB(Dialog, zoomedOutTitleBarPosition, float, 0)
	ATTRIB(Dialog, zoomedOutTitleBar, float, 0)

	ATTRIB(Dialog, backgroundImage, string, string_null)
	ATTRIB(Dialog, closeButtonImage, string, string_null)

	METHOD(Dialog, resizeNotify, void(entity, vector, vector, vector, vector))
	METHOD(Dialog, draw, void(entity))
	ATTRIB(Dialog, borderVec, vector, '0 0 0')

	ATTRIB(Dialog, lastClickPosition, vector, '0 0 0' )
	METHOD(Dialog, mousePress, float(entity, vector))
	METHOD(Dialog, mouseDrag, float(entity, vector))
ENDCLASS(Dialog)
#endif

#ifdef IMPLEMENTATION
void Dialog_Close(entity button, entity me)
{
	me.close(me);
}

void fillDialog(entity me)
{
}

void addItemSimpleDialog(entity me, float row, float col, float rowspan, float colspan, entity e, vector v)
{
	//print(vtos(me.itemSpacing), " ", vtos(me.itemSize), "\n");
	vector o, s;
	o = me.itemOrigin + eX * ( col          * me.itemSpacing_x) + eY * ( row          * me.itemSpacing_y);
	s = me.itemSize   + eX * ((colspan - 1) * me.itemSpacing_x) + eY * ((rowspan - 1) * me.itemSpacing_y);
	o_x -= 0.5 * (me.itemSpacing_x - me.itemSize_x) * v_x;
	s_x +=       (me.itemSpacing_x - me.itemSize_x) * v_x;
	o_y -= 0.5 * (me.itemSpacing_y - me.itemSize_y) * v_y;
	s_y +=       (me.itemSpacing_y - me.itemSize_y) * v_y;
	me.addItem(me, e, o, s, 1);
}

void gotoRCDialog(entity me, float row, float col)
{
	me.currentRow = row;
	me.currentColumn = col;
}

void TRDialog(entity me)
{
	me.currentRow += 1;
	me.currentColumn = me.firstColumn;
}

void TDDialog(entity me, float rowspan, float colspan, entity e)
{
	me.addItemSimple(me, me.currentRow, me.currentColumn, rowspan, colspan, e, '0 0 0');
	me.currentColumn += colspan;
}

void TDNoMarginDialog(entity me, float rowspan, float colspan, entity e, vector v)
{
	me.addItemSimple(me, me.currentRow, me.currentColumn, rowspan, colspan, e, v);
	me.currentColumn += colspan;
}

void setFirstColumnDialog(entity me, float col)
{
	me.firstColumn = col;
}

void TDemptyDialog(entity me, float colspan)
{
	me.currentColumn += colspan;
}

void resizeNotifyDialog(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize)
{
	me.borderVec = me.titleHeight / absSize_y * (eY + eX * (absSize_y / absSize_x));
	me.titleBar.Container_size_y = me.borderVec_y;

	resizeNotifyInputContainer(me, relOrigin, relSize, absOrigin, absSize);

	me.isNexposeeTitleBar = false;
	if(me.zoomedOutTitleBar)
		if(me.parent.instanceOfNexposee)
			if(me.instanceOfDialog)
				me.isNexposeeTitleBar = true;
	if(me.isNexposeeTitleBar)
	{
		vector scrs;
		scrs = eX * conwidth + eY * conheight;
		me.tileBarContainerSize_Nexposeed_x = me.titleBar.Container_size_x / me.Nexposee_smallSize_x * me.Container_size_x;
		me.tileBarContainerSize_Nexposeed_y = me.titleBar.Container_size_y / me.Nexposee_smallSize_y * me.Container_size_y;
		me.tileBarContainerOrigin_Nexposeed_x = (me.tileBarContainerSize_Nexposeed_x - 1.0) * -0.5;
		me.tileBarContainerOrigin_Nexposeed_y = me.tileBarContainerSize_Nexposeed_y * me.zoomedOutTitleBarPosition;
	}

	if(me.closeButton)
	{
		// move the close button to the right place
		me.closeButton.Container_origin = '1 0 0' * (1 - me.borderVec_x);
		me.closeButton.Container_size = me.borderVec;
		me.closeButton.color = me.color;
		me.closeButton.colorC = me.color;
		me.closeButton.colorF = me.color;
	}
}

float mousePressDialog( entity me, vector pos ) {
	me.moveable = (me.moveable >= 1);
	if( pos_y <= me.titleBar.Container_size_y && me.moveable ) {
		me.lastClickPosition = boxToGlobal( pos, me.Container_origin, me.Container_size);
		me.moveable = 2;
		return 1;
	}

	return mousePressInputContainer( me, pos );
}

float mouseDragDialog( entity me, vector pos ) {
	if( me.moveable == 2 ) {
		pos = boxToGlobal( pos, me.Container_origin, me.Container_size);

		vector delta;
		delta = pos - me.lastClickPosition;
		me.Container_origin = me.Container_origin + delta;
		// make sure that everyone gets it :)
		me.ModalController_initialOrigin = me.Container_origin;
		me.Nexposee_initialOrigin = me.Container_origin;
		me.lastClickPosition = pos;

		return 1;
	}

	return mouseDragInputContainer( me, pos );
}

void drawDialog(entity me)
{
	if(me.backgroundImage)
		draw_BorderPicture('0 0 0', me.backgroundImage, '1 1 0', me.color, 1, me.borderVec);

	vector ro, rf;
	if(me.isNexposeeTitleBar)
	{
		// me.parent.Nexposee_animationFactor 0 (small) or 1 (full)
		// default values are for 1
		ro = me.titleBar.Container_origin;
		rf = me.titleBar.Container_size;

		me.titleBar.Container_origin = ro * me.Nexposee_animationFactor + me.tileBarContainerOrigin_Nexposeed * (1 - me.Nexposee_animationFactor);
		me.titleBar.Container_size = rf * me.Nexposee_animationFactor + me.tileBarContainerSize_Nexposeed * (1 - me.Nexposee_animationFactor);
	}

	drawContainer(me);

	if(me.isNexposeeTitleBar)
	{
		// me.Nexposee_animationState 0 (small) or 1 (full)
		// default values are for 1
		me.titleBar.Container_origin = ro;
		me.titleBar.Container_size  = rf;
	}
}

void setupDialog(entity me)
{
	float absWidth, absHeight;
	absWidth = me.intendedWidth * conwidth;
	absHeight = me.titleHeight + me.marginTop + me.rows * me.rowHeight + (me.rows - 1) * me.rowSpacing + me.marginBottom;
	me.itemOrigin  = eX * (me.marginLeft / absWidth)
				   + eY * ((me.titleHeight + me.marginTop) / absHeight);
	me.itemSize    = eX * ((1 - (me.marginLeft + me.marginRight + me.columnSpacing * (me.columns - 1)) / absWidth) / me.columns)
				   + eY * (me.rowHeight / absHeight);
	me.itemSpacing = me.itemSize
				   + eX * (me.columnSpacing / absWidth)
				   + eY * (me.rowSpacing / absHeight);
	me.intendedHeight = absHeight / conheight;
	me.currentRow = -1;
	me.currentColumn = -1;
}

void configureDialogDialog(entity me)
{
	me.titleBar = spawnLabel();
	me.titleBar.focusable = true;
	me.titleBar.configureLabel(me.titleBar, me.title, me.titleFontSize, 0.5);
	me.titleBar.color = me.color;
	me.titleBar.font = me.titleFont;
	me.titleBar.alpha = me.alpha * 2;
	me.addItem(me, me.titleBar, '0 0 0', '1 1 0', 1);

	if not(me.titleFontSize)
		me.titleHeight = 0; // no title bar

	me.setup(me);

	me.fill(me);

	if(me.closable)
	{
		entity closebutton;
		closebutton = me.closeButton = spawnButton();
		closebutton.configureButton(closebutton, "Close", 0, me.closeButtonImage);
		closebutton.onClick = Dialog_Close; closebutton.onClickEntity = me;
		closebutton.srcMulti = 0;
		me.addItem(me, closebutton, '0 0 0', '1 1 0', 1); // put it as LAST
	}
}

void closeDialog(entity me)
{
	if(me.parent.instanceOfNexposee)
	{
		ExposeeCloseButton_Click(me, me.parent);
	}
	else if(me.parent.instanceOfModalController)
	{
		DialogCloseButton_Click(me, me);
	}
}

float keyDownDialog(entity me, float key, float ascii, float shift)
{
	if(me.closable)
	{
		if(key == K_ESCAPE)
		{
			me.close(me);
			return 1;
		}
	}
	return keyDownInputContainer(me, key, ascii, shift);
}
#endif
