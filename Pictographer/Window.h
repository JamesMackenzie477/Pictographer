// includes the windows api header file
#include <Windows.h>
// includes gdi+ for graphics
#include <gdiplus.h>

using namespace Gdiplus;

typedef VOID(__stdcall * Callback)();

// defines the picture class
// used to store picture attributes
class Picture
{
public:
	LIST_ENTRY NextPicture;
	Image* ImageObj;
	INT x;
	INT y;
	INT Width;
	INT Height;
	// defines the class constructor
	Picture(LPCWSTR ImagePath, INT x, INT y, INT Width, INT Height);
	// defines the class destructor
	~Picture();
};

// defines the pictures class
// used to keep track and interact with the pictures associated with a window
class Pictures
{
public:
	// stores the window pictures
	LIST_ENTRY PictureList;
	// stores the count of the pictures
	DWORD PictureCount = 0;
	// defines the class constructor
	Pictures();
	// defines the class destructor
	// ~Pictures();
	// adds a picture widget to the linked list
	VOID AddPicture(Picture* pPicture);
	// removes a picture from the linked list
	BOOL RemovePicture(Picture* pPicture);
};

// defines the button class
// used to store button attributes
class Button
{
public:
	LIST_ENTRY NextButton;
	INT Id;
	HWND hButton;
	Callback pCallback;
	// defines the class constructor
	Button(HWND hWindow, INT Id, LPCWSTR lpButtonText, INT x, INT y, INT Width, INT Height, PVOID pCallback);
	// defines the class destructor
	~Button();
};

// defines the buttons class
// used to keep track and interact with the buttons associated with a window
class Buttons
{
private:
	// stores the window buttons
	LIST_ENTRY ButtonList;
public:
	// defines the class constructor
	Buttons();
	// defines the class destructor
	~Buttons();
	VOID InitalizeHead();
	Button* GetButton(INT ButtonId);
	VOID AddButton(Button* pButton);
	VOID RemoveButton(INT ButtonId);
};

// defines the window class
// creates an allows manipulation of a window
class Window
{
private:
	// stores the window handle
	HWND hWindow;
	// stores the gdi+ token
	ULONG_PTR gdiToken;
	// stores the window buttons
	Buttons* buttons;
	// stores the window pictures
	Pictures* pictures;
	// stores the window background image
	Image* BackgroundImage = NULL;
	// stores the graphics object for drawing
	Graphics* graphics;
public:
	// stores the window close routine
	Callback pCloseRoutine = NULL;
	// stores the window's width
	INT Width = 0;
	// stores the window's height
	INT Height = 0;
	// defines the public functions and variables of the class
	// defines the class constructor
	Window(HINSTANCE hInstance, LPCWSTR lpWindowTitle, INT Width, INT Height, HBRUSH hbrBackground);
	// defines the class destructor
	~Window();
	VOID SetCloseRoutine(PVOID pCallback);
	// defines the window message procedure
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	VOID SetBackgroundImage(LPCWSTR lpImagePath);
	BOOL SetState(INT nState);
	BOOL Update();
	BOOL SetTitle(LPCWSTR lpTitle);
	Button* AddButton(LPCWSTR lpButtonText, INT x, INT y, INT Width, INT Height, PVOID pCallback);
	INT MessagePrompt(LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
	LPCWSTR OpenFileDialog(LPCWSTR lpstrFilter, DWORD nFilterIndex, DWORD Flags);
	Picture* AddPicture(LPCWSTR ImagePath, INT x, INT y, INT Width, INT Height);
	VOID RemovePicture(Picture* PictureObj);
	VOID MainLoop();
};