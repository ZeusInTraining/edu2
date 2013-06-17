#ifdef INTERFACE
CLASS(MineSweeper) EXTENDS(NexuizDialog /*NexuizRootDialog*/)
	METHOD(MineSweeper, fill, void(entity)) // to be overridden by user to fill the dialog with controls
	METHOD(MineSweeper, arrange, void(entity))

	ATTRIB(MineSweeper, title, string, _("MineSweeper")) // ;)
	ATTRIB(MineSweeper, color, vector, SKINCOLOR_DIALOG_TEAMSELECT)
	//ATTRIB(MineSweeper, titleHeight, float, SKINFONTSIZE_NORMAL * SKINHEIGHT_NORMAL ) // pixels
	//ATTRIB(MineSweeper, titleFontSize, float, SKINFONTSIZE_NORMAL ) // pixels

	ATTRIB(MineSweeper, intendedWidth, float, 0.3)
	ATTRIB(MineSweeper, intendedHeight, float, 0.5)
	ATTRIB(MineSweeper, rows, float, 12 )
	ATTRIB(MineSweeper, columns, float, 10)
	ATTRIB(MineSweeper, name, string, "MineSweeper")

	ATTRIB(MineSweeper, modeButton, entity, NULL)
	ATTRIB(MineSweeper, gameFace, entity, NULL)
	ATTRIB(MineSweeper, minesLeftButton, entity, NULL)
ENDCLASS(MineSweeper)
#endif

#ifdef IMPLEMENTATION

// used by the cell buttons (user data)
#define MINESWEEPER_MARKER 		"?"
#define MINESWEEPER_BOMB_MARKER "X"
#define MINESWEEPER_UNMARKED	" "

// use by the labels (the actual data)
#define MINESWEEPER_EMPTY		" "
#define MINESWEEPER_BOMB		"X"

#define MINESWEEPER_BUTTON_CLASS "MineSweeperButton"
#define MINESWEEPER_LABEL_CLASS "MineSweeperLabel"

#define MINESWEEPER_FACE_WAITING 	"o_O'"
#define MINESWEEPER_FACE_HAPPY 		"\\o/"
#define MINESWEEPER_FACE_ANGRY 		"/o\\"

#define MINESWEEPER_MODE_NORMAL		_("Normal")
#define MINESWEEPER_MODE_EXPERT		_("Expert")

.entity link;
.entity left, right, up, down;

// always square?
var float MineSweeper_Rows = 10;
var float MineSweeper_Columns = 10;
// set it in SetupGame
var float MineSweeper_CellsLeft = -100;
var float MineSweeper_MarkedMinesLeft = -10;

void MineSweeper_UpdateMarkedMineCount( entity me ) {
	strunzone( me.minesLeftButton.text );
	me.minesLeftButton.setText( me.minesLeftButton, strzone( ftos( MineSweeper_MarkedMinesLeft ) ) );
}

void MineSweeper_Uncover( entity me, entity item ) {
	// TODO: this is a small hack: we dont know whether item is a label or button
	// but in case that it is a button we would want to make its link (the label in that case) visible
	if( item.link != NULL ) {
		item.link.disabledAlpha = 1.0;
	}
	me.removeItem( me, item );
	remove( item );
}

// open up all cells on game over
void MineSweeper_MarkButton( entity button, string type ) {
	if( type == MINESWEEPER_BOMB_MARKER ) {
		button.colorL = '1 0 0';
		button.setText( button, MINESWEEPER_BOMB_MARKER );
	} else if( type == MINESWEEPER_MARKER ) {
		button.colorL = '0 1 0';
		button.setText( button, MINESWEEPER_MARKER );
	} else if( type == MINESWEEPER_UNMARKED ) {
		button.colorL = '1 1 1';
		button.setText( button, MINESWEEPER_UNMARKED );
	}
}

