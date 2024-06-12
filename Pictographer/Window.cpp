#include "Window.h"

// the window message callback
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// gets our window class
	Window* MainWindow = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	// checks the recieved message
	switch (uMsg)
	{
	case WM_CREATE:
		// if the window is being created
		// sets the address of our class to be used in the message handler function
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
		// breaks out of the switch
		break;
	case WM_DESTROY:
		// if the message is to destory the window
		// checks the close routine
		if (MainWindow->pCloseRoutine)
		{
			// calls the routine
			MainWindow->pCloseRoutine();
		}
		// breaks out of the switch
		break;
	case WM_COMMAND:
		// checks the notification code
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			// if the button was clicked
			// gets the button by it's id
			Button* button = MainWindow->buttons->GetButton(LOWORD(wParam));
			// checks if the button exists
			if (button)
			{
				// checks if a callback exists
				if (button->pCallback)
				{
					// calls the callback
					button->pCallback();
				}
			}
		}
		// breaks out of the switch
		break;
	case WM_PAINT:
		// if the window wants to paint
		// checks the background image
		if (MainWindow->BackgroundImage)
		{
			// draws the background image
			MainWindow->graphics->DrawImage(MainWindow->BackgroundImage, RectF(0, 0, MainWindow->Width, MainWindow->Height));
		}
		// iterates through the pictures
		// stores the current picture
		PLIST_ENTRY CurrentButton = &MainWindow->pictures->PictureList;
		// iterates through the pictures
		while (CurrentButton->Flink != &MainWindow->pictures->PictureList)
		{
			// gets the next entry
			CurrentButton = CurrentButton->Flink;
			// typecasts the entry to a picture
			Picture* image = (Picture*)CurrentButton;
			// draws the image
			MainWindow->graphics->DrawImage(image->ImageObj, RectF(image->x, image->y, image->Width, image->Height));
			// draws the image border
			Pen pen(Color(255, 0, 255, 0));
			MainWindow->graphics->DrawLine(&pen, image->x, image->y, image->x + image->Width, image->y);
			MainWindow->graphics->DrawLine(&pen, image->x, image->y + image->Height, image->x + image->Width, image->y + image->Height);
			MainWindow->graphics->DrawLine(&pen, image->x, image->y, image->x, image->y + image->Height);
			MainWindow->graphics->DrawLine(&pen, image->x + image->Width, image->y, image->x + image->Width, image->y + image->Height);
		}
		// breaks out of the switch
		break;
	}
	// lets windows handle the message
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// creates a new window instance
Window::Window(HINSTANCE hInstance, LPCWSTR lpWindowTitle, INT Width, INT Height, HBRUSH hbrBackground)
{
	// creates the window class structure
	WNDCLASSEX WindowClass;
	// zeros out the structure
	ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));
	// sets the size of the structure
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	// sets the window to redraw if the size changes or the window is moved
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	// sets our window message callback
	WindowClass.lpfnWndProc = Window::WindowProc;
	// sets the number of extra bytes to allocate following the window class
	WindowClass.cbClsExtra = 0;
	// sets the number of extra bytes to allocate following the window instance
	WindowClass.cbWndExtra = 0;
	// sets the instance
	WindowClass.hInstance = hInstance;
	// sets the window icon
	WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// sets the window cursor
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// sets the background brush
	WindowClass.hbrBackground = hbrBackground;
	// sets the menu name
	WindowClass.lpszMenuName = NULL;
	// sets the window name
	WindowClass.lpszClassName = lpWindowTitle;
	// sets the window small icon
	WindowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	// registers the window class
	if (RegisterClassEx(&WindowClass))
	{
		// saves the window's width
		this->Width = Width;
		// saves the window's height
		this->Height = Height;
		// creates the window for our application
		hWindow = CreateWindow(lpWindowTitle, lpWindowTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, hInstance, this);
		// unregisters the window class
		UnregisterClass(lpWindowTitle, hInstance);
		// stores the gdi+ startup input
		GdiplusStartupInput gdiStartup;
		// initalizes gdi+
		GdiplusStartup(&gdiToken, &gdiStartup, NULL);
		// shows the window
		// ShowWindow(hWindow, SW_SHOWNORMAL);
		// forces an update
		// UpdateWindow(hWindow);
		// sets up the button list
		buttons = new Buttons();
		// sets up the picture list
		pictures = new Pictures();
		// creates the graphics object
		graphics = new Graphics(GetDC(hWindow));
	}
}

// destroys the window object
Window::~Window()
{
	// destroys the window
	DestroyWindow(hWindow);
	// shutsdown gdi+
	GdiplusShutdown(gdiToken);
	// calls the buttons destructor
	delete buttons;
}

// sets the window close routine
VOID Window::SetCloseRoutine(PVOID pCallback)
{
	// sets the close routine
	pCloseRoutine = (Callback)pCallback;
}

// sets the window background image
VOID Window::SetBackgroundImage(LPCWSTR lpImagePath)
{
	// sets the background image
	BackgroundImage = new Image(lpImagePath);
	// paints the window
	InvalidateRect(hWindow, NULL, FALSE);
	UpdateWindow(hWindow);
}

