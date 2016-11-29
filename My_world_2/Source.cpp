#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <vector>

//#include "MainMenu.h"  На будущее, перенесём сюда функционал меню

using namespace sf;

float scaleX = 0.24f; // Коэфициенты масштабирования спрайтов
float scaleY = 0.24f;

const int H = 23;
const int W = 135;

float offsetX = 0, offsetY = 0; // Это для сдвига экрана
int bonus = 0;
std::ostringstream BonusSTR;
int scrX = GetSystemMetrics(SM_CXSCREEN);
int scrY = GetSystemMetrics(SM_CYSCREEN);

int ground = 1720; // координата земли

Texture DefaultTexture;

String TileMap[H] = {
	"BBBBBBBBBBB    BBBBBBB   BBBB       BBBBBB  BBBBBBBBBBBBBBBBBBBBBBBBB  BBBBBB  BBBBBBBBBBBBBBBBBBBBBBBBB  BBBBBBBBBBBBBBBBBBBBB        ",
	"B                                                                                                                                      ",
	"                                                                               0            0                                          ",
	"                                  B                                                                                                    ",
	"  00                         BB   B                                           BB           B        0                                  ",
	"                                  B                                                                BB                                  ",
	"                       BB         B                                                                                                    ",
	"                                                                                                                                       ",
	"      BBBBBB                                                 BB                                              BB              0         ",
	"                                                        00                           00                                BB       BB     ",
	"                                                                                                                                       ",
	"                        0 000     B                                                      BB                      0                     ",
	"                                  B                  BB               00                                          BB                   ",
	"                  BB              B                                           BBB                              0                       ",
	"                  BB              B           BB                                                              BB                       ",
	"                  BB              B                                                                                                    ",
	"BBBBBBBBB    BBBBBBBBBBBBBBBBBBBBBBBBBBB                                  B                          BB                                ",
	"                                                                                                                    BB                 ",
	"                                                                                                                                       ",
	"                                                                                                            BB                         ",
	"BBBBBBBBBBBBBBBBBBBBBBB          BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
	"                                                                                                                                       ", 
	"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
};

struct GameObj {

public:
	float dx, dy; //импульс
	FloatRect rect; // место 4-х переменных координат ширины высоты, теперь две
	bool onGround; // на земле мы или нет
	Sprite sprite; // картинка игрока
	float currentFrame; // текущий кадр	
	bool moveLeft = 0; // двигались ли мы вправо или влево
	Clock clock;
	int timeDimention;

	GameObj(Texture &image, float posX, float posY)
	{
		sprite.setTexture(image);
		sprite.setTextureRect(IntRect(0, 500, 500, 500));  //подрезаем картинку в нужный кадр*/
		sprite.setScale(scaleX, scaleY);  // Масштабирование персонажа

		rect = FloatRect(0, 0, 500 * scaleX, 500 * scaleY);  //первоначальные координаты и ширина высота

		rect.left = posX;
		rect.top = posY;
		dx = dy = 0;
		currentFrame = 0;
		timeDimention = 450;


	}

	virtual void update()
	{

		float time = clock.getElapsedTime().asMicroseconds();
		time = time / timeDimention;
		clock.restart();

		if (!((rect.left + dx*time) == 0) && (!((rect.left + dx*time) < 0))) {
			rect.left += dx*time; // перемещаем по горизонтали
		}

		Collision('x'); /*Проверка на столкновение по горизонтали*/

		if (!onGround) dy = dy + .0005f*time; //если мы не на земле, то падаем с ускорением

		if ((!((rect.top + dy*time) < 0)) && (!((rect.top + dy*time) == 2000))) {
			rect.top += dy*time; //перемещаем по вертикали
		}

		onGround = false;

		Collision('y'); /*Проверка на столкновение по вертикали*/

		currentFrame += 0.005f*time;
		if (currentFrame > 10) currentFrame -= 10;

		if ((dx == 0) && (dy == 0) && (moveLeft == false)) { sprite.setTextureRect(IntRect(0, 500, 500, 500)); }  //когда стоим
		if ((dx == 0) && (dy == 0) && (moveLeft == true)) { sprite.setTextureRect(IntRect(500, 500, -500, 500)); }

		if (dx > 0) { sprite.setTextureRect(IntRect(500 * int(currentFrame), 0, 500, 500)); moveLeft = false; }
		if (dx < 0) { sprite.setTextureRect(IntRect(500 * int(currentFrame) + 500, 0, -500, 500)); moveLeft = true; }


		sprite.setPosition(rect.left - offsetX, rect.top - offsetY); //переводим спрайт на координаты 	

		dx = 0;

	}

