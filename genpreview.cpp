#include <algorithm>
#include <lrtypes.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_File_Icon.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Toggle_Button.H>
#include <stdio.h>

static Fl_RGB_Image *converted, *normal;

static void cb(Fl_Widget *w, void *) {

	if (w->image() == converted)
		w->image(normal);
	else
		w->image(converted);

	w->redraw();
}

static void convert() {
	const u32 pixels = converted->w() * converted->h();
	u32 i;

	// Convert to genesis
	if (converted->d() == 3) {
		for (i = 0; i < pixels; i++) {
			((u8 * ) converted->array)[i * 3 + 0] &= 0xe0;
			((u8 * ) converted->array)[i * 3 + 1] &= 0xe0;
			((u8 * ) converted->array)[i * 3 + 2] &= 0xe0;
		}
	} else if (converted->d() == 4) {
		for (i = 0; i < pixels; i++) {
			((u8 * ) converted->array)[i * 4 + 0] &= 0xe0;
			((u8 * ) converted->array)[i * 4 + 1] &= 0xe0;
			((u8 * ) converted->array)[i * 4 + 2] &= 0xe0;
		}
	}
}

int main(int argc, char **argv) {

	const char *name;

	Fl_File_Icon::load_system_icons();

	if (argc < 2) {
		name = fl_file_chooser("Select PNG to preview", "*.png", "", 1);
		if (!name)
			return 1;
	} else {
		name = argv[1];
	}

	Fl_PNG_Image orig(name);

	const u16 screenw = Fl::w();
	const u16 screenh = Fl::h();

	u16 scale, scalex, scaley, w, h;

	scalex = screenw / orig.w();
	if (screenw % orig.w() == 0)
		scalex--;
	scaley = screenh / orig.h();
	if (screenh % orig.h() == 0)
		scaley--;

	if (!scalex)
		scalex = 1;
	if (!scaley)
		scaley = 1;

	scale = std::min(scalex, scaley);

	// New images
	w = orig.w() * scale;
	h = orig.h() * scale;

	normal = (Fl_RGB_Image *) orig.copy(w, h);
	converted = (Fl_RGB_Image *) normal->copy();

	convert();

	char tmp[PATH_MAX];
	sprintf(tmp, "Genesis previewer, %s", name);

	Fl_Double_Window win(w, h, tmp);

	Fl_Toggle_Button but(0, 0, w, h);
	but.callback(cb);
	but.image(converted);

	win.show();

	return Fl::run();
}