// sets the window state
BOOL Window::SetState(INT nState)
{
	// shows the window
	return ShowWindow(hWindow, nState);
}

// forces a window update
BOOL Window::Update()
{
	// updates the window
	return UpdateWindow(hWindow);
}

// sets the window title
BOOL Window::SetTitle(LPCWSTR lpTitle)
{
	// sets the window text
	return SetWindowText(hWindow, lpTitle);
}

// adds a button to the window
Button* Window::AddButton(LPCWSTR lpButtonText, INT x, INT y, INT Width, INT Height, PVOID pCallback)
{
	// generates a button id
	INT ButtonId = rand();
	// creates a new button
	Button* NewButton = new Button(hWindow, rand(), lpButtonText, x, y, Width, Height, pCallback);
	// adds the button to the buttons list
	buttons->AddButton(NewButton);
	// returns the button object
	return NewButton;
}

// creates a user defined message box
INT Window::MessagePrompt(LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	// creates a message box
	return MessageBox(hWindow, lpText, lpCaption, uType);
}

// opens an open file dialog
LPCWSTR Window::OpenFileDialog(LPCWSTR lpstrFilter, DWORD nFilterIndex, DWORD Flags)
{
	// creates the file struct
	OPENFILENAME OpenFileName;
	// creates the buffer for the file
	LPWSTR szFilePath = new WCHAR[MAX_PATH];
	// zeros out the structure
	ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));
	// sets the structure size
	OpenFileName.lStructSize = sizeof(OPENFILENAME);
	// sets the parent window
	OpenFileName.hwndOwner = hWindow;
	// sets the buffer for the file path
	OpenFileName.lpstrFile = szFilePath;
	// nulls the first byte because windows wants us to
	OpenFileName.lpstrFile[0] = '\0';
	// sets the max file path size
	OpenFileName.nMaxFile = MAX_PATH;
	// sets the filter for the dialog
	OpenFileName.lpstrFilter = lpstrFilter;
	// sets the filter index
	OpenFileName.nFilterIndex = nFilterIndex;
	// sets the file title
	OpenFileName.lpstrFileTitle = NULL;
	// sets the max file title size
	OpenFileName.nMaxFileTitle = 0;
	// sets the initial directory of the dialog
	OpenFileName.lpstrInitialDir = NULL;
	// sets the flags for the dialog
	OpenFileName.Flags = Flags;
	// gets the file path and checks if valid
	if (GetOpenFileName(&OpenFileName))
	{
		// returns the string
		return szFilePath;
	}
	// else we return null
	return NULL;
}

// adds a picture widget to the window
Picture* Window::AddPicture(LPCWSTR ImagePath, INT x, INT y, INT Width, INT Height)
{
	// creates a new picture object
	Picture* PictureObj = new Picture(ImagePath, x, y, Width, Height);
	// adds the picture object to the linked list
	pictures->AddPicture(PictureObj);
	// creates the rect to update
	RECT lpRect = RECT();
	lpRect.left = x;
	lpRect.top = y;
	lpRect.right = x + Width;
	lpRect.bottom = y + Height;
	// paints the window
	InvalidateRect(hWindow, &lpRect, FALSE);
	UpdateWindow(hWindow);
	// returns a pointer to the picture object
	return PictureObj;
}

// removes the picture object from the window
VOID Window::RemovePicture(Picture* PictureObj)
{
	// if the picture object exists
	if (PictureObj)
	{
		// removes the picture from the pictures linked list
		pictures->RemovePicture(PictureObj);
		// creates the rect to update
		RECT lpRect = RECT();
		lpRect.left = PictureObj->x;
		lpRect.top = PictureObj->y;
		lpRect.right = PictureObj->x + PictureObj->Width;
		lpRect.bottom = PictureObj->y + PictureObj->Height;
		// deletes the picture object
		delete PictureObj;
		// paints the window
		InvalidateRect(hWindow, &lpRect, FALSE);
		UpdateWindow(hWindow);
	}
}

// runs the main loop of the window
VOID Window::MainLoop()
{
	// enters the main window loop
	// stores our message
	MSG WindowMessage;
	// deals with any pending messages
	while (GetMessage(&WindowMessage, hWindow, 0, 0))
	{
		// translates the message
		TranslateMessage(&WindowMessage);
		// deals with the message
		DispatchMessage(&WindowMessage);
	}
}

// creates a button object
Button::Button(HWND hWindow, INT ButtonId, LPCWSTR lpButtonText, INT x, INT y, INT Width, INT Height, PVOID pProcedure)
{
	// sets the button id
	Id = ButtonId;
	// sets the button handle
	hButton = CreateWindow(L"BUTTON", lpButtonText, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, x, y, Width, Height, hWindow, (HMENU)ButtonId, (HINSTANCE)GetWindowLongPtr(hWindow, GWLP_HINSTANCE), NULL);
	// sets the button callback
	pCallback = (Callback)pProcedure;
}