	virtual void Collision(char dir) { /*Проверка на столкновение по горизонтали*/
		for (int i = int(rect.top / 40); i < (rect.top + rect.height) / 40; i++)
			for (int j = int(rect.left / 40); j < (rect.left + rect.width) / 40; j++)
			{
				if (TileMap[i][j] == 'B')
				{
					if ((dy > 0) && (dir == 'y')) { rect.top = i * 40 - rect.height; dy = 0.0f; onGround = true; }
					if ((dy < 0) && (dir == 'y')) { rect.top = int(i * 40 + 40); dy = 0.0f; }
					if ((dx > 0) && (dir == 'x')) rect.left = j * 40 - rect.width;
					if ((dx < 0) && (dir == 'x')) rect.left = int(j * 40 + 40);
				}
				if (TileMap[i][j] == '0')
				{
					TileMap[i][j] = ' ';
					bonus = bonus + 1;
					BonusSTR.clear();
					BonusSTR << bonus;

				}
			}

	};

	void setImpulseDx(float dxNew){
		try{
			dx = dxNew;
		}
		catch (...) {
			MessageBox(NULL, "Недопустимое значение импульса dx", "", MB_OK);
		};
	};
	float getImpulseDx(){ return dx; };

	void setImpulseDy(float dyNew){
		try{
			dy = dyNew;
		}
		catch (...) {
			MessageBox(NULL, "Недопустимое значение импульса dy", "", MB_OK);
		};
	};
	float getImpulseDy(){ return dy; };

	void setTimeDimention(int timeDim){
		timeDimention = timeDim;
	}
	int getTimeDimention(){ return timeDimention; }
};

class Bullet : public GameObj{
public:

	Bullet(Texture &image, float posX, float posY, bool shotLeft) : GameObj(image, posX, posY){
		if (shotLeft == 0) { dx = 0.02f; }
		if (shotLeft == 1) { dx = -0.02f; }
		

	};

	virtual void update()
	{
		//dx = dx = 0.03f;
		float time = clock.getElapsedTime().asMicroseconds();
		time = time / timeDimention;
		clock.restart();

		if (!((rect.left + dx*time) == 0) && (!((rect.left + dx*time) < 0))) {
			rect.left += dx*time; // перемещаем по горизонтали*
		}	

		currentFrame += 0.005f*time;
		if (currentFrame > 10) currentFrame -= 10;

		if ((dx == 0) && (dy == 0) && (moveLeft == false)) { sprite.setTextureRect(IntRect(0, 500, 500, 500)); }  //когда стоим
		if ((dx == 0) && (dy == 0) && (moveLeft == true)) { sprite.setTextureRect(IntRect(500, 500, -500, 500)); }

		if (dx > 0) { sprite.setTextureRect(IntRect(500 * int(currentFrame), 0, 500, 500)); moveLeft = false; }
		if (dx < 0) { sprite.setTextureRect(IntRect(500 * int(currentFrame) + 500, 0, -500, 500)); moveLeft = true; }


		sprite.setPosition(rect.left - offsetX, rect.top - offsetY); //переводим спрайт на координаты 		

	}	

};



class Player : public GameObj{
public:
	int BulletCount = 0;
	std::vector<Bullet> BulletMassive; // Счётчик и вектор объектов пуль
	Texture aimTexture;
	Sprite aimSprite;
	float aimMathX, aimMathY;
	int iAim;


	Player(Texture &image, float posX, float posY) : GameObj(image, posX, posY){  		
		aimTexture.loadFromFile("Source/CrossAim/aim_1.png"); // ПРИЦЕЛ  <----------<-----------<------------<----------<------------<<<<<
		aimSprite.setTexture(aimTexture);
		aimSprite.setTextureRect(IntRect(0, 0, 100, 100));  //подрезаем картинку в нужный кадр*/
		aimSprite.setScale(0.35, 0.35);  // Масштабирование прицела		
		aimMathX = 60;
		aimMathY = 60;
		};
	
	virtual void update(){
		for (int i = 0; i < BulletCount; i++){ // Отрисовка всех объектов в векторе
			BulletMassive[i].update();			
		}		
		GameObj::update();
		/*Калибровка прицела*/
		if (moveLeft == false){ aimSprite.setPosition(sprite.getPosition().x + aimMathX+70, sprite.getPosition().y + aimMathY); };
		if (moveLeft == true){ aimSprite.setPosition(sprite.getPosition().x - aimMathX, sprite.getPosition().y + aimMathY); };
        };

	void aimMath(char xy, bool increese) {
		
		if (iAim<101){
			if ((xy == 'X') && (increese == 1)){
				aimMathX = 0.5*cos(((iAim/100)*3.14f) / 2);
				iAim++;
			};
			if (xy == 'X' && (increese == 0)){
				aimMathX = 0.5*cos(((iAim/100)*3.14f) / 2);
				iAim--;
			};
		}
		else iAim = 0;



	};

	void fight(){
		if (moveLeft == true) {
		Bullet bulllet(DefaultTexture, rect.left + 25.0f, rect.top + 25.0f, true);
		BulletMassive.push_back(bulllet);
		BulletCount++;
	};
		if (moveLeft == false) {
			Bullet bulllet(DefaultTexture, rect.left + 25.0f, rect.top + 25.0f, false);
			BulletMassive.push_back(bulllet);
			BulletCount++;
		}
}

	};
	