// only bombs if openAll !=
void MineSweeper_RemoveAll( entity me, float bombsOnly ) {
	entity lNode;
	entity lLast;
	lLast = NULL;
	for( lNode = me.firstChild ; lNode ;  ) {
		if( (!bombsOnly && (lNode.name == MINESWEEPER_LABEL_CLASS || lNode.name == MINESWEEPER_BUTTON_CLASS)) ||
			 (lNode.name == MINESWEEPER_BUTTON_CLASS && lNode.link.text == MINESWEEPER_BOMB) ) {
			MineSweeper_Uncover( me, lNode );
			if( lLast ) {
				lNode = lLast.nextSibling;
			} else {
				lNode = me.firstChild;
			}
			continue;
		}

		lLast = lNode;
		lNode = lNode.nextSibling;
	}
}

void MineSweeper_DisableAll( entity me ) {
	entity lNode;
	for( lNode = me.firstChild ; lNode ; lNode = lNode.nextSibling ) {
		if( lNode.name == MINESWEEPER_BUTTON_CLASS ) {
			lNode.disabled = true;
		}
	}
}

void MineSweeper_MarkBombs( entity me ) {
	entity lNode;
	for( lNode = me.firstChild ; lNode ; lNode = lNode.nextSibling ) {
		if( lNode.name == MINESWEEPER_BUTTON_CLASS && lNode.link.text == MINESWEEPER_BOMB ) {
			MineSweeper_MarkButton( lNode, MINESWEEPER_BOMB_MARKER );
		}
	}
}

void MineSweeper_SetFace( entity me, string face ) {
	me.gameFace.setText( me.gameFace, face );
}

// these functions should actually be members of MineSweeper (but they're not supposed to be overriden, so...)
void MineSweeper_GameWon( entity me ) {
	MineSweeper_MarkBombs( me );
	MineSweeper_DisableAll( me );
	MineSweeper_SetFace( me, MINESWEEPER_FACE_HAPPY );

	MineSweeper_MarkedMinesLeft = 0;
	MineSweeper_UpdateMarkedMineCount( me );
}

void MineSweeper_GameLost( entity me ) {
	MineSweeper_RemoveAll( me, true );
	MineSweeper_DisableAll( me );
	MineSweeper_SetFace( me, MINESWEEPER_FACE_ANGRY );
}

void MineSweeper_EmptyCellOpened( entity container ) {
	MineSweeper_CellsLeft = MineSweeper_CellsLeft - 1;
	// MineSweeper_Size == num mines in total
	if( MineSweeper_CellsLeft == MineSweeper_Rows ) {
		MineSweeper_GameWon( container );
	}
}

void MineSweeper_OpenCell( entity label ) {
	MineSweeper_Uncover( label.parent, label.link );

	// reset the link to mark it as opened
	label.link = NULL;

	MineSweeper_EmptyCellOpened( label.parent );
}

void MineSweeper_OpenEmptyCell( entity label ) {
	if( label == NULL || label.link == NULL /*|| label.link.text == MINESWEEPER_BOMB_MARKER*/ ) {
		// already visited or invalid (border cell) or not empty (or maked as bomb by the player - commented out because it makes it less fun..)
		return;
	}

	MineSweeper_OpenCell( label );

	if( label.text != MINESWEEPER_EMPTY ) {
		return;
	}

	MineSweeper_OpenEmptyCell( label.left );
	MineSweeper_OpenEmptyCell( label.right );
	MineSweeper_OpenEmptyCell( label.up );
	MineSweeper_OpenEmptyCell( label.down );

	MineSweeper_OpenEmptyCell( label.right.up );
	MineSweeper_OpenEmptyCell( label.left.up );
	MineSweeper_OpenEmptyCell( label.right.down );
	MineSweeper_OpenEmptyCell( label.left.down );
}

void MineSweeperButton_onClick( entity button, entity me ) {
	me.setFocus( me, NULL );

	if( button.text == MINESWEEPER_BOMB_MARKER ) {
		// dont do anything if it is marked as bomb!
		return;
	}

	if( button.link.text == MINESWEEPER_EMPTY ) {
		MineSweeper_OpenEmptyCell( button.link );
	} else if( button.link.text == MINESWEEPER_BOMB ) {
		MineSweeper_GameLost( me );
	} else {
		MineSweeper_OpenCell( button.link );
	}
}