// destroys the button object
Button::~Button()
{
	// destroys the button associated with this object
	DestroyWindow(hButton);
}

// creates a new linked list of buttons
Buttons::Buttons()
{
	// initalizes the buttons list head
	// sets the flink and blink to the list head
	ButtonList.Flink = ButtonList.Blink = &ButtonList;
}

// destroys the buttons object
Buttons::~Buttons()
{
	// iterates through buttons and call their destructors
	// then remove every button from the list by calling RemoveButton() on them
}

// initalizes the buttons list head
VOID Buttons::InitalizeHead()
{
	// sets the flink and blink to the list head
	ButtonList.Flink = ButtonList.Blink = &ButtonList;
}

// returns a pointer to the button with the given id
Button* Buttons::GetButton(INT ButtonId)
{
	// iterates through the button list
	// stores the current button
	PLIST_ENTRY CurrentButton = &ButtonList;
	// iterates through the buttons
	while (CurrentButton->Flink != &ButtonList)
	{
		// typecasts the entry to a button
		Button* button = (Button*)CurrentButton;
		// checks the button id
		if (ButtonId == button->Id)
		{
			// returns the button object pointer
			return button;
		}
		// gets the next entry
		CurrentButton = CurrentButton->Flink;
	}
	// else we return null
	return NULL;
}

// adds a button to the buttons linked list
VOID Buttons::AddButton(Button* pButton)
{
	// gets to the end of the button list stored in the window
	// sets the flink address
	pButton->NextButton.Flink = &ButtonList;
	// sets the blink of the list head to the newest entry
	ButtonList.Blink = &pButton->NextButton;
	// stores the current button
	PLIST_ENTRY CurrentButton = &ButtonList;
	// iterates through the buttons
	while (CurrentButton->Flink != &ButtonList)
	{
		// gets the next entry
		CurrentButton = CurrentButton->Flink;
	}
	// sets the blink address
	pButton->NextButton.Blink = CurrentButton;
	// sets the flink address of the current button
	CurrentButton->Flink = &pButton->NextButton;
}

// removes a button from the buttons linked list
VOID Buttons::RemoveButton(INT ButtonId)
{

}

// creates a new picture object
Picture::Picture(LPCWSTR ImagePath, INT x, INT y, INT Width, INT Height)
{
	// sets up the class attributes
	this->ImageObj = new Image(ImagePath);
	this->x = x;
	this->y = y;
	this->Width = Width;
	this->Height = Height;
}

// destroys the picture object
Picture::~Picture()
{
	// deletes the image object
	delete ImageObj;
}

// creates a new linked list of pictures
Pictures::Pictures()
{
	// initalizes the pictures list head
	// sets the flink and blink to the list head
	PictureList.Flink = PictureList.Blink = &PictureList;
}

// adds a picture object to the picture list
VOID Pictures::AddPicture(Picture* pPicture)
{
	// gets to the end of the button list stored in the window
	// sets the flink address
	pPicture->NextPicture.Flink = &PictureList;
	// sets the blink of the list head to the newest entry
	PictureList.Blink = &pPicture->NextPicture;
	// stores the current button
	PLIST_ENTRY CurrentButton = &PictureList;
	// iterates through the buttons
	while (CurrentButton->Flink != &PictureList)
	{
		// gets the next entry
		CurrentButton = CurrentButton->Flink;
	}
	// sets the blink address
	pPicture->NextPicture.Blink = CurrentButton;
	// sets the flink address of the current button
	CurrentButton->Flink = &pPicture->NextPicture;
	// increments the picture count
	PictureCount += 1;
}

// removes a picture from the picture list
BOOL Pictures::RemovePicture(Picture* pPicture)
{
	// stores the current button
	PLIST_ENTRY CurrentButton = &PictureList;
	// iterates through the buttons
	while (CurrentButton->Flink != &PictureList)
	{
		// gets the next entry
		CurrentButton = CurrentButton->Flink;
		// checks if the object is the same
		if ((LPBYTE)CurrentButton == (LPBYTE)pPicture)
		{
			// sets the flink of the previous object to the one after this object
			CurrentButton->Blink->Flink = CurrentButton->Flink;
			// sets the blink of the next object to the previous of this object
			CurrentButton->Flink->Blink = CurrentButton->Blink;
			// decrements the picture count
			PictureCount -= 1;
			// breaks out of the loop
			return TRUE;
		}
	}
	// else we return false
	return FALSE;
}

// TODO

// add button creation to the button class constructor

// allow buttons to keep track of their parent window class
// for example: Window* Button::GetParent() { return ParentWindow }

// clean up classes and memory upon class destruction

// think about registering the window class and creating the window in different class functions
// that way a boolean can be returned concerning the status of the function

// point the button message handlers to their defined callbacks
// either by using SetWindowLongPtr() or SetWindowSubclass()

// lable and lables class that is iterated and displayed on WM_PAINT

// make iteration easier via functions like Next() and IsEnd()
// add count to linked list objects