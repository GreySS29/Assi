#include "my_piper.h"

int main ()
{   
    const char* input  {"plughw:3,0"};
    const char* respond {"Hello, my dear friend"}; 
    My_piper piper(respond,input);
}