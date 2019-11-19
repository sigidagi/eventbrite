#include "eventbrite.hpp"

int main()
{
    brite::EventBrite* br = brite::EventBrite::instance();
    //br->init(); 
    
    br->loop().run(0);
    return 0;
}
