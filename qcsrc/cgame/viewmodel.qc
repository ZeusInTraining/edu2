vector viewmodel_ofs;

// The view model
entity viewmodel;

// Set up any things needed by the viewmodel stuff
void ViewModel_Init()
{
  viewmodel = spawn();
  viewmodel.classname = "viewmodel";
  viewmodel.drawmask = MASK_NORMAL;
  viewmodel.renderflags = RF_VIEWMODEL | RF_DEPTHHACK;
  viewmodel.solid = 0;
}

const float VMA_NONE 			= 0;
const float VMA_RAISE 			= 1;
const float VMA_LOWER 			= 2;

float viewModelAnim;

float viewModelLowerRaiseStart;
float viewModelLowerRaiseDist;
float viewModelLowerRaiseTime;

void ShowViewModelInstant (string mdl)
{
  viewModelAnim = VMA_NONE;
  setmodel (viewmodel, mdl);
}

void ShowViewModelRaise (string mdl, float raiseDist, float raiseTime)
{
  viewModelAnim = VMA_RAISE;
  viewModelLowerRaiseStart = time;
  viewModelLowerRaiseDist = raiseDist;
  viewModelLowerRaiseTime = raiseTime;
  setmodel (viewmodel, mdl);
}

void HideViewModelInstant ()
{
  viewModelAnim = VMA_NONE;
  setmodelindex (viewmodel, 0);
}

void HideViewModelLower (float lowerDist, float lowerTime)
{
  viewModelAnim = VMA_LOWER;
  viewModelLowerRaiseStart = time;
  viewModelLowerRaiseDist = lowerDist;
  viewModelLowerRaiseTime = lowerTime;
}

void ViewModel_DoRaise ()
{
	float moveAmt;
	moveAmt = min (1, 1 - (time - viewModelLowerRaiseStart) * (1.0 / viewModelLowerRaiseTime));
	if (moveAmt < 0)
	{
		viewModelAnim = VMA_NONE;
		viewmodel_ofs = '0 0 0';
	}
	else
	{
		viewmodel_ofs = '0 0 1' * viewModelLowerRaiseDist * -moveAmt;
	}
}

void ViewModel_DoLower ()
{
	float moveAmt;
	moveAmt = max (0, (time - viewModelLowerRaiseStart) * (1.0 / viewModelLowerRaiseTime));
	if (moveAmt > 1)
	{
		HideViewModelInstant ();
		viewmodel_ofs = '0 0 0';
	}
	else
	{
		viewmodel_ofs = '0 0 1' * viewModelLowerRaiseDist * -moveAmt;
	}
}

void ViewModel_Update()
{
	switch (viewModelAnim)
	{
	case VMA_NONE:
	  viewmodel_ofs = '0 0 0';
	  break;
	case VMA_RAISE:
	  ViewModel_DoRaise ();
	  break;
	case VMA_LOWER:
	  ViewModel_DoLower ();
	  break;
	}
		
	// Sync viewmodel with player position
	setorigin (viewmodel, viewer_org + viewmodel_ofs);
	viewmodel.angles = input_angles;
	// @@@ Curious
	viewmodel.angles_x = -viewmodel.angles_x;
}
