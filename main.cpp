#define JJ_FULLSCREEN

#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

int width = 1024;
int height = 768;
int roadW = 2000;
int segL = 200; //segment length
float camD = 0.84; //camera depth

void drawQuad(RenderWindow &w, Color c, int x1, int y1, int w1, int x2, int y2, int w2)
{
	ConvexShape shape(4);
	shape.setFillColor(c);
	shape.setPoint(0, Vector2f(x1 - w1, y1));
	shape.setPoint(1, Vector2f(x2 - w2, y2));
	shape.setPoint(2, Vector2f(x2 + w2, y2));
	shape.setPoint(3, Vector2f(x1 + w1, y1));
	w.draw(shape);
}

struct Line
{
	float x, y, z; //3d center of line
	float X, Y, W; //screen coord
	float curve, spriteX, clip, scale;
	Sprite sprite;

	Line()
	{
		spriteX = curve = x = y = z = 0;
	}

	void project(int camX, int camY, int camZ)
	{
		scale = camD / (z - camZ);
		X = (1 + scale * (x - camX)) * width / 2;
		Y = (1 - scale * (y - camY)) * height / 2;
		W = scale * roadW  * width / 2;
	}

	void drawSprite(RenderWindow &app)
	{
		Sprite s = sprite;
		//int w = s.getTextureRect().width;
		//int h = s.getTextureRect().height;
		int w = s.getTextureRect().width;
		int h = s.getTextureRect().height;
		//if(w || h)std::cout << w << " " << h << std::endl;
		//w = 256; h = 848;

		float destX = X + scale * spriteX * width / 2;
		float destY = Y + 4;
		//float destW = w * W / 266;
		//float destH = h * W / 266;
		// Scale to human height
		float destW = (w * W / 266) * 266 / w;
		float destH = (h * W / 266) * 848 / h;

		destX += destW * spriteX; //offsetX
		destY += destH * (-1);    //offsetY

		float clipH = destY + destH - clip;
		if (clipH<0) clipH = 0;

		if (clipH >= destH) return;
		s.setTextureRect(IntRect(0, 0, w, h - h * clipH / destH));
		s.setScale(destW / w, destH / h);
		s.setPosition(destX, destY);
		app.draw(s);
	}
};



void getWithAndHeight(int &screen_w, int &screen_h, int &style)
{
#ifdef JJ_FULLSCREEN
	// Display the list of all the video modes available for fullscreen
	std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	for (std::size_t i = 0; i < modes.size(); ++i)
	{
		sf::VideoMode mode = modes[i];
		std::cout << "Mode #" << i << ": "
			<< mode.width << "x" << mode.height << " - "
			<< mode.bitsPerPixel << " bpp" << std::endl;
	}

	sf::VideoMode mode = modes[0];
	// Create a window with the same pixel depth as the desktop
	//sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	//sf::RenderWindow window(sf::VideoMode(mode.width, mode.height, desktop.bitsPerPixel), "SFML window", sf::Style::Fullscreen);
	screen_w = mode.width;
	screen_h = mode.height;

	style = sf::Style::Fullscreen;
	//#define JJ_KEEP_RATIO
#ifdef JJ_KEEP_RATIO
	screen_w = (screen_h * 1024) / 768;

#endif

#else
	//sf::RenderWindow window(sf::VideoMode(700, 400), "Hoverboard");

	screen_w = 1024;
	screen_h = 768;

	style = sf::Style::Default;
#endif
}




int get_js()
{
	int js = 0;
	sf::Joystick::update();
	while (js < 8)
	{
		if (sf::Joystick::isConnected(js)) {
			//printf("Joystick %i\n", js);
			break;
		}
		js++;
	}
	if (js > 7) {
		//printf("No joystick\n");
	}
	return js;

}





