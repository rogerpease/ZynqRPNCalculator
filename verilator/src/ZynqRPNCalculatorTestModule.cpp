#include "VZynqRPNCalculator.h"
#include "ZynqRPNCalculatorVerification.h"
#include <iostream>
#include <memory> 
#include <sstream> 


class ZynqRPNCalculatorWrapperClass : VZynqRPNCalculator
{

  public:
    bool debug = false;  
    VZynqRPNCalculator * Calculator; 
  
  ZynqRPNCalculatorWrapperClass () 
  {  
    cout << "Building Verilog" << endl; 
  } 

  // Here you would put "helper functions (reset/clock/etc)" 

  void ToggleClock() { 
     this->clock = 1; this->eval(); this->clock = 0; this->eval(); 
     if (debug) printf("Clock Toggled\n"); 
    
  } 
 
  void Nop()   { this->push = 0; this->pop = 0; this->add = 0; this->sub = 0; this->mul = 0; }
  void Push(unsigned int value)
               { this->push = 1; this->value = value; this->ToggleClock(); Nop(); }  
  void Pop()   { this->pop = 1; this->ToggleClock(); Nop(); }  
  void Add()   { this->add = 1; this->ToggleClock(); Nop(); } 
  void Sub()   { this->sub = 1; this->ToggleClock(); Nop(); } 
  void Mul()   { this->mul = 1; this->ToggleClock(); Nop(); } 
  void Reset() { this->reset = 1; ToggleClock(); ToggleClock(); this->reset = 0; ToggleClock(); } 

  void Run() { 
      Nop();
      Reset(); 
      Push(74); 
      Push(21); 
      Add();
      ToggleClock(); 
      Push(3);
      ToggleClock(); 
      ToggleClock(); 
      ToggleClock(); 
      Push(7);
      Add();
      ToggleClock(); 
      ToggleClock(); 
      Mul();
      ToggleClock(); 
      ToggleClock(); 
      assert(this->stack0 == (74+21)*(3+7));
 
   } 

};


int main(int argc, char **argv) 
{

   ZynqRPNCalculatorWrapperClass ZynqRPNCalculatorWrapper; 

   // Normally you'd now build a set of testcases based on the Calculator Wrapper. For simplicity we'll just do a Run() function. 
   ZynqRPNCalculatorWrapper.Run();  
  
   std::cout << "Test: ************************************** PASS!!!!  ************************************" <<std::endl;       
   return 0; 

}
