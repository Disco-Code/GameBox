#include "Game.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Game game;

	while (game.getWindow()->isOpen()) {
		sf::Event event;
		while (game.getWindow()->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				game.getWindow()->close();

			game.handleWindowEvent(event);
		}

		game.update();
	}

	game.getWindow()->close();

	return 0;
}
