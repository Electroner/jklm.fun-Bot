#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <utility>
#include <windows.h>

using namespace std;

std::string browse(HWND hwnd)
{
	std::string path(MAX_PATH, '\0');
	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter =
		"Text files (*.txt)\0*.txt\0"
		"All files\0*.*\0";
	ofn.lpstrFile = &path[0];
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		path.resize(strlen(path.c_str()));
	}
	return path;
}

void writeWord(std::string _word) {
	//print the string with the keyboard
	for (int i = 0; i < _word.length(); i++) {
		keybd_event((BYTE)VkKeyScan(_word[i]), 0, 0, 0);
		keybd_event((BYTE)VkKeyScan(_word[i]), 0, KEYEVENTF_KEYUP, 0);
		//delay between each letter
		Sleep(30);
	}
	//Hit enter
	keybd_event((BYTE)VkKeyScan('\r'), 0, 0, 0);
	keybd_event((BYTE)VkKeyScan('\r'), 0, KEYEVENTF_KEYUP, 0);
}

//make a function that returns the color of the pixel at the given coordinates
COLORREF getPixelColor(int x, int y) {
	HDC hdc = GetDC(NULL);
	COLORREF color = GetPixel(hdc, x, y);
	ReleaseDC(NULL, hdc);
	return color;
}

int main()
{
	vector<string> words;
	vector<string> usedwords;
	string filename;

	cout << "CHOOSE A DICTIONARY..." << endl;

	filename = browse(0);
	std::replace(filename.begin(), filename.end(), '\\', '/');

	if (filename.size() == 0) {
		cout << "Error opening path. Exiting" << endl;
		exit(1);
	}
	cout << "Dictionary Selected:" << filename << endl << endl;
	ifstream file(filename);
	string line;

	if (file.fail()) {
		cout << "Error opening file. Exiting" << endl;
		exit(2);
	}

	while (!file.eof())
	{
		getline(file, line);
		words.push_back(line);
	}
	file.close();

	//Short the vector by length the longest word first
	sort(words.begin(), words.end(), [](const string& a, const string& b) {
		return a.size() < b.size();
	});

	std::pair<int, int> mousePos;
	std::pair<int, int> mousePoslast;
	std::pair<int, int> mousePosColor = { 0, 0 };
	
	bool mouseNoSelected = true;
	bool mouseNoSelectedColor = true;

	LPPOINT lpPoint = new tagPOINT;
	HANDLE hData;
	string text;
	COLORREF color;

	bool onetime = true;

	bool downCtrl = false;
	bool downAlt = false;
	bool downShift = false;

	bool releasedCtrl = false;
	bool releasedAlt = false;
	bool releasedShift = false;

	bool myturn = false;

	cout << "READY" << endl;

	while (onetime)
	{
		//If the Ctrl key is released
		if (GetAsyncKeyState(VK_CONTROL) < 0)
		{
			if (!downCtrl) {
				downCtrl = true;
			}
		}
		else
		{
			if (downCtrl)
			{
				downCtrl = false;
				//Released
				releasedCtrl = true;
			}
		}

		//If the Alt key is released
		if (GetAsyncKeyState(VK_MENU) < 0)
		{
			if (!downAlt) {
				downAlt = true;
			}
		}
		else
		{
			if (downAlt)
			{
				downAlt = false;
				//Released
				releasedAlt = true;
			}
		}

		//If the Shift key is released
		if (GetAsyncKeyState(VK_SHIFT) < 0)
		{
			if (!downShift) {
				downShift = true;
			}
		}
		else
		{
			if (downShift)
			{
				downShift = false;
				//Released
				releasedShift = true;
			}
		}

		if (releasedShift) {
			onetime = false;
		}

		if (releasedAlt)
		{
			GetCursorPos(lpPoint);
			mousePos.first = lpPoint->x;
			mousePos.second = lpPoint->y;
			mouseNoSelected = false;
			releasedAlt = false;
			
			cout << "Letters X: " << mousePos.first << " Letters Y: " << mousePos.second << endl;
		}
		
		if (releasedCtrl) {
			//Set the mousePosColor to the current mouse position
			GetCursorPos(lpPoint);
			mousePosColor.first = lpPoint->x;
			mousePosColor.second = lpPoint->y;
			mouseNoSelectedColor = false;
			releasedCtrl = false;

			cout << "Color X: " << mousePosColor.first << " Color Y: " << mousePosColor.second << endl;
		}

		//Get the color of the pixel at the mouse position
		color = getPixelColor(mousePosColor.first, mousePosColor.second);

		//If the color is yellow (R and G are more than 200)
		if (GetRValue(color) >= 180 && GetGValue(color) >= 180) {
			myturn = true;
			cout << "MY TURN" << endl;
		}
		else {
			myturn = false;
		}
		
		//Set delay
		Sleep(100);

		//When the ctrl is released 
		if (myturn && !mouseNoSelected && !mouseNoSelectedColor)
		{
			//Get current mouse position
			GetCursorPos(lpPoint);
			mousePoslast.first = lpPoint->x;
			mousePoslast.second = lpPoint->y;

			//Move the mouse to the position
			SetCursorPos(mousePos.first, mousePos.second);

			//Double click on the mouse position
			mouse_event(MOUSEEVENTF_LEFTDOWN, mousePos.first, mousePos.second, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, mousePos.first, mousePos.second, 0, 0);
			Sleep(20);
			mouse_event(MOUSEEVENTF_LEFTDOWN, mousePos.first, mousePos.second, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, mousePos.first, mousePos.second, 0, 0);

			//Make Ctrl+C
			keybd_event(VK_CONTROL, 0x9d, KEYEVENTF_EXTENDEDKEY | 0, 0);
			keybd_event(0x43, 0x9d, KEYEVENTF_EXTENDEDKEY | 0, 0);
			keybd_event(0x43, 0x9d, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			keybd_event(VK_CONTROL, 0x9d, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

			Sleep(75);

			//Go back to the last position
			SetCursorPos(mousePoslast.first, mousePoslast.second);

			//Click on the mouse position
			mouse_event(MOUSEEVENTF_LEFTDOWN, mousePoslast.first, mousePoslast.second, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, mousePoslast.first, mousePoslast.second, 0, 0);

			if (!OpenClipboard(NULL)) {
				std::cout << "Error opening clipboard" << std::endl;
				CloseClipboard();
				return false;
			}
			//Get the text from the clipboard
			hData = GetClipboardData(CF_TEXT);
			if (hData == NULL) {
				std::cout << "Error getting clipboard data" << std::endl;
				CloseClipboard();
				return false;
			}
			//Close clipboard
			CloseClipboard();
			//Save the text in a string
			text = (char*)hData;
			//To upper case
			std::transform(text.begin(), text.end(), text.begin(), ::toupper);

			myturn = false;

			//If the text isnt empty
			if (text.size() > 0) {
				cout << "Text: " << text << endl;
				//Search in the dictionary the word that contains the text
				int i = 0;
				bool found = false;
				while (i < words.size() && !found) {
					if (words[i].find(text) != std::string::npos) {
						cout << "Word found: " << words[i] << endl;
						//If the word is found and doesnt exist on the usadas vector, write it
						int k = 0;
						int found_used = false;
						while (k < usedwords.size() && !found_used) {
							if (usedwords[k] == words[i]) {
								found_used = true;
							}
							k++;
						}
						if (!found_used) {
							writeWord(words[i]);
							found = true;
							usedwords.push_back(words[i]);
						}
					}
					i++;
				}
			}
		}
	}

	//free lpPoint
	delete lpPoint;

	return 0;
}
