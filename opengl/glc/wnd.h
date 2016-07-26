#pragma once

struct GLAPP {
	void (*GLInit)(void);
};


struct GLAPP *GetApp();