int main()
{
	int screenStyle;
	getWithAndHeight(width, height, screenStyle);

	RenderWindow app(VideoMode(width, height), "Outrun Racing!", screenStyle);


	app.setMouseCursorVisible(false);


	sf::Font font;
	if (!font.loadFromFile("C:\\Windows\\fonts\\arial.ttf"))
	{
		printf("Error loading font.\n");
		exit(1);
	}
	sf::Text text;
	// select the font
	text.setFont(font); 
	text.setCharacterSize(24 * 4); // in pixels, not points!
	//text.setOutlineColor(sf::Color::Red);
	text.setFillColor(sf::Color::Red);








	app.setFramerateLimit(60);

	Texture t[50];
	Sprite object[50];
	for (int i = 1; i <= 7; i++)
	{
		t[i].loadFromFile("images/" + std::to_string(i) + ".png");
		t[i].setSmooth(true);
		object[i].setTexture(t[i]);
	}

	Texture bg;
	bg.loadFromFile("images/bg.png");
	bg.setRepeated(true);
	Sprite sBackground(bg);
	sBackground.setTextureRect(IntRect(0, 0, 5000, 411));
	sBackground.setPosition(-2000, 0);

	std::vector<Line> lines;

	for (int i = 0; i<1600; i++)
	{
		Line line;
		line.z = i * segL;

		if (i>300 && i<700) line.curve = 0.5;
		if (i>1100) line.curve = -0.7;
		/*
		if (i<300 && i % 20 == 0) { line.spriteX = -2.5; line.sprite = object[5]; }
		if (i % 17 == 0) { line.spriteX = 2.0; line.sprite = object[6]; }
		if (i>300 && i % 20 == 0) { line.spriteX = -0.7; line.sprite = object[4]; }
		if (i>800 && i % 20 == 0) { line.spriteX = -1.2; line.sprite = object[1]; }
		if (i == 400) { line.spriteX = -1.2; line.sprite = object[7]; }
		*/
#if 0
		// 4, 5, 6 are bystanders
		if (i<300 && i % 20 == 0) { line.spriteX = -2.5; line.sprite = object[rand()%3 + 4]; }
		if (i % 17 == 0) { line.spriteX = 3.0; line.sprite = object[rand() % 3 + 4]; }
		if (i>300 && i % 20 == 0) { line.spriteX = -2.7; line.sprite = object[rand() % 3 + 4]; }
		if (i>800 && i % 20 == 0) { line.spriteX = -4.2; line.sprite = object[rand() % 3 + 4]; }
#endif
		if (rand()%100 < 3) { line.spriteX = ((rand()%40) * 0.1 + 2.0)  * ((rand()%2)*2 - 1) ; line.sprite = object[rand() % 3 + 4]; }


		// The flag
		if (i == 400) { line.spriteX = -2.0/*-1.2*/; line.sprite = object[7]; }


		if (i>750) line.y = sin(i / 30.0) * 1500;

		lines.push_back(line);
	}

	int N = lines.size();
	float playerX = 0;
	int pos = 0;
	int H = 1500 +4500;

	int js = get_js();
	int speed = 300.0;

	int speedometer_flag = 0;

	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();

			if (e.key.code == sf::Keyboard::Escape)
				app.close();

			if(e.key.code == sf::Keyboard::V)
				speedometer_flag = 1;


		}


		if (sf::Joystick::isButtonPressed(js, 0)) playerX -= 0.1;
		if (sf::Joystick::isButtonPressed(js, 1)) playerX += 0.1;

		if (Keyboard::isKeyPressed(Keyboard::Right)) playerX += 0.1;
		if (Keyboard::isKeyPressed(Keyboard::Left)) playerX -= 0.1;
		/*
		if (Keyboard::isKeyPressed(Keyboard::Up)) speed = 200;
		if (Keyboard::isKeyPressed(Keyboard::Down)) speed = -200;
		if (Keyboard::isKeyPressed(Keyboard::Tab)) speed *= 3;
		*/
		//speed = 200 + 100;
		speed += 1.0;
		if (Keyboard::isKeyPressed(Keyboard::W)) H += 100;
		if (Keyboard::isKeyPressed(Keyboard::S)) H -= 100;

		// JJ
		if (playerX > 1.0)playerX = 1.0;
		if (playerX < -1.0)playerX = -1.0;
		if (playerX > 0.9 || playerX < -0.9)speed = 0.3*300;

		pos += speed;
		while (pos >= N * segL) pos -= N * segL;
		while (pos < 0) pos += N * segL;

		//app.clear(Color(105, 205, 4));
		app.clear(Color(255, 255, 255));
		app.draw(sBackground);
		int startPos = pos / segL;
		int camH = lines[startPos].y + H;
		if (speed>0) sBackground.move(-lines[startPos].curve * 2, 0);
		if (speed<0) sBackground.move(lines[startPos].curve * 2, 0);

		int maxy = height;
		float x = 0, dx = 0;



		float s = abs(playerX);
		if (s > 0.9) {
			dx += sin(rand())*(sqrt(s) - 0.9)*speed;
			
		}


		// JJ
		//if (speed)playerX -= lines[startPos].curve * 0.022;
		if (speed)playerX -= lines[startPos].curve * 0.05;

		///////draw road////////
		for (int n = startPos; n<startPos + 300; n++)
		{
			Line &l = lines[n%N];
			l.project(playerX*roadW - x, camH, startPos*segL - (n >= N ? N * segL : 0));
			x += dx;
			dx += l.curve;

			l.clip = maxy;
			if (l.Y >= maxy) continue;
			maxy = l.Y;
			/*
			Color grass = (n / 3) % 2 ? Color(16, 200, 16) : Color(0, 154, 0);
			Color rumble = (n / 3) % 2 ? Color(255, 255, 255) : Color(0, 0, 0);
			Color road = (n / 3) % 2 ? Color(107, 107, 107) : Color(105, 105, 105);
			*/
			Color road = (n / 3) % 2 ? Color(245, 245, 255) : Color(240, 240, 255);
			Color rumble = (n / 3) % 2 ? Color(240, 240, 255) : Color(0, 0, 10);
			Color grass = (n / 3) % 2 ? Color(255, 255, 255) : Color(250, 250, 255);



			Line p = lines[(n - 1) % N]; //previous line

			drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);
			drawQuad(app, rumble, p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2);
			drawQuad(app, road, p.X, p.Y, p.W, l.X, l.Y, l.W);
		}

		////////draw objects////////
		for (int n = startPos + 300; n>startPos; n--)
			lines[n%N].drawSprite(app);


		// Display the speed
		if (speedometer_flag) {
			char textBuf[200];
			sprintf_s(textBuf, "     %d km/h", (int)(speed / 20));
			text.setString(textBuf);
			//text.setOutlineColor(sf::Color::Red);
			app.draw(text);
		}







		app.display();
	}

	return 0;
}
