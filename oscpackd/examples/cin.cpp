#include <iostream>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  char v;
  while(1)
    {
      sleep(4);
      std::cin >> v;
      std::cout << "V is [" << v << "]\n";
    }
     
}
