#include "keyboard.h"
#include "core.h"

Keyboard::Keyboard()
{
	bInfo = false;
}

void Keyboard::Events(XKeyEvent *pXKeyEvent)
{
	char Buf;
	KeySym  Key;

	switch(pXKeyEvent->type)
	{
		case KeyRelease:																									break;
		case KeyPress:
		if(XLookupString(pXKeyEvent, &Buf, 1, &Key, NULL))
		{
			if(bInfo)
				p->Info("Key Pressed = '%c' (%d) Shift = (%d) Alt = (%d) Control = (%d)\n", Buf, Buf,
					pXKeyEvent->state & ShiftMask, pXKeyEvent->state & Mod1Mask, pXKeyEvent->state & ControlMask);
		}
		// Alt Only
		if((pXKeyEvent->state & Mod1Mask) && !(pXKeyEvent->state & ControlMask))
		{

		}
		// Control Only
		if(!(pXKeyEvent->state & Mod1Mask) && (pXKeyEvent->state & ControlMask))
		{

		}

		// Control + Alt
		if((pXKeyEvent->state & Mod1Mask) && (pXKeyEvent->state & ControlMask))
		{

		}
		// Neither Control or Alt
		if(!(pXKeyEvent->state & Mod1Mask) && !(pXKeyEvent->state & ControlMask))
		{
			switch(XLookupKeysym(pXKeyEvent, 0))
			{
				case XK_Return:
				p->pRend->pQuadrix->Return();
				break;

				case XK_BackSpace:
				p->pRend->pQuadrix->Backspace();
				break;

				case XK_Delete:
				p->pRend->pQuadrix->Delete();
				break;

				case XK_Left:
				//if(p->pRend->pQuadrix->ScrollX > 0) p->pRend->pQuadrix->ScrollX--;
				p->pRend->pQuadrix->CursorLeft();
				break;

				case XK_Right:
				//p->pRend->pQuadrix->ScrollX++;
				p->pRend->pQuadrix->CursorRight();
				break;

				case XK_Up:
				//if(p->pRend->pQuadrix->ScrollY > 0) p->pRend->pQuadrix->ScrollY--;
				p->pRend->pQuadrix->CursorUp();
				break;

				case XK_Down:
				//p->pRend->pQuadrix->ScrollY++;
				p->pRend->pQuadrix->CursorDown();
				break;

				case XK_Home:
				p->pRend->pQuadrix->Home();
				break;

				case XK_End:
				p->pRend->pQuadrix->End();
				break;

				case XK_Page_Up:
				p->pRend->pQuadrix->PageUp();
				break;

				case XK_Page_Down:
				p->pRend->pQuadrix->PageDown();
				break;

				case XK_F12:
				p->pRend->ScreenShot();
				break;

				case XK_F1:		p->pRend->Mode = 1;		break;
				case XK_F2:		p->pRend->Mode = 2;		break;
				case XK_F3:		p->pRend->Mode = 3;		break;
				case XK_F4:		p->pRend->Mode = 4;		break;
				case XK_F5:		p->pRend->Mode = 5;		break;
				case XK_F6:		p->pRend->Mode = 6;		break;
				case XK_F7:		p->pRend->Mode = 7;		break;
				case XK_F8:		p->pRend->Mode = 8;		break;
				case XK_F9:		p->pRend->Mode = 9;		break;
				case XK_F10:	p->pRend->Mode = 10;	break;


				default:
				break;
			}

		}																													break;
	}
	// Alt Only
	if((pXKeyEvent->state & Mod1Mask) && !(pXKeyEvent->state & ControlMask))
	{
		switch(Buf)
		{
			case 'i':			p->pRend->bInfo = !p->pRend->bInfo;
								p->Info("p->pRend->bInfo = %d\n", p->pRend->bInfo);											break;
			case 'r':			p->pRend->bActive = !p->pRend->bActive;
								p->Info("p->pRend->bActive = %d \n", p->pRend->bActive);							 		break;		// Toggle Rendering
			case 'f':			p->pCoreGLX->SetFullScreen(!p->pCoreGLX->bFullScreen);
								p->pRend->SetBlend(p->pRend->bBlend);														break;
			case 'k':			p->pKeyboard->bInfo = !p->pKeyboard->bInfo;
								p->Info("p->pKeyboard->bInfo = %d\n", p->pKeyboard->bInfo);									break;
			case 'm':			p->pMouse->bInfo = !p->pMouse->bInfo;
								p->Info("p->pMouse->bInfo = %d\n", p->pMouse->bInfo);										break;
			case 'j':			p->pJoystick->bInfo = !p->pJoystick->bInfo;
								p->Info("p->pJoystick->bInfo = %d\n", p->pJoystick->bInfo);									break;
			case 'b':			p->pRend->SetBlend(!p->pRend->bBlend);														break;
			case 'd':			p->pRend->SetDepthTest(!p->pRend->bDepthTest);												break;
			case 'p':			p->pCoreGLX->PrintModes();																	break;
			case 'v':			p->pCoreGLX->SetVSync(!p->pCoreGLX->bVSync);												break;
			case '-':			p->Rate -= 1.0;
								p->Info("p->Rate = %f\n", p->Rate);															break;
			case '+':			p->Rate += 1.0;
								p->Info("p->Rate = %f\n", p->Rate);															break;
		}
	}
	// Control Only
	if(!(pXKeyEvent->state & Mod1Mask) && (pXKeyEvent->state & ControlMask))
	{
		switch(Buf)
		{
			case 19:			p->pRend->pQuadrix->Save();		// Ctrl S
			break;
		}

	}
	// Control + Alt
	if((pXKeyEvent->state & Mod1Mask) && (pXKeyEvent->state & ControlMask))
	{

	}
	// Neither Control or Alt
	if(!(pXKeyEvent->state & Mod1Mask) && !(pXKeyEvent->state & ControlMask))
	{
		switch(Buf)
		{
			// Escape
			case 27:			p->bActive = false;																			break;
			case 'a':
			case 'A':
			case 'b':
			case 'B':
			case 'c':
			case 'C':
			case 'd':
			case 'D':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
			case 'h':
			case 'H':
			case 'i':
			case 'I':
			case 'j':
			case 'J':
			case 'k':
			case 'K':
			case 'l':
			case 'L':
			case 'm':
			case 'M':
			case 'n':
			case 'N':
			case 'o':
			case 'O':
			case 'p':
			case 'P':
			case 'q':
			case 'Q':
			case 'r':
			case 'R':
			case 's':
			case 'S':
			case 't':
			case 'T':
			case 'u':
			case 'U':
			case 'v':
			case 'V':
			case 'w':
			case 'W':
			case 'x':
			case 'X':
			case 'y':
			case 'Y':
			case 'z':
			case 'Z':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
			case '-':
			case '=':
			case '!':
			case '@':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '*':
			case '(':
			case ')':
			case '_':
			case '+':
			case '[':
			case ']':
			case '{':
			case '}':
			case '|':
			case '\\':
			case ';':
			case ':':
			case '\'':
			case '"':
			case ',':
			case '<':
			case '.':
			case '>':
			case '/':
			case ' ':
			case '~':
			case '`':
			case '?':	p->pRend->pQuadrix->AddCharacter(&Buf);
			break;
		}
	}


}
