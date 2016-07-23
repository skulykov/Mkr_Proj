// FCNavGLTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "fcnav_win.h"


void test_fcnav() ;
void main_init();

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[] = _T("SampleApp");          // The title bar text
TCHAR szWindowClass[] = _T("SAMPLEAPP");    // the main window class name

//---------------Toolbar-----------------
#define TOOLBARCLASSNAME TEXT("OGL_Toolbar")
#define IDC_MAIN_TOOL 1234567


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
//	MSG msg;
//	//HACCEL hAccelTable;
//
//	
//	MyRegisterClass(hInstance);
//
//	// Perform application initialization:
//	if (!InitInstance (hInstance, nCmdShow))
//	{
//		return FALSE;
//	}
//
//
////	test_fcnav() ;
//
//	// Main message loop:
//	while (GetMessage(&msg, NULL, 0, 0))
//	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
	main_init();
	return 0;//(int) msg.wParam;
}

///
//  ESWindowProc()
//
//      Main window procedure
//
LRESULT WINAPI ESWindowProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{
   LRESULT  lRet = 1; 
   int wmId, wmEvent;
   switch (uMsg) 
   { 
      case WM_CREATE:
         break;

		
		 case WM_COMMAND:
			 wmId = LOWORD(wParam);
			 wmEvent = HIWORD(wParam);
			 // Parse the menu selections:
			 switch (wmId)
			 {
			 case ID_HELP_ABOUT:
				// DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				// MessageBox(NULL, TEXT("I am just trying my wedding dress"), NULL, NULL);
				 break;
			 case IDM_EXIT:
				 DestroyWindow(hWnd);
				 break;
			 default:
				 return DefWindowProc(hWnd, uMsg, wParam, lParam);
			 }
			 break;
      case WM_SIZE:
         {
            ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
            if ( esContext ) {
               esContext->width = LOWORD( lParam );
               esContext->height = HIWORD( lParam );
               InvalidateRect( esContext->hWnd, NULL, FALSE );
            }
         }

      case WM_PAINT:
         {
            /*  svk I commented out this code
			 
			 ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
            
            if ( esContext && esContext->drawFunc )
               esContext->drawFunc ( esContext );
            
            if ( esContext )
              ValidateRect( esContext->hWnd, NULL );
			  */
         }
         break;

      case WM_DESTROY:
         PostQuitMessage(0);             
         break; 
      
      case WM_CHAR:
         {
            POINT      point;
            ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
            
            GetCursorPos( &point );

            if ( esContext && esContext->keyFunc )
	            esContext->keyFunc ( esContext, (unsigned char) wParam, 
		                             (int) point.x, (int) point.y );
		}
         break;
		  case WM_LBUTTONDOWN: 
			  {
            ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
			if ( esContext && esContext->mouseFunc )
			  esContext->mouseFunc(esContext,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), kMouseDown_State);
			  }
            break;
                    
        case WM_MOUSEMOVE:
			{ 
			ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
			if ( esContext && esContext->mouseFunc )
			  esContext->mouseFunc(esContext,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), kMouseMoved_State);
			}
			break;

        case WM_LBUTTONUP:
			{ 
			ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
			if ( esContext && esContext->mouseFunc )
			  esContext->mouseFunc(esContext,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), kMouseUp_State);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{ 
			ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
			if ( esContext && esContext->mouseFunc )
			  esContext->mouseFunc(esContext,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), kMouseDblClick_State);
			}
			break;
		case WM_RBUTTONDBLCLK:
			{ 
			ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
			if ( esContext && esContext->mouseFunc )
			  esContext->mouseFunc(esContext,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), kMouseRightDblClick_State);
			}
			break;
		case WM_MOUSEWHEEL:
			{ 
				ESContext *esContext = (ESContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
				if ( esContext && esContext->mouseFunc )
				{
					int fwKeys = GET_KEYSTATE_WPARAM(wParam);
					int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

					esContext->mouseFunc(esContext,fwKeys, zDelta, kMouseWheel_State);
				}
			}
			break;
         
      default: 
         lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
         break; 
   } 

   return lRet; 
}



GLboolean WinCreate ( ESContext *esContext, LPCTSTR title )
{
   WNDCLASS wndclass = {0}; 
   DWORD    wStyle   = 0;
   RECT     windowRect;
   HINSTANCE hInstance = GetModuleHandle(NULL);
   
   wndclass.style         = CS_OWNDC|CS_DBLCLKS;
   wndclass.lpfnWndProc   = (WNDPROC)ESWindowProc; 
   wndclass.hInstance     = hInstance; 
   wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
   wndclass.lpszClassName = TEXT("opengles2.0");
   wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

   if (!RegisterClass(&wndclass))
   {
	   return FALSE;
   }
   wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION | WS_SIZEBOX;
   
   // Adjust the window rectangle so that the client area has
   // the correct number of pixels
   windowRect.left = 0;
   windowRect.top = 0;
   windowRect.right = esContext->width;
   windowRect.bottom = esContext->height;

   AdjustWindowRect ( &windowRect, wStyle, FALSE );

   esContext->hWnd = CreateWindow(
                         TEXT("opengles2.0"),
                         title,
                         wStyle,
                         0,
                         0,
                         windowRect.right - windowRect.left,
                         windowRect.bottom - windowRect.top,
                         NULL,
                         NULL,
                         hInstance,
                         NULL);

   // Set the ESContext* to the GWL_USERDATA so that it is available to the 
   // ESWindowProc
   SetWindowLongPtr (  esContext->hWnd, GWL_USERDATA, (LONG) (LONG_PTR) esContext );

   if ( esContext->hWnd == NULL )
      return GL_FALSE;

   //======================Create Toolbar===================================
   //http://www.winprog.org/tutorial/app_three.html
  // esContext->hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
	//   esContext->hWnd, (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);
   // Send the TB_BUTTONSTRUCTSIZE message, which is required for
   // backward compatibility.
  // SendMessage(esContext->hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);




   ShowWindow ( esContext->hWnd, TRUE );

   return GL_TRUE;
}


#if NOT_USED
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FCNAVGLTEST));
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_FCNAVGLTEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_HELP_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
#endif
void WinLoop ( ESContext *esContext )
{
   MSG msg = { 0 };
   int done = 0;
   DWORD lastTime = GetTickCount();
   
   while (!done)
   {
      int gotMsg = (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0);
      DWORD curTime = GetTickCount();
      float deltaTime = (float)( curTime - lastTime ) / 1000.0f;
      lastTime = curTime;

      if ( gotMsg )
      {
         if (msg.message==WM_QUIT)
         {
             done=1; 
         }
         else
         {
             TranslateMessage(&msg); 
             DispatchMessage(&msg); 
         }
      }
      else
         SendMessage( esContext->hWnd, WM_PAINT, 0, 0 );

      // Call update function if registered
      if ( esContext->updateFunc != NULL )
         esContext->updateFunc ( esContext, deltaTime );
   }
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
