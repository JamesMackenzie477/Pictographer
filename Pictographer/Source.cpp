// includes the window file
#include "Window.h"

// defines the window settings
#define WINDOW_TITLE L"Pictographer"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

// stores the main window globally so it can be accessed by other threads
Window* MainWindow;
Picture* CurrentImage = NULL;

// defines the close button callback
VOID CALLBACK ButtonClose()
{
	// sets the windows title
	MainWindow->SetTitle(WINDOW_TITLE);
	// removes the picture
	MainWindow->RemovePicture(CurrentImage);
	// sets the current image to empty
	CurrentImage = NULL;
}

// defines the open button callback
VOID CALLBACK ButtonOpen()
{
	// open a file dialog for the user
	LPCWSTR FilePath = MainWindow->OpenFileDialog(L"Image Files (*.PNG;*.JPG;*.GIF)\0*.PNG;*.JPG;*.GIF\0", NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
	// if the dialog has returned a path (if the user opened a file)
	if (FilePath)
	{
		// if a current image already exists
		if (CurrentImage)
		{
			// remove the image
			ButtonClose();
		}
		// sets the windows title
		MainWindow->SetTitle(FilePath);
		// creates a message prompt
		// MainWindow->MessagePrompt(FilePath, L"You have selected:", NULL);
		// adds the image to the window
		CurrentImage = MainWindow->AddPicture(FilePath, 125, 10, 300, (WINDOW_HEIGHT / 3));
		// CurrentImage = MainWindow->AddPicture(FilePath, 125 + 300 + 10, 10, 300, (WINDOW_HEIGHT / 3));
	}
}

// defines the exit button callback
VOID CALLBACK ButtonExit()
{
	// prompts the user
	if (MainWindow->MessagePrompt(L"Are you sure you wish to exit?", L"Exit Program", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		// exits the program
		exit(0);
	}
}

// the main entry point of this program
INT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	// creates our default window
	MainWindow = new Window(hInstance, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, WHITE_BRUSH);
	// sets the window close routine
	MainWindow->SetCloseRoutine(ButtonExit);
	// shows the window
	MainWindow->SetState(nCmdShow);
	// sets the window background image
	MainWindow->SetBackgroundImage(L"D:\\Downloads\\vgqEbKB.jpg");
	// forces a window update
	// MainWindow->Update();
	// adds the window buttons
	MainWindow->AddButton(L"Open", 10, 10, 100, 30, ButtonOpen);
	MainWindow->AddButton(L"Close", 10, 50, 100, 30, ButtonClose);
	MainWindow->AddButton(L"Copy", 10, WINDOW_HEIGHT - 80, 100, 30, NULL);
	MainWindow->AddButton(L"Invert", 120, WINDOW_HEIGHT - 80, 100, 30, NULL);
	MainWindow->AddButton(L"Exit", WINDOW_WIDTH - 125, WINDOW_HEIGHT - 80, 100, 30, ButtonExit);
	MainWindow->AddButton(L"Blur", 230, WINDOW_HEIGHT - 80, 100, 30, NULL);
	// runs the window's main loop
	MainWindow->MainLoop();
}