/*Здесь функция меню. Ее нужно перенести в отдельный файл, чтобы избежать путаницы. И разберись наконец то, как  
подключать заголовочные файлы к проекту!*/
void MainMenu(RenderWindow &wnd){
	Texture menuTexture1, menuTexture2, menuTexture3, menuBackTexture, aboutTexture;
	menuTexture1.loadFromFile("Source/Menu/Menu111.png");
	menuTexture2.loadFromFile("Source/Menu/Menu222.png");
	menuTexture3.loadFromFile("Source/Menu/Menu333.png");
	menuBackTexture.loadFromFile("Source/Menu/MenuBack.png");
	aboutTexture.loadFromFile("Source/Menu/MenuAbout.png");
	
	

	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBack(menuBackTexture);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(50, 30);   // тут надо будет подкрутить по размерам картинок
	menu2.setPosition(50, 90); 
	menu3.setPosition(50, 150);
	menuBack.scale(0.75f, 0.75f);
	menuBack.setPosition(75, 0);	

	while (isMenu) {
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		menuNum = 3;
		wnd.clear(Color(1, 1, 1));

		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(wnd))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(wnd))) { menu2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(wnd))) { menu3.setColor(Color::Blue); menuNum = 3; }

		if (Mouse::isButtonPressed(Mouse::Left)) {
			if (menuNum == 1) isMenu = false; // Первая кнопка - выход из меню в игру
			if (menuNum == 2) {/*window.draw(aboutScreen); wnd.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape));*/ }
			if (menuNum == 3) { wnd.close(); isMenu = false; }
		}
		wnd.draw(menuBack);
		wnd.draw(menu1);
		wnd.draw(menu2);
		wnd.draw(menu3);
		wnd.display();
	}
}






/*Почисти ф-ю main() - там скопилось много эксперементального дерьма. Но перед удалением просмотри алгоритм хранения объектов игры и работы с ними*/



int main()
{
	

	Font font;
	font.loadFromFile("Source/Visitor_Rus-Regular.ttf");
	Text text("", font, 40); //Создаём объект текст, передаём туда текст, шрифт и размер шрифта(в пикселях)
	text.setColor(Color::Red);
	text.setStyle(sf::Text::Bold | sf::Text::Underlined);

	int ObjCount = 0; std::vector<Player> PayerMassive; // Счётчик и вектор объектов игры 

	RenderWindow window(VideoMode(scrX-10, scrY-10), "BFGirl", Style::None);
	MainMenu(window);

	Texture texture;
	texture.loadFromFile("Source/NEST03.png");
	DefaultTexture.loadFromFile("Source/DefaultTexture.png");
	

	float currentFrame = 0;

	Player mainPlayer(texture, 100.0f, 100.0f);

	RectangleShape rectangle(Vector2f(40, 40));  //элементы уровня

	while (window.isOpen())	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}


		if (Keyboard::isKeyPressed(Keyboard::Left) && Keyboard::isKeyPressed(Keyboard::LShift))
		{
			mainPlayer.aimMath('X', true);
		}
		if (Keyboard::isKeyPressed(Keyboard::Right) && Keyboard::isKeyPressed(Keyboard::LShift))
		{
			mainPlayer.aimMath('X', false);
		}



		if (Keyboard::isKeyPressed(Keyboard::Left))
		{
			mainPlayer.setImpulseDx(-0.1f);
		}

		if (Keyboard::isKeyPressed(Keyboard::Right))
		{
			mainPlayer.setImpulseDx(0.1f);
		}

		if (Keyboard::isKeyPressed(Keyboard::Up))
		{
			if (mainPlayer.onGround) { mainPlayer.setImpulseDy(-0.55f); mainPlayer.onGround = false; }
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))  { window.close(); }

		if (Keyboard::isKeyPressed(Keyboard::Down) && Keyboard::isKeyPressed(Keyboard::LShift)) // генерация объекта и добавление его в вектор ПО НАЖАТИЮ КЛАВИШ!!!

		
		if (Keyboard::isKeyPressed(Keyboard::Down) && Keyboard::isKeyPressed(Keyboard::LShift)) // генерация объекта и добавление его в вектор

		{
			mainPlayer.fight();
		}

		mainPlayer.update();
		if (mainPlayer.rect.left > (scrX / 2)) offsetX = mainPlayer.rect.left - scrX / 2;
		offsetY = mainPlayer.rect.top - scrY / 2;


		window.clear(Color::White);

		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++)
			{
				if (TileMap[i][j] == 'B') rectangle.setFillColor(Color::Black);
				if (TileMap[i][j] == '0') rectangle.setFillColor(Color::Green);
				if (TileMap[i][j] == ' ') continue;

				rectangle.setPosition(j * 40 - offsetX, i * 40 - offsetY);
				window.draw(rectangle);
			}
		}

		text.setString("Bonus packege: " + BonusSTR.str());
		text.setPosition(20, 20);
		window.draw(text);
		window.draw(mainPlayer.sprite);
				
		for (int i = 0; i < mainPlayer.BulletCount; i++){ // Отрисовка всех объектов в векторе
			window.draw(mainPlayer.BulletMassive[i].sprite);
		}
		window.draw(mainPlayer.aimSprite);

		window.display();

	}
	return 0;

}