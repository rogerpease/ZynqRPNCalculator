#include <iostream> 
#include <sstream>
using namespace std; 

class ZynqRPNCalculatorVerification 
{ 

    public :  // Will make private once I'm further down the road. 
        ZynqRPNCalculatorVerification();
        void Initialize();

        bool DEBUG = false;
        unsigned long int value;
        unsigned char * clock;
        unsigned char * reset;
        unsigned char * pop;
        unsigned char * push;
        unsigned char * add;
        unsigned char * sub;
        unsigned char * mul;
        unsigned long int  stack0;

 
}; 
