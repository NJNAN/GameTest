#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
#include <iostream>

struct Item {
    std::string name;
    int heal = 0;
    int attack = 0;
};

struct Inventory {
    std::vector<Item> items;
    void add(const Item &it) { items.push_back(it); std::cout << "Picked up " << it.name << "\n"; }
};

struct Player {
    int x = 1;
    int y = 1;
    int hp = 20;
    int attack = 5;
    int gold = 10;
    Inventory inv;
};

struct Monster { int hp; int attack; Item drop; };
struct NPC { Item good; int price; };
struct Map { std::vector<std::string> grid; };

class Game {
    const int TILE = 32;
    std::vector<Map> maps;
    int current = 0;
    std::map<std::pair<int,int>,Monster> monsters;
    std::map<std::pair<int,int>,NPC> npcs;
    std::map<std::pair<int,int>,Item> items;
    Player player;

    sf::Texture floorTex, wallTex, exitTex, playerTex, monsterTex, itemTex, npcTex;
    sf::Font font;
    sf::RenderWindow window;

public:
    Game() : window(sf::VideoMode(10*TILE, 6*TILE+40), "RPG") { setup(); }

    void run(){
        while(window.isOpen()){
            handleEvents();
            window.clear();
            draw();
            window.display();
        }
    }
private:
    void setup(){
        Map m1{ {"##########",
                 "#........#",
                 "#..M...E.#",
                 "#..N.I...#",
                 "#........#",
                 "##########" } };
        Map m2{ {"##########",
                 "#........#",
                 "#..M.....#",
                 "#...I..E.#",
                 "#..N.....#",
                 "##########" } };
        maps={m1,m2};
        monsters[{3,2}] = Monster{10,3,{"Potion",5,0}};
        monsters[{3,2+6}] = Monster{12,4,{"Sword",0,2}}; // on map2 offset by 6 rows
        npcs[{3,3}] = NPC{{"BigPotion",10,0},5};
        npcs[{3,3+6}] = NPC{{"Axe",0,3},7};
        items[{5,3}] = {"Gold",0,0};
        items[{4,2+6}] = {"Gold",0,0};

        createTextures();
        font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }

    void createTextures(){
        sf::Image img;
        img.create(TILE,TILE,sf::Color(160,110,60)); floorTex.loadFromImage(img);
        img.create(TILE,TILE,sf::Color(80,80,80)); wallTex.loadFromImage(img);
        img.create(TILE,TILE,sf::Color::Magenta); exitTex.loadFromImage(img);
        img.create(TILE,TILE,sf::Color::Blue); playerTex.loadFromImage(img);
        img.create(TILE,TILE,sf::Color::Red); monsterTex.loadFromImage(img);
        img.create(TILE,TILE,sf::Color::Yellow); itemTex.loadFromImage(img);
        img.create(TILE,TILE,sf::Color::Green); npcTex.loadFromImage(img);
    }

    void handleEvents(){
        sf::Event e;
        while(window.pollEvent(e)){
            if(e.type==sf::Event::Closed) window.close();
            else if(e.type==sf::Event::KeyPressed){
                if(e.key.code==sf::Keyboard::Escape) window.close();
                if(e.key.code==sf::Keyboard::Left) move(-1,0);
                if(e.key.code==sf::Keyboard::Right) move(1,0);
                if(e.key.code==sf::Keyboard::Up) move(0,-1);
                if(e.key.code==sf::Keyboard::Down) move(0,1);
            }
        }
    }

    void move(int dx,int dy){
        Map &m=maps[current];
        int nx=player.x+dx, ny=player.y+dy;
        if(nx<0||ny<0||ny>=m.grid.size()||nx>=m.grid[0].size()) return;
        char tile=m.grid[ny][nx];
        if(tile=='#') return;
        player.x=nx; player.y=ny;
        handleTile();
    }

    void handleTile(){
        std::pair<int,int> pos={player.x, player.y + current*6};
        if(monsters.count(pos)) battle(monsters[pos], pos);
        if(npcs.count(pos)) talk(npcs[pos]);
        if(items.count(pos)){ player.inv.add(items[pos]); items.erase(pos); }
        Map &m=maps[current];
        if(m.grid[player.y][player.x]=='E'){ current=(current+1)%maps.size(); player.x=1; player.y=1; }
    }

    void battle(Monster mon, std::pair<int,int> pos){
        while(player.hp>0 && mon.hp>0){
            mon.hp-=player.attack;
            if(mon.hp<=0) break;
            player.hp-=mon.attack;
        }
        if(player.hp<=0){ window.close(); }
        else{ player.inv.add(mon.drop); monsters.erase(pos); }
    }

    void talk(NPC npc){
        if(player.gold>=npc.price){
            player.gold-=npc.price; player.inv.add(npc.good); npcs.erase({player.x, player.y + current*6});
        }
    }

    sf::Texture& texFor(char c){
        if(c=='#') return wallTex;
        if(c=='E') return exitTex;
        return floorTex;
    }

    void draw(){
        Map &m=maps[current];
        sf::Sprite sprite;
        for(size_t y=0;y<m.grid.size();++y){
            for(size_t x=0;x<m.grid[y].size();++x){
                sprite.setTexture(texFor(m.grid[y][x]));
                sprite.setPosition(x*TILE,y*TILE);
                window.draw(sprite);
            }
        }
        // draw objects
        for(const auto &kv:monsters) if(posInMap(kv.first)) { sprite.setTexture(monsterTex); sprite.setPosition((kv.first.first)*TILE,(kv.first.second%6)*TILE); window.draw(sprite); }
        for(const auto &kv:npcs) if(posInMap(kv.first)) { sprite.setTexture(npcTex); sprite.setPosition((kv.first.first)*TILE,(kv.first.second%6)*TILE); window.draw(sprite); }
        for(const auto &kv:items) if(posInMap(kv.first)) { sprite.setTexture(itemTex); sprite.setPosition((kv.first.first)*TILE,(kv.first.second%6)*TILE); window.draw(sprite); }
        sprite.setTexture(playerTex); sprite.setPosition(player.x*TILE,player.y*TILE); window.draw(sprite);
        sf::Text text; text.setFont(font); text.setCharacterSize(14); text.setFillColor(sf::Color::White);
        text.setString("HP:"+std::to_string(player.hp)+" Gold:"+std::to_string(player.gold));
        text.setPosition(5,6*TILE+5); window.draw(text);
    }

    bool posInMap(std::pair<int,int> p){ return (p.second/6)==current; }
};

int main(){
    Game g; g.run();
    return 0;
}