void MineSweeperButton_onRightClick( entity button, entity me ) {
	// switch through possible texts
	string lText;
	lText = button.text;
	if( lText == MINESWEEPER_UNMARKED ) {
		MineSweeper_MarkedMinesLeft = MineSweeper_MarkedMinesLeft - 1;
		MineSweeper_UpdateMarkedMineCount( me );

		MineSweeper_MarkButton( button, MINESWEEPER_BOMB_MARKER );
	} else if( lText == MINESWEEPER_BOMB_MARKER ) {
		MineSweeper_MarkedMinesLeft = MineSweeper_MarkedMinesLeft + 1;
		MineSweeper_UpdateMarkedMineCount( me );

		MineSweeper_MarkButton( button, MINESWEEPER_MARKER );
	} else if( lText == MINESWEEPER_MARKER ) {
		MineSweeper_MarkButton( button, MINESWEEPER_UNMARKED );
	}
}

void MineSweeper_SetupGame( entity me )
{
	MineSweeper_CellsLeft = MineSweeper_Columns * MineSweeper_Rows;
	MineSweeper_MarkedMinesLeft = MineSweeper_Rows;

	MineSweeper_SetFace( me, MINESWEEPER_FACE_WAITING );
	MineSweeper_UpdateMarkedMineCount( me );

	float lRow, lColumn;
	float lPrevRowMine, lCurrentRowMine, lNextRowMine;
	entity lLastLabel, lFirstLabelInRow;
	lFirstLabelInRow = NULL;

	lPrevRowMine = -100; // dont care
	lCurrentRowMine = -100;
	lNextRowMine = floor( random() * MineSweeper_Columns );
	for( lRow = 0 ; lRow < MineSweeper_Rows ; lRow++ ) {
		lPrevRowMine = lCurrentRowMine;
		lCurrentRowMine = lNextRowMine;
		if( lRow < MineSweeper_Rows - 1 ) {
			lNextRowMine = floor( random() * MineSweeper_Columns );
		} else {
			lNextRowMine = -100;
		}

		for( lColumn = 0 ; lColumn < MineSweeper_Columns ; lColumn++ ) {
			string lText;
			vector lColor;
			if( lCurrentRowMine == lColumn ) {
				lColor = '1 0 0';
				lText = MINESWEEPER_BOMB;
			} else {
				lColor = '1 1 1';

				float lNumMines;
				lNumMines = 0;
				if( lPrevRowMine - 1 <= lColumn && lColumn <= lPrevRowMine + 1 ) {
					lNumMines = lNumMines + 1;
				}
				if( lCurrentRowMine - 1 <= lColumn && lColumn <= lCurrentRowMine + 1 ) {
					lNumMines = lNumMines + 1;
				}
				if( lNextRowMine - 1 <= lColumn && lColumn <= lNextRowMine + 1 ) {
					lNumMines = lNumMines + 1;
				}

				switch( lNumMines ) {
					case 0:
						lText = MINESWEEPER_EMPTY;
						break;
					case 1:
						lText = "1";
						break;
					case 2:
						lText = "2";
						break;
					case 3:
						lText = "3";
						break;
					default:
						error( "Bad lNumMines value!" );
				}
			}

			// IMPORTANT: Im switching lRow and lColumn here for a better aspect ratio!! Variable names havent been adapted yet though!
			me.gotoRC( me, lColumn + 2, lRow);
			entity lCellLabel;
			//lCellLabel = makeNexuizTextLabel( 0.5, lText );
			lCellLabel = makeNexuizButton( lText, '0 0 0' );
			lCellLabel.disabled = true;
			lCellLabel.colorD = '0.7 0.7 0.0';
			lCellLabel.disabledAlpha = 0.0;
			// tag used to remove it later
			lCellLabel.name = MINESWEEPER_LABEL_CLASS;
			lCellLabel.colorL = lColor;
			me.TD( me, 1, 1, lCellLabel );

			me.gotoRC( me, lColumn + 2, lRow );
			entity lCellButton;
			lCellButton = makeNexuizButton( "", '0 0 0' );
			// tag used to remove it later
			lCellButton.name = MINESWEEPER_BUTTON_CLASS;
			lCellButton.colorD = '0.2 0.2 1';
			lCellButton.disabledAlpha = 0.9;

			MineSweeper_MarkButton( lCellButton, MINESWEEPER_UNMARKED );

			lCellButton.onClick = MineSweeperButton_onClick;
			lCellButton.onRightClick = MineSweeperButton_onRightClick;
			lCellButton.onClickEntity = me;
			me.TD( me, 1, 1, lCellButton );

			// two way link
			lCellButton.link = lCellLabel;
			lCellLabel.link = lCellButton;

			// link the buttons
			if( lColumn == 0 ) {
				lCellLabel.up = lFirstLabelInRow;
				if( lFirstLabelInRow ) {
					lFirstLabelInRow.down = lCellLabel;
				}
				lFirstLabelInRow = lCellLabel;
			} else {
				lCellLabel.left = lLastLabel;
				lLastLabel.right = lCellLabel;
				if( lLastLabel.up ) {
					lCellLabel.up = lLastLabel.up.right;
					lCellLabel.up.down = lCellLabel;
				}
			}
			lLastLabel = lCellLabel;
		}
	}
}


