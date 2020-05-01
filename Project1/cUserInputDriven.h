#pragma once
#include "jComponent.h"
#include "junks.h"

enum class EventPriority
{
	Highest, High, Mid, Low, Lowest
};
enum class EventResult
{
	StopEvent, TransferEvent
};
enum class MouseButtonType
{
	None = -1, Left = 0, Right = 1, Center = 2
};

struct InputEventArgs
{
	Vector2n pt;
	Vector2n deltaPt;
	Vector3 terrainPt;
	MouseButtonType type;
	jGameObject* hoveredObject;
	int delta;
	char key;
	InputEventArgs() { memset(this, 0x00, sizeof(InputEventArgs)); }
};

class cUserInputDriven :
	public jComponent
{
public:
	cUserInputDriven();
	~cUserInputDriven();

	function<void(void)> EventHoverEnter;
	function<void(void)> EventHoverLeave;
	function<void(void)> EventSelected;
	function<void(void)> EventUnSelected;
	function<void(char)> EventKeyDownObject;

	function<EventResult(InputEventArgs)> EventMouseDown;
	function<EventResult(InputEventArgs)> EventMouseUp;
	function<EventResult(InputEventArgs)> EventMouseMove;
	function<EventResult(InputEventArgs)> EventMouseClick;
	function<EventResult(InputEventArgs)> EventMouseDrag;
	function<EventResult(InputEventArgs)> EventMouseWheel;
	function<EventResult(InputEventArgs)> EventKeyDown;
	function<EventResult(InputEventArgs)> EventKeyUp;
	function<EventResult(InputEventArgs)> EventKeyPressed;

protected:
	virtual void OnLoad();

	Property_GetSetter(EventPriority, Priority, EventPriority::Mid)
};

