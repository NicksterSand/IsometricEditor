#ifndef INPUT_HPP
#define INPUT_HPP
#define SDL_MAIN_HANDLED
#include <SDL.h>

enum MouseButtons{LeftButton, RightButton, MiddleButton};

class InputManager {
	public:
		bool inputLoop() {
			bool quit = false;
		
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}
			}
			keyStates = SDL_GetKeyboardState(NULL);
			mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
			return quit;
		}
		bool isDown(std::string keyName) {
			SDL_Keycode keycode = SDL_GetScancodeFromName(keyName.c_str());
		
			if (keycode == SDLK_UNKNOWN) {
				std::cout << "Could not find key with name '" << keyName << "'! SDL_Error: " << SDL_GetError() << std::endl;
			}

			return keyStates[keycode];
		}
		bool mouseDown(MouseButtons button) {
			if (button == LeftButton) {
				return (mouseButtons & SDL_BUTTON_LMASK) != 0;
			} else if (button == RightButton) {
				return (mouseButtons & SDL_BUTTON_RMASK) != 0;
			} else if(button == MiddleButton) {
				return (mouseButtons & SDL_BUTTON_MMASK) != 0;
			}
			std::cout << "Error: Tried to query mouse button that does not exist" << std::endl;
			return false;
		}
		int mouseX;
		int mouseY;
	private:
		const Uint8* keyStates;
		Uint8 mouseButtons;

};


#endif