void MineSweeper_GameFace_onClick( entity button, entity me ) {
	MineSweeper_RemoveAll( me, false );
	MineSweeper_SetupGame( me );

	// force a renotification of all items
	RenotifyAll();
}

void MineSweeper_ModeButton_onClick( entity button, entity me ) {
	// switch the text
	if( button.text == MINESWEEPER_MODE_NORMAL ) {
		button.setText( button, MINESWEEPER_MODE_EXPERT );
	} else {
		button.setText( button, MINESWEEPER_MODE_NORMAL );
	}

	// reinit the game

	MineSweeper_RemoveAll( me, false );
	me.removeItem( me, me.modeButton );
	me.removeItem( me, me.gameFace );
	me.removeItem( me, me.minesLeftButton );

	MineSweeper_Rows = 10 + mod( MineSweeper_Rows, 20 );
	// NOTE: Ive switched rows and columns but I havent updated the variable names yet!!
	me.intendedWidth = MineSweeper_Rows * 0.03;

	me.rows = MineSweeper_Columns + 2;
	me.columns = MineSweeper_Rows;
	me.setup( me );

	me.arrange( me );
	entity lParent;
	lParent = me.parent;
	lParent.removeItem( lParent, me );

	vector newSize;
	newSize = me.intendedWidth * eX + me.intendedHeight * eY;
	lParent.addItem( lParent, me, me.Container_origin /* + me.Container_size - newSize*/, newSize, SKINALPHAS_MAINMENU_z);

	// force a renotification of all items
	RenotifyAll();
}

void arrangeMineSweeper(entity me)
{
	me.gotoRC( me, 0, 0 );
	// IMPORTANT: switched rows and columns but I havent updated the var names yet!
	me.TD( me, 1, MineSweeper_Rows * 0.5, me.modeButton );
	me.TD( me, 1, MineSweeper_Rows * 0.2, me.gameFace );
	me.TD( me, 1, MineSweeper_Rows * 0.3, me.minesLeftButton );

	MineSweeper_SetupGame( me );
}

void fillMineSweeper(entity me)
{
	me.moveable = true;

	me.modeButton = makeNexuizButton( "Normal", '0 0 0' );
	me.modeButton.onClick = MineSweeper_ModeButton_onClick;
	me.modeButton.onClickEntity = me;

	me.gameFace = makeNexuizButton( "", '0 0 0' );
	me.gameFace.onClick = MineSweeper_GameFace_onClick;
	me.gameFace.onClickEntity = me;

	me.minesLeftButton = makeNexuizButton( strzone( "" ) , '0 0 0' );

	me.arrange( me );
}
#endif

// click. The C-word so you can grep for it.
