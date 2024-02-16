#include <iostream>
#include <fmt/core.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1080

static const double x_start=-2;
static const double x_end=1;
static const double y_start=-1;
static const double y_end=1;

static int MAXITER=1000;

int diverge(double cx,double cy){
    std::vector<unsigned int> colors_ramp = {
            0x000003FF,
            0x000003FF,
            0x0B0726FF,
            0x240B4EFF,
            0x410967FF,
            0x5D126EFF,
            0x781C6DFF,
            0x932567FF,
            0xAE305BFF,
            0xC73E4CFF,
            0xDC5039FF,
            0xED6825FF,
            0xF7850EFF,
            0xFBA40AFF,
            0xF9C52CFF,
            0xF2E660FF,
    };
    int iter=0;
    double vx=cx;
    double vy=cy;
    double tx,ty;
    while (iter<MAXITER && (vx*vx+vy*vy)<4){
        tx=vx*vx-vy*vy+cx;
        ty=2*vx*vy+cy;
        vx=tx;
        vy=ty;
        iter++;
    }
    if(iter>0 && iter<MAXITER){
        return colors_ramp[iter%16];
    }
    else{
        return 0xde4c8a;
    }

}
int* mandelbrotHost(){
    int *d_res=new int[IMAGE_HEIGHT*IMAGE_WIDTH];
    double dx=(x_end-x_start)/IMAGE_WIDTH;
    double dy=(y_end-y_start)/IMAGE_HEIGHT;

    for (int i = 0; i < IMAGE_WIDTH; ++i) {
        for (int j = 0; j < IMAGE_HEIGHT; ++j) {
            //cx+cxi numero complejo
            double cx=x_start+i*dx;
            double cy=y_end-j*dy;
            int color=diverge(cx,cy);

            d_res[j*IMAGE_WIDTH+i]=color;
        }
    }
    return d_res;
}
std::shared_ptr<sf::Uint8 >createTexture(){
    int* buffer=mandelbrotHost();
    sf::Uint8 *pixels=new sf::Uint8 [IMAGE_HEIGHT*IMAGE_WIDTH*4];
    for (int i = 0; i < IMAGE_WIDTH*IMAGE_HEIGHT*4; i+=4) {
        auto color=buffer[i/4];
        pixels[i+0]=color>>16&0xFF;
        pixels[i+1]=color>>8&0xFF;
        pixels[i+2]=color>>0&0xFF;
        pixels[i+3]=0xFF;

    }
    delete[]buffer;
    return std::shared_ptr<sf::Uint8 >(pixels);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(IMAGE_WIDTH, IMAGE_HEIGHT), "My window");
    auto pixels=createTexture();
    sf::Texture texture;
    texture.create(IMAGE_WIDTH,IMAGE_HEIGHT);
    texture.update(pixels.get());

    sf::Sprite sprite;
    sprite.setTexture(texture);

    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        MAXITER++;
        {
            pixels=createTexture();
            texture.update(pixels.get());
        }
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    return 0;
}
